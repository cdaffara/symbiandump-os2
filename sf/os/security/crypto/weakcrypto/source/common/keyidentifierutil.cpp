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


#include "keyidentifierutil.h"
#include "hash.h"
#include "asymmetrickeys.h"

EXPORT_C void KeyIdentifierUtil::RSAKeyIdentifierL(
		const CRSAPublicKey& aKey, TKeyIdentifier& aIdentifier)
	{
	aIdentifier.FillZ();

	// Generate a hash of the appropriate data (for TKeyIdentifier)	
	const TInteger& keyModulus = aKey.N();
	HBufC8* modulusData = keyModulus.BufferLC();
	CSHA1* sha1 = CSHA1::NewL();
	CleanupStack::PushL(sha1);
	TPtrC8 hash = sha1->Final(*modulusData);
	aIdentifier.Copy(hash);
	CleanupStack::PopAndDestroy(2);	//	sha1, modulusData
	}

EXPORT_C void KeyIdentifierUtil::DSAKeyIdentifierL(
		const CDSAPublicKey& aKey, TKeyIdentifier& aIdentifier)
	{
	aIdentifier.FillZ();

	// Generate a hash of the appropriate data (for TKeyIdentifier)	
	const TInteger& Y = aKey.Y();
	HBufC8* YData = Y.BufferLC();					
	CSHA1* sha1 = CSHA1::NewL();
	CleanupStack::PushL(sha1);
	TPtrC8 hash = sha1->Final(*YData);
	aIdentifier.Copy(hash);
	CleanupStack::PopAndDestroy(2);	//	sha1, YData				
	}

EXPORT_C void KeyIdentifierUtil::DHKeyIdentifierL(
		const RInteger& aKey, TKeyIdentifier& aIdentifier)
	{
	if (aKey.IsZero())
		User::Leave(KErrArgument);

	aIdentifier.FillZ();

	// Generate a hash of the appropriate data (for TKeyIdentifier)	
	HBufC8* XData = aKey.BufferLC();								
	CSHA1* sha1 = CSHA1::NewL();
	CleanupStack::PushL(sha1);
	TPtrC8 hash = sha1->Final(*XData);
	aIdentifier.Copy(hash);
	CleanupStack::PopAndDestroy(2);	//	sha1, XData
	}
