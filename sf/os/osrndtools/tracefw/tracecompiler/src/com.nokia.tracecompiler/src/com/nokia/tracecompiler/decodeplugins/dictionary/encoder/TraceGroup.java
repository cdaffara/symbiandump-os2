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
* Writes groups objects
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * Writes groups objects
 * 
 */
public class TraceGroup {

	/**
	 * Writes &lt;options&gt;
	 */
	public static void startOptions() {
		Options.startOptions(TagWriter.INDENT_3);
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
		Options.writeOptions(tag, value, TagWriter.INDENT_4);
	}

	/**
	 * Writes &lt;/options&gt;
	 */
	public static void endOptions() {
		Options.endOptions(TagWriter.INDENT_3);
	}

	/**
	 * Starts &lt;trace data-ref=""&gt; structure
	 * 
	 * @param dataRef
	 *            reference to actual data stored in &lt;data&gt; structure
	 * @param name
	 *            name of the trace. Can be null if trace doesn't have name            
	 */
	public static void startTrace(int dataRef, String name) {
		Trace.startTrace(dataRef, name);
	}

	/**
	 * Writes &lt;/trace&gt;
	 */
	public static void endTrace() {
		Trace.endTrace();
	}

	/**
	 * Starts &lt;group id="", name="" prefix="" suffix=""&gt; structure
	 * 
	 * @param id
	 *            unique group identifier within dictionary
	 * @param name
	 *            group name
	 * @param prefix
	 *            optional prefix for the group
	 * @param suffix
	 *            optional suffix for the group
	 */
	static void startGroup(int id, String name, String prefix, String suffix) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.GROUP_START_OPEN);
		TagWriter.appendAttribute(sb, TagWriter.ID_ATTR, String.valueOf(id),
				false);
		TagWriter.appendAttribute(sb, TagWriter.NAME_ATTR, name, true);
		if (prefix != null) {
			TagWriter.appendAttribute(sb, TagWriter.PREFIX_ATTR, prefix, true);
		}
		if (suffix != null) {
			TagWriter.appendAttribute(sb, TagWriter.SUFFIX_ATTR, suffix, true);
		}
		sb.append(TagWriter.TAG_CLOSE);
		TagWriter.write(sb.toString(), TagWriter.INDENT_2);

	}

	/**
	 * Writes &lt;/group&gt;
	 */
	static void endGroup() {
		TagWriter.write(TagWriter.GROUP_END, TagWriter.INDENT_2);
	}
}