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
* Extension wrapper for XML document element
*
*/
package com.nokia.tracecompiler.engine.propertyfile;

import org.w3c.dom.Element;

import com.nokia.tracecompiler.model.TraceModelExtension;
import com.nokia.tracecompiler.model.TraceObject;

/**
 * Extension wrapper for XML document element
 * 
 */
final class DocumentElementWrapper implements TraceModelExtension {

	/**
	 * Owner
	 */
	private TraceObject owner;

	/**
	 * Constructor
	 * 
	 * @param element
	 *            the document element
	 */
	DocumentElementWrapper(Element element) {
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelExtension#getOwner()
	 */
	public TraceObject getOwner() {
		return owner;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.model.TraceModelExtension#setOwner(com.nokia.tracecompiler.model.TraceObject)
	 */
	public void setOwner(TraceObject owner) {
		this.owner = owner;
	}

}
