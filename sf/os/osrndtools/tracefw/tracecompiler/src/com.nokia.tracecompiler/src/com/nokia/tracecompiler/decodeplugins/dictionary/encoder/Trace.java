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
* Writes Trace objects
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * Writes Trace objects
 * 
 */
public class Trace {

	/**
	 * Starts &lt;trace data-ref=""&gt; structure
	 * 
	 * @param dataRef
	 *            reference to actual data stored in &lt;data&gt; structure
	 * @param name
	 *            name of the trace. Can be null if trace doesn't have name
	 */
	static void startTrace(int dataRef, String name) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.TRACE_START_OPEN);
		if (dataRef >= 0) {
			TagWriter.appendAttribute(sb, TagWriter.DATA_REF_ATTR, String
					.valueOf(dataRef), false);

			if (name != null && name != "") { //$NON-NLS-1$
				TagWriter.appendAttribute(sb, TagWriter.NAME_ATTR, name, true);
			}
		}
		sb.append(TagWriter.TAG_CLOSE);
		TagWriter.write(sb.toString(), TagWriter.INDENT_3);

	}

	/**
	 * Writes &lt;/trace&gt;
	 */
	static void endTrace() {
		TagWriter.write(TagWriter.TRACE_END, TagWriter.INDENT_3);
	}

	/**
	 * Writes &lt;options&gt;
	 */
	public static void startOptions() {
		Options.startOptions(TagWriter.INDENT_4);
	}

	/**
	 * Writes &lt;/options&gt;
	 */
	public static void endOptions() {
		Options.endOptions(TagWriter.INDENT_4);
	}

	/**
	 * Writes user defined tags &lt;mytagXXX&gt;value&lt;/mytagXXX&gt;
	 * 
	 * @param tag
	 *            user defined tag
	 * @param value
	 *            value of the tag
	 */
	public static void writeOption(String tag, String value) {
		Options.writeOptions(tag, value, TagWriter.INDENT_5);
	}

	/**
	 * Writes &lt;instance id="" locRef="" line="" methodName=""
	 * className=""/&gt; structure
	 * 
	 * @param id
	 *            unique id number for this instance in current group structure
	 * @param locRef
	 *            unique location reference for instance (reference to
	 *            &lt;locations&gt; structure
	 * @param line
	 *            positive number of line where data were founded in source file
	 * @param methodName
	 *            name of the function from which trace was generated
	 * @param className
	 *            this is class name or namespace name for method described in
	 *            methodname attribute
	 */
	public static void writeInstance(int id, int locRef, int line,
			String methodName, String className) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.INSTANCE_START_OPEN);
		TagWriter.appendAttribute(sb, TagWriter.ID_ATTR, String.valueOf(id),
				false);
		TagWriter.appendAttribute(sb, TagWriter.LOC_REF_ATTR, String
				.valueOf(locRef), true);
		TagWriter.appendAttribute(sb, TagWriter.LINE_ATTR,
				String.valueOf(line), true);
		TagWriter.appendAttribute(sb, TagWriter.METHODNAME_ATTR, methodName,
				true);
		if (className != null) {
			TagWriter.appendAttribute(sb, TagWriter.CLASSNAME_ATTR, className,
					true);
		}
		sb.append(TagWriter.TAG_END);
		TagWriter.write(sb.toString(), TagWriter.INDENT_4);
	}
}
