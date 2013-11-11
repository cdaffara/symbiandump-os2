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
* Property associated with a trace object
*
*/
package com.nokia.tracecompiler.model;

/**
 * Property associated with a trace object
 * 
 */
public interface TraceObjectProperty {

	/**
	 * Gets the name of the property
	 * 
	 * @return the name
	 */
	public String getName();

	/**
	 * Gets the value of the property
	 * 
	 * @return the value
	 */
	public String getValue();

}
