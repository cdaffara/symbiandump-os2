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
* Writes type def objects
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * Writes type def objects
 * 
 */
public class TypeDef {

	/**
	 * Writes &lt;typemember value="" name="" type=""/&gt; structure
	 * 
	 * @param value
	 *            value which will be assigned to name attribute in case where
	 *            object have classification equals enum
	 * @param name
	 *            text which is assigned to value attribute or to type attribute
	 * @param type
	 *            name of data type that will be assigned to name attribute in
	 *            case where object classification equals compound
	 */
	public static void writeTypeMember(int value, String name, String type) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.TYPEMEMBER_START_OPEN);
		TagWriter.appendAttribute(sb, TagWriter.VALUE_ATTR, String
				.valueOf(value), false);
		TagWriter.appendAttribute(sb, TagWriter.NAME_ATTR, name, true);
		if (type != null) { // optional attribute
			TagWriter.appendAttribute(sb, TagWriter.TYPE_ATTR, type, true);
		}
		sb.append(TagWriter.TAG_END);
		TagWriter.write(sb.toString(), TagWriter.INDENT_3);
	}

}
