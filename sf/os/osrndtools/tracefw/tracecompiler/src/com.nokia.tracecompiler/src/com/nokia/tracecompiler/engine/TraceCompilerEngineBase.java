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
* Base class for all engine classes
*
*/
package com.nokia.tracecompiler.engine;

import com.nokia.tracecompiler.model.TraceCompilerException;

/**
 * Base class for all engine classes
 * 
 */
public abstract class TraceCompilerEngineBase {

	/**
	 * Called after trace project has been opened
	 * @throws TraceCompilerException 
	 */
	public abstract void projectOpened() throws TraceCompilerException;

	/**
	 * Called when trace project is closed
	 */
	public abstract void projectClosed();

	/**
	 * Called when exporting the project
	 * 
	 * @throws TraceCompilerException
	 *             if export fails
	 */
	public abstract void exportProject() throws TraceCompilerException;

}
