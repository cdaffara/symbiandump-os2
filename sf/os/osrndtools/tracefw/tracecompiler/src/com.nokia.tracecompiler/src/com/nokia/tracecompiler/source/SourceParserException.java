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
* Exceptions thrown by source parser
*
*/
package com.nokia.tracecompiler.source;

/**
 * Exceptions thrown by source parser
 * 
 * @see SourceParser
 */
public class SourceParserException extends Exception {

	/**
	 * UID
	 */
	private static final long serialVersionUID = -2332983004093789880L; // CodForChk_Dis_Magic

	/**
	 * Error type
	 */
	private int errorType;

	/**
	 * Constructor with message and source object
	 * 
	 * @param errorType
	 *            the error type
	 */
	public SourceParserException(int errorType) {
		this.errorType = errorType;
	}

	/**
	 * Gets the error type
	 * 
	 * @return the type
	 */
	public int getErrorType() {
		return errorType;
	}

}
