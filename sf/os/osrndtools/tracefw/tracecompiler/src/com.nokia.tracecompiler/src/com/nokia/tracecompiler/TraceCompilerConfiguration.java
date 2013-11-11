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
* Configuration for the console UI
*
*/
package com.nokia.tracecompiler;

import com.nokia.tracecompiler.engine.TraceCompilerEngineConfiguration;
import com.nokia.tracecompiler.engine.TraceCompilerEngineConfigurationListener;

/**
 * Configuration for the TraceCompiler UI
 * 
 */
final class TraceCompilerConfiguration implements TraceCompilerEngineConfiguration {

	/**
	 * Export path
	 */
	private String exportPath;

	/**
	 * Constructor
	 * 
	 * @param exportPath
	 *            the export path for decode files
	 */
	TraceCompilerConfiguration(String exportPath) {
		this.exportPath = exportPath;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerConfiguration#
	 *      addConfigurationListener(com.nokia.tracecompiler.engine.TraceCompilerConfigurationListener)
	 */
	public void addConfigurationListener(
			TraceCompilerEngineConfigurationListener configurationListener) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerConfiguration#
	 *      getFlag(java.lang.String)
	 */
	public boolean getFlag(String flagName) {
		boolean retval;
		if (flagName.equals(TraceCompilerEngineConfiguration.CONSOLE_UI_MODE)) {
			retval = true;
		} else {
			retval = false;
		}
		return retval;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerConfiguration#
	 *      getText(java.lang.String)
	 */
	public String getText(String textName) {
		String retval;
		if (textName.equals(TraceCompilerEngineConfiguration.ENVIRONMENT_ROOT)) {
			retval = exportPath;
		} else {
			retval = ""; //$NON-NLS-1$
		}
		return retval;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerConfiguration#
	 *      getValue(java.lang.String)
	 */
	public int getValue(String valueName) {
		return 0;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerConfiguration#
	 *      hasEntry(java.lang.String)
	 */
	public boolean hasEntry(String entryName) {
		return false;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerConfiguration#isAvailable()
	 */
	public boolean isAvailable() {
		return true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerConfiguration#
	 *      removeConfigurationListener(com.nokia.tracecompiler.engine.TraceCompilerConfigurationListener)
	 */
	public void removeConfigurationListener(
			TraceCompilerEngineConfigurationListener configurationListener) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerConfiguration#setFlag(java.lang.String,
	 *      boolean)
	 */
	public void setFlag(String flagName, boolean flag) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerConfiguration#setText(java.lang.String,
	 *      java.lang.String)
	 */
	public void setText(String textName, String text) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.engine.TraceCompilerConfiguration#setValue(java.lang.String,
	 *      int)
	 */
	public void setValue(String valueName, int value) {
	}

}
