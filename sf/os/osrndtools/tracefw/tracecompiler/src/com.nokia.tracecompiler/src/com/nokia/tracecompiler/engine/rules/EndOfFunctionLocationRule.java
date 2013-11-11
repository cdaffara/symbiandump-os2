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
* Location rule for end of function
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.engine.source.SourceLocationRule;

/**
 * Location rule for end of function
 * 
 */
public final class EndOfFunctionLocationRule extends RuleBase implements
		SourceLocationRule {

	/**
	 * Location rule offset. Should be large enough to cover a function
	 */
	private static final int OFFSET = 100000; // CodForChk_Dis_Magic

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.source.SourceLocationRule#getType()
	 */
	public int getLocationType() {
		return SourceLocationRule.CONTEXT_RELATIVE;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.source.SourceLocationRule#getOffset()
	 */
	public int getLocationOffset() {
		return OFFSET;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.source.SourceLocationRule#isRemovedAfterInsert()
	 */
	public boolean isRemovedAfterInsert() {
		return true;
	}

}
