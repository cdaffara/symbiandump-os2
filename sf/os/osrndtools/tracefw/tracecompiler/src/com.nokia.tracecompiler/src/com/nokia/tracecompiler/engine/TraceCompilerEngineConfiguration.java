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
* Access to configuration of TraceCompiler
*
*/
package com.nokia.tracecompiler.engine;

/**
 * Access to configuration of TraceCompiler
 * 
 */
public interface TraceCompilerEngineConfiguration {

	/**
	 * Name of the currently selected source formatter
	 */
	public String FORMATTER_NAME = "TraceCompiler.sourceFormatter"; //$NON-NLS-1$

	/**
	 * Environment root
	 */
	public String ENVIRONMENT_ROOT = "TraceCompiler.environmentRoot"; //$NON-NLS-1$

	/**
	 * Printf support flag
	 */
	public String PRINTF_SUPPORT = "TraceCompiler.printfSupport"; //$NON-NLS-1$

	/**
	 * Printf extension macro
	 */
	public String PRINTF_EXTENSION = "TraceCompiler.printfMacro"; //$NON-NLS-1$

	/**
	 * Console UI mode flag
	 */
	public String CONSOLE_UI_MODE = "TraceCompiler.consoleUI"; //$NON-NLS-1$

	/**
	 * Flag to show the project files in view
	 */
	public boolean SHOW_PROJECT_FILES = false;

	/**
	 * Compiler flag to show hidden parameters in view
	 */
	public boolean SHOW_HIDDEN_PARAMETERS_IN_VIEW = false;

	/**
	 * Compiler flag to show location tag in view
	 */
	public boolean SHOW_LOCATION_TAG_IN_VIEW = false;

	/**
	 * Assertions events are enabled
	 */
	public boolean ASSERTIONS_ENABLED = true;

	/**
	 * Trace location assertions are enabled
	 */
	public boolean GLOBAL_LOCATION_ASSERTS = false;

	/**
	 * Traces the view state
	 */
	public boolean TRACE_VIEW_STATE = false;

	/**
	 * Allow changes to source formatter
	 */
	public boolean ALLOW_FORMAT_CHANGE = false;

	/**
	 * Comment after trace
	 */
	public String COMMENT_POSITION_AFTER_TRACE = "before"; //$NON-NLS-1$

	/**
	 * Comment before trace
	 */
	public String COMMENT_POSITION_BEFORE_TRACE = "after"; //$NON-NLS-1$

	/**
	 * Flag which determines if the configuration is available
	 * 
	 * @return true if available, false if not
	 */
	public boolean isAvailable();

	/**
	 * Stores a boolean flag into properties
	 * 
	 * @param flagName
	 *            name for the flag
	 * @param flag
	 *            flag to be stored
	 */
	public void setFlag(String flagName, boolean flag);

	/**
	 * Gets a configuration flag based on flag name
	 * 
	 * @param flagName
	 *            the confiugration flag name
	 * @return the flag value
	 */
	public boolean getFlag(String flagName);

	/**
	 * Stores an integer value into properties
	 * 
	 * @param valueName
	 *            name for the value
	 * @param value
	 *            value to be stored
	 */
	public void setValue(String valueName, int value);

	/**
	 * Gets a configuration value based on value name
	 * 
	 * @param valueName
	 *            configuration value name
	 * @return the value
	 */
	public int getValue(String valueName);

	/**
	 * Stores text into properties
	 * 
	 * @param textName
	 *            name for the text
	 * @param text
	 *            text to be stored
	 */
	public void setText(String textName, String text);

	/**
	 * Gets a text entry based on property name
	 * 
	 * @param textName
	 *            configuration value name
	 * @return the text
	 */
	public String getText(String textName);

	/**
	 * Checks if the given entry exists in configuration.
	 * 
	 * @param entryName
	 *            the name of the entry
	 * @return true if entry exists, false otherwise
	 */
	public boolean hasEntry(String entryName);

	/**
	 * Adds a configuration listener to this configuration
	 * 
	 * @param configurationListener
	 *            the listener to be added
	 */
	public void addConfigurationListener(
			TraceCompilerEngineConfigurationListener configurationListener);

	/**
	 * Removes an existing configuration listener
	 * 
	 * @param configurationListener
	 *            the listener to be removed
	 */
	public void removeConfigurationListener(
			TraceCompilerEngineConfigurationListener configurationListener);

}
