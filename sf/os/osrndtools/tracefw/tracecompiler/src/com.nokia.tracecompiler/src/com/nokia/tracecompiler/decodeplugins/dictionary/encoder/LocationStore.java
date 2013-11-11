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
* Location store
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * Location store
 * 
 */
public class LocationStore {

	/**
	 * Writes &lt;locations&gt;
	 */
	public static void startLocations() {
		TagWriter.write(TagWriter.LOCATIONS_START, TagWriter.INDENT_1);
	}

	/**
	 * Starts &lt;path val=""&gt; structure
	 * 
	 * @param path
	 *            relative or absolute path to some directory
	 */
	public static void startPath(String path) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.PATH_START_OPEN);
		TagWriter.appendAttribute(sb, TagWriter.VAL_ATTR, path, false);
		sb.append(TagWriter.TAG_CLOSE);
		TagWriter.write(sb.toString(), TagWriter.INDENT_2);
	}

	/**
	 * Writes &lt;file id=""&gt;fileName&lt;/file&gt; structure
	 * 
	 * @param id
	 *            unique number within &lt;locations&gt; structure
	 * @param fileName
	 *            value of the file element
	 */
	public static void writeFile(int id, String fileName) {
		StringBuffer sb = new StringBuffer();
		sb.append(TagWriter.FILE_START_OPEN);
		TagWriter.appendAttribute(sb, TagWriter.ID_ATTR, String.valueOf(id),
				false);
		sb.append(TagWriter.TAG_CLOSE);
		sb.append(XMLDataFilter.changeData(fileName));
		sb.append(TagWriter.FILE_END);
		TagWriter.write(sb.toString(), TagWriter.INDENT_3);
	}

	/**
	 * Writes &lt;/path&gt;
	 */
	public static void endPath() {
		TagWriter.write(TagWriter.PATH_END, TagWriter.INDENT_2);
	}

	/**
	 * Writes &lt;/locations&gt;
	 */
	public static void endLocations() {
		TagWriter.write(TagWriter.LOCATIONS_END, TagWriter.INDENT_1);
	}

}
