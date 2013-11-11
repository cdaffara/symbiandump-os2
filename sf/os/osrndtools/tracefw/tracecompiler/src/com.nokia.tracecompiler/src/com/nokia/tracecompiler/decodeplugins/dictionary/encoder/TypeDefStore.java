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
* Writes type definition objects
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * Writes type definition objects
 * 
 */
public class TypeDefStore {

	/**
	 * Writes &lt;typedef&gt;
	 */
	public static void startTypeDefs() {
		TagWriter.write(TagWriter.TYPEDEFS_START, TagWriter.INDENT_1);
	}

	/**
	 * Writes &lt;external-def&gt;path&lt;/external-def&gt; structure
	 * 
	 * @param path
	 *            path to external dictionary file
	 */
	public static void writeExternalDefFile(String path) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.EXTERNAL_DEF_START);
		sb.append(XMLDataFilter.changeData(path));
		sb.append(TagWriter.EXTERNAL_DEF_END);
		TagWriter.write(sb.toString(), TagWriter.INDENT_2);
	}

	/**
	 * Starts &lt;object type="" size="" formatChar="" dataType=""&gt; structure
	 * 
	 * @param type
	 *            original type name
	 * @param size
	 *            size of typical object, it’s optional because dictionary user
	 *            can calculate size for compound types by sum size of each
	 *            member. Give -1 if not used.
	 * @param formatChar
	 *            used for formatting
	 * @param dataType
	 *            one value form &lt;datatype&gt; enumeration
	 */
	public static void startTypeDef(String type, int size, String formatChar,
			DataType dataType) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.OBJECT_START_OPEN);
		TagWriter.appendAttribute(sb, TagWriter.TYPE_ATTR, type, false);
		if (size >= 0) { // optional attribute
			TagWriter.appendAttribute(sb, TagWriter.SIZE_ATTR, String
					.valueOf(size), true);
		}
		if (formatChar != null) { // optional attribute
			TagWriter.appendAttribute(sb, TagWriter.FORMATCHAR_ATTR,
					formatChar, true);
		}
		TagWriter.appendAttribute(sb, TagWriter.CLASSIFICATION_ATTR, dataType
				.getType(), true);
		sb.append(TagWriter.TAG_CLOSE);
		TagWriter.write(sb.toString(), TagWriter.INDENT_2);
	}

	/**
	 * Writes &lt;object type="" size="" formatChar="" dataType=""/&gt;
	 * structure
	 * 
	 * @param type
	 *            original type name
	 * @param size
	 *            size of typical object, it’s optional because dictionary user
	 *            can calculate size for compound types by sum size of each
	 *            member. Give -1 if not used.
	 * @param formatChar
	 *            used for formatting
	 * @param dataType
	 *            one value form &lt;datatype&gt; enumeration
	 */
	public static void writeTypeDef(String type, int size, String formatChar,
			DataType dataType) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.OBJECT_START_OPEN);
		TagWriter.appendAttribute(sb, TagWriter.TYPE_ATTR, type, false);
		if (size >= 0) {
			TagWriter.appendAttribute(sb, TagWriter.SIZE_ATTR, String
					.valueOf(size), true);
		}
		if (formatChar != null) {
			TagWriter.appendAttribute(sb, TagWriter.FORMATCHAR_ATTR,
					formatChar, true);
		}
		TagWriter.appendAttribute(sb, TagWriter.CLASSIFICATION_ATTR, dataType
				.getType(), true);
		sb.append(TagWriter.TAG_END);
		TagWriter.write(sb.toString(), TagWriter.INDENT_2);
	}

	/**
	 * Writes &lt;/object&gt;
	 */
	public static void endTypeDef() {
		TagWriter.write(TagWriter.OBJECT_END, TagWriter.INDENT_2);
	}

	/**
	 * Writes &lt;/typedef&gt;
	 */
	public static void endTypeDefs() {
		TagWriter.write(TagWriter.TYPEDEFS_END, TagWriter.INDENT_1);
	}
}
