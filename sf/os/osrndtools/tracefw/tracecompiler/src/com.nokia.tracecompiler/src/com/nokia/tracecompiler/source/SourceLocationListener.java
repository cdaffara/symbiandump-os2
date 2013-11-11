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
* Listener for source locations
*
*/
package com.nokia.tracecompiler.source;

import com.nokia.tracecompiler.engine.TraceLocation;

/**
 * Listener for source locations
 * 
 */
public interface SourceLocationListener {

	/**
	 * Notification that location validity has changed
	 * 
	 * @param location
	 *            the location that changed
	 */
	public void locationValidityChanged(TraceLocation location);	
}
