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

/* CRSAVerifier */

EXPORT_C CRSAVerifier::CRSAVerifier(void)
	{
	}

EXPORT_C TBool CRSAVerifier::VerifyL(const TDesC8& aInput, 
	const CRSASignature& aSignature) const
	{
	TBool retval = EFalse;
	HBufC8* inverseSign = InverseSignLC(aSignature);
	
	if (inverseSign->Compare(aInput)==0)
		{
		retval = ETrue;
		}
	CleanupStack::PopAndDestroy(inverseSign);
	return retval;	
	}

/* CRSAPKCS1v15Verifier */
EXPORT_C CRSAPKCS1v15Verifier* CRSAPKCS1v15Verifier::NewL( 
	const CRSAPublicKey& aKey)
	{
	CRSAPKCS1v15Verifier* self = NewLC(aKey);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CRSAPKCS1v15Verifier* CRSAPKCS1v15Verifier::NewLC(
	const CRSAPublicKey& aKey)
	{
	CRSAPKCS1v15Verifier* self = new(ELeave) CRSAPKCS1v15Verifier(aKey);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

TInt CRSAPKCS1v15Verifier::MaxInputLength(void) const
	{
	return MaxOutputLength() - iPadding->MinPaddingLength();	
	}

TInt CRSAPKCS1v15Verifier::MaxOutputLength(void) const
	{
	return iPublicKey.N().ByteCount();
	}

HBufC8* CRSAPKCS1v15Verifier::InverseSignLC(
	const CRSASignature& aSignature) const
	{
	HBufC8* unpaddedBuf = HBufC8::NewMaxLC(MaxOutputLength());
	TPtr8 unpaddedHash = unpaddedBuf->Des();

	RInteger input = RInteger::NewL(aSignature.S());
	CleanupStack::PushL(input);
	RInteger output;

	RSAFunction::VerifyL(iPublicKey, input, output);
	CleanupStack::PushL(output);

	TPtrC8 paddedHashPtr = *(output.BufferLC());
	
	iPadding->UnPadL(paddedHashPtr, unpaddedHash);
	CleanupStack::PopAndDestroy(3, &input); //BufferLC, output, input
	return unpaddedBuf;
	}

CRSAPKCS1v15Verifier::CRSAPKCS1v15Verifier(const CRSAPublicKey& aKey)
	: iPublicKey(aKey)
	{
	}

void CRSAPKCS1v15Verifier::ConstructL(void)
	{
	iPadding = CPaddingPKCS1Signature::NewL(MaxOutputLength());

	// Check if MaxInputLength() makes sense, if not the key length must 
	// be too small
	if(MaxInputLength() <= 0)
		{
		User::Leave(KErrKeySize);
		}
	}

CRSAPKCS1v15Verifier::~CRSAPKCS1v15Verifier(void)
	{
	delete iPadding;
	}

