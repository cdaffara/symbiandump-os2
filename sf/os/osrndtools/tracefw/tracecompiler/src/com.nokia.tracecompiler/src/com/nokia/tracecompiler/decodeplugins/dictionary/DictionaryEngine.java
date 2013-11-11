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
* TraceCompilerExport implementation for Symbian dictionary files
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary;

import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.plugin.TraceCompilerExport;

/**
 * TraceCompilerExport implementation for Symbian dictionary files
 * 
 */
public final class DictionaryEngine implements TraceCompilerExport {

	/**
	 * The trace model
	 */
	private TraceModel model;

	/**
	 * Dictionary instance
	 */
	private static DictionaryEngine instance;

	/**
	 * Gets the shared instance
	 * 
	 * @return the instance
	 */
	static DictionaryEngine getInstance() {
		return instance;
	}

	/**
	 * Constructor
	 */
	public DictionaryEngine() {
		instance = this;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerPlugin#
	 *      traceProjectOpened(com.nokia.tracecompiler.model.TraceModel)
	 */
	public void traceProjectOpened(TraceModel model) {
		this.model = model;
		DictionaryFile file = model.getExtension(DictionaryFile.class);
		if (file == null) {
			file = new DictionaryFile(DictionaryFileExport
					.getPathForDictionary(model));
			model.addExtension(file);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerPlugin#traceProjectClosed()
	 */
	public void traceProjectClosed() {
		if (model != null && model.isValid()) {
			model.removeExtensions(DictionaryFile.class);
		}
		model = null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerExport#exportTraceProject()
	 */
	public void exportTraceProject() throws TraceCompilerException {
		if (model != null && model.isValid()) {
			DictionaryFile df = model.getExtension(DictionaryFile.class);
			if (df != null) {
				DictionaryFileExport export = new DictionaryFileExport(df);
				export.exportDictionary();
			}
		}
	}

}
