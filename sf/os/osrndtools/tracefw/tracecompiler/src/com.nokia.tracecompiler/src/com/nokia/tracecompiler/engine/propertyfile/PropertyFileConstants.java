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
* Constants for property file engine
*
*/
package com.nokia.tracecompiler.engine.propertyfile;


/**
 * Constants for property file engine
 * 
 */
public interface PropertyFileConstants {

	/**
	 * File name
	 */
	String PROPERTY_FILE_NAME = "trace.properties"; //$NON-NLS-1$

	/**
	 * Number of backup files to keep
	 */
	int BACKUP_COUNT = 10; // CodForChk_Dis_Magic

	/**
	 * Last of the backup headers
	 */
	String LAST_BACKUP = "9.properties.bak"; //$NON-NLS-1$

	/**
	 * Backup extension
	 */
	String BACKUP_EXTENSION = ".bak"; //$NON-NLS-1$

	/**
	 * Root element name
	 */
	String ROOT_ELEMENT = "trace_properties"; //$NON-NLS-1$

	/**
	 * Enum element name
	 */
	String ENUM_ELEMENT = "enum"; //$NON-NLS-1$

	/**
	 * Struct element name
	 */
	String STRUCT_ELEMENT = "struct"; //$NON-NLS-1$

	/**
	 * Component element name
	 */
	String COMPONENT_ELEMENT = "component"; //$NON-NLS-1$

	/**
	 * Component element name
	 */
	String GROUP_ELEMENT = "group"; //$NON-NLS-1$

	/**
	 * Trace element name
	 */
	String TRACE_ELEMENT = "trace"; //$NON-NLS-1$

	/**
	 * Value element name
	 */
	String VALUE_ELEMENT = "value"; //$NON-NLS-1$

	/**
	 * Property element name
	 */
	String PROPERTY_ELEMENT = "property"; //$NON-NLS-1$

	/**
	 * File element name
	 */
	String FILE_ELEMENT = "file"; //$NON-NLS-1$
	
	/**
	 * Name attribute
	 */
	String NAME_ATTRIBUTE = "name"; //$NON-NLS-1$

	/**
	 * Format attribute
	 */
	String FORMAT_ATTRIBUTE = "format"; //$NON-NLS-1$

	/**
	 * ID attribute
	 */
	String ID_ATTRIBUTE = "id"; //$NON-NLS-1$

	/**
	 * Type attribute
	 */
	String TYPE_ATTRIBUTE = "type"; //$NON-NLS-1$

	/**
	 * Size attribute
	 */
	String SIZE_ATTRIBUTE = "size"; //$NON-NLS-1$

}
