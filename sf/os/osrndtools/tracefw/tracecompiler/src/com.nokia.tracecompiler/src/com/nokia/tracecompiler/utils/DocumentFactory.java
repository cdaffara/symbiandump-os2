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
* Factory class to create document interfaces
*
*/
package com.nokia.tracecompiler.utils;

import com.nokia.tracecompiler.source.SourceDocumentFactory;
import com.nokia.tracecompiler.source.SourceDocumentMonitor;

/**
 * Factory class to create document interfaces
 * 
 */
public final class DocumentFactory {

	/**
	 * Document monitor
	 */
	private static SourceDocumentMonitor monitor;


	/**
	 * Registers a document framework to be used by the engine
	 * 
	 * @param monitor
	 *            the document monitor
	 * @param factoryClass
	 *            the document factory class
	 */
	public static void registerDocumentFramework(SourceDocumentMonitor monitor,
			Class<? extends SourceDocumentFactory> factoryClass) {
		DocumentFactory.monitor = monitor;
	}

	/**
	 * Gets the document monitor
	 * 
	 * @return the monitor
	 */
	public static final SourceDocumentMonitor getDocumentMonitor() {
		return monitor;
	}
}
