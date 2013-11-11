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
#include "dhkeypairshim.h"


/* CDHParameters */
EXPORT_C const TInteger& CDHParameters::N(void) const
	{
	return iN;
	}

EXPORT_C const TInteger& CDHParameters::G(void) const
	{
	return iG;
	}

EXPORT_C CDHParameters::~CDHParameters(void)
	{
	iN.Close();
	iG.Close();
	}

EXPORT_C CDHParameters::CDHParameters(RInteger& aN, RInteger& aG) : iN(aN), iG(aG)
	{
	}

/* CDHPublicKey */
EXPORT_C CDHPublicKey* CDHPublicKey::NewL(RInteger& aN, RInteger& aG, 
	RInteger& aX)
	{
	CDHPublicKey* self = new(ELeave) CDHPublicKey(aN, aG, aX);
	return self;
	}

EXPORT_C CDHPublicKey* CDHPublicKey::NewLC(RInteger& aN, RInteger& aG, 
	RInteger& aX)
	{
	CDHPublicKey* self = NewL(aN, aG, aX);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C const TInteger& CDHPublicKey::X(void) const
	{
	return iX;
	}

EXPORT_C CDHPublicKey::CDHPublicKey(RInteger& aN, RInteger& aG, RInteger& aX)
	: CDHParameters(aN, aG), iX(aX)
	{
	}

EXPORT_C CDHPublicKey::~CDHPublicKey(void)
	{
	iX.Close();
	}

/* CDHPrivateKey */
EXPORT_C CDHPrivateKey* CDHPrivateKey::NewL(RInteger& aN, RInteger& aG, 
	RInteger& ax)
	{
	CDHPrivateKey* self = new(ELeave) CDHPrivateKey(aN, aG, ax);
	return self;
	}

EXPORT_C CDHPrivateKey* CDHPrivateKey::NewLC(RInteger& aN, RInteger& aG, 
	RInteger& ax)
	{
	CDHPrivateKey* self = NewL(aN, aG, ax);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C const TInteger& CDHPrivateKey::x(void) const
	{
	return ix;
	}

EXPORT_C CDHPrivateKey::CDHPrivateKey(RInteger& aN, RInteger& aG, RInteger& ax)
	: CDHParameters(aN, aG), ix(ax)
	{
	}

EXPORT_C CDHPrivateKey::~CDHPrivateKey(void)
	{
	ix.Close();
	}

/* CDHKeyPair */

EXPORT_C CDHKeyPair* CDHKeyPair::NewL(RInteger& aN, RInteger& aG)
	{
	CDHKeyPairShim* self = CDHKeyPairShim::NewLC(aN, aG);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDHKeyPair* CDHKeyPair::NewLC(RInteger& aN, RInteger& aG)
	{
	CDHKeyPairShim* self = CDHKeyPairShim::NewLC(aN, aG);
	return self;
	}

EXPORT_C CDHKeyPair* CDHKeyPair::NewL(RInteger& aN, RInteger& aG, RInteger& ax)
	{
	CDHKeyPairShim* self = CDHKeyPairShim::NewLC(aN, aG, ax);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CDHKeyPair* CDHKeyPair::NewLC(RInteger& aN, RInteger& aG, RInteger& ax)
	{
	CDHKeyPairShim* self = CDHKeyPairShim::NewLC(aN, aG, ax);
	return self;
	}

EXPORT_C const CDHPublicKey& CDHKeyPair::PublicKey(void) const
	{
	return *iPublic;
	}

EXPORT_C const CDHPrivateKey& CDHKeyPair::PrivateKey(void) const
	{
	return *iPrivate;
	}

EXPORT_C CDHKeyPair::~CDHKeyPair(void)
	{
	delete iPublic;
	delete iPrivate;
	}

EXPORT_C CDHKeyPair::CDHKeyPair(void)
	{
	}

// Excluded from coverage due to shim replacements.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
EXPORT_C void CDHKeyPair::ConstructL(RInteger& aN, RInteger& aG)
	{
	//declaring a reference just for clarity in NewRandomL statement
	RInteger& nminus2 = aN;
	--nminus2;
	--nminus2;

	//find a random x | 1 <= x <= n-2
	RInteger x = RInteger::NewRandomL(TInteger::One(), nminus2);
	CleanupStack::PushL(x);
	++nminus2;
	++nminus2; // reincrement aN

	ConstructL(aN, aG, x);

	CleanupStack::Pop(&x);
	}

EXPORT_C void CDHKeyPair::ConstructL(RInteger& aN, RInteger& aG, RInteger& ax)
	{
	//declaring a reference just for clarity in if statements
	RInteger& nminus2 = aN;
	--nminus2;
	--nminus2;
	
	if( aG < TInteger::Two() || aG > nminus2 )
		{
		User::Leave(KErrArgument);
		}
	//In the case of the other ConstructL calling this function this if
	//statement is redundant.  However, we need to check as this is can be
	//called without going through the other api.
	if( ax < TInteger::One() || ax > nminus2 )
		{
		User::Leave(KErrArgument);
		}

	++nminus2;
	++nminus2; // reincrement aN

	// Calculate X = g^(x) mod n; (note the case sensitivity)
	RInteger X = TInteger::ModularExponentiateL(aG, ax, aN);
	CleanupStack::PushL(X);

	RInteger n1 = RInteger::NewL(aN);
	CleanupStack::PushL(n1);
	RInteger g1 = RInteger::NewL(aG);
	CleanupStack::PushL(g1);
	iPublic = CDHPublicKey::NewL(n1, g1, X);
	CleanupStack::Pop(3, &X); // g1, n1, X all owned by iPublic
	
	iPrivate = CDHPrivateKey::NewL(aN, aG, ax);
	}

// Unused exported and protected members. So, exclude them from coverage.
EXPORT_C CDHParameters::CDHParameters(void)
	{
	}

EXPORT_C CDHPrivateKey::CDHPrivateKey(void)
	{
	}

EXPORT_C CDHPublicKey::CDHPublicKey(void)
	{
	}
