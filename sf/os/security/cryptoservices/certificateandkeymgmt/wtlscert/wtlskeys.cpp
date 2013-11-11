/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <wtlskeys.h>
#include "wtlsdec.h"
#include <bigint.h>

EXPORT_C CWTLSRSAPublicKey* CWTLSRSAPublicKey::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSRSAPublicKey::NewL(aBinaryData, pos);
	}

EXPORT_C CWTLSRSAPublicKey* CWTLSRSAPublicKey::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSRSAPublicKey::NewLC(aBinaryData, pos);
	}

EXPORT_C CWTLSRSAPublicKey* CWTLSRSAPublicKey::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSRSAPublicKey* self = CWTLSRSAPublicKey::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWTLSRSAPublicKey* CWTLSRSAPublicKey::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSRSAPublicKey* self = new(ELeave) CWTLSRSAPublicKey;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}


void CWTLSRSAPublicKey::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TWTLSDecUnsignedInteger decInt;
	TInt expLength = decInt.DecodeShortL(aBinaryData, aPos, 2);
	iE = decInt.DecodeLongL(aBinaryData, aPos, expLength);
	TInt modLength = decInt.DecodeShortL(aBinaryData, aPos, 2);
	iN = decInt.DecodeLongL(aBinaryData, aPos, modLength);
	}

CWTLSRSAPublicKey::CWTLSRSAPublicKey()
{}

