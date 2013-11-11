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
* Maps trace locations into traces and vice versa
*
*/
package com.nokia.tracecompiler.engine;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

import com.nokia.tracecompiler.engine.source.SourceProperties;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModel;

/**
 * Maps trace locations into traces and vice versa.
 * 
 */
final class TraceLocationMap {

	/**
	 * List of unrelated traces
	 */
	private TraceLocationList unrelated = new TraceLocationList();

	/**
	 * Parser groups
	 */
	private HashMap<String, TraceLocationList> parserGroups = new HashMap<String, TraceLocationList>();

	/**
	 * The trace model
	 */
	private TraceModel model;

	/**
	 * Global list of locations, used for verification purposes with
	 * GLOBAL_LOCATION_ASSERTS configuration flag
	 */
	private ArrayList<TraceLocation> globalList;

	/**
	 * Creates a location mapper
	 * 
	 * @param model
	 *            the trace model
	 */
	public TraceLocationMap(TraceModel model) {
		if (TraceCompilerEngineConfiguration.GLOBAL_LOCATION_ASSERTS) {
			globalList = new ArrayList<TraceLocation>();
		}
		this.model = model;
		model.addModelListener(new LocationMapModelListener(this));
		model.addExtension(unrelated);
	}

	/**
	 * Adds the locations from the source file to the map
	 * 
	 * @param source
	 *            properties of the source to be added
	 * @throws TraceCompilerException 
	 */
	void addSource(SourceProperties source) throws TraceCompilerException {
		for (TraceLocation location : source) {
			if (TraceCompilerEngineConfiguration.GLOBAL_LOCATION_ASSERTS) {
				if (globalList.contains(location)) {
					TraceCompilerEngineGlobals.getEvents().postAssertionFailed(
							"Location already in global list", //$NON-NLS-1$
							location.getConvertedName());
				} else {
					globalList.add(location);
				}
			}
			// Generates locationAdded event via TraceLocationListListener
			addNewLocationToTrace(location);
		}
	}

	/**
	 * Adds a location to trace or to the unrelated list if a trace cannot be
	 * found.
	 * 
	 * @param location
	 *            the location to be added
	 * @throws TraceCompilerException 
	 */
	private void addNewLocationToTrace(TraceLocation location) throws TraceCompilerException {
		TraceLocationList list;
		Trace trace = model.findTraceByName(location.getOriginalName());
		if (trace != null) {
			list = trace.getExtension(TraceLocationList.class);
			if (list == null) {
				list = new TraceLocationList();
				trace.addExtension(list);
			}
		} else {
			String name = location.getParserRule().getLocationParser()
					.getLocationGroup();
			if (name == null) {
				list = unrelated;
			} else {
				list = parserGroups.get(name);
				if (list == null) {
					list = new TraceLocationList(name);
					model.addExtension(list);
					parserGroups.put(name, list);
				}
			}
		}
		list.addLocation(location);
	}

	/**
	 * Moves the locations from trace to unrelated list
	 * 
	 * @param trace
	 *            the trace
	 * @throws TraceCompilerException 
	 */
	void moveToUnrelated(Trace trace) throws TraceCompilerException {
		TraceLocationList list = trace.getExtension(TraceLocationList.class);
		if (list != null) {
			trace.removeExtension(list);
			for (LocationProperties loc : list) {
				unrelated.addLocation((TraceLocation) loc);
			}
		}
	}

	/**
	 * Moves locations from unrelated to the given trace
	 * 
	 * @param trace
	 *            the trace
	 * @throws TraceCompilerException 
	 */
	void moveFromUnrelated(Trace trace) throws TraceCompilerException {
		String name = trace.getName();
		TraceLocationList list = null;
		Iterator<LocationProperties> itr = unrelated.iterator();
		while (itr.hasNext()) {
			TraceLocation location = (TraceLocation) itr.next();
			if (name.equals(location.getOriginalName())) {
				list = trace.getExtension(TraceLocationList.class);
				if (list == null) {
					list = new TraceLocationList();
					trace.addExtension(list);
				}
				// NOTE: This must replicate the behavior of
				// TraceLocationList.removeLocation
				itr.remove();
				unrelated.fireLocationRemoved(location);
				list.addLocation(location);
			}
		}
	}

}
