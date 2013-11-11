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
* Source engine manages source documents that are opened to Eclipse UI
*
*/
package com.nokia.tracecompiler.engine.source;

import java.util.ArrayList;
import java.util.Iterator;

import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.source.SourceDocumentInterface;
import com.nokia.tracecompiler.source.SourceDocumentMonitor;
import com.nokia.tracecompiler.source.SourceDocumentProcessor;
import com.nokia.tracecompiler.utils.DocumentFactory;

/**
 * Source engine manages source documents that are opened to Eclipse UI.
 * 
 */
public class SourceEngine implements SourceDocumentProcessor,
		Iterable<SourceProperties> {

	/**
	 * Document monitor
	 */
	private SourceDocumentMonitor documentMonitor;

	/**
	 * Trace model listener implementation
	 */
	private SourceEngineModelListener modelListener = new SourceEngineModelListener(
			this);

	/**
	 * Trace model extension listener
	 */
	private SourceEngineModelExtensionListener extensionListener = new SourceEngineModelExtensionListener(
			this);

	/**
	 * The callback interfaces are notified about source file changes
	 */
	private ArrayList<SourceListener> listeners = new ArrayList<SourceListener>();

	/**
	 * Running flag
	 */
	private boolean running;

	/**
	 * Trace model
	 */
	private TraceModel model;

	/**
	 * Source list
	 */
	private ArrayList<SourceProperties> tempList = new ArrayList<SourceProperties>();

	/**
	 * Read-only files
	 */
	private String[] READ_ONLY = { ".h" //$NON-NLS-1$
	};

	/**
	 * Non-source file list
	 */
	private ArrayList<String> nonSourceFiles = new ArrayList<String>();

	/**
	 * Constructor
	 * 
	 * @param model
	 *            the trace model
	 */
	public SourceEngine(TraceModel model) {
		this.model = model;
	}

	/**
	 * Starts this engine. Does nothing if already running
	 * @throws Exception 
	 */
	public void start() throws Exception {
		if (!running) {
			documentMonitor = DocumentFactory.getDocumentMonitor();
			documentMonitor.startMonitor(this);
			running = true;
			model.addModelListener(modelListener);
			model.addExtensionListener(extensionListener);
		}
	}

	/**
	 * Shuts down the source engine. Does nothing if already stopped
	 */
	public void shutdown() {
		if (running) {
			documentMonitor.stopMonitor();
			documentMonitor = null;
			running = false;
			model.removeModelListener(modelListener);
			model.removeExtensionListener(extensionListener);
		}
	}

	/**
	 * Adds source listener callback interface
	 * 
	 * @param listener
	 *            the new listener
	 */
	public void addSourceListener(SourceListener listener) {
		listeners.add(listener);
	}

	/**
	 * Removes a source listener
	 * 
	 * @param listener
	 *            the listener to be removed
	 */
	public void removeSourceListener(SourceListener listener) {
		listeners.remove(listener);
	}

	/**
	 * Gets the sources
	 * 
	 * @return the sources
	 */
	public Iterator<SourceProperties> getSources() {
		tempList.clear();
		for (SourceDocumentInterface doc : documentMonitor) {
			tempList.add((SourceProperties) doc.getOwner());
		}
		return tempList.iterator();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Iterable#iterator()
	 */
	public Iterator<SourceProperties> iterator() {
		return getSources();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceDocumentProcessor#
	 *      sourceOpened(com.nokia.tracecompiler.source.SourceDocumentInterface)
	 */
	public void sourceOpened(SourceDocumentInterface source) throws TraceCompilerException {
		SourceProperties properties = new SourceProperties(model,
				documentMonitor.getFactory(), source);
		// Headers are marked read-only
		for (String s : READ_ONLY) {
			String fileName = properties.getFileName();
			if (fileName != null && fileName.endsWith(s)) {
				properties.setReadOnly(true);
				break;
			}
		}
		properties.sourceOpened();
		source.setOwner(properties);
		for (SourceListener l : listeners) {
			l.sourceOpened(properties);
		}
	}

	/**
	 * Adds a non-source file to this list.
	 * 
	 * @param filePath
	 *            the non-source file path to added
	 */
	public void addNonSourceFile(String filePath) {
		nonSourceFiles.add(filePath);
	}

	/**
	 * Removes a non-source file from this list
	 * 
	 * @param filePath
	 *            the non-source file path to be removed
	 * @return true if removed
	 */
	public boolean removeNonSourceFile(String filePath) {
		boolean retVal = nonSourceFiles.remove(filePath);
		return retVal;
	}

	/**
	 * Gets list of non-source files
	 * 
	 * @return the list of non-source file paths
	 */
	public ArrayList<String> getNonSourceFiles() {
		return nonSourceFiles;
	}

}
