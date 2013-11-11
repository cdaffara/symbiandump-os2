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
* String-based document object
*
*/
package com.nokia.tracecompiler.document;

import java.util.ArrayList;

import com.nokia.tracecompiler.source.SourceParserException;
import com.nokia.tracecompiler.utils.DocumentAdapter;

/**
 * String-based document object
 * 
 */
class StringDocument extends DocumentAdapter {

	/**
	 * The document data
	 */
	private String sourceData;

	/**
	 * Line information
	 */
	private ArrayList<Integer> lines = new ArrayList<Integer>();

	/**
	 * Constructor
	 */
	StringDocument() {
	}

	/**
	 * Constructor
	 * 
	 * @param data
	 *            the document data
	 */
	StringDocument(String data) {
		setSourceData(data);
	}

	/**
	 * Sets the source data
	 * 
	 * @param data
	 *            the source data
	 */
	void setSourceData(String data) {
		this.sourceData = data;
		if (data.length() > 0) {
			lines.add(0);
		}
		for (int i = 0; i < data.length(); i++) {
			if (data.charAt(i) == '\n') {
				lines.add(i + 1);
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.utils.DocumentAdapter#get(int, int)
	 */
	@Override
	public String get(int start, int length) throws SourceParserException {
		return sourceData.substring(start, start + length);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.utils.DocumentAdapter#getChar(int)
	 */
	@Override
	public char getChar(int offset) throws SourceParserException {
		return sourceData.charAt(offset);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.utils.DocumentAdapter#getLength()
	 */
	@Override
	public int getLength() {
		return sourceData.length();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.utils.DocumentAdapter#getLineOfOffset(int)
	 */
	@Override
	public int getLineOfOffset(int offset) throws SourceParserException {
		int retval = -1;
		if (offset >= 0 && offset < sourceData.length()) {
			for (int i = 0; i < lines.size(); i++) {
				int lineStart = lines.get(i);
				if (lineStart > offset) {
					retval = i - 1;
					i = lines.size();
				}
			}
		}
		return retval;
	}

}
