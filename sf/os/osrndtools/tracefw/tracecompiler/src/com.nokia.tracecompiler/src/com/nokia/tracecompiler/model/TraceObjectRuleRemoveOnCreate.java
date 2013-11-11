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
* Extension that is removed from the object after it has been created
*
*/
package com.nokia.tracecompiler.model;

/**
 * Extension that is removed from the object after it has been created
 * 
 */
public interface TraceObjectRuleRemoveOnCreate extends TraceObjectRule {

	/**
	 * Returns true if this rule can be removed
	 * 
	 * @return true if remove, false otherwise
	 */
	public boolean canBeRemoved();

}
