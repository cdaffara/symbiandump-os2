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
* Wrapper for Class and name
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.model.TraceModelPersistentExtension;

/**
 * Wrapper for persistent extension class and name
 * 
 */
class ClassNameWrapper {

	/**
	 * Constructor
	 * 
	 * @param name
	 *            the name
	 * @param clasz
	 *            the class
	 */
	ClassNameWrapper(String name,
			Class<? extends TraceModelPersistentExtension> clasz) {
		this.name = name;
		this.clasz = clasz;
	}

	/**
	 * Name for the class
	 */
	String name;

	/**
	 * The class
	 */
	Class<? extends TraceModelPersistentExtension> clasz;

}