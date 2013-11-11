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
* File-based implementation of the document interface
*
*/
package com.nokia.tracecompiler.document;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import com.nokia.tracecompiler.file.FileUtils;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.source.SourcePropertyProvider;

/**
 * File-based implementation of the document interface. This extends the string
 * document with SourcePropertyProvider implementation which can provide the
 * file name to TraceCompiler source framework
 * 
 */
final class FileDocument extends StringDocument implements
		SourcePropertyProvider {

	/**
	 * The file
	 */
	private File file;

	/**
	 * Constructor
	 * 
	 * @param file
	 *            the document file
	 * @throws IOException
	 *             if processing fails
	 */
	FileDocument(File file) throws IOException {
		this.file = file;
		byte[] buf = new byte[(int) file.length()];
		FileInputStream fis = new FileInputStream(file);
		fis.read(buf);
		fis.close();
		setSourceData(new String(buf));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.utils.DocumentAdapter#getPropertyProvider()
	 */
	@Override
	public SourcePropertyProvider getPropertyProvider() {
		return this;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourcePropertyProvider#getFileName()
	 */
	public String getFileName() {
		return file.getName();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourcePropertyProvider#getFilePath()
	 */
	public String getFilePath() {
		return FileUtils.convertSeparators(
				SourceConstants.FORWARD_SLASH_CHAR, file.getParent(), true);
	}

}
