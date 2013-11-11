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
* Contains search variables during SourceParameterTokenizer.tokenizeParameters call
*
*/
package com.nokia.tracecompiler.source;

/**
 * Contains search variables during SourceParameterTokenizer.tokenizeParameters
 * call
 * 
 */
final class TokenizerSearchData {

	/**
	 * Iterator over source characters
	 */
	SourceIterator itr;

	/**
	 * Current character
	 */
	char value;

	/**
	 * Previous character
	 */
	char previousValue = ' ';

	/**
	 * Number of '(' encountered without matching ')' character
	 */
	int openBracketCount;

	/**
	 * Index for start of parameter
	 */
	int tagStartIndex;

	/**
	 * Index for start of parameter
	 */
	int paramStartIndex;

	/**
	 * Initial count of '(' characters at beginning of parameter
	 */
	int initialBracketCount;

	/**
	 * Closing ')' character has been found.
	 */
	boolean complete;

	/**
	 * Ending ';' or '{' character has been found
	 */
	boolean endFound;

	/**
	 * Within quotes flag
	 */
	boolean inQuotes;

	/**
	 * Parameter definition object
	 */
	SourceParameter sourceParameter;

	/**
	 * Data flag
	 */
	boolean hasData;
}