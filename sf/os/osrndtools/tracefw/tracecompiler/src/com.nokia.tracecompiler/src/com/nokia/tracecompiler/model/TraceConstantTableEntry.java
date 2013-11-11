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
* One entry in the integer-to-string mapping table
*
*/
package com.nokia.tracecompiler.model;

/**
 * One entry in the integer-to-string mapping table
 * 
 */
public class TraceConstantTableEntry extends TraceObject {

	/**
	 * Constant table that owns this parameter
	 */
	private TraceConstantTable table;

	/**
	 * Constructor
	 * 
	 * @param table
	 *            the constant table that owns this entry
	 */
	TraceConstantTableEntry(TraceConstantTable table) {
		setModel(table.getModel());
		this.table = table;
		table.addEntry(this);
	}

	/**
	 * Gets the table that owns this entry
	 * 
	 * @return the table
	 */
	public TraceConstantTable getTable() {
		return table;
	}

}