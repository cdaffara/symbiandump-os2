/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Trace rule for automatically adding function return value to exit trace
*
*/
package com.nokia.tracecompiler.engine.rules;

import java.util.ArrayList;

import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorMessages;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.engine.utils.TraceMultiplierRule;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceConstantTable;
import com.nokia.tracecompiler.model.TraceObjectRuleCreateObject;
import com.nokia.tracecompiler.model.TraceObjectRuleRemoveOnCreate;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.source.SourceContext;
import com.nokia.tracecompiler.source.SourceParserException;
import com.nokia.tracecompiler.source.SourceReturn;
import com.nokia.tracecompiler.source.SourceUtils;
import com.nokia.tracecompiler.source.TypeMapping;

/**
 * Trace rule for automatically adding function return value to exit trace
 * 
 */
public final class AutoAddReturnParameterRule extends RuleBase implements
		TraceObjectRuleCreateObject, TraceObjectRuleRemoveOnCreate {

	/**
	 * Parameter name
	 */
	static final String PARAMETER_NAME = "retval"; //$NON-NLS-1$

	/**
	 * The number of handled return statements
	 */
	private static int numberOfHandledReturnStatements = 0;

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectRuleCreateObject#
	 *      createObject(com.nokia.tracecompiler.model.TraceObject)
	 */
	public void createObject() throws TraceCompilerException {
		// If owner has a multiplier (entry trace), the trace is not added to it
		Trace owner = (Trace) getOwner();
		if (owner.getExtension(TraceMultiplierRule.class) == null) {
			SourceContext context = TraceCompilerEngineGlobals
					.getSourceContextManager().getContext();
			if (!context.isVoid()) {
				TraceConstantTable table = RuleUtils.findConstantTableByType(
						owner.getModel(), context);
				if (table != null) {
					createParameter(owner, table);
				} else {
					createParameter(owner, context);
				}
			}
		}
	}

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

	/**
	 * Creates a return parameter that was not associated with a constant table
	 * 
	 * @param owner
	 *            the owner for the parameter
	 * @param context
	 *            the context specifying the parameter type 
	 */
	private void createParameter(Trace owner, SourceContext context) {
		TypeMapping type = SourceUtils.mapSymbianTypeToParameterType(context);
		String return_value_name = PARAMETER_NAME;
		ArrayList<SourceReturn> returnList = new ArrayList<SourceReturn>();

		// Find out return value name
		try {
			context.parseReturnValues(returnList);
			if (numberOfHandledReturnStatements <= (returnList.size() - 1)) {
				return_value_name = returnList.get(
						numberOfHandledReturnStatements).getReturnStatement();
			}

			numberOfHandledReturnStatements++;
		} catch (SourceParserException e) {
			String msg = Messages
					.getString("RuleUtils.FailedToParseReturnValues"); //$NON-NLS-1$
			String cname = context.getClassName();
			String source;
			if (cname != null) {
				source = cname + "::" + context.getFunctionName(); //$NON-NLS-1$;
			} else {
				source = context.getFunctionName();
			}
			TraceCompilerEngineGlobals.getEvents().postErrorMessage(msg, source, true);
		}

		try {
			if (!type.needsCasting) {
				// If an extension header is generated, the parameter needs to
				// be cast, since the return statement may contain anything.
				type.needsCasting = type.type != TraceParameter.HEX32
						&& type.type != TraceParameter.SDEC32
						&& type.type != TraceParameter.UDEC32;
			}
			RuleUtils.createParameterFromType(owner, return_value_name, type);

		} catch (TraceCompilerException e) {

			if (e.getErrorCode() == TraceCompilerErrorCode.INVALID_PARAMETER_NAME) {
				String msg = TraceCompilerEngineErrorMessages
						.getErrorMessage(
								TraceCompilerErrorCode.INVALID_PARAMETER_NAME_IN_RETURN_VALUE,
								null);

				TraceCompilerEngineGlobals.getEvents().postWarningMessage(msg, owner);
			} else {
				TraceCompilerException exception = new TraceCompilerException(e.getErrorCode(), e
						.getErrorParameters(), null);
				TraceCompilerEngineGlobals.getEvents().postError(
						exception);
			}
		}
	}

	/**
	 * Creates a return parameter that was associated with a constant table
	 * 
	 * @param owner
	 *            the owner for the parameter
	 * @param table
	 *            the constant table the parameter was associated to
	 */
	private void createParameter(Trace owner, TraceConstantTable table) {
		try {
			RuleUtils.createParameterFromConstantTable(owner, PARAMETER_NAME,
					table);
		} catch (TraceCompilerException e) {
			TraceCompilerException exception = new TraceCompilerException(e.getErrorCode(), e
					.getErrorParameters(), null);
			TraceCompilerEngineGlobals.getEvents().postError(
					exception);
		}
	}

	/**
	 * Reset static variables
	 */
	static void resetNumberOfHandledReturnStatements() {
		numberOfHandledReturnStatements = 0;
	}

	/**
	 * Increase number of handled return statements
	 */
	static void increaseNumberOfHandledReturnStatements() {
		numberOfHandledReturnStatements++;
	}

}
