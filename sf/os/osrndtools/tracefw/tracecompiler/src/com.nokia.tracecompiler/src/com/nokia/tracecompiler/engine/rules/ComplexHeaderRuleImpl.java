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
* Implementation of ComplexHeaderRule
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.engine.header.ComplexHeaderRule;

/**
 * Implementation of ComplexHeaderRule
 * 
 */
public final class ComplexHeaderRuleImpl extends RuleBase implements
		ComplexHeaderRule {

	/**
	 * Trace ID define extension
	 */
	private String traceIDDefineExtension;

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.header.ComplexHeaderRule#needsFunction()
	 */
	public boolean needsFunction() {
		return true;
	}

	/**
	 * Sets the data to be added to the trace ID define statement after the ID
	 * 
	 * @param traceIDDefineExtension
	 *            the extension
	 */
	void setTraceIDDefineExtension(String traceIDDefineExtension) {
		this.traceIDDefineExtension = traceIDDefineExtension;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.header.ComplexHeaderRule#getTraceIDDefineExtension()
	 */
	public String getTraceIDDefineExtension() {
		return traceIDDefineExtension;
	}
}