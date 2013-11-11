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
* Trace rule for automatically adding function parameters to a trace
*
*/
package com.nokia.tracecompiler.engine.rules;

import java.util.ArrayList;

import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorMessages;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.StringErrorParameters;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.engine.header.ComplexHeaderRule;
import com.nokia.tracecompiler.engine.source.TraceParameterFormattingRule;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceConstantTable;
import com.nokia.tracecompiler.model.TraceObjectRuleCreateObject;
import com.nokia.tracecompiler.model.TraceObjectRuleRemoveOnCreate;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.source.SourceContext;
import com.nokia.tracecompiler.source.SourceParameter;
import com.nokia.tracecompiler.source.SourceParserException;
import com.nokia.tracecompiler.source.SourceUtils;
import com.nokia.tracecompiler.source.TypeMapping;

/**
 * Trace rule for automatically adding function parameters to a trace
 * 
 */
public final class AutoAddFunctionParametersRule extends RuleBase implements
		TraceObjectRuleCreateObject, TraceObjectRuleRemoveOnCreate {

	/**
	 * Warning about value-to-pointer conversion
	 */
	private static final String VALUE_TO_POINTER_WARNING = Messages
			.getString("AutoAddFunctionParametersRule.UnrecognizedTypeWarning"); //$NON-NLS-1$

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.nokia.tracecompiler.model.TraceObjectRuleCreateObject#createObject()
	 */
	public void createObject() throws TraceCompilerException {
		SourceContext context = TraceCompilerEngineGlobals.getSourceContextManager()
				.getContext();
		Trace trace = (Trace) getOwner();
		if (context != null) {
			boolean valid = false;
			ArrayList<SourceParameter> list = tokenizeParameters(context);

			trace.getModel().startProcessing();
			try {
				createParameters(trace, list);
			} finally {
				trace.getModel().processingComplete();
			}
			// At least one parameter must be parsed from source except for function entry extension
			valid = trace.getParameterCount() > 0 || trace.getExtension(EntryTraceRule.class) != null;

			if (!valid) {
				String msg = TraceCompilerEngineErrorMessages.getErrorMessage(
						TraceCompilerErrorCode.CANNOT_PARSE_FUNCTION_PARAMETERS,
						null);
				TraceCompilerEngineGlobals.getEvents().postWarningMessage(msg, trace);
				// Removes the complex rule -> Function is not generated and
				// source does not compile
				trace.removeExtensions(ComplexHeaderRule.class);
			}
		} else {
			String msg = TraceCompilerEngineErrorMessages.getErrorMessage(
					TraceCompilerErrorCode.NO_CONTEXT_FOR_LOCATION, null);
			TraceCompilerEngineGlobals.getEvents().postErrorMessage(msg, null, true);
			// Removes the complex rule -> Function is not generated and
			// source does not compile
			trace.removeExtensions(ComplexHeaderRule.class);
		}
	}

	/**
	 * Creates the parameters to the trace
	 * 
	 * @param owner
	 *            the trace
	 * @param list
	 *            the parameter list parsed from source
	 * @throws TraceCompilerException 
	 */
	private void createParameters(Trace owner, ArrayList<SourceParameter> list) throws TraceCompilerException {

		// If any of the parameters is "...", clear the whole parameter list
		for (SourceParameter param : list) {
			String type = param.getType();
			if (type != null
					&& type.equals(SourceConstants.VARIABLE_ARG_LIST_INDICATOR)) {
				list.clear();
				
				
				String msg = TraceCompilerEngineErrorMessages.getErrorMessage(
						TraceCompilerErrorCode.VAR_ARG_LIST_PARAMETER_FOUND,
						null);
				TraceCompilerEngineGlobals.getEvents().postWarningMessage(msg, owner);
				break;
			}
		}

		StringBuilder str = new StringBuilder();
		for (SourceParameter param : list) {
			TraceParameter parameter = createParameter(owner, param);
			if (parameter != null) {
				str.append(SourceConstants.PARAMETER_SEPARATOR);
				TraceParameterFormattingRule rule = parameter
						.getExtension(TraceParameterFormattingRule.class);
				if (rule != null) {
					str.append(rule.mapNameToSource(param.getName()));
				} else {
					str.append(param.getName());
				}
			}
			// The location will be referenced by event handlers if
			// they need it. Otherwise it will be removed from the
			// source
			param.getSourceLocation().dereference();
		}
		// The header extension is stored in case of complex function entry
		// trace
		EntryTraceRule entryRule = owner.getExtension(EntryTraceRule.class);
		ComplexHeaderRuleImpl complex = owner
				.getExtension(ComplexHeaderRuleImpl.class);
		if (entryRule != null && complex != null) {
			complex.setTraceIDDefineExtension(str.toString());
		}
	}

	/**
	 * Processes the parameters of given source context
	 * 
	 * @param context
	 *            the context
	 * @return list of parameters
	 * @throws TraceCompilerException 
	 */
	private ArrayList<SourceParameter> tokenizeParameters(SourceContext context) throws TraceCompilerException {
		ArrayList<SourceParameter> list = new ArrayList<SourceParameter>();
		try {
			context.parseParameters(list);
		} catch (SourceParserException e) {
			TraceCompilerException exception = new TraceCompilerException(
					TraceCompilerErrorCode.UNEXPECTED_EXCEPTION, e);
			TraceCompilerEngineGlobals.getEvents().postError(
					exception);
			list = null;
		}
		return list;
	}

	/**
	 * Creates a TraceParameter based on parameter parsed from source
	 * 
	 * @param owner
	 *            the owning trace object
	 * @param param
	 *            the parameter found from source
	 * @return the parameter
	 */
	private TraceParameter createParameter(Trace owner, SourceParameter param) {
		TraceConstantTable foundTable = RuleUtils.findConstantTableByType(owner
				.getModel(), param);
		TraceParameter parameter = null;
		if (foundTable == null) {
			TypeMapping type = SourceUtils.mapSymbianTypeToParameterType(param);
			if (type != null) {
				if (!type.isVoid()) {
					parameter = createParameter(owner, param, type);
				}
			} else {
				StringErrorParameters params = new StringErrorParameters();
				params.string = param.getType();
				TraceCompilerException exception = new TraceCompilerException(
						TraceCompilerErrorCode.INVALID_PARAMETER_TYPE,
						params, param.getSourceLocation());
				TraceCompilerEngineGlobals.getEvents().postError(
						exception);
			}
		} else {
			parameter = createParameter(owner, param, foundTable);
		}
		return parameter;
	}

	/**
	 * Processes a non-void parameter type that was not associated with a
	 * constant table
	 * 
	 * @param owner
	 *            the owner for the parameter
	 * @param param
	 *            the parameter found from source
	 * @param type
	 *            the parameter type as parsed by source package
	 * @return the parameter
	 */
	private TraceParameter createParameter(Trace owner, SourceParameter param,
			TypeMapping type) {
		String name = param.getName();
		TraceParameter retval = null;
		if (name == null || name.length() == 0) {
			String s = TraceCompilerEngineErrorMessages.getErrorMessage(
					TraceCompilerErrorCode.EMPTY_PARAMETER_NAME, null);
			TraceCompilerEngineGlobals.getEvents().postWarningMessage(s,
					param.getSourceLocation());
		} else {
			try {
				retval = RuleUtils.createParameterFromType(owner, name, type);
				if (retval != null && type.valueToPointer) {
					// Posts a warning about value-to-pointer conversion
					TraceCompilerEngineGlobals.getEvents()
							.postWarningMessage(VALUE_TO_POINTER_WARNING,
									param.getSourceLocation());
				}
			} catch (TraceCompilerException e) {
				// Changes the source of the error to the location that failed
				TraceCompilerException exception = new TraceCompilerException(e.getErrorCode(), e
						.getErrorParameters(), param
						.getSourceLocation());
				TraceCompilerEngineGlobals.getEvents().postError(
						exception);
			}
		}
		return retval;
	}

	/**
	 * Processes a non-void parameter type that was associated with a constant
	 * table
	 * 
	 * @param owner
	 *            the owner for the parameter
	 * @param param
	 *            the parameter found from source
	 * @param table
	 *            the constant table the parameter was associated to
	 * @return the parameter
	 */
	private TraceParameter createParameter(Trace owner, SourceParameter param,
			TraceConstantTable table) {
		String name = param.getName();
		TraceParameter retval = null;
		if (name == null || name.length() == 0) {
			String s = TraceCompilerEngineErrorMessages.getErrorMessage(
					TraceCompilerErrorCode.EMPTY_PARAMETER_NAME, null);
			TraceCompilerEngineGlobals.getEvents().postWarningMessage(s,
					param.getSourceLocation());
		} else {
			try {
				retval = RuleUtils.createParameterFromConstantTable(owner,
						name, table);
			} catch (TraceCompilerException e) {
				// Changes the source of the error to the location that failed
				TraceCompilerException exception = new TraceCompilerException(e.getErrorCode(), e
						.getErrorParameters(), param
						.getSourceLocation());
				TraceCompilerEngineGlobals.getEvents().postError(
						exception);
			}
		}
		return retval;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * com.nokia.tracecompiler.model.TraceObjectRuleRemoveOnCreate#canBeRemoved()
	 */
	public boolean canBeRemoved() {
		// This is not copied to exit trace -> Can be removed
		return true;
	}

}
