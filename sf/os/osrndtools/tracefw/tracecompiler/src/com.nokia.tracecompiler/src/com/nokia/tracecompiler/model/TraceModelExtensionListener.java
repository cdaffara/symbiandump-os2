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
* Listener interface for trace model extension notifications
*
*/
package com.nokia.tracecompiler.model;

/**
 * Listener interface for trace model extension notifications
 * 
 */
public interface TraceModelExtensionListener {

	/**
	 * Notifies that an extension was added to the trace object
	 * 
	 * @param object
	 *            the object where the extension was added
	 * @param extension
	 *            the new extension object
	 */
	public void extensionAdded(TraceObject object, TraceModelExtension extension);

	/**
	 * Notifies that an extension was removed from the trace object
	 * 
	 * @param object
	 *            the object from where the extension was removed
	 * @param extension
	 *            the removed extension object
	 */
	public void extensionRemoved(TraceObject object,
			TraceModelExtension extension);

}
