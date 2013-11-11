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
* Writer for dictionary file
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;

import com.nokia.tracecompiler.decodeplugins.dictionary.encoder.DataType;
import com.nokia.tracecompiler.decodeplugins.dictionary.encoder.Dictionary;
import com.nokia.tracecompiler.decodeplugins.dictionary.encoder.LocationStore;
import com.nokia.tracecompiler.decodeplugins.dictionary.encoder.TraceComponent;
import com.nokia.tracecompiler.decodeplugins.dictionary.encoder.TraceDataStore;
import com.nokia.tracecompiler.decodeplugins.dictionary.encoder.TraceDictionaryEncoder;
import com.nokia.tracecompiler.decodeplugins.dictionary.encoder.TypeDef;
import com.nokia.tracecompiler.decodeplugins.dictionary.encoder.TypeDefStore;
import com.nokia.tracecompiler.engine.LocationListBase;
import com.nokia.tracecompiler.engine.LocationProperties;
import com.nokia.tracecompiler.file.FileUtils;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceConstantTable;
import com.nokia.tracecompiler.model.TraceConstantTableEntry;
import com.nokia.tracecompiler.model.TraceGroup;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.model.TraceObjectUtils;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.project.FormattingUtils;
import com.nokia.tracecompiler.project.PropertyNames;
import com.nokia.tracecompiler.project.TraceProjectAPI;
import com.nokia.tracecompiler.project.TraceProjectAPI.TraceFormatFlags;
import com.nokia.tracecompiler.rules.FillerParameterRule;
import com.nokia.tracecompiler.rules.HiddenTraceObjectRule;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.source.SourceUtils;

/**
 * Writer for dictionary file
 * 
 */
final class DictionaryFileWriter {

	/**
	 * Comparator for file references
	 */
	private final class FileRefComparator implements
			Comparator<DictionaryFileRef> {
		public int compare(DictionaryFileRef o1, DictionaryFileRef o2) {
			int val = o1.path.compareTo(o2.path);
			if (val == 0) {
				val = o1.file.compareTo(o2.file);
			}
			return val;
		}
	}

	/**
	 * Dictionary file
	 */
	private DictionaryFile dictionaryFile;

	/**
	 * Sequential number for trace definitions
	 */
	private int defref;

	/**
	 * Sequential number for file definitions
	 */
	private int fileref;

	/**
	 * Constructor
	 * 
	 * @param dictionaryFile
	 *            the dictionary file
	 */
	DictionaryFileWriter(DictionaryFile dictionaryFile) {
		this.dictionaryFile = dictionaryFile;
	}

	/**
	 * Writes the dictionary file
	 */
	void write() {
		defref = 0;
		fileref = 0;
		File file = new File(dictionaryFile.getAbsolutePathWithID());
		// Delete removes possible read-only flags
		if (file.exists()) {
			file.delete();
		}

		TraceDictionaryEncoder encoder = new TraceDictionaryEncoder();
		File path = file.getParentFile();
		if (!path.exists()) {
			FileUtils.createDirectories(path);
		}
		encoder.createNewDictionary(file.getAbsolutePath());
		Dictionary.startDictionary();
		createTypedefs();
		createDefs();
		createLocations();
		createComponent();
		Dictionary.endDictionary();
	}

	/**
	 * Creates the type definitions
	 */
	private void createTypedefs() {
		TypeDefStore.startTypeDefs();
		ArrayList<String> typeList = buildTypeList();
		writeTypeDefinitions(typeList);
		TypeDefStore.endTypeDefs();
	}

	/**
	 * Builds the list of parameter types
	 * 
	 * @return the list of types found from the model
	 */
	private ArrayList<String> buildTypeList() {
		TraceModel model = dictionaryFile.getOwner().getModel();
		ArrayList<String> typeList = new ArrayList<String>();
		for (TraceGroup group : model) {
			for (Trace trace : group) {
				for (TraceParameter parameter : trace) {
					if ((parameter.getExtension(HiddenTraceObjectRule.class) == null)
							|| (parameter
									.getExtension(FillerParameterRule.class) != null)) {
						String type = parameter.getType();
						if (!typeList.contains(type)) {
							typeList.add(type);
						}
					}
				}
			}
		}
		return typeList;
	}

