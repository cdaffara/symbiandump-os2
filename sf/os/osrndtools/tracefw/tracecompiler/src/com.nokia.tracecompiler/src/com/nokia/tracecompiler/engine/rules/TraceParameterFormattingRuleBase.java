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
* Base class for parameter formatting rules
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.engine.source.TraceParameterFormattingRule;

/**
 * Base class for parameter formatting rules
 * 
 */
class TraceParameterFormattingRuleBase extends RuleBase implements
		TraceParameterFormattingRule {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.source.TraceParameterFormattingRule#
	 *      isShownInSource()
	 */
	public boolean isShownInSource() {
		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.source.TraceParameterFormattingRule#
	 *      mapNameToSource(java.lang.String)
	 */
	public String mapNameToSource(String originalName) {
		return originalName;
	}

}
