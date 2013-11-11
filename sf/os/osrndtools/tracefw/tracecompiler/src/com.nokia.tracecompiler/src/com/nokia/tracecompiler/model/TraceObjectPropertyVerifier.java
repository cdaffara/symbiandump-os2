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
* Content verifier interface for trace objects
*
*/
package com.nokia.tracecompiler.model;

/**
 * Content verifier interface for trace objects
 * 
 */
public interface TraceObjectPropertyVerifier {

	/**
	 * Checks the properties of a trace before it is created or modified
	 * 
	 * @param group
	 *            the group for the trace
	 * @param trace
	 *            trace to be modified. If creating new, this is null
	 * @param id
	 *            the trace ID
	 * @param name
	 *            the name of the trace
	 * @param data
	 *            the trace data
	 * @throws TraceCompilerException
	 *             if properties are not valid. The error code from the
	 *             exception is used to create an error dialog
	 */
	public void checkTraceProperties(TraceGroup group, Trace trace, int id,
			String name, String data) throws TraceCompilerException;

	/**
	 * Checks the properties of a trace group before it is created or modified
	 * 
	 * @param owner
	 *            the model owning the trace group
	 * @param group
	 *            group to be modified. If creating new, this is null
	 * @param id
	 *            the trace ID
	 * @param name
	 *            the name of the trace
	 * @throws TraceCompilerException
	 *             if properties are not valid. The error code from the
	 *             exception is used to create an error dialog
	 */
	public void checkTraceGroupProperties(TraceModel owner, TraceGroup group,
			int id, String name) throws TraceCompilerException;

	/**
	 * Checks the properties of a parameter before it is created or modified
	 * 
	 * @param owner
	 *            the parameter owner
	 * @param parameter
	 *            parameter to be modified. If creating new, this is null
	 * @param id
	 *            the parameter ID
	 * @param name
	 *            the name of the parameter
	 * @param type
	 *            the type of the parameter
	 * @throws TraceCompilerException
	 *             if properties are not valid. The error code from the
	 *             exception is used to create an error dialog
	 */
	public void checkTraceParameterProperties(Trace owner,
			TraceParameter parameter, int id, String name, String type)
			throws TraceCompilerException;

	/**
	 * Checks the properties of a constant table before it is created or
	 * modified
	 * 
	 * @param owner
	 *            the model owning the constant table
	 * @param table
	 *            the table to be changed. If creating new, this is null
	 * @param id
	 *            the ID of the table
	 * @param tableName
	 *            the name of the table
	 * @throws TraceCompilerException
	 *             if properties are not valid. The error code from the
	 *             exception is used to create an error dialog
	 */
	public void checkConstantTableProperties(TraceModel owner,
			TraceConstantTable table, int id, String tableName)
			throws TraceCompilerException;

	/**
	 * Checks the properties of a constant table entry before it is created
	 * 
	 * @param table
	 *            the constant table
	 * @param entry
	 *            the entry to be modified. If creating new, this is null
	 * @param id
	 *            the ID for the new entry
	 * @param value
	 *            the value for the new entry
	 * @throws TraceCompilerException
	 *             if properties are not valid. The error code from the
	 *             exception is used to create an error dialog
	 */
	public void checkConstantProperties(TraceConstantTable table,
			TraceConstantTableEntry entry, int id, String value)
			throws TraceCompilerException;

	/**
	 * Checks the properties of trace model before it is modified
	 * 
	 * @param model
	 *            the trace model
	 * @param id
	 *            the model ID
	 * @param name
	 *            the model name
	 * @param path
	 *            the model path
	 * @throws TraceCompilerException
	 *             if properties are not valid. The error code from the
	 *             exception is used to create an error dialog
	 */
	public void checkTraceModelProperties(TraceModel model, int id,
			String name, String path) throws TraceCompilerException;

}
