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
* Plug-in formatter / parser management
*
*/
package com.nokia.tracecompiler.engine.rules;

import java.util.ArrayList;
import java.util.Iterator;

import com.nokia.tracecompiler.engine.plugin.TraceAPIPluginManager;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.model.TraceObject;
import com.nokia.tracecompiler.plugin.TraceAPIFormatter;
import com.nokia.tracecompiler.plugin.TraceAPIParser;
import com.nokia.tracecompiler.project.TraceProjectAPI;
import com.nokia.tracecompiler.project.TraceProjectAPIList;

/**
 * Plug-in formatter / parser management
 * 
 */
final class RulesEnginePluginManager implements TraceAPIPluginManager,
		TraceProjectAPIList {

	/**
	 * Rules engine
	 */
	private RulesEngine engine;

	/**
	 * Trace model
	 */
	private TraceModel model;

	/**
	 * List of API's
	 */
	private ArrayList<TraceProjectAPI> apis = new ArrayList<TraceProjectAPI>();

	/**
	 * Constructor
	 * 
	 * @param engine
	 *            the plug-in engine
	 */
	RulesEnginePluginManager(RulesEngine engine) {
		this.engine = engine;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.plugin.TraceAPIPluginManager#
	 *      addFormatters(com.nokia.tracecompiler.plugin.TraceAPIFormatter[])
	 */
	public void addFormatters(TraceAPIFormatter[] formatters) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.plugin.TraceAPIPluginManager#
	 *      addParsers(com.nokia.tracecompiler.plugin.TraceAPIParser[])
	 */
	public void addParsers(TraceAPIParser[] parsers) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.plugin.TraceAPIPluginManager#createDefaultAPI()
	 */
	public void createDefaultAPI() {
		engine.setDefaultTraceAPI();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelExtension#getOwner()
	 */
	public TraceObject getOwner() {
		return model;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelExtension#
	 *      setOwner(com.nokia.tracecompiler.model.TraceObject)
	 */
	public void setOwner(TraceObject owner) {
		model = (TraceModel) owner;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceProjectAPIList#getAPIs()
	 */
	public Iterator<TraceProjectAPI> getAPIs() {
		return apis.iterator();
	}

	/**
	 * Adds an API to the list
	 * 
	 * @param api
	 *            the API
	 */
	void addAPI(TraceProjectAPI api) {
		apis.add(api);
	}

}
