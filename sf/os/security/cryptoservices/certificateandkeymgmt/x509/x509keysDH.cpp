/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <x509keys.h>
#include <asn1dec.h>
#include <asn1enc.h>
#include <x509cert.h>

EXPORT_C CX509DHPublicKey* CX509DHPublicKey::NewL(const TDesC8& aParamsData, const TDesC8& aKeyData)
	{
	CX509DHPublicKey* me = CX509DHPublicKey::NewLC(aParamsData, aKeyData);
	CleanupStack::Pop(me);
	return (me);
	}

EXPORT_C CX509DHPublicKey* CX509DHPublicKey::NewLC(const TDesC8& aParamsData, const TDesC8& aKeyData)
	{
		CX509DHPublicKey* me = new (ELeave) CX509DHPublicKey();
		CleanupStack::PushL(me);
		me->ConstructL(aParamsData, aKeyData);
		return (me);	
	}

EXPORT_C CX509DHPublicKey::~CX509DHPublicKey()
	{
	}

CX509DHPublicKey::CX509DHPublicKey()
	{
	}

void CX509DHPublicKey::ConstructL(const TDesC8& aParamsData, const TDesC8& aKeyData)
	{
	CX509DHDomainParams* params=CX509DHDomainParams::NewLC(aParamsData);
	iN = RInteger::NewL(params->P());	
	iG = RInteger::NewL(params->G());	
	iX = RInteger::NewL(aKeyData);	
	CleanupStack::PopAndDestroy(params);	
	}

//DH params
//DH validation params
EXPORT_C CX509DHValidationParams* CX509DHValidationParams::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509DHValidationParams::NewL(aBinaryData, pos);
	}

EXPORT_C CX509DHValidationParams* CX509DHValidationParams::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509DHValidationParams::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509DHValidationParams* CX509DHValidationParams::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509DHValidationParams* self = CX509DHValidationParams::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509DHValidationParams* CX509DHValidationParams::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509DHValidationParams* self = new(ELeave) CX509DHValidationParams;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509DHValidationParams::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecGeneric dec(aBinaryData.Right(aBinaryData.Length() - aPos));
	dec.InitL();
	if (dec.Tag() != EASN1Sequence)
		{
		User::Leave(KErrArgument);
		}
	TInt end = aPos + dec.LengthDER();
	aPos += dec.LengthDERHeader();//add header length to aPos

	TASN1DecBitString encBS;
	iSeed = encBS.ExtractOctetStringL(aBinaryData, aPos);
	TASN1DecInteger encInt;
	iPGenCounter = encInt.DecodeDERLongL(aBinaryData, aPos);

	if (aPos != end)
		{
		User::Leave(KErrArgument);
		}	
	}

EXPORT_C const TPtrC8 CX509DHValidationParams::Seed() const
	{
	return *iSeed;
	}

EXPORT_C const TInteger& CX509DHValidationParams::PGenCounter() const
	{
	return iPGenCounter;
	}

CX509DHValidationParams::~CX509DHValidationParams()
	{
	delete iSeed;
	iPGenCounter.Close();
	}

CX509DHValidationParams::CX509DHValidationParams()
	{
	}

//DH domain params
EXPORT_C CX509DHDomainParams* CX509DHDomainParams::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509DHDomainParams::NewL(aBinaryData, pos);
	}

EXPORT_C CX509DHDomainParams* CX509DHDomainParams::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509DHDomainParams::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509DHDomainParams* CX509DHDomainParams::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509DHDomainParams* self = CX509DHDomainParams::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509DHDomainParams* CX509DHDomainParams::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509DHDomainParams* self = new(ELeave) CX509DHDomainParams;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509DHDomainParams::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecGeneric dec(aBinaryData.Right(aBinaryData.Length() - aPos));
	dec.InitL();
	if (dec.Tag() != EASN1Sequence)
		{
		User::Leave(KErrArgument);
		}
	TInt end = aPos + dec.LengthDER();
	aPos += dec.LengthDERHeader();//add header length to aPos

	TASN1DecInteger encInt;
	iP = encInt.DecodeDERLongL(aBinaryData, aPos);
	iG = encInt.DecodeDERLongL(aBinaryData, aPos);
	if (aPos < end)
		{
		iQ = encInt.DecodeDERLongL(aBinaryData, aPos);
		}

	if (aPos < end)
		{
		TASN1DecGeneric gen1(aBinaryData.Right(aBinaryData.Length() - aPos));
		gen1.InitL();
		TBool doneVal = EFalse;
		if (gen1.Tag() == EASN1Integer)
			{
			iJ = encInt.DecodeDERLongL(aBinaryData, aPos);
			}
		else
			{
			iValidationParams = CX509DHValidationParams::NewL(aBinaryData, aPos);
			doneVal = EFalse;
			}
		if ((aPos < end) && (!doneVal))
			{
			iValidationParams = CX509DHValidationParams::NewL(aBinaryData, aPos);
			}
		}

	if (aPos != end)
		{
		User::Leave(KErrArgument);
		}	
	}

EXPORT_C const TInteger& CX509DHDomainParams::P() const
	{
	return iP;
	}

EXPORT_C const TInteger& CX509DHDomainParams::G() const
	{
	return iG;
	}

EXPORT_C const TInteger& CX509DHDomainParams::Q() const
	{
	return iQ;
	}

EXPORT_C const TInteger& CX509DHDomainParams::J() const							
	{
	return iJ;
	}

EXPORT_C const CX509DHValidationParams* CX509DHDomainParams::ValidationParams() const
	{
	return iValidationParams;
	}

CX509DHDomainParams::~CX509DHDomainParams()
	{
	iP.Close();
	iG.Close();
	iQ.Close();
	iJ.Close();
	delete iValidationParams;
	}

CX509DHDomainParams::CX509DHDomainParams()
	{
	}

//	CX509DHKeyPair


EXPORT_C CX509DHKeyPair* CX509DHKeyPair::NewL(const TDesC8& aParamsData)
{
	CX509DHKeyPair* me = CX509DHKeyPair::NewLC(aParamsData);
	CleanupStack::Pop(me);
	return (me);
}

EXPORT_C CX509DHKeyPair* CX509DHKeyPair::NewLC(const TDesC8& aParamsData)
{
	CX509DHKeyPair* me = new (ELeave) CX509DHKeyPair();
	CleanupStack::PushL(me);
	me->ConstructL(aParamsData);
	return (me);
}

EXPORT_C CX509DHKeyPair::~CX509DHKeyPair()
{}

CX509DHKeyPair::CX509DHKeyPair()
{}

void CX509DHKeyPair::ConstructL(const TDesC8& aParamsData)
{
	CX509DHDomainParams* params=CX509DHDomainParams::NewLC(aParamsData);
	RInteger n = RInteger::NewL(params->P());
	CleanupStack::PushL(n);
	RInteger g = RInteger::NewL(params->G());
	CleanupStack::PushL(g);
	
	CDHKeyPair::ConstructL(n, g);
	CleanupStack::Pop(2, &n);	//	n, g owned by this now
	CleanupStack::PopAndDestroy(params);	
}
