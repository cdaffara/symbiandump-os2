/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Constants for header engine package
*
*/
package com.nokia.tracecompiler.engine.header;

/**
 * Constants for header engine package
 * 
 */
interface HeaderConstants {

	/**
	 * Number of parent paths to check when locating include directory
	 */
	int INC_SEARCH_DEPTH = 3; // CodForChk_Dis_Magic

	/**
	 * Trace header extension
	 */
	String TRACE_HEADER_EXTENSION = "Traces.h"; //$NON-NLS-1$

	/**
	 * Main trace header name
	 */
	String TRACE_HEADER_NAME = "OstTraceDefinitions.h"; //$NON-NLS-1$

}
