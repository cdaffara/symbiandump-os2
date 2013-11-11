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
* Base class for TraceLocationList and PersistentLocationList
*
*/
package com.nokia.tracecompiler.engine;

import java.util.ArrayList;
import java.util.Iterator;

import com.nokia.tracecompiler.model.TraceModelExtension;
import com.nokia.tracecompiler.model.TraceObject;

/**
 * Base class for TraceLocationList and PersistentLocationList
 * 
 */
public class LocationListBase implements TraceModelExtension,
		Iterable<LocationProperties> {

	/**
	 * List of locations
	 */
	private ArrayList<LocationProperties> locations = new ArrayList<LocationProperties>();

	/**
	 * Owning trace object
	 */
	private TraceObject owner;

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelExtension#getOwner()
	 */
	public TraceObject getOwner() {
		return owner;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelExtension#setOwner(com.nokia.tracecompiler.model.TraceObject)
	 */
	public void setOwner(TraceObject owner) {
		this.owner = owner;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Iterable#iterator()
	 */
	public Iterator<LocationProperties> iterator() {
		return locations.iterator();
	}

	/**
	 * Gets the location count of this list
	 * 
	 * @return the count
	 */
	public final int getLocationCount() {
		return locations.size();
	}

	/**
	 * Checks if there are locations in this list
	 * 
	 * @return true if list has locations
	 */
	public final boolean hasLocations() {
		return !locations.isEmpty();
	}

	/**
	 * Adds a location to this list
	 * 
	 * @param location
	 *            the location to be added
	 */
	void add(LocationProperties location) {
		locations.add(location);
	}

	/**
	 * Removes a location from this list
	 * 
	 * @param location
	 *            the location to be removed
	 * @return true if removed
	 */
	boolean remove(LocationProperties location) {
		return locations.remove(location);
	}

	/**
	 * Checks if location exists
	 * 
	 * @param location
	 *            the location
	 * @return true if exists
	 */
	boolean contains(LocationProperties location) {
		return locations.contains(location);
	}

}
