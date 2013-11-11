/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Document monitor implementation which reads all files from a directory
*
*/
package com.nokia.tracecompiler.document;

import java.io.File;
import java.util.ArrayList;

import com.nokia.tracecompiler.TraceCompilerGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorMessages;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.engine.source.SourceEngine;
import com.nokia.tracecompiler.file.FileUtils;
import com.nokia.tracecompiler.source.SourceDocumentFactory;
import com.nokia.tracecompiler.source.SourceDocumentProcessor;
import com.nokia.tracecompiler.utils.DocumentMonitorBase;

/**
 * Document monitor implementation which reads files
 * 
 */
public final class FileDocumentMonitor extends DocumentMonitorBase {

	/**
	 * Document factory
	 */
	private StringDocumentFactory documentFactory;

	/**
	 * Files to be processed
	 */
	private static String[] files;

	/**
	 * Sets the files to be used by the document monitor
	 * 
	 * @param files
	 *            the files
	 */
	public static void setFiles(String[] files) {
		FileDocumentMonitor.files = files;
	}

	/**
	 * Constructor
	 */
	public FileDocumentMonitor() {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.utils.DocumentMonitorBase#getFactory()
	 */
	@Override
	public SourceDocumentFactory getFactory() {
		if (documentFactory == null) {
			documentFactory = new StringDocumentFactory();
		}
		return documentFactory;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.utils.DocumentMonitorAdapter#
	 *      startMonitor(com.nokia.tracecompiler.source.SourceDocumentProcessor)
	 */
	@Override
	public void startMonitor(SourceDocumentProcessor processor) throws Exception {
		
		// Put source files and non-source files to same list
		SourceEngine sourceEngine = TraceCompilerEngineGlobals.getSourceEngine();
		ArrayList<String> list = sourceEngine.getNonSourceFiles();
		for (int i=0; i < files.length; i++) {
			String fileName = files[i];
			if (!list.contains(fileName)) {
				list.add(fileName);
			}
		}		
		String[] fileArr = new String[list.size()];
		list.toArray(fileArr);
		String err = ""; //$NON-NLS-1$
		
		for (String fileName : fileArr) {
			File file = new File(fileName);
			if (file.exists() && !file.isDirectory()
					&& FileUtils.isFileAllowed(file.getName())) {
				try {
					TraceCompilerEngineGlobals.getEvents().postInfoMessage(Messages.getString("FileDocumentMonitor.processingSourceFileText") + file.getAbsolutePath(), null); //$NON-NLS-1$
					processor.sourceOpened(new FileDocument(file));
				} catch (Exception e) {
					// Error after processing one file
					String msg = TraceCompilerEngineErrorMessages
							.getErrorMessage(
									TraceCompilerErrorCode.CANNOT_OPEN_SOURCE_FILE,
									null);
					TraceCompilerEngineGlobals.getEvents().postErrorMessage(msg,
							file.getAbsolutePath(),true);
					err = e.getMessage();
				} finally {
					if (TraceCompilerEngineGlobals.getEvents().hasErrorHappened() && !TraceCompilerGlobals.keepGoing())  {
						throw new Exception("TraceCompiler stopping on error. " + err); //$NON-NLS-1$
					}
				}
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.utils.DocumentMonitorAdapter#stopMonitor()
	 */
	@Override
	public void stopMonitor() {
	}

}
