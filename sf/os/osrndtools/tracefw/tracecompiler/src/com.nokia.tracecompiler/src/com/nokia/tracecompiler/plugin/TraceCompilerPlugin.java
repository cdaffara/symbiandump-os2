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
* TraceCompiler plug-in interface
*
*/
package com.nokia.tracecompiler.plugin;

import com.nokia.tracecompiler.model.TraceModel;

/**
 * TraceCompiler plug-in interface
 * 
 */
public interface TraceCompilerPlugin {

	/**
	 * Notification that the trace project has been opened. Provides the model
	 * to this plug-in. The model is not changed until closeTraceProject and
	 * thus no null-checks are needed.
	 * 
	 * @param model
	 *            the trace model
	 */
	public void traceProjectOpened(TraceModel model);

	/**
	 * Notification that the trace project has been closed
	 */
	public void traceProjectClosed();
}
