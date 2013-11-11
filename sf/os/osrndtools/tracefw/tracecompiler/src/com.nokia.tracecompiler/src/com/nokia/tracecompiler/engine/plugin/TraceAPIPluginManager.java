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
* Trace API plug-in manager interface
*
*/
package com.nokia.tracecompiler.engine.plugin;

import com.nokia.tracecompiler.model.TraceModelExtension;
import com.nokia.tracecompiler.plugin.TraceAPIFormatter;
import com.nokia.tracecompiler.plugin.TraceAPIParser;

/**
 * Trace API plug-in manager interface
 * 
 */
public interface TraceAPIPluginManager extends TraceModelExtension {

	/**
	 * Adds the formatter API's from the plug-in
	 * 
	 * @param formatters
	 *            the formatters
	 */
	public void addFormatters(TraceAPIFormatter[] formatters);

	/**
	 * Adds the parser API's from the plug-in
	 * 
	 * @param parsers
	 *            the parsers
	 */
	public void addParsers(TraceAPIParser[] parsers);

	/**
	 * Sets the default API to the model.
	 */
	public void createDefaultAPI();

}
