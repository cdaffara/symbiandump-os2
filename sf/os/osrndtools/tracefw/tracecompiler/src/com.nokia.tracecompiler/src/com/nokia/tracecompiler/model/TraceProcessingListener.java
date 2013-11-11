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
* Multi-update optimization
*
*/
package com.nokia.tracecompiler.model;

/**
 * Callback interface which should be notified when performing multiple updates
 * to the trace model. The implementation of TraceModelListener can also
 * implement this interface to avoid performing unnecessary updates.
 * 
 */
public interface TraceProcessingListener {

	/**
	 * Called when a sequence of trace model updates is about to start
	 */
	public void processingStarted();

	/**
	 * Called when a sequence of trace model updates has finished
	 * 
	 * @param changed
	 *            true if the model was changed while processing, false if not
	 */
	public void processingComplete(boolean changed);

}
