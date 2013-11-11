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
* Common properties of locations
*
*/
package com.nokia.tracecompiler.engine;

/**
 * Common properties of locations
 * 
 */
public interface LocationProperties {

	/**
	 * Gets the file path
	 * 
	 * @return the file path
	 */
	public String getFilePath();

	/**
	 * Gets the file name
	 * 
	 * @return the file name
	 */
	public String getFileName();

	/**
	 * Gets the line number
	 * 
	 * @return the line number
	 */
	public int getLineNumber();

	/**
	 * Gets the name of the class which owns this location
	 * 
	 * @return the class name
	 */
	public String getClassName();

	/**
	 * Gets the name of function which owns this location
	 * 
	 * @return the function name
	 */
	public String getFunctionName();

}
