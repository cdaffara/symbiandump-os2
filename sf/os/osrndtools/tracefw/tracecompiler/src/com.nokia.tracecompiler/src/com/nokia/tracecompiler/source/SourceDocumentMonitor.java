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
* Monitor for source files
*
*/
package com.nokia.tracecompiler.source;

/**
 * Monitor for source files
 * 
 */
public interface SourceDocumentMonitor extends
		Iterable<SourceDocumentInterface> {

	/**
	 * Gets a document factory, which is shared between all documents created
	 * into this monitor
	 * 
	 * @return the factory
	 */
	public SourceDocumentFactory getFactory();

	/**
	 * Starts the monitor
	 * 
	 * @param processor
	 *            document processor callback
	 * @throws Exception 
	 */
	public void startMonitor(SourceDocumentProcessor processor) throws Exception;

	/**
	 * Stops the monitor
	 */
	public void stopMonitor();

}
