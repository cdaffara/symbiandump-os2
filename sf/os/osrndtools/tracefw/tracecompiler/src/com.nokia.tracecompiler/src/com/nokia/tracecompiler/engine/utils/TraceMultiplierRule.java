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
* Rule which can be used to create copies of new traces
*
*/
package com.nokia.tracecompiler.engine.utils;

import java.util.Iterator;

import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceObjectRule;

/**
 * Rule which can be used to create copies of new traces
 * 
 */
public interface TraceMultiplierRule extends TraceObjectRule {

	/**
	 * Gets copies of the given trace which is about to be inserted into given
	 * context. The new trace is added first and the the copies will be inserted
	 * after it in the order they are received from the iterator.
	 * 
	 * @param trace
	 *            the new trace
	 * @return a list of copies
	 */
	public Iterator<Trace> createCopies(Trace trace);

}
