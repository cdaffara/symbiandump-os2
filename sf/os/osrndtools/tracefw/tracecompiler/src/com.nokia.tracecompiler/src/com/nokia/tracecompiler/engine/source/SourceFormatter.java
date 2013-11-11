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
 * Formatting rules are provided by Trace objects
 *
 */
package com.nokia.tracecompiler.engine.source;

import java.util.Iterator;

import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.rules.StateTraceRule;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.plugin.TraceFormatConstants;
import com.nokia.tracecompiler.plugin.TraceAPIFormatter.TraceFormatType;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.source.SourceUtils;
import com.nokia.tracecompiler.source.SymbianConstants;

/**
 * Static functions for source formatting
 * 
 */
public class SourceFormatter {

	/**
	 * Value parameter name
	 */
	public static final String VALUE_PARAMETER_NAME = "value"; //$NON-NLS-1$	

	/**
	 * Event start trace id parameter name
	 */
	public static final String EVENT_START_TRACE_ID_PARAMETER_NAME = "linkToStart"; //$NON-NLS-1$		

	/**
	 * Constructor is hidden
	 */
	private SourceFormatter() {
	}

	/**
	 * Formats a trace to string format using the formatting rule from the trace
	 * 
	 * @param trace
	 *            the trace
	 * @param formatType
	 *            the type of format
	 * @return the trace string
	 */
	public static String formatTrace(Trace trace, TraceFormatType formatType) {
		return formatTrace(trace, null, formatType, null, false);
	}

	/**
	 * Formats a trace to string format
	 * 
	 * @param trace
	 *            the trace
	 * @param traceRule
	 *            the formatting rule to be used
	 * @param formatType
	 *            the type of format
	 * @param tags
	 *            the tags for parameters or null if parameter names are used
	 * @param fixedTags
	 *            true if the <i>tags</i> iterator is fixed, false if the
	 *            contents of <i>tags</i> should go through
	 *            <code>SourceRule.mapNameToSource</code>
	 * @return the trace string
	 */
	static String formatTrace(Trace trace, TraceFormattingRule traceRule,
			TraceFormatType formatType, Iterator<String> tags, boolean fixedTags) {
		StringBuffer data = new StringBuffer();
		String format = null;
		if (traceRule == null) {

			// If rule is not explicitly provided, it is fetched from the trace
			traceRule = trace.getExtension(TraceFormattingRule.class);
			if (traceRule == null) {

				// If trace does not have a formatting rule, the project API's
				// should implement default rule
				traceRule = trace.getModel().getExtension(
						TraceFormattingRule.class);
			}
		}
		if (traceRule != null) {
			format = traceRule.getFormat(trace, formatType);
		}
		if (format != null && traceRule != null) {
			data.append(format);
			data.append(SourceConstants.LINE_FEED);
			buildParameterList(trace, traceRule, data, formatType, tags,
					fixedTags);

			String traceName = traceRule.mapNameToSource(trace);
			String traceGroupName = trace.getGroup().getName();

			// %NAME% is replaced with rule-mapped trace name
			replaceData(data, traceName, TraceFormatConstants.NAME_FORMAT);

			// %GROUP% is replaced with group name
			replaceData(data, traceGroupName, TraceFormatConstants.GROUP_FORMAT);

			// %TEXT% is replaced with trace text
			replaceData(data, "\"" + trace.getTrace() //$NON-NLS-1$
					+ "\"", TraceFormatConstants.TEXT_FORMAT); //$NON-NLS-1$

			// %FORMATTED_TRACE% is replaced with trace data
			replaceData(data, trace.getTrace(),
					TraceFormatConstants.FORMATTED_TRACE);

			// Comment is inserted before the trace
			int index = data.indexOf(TraceFormatConstants.COMMENT_FORMAT);
			if (index >= 0) {
				String comment = data.substring(index + 1);
				data.delete(index, data.length());
				data.insert(0, comment);
				data.append(SourceConstants.LINE_FEED);
			}
		}

		// If trace does not have formatting, it is not shown in source
		return data.toString();
	}

