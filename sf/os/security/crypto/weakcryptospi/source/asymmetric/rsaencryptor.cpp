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


#include "rsashim.h"

EXPORT_C CRSAPKCS1v15Encryptor* CRSAPKCS1v15Encryptor::NewL(
	const CRSAPublicKey& aKey)
	{
	return CRSAPKCS1v15EncryptorShim::NewL(aKey);
	}

EXPORT_C CRSAPKCS1v15Encryptor* CRSAPKCS1v15Encryptor::NewLC(
	const CRSAPublicKey& aKey)
	{
	return CRSAPKCS1v15EncryptorShim::NewLC(aKey);
	}

CRSAPKCS1v15Encryptor::CRSAPKCS1v15Encryptor(const CRSAPublicKey& aKey)
	: iPublicKey(aKey)
	{
	}

CRSAPKCS1v15Encryptor::~CRSAPKCS1v15Encryptor(void)
	{
	}

// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
TInt CRSAPKCS1v15Encryptor::MaxInputLength(void) const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return 0;
	}

TInt CRSAPKCS1v15Encryptor::MaxOutputLength() const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return 0;
	}

void CRSAPKCS1v15Encryptor::EncryptL(const TDesC8& /*aInput*/, TDes8& /*aOutput*/) const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	}

void CRSAPKCS1v15Encryptor::ConstructL(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	}