	/**
	 * Writes the data type definitions
	 * 
	 * @param typeList
	 *            the list of data types
	 */
	private void writeTypeDefinitions(ArrayList<String> typeList) {
		for (String type : typeList) {
			DataType dt = mapTypeToDataType(type);
			if (dt != null) {
				// Normal parameters
				int size = mapParameterTypeToSize(type);
				String formatChar = SourceUtils.mapNormalTypeToFormat(type);
				if (formatChar != null && formatChar.length() > 1
						&& formatChar.charAt(0) == '%') {
					formatChar = formatChar.substring(1);
					TypeDefStore.writeTypeDef(type, size, formatChar, dt);

					// Write alternative format characters
					writeAlternativeFormatChars(formatChar, type, size, dt);

				} else {
					TypeDefStore.writeTypeDef(type, size, null, dt);
				}
			} else {
				// Enum parameters
				TraceModel model = dictionaryFile.getOwner().getModel();
				TraceConstantTable table = model.findConstantTableByName(type);
				if (table != null) {
					TypeDefStore.startTypeDef(table.getName(),
							mapParameterTypeToSize(table.getType()), null,
							DataType.ENUM);
					for (TraceConstantTableEntry entry : table) {
						TypeDef.writeTypeMember(entry.getID(), entry.getName(),
								null);
					}
					TypeDefStore.endTypeDef();
				}
			}
		}
	}

	/**
	 * Writes alternative format characters to the Dictionary
	 * 
	 * @param formatChar
	 *            formatchar
	 * @param type
	 *            parameter type
	 * @param size
	 *            parameter size
	 * @param dt
	 *            data type
	 */
	private void writeAlternativeFormatChars(String formatChar, String type,
			int size, DataType dt) {

		// If there's big L, write also 'll' type
		if (formatChar.indexOf('L') != -1) {

			// Double small l
			String newFormatChar = formatChar.replace("L", "ll"); //$NON-NLS-1$ //$NON-NLS-2$
			TypeDefStore.writeTypeDef(type, size, newFormatChar, dt);
			writeCapitalHexType(newFormatChar, type, size, dt);

		}

		// Write alternative option to float types
		else if (formatChar.equals("f") || formatChar.equals("e") //$NON-NLS-1$//$NON-NLS-2$
				|| formatChar.equals("g")) { //$NON-NLS-1$

			String newFormatChar = 'L' + formatChar;
			TypeDefStore.writeTypeDef(type, size, newFormatChar, dt);
		}

		else {
			// If length is one, add also formatchar with "l"
			if (formatChar.length() == 1) {
				String newFormatChar = "l" + formatChar; //$NON-NLS-1$
				TypeDefStore.writeTypeDef(type, size, newFormatChar, dt);
				writeCapitalHexType(newFormatChar, type, size, dt);
			}

			// Check capital hex need
			writeCapitalHexType(formatChar, type, size, dt);
		}
	}

	/**
	 * Writes capital hex type
	 * 
	 * @param formatChar
	 *            formatchar
	 * @param type
	 *            parameter type
	 * @param size
	 *            parameter size
	 * @param dt
	 *            data type
	 */
	private void writeCapitalHexType(String formatChar, String type, int size,
			DataType dt) {

		// Write also capital X if there are hex values
		if (formatChar.indexOf('x') != -1) {
			type = type.toUpperCase();
			TypeDefStore.writeTypeDef(type, size, formatChar.replace('x', 'X'),
					dt);
		}
	}

	/**
	 * Gets the parameter size from type
	 * 
	 * @param type
	 *            the type
	 * @return the size in bytes
	 */
	private int mapParameterTypeToSize(String type) {
		int size = SourceUtils.mapParameterTypeToSize(type);
		if (size == 0) {
			if (type.equals(TraceParameter.ASCII)) {
				size = 1;
			} else if (type.equals(TraceParameter.UNICODE)) {
				size = 2; // CodForChk_Dis_Magic
			}
		}
		return size;
	}

	/**
	 * Creates the trace definitions
	 */
	private void createDefs() {
		// This should check for duplicates
		TraceDataStore.startDataStore();
		TraceModel model = dictionaryFile.getOwner().getModel();
		TraceProjectAPI api = model.getExtension(TraceProjectAPI.class);
		for (TraceGroup group : model) {
			for (Trace trace : group) {
				trace.addExtension(new DictionaryDefRef(++defref));
				TraceFormatFlags flags = new TraceFormatFlags();
				flags.isFormattingSupported = true;
				String data = api.formatTraceForExport(trace, flags);
				data = replaceUnescapeQuotes(data);
				TraceDataStore.writeData(defref, DataType.STRING, data);
			}
		}
		TraceDataStore.endDataStore();
	}

	/**
	 * Replaces unescape quates
	 * 
	 * @param data
	 *            the data
	 * @return the new string
	 */
	private String replaceUnescapeQuotes(String data) {
		data = data.replace("\\\"", "\""); //$NON-NLS-1$ //$NON-NLS-2$
		return data;
	}

