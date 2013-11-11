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
* Result from parameter parsing
*
*/
package com.nokia.tracecompiler.engine.source;

import java.util.List;

/**
 * Result from parameter parsing
 * 
 */
public class SourceParserResult {

	/**
	 * Trace name as parsed from source
	 */
	public String originalName;

	/**
	 * Parsed name after conversion to valid name
	 */
	public String convertedName;

	/**
	 * Trace text as parsed from source
	 */
	public String traceText;

	/**
	 * Parameters parsed from source
	 */
	public List<String> parameters;

	/**
	 * List of parser-specific data
	 */
	public List<String> parserData;

}