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
* Version number abstraction
*
*/
package com.nokia.tracecompiler.utils;

/**
 * Version number abstraction
 * 
 */
public abstract class TraceCompilerVersion {

	/**
	 * Sub-class, which implements getVersion
	 */
	private static TraceCompilerVersion versionImpl;

	/**
	 * Gets TraceCompiler version number
	 * 
	 * @return version number
	 */
	public static String getVersion() {
		String version = null;
		if (versionImpl != null) {
			version = versionImpl.getTraceCompilerVersion();
		} else {
			// TODO: This is used in console builds -> Maintenance needed
			version = "2.3.0"; //$NON-NLS-1$
		}
		return version;
	}

	/**
	 * Gets the version number as x.y.z
	 * 
	 * @return the version string
	 */
	protected abstract String getTraceCompilerVersion();

}
