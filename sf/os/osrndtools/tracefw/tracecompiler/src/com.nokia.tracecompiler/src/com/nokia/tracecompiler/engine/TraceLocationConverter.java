/*
 * Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 *
 * Location converter monitors locations and converts them to traces if necessary.
 *
 */
package com.nokia.tracecompiler.engine;

import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;

import com.nokia.tracecompiler.TraceCompilerConstants;
import com.nokia.tracecompiler.TraceCompilerLogger;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.engine.project.ProjectConstants;
import com.nokia.tracecompiler.engine.project.ProjectEngine;
import com.nokia.tracecompiler.engine.project.SortedProperties;
import com.nokia.tracecompiler.engine.project.TraceIDCache;
import com.nokia.tracecompiler.engine.rules.PerformanceEventRuleBase;
import com.nokia.tracecompiler.engine.rules.StateTraceRule;
import com.nokia.tracecompiler.engine.source.SourceParserRule;
import com.nokia.tracecompiler.engine.source.SourceProperties;
import com.nokia.tracecompiler.engine.source.SourceParserRule.ParameterConversionResult;
import com.nokia.tracecompiler.engine.source.SourceParserRule.TraceConversionResult;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceGroup;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.model.TraceModelExtension;
import com.nokia.tracecompiler.model.TraceModelPersistentExtension;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.project.FormattingUtils;
import com.nokia.tracecompiler.project.GroupNames;
import com.nokia.tracecompiler.project.ProjectUtils;
import com.nokia.tracecompiler.source.SourceConstants;

/**
 * Location converter monitors locations and converts them to traces if
 * necessary.
 * 
 */
public final class TraceLocationConverter {

	/**
	 * Number of deprecated groups
	 */
	final static int NUMBER_OF_DEPRECATED_GROUPS = 3;

	/**
	 * Deprecated group name map
	 */
	Map<String, String> deprecatedGroupNameMap = new HashMap<String, String>(
			NUMBER_OF_DEPRECATED_GROUPS);

	/**
	 * Trace model
	 */
	private TraceModel model;

	/**
	 * Constructor
	 * 
	 * @param model
	 *            the trace model
	 */
	TraceLocationConverter(TraceModel model) {
		this.model = model;
		initializeDeprecatedGroupNameMap();
	}

	/**
	 * Initialize deprecated group name map
	 */
	private void initializeDeprecatedGroupNameMap() {
		deprecatedGroupNameMap.put(GroupNames.TRACE_API,
				GroupNames.TRACE_BORDER);
		deprecatedGroupNameMap.put(GroupNames.TRACE_DEBUG,
				GroupNames.TRACE_DUMP);
		deprecatedGroupNameMap.put(GroupNames.TRACE_DETAILED,
				GroupNames.TRACE_INTERNALS);
	}

	/**
	 * Source opened notification
	 * 
	 * @param properties
	 *            the source properties
	 * @throws TraceCompilerException 
	 */
	void sourceOpened(SourceProperties properties) throws TraceCompilerException {
		//make sure all errors are reported AFTER processing a file
		TraceCompilerEngineGlobals.getSourceContextManager()
				.setConverting(true);
		model.startProcessing();
		try {
			// Load fixed group and trace ids from definition file to model
			//no errors are reported here 
			if (model.getFixedIds() == null) {
				loadFixedIdsFromDefinitionFileToModel();
			}

			for (TraceLocation loc : properties) {
				autoConvertLocation(loc);
			}
			// If there are duplicates or unused traces, they are removed
			// Note that this will work across source files although this
			// function is processing only one file.
			// If a trace is created, all locations from all open source files
			// are linked to that trace and thus it will be removed as
			// duplicate.
			removeDuplicateTraces();

		} finally {
			model.processingComplete();
			SourceContextManager manager = TraceCompilerEngineGlobals
					.getSourceContextManager();
			manager.setConverting(false);
			manager.setContext(null);
		}
	}

