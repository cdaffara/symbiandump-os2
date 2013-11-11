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
* Dictionary tag
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * Dictionary tag
 * 
 */
public class Dictionary {

	/**
	 * Writes start of the XML file and starts &lt;tracedictionary&gt; structure
	 */
	public static void startDictionary() {
		TagWriter.write(TagWriter.XML_START, TagWriter.INDENT_0);
		TagWriter.write(TagWriter.DICTIONARY_START, TagWriter.INDENT_0);
	}

	/**
	 * Writes &lt;/tracedictionary&gt;
	 */
	public static void endDictionary() {
		TagWriter.write(TagWriter.DICTIONARY_END, TagWriter.INDENT_0);
		TagWriter.close();
	}

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
	public static void startComponent(int id, String name, String prefix,
			String suffix) {
		TraceComponent.startComponent(id, name, prefix, suffix);

	}

	/**
	 * Writes &lt;/component&gt;
	 */
	public static void endComponent() {
		TraceComponent.endComponent();
	}

	/**
	 * Starts &lt;metadata&gt; structure
	 */
	public static void startMetaData() {
		TagWriter.write(TagWriter.METADATA_START, TagWriter.INDENT_1);
	}

	/**
	 * Writes user defined tags &lt;mytagXXX&gt;value&lt;/mytagXXX&gt;
	 * 
	 * @param name
	 *            tag
	 * @param value
	 *            value of the tag
	 */
	public static void writeMetaData(String name, String value) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.START_TAG_OPEN);
		sb.append(name);
		sb.append(TagWriter.TAG_CLOSE);
		sb.append(XMLDataFilter.changeData(value));
		sb.append(TagWriter.END_TAG_OPEN);
		sb.append(name);
		sb.append(TagWriter.TAG_CLOSE);
		TagWriter.write(sb.toString(), TagWriter.INDENT_2);
	}

	/**
	 * Writes &lt;/metadata&gt;
	 */
	public static void endMetaData() {
		TagWriter.write(TagWriter.METADATA_END, TagWriter.INDENT_1);
	}
}