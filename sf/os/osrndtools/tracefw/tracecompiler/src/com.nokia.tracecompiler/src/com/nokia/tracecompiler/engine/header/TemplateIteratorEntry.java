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
* Iterator base class
*
*/
package com.nokia.tracecompiler.engine.header;

import java.io.IOException;

/**
 * Iterator base class
 * 
 */
abstract class TemplateIteratorEntry {

	/**
	 * Header writer
	 */
	protected TraceHeaderWriter writer;

	/**
	 * Sets the writer
	 * 
	 * @param writer
	 *            the writer to set
	 */
	final void setWriter(TraceHeaderWriter writer) {
		this.writer = writer;
	}

	/**
	 * Runs the iterator on given template
	 * 
	 * @param template
	 *            the template
	 * @throws IOException
	 *             if operation fails
	 */
	abstract void iterate(Object[] template) throws IOException;

}
