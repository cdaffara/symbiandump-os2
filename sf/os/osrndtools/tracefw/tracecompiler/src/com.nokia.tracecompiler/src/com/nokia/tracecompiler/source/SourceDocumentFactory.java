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
* Interface to be implemented by document framework
*
*/
package com.nokia.tracecompiler.source;

/**
 * Interface to be implemented by document framework
 * 
 */
public interface SourceDocumentFactory {

	/**
	 * Creates a new location
	 * 
	 * @param base
	 *            the location which owns the position
	 * @param offset
	 *            the position offset
	 * @param length
	 *            the position length
	 * @return the location
	 */
	public SourceLocationInterface createLocation(SourceLocationBase base,
			int offset, int length);

	/**
	 * Creates a new document
	 * 
	 * @param sourceData
	 *            the document data
	 * @return the document
	 */
	public SourceDocumentInterface createDocument(String sourceData);

}