	/**
	 * Load fixed Ids from definiton file to properties
	 * 
	 */
	private void loadFixedIdsFromDefinitionFileToModel() {
		TraceIDCache cache = model.getExtension(TraceIDCache.class);

		// Create trace Id cache if it does not exist
		try {
			if (cache == null) {
				String path;
				path = ProjectUtils.getLocationForFile(model,
						ProjectEngine.traceFolderName,
						ProjectConstants.FIXED_ID_DEFINITIONS_FILE_NAME, false);
				if (path != null) {
					cache = new TraceIDCache(new File(path).getParent());
					model.addExtension(cache);
				}
			}
			if (cache != null) {
				File cacheFile = new File(cache.getAbsolutePath());
				// Try to load Ids from trace Id cache file
				try {
					SortedProperties fixedIds = new SortedProperties();
					if (cacheFile.exists()) {
						String thisLine;
						boolean allOldFixedIdsAreValid = true;
						String group = model.GROUP_PROPERTY_PREFIX;
						FileInputStream fis = new FileInputStream(cacheFile);
						BufferedReader myInput = new BufferedReader(
								new InputStreamReader(fis));
						// Read the old fixed id definition file
						while ((thisLine = myInput.readLine()) != null) {
							if (thisLine.indexOf(group) != -1) {
								thisLine = thisLine.substring(group.length(),
										thisLine.length());
								if (!searchForOldGroupIds(thisLine)) {						
									String msg  = Messages.getString("TraceLocationConverter.GroupIdValueError"); //$NON-NLS-1$
									TraceCompilerEngineGlobals.getEvents().postWarningMessage(msg, null);
									// We need only one old invalid id to make all of them invalid
									allOldFixedIdsAreValid = false;
									break;
								}
							}
						}
						fis.close();
						
						if (allOldFixedIdsAreValid) {
							// Create file input stream again
							FileInputStream newFis = new FileInputStream(cacheFile);
							fixedIds.load(newFis);
							// Load fixed Ids from properties to model
							model.setFixedIds(fixedIds);
							newFis.close();
						}
						
					}
				} catch (IOException e) {
					// If there is a problem with the cache file, just delete it
					// make sure the model fixed ids is null
					model.setFixedIds(null);
					cacheFile.delete();
					// We do not raise an exception but we need to report a info
					TraceCompilerLogger.printWarning(Messages.getString("TraceLocationConverter.FixedIdProblemWarningBeginText") + cacheFile.toString() + Messages.getString("TraceLocationConverter.FixedIdProblemWarningMiddleText") + e.getMessage()); //$NON-NLS-1$ //$NON-NLS-2$
				}
			}
		} catch (TraceCompilerException e) {
			// This exception can probably be ignored
			TraceCompilerLogger.printWarning("Could not create ID cache : " + e.getMessage()); //$NON-NLS-1$
		}
	}

	/**
	 * Searches the fixed_id file to see if there are any old definitions
	 * 
	 * @param string
	 *            of text from the fixed_id files
	 * @return boolean 
	 */
	public static boolean searchForOldGroupIds(String str) {
		
		boolean oldGroupIdValid = true;

		//get group name from the string
		int index = str.indexOf(SourceConstants.ASSIGN_WITH_OUT_SPACES);
		if (index == -1) {
			//not valid string, so ignore it, just return success
			return true;
		}

		String groupName = str.substring(0, index);

		//get group id
		str = str.substring(str.indexOf(SourceConstants.HEX_PREFIX) + 2, str
				.length());

		int groupId = -1;

		try {
			groupId = Integer.parseInt(str.trim(),
					TraceCompilerConstants.HEX_RADIX);
		} catch (NumberFormatException e) {
			//it's not an hex number so fail
			oldGroupIdValid = false;
		}

		//get group id from ost header file
		int fileGroupId = GroupNames.getIdByName(groupName);

		if (fileGroupId != 0) {
			//found 1
			if (fileGroupId != groupId) //group id has changed, so old one can't be used
				oldGroupIdValid = false;
		} else {
			if ((GroupNames.USER_GROUP_ID_FIRST > groupId)
					|| (groupId > GroupNames.USER_GROUP_ID_LAST)) //not within new user defined Ids
				oldGroupIdValid = false;
		}

		return oldGroupIdValid;
	}

	/**
	 * Converts the given location to trace if parser supports auto-conversion
	 * 
	 * @param location
	 *            the location
	 */
	private void autoConvertLocation(TraceLocation location) {
		// Stores the context of the location to the context manager.
		TraceCompilerEngineGlobals.getSourceContextManager().setContext(
				location.getParser().getContext(location.getOffset()));
		Trace trace = location.getTrace();
		if (trace == null) {
			// If the trace does not exist, the parser determines if the
			// location can be converted
			if (location.getParserRule().getLocationParser()
					.isLocationConverted(location)) {
				try {
					convertLocation(location, null, true);
				} catch (TraceCompilerException e) {
					// If converter fails, the error code is stored into the
					// location. The location notifies all validity listeners
					// about the change
					location.setConverterErrorCode((TraceCompilerErrorCode) e
							.getErrorCode(), e.getErrorParameters());
				}
			}
		} else {
			// If the trace already exists in the model, it is updated
			// based on the source file contents
			updateLocation(location);
		}
	}

