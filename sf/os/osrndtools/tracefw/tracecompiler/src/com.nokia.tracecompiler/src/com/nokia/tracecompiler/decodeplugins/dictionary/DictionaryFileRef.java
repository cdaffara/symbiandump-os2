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
* Reference to a file
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary;

/**
 * Reference to a file
 * 
 */
final class DictionaryFileRef extends DictionaryRef {

	/**
	 * File name
	 */
	String file;

	/**
	 * File path
	 */
	String path;

	/**
	 * Trace
	 */
	com.nokia.tracecompiler.model.Trace trace;

	/**
	 * Constructor
	 * 
	 * @param file
	 *            the file name
	 * @param path
	 *            the file path
	 * @param trace
	 *            the trace
	 */
	DictionaryFileRef(String file, String path,
			com.nokia.tracecompiler.model.Trace trace) {
		super(0);
		this.file = file;
		this.path = path;
		this.trace = trace;
	}

}