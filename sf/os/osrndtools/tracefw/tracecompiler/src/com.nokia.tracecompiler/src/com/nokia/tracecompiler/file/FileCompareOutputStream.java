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
* Output stream which compares the data to existing file before writing
*
*/
package com.nokia.tracecompiler.file;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * Output stream which writes to a byte array. When writing is complete, the
 * array contents are compared to existing file and the file is written only if
 * there are changes
 * 
 */
public class FileCompareOutputStream extends ByteArrayOutputStream {

	/**
	 * The header file
	 */
	private File file;

	/**
	 * Creates a new stream
	 * 
	 * @param file
	 *            the header file
	 */
	public FileCompareOutputStream(File file) {
		this.file = file;
	}

	/**
	 * Writes the buffer to file if it has changed
	 * 
	 * @return true if header was written, false if it matched the existing
	 *         header
	 * @throws IOException
	 *             if writing fails
	 */
	public boolean writeFile() throws IOException {
		// Compares the file length to number of bytes in array
		boolean writeHeader = true;
		if (file.length() == count) {
			FileInputStream fis = new FileInputStream(file);
			byte[] temp = new byte[count];
			fis.read(temp);
			fis.close();
			writeHeader = false;
			// Compares the file content
			for (int i = 0; i < count; i++) {
				if (temp[i] != buf[i]) {
					writeHeader = true;
					i = count;
				}
			}
		}
		if (writeHeader) {
			OutputStream fos = FileUtils.createOutputStream(file);
			writeTo(fos);
			fos.close();
		}
		return writeHeader;
	}
}