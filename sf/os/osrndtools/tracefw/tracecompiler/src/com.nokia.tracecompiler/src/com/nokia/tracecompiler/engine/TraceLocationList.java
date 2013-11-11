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
* List of trace locations
*
*/
package com.nokia.tracecompiler.engine;

import java.util.ArrayList;

import com.nokia.tracecompiler.model.TraceCompilerException;

/**
 * List of trace locations.
 * 
 */
public class TraceLocationList extends LocationListBase {

	/**
	 * Location list listeners
	 */
	private ArrayList<TraceLocationListListener> listeners = new ArrayList<TraceLocationListListener>();

	/**
	 * List title, returned by getTitle
	 */
	private String listTitle;

	/**
	 * Constructor
	 */
	TraceLocationList() {
	}

	/**
	 * Constructor
	 * 
	 * @param listTitle
	 *            the title of this location list
	 */
	TraceLocationList(String listTitle) {
		this.listTitle = listTitle;
	}

	/**
	 * Adds a location to this list.
	 * 
	 * @param location
	 *            the location to be added
	 * @throws TraceCompilerException 
	 */
	void addLocation(TraceLocation location) throws TraceCompilerException {
		if (TraceCompilerEngineConfiguration.ASSERTIONS_ENABLED) {
			if (contains(location)) {
				//this can only happen if there is a programming error
				TraceCompilerEngineGlobals.getEvents().postCriticalAssertionFailed(
						"Duplicate trace location", //$NON-NLS-1$
						location.getConvertedName());
				throw new TraceCompilerException(null);
			}
		}
		location.setLocationList(this);
		add(location);
		fireLocationAdded(location);
		location.runValidityCheck();
	}

	/**
	 * Removes a location from this list.
	 * 
	 * @param location
	 *            the location to be removed
	 */
	void removeLocation(TraceLocation location) {
		if (remove(location)) {
			fireLocationRemoved(location);
			location.setLocationList(null);
		}
	}

	/**
	 * Adds a location list listener to this object
	 * 
	 * @param listener
	 *            the listener interface
	 */
	public void addLocationListListener(TraceLocationListListener listener) {
		listeners.add(listener);
	}

	/**
	 * Removes the location list listener
	 * 
	 * @param listener
	 *            the listener interface
	 */
	public void removeLocationListListener(TraceLocationListListener listener) {
		listeners.remove(listener);
	}

	/**
	 * Creates locationAdded event to location list listeners
	 * 
	 * @param location
	 *            the location that was added
	 */
	private void fireLocationAdded(TraceLocation location) {
		for (TraceLocationListListener l : listeners) {
			l.locationAdded(location);
		}
	}

	/**
	 * Creates locationRemoved event to location list listeners
	 * 
	 * @param location
	 *            the location that was added
	 */
	void fireLocationRemoved(TraceLocation location) {
		for (TraceLocationListListener l : listeners) {
			l.locationRemoved(location);
		}
	}

	/**
	 * Gets the list title or null
	 * 
	 * @return the list title
	 */
	public String getListTitle() {
		return listTitle;
	}

}
