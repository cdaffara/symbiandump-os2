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
* Error codes for source parser
*
*/
package com.nokia.tracecompiler.source;

/**
 * Error codes for source parser
 * 
 */
public interface SourceErrorCodes {

	/**
	 * Something unexpected happened while parsing source. This should be
	 * asserted
	 */
	int UNEXPECTED_EXCEPTION = 0; // CodForChk_Dis_Magic

	/**
	 * Missing owner from source context. This should be asserted
	 */
	int CONTEXT_MISSING_OWNER = 1; // CodForChk_Dis_Magic

	/**
	 * Unexpected separator while parsing parameters
	 */
	int UNEXPECTED_PARAMETER_SEPARATOR = 2; // CodForChk_Dis_Magic

	/**
	 * Unexpected end of file while parsing
	 */
	int UNEXPECTED_END_OF_FILE = 3; // CodForChk_Dis_Magic

	/**
	 * Unexpected quote character was encountered
	 */
	int UNEXPECTED_QUOTE_CHARACTER = 4; // CodForChk_Dis_Magic

	/**
	 * Bracket mismatch
	 */
	int BRACKET_MISMATCH = 5; // CodForChk_Dis_Magic

	/**
	 * Offset / length was not valid
	 */
	int BAD_LOCATION = 6; // CodForChk_Dis_Magic

}
