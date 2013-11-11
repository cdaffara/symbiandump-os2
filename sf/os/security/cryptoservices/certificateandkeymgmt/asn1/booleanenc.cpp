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
* Implementation for DER encoding of Booleans
*
*/


#include <asn1enc.h>

const TUint8 KTrueEncodingDER = 0xFF;
const TUint8 KFalseEncodingDER = 0x00;

EXPORT_C CASN1EncBoolean* CASN1EncBoolean::NewLC(const TBool aBool)
	{
	CASN1EncBoolean* self = new (ELeave) CASN1EncBoolean(aBool);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CASN1EncBoolean* CASN1EncBoolean::NewL(const TBool aBool)
	{
	CASN1EncBoolean* self = NewLC(aBool);
	CleanupStack::Pop(self);
	return self;
	}

CASN1EncBoolean::CASN1EncBoolean(const TBool aBool)
: CASN1EncPrimitive(EASN1Boolean), iBool(aBool)
	{
	}


void CASN1EncBoolean::CalculateContentsLengthDER()
	{
	iContentsLengthDER = 1;
	}


void CASN1EncBoolean::WriteContentsDERL(TDes8& aBuf) const
	{
	if (iBool)
		{
		aBuf[0] = KTrueEncodingDER;
		}
	else
		{
		aBuf[0] = KFalseEncodingDER;
		}
	}
