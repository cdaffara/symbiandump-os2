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
* Interface which abstracts the underlying position framework
*
*/
package com.nokia.tracecompiler.source;

/**
 * Interface which abstracts the underlying position framework
 * 
 */
public interface SourceLocationInterface {

	/**
	 * Gets the offset of the location
	 * 
	 * @return the offset
	 */
	int getOffset();

	/**
	 * Sets the offset
	 * 
	 * @param offset
	 *            the new offset
	 */
	void setOffset(int offset);

	/**
	 * Gets the length of the location
	 * 
	 * @return the length
	 */
	int getLength();

	/**
	 * Sets the length
	 * 
	 * @param length
	 *            the length
	 */
	void setLength(int length);

	/**
	 * Checks if the location has been deleted
	 * 
	 * @return true if deleted
	 */
	boolean isDeleted();

	/**
	 * Marks the location as deleted
	 */
	void delete();

}