/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Property verifier for trace objects
*
*/
package com.nokia.tracecompiler.engine.plugin;

import java.util.Iterator;

import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.RangeErrorParameters;
import com.nokia.tracecompiler.engine.TraceCompilerEngineErrorCodes.TraceCompilerErrorCode;
import com.nokia.tracecompiler.model.Trace;
import com.nokia.tracecompiler.model.TraceCompilerException;
import com.nokia.tracecompiler.model.TraceConstantTable;
import com.nokia.tracecompiler.model.TraceConstantTableEntry;
import com.nokia.tracecompiler.model.TraceGroup;
import com.nokia.tracecompiler.model.TraceModel;
import com.nokia.tracecompiler.model.TraceObjectPropertyVerifier;
import com.nokia.tracecompiler.model.TraceParameter;
import com.nokia.tracecompiler.plugin.TraceCompilerPlugin;
import com.nokia.tracecompiler.source.SourceConstants;
import com.nokia.tracecompiler.source.SourceUtils;
import com.nokia.tracecompiler.project.*;

/**
 * Property verifier for trace objects
 * 
 */
public final class PluginTracePropertyVerifier implements TraceObjectPropertyVerifier {

	/**
	 * Valid data character range start
	 */
	private static final int DATA_CHAR_START = 0x20; // CodForChk_Dis_Magic

	/**
	 * Valid data character range end
	 */
	private static final int DATA_CHAR_END = 0x7E; // CodForChk_Dis_Magic
	/**
	 * TAB character
	 */
	private static final int TAB =  0x9;  // CodForChk_Dis_Magic

	/**
	 * Plugin engine
	 */
	private PluginEngine pluginEngine;

	/**
	 * Constructor. Empty public constructor is needed by test class.
	 */
	public PluginTracePropertyVerifier() {
		
	}
	
