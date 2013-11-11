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


#include <asymmetrickeys.h>
#include <bigint.h>
#include <random.h>
#include <hash.h>
#include "../common/inlines.h"
#include "../bigint/mont.h"
#include "dsakeypairshim.h"

const TUint SHASIZE = 20;
const TUint KMinPrimeLength = 512;
const TUint KMaxPrimeLength = 1024;
const TUint KPrimeLengthMultiple = 64;

/* CDSAParameters */

EXPORT_C const TInteger& CDSAParameters::P(void) const
	{
	return iP;
	}

EXPORT_C const TInteger& CDSAParameters::Q(void) const
	{
	return iQ;
	}

EXPORT_C const TInteger& CDSAParameters::G(void) const
	{
	return iG;
	}

EXPORT_C CDSAParameters::~CDSAParameters(void)
	{
	iP.Close();
	iQ.Close();
	iG.Close();
	}

EXPORT_C CDSAParameters* CDSAParameters::NewL(RInteger& aP, RInteger& aQ, 
	RInteger& aG)
	{
	CDSAParameters* me = new (ELeave) CDSAParameters(aP, aQ, aG);
	return (me);
	}

EXPORT_C TBool CDSAParameters::ValidatePrimesL(const CDSAPrimeCertificate& aCert)
	const
	{
	TBool result = EFalse;
	RInteger p;
	RInteger q;
	//Regenerate primes using aCert's seed and counter
	TUint counter = aCert.Counter();
	if(!CDSAParameters::GeneratePrimesL(aCert.Seed(), counter, p, 
		P().BitCount(), q, ETrue))
		{
		return result;
		}
	//this doesn't leave, no need to push p and q
	if(p == P() && q == Q() && counter == aCert.Counter())
		{
		result = ETrue;
		}
	p.Close();
	q.Close();
	return result;
	}

EXPORT_C TBool CDSAParameters::ValidPrimeLength(TUint aPrimeBits)
	{
	return (aPrimeBits >= KMinPrimeLength &&
		aPrimeBits <= KMaxPrimeLength &&
		aPrimeBits % KPrimeLengthMultiple == 0);
	}

EXPORT_C CDSAParameters::CDSAParameters(RInteger& aP, RInteger& aQ, 	
	RInteger& aG) : iP(aP), iQ(aQ), iG(aG)
	{
	}

EXPORT_C CDSAParameters::CDSAParameters(void)
	{
	}

