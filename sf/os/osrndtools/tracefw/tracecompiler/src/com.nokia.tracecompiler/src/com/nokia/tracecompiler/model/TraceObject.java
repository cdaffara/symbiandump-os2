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
* Base class for all trace model objects
*
*/
package com.nokia.tracecompiler.model;

import java.util.ArrayList;
import java.util.Iterator;

/**
 * Base class for all trace model objects
 * 
 */
public abstract class TraceObject {

	/**
	 * The extensions can be used by trace providers to associate extra data to
	 * the object.
	 */
	private ArrayList<TraceModelExtension> extensions = new ArrayList<TraceModelExtension>();

	/**
	 * Object ID.
	 */
	private int id;

	/**
	 * Object name.
	 */
	private String name = ""; //$NON-NLS-1$

	/**
	 * The trace model.
	 */
	private TraceModel model;

	/**
	 * Complete flag
	 */
	private boolean complete;

	/**
	 * Associates this object to a trace model.
	 * 
	 * @param model
	 *            the model where this object belongs to
	 */
	void setModel(TraceModel model) {
		this.model = model;
	}

	/**
	 * Returns the trace model this object is associated to.
	 * 
	 * @return the trace model
	 */
	public TraceModel getModel() {
		return model;
	}

	/**
	 * Sets the ID of this object. Generates propertyUpdated event to model
	 * listeners if the ID changes
	 * 
	 * @see TraceModelListener#propertyUpdated(TraceObject, int)
	 * @param id
	 *            the new ID
	 * @throws TraceCompilerException 
	 */
	public void setID(int id) throws TraceCompilerException {
		if (this.id != id) {
			this.id = id;
			model.notifyPropertyUpdated(this, TraceModelListener.ID);
		}
	}

	/**
	 * Sets the ID of this object but does not generate any notifications
	 * 
	 * @param id
	 *            the new ID
	 */
	void internalSetID(int id) {
		this.id = id;
	}

	/**
	 * Returns the ID of this object.
	 * 
	 * @return the ID
	 */
	public int getID() {
		return id;
	}

	/**
	 * Sets the name of this object. Generates propertyUpdated event to model
	 * listeners if the name changes
	 * 
	 * @see TraceModelListener#propertyUpdated(TraceObject, int)
	 * @param name
	 *            the new name
	 * @throws TraceCompilerException 
	 */
	public void setName(String name) throws TraceCompilerException {
		if (name == null) {
			name = ""; //$NON-NLS-1$
		}
		if (!name.equals(this.name)) {
			this.name = name;
			model.notifyPropertyUpdated(this, TraceModelListener.NAME);
		}
	}

	/**
	 * Returns the name of this object. This never returns null
	 * 
	 * @return the name
	 */
	public String getName() {
		return name;
	}

	/**
	 * Adds an extension to this object. Generates extensionAdded event to model
	 * listeners
	 * 
	 * @param extension
	 *            the new extension
	 */
	public void addExtension(TraceModelExtension extension) {
		if (extension == null) {
			throw new NullPointerException();
		}
		extensions.add(extension);
		extension.setOwner(this);
		model.notifyExtensionAdded(this, extension);
	}

	/**
	 * Removes an extension from this object. Generates extensionRemoved event
	 * to model listeners if the extension was actually removed
	 * 
	 * @param extension
	 *            the extension to be removed
	 */
	public void removeExtension(TraceModelExtension extension) {
		if (extensions.remove(extension)) {
			extension.setOwner(null);
			model.notifyExtensionRemoved(this, extension);
		}
	}

	/**
	 * Removes all extensions which are of given type
	 * 
	 * @param <T>
	 *            the type of the extension
	 * @param extClass
	 *            the extension type
	 */
	public <T extends TraceModelExtension> void removeExtensions(
			Class<T> extClass) {
		Iterator<TraceModelExtension> itr = extensions.iterator();
		while (itr.hasNext()) {
			TraceModelExtension ext = itr.next();
			if (extClass.isAssignableFrom(ext.getClass())) {
				itr.remove();
				ext.setOwner(null);
				model.notifyExtensionRemoved(this, ext);
			}
		}
	}

	/**
	 * Returns the first extension which is of given type. The extensions are
	 * stored in a list in the order they have been added. This returns the
	 * first instance found from the list.
	 * 
	 * @param <T>
	 *            the type of the extension
	 * @param extClass
	 *            the extension type
	 * @return the extension or null if not found
	 */
	@SuppressWarnings("unchecked")
	public <T extends TraceModelExtension> T getExtension(Class<T> extClass) {
		T ret = null;
		for (TraceModelExtension ext : extensions) {
			if (extClass.isAssignableFrom(ext.getClass())) {
				ret = (T) ext;
				break;
			}
		}
		return ret;
	}

	/**
	 * Returns the extensions which are of given type.
	 * 
	 * @param <T>
	 *            the type of the extension
	 * @param extClass
	 *            the extension type
	 * @return iterator over the list of extension
	 */
	@SuppressWarnings("unchecked")
	public <T extends TraceModelExtension> Iterator<T> getExtensions(
			Class<T> extClass) {
		ArrayList<T> list = new ArrayList<T>();
		for (TraceModelExtension ext : extensions) {
			if (extClass.isAssignableFrom(ext.getClass())) {
				list.add((T) ext);
			}
		}
		return list.iterator();
	}

	/**
	 * Resets the ID and name
	 */
	void reset() {
		id = 0;
		name = ""; //$NON-NLS-1$
	}

	/**
	 * Sets the complete flag and fires complete event
	 * @throws TraceCompilerException 
	 */
	void setComplete() throws TraceCompilerException {
		complete = true;
		model.notifyObjectCreationComplete(this);
	}

	/**
	 * Calls OnDelete rules if the object owns them
	 * 
	 * @param object
	 *            the object that was removed
	 */
	protected void notifyOnDelete(TraceObject object) {
		Iterator<TraceObjectRuleOnDelete> rules = object
				.getExtensions(TraceObjectRuleOnDelete.class);
		while (rules.hasNext()) {
			rules.next().objectDeleted();
		}
	}

	/**
	 * Gets the complete flag
	 * 
	 * @return the flag
	 */
	public boolean isComplete() {
		return complete;
	}

}