	/**
	 * Constructor
	 * 
	 * @param engine
	 *            plug-in engine
	 */
	PluginTracePropertyVerifier(PluginEngine engine) {
		this.pluginEngine = engine;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectPropertyVerifier#
	 *      checkConstantProperties(com.nokia.tracecompiler.model.TraceConstantTable,
	 *      com.nokia.tracecompiler.model.TraceConstantTableEntry, int,
	 *      java.lang.String)
	 */
	public void checkConstantProperties(TraceConstantTable table,
			TraceConstantTableEntry entry, int id, String value)
			throws TraceCompilerException {
		if (!SourceUtils.isValidName(value)) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.INVALID_CONSTANT_VALUE);
		}
		if (table != null) {
			// If table exists, the value and ID must be unique
			TraceConstantTableEntry old = table.findEntryByID(id);
			if (old != null && old != entry) {
				throw new TraceCompilerException(
						TraceCompilerErrorCode.DUPLICATE_CONSTANT_ID);
			}
			old = table.findEntryByName(value);
			if (old != null && old != entry) {
				throw new TraceCompilerException(
						TraceCompilerErrorCode.DUPLICATE_CONSTANT_VALUE);
			}
		}
		Iterator<TraceCompilerPlugin> itr = pluginEngine.getPlugins();
		while (itr.hasNext()) {
			TraceCompilerPlugin provider = itr.next();
			if (provider instanceof TraceObjectPropertyVerifier) {
				((TraceObjectPropertyVerifier) provider)
						.checkConstantProperties(table, entry, id, value);
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectPropertyVerifier#
	 *      checkConstantTableProperties(com.nokia.tracecompiler.model.TraceModel,
	 *      com.nokia.tracecompiler.model.TraceConstantTable, int,
	 *      java.lang.String)
	 */
	public void checkConstantTableProperties(TraceModel model,
			TraceConstantTable table, int id, String tableName)
			throws TraceCompilerException {
		if (!SourceUtils.isValidName(tableName)) {
			 throw new TraceCompilerException(
			 TraceCompilerErrorCode.INVALID_CONSTANT_TABLE_NAME, false);
		}
		TraceConstantTable old = model.findConstantTableByID(id);
		if (old != null && old != table) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.DUPLICATE_CONSTANT_TABLE_ID);
		}
		old = model.findConstantTableByName(tableName);
		if (old != null && old != table) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.DUPLICATE_CONSTANT_TABLE_NAME);
		}
		Iterator<TraceCompilerPlugin> itr = pluginEngine.getPlugins();
		while (itr.hasNext()) {
			TraceCompilerPlugin provider = itr.next();
			if (provider instanceof TraceObjectPropertyVerifier) {
				((TraceObjectPropertyVerifier) provider)
						.checkConstantTableProperties(model, table, id,
								tableName);
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectPropertyVerifier#
	 *      checkTraceGroupProperties(com.nokia.tracecompiler.model.TraceModel,
	 *      com.nokia.tracecompiler.model.TraceGroup, int, java.lang.String)
	 */
	public void checkTraceGroupProperties(TraceModel model, TraceGroup group,
			int id, String name) throws TraceCompilerException {
		if (!SourceUtils.isValidName(name)) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.INVALID_GROUP_NAME);
		} else if ((id < 0) || (id > TraceCompilerEngineGlobals.MAX_GROUP_ID)) {
			RangeErrorParameters params = new RangeErrorParameters();
			params.start = 0;
			params.end = TraceCompilerEngineGlobals.MAX_GROUP_ID;
			params.isHex = true;
			throw new TraceCompilerException(
					TraceCompilerErrorCode.INVALID_GROUP_ID, params);
		} else {
			TraceGroup old = model.findGroupByID(id);
			// this condition is to check that user defined group ids are unique.
			// system defined group ids at present are not required to be unique.
			if ((old != null) && (old != group) && (id >= GroupNames.USER_GROUP_ID_FIRST  ) && (id <= GroupNames.USER_GROUP_ID_LAST )) {
				throw new TraceCompilerException(
						TraceCompilerErrorCode.DUPLICATE_GROUP_ID);
			}
			old = model.findGroupByName(name);
			if (old != null && old != group) {
				throw new TraceCompilerException(
						TraceCompilerErrorCode.DUPLICATE_GROUP_NAME);
			}
		}
		Iterator<TraceCompilerPlugin> itr = pluginEngine.getPlugins();
		while (itr.hasNext()) {
			TraceCompilerPlugin provider = itr.next();
			if (provider instanceof TraceObjectPropertyVerifier) {
				((TraceObjectPropertyVerifier) provider)
						.checkTraceGroupProperties(model, group, id, name);
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectPropertyVerifier#
	 *      checkTraceModelProperties(com.nokia.tracecompiler.model.TraceModel,
	 *      int, java.lang.String, java.lang.String)
	 */
	public void checkTraceModelProperties(TraceModel model, int id,
			String name, String path) throws TraceCompilerException {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectPropertyVerifier#
	 *      checkTraceParameterProperties(com.nokia.tracecompiler.model.Trace,
	 *      com.nokia.tracecompiler.model.TraceParameter, int, java.lang.String,
	 *      java.lang.String)
	 */
	public void checkTraceParameterProperties(Trace owner,
			TraceParameter parameter, int id, String name, String type)
			throws TraceCompilerException {
		if (!SourceUtils.isValidParameterName(name)) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.INVALID_PARAMETER_NAME);
		}
		TraceParameter old = owner.findParameterByID(id);
		if (old != null && old != parameter) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.DUPLICATE_PARAMETER_ID);
		}
		old = owner.findParameterByName(name);
		if (old != null && old != parameter) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.DUPLICATE_PARAMETER_NAME);
		}
		Iterator<TraceCompilerPlugin> itr = pluginEngine.getPlugins();
		while (itr.hasNext()) {
			TraceCompilerPlugin provider = itr.next();
			if (provider instanceof TraceObjectPropertyVerifier) {
				((TraceObjectPropertyVerifier) provider)
						.checkTraceParameterProperties(owner, parameter, id,
								name, type);
			}
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectPropertyVerifier#
	 *      checkTraceProperties(com.nokia.tracecompiler.model.TraceGroup,
	 *      com.nokia.tracecompiler.model.Trace, int, java.lang.String,
	 *      java.lang.String)
	 */
	public void checkTraceProperties(TraceGroup group, Trace trace, int id,
			String name, String data) throws TraceCompilerException {
		if (!isValidData(data)) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.INVALID_TRACE_DATA);
		} else if (!SourceUtils.isValidName(name)) {
			throw new TraceCompilerException(
					TraceCompilerErrorCode.INVALID_TRACE_NAME);
		} else if ((id < 0) || (id > TraceCompilerEngineGlobals.MAX_TRACE_ID)) {
			RangeErrorParameters params = new RangeErrorParameters();
			params.start = 0;
			params.end = TraceCompilerEngineGlobals.MAX_TRACE_ID;
			params.isHex = true;
			throw new TraceCompilerException(
					TraceCompilerErrorCode.INVALID_TRACE_ID, params);
		} else {
			// Verifies the trace name is globally unique
			Trace old = TraceCompilerEngineGlobals.getTraceModel().findTraceByName(
					name);
			if (old != trace && old != null) {
				throw new TraceCompilerException(
						TraceCompilerErrorCode.DUPLICATE_TRACE_NAME);
			}
			if (group != null) {
				// If group exists, the trace ID and text must be unique within
				// the group
				old = group.findTraceByID(id);
				if (old != trace && old != null) {
					// Trace ID's must be unique within group
					throw new TraceCompilerException(
							TraceCompilerErrorCode.DUPLICATE_TRACE_ID);
				}
			}
		}
		Iterator<TraceCompilerPlugin> itr = pluginEngine.getPlugins();
		while (itr.hasNext()) {
			TraceCompilerPlugin provider = itr.next();
			if (provider instanceof TraceObjectPropertyVerifier) {
				((TraceObjectPropertyVerifier) provider).checkTraceProperties(
						group, trace, id, name, data);
			}
		}
	}

	/**
	 * Checks the validity of data
	 * 
	 * @param data
	 *            the data
	 * @return true if valid
	 */
	private boolean isValidData(String data) {
		boolean retval;
		if (data != null) {
			retval = true;
			for (int i = 0; i < data.length() && retval; i++) {
				char c = data.charAt(i);
				// Unescaped quotes are not allowed
				if (c == SourceConstants.QUOTE_CHAR
						&& (i == 0 || data.charAt(i - 1) != SourceConstants.BACKSLASH_CHAR)) {
					retval = false;
				} else {
					retval = isValidDataChar(c);
				}
			}
		} else {
			retval = false;
		}
		return retval;
	}

	/**
	 * Checks data character validity
	 * 
	 * @param c
	 *            character
	 * @return true if valid
	 */
	private boolean isValidDataChar(char c) {
		// Special and extended characters are not allowed, except TAB
		return c >= DATA_CHAR_START && c <= DATA_CHAR_END || c == TAB;
	}

}
