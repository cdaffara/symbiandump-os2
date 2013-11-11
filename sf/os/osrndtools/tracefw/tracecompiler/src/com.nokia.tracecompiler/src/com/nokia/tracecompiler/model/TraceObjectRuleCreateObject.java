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
* Rule for automatic object creation
*
*/
package com.nokia.tracecompiler.model;

/**
 * Rule for automatic object creation
 * 
 */
public interface TraceObjectRuleCreateObject extends TraceObjectRule {

	/**
	 * Called to create a new object into the owner of this rule
	 * @throws TraceCompilerException 
	 */
	public void createObject() throws TraceCompilerException;

}