	/**
	 * Adds the parameters to the data buffer
	 * 
	 * @param trace
	 *            the trace
	 * @param format
	 *            the formatter from trace
	 * @param data
	 *            the data buffer where the formatted data is stored
	 * @param formatType
	 *            the format type to be applied
	 * @param tags
	 *            the tags for parameters or null if parameter names are used
	 * @param fixedTags
	 *            true if the <i>tags</i> iterator is fixed, false if the
	 *            contents of <i>tags</i> should go through
	 *            <code>SourceRule.mapNameToSource</code>
	 */
	private static void buildParameterList(Trace trace,
			TraceFormattingRule format, StringBuffer data,
			TraceFormatType formatType, Iterator<String> tags, boolean fixedTags) {

		int count = trace.getParameterCount();

		Iterator<TraceParameter> itr = trace.getParameters();
		StringBuffer paramList = new StringBuffer();
		// Index is incremented by one for each parameter that has been added to
		// source
		int parameterIndex = 0;
		while (itr.hasNext()) {
			TraceParameter param = itr.next();
			TraceParameterFormattingRule rule = param
					.getExtension(TraceParameterFormattingRule.class);
			String name;
			// Count and name may be adjusted by rules provided by parameters
			if (rule != null) {
				boolean isInSource = rule.isShownInSource();
				// If the parameter iterator is explicitly provided, the
				// parameter name is fetched from it. If the parameter list does
				// not have enough tags (for example when a new parameter is
				// added to trace) the name of the parameter is used. The source
				// rule is used to map the parameter name to correct format
				if (isInSource) {
					name = getTagWithoutMapping(tags, param);
					addParameter(paramList, param, name, ++parameterIndex,
							formatType);
				} else {
					// If the parameter is not shown in source, it is skipped
					count--;
				}
			} else {
				// If the parameter is not associated with a source rule, it is
				// added without mapping
				name = getTagWithoutMapping(tags, param);
				addParameter(paramList, param, name, ++parameterIndex,
						formatType);
			}
		}
		// %PC% is replaced with adjusted parameter count
		// In case of packed trace, the header engine does the count mapping
		if (formatType != TraceFormatType.TRACE_PACKED) {
			String val = format.mapParameterCountToSource(trace, count);

			if (trace.getExtension(StateTraceRule.class) != null
					&& data.toString().startsWith("OstTraceState")) { //$NON-NLS-1$

				// In case of State Trace macro value in trace macro is
				// parameter count - 2
				if (count > 1) {
					val = String.valueOf(count - 2); // CodForChk_Dis_Magic
				} else {
					val = String.valueOf(count);
				}
			}
			replaceData(data, val, TraceFormatConstants.PARAM_COUNT_FORMAT);
		}
		// %PARAMETERS% is replaced with parameter names
		replaceData(data, paramList.toString(),
				TraceFormatConstants.PARAMETERS_FORMAT);
	}

	/**
	 * Gets the name for a parameter without source rule mapping. If the tags
	 * iterator contains a valid entry, the name is fetched from it. If not, the
	 * parameter name is used instead.
	 * 
	 * @param tags
	 *            the list of tags
	 * @param param
	 *            the parameter
	 * @return the parameter name
	 */
	private static String getTagWithoutMapping(Iterator<String> tags,
			TraceParameter param) {
		String name;
		// If the parameter iterator is explicitly provided, the
		// parameter name is fetched from it
		if (tags != null && tags.hasNext()) {
			name = tags.next();
			// The list may contain 0-length items to represent
			// that that parameter name should be used instead
			if (name == null || name.length() == 0) {
				name = param.getName();
			}
		} else {
			name = param.getName();
		}
		return name;
	}

	/**
	 * Adds a parameter to the parameter list
	 * 
	 * @param paramList
	 *            the parameter list
	 * @param param
	 *            the parameter to be added
	 * @param name
	 *            a name replacement for the parameter
	 * @param parameterIndex
	 *            the index of the parameter
	 * @param formatType
	 *            the type of the format
	 */
	private static void addParameter(StringBuffer paramList,
			TraceParameter param, String name, int parameterIndex,
			TraceFormatType formatType) {
		paramList.append(SourceConstants.PARAMETER_SEPARATOR);
		if (formatType == TraceFormatType.HEADER) {
			paramList.append(SourceUtils.mapParameterTypeToSymbianType(param));
			paramList.append(SourceConstants.SPACE_CHAR);
			paramList.append(SymbianConstants.PARAMETER_DECLARATION_PREFIX);
			paramList.append(parameterIndex);
		} else if (formatType == TraceFormatType.EMPTY_MACRO) {
			paramList.append(SymbianConstants.PARAMETER_DECLARATION_PREFIX);
			paramList.append(parameterIndex);
		}
	}

	/**
	 * Replaces data from the stringbuffer
	 * 
	 * @param data
	 *            the data
	 * @param replaceData
	 *            the data to be used
	 * @param replaceFormat
	 *            the format to be replaced
	 */
	private static void replaceData(StringBuffer data, String replaceData,
			String replaceFormat) {
		TraceCompilerEngineGlobals.getEvents().postInfoMessage(Messages.getString("SourceFormatter.replaceAllBeginText") + replaceFormat + Messages.getString("SourceFormatter.replaceAllMiddleText") + replaceData , null); //$NON-NLS-1$ //$NON-NLS-2$

		int replaceOffset = 0;
		do {
			replaceOffset = data.indexOf(replaceFormat, replaceOffset);
			if (replaceOffset >= 0) {
				data.replace(replaceOffset, replaceOffset
						+ replaceFormat.length(), replaceData);
				replaceOffset += replaceData.length();
			}
		} while (replaceOffset != -1);
	}

}