	/**
	 * Maps a basic type to a Dictionary data type
	 * 
	 * @param type
	 *            the type
	 * @return the data type
	 */
	private DataType mapTypeToDataType(String type) { // CodForChk_Dis_ComplexFunc
		DataType retval;
		// Unsigned is not supported in Dictionary
		if (type.equals(TraceParameter.SDEC32)) {
			retval = DataType.INTEGER;
		} else if (type.equals(TraceParameter.HEX32)) {
			retval = DataType.HEX;
		} else if (type.equals(TraceParameter.UDEC32)) {
			retval = DataType.INTEGER;
		} else if (type.equals(TraceParameter.OCT32)) {
			retval = DataType.OCTAL;
		} else if (type.equals(TraceParameter.SDEC16)) {
			retval = DataType.INTEGER;
		} else if (type.equals(TraceParameter.HEX16)) {
			retval = DataType.HEX;
		} else if (type.equals(TraceParameter.UDEC16)) {
			retval = DataType.INTEGER;
		} else if (type.equals(TraceParameter.OCT16)) {
			retval = DataType.OCTAL;
		} else if (type.equals(TraceParameter.SDEC8)) {
			retval = DataType.INTEGER;
		} else if (type.equals(TraceParameter.HEX8)) {
			retval = DataType.HEX;
		} else if (type.equals(TraceParameter.UDEC8)) {
			retval = DataType.INTEGER;
		} else if (type.equals(TraceParameter.OCT8)) {
			retval = DataType.OCTAL;
		} else if (type.equals(TraceParameter.SDEC64)) {
			retval = DataType.INTEGER;
		} else if (type.equals(TraceParameter.HEX64)) {
			retval = DataType.HEX;
		} else if (type.equals(TraceParameter.UDEC64)) {
			retval = DataType.INTEGER;
		} else if (type.equals(TraceParameter.OCT64)) {
			retval = DataType.OCTAL;
		} else if (type.equals(TraceParameter.ASCII)) {
			retval = DataType.STRING;
		} else if (type.equals(TraceParameter.UNICODE)) {
			retval = DataType.STRING;
		} else if (type.equals(TraceParameter.FLOAT_FIX)) {
			retval = DataType.FLOAT;
		} else if (type.equals(TraceParameter.FLOAT_EXP)) {
			retval = DataType.FLOAT;
		} else if (type.equals(TraceParameter.FLOAT_OPT)) {
			retval = DataType.FLOAT;
		} else if (type.equals(TraceParameter.POINTER)) {
			retval = DataType.HEX;
		} else {
			retval = null;
		}
		return retval;
	}

	/**
	 * Creates the location definitions
	 */
	private void createLocations() {
		ArrayList<DictionaryFileRef> files = new ArrayList<DictionaryFileRef>();
		LocationStore.startLocations();
		for (TraceGroup group : dictionaryFile.getOwner().getModel()) {
			for (Trace trace : group) {
				writeLocation(files, trace);
			}
		}
		// Build XML and assign ID's to refs
		Collections.sort(files, new FileRefComparator());
		String lastpath = null;
		for (DictionaryFileRef ref : files) {
			if (!ref.path.equals(lastpath)) {
				if (lastpath != null) {
					LocationStore.endPath();
				}
				LocationStore.startPath(ref.path);
				lastpath = ref.path;
			}
			LocationStore.writeFile(++fileref, ref.file);
			ref.refid = fileref;
		}
		if (lastpath != null) {
			LocationStore.endPath();
		}
		LocationStore.endLocations();
	}

	/**
	 * Writes the location of a trace
	 * 
	 * @param files
	 *            file references
	 * @param trace
	 *            trace to be written
	 */
	private void writeLocation(ArrayList<DictionaryFileRef> files, Trace trace) {
		LocationProperties loc = findFirstLocation(trace);
		if (loc != null) {
			String path = loc.getFilePath();
			String file = loc.getFileName();
			if (path != null) {
				path = FileUtils.convertSeparators(
						SourceConstants.FORWARD_SLASH_CHAR, path, true);
				// TODO: Remove drive letter. Actually cannot remove drive
				// letter because EPOCROOT might not be in the root of the drive
			}
			DictionaryFileRef ref = getRef(files, file, path);
			if (ref == null) {
				ref = new DictionaryFileRef(file, path, trace);
				files.add(ref);
				trace.addExtension(ref);
			} else {
				trace.addExtension(ref);
			}
		}
	}

