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
* Base class for project file parsers
*
*/
package com.nokia.tracecompiler.project;

import java.io.File;

import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.FileErrorParameters;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;

/**
 * Base class for project file parsers
 * 
 */
public abstract class ProjectFileParser {

	/**
	 * Trace model
	 */
	protected TraceModel model;

	/**
	 * Project file to be parsed
	 */
	protected File projectFile;

	/**
	 * Constructor
	 * 
	 * @param model
	 *            the trace model
	 * @param fileName
	 *            the project file name
	 * @throws TraceCompilerException
	 *             if file does not exist
	 */
	protected ProjectFileParser(TraceModel model, String fileName)
			throws TraceCompilerException {
		File file = new File(fileName);
		if (file.exists()) {
			this.model = model;
			this.projectFile = file;
			createParser();
		} else {
			FileErrorParameters params = new FileErrorParameters();
			params.file = fileName;
			throw new TraceCompilerException(
					TraceCompilerErrorCode.FILE_NOT_FOUND, params);
		}
	}

	/**
	 * Creates the file parser
	 * 
	 * @throws TraceCompilerException
	 *             if creation fails
	 */
	protected abstract void createParser() throws TraceCompilerException;

	/**
	 * Parses the project file
	 * 
	 * @throws TraceCompilerException
	 *             if parser fails
	 */
	public abstract void parse() throws TraceCompilerException;

}
