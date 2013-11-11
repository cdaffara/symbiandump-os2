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
* Parameter name modifier
*
*/
package com.nokia.tracecompiler.model;

/**
 * Parameter name modifier
 * 
 */
final class DuplicateParameterNameModifier extends DuplicateValueModifier {

	/**
	 * The parameter owner
	 */
	private Trace owner;

	/**
	 * Constructor
	 * 
	 * @param owner
	 *            the owner of the parameter
	 * @param name
	 *            the parameter name
	 */
	DuplicateParameterNameModifier(Trace owner, String name) {
		super(name);
		this.owner = owner;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceObjectUtils.DuplicateValueModifier#
	 *      findObject(java.lang.String)
	 */
	@Override
	TraceObject findObject(String name) {
		TraceObject retval;
		if (owner != null) {
			retval = owner.findParameterByName(name);
		} else {
			retval = null;
		}
		return retval;
	}
}