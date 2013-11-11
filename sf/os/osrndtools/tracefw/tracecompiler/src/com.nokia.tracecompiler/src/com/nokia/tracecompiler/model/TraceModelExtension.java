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
* Interface implemented by trace model extensions
*
*/
package com.nokia.tracecompiler.model;

/**
 * Interface implemented by trace model extensions
 * 
 */
public interface TraceModelExtension {

	/**
	 * Sets the owner of this extension. Called when extension is added to a
	 * trace object and called with null when removed from trace object
	 * 
	 * @param owner
	 *            the owning object
	 */
	public void setOwner(TraceObject owner);

	/**
	 * Gets the owner of this extension
	 * 
	 * @return the owner
	 */
	public TraceObject getOwner();

}
