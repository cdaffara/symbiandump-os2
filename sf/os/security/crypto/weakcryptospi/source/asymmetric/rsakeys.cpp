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
#include "rsakeypairshim.h"
#include "../common/inlines.h"

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
	CRSAKeyPairShim* self = CRSAKeyPairShim::NewLC(aModulusBits, aKeyType);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CRSAKeyPair* CRSAKeyPair::NewLC(TUint aModulusBits, 
	TRSAPrivateKeyType aKeyType /*= EStandardCRT*/)
	{
	CRSAKeyPairShim* self = CRSAKeyPairShim::NewLC(aModulusBits, aKeyType);
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

