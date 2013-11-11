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
* Tag writer
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;

/**
 * Tag writer
 * 
 */
class TagWriter {

	/**
	 * 
	 */
	private static final String OUTPUT_FORMAT_UTF8 = "UTF-8"; //$NON-NLS-1$

	/**
	 * 
	 */
	private static OutputStreamWriter m_writer = null;

	/**
	 * 
	 */
	static final String XML_START = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String CLASSIFICATION_ATTR = "classification="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String CLASSNAME_ATTR = "classname="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String COMPONENT_START_OPEN = "<component "; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String COMPONENT_END = "</component>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String DATA_REF_ATTR = "data-ref="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String DATASTORE_START = "<datastore>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String DATASTORE_END = "<datastore>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String DEF_END = "</def>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String DEF_START_OPEN = "<def "; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String DICTIONARY_START = "<tracedictionary xmlns:xsi=" + //$NON-NLS-1$
			"\"http://www.w3.org/2001/XMLSchema-instance\" " + //$NON-NLS-1$
			"xsi:noNamespaceSchemaLocation=\"DictionarySchema.xsd\">";//$NON-NLS-1$

	/**
	 * 
	 */
	static final String DICTIONARY_END = "</tracedictionary>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String END_TAG_OPEN = "</"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String EXTERNAL_DEF_END = "</external-def>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String EXTERNAL_DEF_START = "<external-def>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String FILE_START_OPEN = "<file "; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String FILE_END = "</file>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String FORMATCHAR_ATTR = "formatchar="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String GROUP_END = "</group>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String GROUP_START_OPEN = "<group "; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String ID_ATTR = "id="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String INSTANCE_START_OPEN = "<instance "; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String INSTANCE_END = "</instance>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String LINE_ATTR = "line="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String LOC_REF_ATTR = "loc-ref="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String LOCATIONS_START = "<locations>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String LOCATIONS_END = "</locations>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String METHODNAME_ATTR = "methodname="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String METADATA_START = "<metadata>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String METADATA_END = "</metadata>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String NAME_ATTR = "name="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String NAME_START = "<name>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String NAME_END = "</name>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String OBJECT_END = "</object>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String OBJECT_START = "<object>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String OBJECT_START_OPEN = "<object "; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String OPTIONS_START = "<options>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String OPTIONS_START_OPEN = "<options "; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String OPTIONS_END = "</options>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String PATH_START_OPEN = "<path "; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String PATH_END = "</path>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String PREFIX_ATTR = "prefix="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String QUOTE = "\""; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String ROW_CHANGE = "\n"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String SIZE_ATTR = "size="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String SPACE = " "; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String START_TAG_OPEN = "<"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String SUFFIX_ATTR = "suffix="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TAB = "\t"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TAG_CLOSE = ">"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TAG_END = "/>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TRACE_END = "</trace>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TRACE_START = "<trace>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TRACE_START_OPEN = "<trace "; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TRACE_DATA_END = "</data>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TRACE_DATA_START = "<data>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TYPE_ATTR = "type="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TYPEDEFS_START = "<typedefs>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TYPEDEFS_END = "</typedefs>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TYPEMEMBER_END = "</typemember>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TYPEMEMBER_START = "<typemember>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String TYPEMEMBER_START_OPEN = "<typemember "; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String VAL_ATTR = "val="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String VALUE_ATTR = "value="; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String VALUE_START = "<value>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final String VALUE_END = "</value>"; //$NON-NLS-1$

	/**
	 * 
	 */
	static final int INDENT_0 = 0;

	/**
	 * 
	 */
	static final int INDENT_1 = 1;

	/**
	 * 
	 */
	static final int INDENT_2 = 2; // CodForChk_Dis_Magic

	/**
	 * 
	 */
	static final int INDENT_3 = 3;// CodForChk_Dis_Magic

	/**
	 * 
	 */
	static final int INDENT_4 = 4;// CodForChk_Dis_Magic

	/**
	 * 
	 */
	static final int INDENT_5 = 5;// CodForChk_Dis_Magic

	/**
	 * 
	 */
	static final int INDENT_6 = 6;// CodForChk_Dis_Magic

	/**
	 * Initializes TagWriter
	 * 
	 * @param filePath
	 *            name of the output file
	 */
	static void initialize(String filePath) {
		try {
			OutputStream fout = new FileOutputStream(filePath);
			OutputStream bout = new BufferedOutputStream(fout);
			m_writer = new OutputStreamWriter(bout, OUTPUT_FORMAT_UTF8);
		} catch (java.io.IOException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Writes tag to XML file
	 * 
	 * @param tag
	 *            name of the tag
	 * @param indent
	 *            amount of tabs for indent
	 */
	static void write(String tag, int indent) {
		try {
			StringBuffer sb = new StringBuffer();
			for (int i = 0; i < indent; i++) {
				sb.append(TAB);
			}
			sb.append(tag);
			sb.append(ROW_CHANGE);
			m_writer.write(sb.toString());
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Appends attribute to given StringBuffer
	 * 
	 * @param sb
	 *            buffer to be updated
	 * @param attribute
	 *            attribute to append
	 * @param value
	 *            value of the attribute
	 * @param withSpace
	 *            is there leading space before attribute
	 */
	static void appendAttribute(StringBuffer sb, String attribute,
			String value, boolean withSpace) {
		if (value != null) {
			if (withSpace) {
				sb.append(SPACE);
			}
			sb.append(attribute);
			sb.append(QUOTE);
			sb.append(value);
			sb.append(QUOTE);
		}
	}

	/**
	 * Closes TagWriter
	 */
	static void close() {
		try {
			m_writer.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}

		try {
			m_writer.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
