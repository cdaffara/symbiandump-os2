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
* Selects an area from a source
*
*/
package com.nokia.tracecompiler.source;

/**
 * Selects an area from a source
 * 
 */
public interface SourceSelector {

	/**
	 * Selects an area defined by given offset and length
	 * 
	 * @param offset
	 *            the offset
	 * @param length
	 *            the length
	 */
	public void setSelection(int offset, int length);

}
