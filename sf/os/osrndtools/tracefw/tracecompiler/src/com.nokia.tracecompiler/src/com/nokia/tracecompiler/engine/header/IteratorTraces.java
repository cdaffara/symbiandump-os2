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
* Iterator for all traces
*
*/
package com.nokia.tracecompiler.engine.header;

import java.io.IOException;

import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceGroup;
import com.nokia.tracecompiler.model.TraceModel;

/**
 * Iterator for all traces
 * 
 */
public final class IteratorTraces extends TemplateIteratorEntry {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.header.TemplateIteratorEntry#
	 *      iterate(java.lang.Object[])
	 */
	@Override
	void iterate(Object[] template) throws IOException {
		TraceModel model = writer.getHeader().getOwner().getModel();
		for (TraceGroup group : model) {
			for (Trace trace : group) {
				if (writer.startTrace(trace)) {
					writer.writeTemplate(template);
				}
			}
		}
	}

}
