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
* Format to parameter type mapping
*
*/
package com.nokia.tracecompiler.source;

/**
 * Type returned by {@link SourceUtils#mapFormatToParameterType(String)}
 * 
 */
public final class FormatMapping {

	/**
	 * The type
	 */
	public String type;

	/**
	 * Array type flag
	 */
	public boolean isArray;

	/**
	 * Simple 32-bit type flag
	 */
	public boolean isSimple;

	/**
	 * Constructor
	 * 
	 * @param type
	 *            the type
	 */
	public FormatMapping(String type) {
		this.type = type;
	}

}
