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
* Properties of function return statement
*
*/
package com.nokia.tracecompiler.source;

/**
 * Properties of function return statement
 * 
 */
public class SourceReturn extends SourceLocationBase {

	/**
	 * Previous character might cause a problem
	 */
	private boolean charHazard;

	/**
	 * The return statement itself might cause a problem
	 */
	private boolean tagHazard;

	/**
	 * Constructor
	 * 
	 * @param parser
	 *            the parser owning this location
	 * @param offset
	 *            the offset to the beginning of return statement
	 * @param length
	 *            the return statement length
	 */
	SourceReturn(SourceParser parser, int offset, int length) {
		super(parser, offset, length);
	}

	/**
	 * Gets the return statement
	 * 
	 * @return the statement
	 */
	public String getReturnStatement() {
		String retval = ""; //$NON-NLS-1$
		if (getParser() != null) {
			retval = getParser().getData(getOffset(), getLength());
		}
		return retval;
	}

	/**
	 * Sets a flag which indicates that the character preceeding the return
	 * statement might cause problems when a trace is added prior to the return
	 * statement
	 */
	void setPreviousCharHazard() {
		this.charHazard = true;
	}

	/**
	 * Gets the flag which indicates if the character preceeding the return
	 * statement might cause problems when a trace is added prior to the return
	 * statement
	 * 
	 * @return the flag
	 */
	public boolean hasPreviousCharHazard() {
		return charHazard;
	}

	/**
	 * Sets a flag which indicates that the return statement might cause
	 * problems when duplicated to a trace
	 */
	void setTagHazard() {
		tagHazard = true;
	}

	/**
	 * Gets the flag which indicates if the return statement might cause
	 * problems when duplicated to a trace
	 * 
	 * @return the flag
	 */
	public boolean hasTagHazard() {
		return tagHazard;
	}

}
