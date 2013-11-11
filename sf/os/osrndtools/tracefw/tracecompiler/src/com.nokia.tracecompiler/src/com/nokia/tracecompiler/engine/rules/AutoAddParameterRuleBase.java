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
* Base class for the auto-add parameter rules
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.utils.TraceMultiplierRule;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceModelExtension;
import com.nokia.tracecompiler.model.TraceObjectRule;
import com.nokia.tracecompiler.model.TraceObjectRuleCreateObject;
import com.nokia.tracecompiler.model.TraceObjectRuleRemoveOnCreate;
import com.nokia.tracecompiler.model.TraceObjectUtils;

/**
 * Base class for the auto-add parameter rules
 * 
 */
abstract class AutoAddParameterRuleBase extends RuleBase implements
		TraceObjectRuleCreateObject, TraceObjectRuleRemoveOnCreate {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectRuleCreateObject#createObject()
	 */
	public void createObject() throws TraceCompilerException {
		TraceObjectRule rule = getRule();
		TraceModelExtension[] extensions = null;
		if (rule != null) {
			extensions = new TraceModelExtension[] { rule };
		}
		Trace owner = (Trace) getOwner();
		int id = owner.getNextParameterID();
		String name = TraceObjectUtils.modifyDuplicateParameterName(owner,
				getName()).getData();
		String type = getType();
		try {
			owner.getModel().getVerifier().checkTraceParameterProperties(owner,
					null, id, name, type);
			owner.getModel().getFactory().createTraceParameter(owner, id, name,
					type, extensions);
		} catch (TraceCompilerException e) {
			TraceCompilerEngineGlobals.getEvents().postError(e);
				throw e;
		}
	}

	/**
	 * Gets the name for the new parameter
	 * 
	 * @return the parameter name
	 */
	protected abstract String getName();

	/**
	 * Gets the new parameter type
	 * 
	 * @return the type
	 */
	protected abstract String getType();

	/**
	 * Gets the rule for the parameter
	 * 
	 * @return the rule
	 */
	protected abstract TraceObjectRule getRule();

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectRuleRemoveOnCreate#canBeRemoved()
	 */
	public boolean canBeRemoved() {
		// If the owner has a multiplier, this needs to be moved to it using the
		// CopyAndRemoveExtensionRule
		return getOwner().getExtension(TraceMultiplierRule.class) == null;
	}

}
