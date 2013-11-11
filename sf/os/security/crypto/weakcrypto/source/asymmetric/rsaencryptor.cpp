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
#include <padding.h>
#include <cryptostrength.h>
#include <securityerr.h>
#include <cryptopanic.h>
#include "rsafunction.h"


EXPORT_C CRSAPKCS1v15Encryptor* CRSAPKCS1v15Encryptor::NewL( 
	const CRSAPublicKey& aKey)
	{
	CRSAPKCS1v15Encryptor* self = NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CRSAPKCS1v15Encryptor* CRSAPKCS1v15Encryptor::NewLC(
	const CRSAPublicKey& aKey)
	{
	CRSAPKCS1v15Encryptor* self = new(ELeave) CRSAPKCS1v15Encryptor(aKey);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

TInt CRSAPKCS1v15Encryptor::MaxInputLength(void) const
	{
	return MaxOutputLength() - iPadding->MinPaddingLength();
	}

TInt CRSAPKCS1v15Encryptor::MaxOutputLength() const
	{
	return iPublicKey.N().ByteCount();	
	}

void CRSAPKCS1v15Encryptor::EncryptL(const TDesC8& aInput, TDes8& aOutput)const
	{
	__ASSERT_DEBUG(aOutput.MaxLength() >= MaxOutputLength(), User::Panic(KCryptoPanic, ECryptoPanicOutputDescriptorOverflow));
	__ASSERT_DEBUG(aInput.Length() <= MaxInputLength(), User::Panic(KCryptoPanic, ECryptoPanicInputTooLarge));

	HBufC8* buf = HBufC8::NewLC(MaxOutputLength());
	TPtr8 ptr = buf->Des();
	
	iPadding->DoPadL(aInput, ptr);
	RInteger input = RInteger::NewL(ptr);
	CleanupStack::PushL(input);
	
	RInteger output;
	RSAFunction::EncryptL(iPublicKey, input, output);
	CleanupStack::PushL(output);

	aOutput.Append(*(output.BufferLC()));
	CleanupStack::PopAndDestroy(4, buf); //BufferLC, output, input, buf
	}

CRSAPKCS1v15Encryptor::CRSAPKCS1v15Encryptor(const CRSAPublicKey& aKey) 
	: iPublicKey(aKey)
	{
	}

void CRSAPKCS1v15Encryptor::ConstructL(void)
	{
	iPadding = CPaddingPKCS1Encryption::NewL(MaxOutputLength());

	// Check if MaxInputLength() makes sense, if not the key length must 
	// be too small
	if(MaxInputLength() <= 0)
		{
		User::Leave(KErrKeySize);
		}
	TCrypto::IsAsymmetricWeakEnoughL(iPublicKey.N().BitCount());
	}

CRSAPKCS1v15Encryptor::~CRSAPKCS1v15Encryptor(void)
	{
	delete iPadding;
	}
