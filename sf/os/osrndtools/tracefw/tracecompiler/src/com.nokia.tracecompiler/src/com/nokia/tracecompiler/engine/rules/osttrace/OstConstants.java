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
* Constants for OST parser and formatter
*
*/
package com.nokia.tracecompiler.engine.rules.osttrace;

/**
 * Constants for OST parser and formatter
 * 
 */
interface OstConstants {

	/**
	 * API tag for macros which have the preprocessor level parameter
	 */
	String PREPROCESSOR_LEVEL_TAG = "Def"; //$NON-NLS-1$

	/**
	 * API tag for function entry traces
	 */
	String FUNCTION_ENTRY_TAG = "FunctionEntry"; //$NON-NLS-1$

	/**
	 * API tag for function exit traces
	 */
	String FUNCTION_EXIT_TAG = "FunctionExit"; //$NON-NLS-1$

	/**
	 * API tag for performance event start
	 */
	String PERFORMANCE_EVENT_START_TAG = "EventStart"; //$NON-NLS-1$

	/**
	 * API tag for performance event stop
	 */
	String PERFORMANCE_EVENT_STOP_TAG = "EventStop"; //$NON-NLS-1$

	/**
	 * API tag for traces with user-defined data format
	 */
	String DATA_TRACE_TAG = "Data"; //$NON-NLS-1$
	
	/**
	 * API tag for state traces
	 */
	String STATE_TRACE_TAG = "State"; //$NON-NLS-1$

	/**
	 * API tag for traces with extension parameters
	 */
	String EXTENSION_TRACE_TAG = "Ext"; //$NON-NLS-1$

}
