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
* Implementation of formatting rule
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.engine.source.TraceFormattingRule;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.project.TraceProjectAPI;

/**
 * Base class for project API's. Instance of this is added to the model and
 * affect all traces which do not have their own formatter.
 * 
 */
public abstract class TraceFormatRuleBase extends RuleBase implements
		TraceFormattingRule, TraceProjectAPI {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.source.TraceFormattingRule#
	 *      mapNameToSource(com.nokia.tracecompiler.model.Trace)
	 */
	public String mapNameToSource(Trace trace) {
		return trace.getName();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.source.TraceFormattingRule#
	 *      mapParameterCountToSource(com.nokia.tracecompiler.model.Trace, int)
	 */
	public String mapParameterCountToSource(Trace trace, int count) {
		return String.valueOf(count);
	}

}