TBool CDSAParameters::GeneratePrimesL(const TDesC8& aSeed, TUint& aCounter, 
	RInteger& aP, TUint aL, RInteger& aQ, TBool aUseInputCounter)
	{
	//This follows the steps in FIPS 186-2 
	//See DSS Appendix 2.2
	//Note. Step 1 is performed prior to calling GeneratePrimesL, so that this
	//routine can be used for both generation and validation.
	//Step 1.  Choose an arbitrary sequence of at least 160 bits and call it
	//SEED.  Let g be the length of SEED in bits.

	if(!CDSAParameters::ValidPrimeLength(aL))
		{
		User::Leave(KErrNotSupported);
		}
	
	CSHA1* sha1 = CSHA1::NewL();
	CleanupStack::PushL(sha1);

	HBufC8* seedBuf = aSeed.AllocLC();
	TPtr8 seed = seedBuf->Des();
	TUint gBytes = aSeed.Size();
	//Note that the DSS's g = BytesToBits(gBytes) ie. the number of random bits
	//in the seed.  
	//This function has made the assumption (for ease of computation) that g%8
	//is 0.  Ie the seed is a whole number of random bytes.
	TBuf8<SHASIZE> U; 
	TBuf8<SHASIZE> temp; 
	const TUint n = (aL-1)/160;
	const TUint b = (aL-1)%160;
	HBufC8* Wbuf = HBufC8::NewMaxLC((n+1) * SHASIZE);
	TUint8* W = const_cast<TUint8*>(Wbuf->Ptr());

	U.Copy(sha1->Final(seed));
	
	//Step 2. U = SHA-1[SEED] XOR SHA-1[(SEED+1) mod 2^g]
	for(TInt i=gBytes - 1, carry=ETrue; i>=0 && carry; i--)
		{
		//!++(TUint) adds one to the current word which if it overflows to zero
		//sets carry to 1 thus letting the loop continue.  It's a poor man's
		//multi-word addition.  Swift eh?
		carry = !++(seed[i]);
		}

	temp.Copy(sha1->Final(seed));
	XorBuf(const_cast<TUint8*>(U.Ptr()), temp.Ptr(), SHASIZE);

	//Step 3. Form q from U by setting the most significant bit (2^159)
	//and the least significant bit to 1.
	U[0] |= 0x80;
	U[SHASIZE-1] |= 1;

	aQ = RInteger::NewL(U);
	CleanupStack::PushL(aQ);

	//Step 4. Use a robust primality testing algo to test if q is prime
	//The robust part is the calling codes problem.  This will use whatever
	//random number generator you set for the thread.  To attempt FIPS 186-2
	//compliance, set a FIPS 186-2 compliant RNG.
	if( !aQ.IsPrimeL() )
		{
		//Step 5. If not exit and get a new seed
		CleanupStack::PopAndDestroy(&aQ);
		CleanupStack::PopAndDestroy(Wbuf);
		CleanupStack::PopAndDestroy(seedBuf);
		CleanupStack::PopAndDestroy(sha1);
		return EFalse;
		}
	
	TUint counterEnd = aUseInputCounter ? aCounter+1 : 4096;
	
	//Step 6. Let counter = 0 and offset = 2
	//Note 1. that the DSS speaks of SEED + offset + k because they always
	//refer to a constant SEED.  We update our seed as we go so the offset
	//variable has already been added to seed in the previous iterations.
	//Note 2. We've already added 1 to our seed, so the first time through this
	//the offset in DSS speak will be 2.
	for(TUint counter=0; counter < counterEnd; counter++)
		{
		//Step 7. For k=0, ..., n let
		// Vk = SHA-1[(SEED + offset + k) mod 2^g]
		//I'm storing the Vk's inside of a big W buffer.
		for(TUint k=0; k<=n; k++)
			{
			for(TInt i=gBytes-1, carry=ETrue; i>=0 && carry; i--)
				{
				carry = !++(seed[i]);
				}
			if(!aUseInputCounter || counter == aCounter)
				{
				TPtr8 Wptr(W+(n-k)*SHASIZE, gBytes);
				Wptr.Copy(sha1->Final(seed));
				}
			}
		if(!aUseInputCounter || counter == aCounter)
			{
			//Step 8. Let W be the integer...  and let X = W + 2^(L-1)
			const_cast<TUint8&>((*Wbuf)[SHASIZE - 1 - b/8]) |= 0x80;
			TPtr8 Wptr(W + SHASIZE - 1 - b/8, aL/8, aL/8);
			RInteger X = RInteger::NewL(Wptr);
			CleanupStack::PushL(X);
			//Step 9. Let c = X mod 2q and set p = X - (c-1)
			RInteger twoQ = aQ.TimesL(TInteger::Two());
			CleanupStack::PushL(twoQ);
			RInteger c = X.ModuloL(twoQ);
			CleanupStack::PushL(c);
			--c;
			aP = X.MinusL(c);
			CleanupStack::PopAndDestroy(3, &X); //twoQ, c, X
			CleanupStack::PushL(aP);
			
			//Step 10 and 11: if p >= 2^(L-1) and p is prime
			if( aP.Bit(aL-1) && aP.IsPrimeL() )
				{
				aCounter = counter;
				CleanupStack::Pop(&aP);
				CleanupStack::Pop(&aQ);
				CleanupStack::PopAndDestroy(Wbuf);
				CleanupStack::PopAndDestroy(seedBuf);
				CleanupStack::PopAndDestroy(sha1);
				return ETrue;
				}
			CleanupStack::PopAndDestroy(&aP);
			}
		}
	CleanupStack::PopAndDestroy(&aQ);
	CleanupStack::PopAndDestroy(Wbuf);
	CleanupStack::PopAndDestroy(seedBuf);
	CleanupStack::PopAndDestroy(sha1);
	return EFalse;
	}

/* CDSAPublicKey */

EXPORT_C CDSAPublicKey* CDSAPublicKey::NewL(RInteger& aP, RInteger& aQ, 
	RInteger& aG, RInteger& aY)
	{
	CDSAPublicKey* self = new(ELeave) CDSAPublicKey(aP, aQ, aG, aY);
	return self;
	}

