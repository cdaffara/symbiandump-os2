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
* Creates trace objects and provides configurable rules for trace object creation
*
*/
package com.nokia.tracecompiler.model;

/**
 * Allows registration of rules into the trace object factory
 * 
 */
public interface TraceObjectRuleFactory {

	/**
	 * Called before the rules of a new object are processed by object factory
	 * 
	 * @param object
	 *            new object
	 */
	public void preProcessNewRules(TraceObject object);

	/**
	 * Called after the rules of a new object are processed by object factory
	 * 
	 * @param object
	 *            new object
	 */
	public void postProcessNewRules(TraceObject object);

	/**
	 * Creates an extension based on extension name
	 * 
	 * @param object
	 *            the target object
	 * @param name
	 *            the name of the extension
	 * @return the new extension
	 */
	public TraceModelPersistentExtension createExtension(TraceObject object,
			String name);

}
