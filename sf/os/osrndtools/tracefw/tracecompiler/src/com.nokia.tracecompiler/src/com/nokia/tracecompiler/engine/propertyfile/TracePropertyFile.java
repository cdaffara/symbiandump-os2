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
* Trace property file properties
*
*/
package com.nokia.tracecompiler.engine.propertyfile;

import org.w3c.dom.Document;

import com.nokia.tracecompiler.project.TraceProjectFile;

/**
 * Trace property file properties
 * 
 */
final class TracePropertyFile extends TraceProjectFile {

	/**
	 * Title shown in UI
	 */
	private static final String PROPERTY_FILE = Messages
			.getString("TracePropertyFile.Title"); //$NON-NLS-1$

	/**
	 * Creates a new property file
	 * 
	 * @param filePath
	 *            path to the file
	 * @param document
	 *            the document representing the property file
	 */
	TracePropertyFile(String filePath, Document document) {
		super(filePath, ""); //$NON-NLS-1$
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceProjectFile#getFileExtension()
	 */
	@Override
	protected String getFileExtension() {
		return PropertyFileConstants.PROPERTY_FILE_NAME;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceProjectFile#getTitle()
	 */
	@Override
	public String getTitle() {
		return PROPERTY_FILE;
	}
}
