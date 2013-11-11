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
* Preferences for dictionary
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary;

/**
 * Preferences for dictionary
 * 
 */
public interface DictionaryPreferences {

	/**
	 * Export path for dictionary files
	 */
	String EXPORT_PATH = "Dictionary.exportPath"; //$NON-NLS-1$

	/**
	 * Default path where to export dictionary files
	 */
	String DEFAULT_EXPORT_PATH = "epoc32/ost_dictionaries"; //$NON-NLS-1$

	/**
	 * Gets a property from preferences
	 * 
	 * @param name
	 *            the property name
	 * @return the property value
	 */
	public String getString(String name);

}
