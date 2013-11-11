/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Contains XML datatype definitions
*
*/









/**
 * DO NOT USE THIS API. DEPRECATED AND REMOVED IN S60 5.0. USE SYMBIAN XML FRAMEWORK INSTEAD.
 */














#ifndef __XMLUTILS_H
#define __XMLUTILS_H

#warning The XML API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information


// Error Codes
const TInt KErrDictionaryInitFailed=(-90);
const TInt KErrParseFailed=(-91);
enum TXMLErrorCodeSeverity
	{
		EXMLFatal,
		EXMLIndeterminate,
		EXMLWorkable,
		EXMLNone
	};

// Features
enum TXMLFeature
	{
		EXMLValidation,
		EXMLBinary
	};


#endif      // __XMLUTILS_H   
            
// End of File
