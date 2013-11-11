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

EXPORT_C CDSASigner* CDSASigner::NewL(const CDSAPrivateKey& aKey)
	{
	CDSASigner* self = new(ELeave) CDSASigner(aKey);
	return self;
	}

EXPORT_C CDSASigner* CDSASigner::NewLC(const CDSAPrivateKey& aKey) 
	{
	CDSASigner* self = NewL(aKey); 
	CleanupStack::PushL(self);
	return self;
	}

TInt CDSASigner::MaxInputLength(void) const
	{
	return SHA1_HASH;  // from hash.h
	}

CDSASignature* CDSASigner::SignL(const TDesC8& aInput) const
	{
	//see HAC 11.56 or DSS section 5
	//I'll follow HAC as I like its description better
	//We don't check that r and s are non both non-null like the DSS
	//states you _optionally_ can.  The chances of this are _incredibly_ small.
	//You've got a much better chance of a bit failure ocurring in the hardware
	//than this.

	// a) Select a random secret integer (k | 0 < k < q)
	RInteger qminus1 = RInteger::NewL(iPrivateKey.Q());
	CleanupStack::PushL(qminus1);
	--qminus1;
	RInteger k = RInteger::NewRandomL(TInteger::One(), qminus1);
	CleanupStack::PopAndDestroy(&qminus1);
	CleanupStack::PushL(k);

	// b) compute r = (g^k mod p) mod q
	RInteger r = TInteger::ModularExponentiateL(iPrivateKey.G(), k, 	
		iPrivateKey.P());
	CleanupStack::PushL(r);
	r %= iPrivateKey.Q();

	// c) compute k^(-1) mod q 
	RInteger kinv = k.InverseModL(iPrivateKey.Q());
	CleanupStack::PushL(kinv);

	// d) compute s = k^(-1) * {h(m) + xr} mod q
	// Note that in order to be interoperable, compliant with the DSS, and
	// secure, aInput must be the result of a SHA-1 hash
	RInteger hm = RInteger::NewL(aInput);
	CleanupStack::PushL(hm);
	
	RInteger s = iPrivateKey.X().TimesL(r);
	CleanupStack::PushL(s);
	s += hm;
	s *= kinv;
	s %= iPrivateKey.Q();

	// e) signature for m is the pair (r,s)
	CDSASignature* signature = CDSASignature::NewL(r, s);//transfers ownership
	CleanupStack::Pop(&s);
	CleanupStack::PopAndDestroy(&hm);
	CleanupStack::PopAndDestroy(&kinv);
	CleanupStack::Pop(&r);
	CleanupStack::PopAndDestroy(&k);	
	return signature;
	}

CDSASigner::CDSASigner(const CDSAPrivateKey& aKey) 
	: iPrivateKey(aKey)
	{
	}

