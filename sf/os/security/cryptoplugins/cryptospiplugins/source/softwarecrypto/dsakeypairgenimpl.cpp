/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* DSA Keypair implementation
* DSA keypair generation implementation
*
*/


/**
 @file
*/

#include "dsakeypairgenimpl.h"
#include "pluginconfig.h"
#include "keypair.h"
#include "common/inlines.h"    // For TClassSwap
#include "mont.h"
#include "sha1impl.h"
#include <random.h>
#include <securityerr.h>


const TUint KShaSize = 20;
const TUint KMinPrimeLength = 512;
const TUint KMaxPrimeLength = 1024;
const TUint KPrimeLengthMultiple = 64;

using namespace SoftwareCrypto;


/* CDSAPrimeCertificate */

CDSAPrimeCertificate* CDSAPrimeCertificate::NewL(const TDesC8& aSeed, TUint aCounter)
	{
	CDSAPrimeCertificate* self = NewLC(aSeed, aCounter);
	CleanupStack::Pop();
	return self;
	}

CDSAPrimeCertificate* CDSAPrimeCertificate::NewLC(const TDesC8& aSeed, TUint aCounter)
	{
	CDSAPrimeCertificate* self = new(ELeave) CDSAPrimeCertificate(aCounter);
	CleanupStack::PushL(self);
	self->ConstructL(aSeed);
	return self;
	}

const TDesC8& CDSAPrimeCertificate::Seed() const
	{
	return *iSeed;
	}

TUint CDSAPrimeCertificate::Counter() const
	{
	return iCounter;
	}

CDSAPrimeCertificate::~CDSAPrimeCertificate() 
	{
	delete const_cast<HBufC8*>(iSeed);
	}

void CDSAPrimeCertificate::ConstructL(const TDesC8& aSeed)
	{
	iSeed = aSeed.AllocL();
	}

CDSAPrimeCertificate::CDSAPrimeCertificate(TUint aCounter) 
	: iCounter(aCounter)
	{
	}

CDSAPrimeCertificate::CDSAPrimeCertificate() 
	{
	}


/* CDSAKeyPairGenImpl */
CDSAKeyPairGenImpl::CDSAKeyPairGenImpl()
	{
	}

CDSAKeyPairGenImpl::~CDSAKeyPairGenImpl()
	{
	delete iPrimeCertificate;
	}

CDSAKeyPairGenImpl* CDSAKeyPairGenImpl::NewL()
	{
	CDSAKeyPairGenImpl* self = CDSAKeyPairGenImpl::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CDSAKeyPairGenImpl* CDSAKeyPairGenImpl::NewLC()
	{
	CDSAKeyPairGenImpl* self = new(ELeave) CDSAKeyPairGenImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDSAKeyPairGenImpl::ConstructL(void)
	{
	CKeyPairGenImpl::ConstructL();
	}

CExtendedCharacteristics* CDSAKeyPairGenImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

const CExtendedCharacteristics* CDSAKeyPairGenImpl::GetExtendedCharacteristicsL()
	{
	return CDSAKeyPairGenImpl::CreateExtendedCharacteristicsL();
	}

TUid CDSAKeyPairGenImpl::ImplementationUid() const
	{
	return KCryptoPluginDsaKeyPairGenUid;
	}

void CDSAKeyPairGenImpl::Reset()
	{
	// does nothing in this plugin
	}

TBool CDSAKeyPairGenImpl::ValidPrimeLength(TUint aPrimeBits)
	{
	return (aPrimeBits >= KMinPrimeLength &&
			aPrimeBits <= KMaxPrimeLength &&
			aPrimeBits % KPrimeLengthMultiple == 0);
	}

TBool CDSAKeyPairGenImpl::GeneratePrimesL(const TDesC8& aSeed,
										 TUint& aCounter, 
										 RInteger& aP, 
										 TUint aL, 
										 RInteger& aQ, 
										 TBool aUseInputCounter)
	{
	//This follows the steps in FIPS 186-2 
	//See DSS Appendix 2.2
	//Note. Step 1 is performed prior to calling GeneratePrimesL, so that this
	//routine can be used for both generation and validation.
	//Step 1.  Choose an arbitrary sequence of at least 160 bits and call it
	//SEED.  Let g be the length of SEED in bits.

	if(!ValidPrimeLength(aL))
		{
		User::Leave(KErrNotSupported);
		}
	
	CSHA1Impl* sha1 = CSHA1Impl::NewL();
	CleanupStack::PushL(sha1);

	HBufC8* seedBuf = aSeed.AllocLC();
	TPtr8 seed = seedBuf->Des();
	TUint gBytes = aSeed.Size();
	
	//Note that the DSS's g = BytesToBits(gBytes) ie. the number of random bits
	//in the seed.  
	//This function has made the assumption (for ease of computation) that g%8
	//is 0.  Ie the seed is a whole number of random bytes.
	TBuf8<KShaSize> U; 
	TBuf8<KShaSize> temp; 
	const TUint n = (aL-1)/160;
	const TUint b = (aL-1)%160;
	HBufC8* Wbuf = HBufC8::NewMaxLC((n+1) * KShaSize);
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
	XorBuf(const_cast<TUint8*>(U.Ptr()), temp.Ptr(), KShaSize);

	//Step 3. Form q from U by setting the most significant bit (2^159)
	//and the least significant bit to 1.
	U[0] |= 0x80;
	U[KShaSize-1] |= 1;

	aQ = RInteger::NewL(U);
	CleanupStack::PushL(aQ);

	//Step 4. Use a robust primality testing algo to test if q is prime
	//The robust part is the calling codes problem.  This will use whatever
	//random number generator you set for the thread.  To attempt FIPS 186-2
	//compliance, set a FIPS 186-2 compliant RNG.
	if( !aQ.IsPrimeL() )
		{
		//Step 5. If not exit and get a new seed
		CleanupStack::PopAndDestroy(4, sha1);
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
				TPtr8 Wptr(W+(n-k)*KShaSize, gBytes);
				Wptr.Copy(sha1->Final(seed));
				}
			}
		if(!aUseInputCounter || counter == aCounter)
			{
			//Step 8. Let W be the integer...  and let X = W + 2^(L-1)
			const_cast<TUint8&>((*Wbuf)[KShaSize - 1 - b/8]) |= 0x80;
			TPtr8 Wptr(W + KShaSize - 1 - b/8, aL/8, aL/8);
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
				CleanupStack::Pop(2, &aQ);
				CleanupStack::PopAndDestroy(3, sha1);
				return ETrue;
				}
			CleanupStack::PopAndDestroy(&aP);
			}
		}
	CleanupStack::PopAndDestroy(4, &sha1);
	return EFalse;
	}

