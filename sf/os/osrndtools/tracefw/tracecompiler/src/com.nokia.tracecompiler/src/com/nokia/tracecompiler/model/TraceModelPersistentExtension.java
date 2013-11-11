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
* Interface implemented by trace model extensions that need to store themselves 
* into persistent storage along with other model elements
*
*/
package com.nokia.tracecompiler.model;

/**
 * Interface implemented by trace model extensions that need to store themselves
 * into persistent storage along with other model elements. The storage format
 * is left to the implementation. When loading, the trace provider calls
 * TraceObjectFactory.createExtension with the storage name. The object factory
 * returns an instance of this interface and the provider calls setData to
 * initialize this.
 * 
 */
public interface TraceModelPersistentExtension extends TraceModelExtension {

	/**
	 * Gets the name used when storing this extension
	 * 
	 * @return the storage name
	 */
	public String getStorageName();

	/**
	 * Gets the data of this extension
	 * 
	 * @return the extension data
	 */
	public String getData();

	/**
	 * Sets the data of this extension
	 * 
	 * @param data
	 *            the data
	 * @return true if extension should be added to model, false if not
	 */
	public boolean setData(String data);

}
