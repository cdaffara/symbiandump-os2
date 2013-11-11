/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Rule for state traces
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.engine.utils.TraceUtils;
import com.nokia.tracecompiler.source.SourceContext;

/**
 * Rule for state traces
 * 
 */
public class StateTraceRule extends AutomaticTraceTextRule {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.rules.AutomaticTraceTextRule#
	 *      formatTrace(com.nokia.tracecompiler.source.SourceContext)
	 */
	@Override
	public String formatTrace(SourceContext context) {
		return TraceUtils.formatTrace(RuleUtils.TEXT_FORMAT_BASE, context
				.getClassName(), context.getFunctionName());
	}
}
