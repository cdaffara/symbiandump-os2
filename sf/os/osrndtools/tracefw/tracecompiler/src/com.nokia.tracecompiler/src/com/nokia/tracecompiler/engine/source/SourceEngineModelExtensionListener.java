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
* Trace model extension listener implementation for SourceEngine
*
*/
package com.nokia.tracecompiler.engine.source;

import com.nokia.tracecompiler.model.TraceModelExtension;
import com.nokia.tracecompiler.model.TraceModelExtensionListener;
import com.nokia.tracecompiler.model.TraceObject;

/**
 * Trace model extension listener implementation for SourceEngine
 * 
 */
final class SourceEngineModelExtensionListener implements
		TraceModelExtensionListener {

	/**
	 * Constructor
	 * 
	 * @param engine
	 *            the source engine
	 */
	SourceEngineModelExtensionListener(SourceEngine engine) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 *      extensionAdded(com.nokia.tracecompiler.model.TraceObject,
	 *      com.nokia.tracecompiler.model.TraceModelExtension)
	 */
	public void extensionAdded(TraceObject object, TraceModelExtension extension) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelListener#
	 *      extensionRemoved(com.nokia.tracecompiler.model.TraceObject,
	 *      com.nokia.tracecompiler.model.TraceModelExtension)
	 */
	public void extensionRemoved(TraceObject object,
			TraceModelExtension extension) {
	}

}