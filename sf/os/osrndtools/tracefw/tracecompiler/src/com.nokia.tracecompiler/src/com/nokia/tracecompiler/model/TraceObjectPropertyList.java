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
* List of properties associated to a trace object
*
*/
package com.nokia.tracecompiler.model;

/**
 * List of properties associated to a trace object
 * 
 */
public interface TraceObjectPropertyList extends TraceModelExtension,
		Iterable<TraceObjectProperty> {

	/**
	 * Gets a property by name
	 * 
	 * @param name
	 *            the property name
	 * @return the property
	 */
	public TraceObjectProperty getProperty(String name);

}