	/**
	 * Recreates the trace from changed location when source is saved
	 * 
	 * @param location
	 *            the location to be checked
	 */
	private void updateLocation(TraceLocation location) {
		// Parser determines if the location can be converted
		if (location.getParserRule().getLocationParser().isLocationConverted(
				location)) {
			try {
				Trace trace = location.getTrace();
				// If a location has changed, the old trace is removed
				// and a new one created. Persistent extensions are moved to the
				// new trace
				Iterator<TraceModelPersistentExtension> extensions = null;
				if (trace != null) {
					extensions = trace
							.getExtensions(TraceModelPersistentExtension.class);
					trace.getGroup().removeTrace(trace);
				}
				convertLocation(location, extensions, true);

				// Check that the location is inside a function. Otherwise throw
				// an error because the code is unreachable
				if (location.getFunctionName() == null) {
					throw new TraceCompilerException(
							TraceCompilerErrorCode.UNREACHABLE_TRACE_LOCATION);
				}

			} catch (TraceCompilerException e) {
				// If converter fails, the error code is stored into the
				// location. The location notifies all validity listeners about
				// the change
				location.setConverterErrorCode((TraceCompilerErrorCode) e
						.getErrorCode(), e.getErrorParameters());
			}
		}
	}

	/**
	 * Removes all duplicate traces from the model
	 * @throws TraceCompilerException 
	 */
	private void removeDuplicateTraces() throws TraceCompilerException {
		boolean groupRemoved = true;
		while (groupRemoved) {
			groupRemoved = false;
			for (TraceGroup group : model) {
				removeDuplicateTracesFromGroup(group);
				if (!group.hasTraces()) {
					model.removeGroup(group);
					groupRemoved = true;
					break;
				}
			}
		}
	}

	/**
	 * Removes duplicate traces from a trace group
	 * 
	 * @param group
	 *            the group
	 * @throws TraceCompilerException 
	 */
	private void removeDuplicateTracesFromGroup(TraceGroup group) throws TraceCompilerException {
		boolean traceRemoved = true;
		while (traceRemoved) {
			traceRemoved = false;
			for (Trace trace : group) {
				TraceLocationList list = trace
						.getExtension(TraceLocationList.class);
				if (list != null) {
					if (list.getLocationCount() > 1) {
						// All the locations are marked as duplicates and the
						// trace is deleted
						TraceCompilerErrorCode code = TraceCompilerErrorCode.TRACE_HAS_MULTIPLE_LOCATIONS;
						for (LocationProperties loc : list) {
							((TraceLocation) loc).setConverterErrorCode(code,
									null);
						}
						group.removeTrace(trace);
						traceRemoved = true;
						break;
					}
				}
			}
		}
	}

	/**
	 * Converts a location to a Trace object.
	 * 
	 * @param location
	 *            the location to be converted
	 * @param extensions
	 *            persistent extensions to be added to the new trace
	 * @param autoConvert
	 *            true if converting without user interaction
	 * @return the new trace
	 * @throws TraceCompilerException
	 *             if conversion fails
	 */
	private Trace convertLocation(TraceLocation location,
			Iterator<TraceModelPersistentExtension> extensions,
			boolean autoConvert) throws TraceCompilerException {
		Trace trace = null;
		// If the parser has failed, the validity code is not OK and the
		// location cannot be converted. Traces marked with no-trace error code
		// have not yet been converted, so that is OK. Traces that have
		// duplicate ID's error code can be parsed, since the duplicates might
		// no longer exist.
		if (!autoConvert
				|| location.getValidityCode() == TraceCompilerErrorCode.OK
				|| location.getValidityCode() == TraceCompilerErrorCode.TRACE_DOES_NOT_EXIST
				|| location.getValidityCode() == TraceCompilerErrorCode.TRACE_HAS_MULTIPLE_LOCATIONS) {
			// The parser does the actual conversion
			SourceParserRule rule = location.getParserRule();
			TraceConversionResult result = rule.getLocationParser()
					.convertLocation(location);
			// After parser has finished, the trace is created.
			trace = convertWithoutUI(result, extensions);

			if (trace != null) {
				model.startProcessing();
				try {
					createParametersFromConversionResult(location, result,
							trace);
					// Runs a location validity check and notifies listeners
					// that location is now OK
					location.setConverterErrorCode(TraceCompilerErrorCode.OK,
							null);
				} catch (TraceCompilerException e) {
					// If parameters cannot be created, the trace is removed
					TraceGroup group = trace.getGroup();
					trace.getGroup().removeTrace(trace);
					if (!group.hasTraces()) {
						group.getModel().removeGroup(group);
					}
					throw e;
				} finally {
					model.processingComplete();
				}
			}
		}
		return trace;
	}

