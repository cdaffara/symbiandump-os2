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
* TraceCompiler project file properties
*
*/
package com.nokia.tracecompiler.engine.project;

import com.nokia.tracecompiler.project.TraceProjectFile;

/**
 * TraceCompiler project file properties
 * 
 */
final class TraceCompilerProject extends TraceProjectFile {

	/**
	 * Title shown in UI
	 */
	private static final String TITLE = Messages
			.getString("TraceCompilerProject.Title"); //$NON-NLS-1$

	/**
	 * Constructor
	 * 
	 * @param absolutePath
	 *            the path to the file, including file name
	 */
	TraceCompilerProject(String absolutePath) {
		super(absolutePath, true);
	}

	/**
	 * Constructor
	 * 
	 * @param path
	 *            the path to the file
	 * @param name
	 *            the file name
	 */
	TraceCompilerProject(String path, String name) {
		super(path, name);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceProjectFile#getFileExtension()
	 */
	@Override
	protected String getFileExtension() {
		return null;
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
