/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Implementation of TraceObjectRuleFactory interface
*
*/
package com.nokia.tracecompiler.engine.rules;

import java.util.Iterator;

import com.nokia.tracecompiler.engine.TraceCompilerEngineConfiguration;
import com.nokia.tracecompiler.engine.TraceCompilerEngineConfigurationListener;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.header.ComplexHeaderRule;
import com.nokia.tracecompiler.engine.rules.osttrace.OstTraceFormatRule;
import com.nokia.tracecompiler.engine.source.SourceParserRule;
import com.nokia.tracecompiler.engine.source.TraceParameterFormattingRule;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.model.TraceModelExtension;
import com.nokia.tracecompiler.model.TraceModelListener;
import com.nokia.tracecompiler.model.TraceModelPersistentExtension;
import com.nokia.tracecompiler.model.TraceModelResetListener;
import com.nokia.tracecompiler.model.TraceObject;
import com.nokia.tracecompiler.model.TraceObjectRuleFactory;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.project.TraceProjectAPI;
import com.nokia.tracecompiler.rules.FillerParameterRule;
import com.nokia.tracecompiler.source.SourceUtils;

/**
 * Provides rules for trace objects.
 * 
 */
public class RulesEngine implements TraceObjectRuleFactory {

	/**
	 * Number of parameters in a simple trace
	 */
	private static final int SIMPLE_TRACE_MAX_PARAMETER_COUNT = 1;

	/**
	 * Trace model listener updates the fillers and complex type flagging when
	 * traces and parameters are modified
	 */
	private TraceModelListener modelListener;

	/**
	 * Trace model reset listener uses modelValid to update the complex header
	 * rules
	 */
	private TraceModelResetListener resetListener;

	/**
	 * Manager for plug-in API's
	 */
	private RulesEnginePluginManager pluginManager;

	/**
	 * Configuration listener for source format changes
	 */
	private TraceCompilerEngineConfigurationListener configurationListener = new RulesEngineConfigurationListener(
			this);

	/**
	 * Trace model
	 */
	private TraceModel model;