	/**
	 * Converts a location to trace without UI
	 * 
	 * @param result
	 *            the conversion result from parser
	 * @param extensions
	 *            persistent extensions to be added to the new trace
	 * @return the converted trace
	 * @throws TraceCompilerException
	 *             if location properties are not valid
	 */
	private Trace convertWithoutUI(TraceConversionResult result,
			Iterator<TraceModelPersistentExtension> extensions)
			throws TraceCompilerException {
		Trace trace = null;
		if (result.group != null) {
			String groupName = result.group;
			TraceGroup group = handleGroup(groupName);
			trace = handleTrace(result, extensions, group);
		} else {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.GROUP_NOT_SELECTED);
		}
		return trace;
	}

	/**
	 * Handle trace
	 * 
	 * @param result
	 *            the conversion result from parser
	 * @param extensions
	 *            persistent extensions to be added to the new trace
	 * @param group
	 *            the group where trace belongs to
	 * @return the trace
	 * @throws TraceCompilerException
	 */
	private Trace handleTrace(TraceConversionResult result,
			Iterator<TraceModelPersistentExtension> extensions, TraceGroup group)
			throws TraceCompilerException {
		// First try to find Id to trace from fixed Ids
		Trace trace = null;
		Properties fixedIds = model.getFixedIds();
		String groupName = result.group;
		String traceName = result.name;
		int groupId = group.getID();
		String groupIdString = SourceConstants.HEX_PREFIX
				+ Integer.toString(groupId, model.HEX_RADIX).toUpperCase();
		int traceId = 0;
		if (fixedIds != null) {
			String tracePropertyName = groupName + model.GROUP_ID_PREFIX
					+ groupIdString + model.GROUP_ID_SUFFIX
					+ SourceConstants.UNDERSCORE + traceName;
			String value = fixedIds.getProperty(model.TRACE_PROPERTY_PREFIX
					+ tracePropertyName);
			if (value != null) {
				try {
					traceId = Integer.decode(value).intValue();
				} catch (NumberFormatException e) {
					// Corrupted, assign a proper Id later on
					traceId = 0;
				}
			}
		}
		// If there was no fixed Id to this trace, get Id from model
		if (traceId == 0) {
			traceId = model.getNextTraceId(group);
		}

		String text = result.text;
		model.getVerifier().checkTraceProperties(group, null, traceId,
				traceName, text);
		TraceModelExtension[] extArray = createExtensionArray(result,
				extensions);
		trace = model.getFactory().createTrace(group, traceId, traceName, text,
				extArray);

		if ((trace.getExtension(StateTraceRule.class) == null)
				&& (group.getName()
						.equals("TRACE_STATE"))) { //$NON-NLS-1$
			throw new TraceCompilerException(
					TraceCompilerErrorCode.INVALID_USAGE_OF_TRACE_STATE_GROUP_NAME);
		} else if ((trace.getExtension(PerformanceEventRuleBase.class) == null)
				&& (group.getName()
						.equals("TRACE_PERFORMANCE"))) { //$NON-NLS-1$
			throw new TraceCompilerException(
					TraceCompilerErrorCode.INVALID_USAGE_OF_TRACE_PERFORMACE_GROUP_NAME);
		}

		return trace;
	}

	/**
	 * Handle group. Try to fnd group from model. If it does not exist then
	 * create new group.
	 * 
	 * @param groupName
	 *            the name of the group
	 * @return the handled group
	 * @throws TraceCompilerException
	 */
	private TraceGroup handleGroup(String groupName)
			throws TraceCompilerException {
		String deprecatedGroupName = null;
		
		// Convert deprecated group name to valid group name if needed
		if (deprecatedGroupNameMap.containsKey(groupName)) {
			deprecatedGroupName = groupName;
			groupName = deprecatedGroupNameMap.get(groupName);
		}

		// If auto-convert flag is set, the location is converted without
		// user interaction. A new trace group is created if not found
		TraceGroup group = model.findGroupByName(groupName);
		if (group == null) {
			
			// Print warning related to deprecated group name change if needed
			if (deprecatedGroupName != null) {
				String message = Messages
						.getString("TraceLocationConverter.DeprecatedGroupIdWarningStart");//$NON-NLS-1$
				message += SourceConstants.SPACE + deprecatedGroupName
						+ SourceConstants.SPACE;
				message += Messages
						.getString("TraceLocationConverter.DeprecatedGroupIdWarningMiddle");//$NON-NLS-1$
				message += SourceConstants.SPACE + groupName
						+ SourceConstants.SPACE;
				message += Messages
						.getString("TraceLocationConverter.DeprecatedGroupIdWarningEnd");//$NON-NLS-1$
				TraceCompilerLogger.printWarning(message);
			}

			int groupId = 0;
			Properties fixedIds = model.getFixedIds();

			// First try to find Id to group from fixed Ids
			if (fixedIds != null) {
				String value = fixedIds.getProperty(model.GROUP_PROPERTY_PREFIX
						+ groupName);
				if (value != null) {
					try {
						groupId = Integer.decode(value).intValue();
					} catch (NumberFormatException e) {
						// Corrupted, assign a proper Id later on
						groupId = 0;
					}
				}
			}
			// If there was no fixed Id to this group, get Id from model
			if (groupId == 0) {
				groupId = FormattingUtils.getGroupID(model, groupName);
			}
			model.getVerifier().checkTraceGroupProperties(model, null, groupId,
					groupName);
			group = model.getFactory().createTraceGroup(groupId, groupName,
					null);
		}

		return group;
	}

	/**
	 * Combines extensions into one array
	 * 
	 * @param result
	 *            the conversion result
	 * @param extensions
	 *            the persistent extensions from old trace
	 * @return the combined array of extensions
	 */
	private TraceModelExtension[] createExtensionArray(
			TraceConversionResult result,
			Iterator<TraceModelPersistentExtension> extensions) {
		TraceModelExtension[] extArray = null;
		ArrayList<TraceModelExtension> ext = null;
		if (result.extensions != null) {
			ext = new ArrayList<TraceModelExtension>();
			ext.addAll(result.extensions);
		}
		if (extensions != null) {
			if (ext == null) {
				ext = new ArrayList<TraceModelExtension>();
			}
			while (extensions.hasNext()) {
				ext.add(extensions.next());
			}
		}
		if (ext != null) {
			extArray = new TraceModelExtension[ext.size()];
			ext.toArray(extArray);
		}
		return extArray;
	}

	/**
	 * Creates the trace parameters based on trace conversion result
	 * 
	 * @param converted
	 *            the location that was converted
	 * @param result
	 *            the conversion result
	 * @param trace
	 *            the trace
	 * @throws TraceCompilerException
	 *             if parameters cannot be created
	 */
	private void createParametersFromConversionResult(TraceLocation converted,
			TraceConversionResult result, Trace trace)
			throws TraceCompilerException {
		if (result.parameters != null) {
			for (int i = 0; i < result.parameters.size(); i++) {
				int id = trace.getNextParameterID();
				ParameterConversionResult res = result.parameters.get(i);
				boolean warning = false;
				if (res.type == null) {
					warning = true;
					res.type = TraceParameter.HEX32;
				}
				model.getVerifier().checkTraceParameterProperties(trace, null,
						id, res.name, res.type);
				TraceModelExtension[] extArray = null;
				if (res.extensions != null) {
					extArray = new TraceModelExtension[res.extensions.size()];
					res.extensions.toArray(extArray);
				}
				TraceParameter param = model.getFactory().createTraceParameter(
						trace, id, res.name, res.type, extArray);
				if (warning) {
					String msg = Messages
							.getString("TraceCompiler.UnknownTypeWarning"); //$NON-NLS-1$
					TraceCompilerEngineGlobals.getEvents().postWarningMessage(
							msg, param);
				}
			}
		}
	}

}
