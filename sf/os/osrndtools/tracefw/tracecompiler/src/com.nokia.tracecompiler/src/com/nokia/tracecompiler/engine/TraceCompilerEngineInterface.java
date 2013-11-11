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
* TraceCompiler interface
*
*/
package com.nokia.tracecompiler.engine;

import com.nokia.tracecompiler.model.TraceCompilerException;

/**
 * TraceCompiler interface
 * 
 */
public interface TraceCompilerEngineInterface {

	/**
	 * Opens the trace project related to given source file
	 * 
	 * @param modelName
	 *            the name for the model
	 * @throws TraceCompilerException
	 *             if startup fails
	 * @throws Exception 
	 */
	public void openProject(String modelName)
			throws Exception; //this include TraceCompilerException

	/**
	 * Exports the trace project
	 * 
	 * @throws TraceCompilerException
	 *             if export fails
	 */
	public void exportProject() throws TraceCompilerException;

	/**
	 * Closes the trace project
	 * @throws TraceCompilerException 
	 */
	public void closeProject() throws TraceCompilerException;

}
