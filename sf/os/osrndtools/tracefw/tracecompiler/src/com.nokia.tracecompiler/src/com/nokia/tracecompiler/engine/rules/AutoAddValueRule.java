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
* Creation rule for value parameter
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceObjectRule;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.source.SourceContext;

/**
 * Creation rule for "value" parameter
 * 
 */
public final class AutoAddValueRule extends AutoAddParameterRuleBase {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.rules.AutoAddParameterRuleBase#createObject()
	 */
	@Override
	public void createObject() throws TraceCompilerException {
		SourceContext context = TraceCompilerEngineGlobals.getSourceContextManager()
				.getContext();
		if (context != null) {
			super.createObject();
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.rules.AutoAddParameterRuleBase#getName()
	 */
	@Override
	protected String getName() {
		return ValueParameterTemplate.PARAMETER_NAME;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.rules.AutoAddParameterRuleBase#getRule()
	 */
	@Override
	protected TraceObjectRule getRule() {
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.rules.AutoAddParameterRuleBase#getType()
	 */
	@Override
	protected String getType() {
		return TraceParameter.SDEC32;
	}

}
