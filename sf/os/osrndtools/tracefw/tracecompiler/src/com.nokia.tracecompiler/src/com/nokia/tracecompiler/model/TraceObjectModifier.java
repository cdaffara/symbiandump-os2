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
* Modifier properties is returned by one of the TraceObjectUtils.modify methods
*
*/
package com.nokia.tracecompiler.model;

/**
 * Modifier properties is returned by one of the TraceObjectUtils.modify
 * methods.
 * 
 */
public interface TraceObjectModifier {

	/**
	 * Gets the data
	 * 
	 * @return the data
	 */
	public String getData();

	/**
	 * Checks if the data was changed during the modify call
	 * 
	 * @return true if data was changed
	 */
	public boolean hasChanged();

}