EXPORT_C CDSAPublicKey* CDSAPublicKey::NewLC(RInteger& aP, RInteger& aQ, 
	RInteger& aG, RInteger& aY)
	{
	CDSAPublicKey* self = NewL(aP, aQ, aG, aY);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C const TInteger& CDSAPublicKey::Y(void) const
	{
	return iY;
	}

EXPORT_C CDSAPublicKey::CDSAPublicKey(void)
	{
	} 

EXPORT_C CDSAPublicKey::CDSAPublicKey(RInteger& aP, RInteger& aQ, RInteger& aG, 
	RInteger& aY) : CDSAParameters(aP, aQ, aG), iY(aY)
	{
	}

EXPORT_C CDSAPublicKey::~CDSAPublicKey(void)
	{
	iY.Close();
	}

/* CDSAPrivateKey */

EXPORT_C CDSAPrivateKey* CDSAPrivateKey::NewL(RInteger& aP, RInteger& aQ, 
	RInteger& aG, RInteger& aX)
	{
	CDSAPrivateKey* self = new(ELeave) CDSAPrivateKey(aP, aQ, aG, aX);
	return self;
	}

EXPORT_C CDSAPrivateKey* CDSAPrivateKey::NewLC(RInteger& aP, RInteger& aQ, 
	RInteger& aG, RInteger& aX)
	{
	CDSAPrivateKey* self = NewL(aP, aQ, aG, aX);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C const TInteger& CDSAPrivateKey::X(void) const
	{
	return iX;
	}

EXPORT_C CDSAPrivateKey::CDSAPrivateKey(RInteger& aP, RInteger& aQ, RInteger& aG, 
	RInteger& aX) : CDSAParameters(aP, aQ, aG), iX(aX)
	{
	}

EXPORT_C CDSAPrivateKey::~CDSAPrivateKey(void)
	{
	iX.Close();
	}

/* CDSAKeyPair */

EXPORT_C CDSAKeyPair* CDSAKeyPair::NewL(TUint aKeyBits)
	{
 	CDSAKeyPairShim* self = CDSAKeyPairShim::NewLC(aKeyBits);
 	CleanupStack::Pop();
 	return self;
	}

EXPORT_C CDSAKeyPair* CDSAKeyPair::NewLC(TUint aKeyBits)
	{
 	CDSAKeyPairShim* self = CDSAKeyPairShim::NewLC(aKeyBits);
 	return self;
	}

EXPORT_C const CDSAPublicKey& CDSAKeyPair::PublicKey(void) const
	{
	return *iPublic;
	}
	
EXPORT_C const CDSAPrivateKey& CDSAKeyPair::PrivateKey(void) const
	{
	return *iPrivate;
	}

EXPORT_C CDSAKeyPair::~CDSAKeyPair(void) 
	{
	delete iPublic;
	delete iPrivate;
	delete iPrimeCertificate;
	}

EXPORT_C CDSAKeyPair::CDSAKeyPair(void) 
	{
	}

EXPORT_C const CDSAPrimeCertificate& CDSAKeyPair::PrimeCertificate(void) const
	{
	return *iPrimeCertificate;
	}

/* CDSAPrimeCertificate */

EXPORT_C CDSAPrimeCertificate* CDSAPrimeCertificate::NewL(const TDesC8& aSeed,
	TUint aCounter)
	{
	CDSAPrimeCertificate* self = NewLC(aSeed, aCounter);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CDSAPrimeCertificate* CDSAPrimeCertificate::NewLC(const TDesC8& aSeed,
	TUint aCounter)
	{
	CDSAPrimeCertificate* self = new(ELeave) CDSAPrimeCertificate(aCounter);
	CleanupStack::PushL(self);
	self->ConstructL(aSeed);
	return self;
	}

EXPORT_C const TDesC8& CDSAPrimeCertificate::Seed(void) const
	{
	return *iSeed;
	}

EXPORT_C TUint CDSAPrimeCertificate::Counter(void) const
	{
	return iCounter;
	}

EXPORT_C CDSAPrimeCertificate::~CDSAPrimeCertificate(void) 
	{
	delete const_cast<HBufC8*>(iSeed);
	}

void CDSAPrimeCertificate::ConstructL(const TDesC8& aSeed)
	{
	iSeed = aSeed.AllocL();
	}

EXPORT_C CDSAPrimeCertificate::CDSAPrimeCertificate(TUint aCounter) 
	: iCounter(aCounter)
	{
	}

// Over taken by  shim version. so, exclude it from coverage.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
void CDSAKeyPair::ConstructL(TUint /*aPBits*/)
	{
	}

// Unused exported and protected method can be excluded from coverage.
EXPORT_C CDSAPrimeCertificate::CDSAPrimeCertificate(void) 
	{
	}

EXPORT_C CDSAPrivateKey::CDSAPrivateKey(void)
	{
	}
