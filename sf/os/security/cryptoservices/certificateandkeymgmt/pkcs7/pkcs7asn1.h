/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef __PKCS7_ASN1_H__
#define __PKCS7_ASN1_H__

#include <e32base.h>

class TASN1DecGeneric;
/**
 * Decode a generic ASN1 sequence, optionally with a minimum/maximum number of top level terms.
 * @internalTechnology
 */
namespace PKCS7ASN1
{
	CArrayPtr<TASN1DecGeneric>* DecodeSequenceLC(const TDesC8& aRawData);
	CArrayPtr<TASN1DecGeneric>* DecodeSequenceLC(const TDesC8& aRawData,
														const TInt aMinTerms,
														const TInt aMaxTerms);
}
#endif
