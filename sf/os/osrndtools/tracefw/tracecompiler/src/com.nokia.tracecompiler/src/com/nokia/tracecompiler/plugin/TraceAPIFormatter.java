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
* Trace API formatter plug-in
*
*/
package com.nokia.tracecompiler.plugin;

/**
 * Trace API formatter plug-in
 * 
 */
public interface TraceAPIFormatter {

	/**
	 * Formatting types for traces
	 */
	public enum TraceFormatType {

		/**
		 * Format for extension function header
		 */
		HEADER,

		/**
		 * Format for trace buffer function. Added to extension function
		 * definition if trace contains more data than is supported by the API
		 */
		TRACE_BUFFER,

		/**
		 * Format for trace function if the extension function parameters can be
		 * packed into normal trace API call. For example, if the extension
		 * function takes 4 8-bit arguments, they are packed into a single
		 * 32-bit parameter
		 */
		TRACE_PACKED,

		/**
		 * Format for trace activation check function. Added to extension
		 * function definition
		 */
		TRACE_ACTIVATION,

		/**
		 * Empty declaration for an extension function header
		 */
		EMPTY_MACRO
	}

	/**
	 * Gets the name of this formatter
	 * 
	 * @return the name
	 */
	public String getName();

	/**
	 * Gets the title shown in the API selection preferences dialog
	 * 
	 * @return the title
	 */
	public String getTitle();

	/**
	 * Gets the trace format based on format type
	 * 
	 * @param type
	 *            the format type
	 * @return the trace format
	 */
	public String getTraceFormat(TraceFormatType type);

}
