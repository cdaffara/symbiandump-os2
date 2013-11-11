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
#include "../common/inlines.h"

const TUint KFermat4 = 65537;

/* CRSAParameters */

EXPORT_C const TInteger& CRSAParameters::N(void) const
	{
	return iN;
	}

EXPORT_C CRSAParameters::~CRSAParameters(void)
	{
	iN.Close();
	}

EXPORT_C CRSAParameters::CRSAParameters(RInteger& aN) : iN(aN)
	{
	}

EXPORT_C CRSAParameters::CRSAParameters(void)
	{
	}

/* CRSAPublicKey */

EXPORT_C CRSAPublicKey* CRSAPublicKey::NewL(RInteger& aN, RInteger& aE)
	{
	CRSAPublicKey* self = NewLC(aN, aE);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CRSAPublicKey* CRSAPublicKey::NewLC(RInteger& aN, RInteger& aE)
	{
	CRSAPublicKey* self = new(ELeave) CRSAPublicKey(aN, aE);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


void CRSAPublicKey::ConstructL()
	{ 
	// Check that the modulus and exponent are positive integers 
	// as specified by RSA
	if(!N().IsPositive() || !E().IsPositive() || (E() <= 1))
		{
		// If we need to leave during construction we must release ownership
		// of the RInteger parameters that were passed in.
		// These parameters should be on the cleanup stack so if we don't 
		// release ownership they will be deleted twice, causing a panic
		iN = RInteger();
		iE = RInteger();
		User::Leave(KErrArgument);
		}
	}


EXPORT_C const TInteger& CRSAPublicKey::E(void) const
	{
	return iE;
	}

EXPORT_C CRSAPublicKey::CRSAPublicKey()
	{
	}

EXPORT_C CRSAPublicKey::CRSAPublicKey(RInteger& aN, RInteger& aE)
	: CRSAParameters(aN), iE(aE)
	{
	}

EXPORT_C CRSAPublicKey::~CRSAPublicKey(void)
	{
	iE.Close();
	}

/* CRSAPrivateKeyType */

CRSAPrivateKey::CRSAPrivateKey(const TRSAPrivateKeyType aKeyType, RInteger& aN)
:	CRSAParameters(aN), iKeyType(aKeyType)
{}


/* CRSAPrivateKeyStandard */

EXPORT_C CRSAPrivateKeyStandard* CRSAPrivateKeyStandard::NewL(RInteger& aN, 
	RInteger& aD)
	{
	CRSAPrivateKeyStandard* self = NewLC(aN, aD);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CRSAPrivateKeyStandard* CRSAPrivateKeyStandard::NewLC(RInteger& aN,
	RInteger& aD)
	{
	CRSAPrivateKeyStandard* self = new(ELeave) CRSAPrivateKeyStandard(aN, aD);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CRSAPrivateKeyStandard::ConstructL()
	{
	// Check that the modulus and exponent are positive integers
	if(!N().IsPositive() || !D().IsPositive() || (D() <= 1))
		{
		// If we need to leave during construction we must release ownership
		// of the RInteger parameters that were passed in.
		// These parameters should be on the cleanup stack so if we don't 
		// release ownership they will be deleted twice, causing a panic
		iN = RInteger();
		iD = RInteger();
		User::Leave(KErrArgument);
		}
	}

EXPORT_C const TInteger& CRSAPrivateKeyStandard::D(void) const
	{
	return iD;
	}

EXPORT_C CRSAPrivateKeyStandard::CRSAPrivateKeyStandard(RInteger& aN, 
	RInteger& aD) : CRSAPrivateKey(EStandard, aN), iD(aD)
	{
	}

EXPORT_C CRSAPrivateKeyStandard::~CRSAPrivateKeyStandard()
	{	
	iD.Close();
	}

/* CRSAPrivateKeyCRT */

EXPORT_C CRSAPrivateKeyCRT* CRSAPrivateKeyCRT::NewL(RInteger& aN, RInteger& aP,
	RInteger& aQ, RInteger& aDP, RInteger& aDQ, RInteger& aQInv)
	{
	CRSAPrivateKeyCRT* self = NewLC(aN, aP, aQ, aDP, aDQ, aQInv);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CRSAPrivateKeyCRT* CRSAPrivateKeyCRT::NewLC(RInteger& aN, RInteger& aP, 
	RInteger& aQ, RInteger& aDP, RInteger& aDQ, RInteger& aQInv)
	{
	CRSAPrivateKeyCRT* self = new(ELeave) CRSAPrivateKeyCRT(aN, aP, aQ, 
		aDP, aDQ, aQInv);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CRSAPrivateKeyCRT::CRSAPrivateKeyCRT(RInteger& aN, RInteger& aP, 
	RInteger& aQ, RInteger& aDP, RInteger& aDQ, RInteger& aQInv) 
	: CRSAPrivateKey(EStandardCRT, aN), iP(aP), iQ(aQ), iDP(aDP), iDQ(aDQ), 
		iQInv(aQInv)
	{
	}

void CRSAPrivateKeyCRT::ConstructL()
	{
	// Check that all parameters are positive integers
	if(!P().IsPositive() || !Q().IsPositive() || !DP().IsPositive() 
		|| !DQ().IsPositive() || !QInv().IsPositive())
		{
		// If we need to leave during construction we must release ownership
		// of the RInteger parameters that were passed in.
		// These parameters should be on the cleanup stack so if we don't 
		// release ownership they will be deleted twice, causing a panic
		iN = RInteger();
		iP = RInteger();
		iQ = RInteger();
		iDP = RInteger();
		iDQ = RInteger();
		iQInv = RInteger();
		User::Leave(KErrArgument);
		}
	}


EXPORT_C CRSAPrivateKeyCRT::~CRSAPrivateKeyCRT()
	{	
	iP.Close();
	iQ.Close();
	iDP.Close();
	iDQ.Close();
	iQInv.Close();
	}

EXPORT_C const TInteger& CRSAPrivateKeyCRT::P(void) const
	{
	return iP;
	}

EXPORT_C const TInteger& CRSAPrivateKeyCRT::Q(void) const
	{
	return iQ;
	}

EXPORT_C const TInteger& CRSAPrivateKeyCRT::DP(void) const
	{
	return iDP;
	}

EXPORT_C const TInteger& CRSAPrivateKeyCRT::DQ(void) const
	{
	return iDQ;
	}

EXPORT_C const TInteger& CRSAPrivateKeyCRT::QInv(void) const
	{
	return iQInv;
	}

/* CRSAKeyPair */

EXPORT_C CRSAKeyPair* CRSAKeyPair::NewL(TUint aModulusBits, 
	TRSAPrivateKeyType aKeyType /*= EStandardCRT*/)
	{
	CRSAKeyPair* self = NewLC(aModulusBits, aKeyType);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CRSAKeyPair* CRSAKeyPair::NewLC(TUint aModulusBits, 
	TRSAPrivateKeyType aKeyType /*= EStandardCRT*/)
	{
	CRSAKeyPair* self = new(ELeave) CRSAKeyPair();
	CleanupStack::PushL(self);
	self->ConstructL(aModulusBits, aKeyType, KFermat4);
	return self;
	}

EXPORT_C const CRSAPublicKey& CRSAKeyPair::PublicKey(void) const
	{
	return *iPublic;
	}
	
EXPORT_C const CRSAPrivateKey& CRSAKeyPair::PrivateKey(void) const
	{
	return *iPrivate;
	}

EXPORT_C CRSAKeyPair::~CRSAKeyPair(void) 
	{
	delete iPublic;
	delete iPrivate;
	}

EXPORT_C CRSAKeyPair::CRSAKeyPair(void)
	{
	}

void CRSAKeyPair::ConstructL(TUint aModulusBits, 
	TRSAPrivateKeyType aKeyType, TUint aPublicExponent)
	{
	RInteger e = RInteger::NewL(aPublicExponent);
	CleanupStack::PushL(e);

	RInteger p;
	RInteger q;
	
	//these make sure n is a least aModulusBits long
    TInt pbits=(aModulusBits+1)/2;
    TInt qbits=aModulusBits-pbits;

	//generate a prime p such that GCD(e,p-1) == 1
	for (;;)
		{
		p = RInteger::NewPrimeL(pbits,TInteger::ETop2BitsSet);
		CleanupStack::PushL(p);
		--p;

		RInteger gcd = e.GCDL(p);
		if( gcd == 1 )
			{
			++p;
			gcd.Close();
			//p is still on cleanup stack
			break;
			}
		CleanupStack::PopAndDestroy(&p);
		gcd.Close();
		}

	//generate a prime q such that GCD(e,q-1) == 1 && (p != q)
	for (;;)
		{
		q = RInteger::NewPrimeL(qbits,TInteger::ETop2BitsSet);
		CleanupStack::PushL(q);
		--q;

		RInteger gcd = e.GCDL(q);
		if( gcd == 1 )
			{
			++q;
			if( p != q )
				{
				gcd.Close();
				//q is still on cleanup stack
				break;
				}
			}
		CleanupStack::PopAndDestroy(&q);
		gcd.Close();
		}
		
	//make sure p > q
    if ( p < q)
        {
		TClassSwap(p,q);
        }

	//calculate n = p * q 
	RInteger n = p.TimesL(q);
	CleanupStack::PushL(n);

	--p;
	--q;

	//temp = (p-1)(q-1)
	RInteger temp = p.TimesL(q);
	CleanupStack::PushL(temp);

	//e * d = 1 mod ((p-1)(q-1)) 
	//d = e^(-1) mod ((p-1)(q-1))
	RInteger d = e.InverseModL(temp);
	CleanupStack::PopAndDestroy(&temp); //temp
	CleanupStack::PushL(d);

	if (aKeyType==EStandardCRT)
	{
		//calculate dP = d mod (p-1) 
		RInteger dP = d.ModuloL(p); //p is still p-1
		CleanupStack::PushL(dP);

		//calculate dQ = d mod (q-1) 
		RInteger dQ = d.ModuloL(q); //q is still q-1
		CleanupStack::PushL(dQ);

		++p;
		++q;
		//calculate inverse of qInv = q^(-1)mod(p)
		RInteger qInv = q.InverseModL(p);
		CleanupStack::PushL(qInv);

		iPrivate = CRSAPrivateKeyCRT::NewL(n,p,q,dP,dQ,qInv);
		
		CleanupStack::Pop(3, &dP); //qInv, dQ, dP
		CleanupStack::PopAndDestroy(&d); //d	
		CleanupStack::Pop(3, &p); //n, q, p
		//e is still on cleanup stack
	}
	else if (aKeyType==EStandard)
	{
		iPrivate = CRSAPrivateKeyStandard::NewL(n,d);

		CleanupStack::Pop(2, &n); //d, n
		CleanupStack::PopAndDestroy(2, &p); //q, p
		//e is still on cleanup stack
	}
	else
	{
		User::Leave(KErrNotSupported);
	}

	//make a copy of n for the public parameters
	RInteger n1 = RInteger::NewL(PrivateKey().N());
	CleanupStack::PushL(n1);
	iPublic = CRSAPublicKey::NewL(n1,e); 
	CleanupStack::Pop(2, &e); //n1, e
	}

