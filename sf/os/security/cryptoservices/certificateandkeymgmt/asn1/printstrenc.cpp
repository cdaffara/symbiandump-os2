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
* Implementation for class encoding printable strings in ASN1 DER
*
*/


#include <asn1enc.h>

const TInt8 PrintableStringValid[256]=
	{
//  0 1 2 3 4 5 6 7 8 9 a b c d e f
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 0
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 1
	0,0,0,0,0,0,0,1,1,1,0,1,1,1,1,1,  // 2
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,  // 3
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 4
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,  // 5 
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 6
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,  // 7 
	};

EXPORT_C CASN1EncPrintableString* CASN1EncPrintableString::NewLC(const TDesC8& aStr)
	{
	CASN1EncPrintableString* self = new (ELeave) CASN1EncPrintableString();
	CleanupStack::PushL(self);
	self->ConstructL(aStr);
	return self;
	}

EXPORT_C CASN1EncPrintableString* CASN1EncPrintableString::NewL(const TDesC8& aStr)
	{
	CASN1EncPrintableString* self = NewLC(aStr);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CASN1EncPrintableString::~CASN1EncPrintableString()
	{
	delete iContents;
	}


CASN1EncPrintableString::CASN1EncPrintableString()
: CASN1EncPrimitive(EASN1PrintableString)
	{
	}

TInt CASN1EncPrintableString::CheckValid(const TDesC8& aString)
	{
	TInt i;
	for (i = 0; i < aString.Length(); ++i)
		{
		if ((aString[i] > 0x7F) || (PrintableStringValid[aString[i]] == 0))
			{
			return EFalse;
			}
		}
	return ETrue;
	}

void CASN1EncPrintableString::ConstructL(const TDesC8& aStr)
	{
	if (CheckValid(aStr) == EFalse)
		{
		User::Leave(KErrArgument);
		}
	iContents = aStr.AllocL();
	CASN1EncPrimitive::ConstructL();
	}


void CASN1EncPrintableString::CalculateContentsLengthDER()
	{
	iContentsLengthDER = iContents->Length();
	}


void CASN1EncPrintableString::WriteContentsDERL(TDes8& aBuf) const
	{
	aBuf.Copy(*iContents);
	}