	/**
	 * Constructor
	 */
	public RulesEngine() {
		TraceCompilerEngineGlobals.getConfiguration().addConfigurationListener(
				configurationListener);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectRuleFactory#
	 *      createExtension(com.nokia.tracecompiler.model.TraceObject,
	 *      java.lang.String)
	 */
	public TraceModelPersistentExtension createExtension(TraceObject target,
			String name) {
		TraceModelPersistentExtension retval = null;
		ClassNameWrapper[] table = RulesEngineConstants.PERSISTENT_EXTENSIONS;
		for (int i = 0; i < table.length && retval == null; i++) {
			if (name.equals(table[i].name)) {
				retval = createPersistentExtensionAt(target, i);
			}
		}
		return retval;
	}

	/**
	 * Creates a persistent extension
	 * 
	 * @param target
	 *            the target object
	 * @param i
	 *            index to the persistent extensions array
	 * @return the extension
	 */
	private TraceModelPersistentExtension createPersistentExtensionAt(
			TraceObject target, int i) {
		ClassNameWrapper wrapper = RulesEngineConstants.PERSISTENT_EXTENSIONS[i];
		TraceModelPersistentExtension retval = null;
		TraceModelPersistentExtension o = target.getExtension(wrapper.clasz);
		if (o == null) {
			try {
				retval = wrapper.clasz.newInstance();
			} catch (Exception e) {
				if (TraceCompilerEngineConfiguration.ASSERTIONS_ENABLED) {
					TraceCompilerEngineGlobals.getEvents().postAssertionFailed(
							"Invalid extension - " + wrapper.name, null); //$NON-NLS-1$
				}
			}
		}
		return retval;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectRuleFactory#
	 *      preProcessNewRules(com.nokia.tracecompiler.model.TraceObject)
	 */
	public void preProcessNewRules(TraceObject object) {
		if (object instanceof TraceModel) {
			// NOTE: This is only called once when builder is started
			// There is no cleanup code
			this.model = (TraceModel) object;
			modelListener = new RulesEngineModelListener(this);
			resetListener = new RulesEngineResetListener(this, model);
			model.addModelListener(modelListener);
			model.addResetListener(resetListener);
			// Adds the plug-in trace parser / formatter manager to the model as
			// extension. The plug-in manager delegates the formatters and
			// parsers to this object when plug-in components register to
			// TraceCompiler.
			pluginManager = new RulesEnginePluginManager(this);
			model.addExtension(pluginManager);
			createTraceParsers();
			createTraceAPIs();
		}
	}

	/**
	 * Creates the trace parsers and stores them to the model
	 */
	private void createTraceParsers() {
		for (SourceParserRule element : RulesEngineConstants.TRACE_PARSERS) {
			// Creates all source parsers specified in the constants
			model.addExtension(element);
		}
	}

	/**
	 * Creates the default trace API's and adds them to the plug-in manager
	 */
	private void createTraceAPIs() {
		for (TraceProjectAPI api : RulesEngineConstants.TRACE_APIS) {
			pluginManager.addAPI(api);
		}
	}

	/**
	 * Creates the trace formatter and stores it to the model
	 */
	void setDefaultTraceAPI() {
		// If the formatter did not exist in the project file, it is added based
		// on the configuration default
		if (model.getExtension(TraceProjectAPI.class) == null) {
			String api = TraceCompilerEngineGlobals.getConfiguration().getText(
					TraceCompilerEngineConfiguration.FORMATTER_NAME);
			traceAPIChanged(api);
		}
	}

	/**
	 * API change notification
	 * 
	 * @param apiName
	 *            the name of new api
	 */
	void traceAPIChanged(String apiName) {
		TraceProjectAPI api = model.getExtension(TraceProjectAPI.class);
		boolean found = false;
		if (api != null) {
			if (api.getName().equals(apiName)) {
				found = true;
			} else {
				model.removeExtension(api);
			}
		}
		if (!found) {
			changeTraceAPI(apiName);
		}
	}

	/**
	 * Creates a trace API
	 * 
	 * @param apiName
	 *            the name of the API to be created
	 */
	void changeTraceAPI(String apiName) {
		if (apiName == null || apiName.length() == 0) {
			apiName = OstTraceFormatRule.STORAGE_NAME;
		}
		Iterator<TraceProjectAPI> apis = pluginManager.getAPIs();
		boolean apifound = false;
		while (apis.hasNext()) {
			TraceProjectAPI api = apis.next();
			if (api.getName().equals(apiName)) {
				model.addExtension(api);
				apifound = true;
			}
		}
		if (!apifound) {
			// If API from configuration was not found, the first one is used
			apis = pluginManager.getAPIs();
			if (apis.hasNext()) {
				model.addExtension(apis.next());
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectRuleFactory#
	 *      postProcessNewRules(com.nokia.tracecompiler.model.TraceObject)
	 */
	public void postProcessNewRules(TraceObject object) {
	}

	/**
	 * Checks the count and types of parameters of given trace and flags it with
	 * ComplexParameterRule if necessary
	 * 
	 * @param trace
	 *            the trace
	 */
	void checkParameterTypes(Trace trace) {
		// When converting traces from source, the converter takes care of
		// flagging the traces as complex. The complex flag needs to be checked
		// when trace are modified via UI
		if (!TraceCompilerEngineGlobals.getSourceContextManager().isConverting()) {
			boolean complex = false;
			int count = trace.getParameterCount();
			Iterator<TraceParameter> itr = trace.getParameters();
			while (itr.hasNext() && !complex) {
				TraceParameter parameter = itr.next();
				TraceParameterFormattingRule rule = parameter
						.getExtension(TraceParameterFormattingRule.class);
				boolean isShown = true;
				if (rule != null && !rule.isShownInSource()) {
					isShown = false;
				}
				if (isShown) {
					complex = !SourceUtils.isSimpleType(parameter);
				} else {
					count--;
				}
			}
			// Any trace with more than one parameter is a complex trace
			if (!complex && count > SIMPLE_TRACE_MAX_PARAMETER_COUNT) {
				complex = true;
			}
			ComplexHeaderRule rule = trace
					.getExtension(ComplexHeaderRule.class);
			if (complex && rule == null) {
				trace.addExtension(new ComplexHeaderRuleImpl());
			} else if (!complex && rule != null) {
				trace.removeExtension(rule);
			}
		}
	}

	/**
	 * Adds fillers to align trace parameters to 32-bit boundaries.
	 * 
	 * @param trace
	 *            the trace to be updated
	 * @throws TraceCompilerException 
	 */
	void checkFillerParameters(Trace trace) throws TraceCompilerException {
		// Flags the model so listeners don't perform intermediate updates
		trace.getModel().startProcessing();
		try {
			// Removes all existing fillers
			for (int i = 0; i < trace.getParameterCount(); i++) {
				TraceParameter parameter = trace.getParameter(i);
				if (parameter.getExtension(FillerParameterRule.class) != null) {
					trace.removeParameterAt(i);
					i--;
				}
			}
			int bytesInBlock = 0;
			int parameterIndex = 0;
			for (; parameterIndex < trace.getParameterCount(); parameterIndex++) {
				TraceParameter parameter = trace.getParameter(parameterIndex);
				int paramSize = SourceUtils.mapParameterTypeToSize(parameter);
				// Parameters are aligned to 32 bits. Parameter after
				// end-of-string is aligned dynamically and thus no filler is
				// created for it
				if (paramSize == 0 || paramSize == 4 || paramSize == 8) { // CodForChk_Dis_Magic
					if (bytesInBlock > 0) {
						int fillerCount = 4 - bytesInBlock; // CodForChk_Dis_Magic
						for (int i = 0; i < fillerCount; i++) {
							createFillerParameter(trace, parameterIndex++);
						}
						bytesInBlock = 0;
					}
				} else if (paramSize == 2) { // CodForChk_Dis_Magic
					if (bytesInBlock == 1 || bytesInBlock == 3) { // CodForChk_Dis_Magic
						createFillerParameter(trace, parameterIndex++);
						// If there was 1 existing byte and filler was added,
						// the number of bytes in the block is now 4 including
						// the 2-byte parameter. If there was 3 bytes, the
						// filler brings it to 4 and the 16-bit parameter
						// changes it to 2
						bytesInBlock += 3; // CodForChk_Dis_Magic
					} else {
						bytesInBlock += 2; // CodForChk_Dis_Magic
					}
					if (bytesInBlock >= 4) { // CodForChk_Dis_Magic
						bytesInBlock -= 4; // CodForChk_Dis_Magic
					}
				} else {
					bytesInBlock++;
					if (bytesInBlock == 4) { // CodForChk_Dis_Magic
						bytesInBlock = 0;
					}
				}
			}
			// Adds fillers also the the end of the parameter list
			if (bytesInBlock > 0) {
				int fillerCount = 4 - bytesInBlock; // CodForChk_Dis_Magic
				for (int i = 0; i < fillerCount; i++) {
					createFillerParameter(trace, parameterIndex++);
				}
				bytesInBlock = 0;
			}
		} finally {
			trace.getModel().processingComplete();
		}
	}

	/**
	 * Creates a filler parameter
	 * 
	 * @param trace
	 *            the trace for the parameter
	 * @param parameterIndex
	 *            the index where the filler is inserted
	 * @throws TraceCompilerException 
	 */
	private void createFillerParameter(Trace trace, int parameterIndex) throws TraceCompilerException {
		trace.getModel().getFactory().createTraceParameter(parameterIndex,
				trace, trace.getParameterCount(),
				"Filler", //$NON-NLS-1$
				TraceParameter.HEX8,
				new TraceModelExtension[] { new FillerParameterRuleImpl() });
	}

}
