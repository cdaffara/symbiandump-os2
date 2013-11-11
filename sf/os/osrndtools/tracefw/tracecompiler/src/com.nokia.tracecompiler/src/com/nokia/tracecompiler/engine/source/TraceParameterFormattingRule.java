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
* Rule interface for parameters
*
*/
package com.nokia.tracecompiler.engine.source;

import com.nokia.tracecompiler.model.TraceObjectRule;

/**
 * Rule interface for parameters. Each parameter may provide replacement text
 * for the %EXT% formatting.
 * 
 */
public interface TraceParameterFormattingRule extends TraceObjectRule {

	/**
	 * Determines if this object should be shown in source
	 * 
	 * @return true if shown in source, false otherwise
	 */
	public boolean isShownInSource();

	/**
	 * Maps parameter name to name shown in source. Note that the original name
	 * passed to this function is not necessarily the name returned by
	 * getOwner().getName()
	 * 
	 * @param originalName
	 *            the original name to be inserted to source
	 * @return the name shown in source
	 */
	public String mapNameToSource(String originalName);

}
