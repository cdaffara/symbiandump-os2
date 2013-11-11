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
* Base class for document monitors
*
*/
package com.nokia.tracecompiler.utils;

import java.util.ArrayList;
import java.util.Iterator;

import com.nokia.tracecompiler.source.SourceDocumentFactory;
import com.nokia.tracecompiler.source.SourceDocumentInterface;
import com.nokia.tracecompiler.source.SourceDocumentMonitor;
import com.nokia.tracecompiler.source.SourceDocumentProcessor;

/**
 * Base class for document monitors.
 * 
 */
public class DocumentMonitorBase implements SourceDocumentMonitor {

	/**
	 * Document factory adapter
	 */
	private DocumentFactoryBase factory;

	/**
	 * List of sources
	 */
	private ArrayList<SourceDocumentInterface> sources = new ArrayList<SourceDocumentInterface>();

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceDocumentMonitor#getFactory()
	 */
	public SourceDocumentFactory getFactory() {
		if (factory == null) {
			factory = new DocumentFactoryBase();
		}
		return factory;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Iterable#iterator()
	 */
	public Iterator<SourceDocumentInterface> iterator() {
		return sources.iterator();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceDocumentMonitor#
	 *      startMonitor(com.nokia.tracecompiler.source.SourceDocumentProcessor)
	 */
	public void startMonitor(SourceDocumentProcessor processor) throws Exception {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceDocumentMonitor#stopMonitor()
	 */
	public void stopMonitor() {
		sources.clear();
	}

}