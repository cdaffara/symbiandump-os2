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
* Represents the mapping of a trace ID into actual trace string
*
*/
package com.nokia.tracecompiler.model;

import java.util.ArrayList;
import java.util.Iterator;

/**
 * Represents the mapping of a trace ID into actual trace string. When a trace
 * is decoded, the trace ID found from the binary trace file is converted to the
 * text stored in this trace.
 * 
 */
public class Trace extends TraceObject implements Iterable<TraceParameter> {

	/**
	 * The trace string
	 */
	private String trace = ""; //$NON-NLS-1$

	/**
	 * List of TraceParameter objects.
	 */
	private ArrayList<TraceParameter> parameters = new ArrayList<TraceParameter>();

	/**
	 * The trace group this trace belongs to
	 */
	private TraceGroup group;

	/**
	 * Creates a new trace and associates it with given TraceGroup
	 * 
	 * @param group
	 *            the trace group
	 */
	Trace(TraceGroup group) {
		setModel(group.getModel());
		this.group = group;
		group.addTrace(this);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObject#reset()
	 */
	@Override
	void reset() {
		super.reset();
		for (TraceParameter parameter : parameters) {
			// Model listeners are not notified on reset, but if the
			// parameter itself implements a delete notification interface,
			// it must be called to do appropriate cleanup
			notifyOnDelete(parameter);
			parameter.reset();
		}
		parameters.clear();
	}

	/**
	 * Sets the trace string. Generates propertyUpdated event to model listeners
	 * if the string changes
	 * 
	 * @see TraceModelListener#propertyUpdated(TraceObject, int)
	 * @param trace
	 *            the trace string
	 * @throws TraceCompilerException 
	 */
	public void setTrace(String trace) throws TraceCompilerException {
		if (trace == null) {
			trace = ""; //$NON-NLS-1$
		}
		if (!trace.equals(this.trace)) {
			this.trace = trace;
			getModel().notifyPropertyUpdated(this, TraceModelListener.TRACE);
		}
	}

	/**
	 * Returns the trace string. This never returns null
	 * 
	 * @return the trace string
	 */
	public String getTrace() {
		return trace;
	}

	/**
	 * Returns the group to which this trace belongs
	 * 
	 * @return trace group
	 */
	public TraceGroup getGroup() {
		return group;
	}

	/**
	 * Adds a new parameter to the end of parameter list. Generates objectAdded
	 * event to model listeners
	 * 
	 * @see TraceModelListener#objectAdded(TraceObject, TraceObject)
	 * @param parameter
	 *            the new parameter
	 */
	public void addParameter(TraceParameter parameter) {
		parameters.add(parameter);
		getModel().notifyObjectAdded(this, parameter);
	}

	/**
	 * Inserts a new parameter to the given location. Generates objectAdded
	 * event to model listeners
	 * 
	 * @see TraceModelListener#objectAdded(TraceObject, TraceObject)
	 * @param index
	 *            the index for new parameter
	 * @param parameter
	 *            the new parameter
	 */
	void insertParameter(int index, TraceParameter parameter) {
		parameters.add(index, parameter);
		getModel().notifyObjectAdded(this, parameter);
	}

	/**
	 * Removes a parameter from this trace object. Generates a objectRemoved
	 * event to model listeners if the parameter is actually removed
	 * 
	 * @see TraceModelListener#objectRemoved(TraceObject, TraceObject)
	 * @param parameter
	 *            the parameter to be removed
	 * @throws TraceCompilerException 
	 */
	public void removeParameter(TraceParameter parameter) throws TraceCompilerException {
		if (parameters.remove(parameter)) {
			getModel().notifyObjectRemoved(this, parameter);
			parameter.reset();
		}
	}

	/**
	 * Removes parameter at given index
	 * 
	 * @param index
	 *            the index of the parameter
	 * @throws TraceCompilerException 
	 */
	public void removeParameterAt(int index) throws TraceCompilerException {
		if (index >= 0 && index < parameters.size()) {
			TraceParameter parameter = parameters.get(index);
			parameters.remove(index);
			getModel().notifyObjectRemoved(this, parameter);
			parameter.reset();
		}
	}

	/**
	 * Determines if this object has parameters.
	 * 
	 * @return true if this object has parameters
	 */
	public boolean hasParameters() {
		return !parameters.isEmpty();
	}

	/**
	 * Gets the number of parameters
	 * 
	 * @return the number of parameters
	 */
	public int getParameterCount() {
		return parameters.size();
	}

	/**
	 * Returns highest parameter ID + 1. Can be used to create an unique ID for
	 * a new parameter.
	 * 
	 * @return the next parameter ID
	 */
	public int getNextParameterID() {
		int max = 0;
		for (TraceParameter parameter : parameters) {
			int id = parameter.getID();
			if (id > max) {
				max = id;
			}
		}
		return max + 1;
	}

	/**
	 * Gets the parameter at given index
	 * 
	 * @param index
	 *            the parameter index
	 * @return the parameter at the index
	 */
	public TraceParameter getParameter(int index) {
		return parameters.get(index);
	}

	/**
	 * Returns the parameters of this object
	 * 
	 * @return iterator over the parameters
	 */
	public Iterator<TraceParameter> getParameters() {
		return parameters.iterator();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Iterable#iterator()
	 */
	public Iterator<TraceParameter> iterator() {
		return parameters.iterator();
	}

	/**
	 * Gets a parameter by ID
	 * 
	 * @param id
	 *            the parameter ID
	 * @return the parameter or null
	 */
	public TraceParameter findParameterByID(int id) {
		TraceParameter retval = null;
		for (TraceParameter parameter : parameters) {
			if (parameter.getID() == id) {
				retval = parameter;
				break;
			}
		}
		return retval;
	}

	/**
	 * Gets a parameter by name
	 * 
	 * @param name
	 *            the name of a parameter
	 * @return the parameter or null
	 */
	public TraceParameter findParameterByName(String name) {
		TraceParameter retval = null;
		for (TraceParameter parameter : parameters) {
			if (parameter.getName().equals(name)) {
				retval = parameter;
				break;
			}
		}
		return retval;
	}
}
