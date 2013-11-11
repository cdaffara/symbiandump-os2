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
* Event listener interface
*
*/
package com.nokia.tracecompiler.engine;

import com.nokia.tracecompiler.model.TraceCompilerException;

/**
 * Event listener interface
 * 
 */
public interface TraceCompilerEngineEvents {

	/**
	 * Posts an error event to TraceCompiler
	 * 
	 * @param exception
	 *            the error data
	 */
	public void postError(TraceCompilerException exception);

	/**
	 * Posts an error event to TraceCompiler
	 * 
	 * @param message
	 *            the error message
	 * @param source
	 *            the error source
	 * @param postEvent
	 *            defines is error event also posted to trace event view
	 */
	public void postErrorMessage(String message, Object source, boolean postEvent);

	/**
	 * Posts a warning event to TraceCompiler
	 * 
	 * @param message
	 *            the warning message
	 * @param source
	 *            the warning source
	 */
	public void postWarningMessage(String message, Object source);

	/**
	 * Posts an info event to TraceCompiler
	 * 
	 * @param message
	 *            the info message
	 * @param source
	 *            the info source
	 */
	public void postInfoMessage(String message, Object source);

	/**
	 * Posts a critical assertion failed event
	 * 
	 * @param message
	 *            the message
	 * @param source
	 *            the source of the assertion
	 * @throws TraceCompilerException 
	 */
	public void postCriticalAssertionFailed(String message, Object source) throws TraceCompilerException;

	/**
	 * Posts an assertion failed event
	 * 
	 * @param message
	 *            the message
	 * @param source
	 *            the source of the assertion
	 */
	public void postAssertionFailed(String message, Object source);

	/**
	 * Gets the processing error
	 * 
	 * @return the error
	 */
	public boolean hasErrorHappened();


}
