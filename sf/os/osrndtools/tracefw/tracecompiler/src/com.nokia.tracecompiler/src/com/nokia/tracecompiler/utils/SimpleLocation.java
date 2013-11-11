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
* Adapter for location interface
*
*/
package com.nokia.tracecompiler.utils;

import com.nokia.tracecompiler.source.SourceLocationInterface;

/**
 * Location interface implementation
 * 
 */
class SimpleLocation implements SourceLocationInterface {

	/**
	 * Location offset
	 */
	private int offset;

	/**
	 * Location length
	 */
	private int length;

	/**
	 * Deleted flag
	 */
	private boolean deleted;

	/**
	 * Constructor
	 * 
	 * @param offset
	 *            location offset
	 * @param length
	 *            location length
	 */
	SimpleLocation(int offset, int length) {
		this.offset = offset;
		this.length = length;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceLocationInterface#delete()
	 */
	public void delete() {
		deleted = true;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceLocationInterface#getLength()
	 */
	public int getLength() {
		return length;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceLocationInterface#getOffset()
	 */
	public int getOffset() {
		return offset;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceLocationInterface#isDeleted()
	 */
	public boolean isDeleted() {
		return deleted;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceLocationInterface#setLength(int)
	 */
	public void setLength(int length) {
		this.length = length;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceLocationInterface#setOffset(int)
	 */
	public void setOffset(int offset) {
		this.offset = offset;
	}

}
