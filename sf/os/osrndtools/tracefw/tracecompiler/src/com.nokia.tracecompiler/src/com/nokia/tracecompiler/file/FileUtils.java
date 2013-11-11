/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* File utility functions
*
*/
package com.nokia.tracecompiler.file;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import com.nokia.tracecompiler.source.SourceConstants;

/**
 * File utility functions
 * 
 */
public final class FileUtils {

	/**
	 * File copy buffer size
	 */
	private static final int COPY_BUFFER_SIZE = 4096; // CodForChk_Dis_Magic

	/**
	 * cpp extension
	 */
	public final static String CPP_EXTENSION = "cpp"; //$NON-NLS-1$	

	/**
	 * c extension
	 */
	public final static String C_EXTENSION = "c"; //$NON-NLS-1$	

	/**
	 * Allowed files
	 */
	private final static String[] FILE_FILTERS = { ".cpp", //$NON-NLS-1$
			".c", //$NON-NLS-1$
			".inl", //$NON-NLS-1$
			".h" //$NON-NLS-1$
	};

	/**
	 * MMP file extension
	 */
	public static final String MMP = ".mmp"; //$NON-NLS-1$

	/**
	 * number of times trying to create a directy(ies)
	 */
	private static final int MAX_DIRECTORY_CREATION_TRIES = 5;
	
	/**
	 * Time to sleep between tries to create a directory(ies)
	 */
	private static final int SLEEP_TIME_BETWEEN_DIRECTORY_CREATION_TRIES = 100;

	
	/**
	 * Creates a file output stream. This creates directories and overwriting
	 * possible read-only flag
	 * 
	 * @param file
	 *            the file
	 * @return the file output stream
	 * @throws FileNotFoundException
	 *             if file cannot be created
	 */
	public static OutputStream createOutputStream(File file)
			throws FileNotFoundException {
		File parent = file.getParentFile();
		if (!parent.exists()) {
			createDirectories(parent);
		}
		FileOutputStream fos;
		try {
			fos = new FileOutputStream(file);
		} catch (IOException e) {
			if (file.exists()) {
				file.delete();
			}
			fos = new FileOutputStream(file);
		}
		return new BufferedOutputStream(fos);
	}

	/**
	 * Creates a copy of a file
	 * 
	 * @param source
	 *            the source file
	 * @param target
	 *            the target file
	 * @return true if written successfully
	 */
	public static boolean copyFile(File source, File target) {
		boolean backup = true;
		try {
			byte[] buf = new byte[COPY_BUFFER_SIZE];
			FileInputStream fis = new FileInputStream(source);
			OutputStream fos = createOutputStream(target);
			int len;
			do {
				len = fis.read(buf);
				if (len > 0) {
					fos.write(buf, 0, len);
				}
			} while (len > 0);
			fis.close();
			fos.close();
		} catch (Exception e) {
			backup = false;
		}
		return backup;
	}

	/**
	 * Converts file separator characters
	 * 
	 * @param separator
	 *            separator to be used
	 * @param path
	 *            string to be converted
	 * @param addLast
	 *            true if the converted string should end with a separator
	 * @return the converted string
	 */
	public static String convertSeparators(char separator, String path,
			boolean addLast) {
		path = path.replace(SourceConstants.FORWARD_SLASH_CHAR, separator);
		path = path.replace(SourceConstants.BACKSLASH_CHAR, separator);
		String sepStr = String.valueOf(separator);
		if (addLast && !path.endsWith(sepStr)) {
			path += separator;
		} else if (!addLast && path.endsWith(sepStr)) {
			path = path.substring(0, path.length() - 1);
		}
		return path;
	}

	/**
	 * Checks if given file is allowed to be opened into TraceCompiler
	 * 
	 * @param fileName
	 *            the file to be checked
	 * @return true if filtered, false if not
	 */
	public static boolean isFileAllowed(String fileName) {
		boolean allowed = false;
		fileName = fileName.toLowerCase();
		for (String filter : FILE_FILTERS) {
			if (fileName.endsWith(filter)) {
				allowed = true;
				break;
			}
		}
		return allowed;
	}

	/**
	 * Create directories safely in a multiple instances case
	 * @param path
	 * @param access
	 * @return boolean pass/fail
	 */
	public static boolean createDirectories(File path) {

		boolean retVal = true;

		int tries = 0;

		if (path.exists()) {
			return retVal;
		}

		// try few times
		while (tries < MAX_DIRECTORY_CREATION_TRIES) {

			retVal = path.mkdirs();
			// mkdirs can fail for a number of reasons including the case where
			// the directory is open by another process. The API does not make
			// any difference so we assume the latter case (worst case)
			if (!retVal) { // sleep for moment and try again.

				tries++;

				try {
					Thread.sleep(SLEEP_TIME_BETWEEN_DIRECTORY_CREATION_TRIES);
				} catch (InterruptedException e) {
					// Do nothing
				}
			} else { // pass
				break;
			}
		}

		return retVal;

	}
}
