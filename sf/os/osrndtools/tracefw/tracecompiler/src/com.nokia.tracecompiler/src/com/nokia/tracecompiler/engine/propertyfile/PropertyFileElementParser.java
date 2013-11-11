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
* Property file element parser interface
*
*/
package com.nokia.tracecompiler.engine.propertyfile;

import org.w3c.dom.Element;

import com.nokia.tracecompiler.model.TraceCompilerException;

/**
 * Property file element parser interface
 * 
 */
interface PropertyFileElementParser {

	/**
	 * Parses the given element
	 * 
	 * @param owner
	 *            the owning object
	 * @param element
	 *            the element to be parsed
	 * @throws TraceCompilerException
	 *             if parser fails
	 */
	void parse(Object owner, Element element) throws TraceCompilerException;
}
