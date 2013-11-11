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
* Listener interface for trace model reset notifications
*
*/
package com.nokia.tracecompiler.model;

/**
 * Listener interface for trace model reset notifications
 * 
 */
public interface TraceModelResetListener {

	/**
	 * Notification that is fired before the model is reset.
	 */
	public void modelResetting();

	/**
	 * Notification that is fired after the model is reset.
	 */
	public void modelReset();

	/**
	 * Validity changed notification
	 * 
	 * @param valid
	 *            the validity changed flag
	 * @throws TraceCompilerException 
	 */
	public void modelValid(boolean valid) throws TraceCompilerException;

}
