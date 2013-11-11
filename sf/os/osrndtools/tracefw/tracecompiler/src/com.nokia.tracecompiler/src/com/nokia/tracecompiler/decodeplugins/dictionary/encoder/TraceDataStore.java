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
* Writes Data objects
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * Writes Data objects
 * 
 */
public class TraceDataStore {

	/**
	 * Writes &lt;data&gt;
	 */
	public static void startDataStore() {
		TagWriter.write(TagWriter.TRACE_DATA_START, TagWriter.INDENT_1);
	}

	/**
	 * Writes &lt;def id="" dataType=""&gt;value&lt;/def&gt; structure
	 * 
	 * @param id
	 *            unique number in &lt;data&gt; structure, it is data identifier
	 * @param dataType
	 *            data type, defaults to string
	 * @param value
	 *            the value
	 * 
	 */
	public static void writeData(int id, DataType dataType, String value) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.DEF_START_OPEN);
		TagWriter.appendAttribute(sb, TagWriter.ID_ATTR, String.valueOf(id),
				false);
		if (dataType != null) {
			TagWriter.appendAttribute(sb, TagWriter.TYPE_ATTR, dataType
					.getType(), true);
		}
		sb.append(TagWriter.TAG_CLOSE);
		sb.append(XMLDataFilter.changeData(value));
		sb.append(TagWriter.DEF_END);
		TagWriter.write(sb.toString(), TagWriter.INDENT_2);
	}

	/**
	 * Writes &lt;/data&gt;
	 */
	public static void endDataStore() {
		TagWriter.write(TagWriter.TRACE_DATA_END, TagWriter.INDENT_1);
	}
}
