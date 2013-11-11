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
* Replaces special characters with escape sequences 
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary.encoder;

/**
 * Replaces special characters with escape sequences
 * 
 */
class XMLDataFilter {

	/**
	 * XML start tag
	 */
	private static final String LEFT_ARROW = "<"; //$NON-NLS-1$

	/**
	 * XML end tag
	 */
	private static final String RIGHT_ARROW = ">"; //$NON-NLS-1$

	/**
	 * XML entity tag
	 */
	private static final String AMPERSAND = "&"; //$NON-NLS-1$

	/**
	 * XML start tag escaped
	 */
	private static final String LEFT_ARROW_SUBST = "&lt;"; //$NON-NLS-1$

	/**
	 * XML end tag escaped
	 */
	private static final String RIGHT_ARROW_SUBST = "&gt;"; //$NON-NLS-1$

	/**
	 * XML entity tag escaped
	 */
	private static final String AMPERSAND_SUBST = "&amp;"; //$NON-NLS-1$

	/**
	 * Not found result
	 */
	private static final int NOT_FOUND = -1;

	/**
	 * Changes XML data
	 * 
	 * @param data
	 *            data to be changed
	 * @return changed data
	 */
	static String changeData(String data) {

		// This must be before any other that will add & marks to the text.
		// Otherwise those will be replaced also.
		if (data.indexOf(AMPERSAND) != NOT_FOUND) {
			data = data.replaceAll(AMPERSAND, AMPERSAND_SUBST);
		}
		if (data.indexOf(LEFT_ARROW) != NOT_FOUND) {
			data = data.replaceAll(LEFT_ARROW, LEFT_ARROW_SUBST);
		}
		if (data.indexOf(RIGHT_ARROW) != NOT_FOUND) {
			data = data.replaceAll(RIGHT_ARROW, RIGHT_ARROW_SUBST);
		}

		return data;
	}
}
