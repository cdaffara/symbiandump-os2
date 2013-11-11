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
* Changes the new line count
*
*/
package com.nokia.tracecompiler.engine.header;

/**
 * Changes the new line count
 * 
 */
public final class SetNewLineCount {

	/**
	 * Line feed count
	 */
	private int count;

	/**
	 * Constructor
	 * 
	 * @param count
	 *            the line feed count
	 */
	SetNewLineCount(int count) {
		this.count = count;
	}

	/**
	 * Gets the count
	 * 
	 * @return the count
	 */
	final int getLineCount() {
		return count;
	}

}
