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
* Constant values related to project files
*
*/
package com.nokia.tracecompiler.engine.project;

/**
 * Constant values related to project files
 * 
 */
public interface ProjectConstants {

	/**
	 * Title of the cache file
	 */
	String FIXED_ID_DEFINITION_FILE_TITLE = Messages
			.getString("ProjectConstants.FixedIdDefinitionFileTitle"); //$NON-NLS-1$

	/**
	 * Cache file extension
	 */
	String CACHE_FILE_NAME = "tracecompiler.cache"; //$NON-NLS-1$

	/**
	 * Fixed ID file extension
	 */
	String FIXED_ID_DEFINITIONS_FILE_NAME = "fixed_id.definitions"; //$NON-NLS-1$

	/**
	 * Extension for the file within zip file
	 */
	String FILE_EXTENSION = ".xml"; //$NON-NLS-1$

	/**
	 * Less than escape sequence
	 */
	String LT = "&lt;"; //$NON-NLS-1$

	/**
	 * Greater than escape sequence
	 */
	String GT = "&gt;"; //$NON-NLS-1$

	/**
	 * Indent
	 */
	String INDENT = "  "; //$NON-NLS-1$

	/**
	 * End a tag
	 */
	String END_TAG = ">"; //$NON-NLS-1$

	/**
	 * Start of end tag
	 */
	String START_END_TAG = "</"; //$NON-NLS-1$

	/**
	 * End tag with line feed
	 */
	String END_TAG_LF = ">\r\n"; //$NON-NLS-1$

	/**
	 * Start a tag
	 */
	String START_TAG = "<"; //$NON-NLS-1$

	/**
	 * Extension tag
	 */
	String EXTENSION_TAG = "extension"; //$NON-NLS-1$

	/**
	 * Data tag
	 */
	String DATA_TAG = "data"; //$NON-NLS-1$

	/**
	 * Name tag
	 */
	String NAME_TAG = "name"; //$NON-NLS-1$

	/**
	 * Object ID tag
	 */
	String ID_TAG = "id"; //$NON-NLS-1$

	/**
	 * Constant table entry tag
	 */
	String CONSTANT_TABLE_ENTRY_TAG = "entry"; //$NON-NLS-1$

	/**
	 * Parameter tag
	 */
	String PARAMETER_TAG = "parameter"; //$NON-NLS-1$

	/**
	 * Trace text tag
	 */
	String TRACE_TEXT_TAG = "text"; //$NON-NLS-1$

	/**
	 * Trace tag
	 */
	String TRACE_TAG = "trace"; //$NON-NLS-1$

	/**
	 * Parameter type tag
	 */
	String PARAMETER_TYPE_TAG = "type"; //$NON-NLS-1$

	/**
	 * Constant table tag
	 */
	String CONSTANT_TABLE_TAG = "table"; //$NON-NLS-1$

	/**
	 * Group tag
	 */
	String GROUP_TAG = "group"; //$NON-NLS-1$

	/**
	 * Model tag
	 */
	String MODEL_TAG = "model"; //$NON-NLS-1$

	/**
	 * Location tag
	 */
	String LOCATION_TAG = "location"; //$NON-NLS-1$

	/**
	 * File tag
	 */
	String LOCATION_FILE_TAG = "file"; //$NON-NLS-1$

	/**
	 * Line tag
	 */
	String LOCATION_LINE_TAG = "line"; //$NON-NLS-1$

	/**
	 * Class name tag
	 */
	String LOCATION_CLASS_NAME_TAG = "class"; //$NON-NLS-1$

	/**
	 * Function name tag
	 */
	String LOCATION_FUNCTION_NAME_TAG = "function"; //$NON-NLS-1$

}
