/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Utility functions related to traces
*
*/
package com.nokia.tracecompiler.engine.utils;

import com.nokia.tracecompiler.plugin.TraceFormatConstants;

/**
 * Utility functions related to traces
 * 
 */
public final class TraceUtils {

	/**
	 * Underscore character
	 */
	private static final String UNDERSCORE = "_"; //$NON-NLS-1$

	/**
	 * Underscore character
	 */
	private static final char UNDERSCORE_CHAR = '_';

	/**
	 * Cannot be constructed
	 */
	private TraceUtils() {
	}

	/**
	 * Formats a trace
	 * 
	 * @param format
	 *            the format specification
	 * @param cname
	 *            the class name
	 * @param fname
	 *            the function name
	 * @return the formatted trace
	 */
	public static String formatTrace(String format, String cname, String fname) {
		StringBuffer sb = new StringBuffer(format);
		int cnindex = sb
				.indexOf(TraceFormatConstants.FORMAT_CLASS_NAME_NORMAL_CASE);
		if (cnindex >= 0) {
			if (cname != null) {
				sb.replace(cnindex, cnindex
						+ TraceFormatConstants.FORMAT_CLASS_NAME_NORMAL_CASE
								.length(), cname);
			} else {
				sb.replace(cnindex, cnindex
						+ TraceFormatConstants.FORMAT_CLASS_NAME_NORMAL_CASE
								.length(), ""); //$NON-NLS-1$
			}
		}
		int cnup = sb
				.indexOf(TraceFormatConstants.FORMAT_CLASS_NAME_UPPER_CASE);
		if (cnup >= 0) {
			if (cname != null) {
				sb.replace(cnup, cnup
						+ TraceFormatConstants.FORMAT_CLASS_NAME_UPPER_CASE
								.length(), cname.toUpperCase());
			} else {
				sb.replace(cnup, cnup
						+ TraceFormatConstants.FORMAT_CLASS_NAME_UPPER_CASE
								.length(), ""); //$NON-NLS-1$
			}
		}
		int fnindex = sb
				.indexOf(TraceFormatConstants.FORMAT_FUNCTION_NAME_NORMAL_CASE);
		if (fnindex >= 0) {
			sb.replace(fnindex, fnindex
					+ TraceFormatConstants.FORMAT_FUNCTION_NAME_NORMAL_CASE
							.length(), fname);
		}
		int fnup = sb
				.indexOf(TraceFormatConstants.FORMAT_FUNCTION_NAME_UPPER_CASE);
		if (fnup >= 0) {
			sb.replace(fnup, fnup
					+ TraceFormatConstants.FORMAT_FUNCTION_NAME_UPPER_CASE
							.length(), fname.toUpperCase());
		}
		return sb.toString();
	}

	/**
	 * Replaces invalid characters with '_'
	 * 
	 * @param name
	 *            name to be converted
	 * @return the converted name
	 */
	public static String convertName(String name) {
		StringBuffer sb;
		if (name.length() > 0) {
			boolean underscore = false;
			sb = new StringBuffer(name);
			if (Character.isDigit(name.charAt(0))) {
				sb.insert(0, UNDERSCORE_CHAR);
			}
			for (int i = 0; i < sb.length(); i++) {
				char c = sb.charAt(i);
				if (!Character.isLetterOrDigit(c)) {
					if (!underscore) {
						sb.setCharAt(i, UNDERSCORE_CHAR);
						underscore = true;
					} else {
						sb.deleteCharAt(i);
						i--;
					}
				} else {
					underscore = false;
				}
			}
			if (sb.length() > 0) {
				if (sb.charAt(sb.length() - 1) == UNDERSCORE_CHAR) {
					sb.deleteCharAt(sb.length() - 1);
				}
			} else {
				sb.append(UNDERSCORE);
			}
		} else {
			sb = new StringBuffer();
		}
		// If parameter value is NULL, it would be used as name
		String s = sb.toString();
		if (s.equals("NULL")) { //$NON-NLS-1$
			s = "_NULL"; //$NON-NLS-1$
		}
		return s;
	}
}
