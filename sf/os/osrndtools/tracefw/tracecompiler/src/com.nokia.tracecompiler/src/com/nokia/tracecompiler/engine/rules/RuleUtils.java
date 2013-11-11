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
 * Utility functions for rules package
 *
 */
package com.nokia.tracecompiler.engine.rules;

import java.util.Iterator;
import java.util.List;

import com.nokia.tracecompiler.engine.TraceCompilerEngineConfiguration;
import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceConstantTable;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.model.TraceModelExtension;
import com.nokia.tracecompiler.model.TraceObjectUtils;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.source.ParsedType;
import com.nokia.tracecompiler.source.SourceContext;
import com.nokia.tracecompiler.source.SourceParserException;
import com.nokia.tracecompiler.source.SourceReturn;
import com.nokia.tracecompiler.source.TypeMapping;

/**
 * Utility functions for rules package
 * 
 */
public final class RuleUtils {

	/**
	 * Entry-exit template
	 */
	public static final int TYPE_ENTRY_EXIT = 0; // CodForChk_Dis_Magic

	/**
	 * Start-stop event template
	 */
	public static final int TYPE_PERF_EVENT = 1; // CodForChk_Dis_Magic

	/**
	 * State trace template
	 */
	public static final int TYPE_STATE_TRACE = 2; // CodForChk_Dis_Magic

	/**
	 * Trace name format base
	 */
	static final String NAME_FORMAT_BASE = "{$CN}_{$FN}"; //$NON-NLS-1$

	/**
	 * Trace text format base
	 */
	static final String TEXT_FORMAT_BASE = "{$cn}::{$fn}"; //$NON-NLS-1$

	/**
	 * Template titles
	 */
	private static final String[] ENTRY_TEMPLATE_TITLES = {
			Messages.getString("RuleUtils.FunctionEntryTitle"), //$NON-NLS-1$
			Messages.getString("RuleUtils.PerformanceEventTitle"), //$NON-NLS-1$
			Messages.getString("RuleUtils.StateTraceTitle"), //$NON-NLS-1$
	};

	/**
	 * Entry name prefixes
	 */
	public static final String[] ENTRY_NAME_PREFIXES = { "", //$NON-NLS-1$
			"EVENT_", //$NON-NLS-1$
			"STATE_" //$NON-NLS-1$
	};

	/**
	 * Entry name suffixes
	 */
	public static final String[] ENTRY_NAME_SUFFIXES = { "_ENTRY", //$NON-NLS-1$
			"_START", //$NON-NLS-1$
			"" //$NON-NLS-1$
	};

	/**
	 * Exit name prefixes
	 */
	private static final String[] EXIT_NAME_PREFIXES = { "", //$NON-NLS-1$
			"EVENT_", //$NON-NLS-1$
			"" //$NON-NLS-1$
	};

	/**
	 * Exit name suffixes
	 */
	public static final String[] EXIT_NAME_SUFFIXES = { "_EXIT", //$NON-NLS-1$
			"_STOP", //$NON-NLS-1$
			"" //$NON-NLS-1$
	};

	/**
	 * Gets an entry template title
	 * 
	 * @param type
	 *            the template type
	 * @return the template title
	 */
	static String getEntryTemplateTitle(int type) {
		return ENTRY_TEMPLATE_TITLES[type];
	}

	/**
	 * Creates exit name from entry name
	 * 
	 * @param entryName
	 *            the entry name
	 * @param type
	 *            the type of entry
	 * @return the exit name
	 */
	static String createExitName(String entryName, int type) {
		// Strips the duplicate modifier from the name
		entryName = TraceObjectUtils.removeDuplicateModifier(entryName);
		StringBuffer sb = new StringBuffer();
		if (entryName.startsWith(ENTRY_NAME_PREFIXES[type])) {
			entryName = entryName.substring(ENTRY_NAME_PREFIXES[type].length());
		}
		if (entryName.endsWith(ENTRY_NAME_SUFFIXES[type])) {
			entryName = entryName.substring(0, entryName.length()
					- ENTRY_NAME_SUFFIXES[type].length());
		}
		sb.append(EXIT_NAME_PREFIXES[type]);
		sb.append(entryName);
		sb.append(EXIT_NAME_SUFFIXES[type]);
		return sb.toString();
	}

	/**
	 * Creates an entry trace name format from type
	 * 
	 * @param type
	 *            the trace format type
	 * @return the trace name format
	 */
	public static String createEntryTraceNameFormat(int type) {
		StringBuffer sb = new StringBuffer();
		if (type == TYPE_PERF_EVENT || type == TYPE_STATE_TRACE) {
			sb.append(NAME_FORMAT_BASE);
		} else {
			sb.append(ENTRY_NAME_PREFIXES[type]);
			sb.append(NAME_FORMAT_BASE);
			sb.append(ENTRY_NAME_SUFFIXES[type]);
		}
		return sb.toString();
	}

