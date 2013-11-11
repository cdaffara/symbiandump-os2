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
* DataType enumeration
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * DataType enumeration
 * 
 */
public enum DataType {

	/**
	 * String type
	 */
	STRING("string"), //$NON-NLS-1$

	/**
	 * Integer type
	 */
	INTEGER("integer"), //$NON-NLS-1$

	/**
	 * Float type
	 */
	FLOAT("float"), //$NON-NLS-1$

	/**
	 * Hex type
	 */
	HEX("hex"), //$NON-NLS-1$

	/**
	 * Binary type
	 */
	BINARY("binary"), //$NON-NLS-1$

	/**
	 * Octal type
	 */
	OCTAL("octal"), //$NON-NLS-1$

	/**
	 * Enum type
	 */
	ENUM("enum"), //$NON-NLS-1$

	/**
	 * Raw type
	 */
	RAW("raw"), //$NON-NLS-1$

	/**
	 * Compound type
	 */
	COMPOUND("compound"); //$NON-NLS-1$

	/**
	 * Data type
	 */
	private final String type;

	/**
	 * Constructor
	 * 
	 * @param type
	 *            the type
	 */
	private DataType(String type) {
		this.type = type;
	}

	/**
	 * Gets the type as string
	 * 
	 * @return type
	 */
	public String getType() {
		return type;
	}

}
