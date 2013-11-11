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
* Creates exit traces from entry traces
*
*/
package com.nokia.tracecompiler.engine.rules;

/**
 * Creates exit traces from entry traces
 * 
 */
interface ExitTracePropertyBuilder {

	/**
	 * Gets the name for exit trace
	 * 
	 * @param entryName
	 *            name of the entry trace
	 * @return the name for exit trace
	 */
	String createExitName(String entryName);

	/**
	 * Gets the exit trace text based on entry trace
	 * 
	 * @param entryText
	 *            entry trace text
	 * @return the text for exit trace
	 */
	String createExitText(String entryText);

}
