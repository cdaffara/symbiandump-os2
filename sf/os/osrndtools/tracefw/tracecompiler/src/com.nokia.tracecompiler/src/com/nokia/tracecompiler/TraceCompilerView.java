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
 * Console view implementation
 *
 */
package com.nokia.tracecompiler;

import java.io.File;

import com.nokia.tracecompiler.engine.TraceCompilerEngineConfiguration;
import com.nokia.tracecompiler.engine.ViewAdapter;

/**
 * TraceCompiler view implementation
 * 
 */
class TraceCompilerView extends ViewAdapter {

	/**
	 * Epoc root name in environment variables
	 */
	private static final String EPOCROOT = "EPOCROOT"; //$NON-NLS-1$

	/**
	 * Export path
	 */
	private String exportPath;

	/**
	 * Configuration
	 */
	private TraceCompilerConfiguration configuration;

	/**
	 * Constructor
	 * 
	 * @param projectPath
	 *            the project path
	 */
	TraceCompilerView(String projectPath) {
		// First try to get EPOCROOT from environment
		exportPath = System.getenv(EPOCROOT);

		// Windows substed drive
		if (exportPath == null) {
			int index = projectPath.indexOf(':');
			if (index >= 0) {
				exportPath = projectPath.substring(0, index + 2); // CodForChk_Dis_Magic
			} else {
				exportPath = File.separator;
			}

			// Check if it ends with a file separator, if not add one at the end
		} else if (!exportPath.endsWith(File.separator)) {
			exportPath = exportPath + File.separator;
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.ViewAdapter#getConfiguration()
	 */
	@Override
	public TraceCompilerEngineConfiguration getConfiguration() {
		if (configuration == null) {
			configuration = new TraceCompilerConfiguration(exportPath);
		}
		return configuration;
	}
}
