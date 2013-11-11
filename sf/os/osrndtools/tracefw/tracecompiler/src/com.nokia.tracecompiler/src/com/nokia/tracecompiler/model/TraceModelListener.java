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
* Listener interface for trace model change notifications
*
*/
package com.nokia.tracecompiler.model;

/**
 * Listener interface for trace model change notifications
 * 
 */
public interface TraceModelListener {

	/**
	 * Object name has been updated
	 */
	int NAME = 1;

	/**
	 * Object ID has been updated
	 */
	int ID = 2;

	/**
	 * Trace text has been updated
	 */
	int TRACE = 3;

	/**
	 * Parameter type has been updated
	 */
	int TYPE = 4;

	/**
	 * Notification which is fired when a trace object has been added
	 * 
	 * @param owner
	 *            the owner of the object
	 * @param object
	 *            the object
	 */
	public void objectAdded(TraceObject owner, TraceObject object);

	/**
	 * Notification that is fired a trace object is removed from the model
	 * 
	 * @param owner
	 *            the owner of the object
	 * @param object
	 *            the removed object
	 * @throws TraceCompilerException 
	 */
	public void objectRemoved(TraceObject owner, TraceObject object) throws TraceCompilerException;

	/**
	 * Notifies that the one of the properties of an object have been updated
	 * 
	 * @param object
	 *            the object which was updated
	 * @param property
	 *            the property that was updated
	 * @throws TraceCompilerException 
	 */
	public void propertyUpdated(TraceObject object, int property) throws TraceCompilerException;

	/**
	 * Notifies that object creation is complete
	 * 
	 * @param object
	 *            the object
	 * @throws TraceCompilerException 
	 */
	public void objectCreationComplete(TraceObject object) throws TraceCompilerException;
}
