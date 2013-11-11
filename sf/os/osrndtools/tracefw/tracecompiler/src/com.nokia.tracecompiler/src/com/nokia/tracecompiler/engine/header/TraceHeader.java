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
* Representation of a header file that gets included into sources
*
*/
package com.nokia.tracecompiler.engine.header;

import com.nokia.tracecompiler.project.TraceProjectFile;

/**
 * Representation of a header file that gets included into sources when traces
 * are added.
 * 
 */
final class TraceHeader extends TraceProjectFile {

	/**
	 * Title shown in UI
	 */
	private static final String TITLE = Messages.getString("TraceHeader.Title"); //$NON-NLS-1$

	/**
	 * Constructor
	 * 
	 * @param target
	 *            target path
	 * @param hasModelName
	 *            true if model name should be used with the file
	 */
	TraceHeader(String target, boolean hasModelName) {
		super(target, hasModelName);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.project.TraceProjectFile#getFileExtension()
	 */
	@Override
	protected String getFileExtension() {
		String retval;
		if (hasModelName) {
			retval = HeaderConstants.TRACE_HEADER_EXTENSION;
		} else {
			retval = ""; //$NON-NLS-1$
		}
		return retval;
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
