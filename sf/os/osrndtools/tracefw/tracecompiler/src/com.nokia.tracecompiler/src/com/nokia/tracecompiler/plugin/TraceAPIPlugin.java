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
* Plug-in which provides trace API extensions
*
*/
package com.nokia.tracecompiler.plugin;

/**
 * Plug-in which provides trace API extensions
 * 
 */
public interface TraceAPIPlugin extends TraceCompilerPlugin {

	/**
	 * Gets the list of trace API formatters
	 * 
	 * @return the formatters
	 */
	public TraceAPIFormatter[] getFormatters();

	/**
	 * Gets the list of trace API parsers
	 * 
	 * @return the parsers
	 */
	public TraceAPIParser[] getParsers();

}
