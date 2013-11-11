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
* Project file for Dictionary format
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary;

import com.nokia.tracecompiler.project.TraceProjectFile;

/**
 * Project file for Dictionary format
 * 
 */
final class DictionaryFile extends TraceProjectFile {

	/**
	 * Title shown in UI
	 */
	private static final String TITLE = Messages
			.getString("DictionaryFile.Title"); //$NON-NLS-1$

	/**
	 * Constructor
	 * 
	 * @param absolutePath
	 *            the path to the dictionary file
	 */
	DictionaryFile(String absolutePath) {
		super(absolutePath, true);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceProjectFile#getFileExtension()
	 */
	@Override
	protected String getFileExtension() {
		return DictionaryFileConstants.DICTIONARY_FILE_EXTENSION;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceProjectFile#getTitle()
	 */
	@Override
	public String getTitle() {
		return TITLE;
	}

}
