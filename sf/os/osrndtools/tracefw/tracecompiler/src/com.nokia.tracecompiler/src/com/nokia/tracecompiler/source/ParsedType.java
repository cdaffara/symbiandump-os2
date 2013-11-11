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
* Interface for types parsed from source files
*
*/
package com.nokia.tracecompiler.source;

/**
 * Interface for types parsed from source files
 * 
 */
public interface ParsedType {

	/**
	 * Checks if parameter is pointer (*) or pointer reference (*&)
	 * 
	 * @return true if pointer, false if not
	 */
	public boolean isPointer();

	/**
	 * Checks if parameter type equals given type
	 * 
	 * @param type
	 *            the type to be checked
	 * @return true if types match
	 */
	public boolean typeEquals(String type);

	/**
	 * Checks if the type has the given qualifier
	 * 
	 * @param qualifier
	 *            the qualifier to be checked
	 * @return true if qualifier exists, false if not
	 */
	public boolean hasQualifier(String qualifier);

}
