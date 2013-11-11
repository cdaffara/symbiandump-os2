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
#include <securityerr.h>
#include "rsafunction.h"

/* CRSASigner */

EXPORT_C CRSASigner::CRSASigner(void)
	{
	}

/* CRSAPKCS1v15Signer */
EXPORT_C CRSAPKCS1v15Signer* CRSAPKCS1v15Signer::NewL(
	const CRSAPrivateKey& aKey)
	{
	CRSAPKCS1v15Signer* self = NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CRSAPKCS1v15Signer* CRSAPKCS1v15Signer::NewLC(
	const CRSAPrivateKey& aKey)
	{
	CRSAPKCS1v15Signer* self = new(ELeave) CRSAPKCS1v15Signer(aKey);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

TInt CRSAPKCS1v15Signer::MaxInputLength(void) const
	{
	return MaxOutputLength() - iPadding->MinPaddingLength();	
	}

TInt CRSAPKCS1v15Signer::MaxOutputLength(void) const
	{
	return iPrivateKey.N().ByteCount();	
	}

CRSASignature* CRSAPKCS1v15Signer::SignL(const TDesC8& aInput) const
	{
	HBufC8* buf = HBufC8::NewMaxLC(MaxOutputLength());
	TPtr8 ptr = buf->Des();

	ptr.SetLength(aInput.Length());

	//The following will panic if aInput is larger than MaxOutputLength() It is
	//likely that the caller has passed in something that has not been hashed.
	//This is a programming, and likely a security error, in client code, not a
	//problem here.
	iPadding->PadL(aInput, ptr);

	RInteger input = RInteger::NewL(ptr);
	CleanupStack::PushL(input);
	RInteger output;

	RSAFunction::SignL(iPrivateKey, input, output);
	CleanupStack::PushL(output);

	CRSASignature* signature = CRSASignature::NewL(output);
	CleanupStack::Pop(&output); //output, now owned by CRSASignature
	CleanupStack::PopAndDestroy(2, buf); //input, buf
	return signature;
	}

CRSAPKCS1v15Signer::CRSAPKCS1v15Signer(const CRSAPrivateKey& aKey)  
	: iPrivateKey(aKey)
	{
	}

void CRSAPKCS1v15Signer::ConstructL(void)  
	{
	iPadding = CPaddingPKCS1Signature::NewL(MaxOutputLength());

	// Check if MaxInputLength() makes sense, if not the key length must 
	// be too small
	if(MaxInputLength() <= 0)
		{
		User::Leave(KErrKeySize);
		}
	}

CRSAPKCS1v15Signer::~CRSAPKCS1v15Signer(void)  
	{
	delete iPadding;
	}
