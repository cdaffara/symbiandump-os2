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
* SourceContextManager interface implementation
*
*/
package com.nokia.tracecompiler.engine;

import com.nokia.tracecompiler.source.SourceContext;

/**
 * SourceContextManager interface implementation
 * 
 */
final class SourceContextManagerImpl implements SourceContextManager {

	/**
	 * Active context
	 */
	private SourceContext activeContext;

	/**
	 * Converting flag
	 */
	private boolean isConverting;

	/**
	 * Constructor
	 * 
	 * @param sourceEngine
	 *            the source engine
	 */
	SourceContextManagerImpl() {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.SourceContextManager#getContext()
	 */
	public SourceContext getContext() {
		return activeContext;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.SourceContextManager#
	 *      setContext(com.nokia.tracecompiler.source.SourceContext)
	 */
	public void setContext(SourceContext context) {
		activeContext = context;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.SourceContextManager#isConverting()
	 */
	public boolean isConverting() {
		return isConverting;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.SourceContextManager#setConverting(boolean)
	 */
	public void setConverting(boolean flag) {
		this.isConverting = flag;
	}
}