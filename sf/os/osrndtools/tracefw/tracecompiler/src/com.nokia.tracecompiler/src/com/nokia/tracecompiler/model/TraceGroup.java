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
* Represents a logical collection of traces somehow related to each other
*
*/
package com.nokia.tracecompiler.model;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;

/**
 * Represents a logical collection of traces somehow related to each other. Each
 * trace group specifies a prefix that is attached to all trace lines made
 * within that group.
 * 
 */
public class TraceGroup extends TraceObject implements Iterable<Trace> {

	/**
	 * List of traces, sorted by ID
	 */
	private ArrayList<Trace> tracesByID = new ArrayList<Trace>();

	/**
	 * List of traces, sorted by name
	 */
	private ArrayList<Trace> tracesByName = new ArrayList<Trace>();

	/**
	 * Creates a new trace group and associates it with given trace model.
	 * 
	 * @param model
	 *            the trace model
	 */
	TraceGroup(TraceModel model) {
		setModel(model);
		model.addGroup(this);
	}

	/**
	 * Adds a trace to this group. Creates objectAdded event to model listeners
	 * 
	 * @see TraceModelListener#objectAdded(TraceObject, TraceObject)
	 * @param trace
	 *            the trace to be added
	 */
	void addTrace(Trace trace) {
		// Sorted when ID is set to trace
		tracesByID.add(trace);
		// Sorted when name is set to trace
		tracesByName.add(trace);
		getModel().notifyObjectAdded(this, trace);
	}

	/**
	 * Removes a trace from this group. Creates objectRemoved event to model
	 * listeners
	 * 
	 * @see TraceModelListener#objectRemoved(TraceObject, TraceObject)
	 * @param trace
	 *            the trace to be removed
	 * @throws TraceCompilerException 
	 */
	public void removeTrace(Trace trace) throws TraceCompilerException {
		int index = Collections.binarySearch(tracesByID, trace,
				TraceObjectUtils.traceObjectIDComparator);
		if (index >= 0) {
			tracesByID.remove(index);
			index = Collections.binarySearch(tracesByName, trace,
					TraceObjectUtils.traceObjectNameComparator);
			tracesByName.remove(index);
			getModel().notifyObjectRemoved(this, trace);
			trace.reset();
		}
	}

	/**
	 * Determines if this group has traces.
	 * 
	 * @return true if the group contains traces
	 */
	public boolean hasTraces() {
		return !tracesByID.isEmpty();
	}

	/**
	 * Gets the number of traces in this group.
	 * 
	 * @return the number of traces
	 */
	public int getTraceCount() {
		return tracesByID.size();
	}

	/**
	 * Returns an iterator over the traces within this group.
	 * 
	 * @return the iterator
	 */
	public Iterator<Trace> getTraces() {
		return tracesByID.iterator();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Iterable#iterator()
	 */
	public Iterator<Trace> iterator() {
		return tracesByID.iterator();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObject#reset()
	 */
	@Override
	void reset() {
		// Reset removes constant table references
		for (Trace trace : tracesByID) {
			// Model listeners are not notified on reset, but if the
			// trace itself implements a delete notification interface,
			// it must be called to do appropriate cleanup
			notifyOnDelete(trace);
			trace.reset();
		}
		tracesByID.clear();
		tracesByName.clear();
		super.reset();
	}

	/**
	 * Returns highest trace ID + 1. Can be used to create an unique ID for a
	 * new trace.
	 * 
	 * @return the next trace ID
	 */
	public int getNextTraceID() {
		int ret;
		if (tracesByID.size() > 0) {
			ret = tracesByID.get(tracesByID.size() - 1).getID() + 1;
		} else {
			ret = 1;
		}
		return ret;
	}

	/**
	 * Gets the trace which has given ID
	 * 
	 * @param id
	 *            the trace ID
	 * @return the trace or null
	 */
	public Trace findTraceByID(int id) {
		int index = Collections.binarySearch(tracesByID, id,
				TraceObjectUtils.traceToIDComparator);
		Trace retval;
		if (index >= 0) {
			retval = tracesByID.get(index);
		} else {
			retval = null;
		}
		return retval;
	}

	/**
	 * Called by the model when a trace property is updated
	 * 
	 * @param source
	 *            the trace that was changed
	 * @param property
	 *            the property that was changed
	 */
	void tracePropertyUpdated(TraceObject source, int property) {
		if (property == TraceModelListener.NAME) {
			Collections.sort(tracesByName,
					TraceObjectUtils.traceObjectNameComparator);
		} else if (property == TraceModelListener.ID) {
			traceIDsUpdated();
		}
	}

	/**
	 * Sorts the trace array based on new ID configuration
	 */
	void traceIDsUpdated() {
		Collections.sort(tracesByID, TraceObjectUtils.traceObjectIDComparator);
	}
}
