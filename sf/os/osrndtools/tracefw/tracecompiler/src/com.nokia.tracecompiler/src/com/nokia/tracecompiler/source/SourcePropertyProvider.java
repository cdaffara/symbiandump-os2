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
* Property provider for SourceLocation objects
*
*/
package com.nokia.tracecompiler.source;

/**
 * Property provider for SourceLocation objects
 * 
 */
public interface SourcePropertyProvider {

	/**
	 * Gets a file name that can be shown in the UI when viewing a
	 * SourceLocation
	 * 
	 * @return the source file name
	 */
	public String getFileName();

	/**
	 * Gets the path to the source file including the ending separator.
	 * 
	 * @return the path
	 */
	public String getFilePath();

}
