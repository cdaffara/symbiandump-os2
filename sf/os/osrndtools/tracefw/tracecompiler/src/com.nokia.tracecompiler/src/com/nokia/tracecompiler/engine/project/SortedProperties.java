/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Sorted properties
*
*/
package com.nokia.tracecompiler.engine.project;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.util.Collections;
import java.util.Enumeration;
import java.util.Properties;
import java.util.Vector;

import com.nokia.tracecompiler.engine.TraceCompilerEngineGlobals;

/**
 * Sorted properties
 * 
 */
public final class SortedProperties extends Properties {

	/**
	 * UID
	 */
	private static final long serialVersionUID = 746540919659257261L;

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.util.Hashtable#keys()
	 */
	@Override
	@SuppressWarnings("unchecked")
	public synchronized Enumeration<Object> keys() {
		Enumeration<Object> keysEnum = super.keys();
		Vector keyList = new Vector();
		while (keysEnum.hasMoreElements()) {
			keyList.add(keysEnum.nextElement());
		}
		Collections.sort(keyList);
		return keyList.elements();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.util.Properties#store(java.io.OutputStream, java.lang.String)
	 */
	@Override
	public synchronized void store(OutputStream out, String comments)
			throws IOException {
		BufferedWriter awriter;
		awriter = new BufferedWriter(new OutputStreamWriter(out, "8859_1")); //$NON-NLS-1$
		String licence = TraceCompilerEngineGlobals.getDefaultLicence(false);
		if (licence != null) {
			out.write(licence.getBytes());
		}
		if (comments != null)
			writeln(awriter, "#" + comments); //$NON-NLS-1$

		for (Enumeration<Object> e = keys(); e.hasMoreElements();) {
			String key = (String) e.nextElement();
			String val = (String) get(key);
			key = saveConvert(key, true);

			/*
			 * No need to escape embedded and trailing spaces for value, hence
			 * pass false to flag.
			 */
			val = saveConvert(val, false);
			writeln(awriter, key + "=" + val); //$NON-NLS-1$
		}
		awriter.flush();
	}

	/**
	 * Converts unicodes to encoded &#92;uxxxx and escapes special characters
	 * with a preceding slash
	 * 
	 * @param theString
	 *            the string to be convert
	 * @param escapeSpace
	 *            the escape space flag
	 * @return the coverted string
	 */
	private String saveConvert(String theString, boolean escapeSpace) { // CodForChk_Dis_ComplexFunc
		int len = theString.length();
		int bufLen = len * 2; // CodForChk_Dis_Magic
		if (bufLen < 0) {
			bufLen = Integer.MAX_VALUE;
		}
		StringBuffer outBuffer = new StringBuffer(bufLen);

		for (int x = 0; x < len; x++) {
			char aChar = theString.charAt(x);
			// Handle common case first, selecting largest block that
			// avoids the specials below
			if ((aChar > 61) && (aChar < 127)) { // CodForChk_Dis_Magic
				if (aChar == '\\') {
					outBuffer.append('\\');
					outBuffer.append('\\');
					continue;
				}
				outBuffer.append(aChar);
				continue;
			}
			switch (aChar) {
			case ' ':
				if (x == 0 || escapeSpace)
					outBuffer.append('\\');
				outBuffer.append(' ');
				break;
			case '\t':
				outBuffer.append('\\');
				outBuffer.append('t');
				break;
			case '\n':
				outBuffer.append('\\');
				outBuffer.append('n');
				break;
			case '\r':
				outBuffer.append('\\');
				outBuffer.append('r');
				break;
			case '\f':
				outBuffer.append('\\');
				outBuffer.append('f');
				break;
			case '=': // Fall through
			case ':': // Fall through
			case '#': // Fall through
			case '!':
				outBuffer.append('\\');
				outBuffer.append(aChar);
				break;
			default:
				if ((aChar < 0x0020) || (aChar > 0x007e)) { // CodForChk_Dis_Magic
					outBuffer.append('\\');
					outBuffer.append('u');
					outBuffer.append(toHex((aChar >> 12) & 0xF)); // CodForChk_Dis_Magic
					outBuffer.append(toHex((aChar >> 8) & 0xF)); // CodForChk_Dis_Magic
					outBuffer.append(toHex((aChar >> 4) & 0xF)); // CodForChk_Dis_Magic
					outBuffer.append(toHex(aChar & 0xF)); // CodForChk_Dis_Magic
				} else {
					outBuffer.append(aChar);
				}
			}
		}
		return outBuffer.toString();
	}

	/**
	 * Convert a nibble to a hex character
	 * 
	 * @param nibble
	 *            the nibble to convert
	 * @return the hex character
	 */
	private static char toHex(int nibble) {
		return hexDigit[(nibble & 0xF)]; // CodForChk_Dis_Magic
	}

	/**
	 * A table of hex digits
	 */
	private static final char[] hexDigit = { '0', '1', '2', '3', '4', '5', '6',
			'7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	/**
	 * Write line
	 * 
	 * @param bw
	 *            the buffered writer that used to write
	 * @param s
	 *            the string what to write
	 * @throws IOException
	 */
	private static void writeln(BufferedWriter bw, String s) throws IOException {
		bw.write(s);
		bw.newLine();
	}
}
