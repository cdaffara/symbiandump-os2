/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Formatting constants for traces
 *
 */
package com.nokia.tracecompiler.plugin;

/**
 * Formatting constants for traces
 * 
 */
public interface TraceFormatConstants {

	/**
	 * Function name in upper case
	 */
	String FORMAT_FUNCTION_NAME_UPPER_CASE = "{$FN}"; //$NON-NLS-1$

	/**
	 * Function name in normal case
	 */
	String FORMAT_FUNCTION_NAME_NORMAL_CASE = "{$fn}"; //$NON-NLS-1$

	/**
	 * Class name in upper case
	 */
	String FORMAT_CLASS_NAME_UPPER_CASE = "{$CN}"; //$NON-NLS-1$

	/**
	 * Class name in normal case
	 */
	String FORMAT_CLASS_NAME_NORMAL_CASE = "{$cn}"; //$NON-NLS-1$

	/**
	 * Tag which is replaced by trace text when inserting a trace to source
	 */
	String FORMATTED_TRACE = "%FORMATTED_TRACE%"; //$NON-NLS-1$

	/**
	 * Tag which is replaced by parameters when inserting a trace to source
	 */
	String PARAMETERS_FORMAT = "%PARAMETERS%"; //$NON-NLS-1$

	/**
	 * Insert format for include header
	 */
	String INCLUDE_FORMAT = "%INCLUDE%"; //$NON-NLS-1$

	/**
	 * Tag which is replaced by parameter count when inserting a trace to source
	 */
	String PARAM_COUNT_FORMAT = "%PC%"; //$NON-NLS-1$

	/**
	 * Tag which is replaced by group name when inserting a trace to source
	 */
	String GROUP_FORMAT = "%GROUP%"; //$NON-NLS-1$

	/**
	 * Tag which is replaced by trace name when inserting a trace to source
	 */
	String NAME_FORMAT = "%NAME%"; //$NON-NLS-1$

	/**
	 * Tag which is replaced by state machine name when inserting a trace to
	 * source
	 */
	String STATE_MACHINE_NAME_FORMAT = "%STATE_MACHINE_NAME%"; //$NON-NLS-1$

	/**
	 * Tag which is replaced by state machine state when inserting a trace to
	 * source
	 */
	String STATE_MACHINE_STATE_FORMAT = "%STATE_MACHINE_STATE%"; //$NON-NLS-1$

	/**
	 * Tag which is replaced by event name when inserting a trace to source
	 */
	String EVENT_NAME_FORMAT = "%EVENT_NAME%"; //$NON-NLS-1$

	/**
	 * Tag which is replaced by event start trace name when inserting a trace to
	 * source
	 */
	String EVENT_START_TRACE_NAME_FORMAT = "%EVENT_START_TRACE_NAME%"; //$NON-NLS-1$

	/**
	 * Tag which is replaced by trace text when inserting a trace to source
	 */
	String TEXT_FORMAT = "%TEXT%"; //$NON-NLS-1$

	/**
	 * Tag which is replaced with the data buffer pointer when formatting
	 * complex traces into the trace header
	 */
	String DATA_BUFFER_FORMAT = "%DATA%"; //$NON-NLS-1$

	/**
	 * Tag which is replaced with the data buffer length when formatting complex
	 * traces into the trace header
	 */
	String DATA_LENGTH_FORMAT = "%LENGTH%"; //$NON-NLS-1$

	/**
	 * Comment format which needs to be used. Otherwise the comment before /
	 * after trace configuration will not work
	 */
	String COMMENT_FORMAT = " //"; //$NON-NLS-1$

}
