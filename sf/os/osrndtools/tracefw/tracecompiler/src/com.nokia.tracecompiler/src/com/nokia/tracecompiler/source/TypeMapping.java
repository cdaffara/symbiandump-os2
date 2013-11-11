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
* Source to parameter type mapping
*
*/
package com.nokia.tracecompiler.source;

/**
 * Type returned by
 * {@link SourceUtils#mapSymbianTypeToParameterType(ParsedType)}
 * 
 */
public final class TypeMapping {

	/**
	 * Parameter type
	 */
	public String type;

	/**
	 * Explicit casting needs to be used when calling trace function
	 */
	public boolean needsCasting;

	/**
	 * Value is changed to pointer before passing to trace function
	 */
	public boolean valueToPointer;

	/**
	 * Contructor
	 * 
	 * @param type
	 *            the parameter type
	 */
	public TypeMapping(String type) {
		this.type = type;
	}

	/**
	 * Checks if type is void
	 * 
	 * @return true if type is void
	 */
	public boolean isVoid() {
		return type == null;
	}

}