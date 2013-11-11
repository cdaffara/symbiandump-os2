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
* Rules for mapping trace model entries into source code
*
*/
package com.nokia.tracecompiler.engine.source;

import com.nokia.tracecompiler.model.TraceObjectRule;

/**
 * Rules for relocating traces in source code
 * 
 */
public interface SourceLocationRule extends TraceObjectRule {

	/**
	 * Rule type for context-relative location
	 */
	int CONTEXT_RELATIVE = 1;

	/**
	 * Rule type for absolute location
	 */
	int ABSOLUTE = 2;

	/**
	 * Gets the rule type
	 * 
	 * @return rule type
	 */
	public int getLocationType();

	/**
	 * Gets the offset where the trace is inserted
	 * 
	 * @return the offset
	 */
	public int getLocationOffset();

	/**
	 * Flag which determines whether this locator should be removed from the
	 * trace after the trace has been inserted to source. If this is left to the
	 * trace, the trace is relocated every time it is inserted.
	 * <p>
	 * Note that if this returns false, the extension should also be persistent
	 * so that the relocations work over TraceCompiler restarts.
	 * 
	 * @return true if this should be removed after insertion
	 */
	public boolean isRemovedAfterInsert();

}
