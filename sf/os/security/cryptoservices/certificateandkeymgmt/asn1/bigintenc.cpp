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
* Implementation for the class encoding Big Integers in ASN1 DER
*
*/


#include <asn1enc.h>

#include <bigint.h>


EXPORT_C CASN1EncBigInt* CASN1EncBigInt::NewLC(const TInteger& aInteger)
	{
	CASN1EncBigInt* self = new (ELeave) CASN1EncBigInt();
	CleanupStack::PushL(self);
	self->ConstructL(aInteger);
	return self;
	}

EXPORT_C CASN1EncBigInt* CASN1EncBigInt::NewL(const TInteger& aInteger)
	{
	CASN1EncBigInt* self = NewLC(aInteger);
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C CASN1EncBigInt::~CASN1EncBigInt()
	{
	delete iContents;
	}


CASN1EncBigInt::CASN1EncBigInt()
: CASN1EncPrimitive(EASN1Integer)
	{
	}

void CASN1EncBigInt::ConstructL(const TInteger& aInteger)
{
	iContents = aInteger.BufferLC();
	
	CleanupStack::Pop(); // We take ownership of the contents

	// May wish to skip leading byte(s) if they're unnecessary - use
	// iWriteContents to point to the minimal part of Contents, that we'll actually use.
	TInt last = iContents->Length() - 1;
	TInt first = 0;
//	if (aInteger >= 0)	//	Only signed integers now...
//		{
		while (first < last
			&& (*iContents)[first] == 0
			&& !((*iContents)[first + 1] & 0x80))
			{
			++first;
			}
//		}
/*	else
		{
		while (first < last
			&& (*iContents)[first] == 0xFF
			&& (*iContents)[first + 1] & 0x80)
			{
			++first;
			}
		}
*/
		//to encode positive integers correctly add a leading zero byte
		//if the first bit is set DEF038956
		if (iContents->Length() != 0)
			{
			if ((*iContents)[first] & 0x80)
				{
				_LIT8(KPadZero, "\0");
				TPtr8 ptr = iContents->Des();
				//check if the buffer does not have enough space to insert a byte
				if (ptr.Length() == ptr.MaxLength())
					{
					iContents = iContents->ReAllocL(iContents->Length() + 1);
					TPtr8 reallocptr = iContents->Des();
					reallocptr.Insert(0, KPadZero);
					}
				else
					ptr.Insert(0, KPadZero);
				}
			}


	iWriteContents.Set(iContents->Mid(first));

	CASN1EncPrimitive::ConstructL();
	}


void CASN1EncBigInt::CalculateContentsLengthDER()
	{
	iContentsLengthDER = iWriteContents.Length();
	}


void CASN1EncBigInt::WriteContentsDERL(TDes8& aBuf) const
	{
	aBuf.Copy(iWriteContents);
	}
