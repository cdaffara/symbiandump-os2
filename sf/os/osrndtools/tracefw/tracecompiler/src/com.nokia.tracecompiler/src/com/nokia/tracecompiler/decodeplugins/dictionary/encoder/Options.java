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
* Options
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * Options
 * 
 */
public class Options {

	/**
	 * Writes &lt;options&gt;
	 * 
	 * @param indent
	 *            amount of tabs for indent
	 */
	static void startOptions(int indent) {
		TagWriter.write(TagWriter.OPTIONS_START, indent);
	}

	/**
	 * Writes &lt;/options&gt;
	 * 
	 * @param indent
	 *            amount of tabs for indent
	 */
	static void endOptions(int indent) {
		TagWriter.write(TagWriter.OPTIONS_END, indent);
	}

	/**
	 * Writes user defined tags &lt;mytagXXX&gt;value&lt;/mytagXXX&gt;
	 * 
	 * @param tag
	 *            user defined tag
	 * @param value
	 *            value of the tag
	 * @param indent
	 *            amount of tabs for indent
	 */
	static void writeOptions(String tag, String value, int indent) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.START_TAG_OPEN);
		sb.append(tag);
		sb.append(TagWriter.TAG_CLOSE);
		sb.append(XMLDataFilter.changeData(value));
		sb.append(TagWriter.END_TAG_OPEN);
		sb.append(tag);
		sb.append(TagWriter.TAG_CLOSE);
		TagWriter.write(sb.toString(), indent);
	}

}
