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
* Methods for encoding object identifiers
*
*/


#include "base128enc.h"
#include "panic.h"

#include <asn1enc.h>

const TInt KArrayGranularity = 5;

const TUint KSecondTermBigLimit = 175;
const TUint KFirstTermMultiplier = 40;
const TUint KFirstTermMax = 2;


EXPORT_C CASN1EncObjectIdentifier* CASN1EncObjectIdentifier::NewLC(const TDesC& aStr)
	{
	CASN1EncObjectIdentifier* self = new (ELeave) CASN1EncObjectIdentifier();
	CleanupStack::PushL(self);
	self->ConstructL(aStr);
	return self;
	}

EXPORT_C CASN1EncObjectIdentifier* CASN1EncObjectIdentifier::NewL(const TDesC& aStr)
	{
	CASN1EncObjectIdentifier* self = NewLC(aStr);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CASN1EncObjectIdentifier::~CASN1EncObjectIdentifier()
	{
	iData.Close();
	}


CASN1EncObjectIdentifier::CASN1EncObjectIdentifier()
: CASN1EncPrimitive(EASN1ObjectIdentifier), iData(KArrayGranularity)
	{
	}


// Takes ints in a string, delimited by '.' characters in between (not at ends)
void CASN1EncObjectIdentifier::ConstructL(const TDesC& aStr)
	{
	iData.Reset();
	TInt index = 0;
	TLex lex(aStr);

	// First term
	__ASSERT_ALWAYS(!lex.Eos(), User::Leave(KErrBadDescriptor));
	TUint first;
	User::LeaveIfError(lex.Val(first));
	__ASSERT_ALWAYS(first <= KFirstTermMax, User::Leave(KErrBadDescriptor));
	// Static cast takes 8 least sig bits
	iFirstOctet = STATIC_CAST(TUint8, KFirstTermMultiplier * first);

	// A '.' to delimit
	__ASSERT_ALWAYS(!lex.Eos() && lex.Get() == '.',	User::Leave(KErrBadDescriptor));

	// Second term
	__ASSERT_ALWAYS(!lex.Eos(), User::Leave(KErrBadDescriptor));
	TUint second;
	User::LeaveIfError(lex.Val(second));
	__ASSERT_ALWAYS((first < KFirstTermMax && second < KFirstTermMultiplier)
		|| (first == KFirstTermMax && second <= KSecondTermBigLimit),
		User::Leave(KErrBadDescriptor));
	// Static cast takes 8 least sig bits
	iFirstOctet = STATIC_CAST(TUint8, iFirstOctet + second);

	// Remaining terms
	while (!lex.Eos())
		{
		// Delimiter, and check we're not at end after that
		__ASSERT_ALWAYS(lex.Get() == '.' && !lex.Eos(),	User::Leave(KErrBadDescriptor));

		TUint value;
		User::LeaveIfError(lex.Val(value));
		++index;

		// Store the data away for later
		TASN1EncBase128DER encoder(value);
		User::LeaveIfError(iData.Append(encoder));
		}

	// Remainder of ConstructL is here - safe to call CalculateContentsLengthDER now.
	CASN1EncPrimitive::ConstructL();
	}


void CASN1EncObjectIdentifier::CalculateContentsLengthDER()
	{
	iContentsLengthDER = 1;
	for (TInt i = iData.Count() - 1; i >= 0; --i)
		{
		iContentsLengthDER += iData[i].LengthDER();
		}
	}


void CASN1EncObjectIdentifier::WriteContentsDERL(TDes8& aBuf) const
	{
	aBuf[0] = iFirstOctet;
	TUint cursor = 1;
	TInt count = iData.Count();
	for (TInt i = 0; i < count; ++i)
		{
		iData[i].WriteDERL(aBuf, cursor);
		}
	}
