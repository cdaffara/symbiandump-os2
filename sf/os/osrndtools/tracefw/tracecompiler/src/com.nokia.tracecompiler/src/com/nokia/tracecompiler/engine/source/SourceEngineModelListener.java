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
* Trace model listener implementation for SourceEngine
*
*/
package com.nokia.tracecompiler.engine.source;

import com.nokia.tracecompiler.model.TraceModelListener;
import com.nokia.tracecompiler.model.TraceObject;

/**
 * Trace model listener implementation for SourceEngine
 * 
 */
final class SourceEngineModelListener implements TraceModelListener {

	/**
	 * Constructor
	 * 
	 * @param engine
	 *            the source engine
	 */
	SourceEngineModelListener(SourceEngine engine) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 *      objectAdded(com.nokia.tracecompiler.model.TraceObject,
	 *      com.nokia.tracecompiler.model.TraceObject)
	 */
	public void objectAdded(TraceObject owner, TraceObject object) {
		// Update is called from TraceCompiler engine
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 *      objectRemoved(com.nokia.tracecompiler.model.TraceObject,
	 *      com.nokia.tracecompiler.model.TraceObject)
	 */
	public void objectRemoved(TraceObject owner, TraceObject object) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 *      objectCreationComplete(com.nokia.tracecompiler.model.TraceObject)
	 */
	public void objectCreationComplete(TraceObject object) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 *      propertyUpdated(com.nokia.tracecompiler.model.TraceObject, int)
	 */
	public void propertyUpdated(TraceObject object, int property) {
	}
}