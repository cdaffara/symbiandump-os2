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
* Interface to change the currently active source context
*
*/
package com.nokia.tracecompiler.engine;

import com.nokia.tracecompiler.source.SourceContext;

/**
 * Interface to the currently active source context
 * 
 */
public interface SourceContextManager {

	/**
	 * Gets the active source context
	 * 
	 * @return the context
	 */
	public SourceContext getContext();

	/**
	 * Sets the active source context
	 * 
	 * @param context
	 *            the context
	 */
	public void setContext(SourceContext context);

	/**
	 * Checks if the auto-converter is running
	 * 
	 * @return true if converter is running, false otherwise
	 */
	public boolean isConverting();

	/**
	 * Sets the auto-converter flag
	 * 
	 * @param flag
	 *            the new flag value
	 */
	public void setConverting(boolean flag);

}
