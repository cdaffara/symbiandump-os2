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
* Rule which specifies that a trace cannot be represented by the normal trace API macros
*
*/
package com.nokia.tracecompiler.engine.header;

import com.nokia.tracecompiler.model.TraceObjectRule;

/**
 * Rule which specifies that a trace cannot be represented by the normal trace
 * API macros
 * 
 */
public interface ComplexHeaderRule extends TraceObjectRule {

	/**
	 * Returns true if an ext-function needs to be written for this trace
	 * 
	 * @return function flag
	 */
	public boolean needsFunction();

	/**
	 * Gets an extension to be added to the trace ID define
	 * 
	 * @return the extension
	 */
	public String getTraceIDDefineExtension();

}
