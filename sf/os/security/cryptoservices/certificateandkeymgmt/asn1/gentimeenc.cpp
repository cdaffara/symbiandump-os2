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
* Implementation for DER encoding of GeneralizedTime.
* Following from the decoding class
* 1) fractions of seconds are not supported
* 2) regional time zone offsets are not supported
*
*/


#include "panic.h"

#include <asn1enc.h>

const TUint KEncodingLengthWithoutSeconds = 13;
const TUint KEncodingLengthWithSeconds = 15;


EXPORT_C CASN1EncGeneralizedTime* CASN1EncGeneralizedTime::NewLC(const TTime& aTime)
	{
	CASN1EncGeneralizedTime* self = new (ELeave) CASN1EncGeneralizedTime(aTime);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CASN1EncGeneralizedTime* CASN1EncGeneralizedTime::NewL(const TTime& aTime)
	{
	CASN1EncGeneralizedTime* self = NewLC(aTime);
	CleanupStack::Pop(self);
	return self;
	}

CASN1EncGeneralizedTime::CASN1EncGeneralizedTime(const TTime& aTime)
: CASN1EncPrimitive(EASN1GeneralizedTime), iDateTime(aTime.DateTime())
	{
	__ASSERT_ALWAYS(iDateTime.Year() < 10000, Panic(KErrYearTooBigForGeneralizedTime));
	}


void CASN1EncGeneralizedTime::CalculateContentsLengthDER()
	{
	iContentsLengthDER = 
		iDateTime.Second() ? KEncodingLengthWithSeconds : KEncodingLengthWithoutSeconds;
	}


void CASN1EncGeneralizedTime::WriteContentsDERL(TDes8& aBuf) const
	{
	aBuf.SetLength(0);
	aBuf.AppendNumFixedWidth(iDateTime.Year(), EDecimal, 4);
	aBuf.AppendNumFixedWidth(iDateTime.Month() + 1, EDecimal, 2);  // Our months are zero-based
	aBuf.AppendNumFixedWidth(iDateTime.Day() + 1, EDecimal, 2);    // Our days are zero-based
	aBuf.AppendNumFixedWidth(iDateTime.Hour(), EDecimal, 2);
	aBuf.AppendNumFixedWidth(iDateTime.Minute(), EDecimal, 2);
	if (iDateTime.Second())
		{
		aBuf.AppendNumFixedWidth(iDateTime.Second(), EDecimal, 2);
		}

	aBuf.Append('Z');
	}
