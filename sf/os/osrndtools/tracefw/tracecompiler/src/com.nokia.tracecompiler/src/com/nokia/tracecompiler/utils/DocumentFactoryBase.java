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
* Base class for document factories
*
*/
package com.nokia.tracecompiler.utils;

import com.nokia.tracecompiler.source.SourceDocumentFactory;
import com.nokia.tracecompiler.source.SourceDocumentInterface;
import com.nokia.tracecompiler.source.SourceLocationBase;
import com.nokia.tracecompiler.source.SourceLocationInterface;

/**
 * Base class for document factories
 * 
 */
public class DocumentFactoryBase implements SourceDocumentFactory {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceDocumentFactory#
	 *      createDocument(java.lang.String)
	 */
	public SourceDocumentInterface createDocument(String sourceData) {
		return new DocumentAdapter();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.source.SourceDocumentFactory#
	 *      createLocation(com.nokia.tracecompiler.source.SourceLocationBase,
	 *      int, int)
	 */
	public SourceLocationInterface createLocation(SourceLocationBase base,
			int offset, int length) {
		return new SimpleLocation(offset, length);
	}

}
