/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Formatting rule for OST traces
 *
 */
package com.nokia.tracecompiler.engine.rules.osttrace;

import com.nokia.tracecompiler.engine.header.ComplexHeaderRule;
import com.nokia.tracecompiler.engine.rules.AutomaticTraceTextRule;
import com.nokia.tracecompiler.engine.rules.EntryTraceRule;
import com.nokia.tracecompiler.engine.rules.ExitTraceRule;
import com.nokia.tracecompiler.engine.rules.PerformanceEventRuleBase;
import com.nokia.tracecompiler.engine.rules.PerformanceEventStartRule;
import com.nokia.tracecompiler.engine.rules.PerformanceEventStopRule;
import com.nokia.tracecompiler.engine.rules.StateTraceRule;
import com.nokia.tracecompiler.engine.rules.TraceFormatRuleBase;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceModelListener;
import com.nokia.tracecompiler.model.TraceObject;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.plugin.TraceFormatConstants;
import com.nokia.tracecompiler.plugin.TraceHeaderContribution;
import com.nokia.tracecompiler.plugin.TraceAPIFormatter.TraceFormatType;
import com.nokia.tracecompiler.source.SourceUtils;

/**
 * Formatting rule for OST traces.
 * 
 */
public final class OstTraceFormatRule extends TraceFormatRuleBase implements
		TraceHeaderContribution, TraceModelListener {

	/**
	 * Separator for parameters within trace text
	 */
	private static final String PARAMETER_VALUE_SEPARATOR = "="; //$NON-NLS-1$

	/**
	 * Separator for parameter name value
	 */
	private static final String PARAMETER_SEPARATOR = ";"; //$NON-NLS-1$

	/**
	 * String parameter tag
	 */
	private static final String STRING_PARAMETER_TAG = "%s"; //$NON-NLS-1$

	/**
	 * Hex parameter tag
	 */
	private static final String HEX_PARAMETER_TAG = "0x%x"; //$NON-NLS-1$

	/**
	 * SDEC parameter tag
	 */
	private static final String SDEC_PARAMETER_TAG = "%d"; //$NON-NLS-1$

	/**
	 * UDEC parameter tag
	 */
	private static final String UDEC_PARAMETER_TAG = "%u"; //$NON-NLS-1$	

	/**
	 * Category for traces
	 */
	private static final String TRACE_CATEGORY = "EXTRACT_GROUP_ID(aTraceID)"; //$NON-NLS-1$

	/**
	 * Name for storage
	 */
	public static final String STORAGE_NAME = "OstTraceFormat"; //$NON-NLS-1$

	/**
	 * OstTraceEventStart tag
	 */
	public static final String OST_TRACE_EVENT_START_TAG = "OstTraceEventStart"; //$NON-NLS-1$

	/**
	 * OstTraceEventStop tag
	 */
	public static final String OST_TRACE_EVENT_STOP_TAG = "OstTraceEventStop"; //$NON-NLS-1$

	/**
	 * Ext-function declaration format
	 */
	private static final String HEADER_FORMAT = "OstTraceGen" //$NON-NLS-1$
			+ TraceFormatConstants.PARAM_COUNT_FORMAT // Number of parameters
			+ "( TUint32 aTraceID" //$NON-NLS-1$
			+ TraceFormatConstants.PARAMETERS_FORMAT // Trace parameters
			+ " )"; //$NON-NLS-1$

	/**
	 * Activation query formatting
	 */
	private static final String ACTIVATION_FORMAT = "BTraceFiltered8( " //$NON-NLS-1$
			+ TRACE_CATEGORY + ", " //$NON-NLS-1$
			+ "EOstTraceActivationQuery, KOstTraceComponentID, aTraceID )"; //$NON-NLS-1$

	/**
	 * Buffered trace format
	 */
	private static final String TRACE_BUFFER_FORMAT = "OstSendNBytes( " //$NON-NLS-1$
			+ TRACE_CATEGORY + ", " //$NON-NLS-1$
			+ "EOstTrace, KOstTraceComponentID, aTraceID, " //$NON-NLS-1$
			+ TraceFormatConstants.DATA_BUFFER_FORMAT // Trace data
			+ ", " //$NON-NLS-1$
			+ TraceFormatConstants.DATA_LENGTH_FORMAT // Trace data length
			+ " );"; //$NON-NLS-1$

	/**
	 * Packed trace format
	 */
	private static final String TRACE_PACKED_FORMAT = "BTraceFilteredContext12( " //$NON-NLS-1$
			+ TRACE_CATEGORY + ", " //$NON-NLS-1$
			+ "EOstTrace, KOstTraceComponentID, aTraceID, " //$NON-NLS-1$
			+ TraceFormatConstants.DATA_BUFFER_FORMAT // Trace data
			+ " );"; //$NON-NLS-1$

	/**
	 * Template for the OstTraceDefinitions.h header file
	 */
	private static final String[] MAIN_HEADER_TEMPLATE = { "\r\n" //$NON-NLS-1$
			+ "// OST_TRACE_COMPILER_IN_USE flag has been added by Trace Compiler\r\n" //$NON-NLS-1$
			+ "// REMOVE BEFORE CHECK-IN TO VERSION CONTROL\r\n" //$NON-NLS-1$
			+ "#define OST_TRACE_COMPILER_IN_USE\r\n" //$NON-NLS-1$
			+ "#include <opensystemtrace.h>\r\n#endif\r\n" }; //$NON-NLS-1$

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceProjectAPI#getName()
	 */
	public String getName() {
		return STORAGE_NAME;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.source.TraceFormattingRule#
	 * getFormat(com.nokia.tracecompiler.model.Trace,
	 * com.nokia.tracecompiler.plugin.TraceAPIFormatter.TraceFormatType)
	 */
	public String getFormat(Trace trace, TraceFormatType formatType) { // CodForChk_Dis_ComplexFunc
		String retval;
		if (formatType == TraceFormatType.HEADER) {
			retval = HEADER_FORMAT;
		} else if (formatType == TraceFormatType.TRACE_BUFFER) {
			retval = TRACE_BUFFER_FORMAT;
		} else if (formatType == TraceFormatType.TRACE_PACKED) {
			retval = TRACE_PACKED_FORMAT;
		} else if (formatType == TraceFormatType.TRACE_ACTIVATION) {
			retval = ACTIVATION_FORMAT;
		} else {
			retval = null;
		}

		return retval;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.source.SourceFormatRule#
	 * mapParameterCountToSource(com.nokia.tracecompiler.model.Trace, int)
	 */
	@Override
	public String mapParameterCountToSource(Trace trace, int count) {
		String retval;
		ComplexHeaderRule rule = trace.getExtension(ComplexHeaderRule.class);
		if (rule != null && rule.getTraceIDDefineExtension() != null || trace.getExtension(EntryTraceRule.class)!= null) {
			// Uses the extension tag with extension headers
			retval = OstConstants.EXTENSION_TRACE_TAG;
		} else {
			retval = String.valueOf(count);
		}
		return retval;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceHeaderContribution#
	 * getContribution(com.nokia.tracecompiler.project.TraceHeaderContribution.
	 * TraceHeaderContributionType)
	 */
	public String[] getContribution(TraceHeaderContributionType type) {
		String[] retval = null;
		if (type == TraceHeaderContributionType.GLOBAL_DEFINES) {
			retval = new String[] { "KOstTraceComponentID 0x" //$NON-NLS-1$
					+ Integer.toHexString(getOwner().getModel().getID()) };
		} else if (type == TraceHeaderContributionType.MAIN_HEADER_CONTENT) {
			retval = MAIN_HEADER_TEMPLATE;
		}
		return retval;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.nokia.tracecompiler.engine.rules.ExtensionBase#setOwner(com.nokia
	 * .tracecompiler.model.TraceObject)
	 */
	@Override
	public void setOwner(TraceObject owner) {
		if (getOwner() != null) {
			getOwner().getModel().removeModelListener(this);
		}
		super.setOwner(owner);
		if (owner != null) {
			owner.getModel().addModelListener(this);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 * objectAdded(com.nokia.tracecompiler.model.TraceObject,
	 * com.nokia.tracecompiler.model.TraceObject)
	 */
	public void objectAdded(TraceObject owner, TraceObject object) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 * objectCreationComplete(com.nokia.tracecompiler.model.TraceObject)
	 */
	public void objectCreationComplete(TraceObject object) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 * objectRemoved(com.nokia.tracecompiler.model.TraceObject,
	 * com.nokia.tracecompiler.model.TraceObject)
	 */
	public void objectRemoved(TraceObject owner, TraceObject object) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 * propertyUpdated(com.nokia.tracecompiler.model.TraceObject, int)
	 */
	public void propertyUpdated(TraceObject object, int property) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceProjectAPI#
	 * formatTraceForExport(com.nokia.tracecompiler.model.Trace,
	 * com.nokia.tracecompiler.project.TraceProjectAPI.TraceFormatFlags)
	 */
	public String formatTraceForExport(Trace trace, TraceFormatFlags flags) {
		// TODO: This uses default formats
		// -> Should be configurable
		// > for entry traces
		// < for exit traces
		// Logic analyzer format for performance traces
		String retval = trace.getTrace();
		AutomaticTraceTextRule rule = trace
				.getExtension(AutomaticTraceTextRule.class);
		int parameterCount = trace.getParameterCount();
		if (rule != null) {
			if (rule instanceof EntryTraceRule) {
				retval = "> " //$NON-NLS-1$
						+ retval;
			} else if (rule instanceof ExitTraceRule) {
				retval = "< " //$NON-NLS-1$
						+ retval;
			} else if (rule instanceof StateTraceRule) {
				if (parameterCount == 2) { // CodForChk_Dis_Magic
					retval = retval + "Machine" + PARAMETER_VALUE_SEPARATOR //$NON-NLS-1$
							+ STRING_PARAMETER_TAG + PARAMETER_SEPARATOR
							+ "State" + PARAMETER_VALUE_SEPARATOR //$NON-NLS-1$
							+ STRING_PARAMETER_TAG;
				} else {
					retval = retval
							+ "Machine" + PARAMETER_VALUE_SEPARATOR //$NON-NLS-1$
							+ STRING_PARAMETER_TAG + PARAMETER_SEPARATOR
							+ "State" + PARAMETER_VALUE_SEPARATOR //$NON-NLS-1$
							+ STRING_PARAMETER_TAG + PARAMETER_SEPARATOR
							+ "Instance" + PARAMETER_VALUE_SEPARATOR //$NON-NLS-1$
							+ HEX_PARAMETER_TAG;
				}
			}

			if (parameterCount >= 1 && !(rule instanceof StateTraceRule)) {
				TraceParameter param;
				for (int i = 0; i < parameterCount; i++) {
					param = trace.getParameter(i);
					retval += PARAMETER_SEPARATOR + param.getName()
							+ PARAMETER_VALUE_SEPARATOR
							+ SourceUtils.mapParameterTypeToFormat(param);
				}
			}
		} else {
			PerformanceEventRuleBase perf = trace
					.getExtension(PerformanceEventRuleBase.class);
			if (perf != null) {
				if (perf instanceof PerformanceEventStartRule) {
					retval = retval + ">" + PARAMETER_SEPARATOR //$NON-NLS-1$
							+ "Value" + PARAMETER_VALUE_SEPARATOR //$NON-NLS-1$
							+ SDEC_PARAMETER_TAG;
				} else if (perf instanceof PerformanceEventStopRule) {
					retval = retval
							+ "<" //$NON-NLS-1$
							+ PARAMETER_SEPARATOR
							+ "Value" + PARAMETER_VALUE_SEPARATOR //$NON-NLS-1$
							+ SDEC_PARAMETER_TAG
							+ PARAMETER_SEPARATOR
							+ "Start Event Trace ID" + PARAMETER_VALUE_SEPARATOR //$NON-NLS-1$
							+ UDEC_PARAMETER_TAG;
				}
			}
		}
		// If formatting is not supported the format characters and parameters
		// are removed.
		if (!flags.isFormattingSupported) {
			int index = retval.indexOf(PARAMETER_SEPARATOR);
			if (index > 0) {
				retval = retval.substring(0, index);
			}
			retval = SourceUtils.removePrintfFormatting(retval).trim();
		}
		return retval;
	}
}
