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

EXPORT_C CDSAVerifier* CDSAVerifier::NewL(const CDSAPublicKey& aKey)
	{
	CDSAVerifier* self = new(ELeave)CDSAVerifier(aKey);
	return self;
	}

EXPORT_C CDSAVerifier* CDSAVerifier::NewLC(const CDSAPublicKey& aKey)
	{
	CDSAVerifier* self = NewL(aKey);
	CleanupStack::PushL(self);
	return self;
	}

TInt CDSAVerifier::MaxInputLength(void) const
	{
	// return CSHA1::DIGESTBYTES
	return 160;
	}

TBool CDSAVerifier::VerifyL(const TDesC8& aInput, 
	const CDSASignature& aSignature) const
	{
	//see HAC 11.56 or DSS section 6
	//I'll follow HAC as I like the description better

	// a) Obtain A's authenticate public key

	// b) Verify that 0 < r < q and 0 < s < q; if not reject signature
	if (aSignature.R() <= 0 || aSignature.R() >= iPublicKey.Q())
		{
		return EFalse;
		}
	if (aSignature.S() <= 0 || aSignature.S() >= iPublicKey.Q())
		{
		return EFalse;
		}

	TBool result = EFalse;

	// c) Compute w = s^(-1) mod q and h(m)
	RInteger w = aSignature.S().InverseModL(iPublicKey.Q());
	CleanupStack::PushL(w);
	// Note that in order to be interoperable, compliant with the DSS, and
	// secure, aInput must be the result of a SHA-1 hash
	RInteger hm = RInteger::NewL(aInput);
	CleanupStack::PushL(hm);

	// d) Compute u1 = w * hm mod q and u2 = r * w mod q
	RInteger u1 = TInteger::ModularMultiplyL(w, hm, iPublicKey.Q());
	CleanupStack::PushL(u1);

	RInteger u2 = TInteger::ModularMultiplyL(aSignature.R(), w, iPublicKey.Q());
	CleanupStack::PushL(u2);

	// e) Compute v = ((g^u1 * y^u2) mod p) mod q
	RInteger temp = TInteger::ModularExponentiateL(iPublicKey.G(), u1,
		iPublicKey.P());
	CleanupStack::PushL(temp);
	RInteger temp1 = TInteger::ModularExponentiateL(iPublicKey.Y(), u2,
		iPublicKey.P());
	CleanupStack::PushL(temp1);
	RInteger v = TInteger::ModularMultiplyL(temp, temp1, iPublicKey.P());
	CleanupStack::PushL(v);
	v %= iPublicKey.Q();

	// f) Accept the signature iff v == r
	if(v == aSignature.R())
		{
		result = ETrue;
		}

	CleanupStack::PopAndDestroy(&v);
	CleanupStack::PopAndDestroy(&temp1);
	CleanupStack::PopAndDestroy(&temp);
	CleanupStack::PopAndDestroy(&u2);
	CleanupStack::PopAndDestroy(&u1);
	CleanupStack::PopAndDestroy(&hm);
	CleanupStack::PopAndDestroy(&w); 

	return result;	
	}

CDSAVerifier::CDSAVerifier(const CDSAPublicKey& aKey)  
	: iPublicKey(aKey)
	{
	}