	/**
	 * Creates an exit trace name format based on type
	 * 
	 * @param type
	 *            the format type
	 * @return the trace name format
	 */
	public static String createExitTraceNameFormat(int type) {
		StringBuffer sb = new StringBuffer();
		sb.append(EXIT_NAME_PREFIXES[type]);
		sb.append(NAME_FORMAT_BASE);
		sb.append(EXIT_NAME_SUFFIXES[type]);
		return sb.toString();
	}

	/**
	 * Gets the return statements from current context from the context manager.
	 * 
	 * @param returnStatements
	 *            the list where the return statements are stored
	 */
	static void getCurrentContextReturns(List<SourceReturn> returnStatements) {
		SourceContext context = TraceCompilerEngineGlobals.getSourceContextManager()
				.getContext();
		if (context != null) {
			try {
				context.parseReturnValues(returnStatements);
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
				TraceCompilerEngineGlobals.getEvents().postErrorMessage(msg, source,
						true);
			}
		} else {
			if (TraceCompilerEngineConfiguration.ASSERTIONS_ENABLED) {
				TraceCompilerEngineGlobals.getEvents().postAssertionFailed(
						"Function return not in function", null); //$NON-NLS-1$
			}
		}
	}

	/**
	 * Checks if the function is static. Currently this is just dummy, checking
	 * that the class name exists and the function name is not New / NewL /
	 * NewLC
	 * 
	 * @param context
	 *            the context to be checked
	 * @return true if static, false if not
	 */
	static boolean isStaticFunction(SourceContext context) {
		boolean retval;
		if (context.getClassName() == null) {
			retval = true;
		} else {
			String fname = context.getFunctionName();
			if (fname.equals("New") //$NON-NLS-1$
					|| fname.equals("NewL") //$NON-NLS-1$
					|| fname.equals("NewLC") //$NON-NLS-1$
			) {
				retval = true;
			} else {
				retval = false;
			}
		}
		return retval;
	}

	/**
	 * Locates a constant table based on type definition parsed from source
	 * 
	 * @param model
	 *            the trace model
	 * @param type
	 *            the parameter type
	 * @return the constant table or null if not found
	 */
	static TraceConstantTable findConstantTableByType(TraceModel model,
			ParsedType type) {
		Iterator<TraceConstantTable> tables = model.getConstantTables();
		TraceConstantTable foundTable = null;
		while (tables.hasNext() && foundTable == null) {
			TraceConstantTable table = tables.next();
			if (type.typeEquals(table.getName())) {
				foundTable = table;
			}
		}
		return foundTable;
	}

	/**
	 * Creates a parameter based on TypeMapping definition
	 * 
	 * @param owner
	 *            the parameter owner
	 * @param name
	 *            the parameter name
	 * @param type
	 *            the type mapping
	 * @return the parameter
	 * @throws TraceCompilerException
	 *             if parameter cannot be created
	 */
	static TraceParameter createParameterFromType(Trace owner, String name,
			TypeMapping type) throws TraceCompilerException {
		int id = owner.getNextParameterID();
		owner.getModel().getVerifier().checkTraceParameterProperties(owner,
				null, id, name, type.type);
		TraceModelExtension[] extensions = null;
		if (type.needsCasting || type.valueToPointer
				|| type.type.equals(TraceParameter.TIME)) {
			extensions = new TraceModelExtension[1];
			extensions[0] = new ParameterTypeMappingRule(type);
		}
		TraceParameter parameter = owner.getModel().getFactory()
				.createTraceParameter(owner, id, name, type.type, extensions);
		return parameter;
	}

	/**
	 * Creates a parameter object from constant table
	 * 
	 * @param owner
	 *            the parameter owner
	 * @param name
	 *            the name for the parameter
	 * @param table
	 *            the constant table
	 * @return the parameter
	 * @throws TraceCompilerException
	 *             if parameter cannot be created
	 */
	static TraceParameter createParameterFromConstantTable(Trace owner,
			String name, TraceConstantTable table) throws TraceCompilerException {
		int id = owner.getNextParameterID();
		owner.getModel().getVerifier().checkTraceParameterProperties(owner,
				null, id, name, table.getType());
		// Constant tables need a cast to constant table type
		TypeMapping type = new TypeMapping(table.getType());
		type.needsCasting = true;
		TraceModelExtension[] extensions = new TraceModelExtension[1];
		extensions[0] = new ParameterTypeMappingRule(type);
		return owner.getModel().getFactory().createTraceParameter(owner, id,
				name, table.getName(), extensions);
	}

}