void CDSAKeyPairGenImpl::GenerateKeyPairL(TInt aKeySize, 
										const CCryptoParams& aKeyParameters,
										CKeyPair*& aKeyPair)
	{
	//This is the first step of DSA prime generation.  The remaining steps are
	//performed in CDSAParameters::GeneratePrimesL
	//Step 1.  Choose an arbitrary sequence of at least 160 bits and call it
	//SEED.  Let g be the length of SEED in bits.	
	TBuf8<KShaSize> seed(KShaSize);
	TUint c;
	RInteger p;
	RInteger q;
	
	do 
		{
	    TRAPD(err, GenerateRandomBytesL(seed));
	    if((err != KErrNone) && (err != KErrNotSecure))
	        User::Leave(err);
		}
	while(!GeneratePrimesL(seed, c, p, aKeySize, q));
	
	//Double PushL will not fail as GeneratePrimesL uses the CleanupStack
	//(at least one push and pop ;)
	CleanupStack::PushL(p);
	CleanupStack::PushL(q);

	iPrimeCertificate = CDSAPrimeCertificate::NewL(seed, c);
	
	// aKeyParameters isn't const here anymore
	CCryptoParams& paramRef=const_cast<CCryptoParams&>(aKeyParameters);
	paramRef.AddL(c, KDsaKeyGenerationCounterUid);
	paramRef.AddL(seed, KDsaKeyGenerationSeedUid);
	
	CMontgomeryStructure* montP = CMontgomeryStructure::NewLC(p);
	
	--p;

	// e = (p-1)/q
	RInteger e = p.DividedByL(q);
	CleanupStack::PushL(e);

	--p; //now it's p-2 :)

	RInteger h;
	const TInteger* g = 0;
	do
		{
		// find a random h | 1 < h < p-1
		h = RInteger::NewRandomL(TInteger::Two(), p);
		CleanupStack::PushL(h);
		// g = h^e mod p
		g = &(montP->ExponentiateL(h, e));
		CleanupStack::PopAndDestroy(&h); 
		}
	while( *g <= TInteger::One() );
	CleanupStack::PopAndDestroy(&e);

	++p; //reincrement p to original value
	++p;


	RInteger g1 = RInteger::NewL(*g); //take a copy of montP's g
	CleanupStack::PushL(g1);
	--q;
	// select random x | 0 < x < q
	RInteger x = RInteger::NewRandomL(TInteger::One(), q);
	CleanupStack::PushL(x);
	++q;

	//
	// create the keys parameters
	CCryptoParams* privateKeyParameters = CCryptoParams::NewLC();
	privateKeyParameters->AddL(p, KDsaKeyParameterPUid);
	privateKeyParameters->AddL(q, KDsaKeyParameterQUid);
	privateKeyParameters->AddL(g1, KDsaKeyParameterGUid);
	privateKeyParameters->AddL(x, KDsaKeyParameterXUid);
	TKeyProperty privateKeyProperties = {KDSAKeyPairGeneratorUid, 
										 KCryptoPluginDsaKeyPairGenUid,
									     KDsaPrivateKeyUid, 
									     KNonEmbeddedKeyUid};

	CCryptoParams* publicKeyParameters = CCryptoParams::NewLC();
	publicKeyParameters->AddL(p, KDsaKeyParameterPUid);
	publicKeyParameters->AddL(q, KDsaKeyParameterQUid);
	publicKeyParameters->AddL(g1, KDsaKeyParameterGUid);
	RInteger y = RInteger::NewL(montP->ExponentiateL(*g, x));
	CleanupStack::PushL(y);
	publicKeyParameters->AddL(y, KDsaKeyParameterYUid);
	TKeyProperty publicKeyProperties = {KDSAKeyPairGeneratorUid,
										KCryptoPluginDsaKeyPairGenUid, 
										KDsaPublicKeyUid,
										KNonEmbeddedKeyUid};

	//
	// create the private key
	//
	CKey* privateKey = CKey::NewL(privateKeyProperties, *privateKeyParameters);
	CleanupStack::PushL(privateKey);

	//
	// create the public key
	//
	CKey* publicKey = CKey::NewL(publicKeyProperties, *publicKeyParameters);
	CleanupStack::PushL(publicKey);

	aKeyPair = CKeyPair::NewL(publicKey, privateKey);

	//publicKey, publicKeyParameters, y, privateKey, privateKeyParameters, x, g1, montP, q, p
	CleanupStack::Pop(2, privateKey);
	CleanupStack::PopAndDestroy(8, &p);	
	}
