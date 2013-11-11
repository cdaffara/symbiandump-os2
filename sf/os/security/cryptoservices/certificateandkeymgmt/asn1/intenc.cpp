/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation for class encoding TInt in ASN1 DER
*
*/


#include <asn1enc.h>

EXPORT_C CASN1EncInt* CASN1EncInt::NewLC(const TInt aInt)
	{
	CASN1EncInt* self = new (ELeave) CASN1EncInt(aInt);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CASN1EncInt* CASN1EncInt::NewL(const TInt aInt)
	{
	CASN1EncInt* self = NewLC(aInt);
	CleanupStack::Pop(self);
	return self;
	}

CASN1EncInt::CASN1EncInt(const TInt aInt)
: CASN1EncPrimitive(EASN1Integer), iInt(aInt)
	{
	}

void CASN1EncInt::CalculateContentsLengthDER()
	{
	iContentsLengthDER = 1;

	// Flip bits in -ve numbers
	TUint working = iInt < 0 ? ~iInt : iInt;
	TUint last = working;
	while (working >>= 8)
		{
		last = working;
		++iContentsLengthDER;
		}
	
	// If most sig non-zero byte had a 1 top bit, we'll need the next byte too
	// (applies to both +ve and -ve cases)
	if (last & 0x80)
		{
		++iContentsLengthDER;
		}
	}


void CASN1EncInt::WriteContentsDERL(TDes8& aBuf) const
	{
	// Unsigned, so operator>>= works sensibly
	TUint working = iInt;
	TUint position = iContentsLengthDER;
	while (position)
		{
		// Cast takes least sig 8 bits
		aBuf[--position] = STATIC_CAST(TUint8, working);
		working >>= 8;
		}
	}
