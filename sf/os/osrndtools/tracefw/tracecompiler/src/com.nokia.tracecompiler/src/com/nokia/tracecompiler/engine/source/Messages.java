/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Localized strings for engine package
*
*/
package com.nokia.tracecompiler.engine.source;

import java.util.MissingResourceException;
import java.util.ResourceBundle;

public class Messages {

	/**
	 * Bundle name
	 */	
	private static final String BUNDLE_NAME = "com.nokia.tracecompiler.engine.source.messages"; //$NON-NLS-1$

	/**
	 * Bundle
	 */
	private static final ResourceBundle RESOURCE_BUNDLE = ResourceBundle
			.getBundle(BUNDLE_NAME);

	/**
	 * Prevents construction
	 */
	private Messages() {
	}

	/**
	 * Gets localized string based on key
	 * 
	 * @param key
	 *            the resource key
	 * @return the localized resource
	 */
	public static String getString(String key) {
		try {
			return RESOURCE_BUNDLE.getString(key);
		} catch (MissingResourceException e) {
			return '!' + key + '!';
		}
	}
}
