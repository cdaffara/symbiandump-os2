/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef CMSDEFS_H
#define CMSDEFS_H

#include <e32base.h>

/** CMS-1 data */
_LIT(KCmsDataOID, "1.2.840.113549.1.7.1");

/** CMS-2 SignedData */
_LIT(KCmsSignedDataOID, "1.2.840.113549.1.7.2"); 

/** CMS-3 EnvelopedData */
_LIT(KCmsEnvelopedDataOID, "1.2.840.113549.1.7.3"); 

/** CMS-5 DigestedData */
_LIT(KCmsDigestedDataOID, "1.2.840.113549.1.7.5");   

/** CMS-6 EncryptedData */
_LIT(KCmsEncryptedDataOID, "1.2.840.113549.1.7.6"); 

/** CMS AuthenticatedData */
_LIT(KCmsAuthenticatedDataOID, "1.2.840.113549.1.9.16.1.2"); 


/**
Cms Content Info Types
*/
enum TCmsContentInfoType
	{
	/**
	Data Type
	*/
	EContentTypeData = 1,
	/**
	Signed Data Type
	*/
	EContentTypeSignedData =2,
	/**
	Enveloped Data Type
	*/
	EContentTypeEnvelopedData = 3,
	/**
	Digested Data Type
	*/
	EContentTypeDigestedData = 5,
	/**
	Encrypted Data Type
	*/
	EContentTypeEncryptedData =6,	
	/**
	Authenticated Data Type
	*/
	EContentTypeAuthenticatedData =7
	};

/**
CMS version definition
*/	
enum TCmsVersion
	{
	/**
	CMS version 0
	*/
	EVersion_0 = 0,
	/**
	CMS version 1
	*/
	EVersion_1 = 1,
	/**
	CMS version 2
	*/		
	EVersion_2 = 2,
	/**
	CMS version 3
	*/
	EVersion_3 = 3,
	/**
	CMS version 4
	*/		
	EVersion_4 = 4,	
	};
	

#endif // CMSDEFS_H

