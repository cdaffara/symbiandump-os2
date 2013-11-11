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
* Contains search variables during SourceParser.createContext call
*
*/
package com.nokia.tracecompiler.source;

/**
 * Contains search variables during SourceParser.createContext call
 * 
 */
final class ContextSearchData {

	/**
	 * Index is currently in parameters
	 */
	int inParameters;

	/**
	 * Parameters have been found
	 */
	boolean parametersFound;

	/**
	 * Start of function name
	 */
	int functionStartIndex = -1;

	/**
	 * End of function name
	 */
	int functionEndIndex = -1;

	/**
	 * Start of class name
	 */
	int classStartIndex = -1;

	/**
	 * End of class name
	 */
	int classEndIndex = -1;

	/**
	 * End of return statement
	 */
	int returnEndIndex = -1;

	/**
	 * Context is valid
	 */
	boolean valid;

	/**
	 * Processing is finished
	 */
	boolean finished;

	/**
	 * The iterator
	 */
	SourceIterator itr;

	/**
	 * The context
	 */
	SourceContext context;

}