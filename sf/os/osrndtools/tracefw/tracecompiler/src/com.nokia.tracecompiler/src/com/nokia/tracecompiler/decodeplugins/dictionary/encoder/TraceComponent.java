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
* Writes component objects
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * Writes component objects
 * 
 */
public class TraceComponent {

	/**
	 * Starts &lt;component id="", name="" prefix="" suffix=""&gt; structure
	 * 
	 * @param id
	 *            unique component id within dictionary
	 * @param name
	 *            name of the component
	 * @param prefix
	 *            optional prefix for the component
	 * @param suffix
	 *            optional suffix for the component
	 */
	static void startComponent(int id, String name, String prefix, String suffix) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.COMPONENT_START_OPEN);
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
		TagWriter.write(sb.toString(), TagWriter.INDENT_1);
	}

	/**
	 * Writes &lt;/component&gt;
	 */
	static void endComponent() {
		TagWriter.write(TagWriter.COMPONENT_END, TagWriter.INDENT_1);
	}

	/**
	 * Writes &lt;options&gt;
	 */
	public static void startOptions() {
		Options.startOptions(TagWriter.INDENT_2);
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
		Options.writeOptions(tag, value, TagWriter.INDENT_3);
	}

	/**
	 * Writes &lt;/options&gt;
	 */
	public static void endOptions() {
		Options.endOptions(TagWriter.INDENT_2);
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
	public static void startGroup(int id, String name, String prefix,
			String suffix) {
		TraceGroup.startGroup(id, name, prefix, suffix);
	}

	/**
	 * Writes &lt;/group&gt;
	 */
	public static void endGroup() {
		TraceGroup.endGroup();
	}
}
