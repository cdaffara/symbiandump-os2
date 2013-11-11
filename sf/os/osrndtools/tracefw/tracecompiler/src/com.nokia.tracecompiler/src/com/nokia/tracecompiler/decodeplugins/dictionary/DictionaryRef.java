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
* Reference base class
*
*/
package com.nokia.tracecompiler.decodeplugins.dictionary;

import com.nokia.tracecompiler.model.TraceModelExtension;
import com.nokia.tracecompiler.model.TraceObject;

/**
 * Reference base class
 * 
 */
class DictionaryRef implements TraceModelExtension {

	/**
	 * Reference id
	 */
	int refid;

	/**
	 * Constructor
	 * 
	 * @param refid
	 *            the reference id
	 */
	DictionaryRef(int refid) {
		this.refid = refid;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelExtension#getOwner()
	 */
	public TraceObject getOwner() {
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelExtension#
	 *      setOwner(com.nokia.tracecompiler.model.TraceObject)
	 */
	public void setOwner(TraceObject owner) {
	}

	/**
	 * Gets the reference id
	 * 
	 * @return ID
	 */
	int getRefId() {
		return refid;
	}

}