	/**
	 * Finds the first location from trace
	 * 
	 * @param trace
	 *            the trace
	 * @return the location
	 */
	private LocationProperties findFirstLocation(Trace trace) {
		Iterator<LocationListBase> itr = trace
				.getExtensions(LocationListBase.class);
		LocationProperties loc = null;
		while (itr.hasNext() && loc == null) {
			Iterator<LocationProperties> locs = itr.next().iterator();
			if (locs.hasNext()) {
				loc = locs.next();
			}
		}
		return loc;
	}

	/**
	 * Gets a file reference
	 * 
	 * @param files
	 *            the list of file references
	 * @param file
	 *            file name
	 * @param path
	 *            file path
	 * @return the file reference
	 */
	private DictionaryFileRef getRef(ArrayList<DictionaryFileRef> files,
			String file, String path) {
		DictionaryFileRef retval = null;
		for (int i = 0; i < files.size() && retval == null; i++) {
			DictionaryFileRef ref = files.get(i);
			if (ref.file.equals(file) && ref.path.equals(path)) {
				retval = ref;
			}
		}
		return retval;
	}

	/**
	 * Creates the component definition
	 */
	private void createComponent() {
		TraceModel model = dictionaryFile.getOwner().getModel();
		int compid = model.getID();
		// Component prefix and suffix are in property file.
		// If not there, the default values are used
		String prefix = TraceObjectUtils.findProperty(model,
				PropertyNames.PREFIX);
		if (prefix == null || prefix.length() == 0) {
			prefix = FormattingUtils.getDefaultComponentPrefix(model);
		}
		String suffix = TraceObjectUtils.findProperty(model,
				PropertyNames.SUFFIX);
		if (suffix == null || suffix.length() == 0) {
			suffix = FormattingUtils.getDefaultComponentSuffix(model);
		}
		Dictionary.startComponent(compid, dictionaryFile.getProjectName(),
				prefix, suffix);
		for (TraceGroup group : model) {
			createGroup(group);
		}
		Dictionary.endComponent();
	}

	/**
	 * Creates a group definition
	 * 
	 * @param group
	 *            the group
	 */
	private void createGroup(TraceGroup group) {
		String prefix = TraceObjectUtils.findProperty(group,
				PropertyNames.PREFIX);
		if (prefix == null || prefix.length() == 0) {
			prefix = FormattingUtils.getDefaultGroupPrefix(group);
		}
		String suffix = TraceObjectUtils.findProperty(group,
				PropertyNames.SUFFIX);
		if (suffix == null || suffix.length() == 0) {
			suffix = FormattingUtils.getDefaultGroupSuffix(group);
		}
		TraceComponent.startGroup(group.getID(), group.getName(), prefix,
				suffix);
		for (Trace trace : group) {
			createTrace(trace);
		}
		TraceComponent.endGroup();
	}

	/**
	 * Creates a trace definition
	 * 
	 * @param trace
	 *            the trace
	 */
	private void createTrace(Trace trace) {
		DictionaryDefRef defref = trace.getExtension(DictionaryDefRef.class);
		if (defref != null) {
			DictionaryFileRef fileref = trace
					.getExtension(DictionaryFileRef.class);
			com.nokia.tracecompiler.decodeplugins.dictionary.encoder.TraceGroup.startTrace(defref
					.getRefId(), trace.getName());
			if (fileref != null) {
				writeTraceWithLocation(fileref.getRefId(), trace);
			} else {
				com.nokia.tracecompiler.decodeplugins.dictionary.encoder.Trace.writeInstance(trace
						.getID(), 0, 0, "", //$NON-NLS-1$
						""); //$NON-NLS-1$
			}
			com.nokia.tracecompiler.decodeplugins.dictionary.encoder.TraceGroup.endTrace();
		}
		trace.removeExtensions(DictionaryRef.class);

	}

	/**
	 * Writes a trace which has a source location
	 * 
	 * @param refId
	 *            file reference number
	 * @param trace
	 *            the trace
	 */
	private void writeTraceWithLocation(int refId, Trace trace) {
		LocationProperties loc = findFirstLocation(trace);
		int line = 0;
		String className = ""; //$NON-NLS-1$
		String functionName = ""; //$NON-NLS-1$
		if (loc != null) {
			line = loc.getLineNumber();
			className = loc.getClassName();
			functionName = loc.getFunctionName();
		}
		com.nokia.tracecompiler.decodeplugins.dictionary.encoder.Trace.writeInstance(trace.getID(),
				refId, line, functionName, className);
	}

}
