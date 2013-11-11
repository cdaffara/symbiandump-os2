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
* Document factory for file-based documents
*
*/
package com.nokia.tracecompiler.document;

import com.nokia.tracecompiler.source.SourceDocumentInterface;
import com.nokia.tracecompiler.utils.DocumentFactoryBase;

/**
 * Document factory for string-based documents and simple locations which do not
 * support changes or selections.
 * 
 */
public final class StringDocumentFactory extends DocumentFactoryBase {

	/*
	 * (non-Javadoc)
	 * 
	 * @see com.nokia.tracecompiler.utils.DocumentFactoryBase#createDocument(java.lang.String)
	 */
	@Override
	public SourceDocumentInterface createDocument(String sourceData) {
		return new StringDocument(sourceData);
	}

}
