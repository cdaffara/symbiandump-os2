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
* Rule which defines a parameter as array type
*
*/
package com.nokia.tracecompiler.engine.rules;

import com.nokia.tracecompiler.model.TraceModelPersistentExtension;
import com.nokia.tracecompiler.rules.ArrayParameterRule;

/**
 * Rule which defines a parameter as array type
 * 
 */
final class ArrayParameterRuleImpl extends RuleBase implements
		ArrayParameterRule, TraceModelPersistentExtension {

	/**
	 * Storage name for array parameter
	 */
	static final String STORAGE_NAME = "Array"; //$NON-NLS-1$

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelPersistentExtension#getData()
	 */
	public String getData() {
		return ""; //$NON-NLS-1$
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelPersistentExtension#getStorageName()
	 */
	public String getStorageName() {
		return STORAGE_NAME;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelPersistentExtension#
	 *      setData(java.lang.String)
	 */
	public boolean setData(String data) {
		return true;
	}
}
