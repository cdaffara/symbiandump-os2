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
* Parameter iterator
*
*/
package com.nokia.tracecompiler.engine.header;

import java.io.IOException;
import java.util.Iterator;

import com.nokia.tracecompiler.model.TraceParameter;

/**
 * Parameter iterator
 * 
 */
public final class IteratorParameters extends TemplateIteratorEntry {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.header.TemplateIteratorEntry#iterate()
	 */
	@Override
	public void iterate(Object[] template) throws IOException {
		Iterator<TraceParameter> itr = writer.getCurrentTrace().getParameters();
		while (itr.hasNext()) {
			writer.startParameter(itr.next());
			writer.writeTemplate(template);
		}
	}

}
