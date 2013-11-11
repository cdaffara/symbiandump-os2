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
* Integer-to-string mapping table for parameters
*
*/
package com.nokia.tracecompiler.model;

import java.util.ArrayList;
import java.util.Iterator;

/**
 * Integer-to-string mapping table for parameters
 * 
 */
public class TraceConstantTable extends TraceObject implements
		Iterable<TraceConstantTableEntry> {

	/**
	 * Table entries
	 */
	private ArrayList<TraceConstantTableEntry> entries = new ArrayList<TraceConstantTableEntry>();

	/**
	 * Table type
	 */
	private String type = TraceParameter.UDEC32;

	/**
	 * Parameter reference count
	 */
	private ArrayList<TraceParameter> parameterReferences = new ArrayList<TraceParameter>();

	/**
	 * Creates a new constant table
	 * 
	 * @param model
	 *            the trace model
	 */
	TraceConstantTable(TraceModel model) {
		setModel(model);
		model.addConstantTable(this);
	}

	/**
	 * Gets the type
	 * 
	 * @return the type
	 */
	public String getType() {
		return type;
	}

	/**
	 * Sets the constant type
	 * 
	 * @param type
	 *            the parameter type
	 */
	public void setType(String type) {
		this.type = type;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObject#setName(java.lang.String)
	 */
	@Override
	public void setName(String name) throws TraceCompilerException {
		super.setName(name);
		// Note: Currently name changes are disabled when a table is referenced
		// by parameters. If enabled at some point, a notification about this
		// needs to be sent to the parameters
	}

	/**
	 * Adds a constant table entry to this table. This is called from the
	 * constant table entry constructor, to this is not public. Generates a
	 * objectAdded event to model listeners
	 * 
	 * @see TraceModelListener#objectAdded(TraceObject, TraceObject)
	 * @param entry
	 *            the table entry
	 */
	void addEntry(TraceConstantTableEntry entry) {
		entries.add(entry);
		getModel().notifyObjectAdded(this, entry);
	}

	/**
	 * Removes the given constant table entry. Creates a objectRemoved event to
	 * the model listeners
	 * 
	 * @see TraceModelListener#objectRemoved(TraceObject, TraceObject)
	 * @param entry
	 *            the entry to be removed
	 * @throws TraceCompilerException 
	 */
	public void removeEntry(TraceConstantTableEntry entry) throws TraceCompilerException {
		if (entries.remove(entry)) {
			getModel().notifyObjectRemoved(this, entry);
			entry.reset();
		}
	}

	/**
	 * Gets the entry which has given ID
	 * 
	 * @param id
	 *            the id
	 * @return the entry or null
	 */
	public TraceConstantTableEntry findEntryByID(int id) {
		TraceConstantTableEntry retval = null;
		for (TraceConstantTableEntry entry : entries) {
			if (entry.getID() == id) {
				retval = entry;
				break;
			}
		}
		return retval;
	}

	/**
	 * Gets the entry which has the given name
	 * 
	 * @param name
	 *            the name
	 * @return the entry or null
	 */
	public TraceConstantTableEntry findEntryByName(String name) {
		TraceConstantTableEntry retval = null;
		for (TraceConstantTableEntry entry : entries) {
			if (entry.getName().equals(name)) {
				retval = entry;
				break;
			}
		}
		return retval;
	}

	/**
	 * Gets the constant table entries
	 * 
	 * @return the entries iterator
	 */
	public Iterator<TraceConstantTableEntry> getEntries() {
		return entries.iterator();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Iterable#iterator()
	 */
	public Iterator<TraceConstantTableEntry> iterator() {
		return entries.iterator();
	}

	/**
	 * Determines if there are any entries in this table
	 * 
	 * @return true if there are entries
	 */
	public boolean hasEntries() {
		return !entries.isEmpty();
	}

	/**
	 * Adds a parameter reference
	 * 
	 * @param parameter
	 *            the parameter to be added
	 */
	void addParameterReference(TraceParameter parameter) {
		parameterReferences.add(parameter);
	}

	/**
	 * Removes a parameter reference
	 * 
	 * @param parameter
	 *            the reference to be removed
	 */
	void removeParameterReference(TraceParameter parameter) {
		parameterReferences.remove(parameter);
	}

	/**
	 * Checks if there are parameter references
	 * 
	 * @return true if this table is referenced from parameters
	 */
	public boolean hasParameterReferences() {
		return !parameterReferences.isEmpty();
	}

	/**
	 * Gets the parameter references
	 * 
	 * @return the iterator of the references
	 */
	public Iterator<TraceParameter> getParameterReferences() {
		return parameterReferences.iterator();
	}

	/**
	 * Gets the largest constant ID + 1
	 * 
	 * @return the ID
	 */
	public int getNextEntryID() {
		int max = 0;
		for (TraceConstantTableEntry entry : entries) {
			int id = entry.getID();
			if (id > max) {
				max = id;
			}
		}
		return max + 1;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObject#reset()
	 */
	@Override
	void reset() {
		for (TraceConstantTableEntry entry : entries) {
			// Model listeners are not notified on reset, but if the
			// entry itself implements a delete notification interface,
			// it must be called to do appropriate cleanup
			notifyOnDelete(entry);
			entry.reset();
		}
		super.reset();
	}

}
