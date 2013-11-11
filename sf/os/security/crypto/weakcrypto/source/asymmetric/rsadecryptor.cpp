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


#include <asymmetric.h>
#include <asymmetrickeys.h>
#include <bigint.h>
#include <cryptostrength.h>
#include <securityerr.h>
#include <cryptopanic.h>
#include "rsafunction.h"

/* CRSAPKCS1v15Decryptor */

EXPORT_C CRSAPKCS1v15Decryptor* CRSAPKCS1v15Decryptor::NewL(
	const CRSAPrivateKey& aKey)
	{
	CRSAPKCS1v15Decryptor* self = NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CRSAPKCS1v15Decryptor* CRSAPKCS1v15Decryptor::NewLC(
	const CRSAPrivateKey& aKey)
	{
	CRSAPKCS1v15Decryptor* self = new(ELeave) CRSAPKCS1v15Decryptor(aKey);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

TInt CRSAPKCS1v15Decryptor::MaxInputLength(void) const
	{
	return iPrivateKey.N().ByteCount();
	}

TInt CRSAPKCS1v15Decryptor::MaxOutputLength(void) const
	{
	return MaxInputLength() - iPadding->MinPaddingLength();
	}

void CRSAPKCS1v15Decryptor::DecryptL(const TDesC8& aInput, 
	TDes8& aOutput)const
	{
	__ASSERT_DEBUG(aOutput.MaxLength() >= MaxOutputLength(), User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));
	__ASSERT_DEBUG(aInput.Length() <= MaxInputLength(), User::Panic(KCryptoPanic, ECryptoPanicInputTooLarge));

	RInteger input = RInteger::NewL(aInput);
	CleanupStack::PushL(input);

	RInteger output;

	RSAFunction::DecryptL(iPrivateKey, input, output);
	CleanupStack::PushL(output);

    TPtrC8 ptr = *(output.BufferLC());
    iPadding->UnPadL(ptr, aOutput);

    CleanupStack::PopAndDestroy(3, &input); //BufferLC(), output, input
	}

CRSAPKCS1v15Decryptor::CRSAPKCS1v15Decryptor(const CRSAPrivateKey& aKey)  
	: iPrivateKey(aKey)
	{
	}

void CRSAPKCS1v15Decryptor::ConstructL(void)  
	{
	iPadding = CPaddingPKCS1Encryption::NewL(MaxInputLength());

	// Check if MaxInputLength() makes sense, if not the key length must 
	// be too small
	if(MaxOutputLength() <= 0)
		{
		User::Leave(KErrKeySize);
		}

	TCrypto::IsAsymmetricWeakEnoughL(iPrivateKey.N().BitCount());
	}

CRSAPKCS1v15Decryptor::~CRSAPKCS1v15Decryptor(void)  
	{
	delete iPadding;
	}
