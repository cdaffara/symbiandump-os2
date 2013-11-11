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
* Cache file for storing trace and group identifiers
*
*/
package com.nokia.tracecompiler.engine.project;

import com.nokia.tracecompiler.project.TraceProjectFile;

/**
 * Cache file for storing trace and group identifiers
 * 
 */
public final class TraceIDCache extends TraceProjectFile {

	/**
	 * Title shown in UI
	 */
	private static final String TITLE = Messages
			.getString("TraceIDCache.Title"); //$NON-NLS-1$

	/**
	 * Constructor
	 * 
	 * @param cachePath
	 *            path to the cache file
	 */
	public TraceIDCache(String cachePath) {
		super(cachePath, ""); //$NON-NLS-1$
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceProjectFile#getFileExtension()
	 */
	@Override
	protected String getFileExtension() {
		return ProjectConstants.FIXED_ID_DEFINITIONS_FILE_NAME;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceProjectFile#getTitle()
	 */
	@Override
	public String getTitle() {
		return TITLE;
	}

}
