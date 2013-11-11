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
* Interface that is notified about changes in source documents
*
*/
package com.nokia.tracecompiler.engine.source;

import com.nokia.tracecompiler.model.TraceCompilerException;


/**
 * Interface that is notified about changes in source documents managed by
 * {@link SourceEngine}
 * 
 */
public interface SourceListener {

	/**
	 * Event that is fired when a source is opened
	 * 
	 * @param properties
	 *            the source properties
	 * @throws TraceCompilerException 
	 */
	public void sourceOpened(SourceProperties properties) throws TraceCompilerException;

}
