/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Exporter for dictionary file
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary;

import java.io.File;

import com.nokia.tracecompiler.engine.TraceCompilerEngineConfiguration;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.file.FileUtils;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.source.SourceConstants;

/**
 * Exporter for dictionary file
 * 
 */
final class DictionaryFileExport {

	/**
	 * The dictionary file properties
	 */
	private DictionaryFile dictionaryFile;

	/**
	 * Constructor
	 * 
	 * @param dictionary
	 *            the dictionary file
	 */
	DictionaryFileExport(DictionaryFile dictionary) {
		this.dictionaryFile = dictionary;
	}

	/**
	 * Exports the dictionary file
	 */
	void exportDictionary() {
		String exportPath = getExportPath(File.separatorChar);
		String exportFile = dictionaryFile.getFileName();
		String envRoot = TraceCompilerEngineGlobals.getConfiguration().getText(
				TraceCompilerEngineConfiguration.ENVIRONMENT_ROOT);
		if (envRoot != null) {
			File target = new File(envRoot + exportPath + exportFile);
			// The dictionary file is updated with the new environment root
			dictionaryFile.updatePath(target.getAbsolutePath());
			DictionaryFileWriter writer = new DictionaryFileWriter(
					dictionaryFile);
			writer.write();
			dictionaryFile.postFileWrittenEvent(dictionaryFile
					.getAbsolutePathWithID());
		}
	}

	/**
	 * Returns the export path for dictionary files
	 * 
	 * @param separator
	 *            the file separator to use
	 * @return the export path
	 */
	private static String getExportPath(char separator) {
		String exportPath = DictionaryPreferences.DEFAULT_EXPORT_PATH;

		// Accepts both types of separators, ends with separator
		exportPath = FileUtils.convertSeparators(separator, exportPath, true);
		return exportPath;
	}

	/**
	 * Gets the default path to the dictionary file
	 * 
	 * @param model
	 *            trace model
	 * @return the file
	 */
	static String getPathForDictionary(TraceModel model) {
		String fileName = model.getName()
				+ DictionaryFileConstants.DICTIONARY_FILE_EXTENSION;
		String exportPath = getExportPath(File.separatorChar);
		String envRoot = TraceCompilerEngineGlobals.getConfiguration().getText(
				TraceCompilerEngineConfiguration.ENVIRONMENT_ROOT);
		return FileUtils.convertSeparators(SourceConstants.FORWARD_SLASH_CHAR,
				envRoot + exportPath + fileName, false);
	}

}
