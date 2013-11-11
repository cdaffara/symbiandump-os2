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
* Define methods for asn1-encoding base class
*
*/


#include "base128enc.h"
#include "panic.h"

#include <asn1enc.h>

const TUint KMaxLowNumberFormTag = 30;
const TUint KMaxShortFormLength = 127;


EXPORT_C CASN1EncBase::CASN1EncBase(const TTagType aType, const TASN1Class aClass)
: iType(aType), iClass(aClass)
	{
	CalculateTagLengthDER();
	}


EXPORT_C CASN1EncBase::~CASN1EncBase()
	{
	}


EXPORT_C void CASN1EncBase::ConstructL()
	{
	ContentsLengthChanged();
	}


void CASN1EncBase::ContentsLengthChanged()
	{
	CalculateContentsLengthDER();
	CalculateLengthLengthDER();

	if (iParent)
		{
		iParent->ContentsLengthChanged();
		}
	}


// Set tagging scheme/tag/class
EXPORT_C void CASN1EncBase::SetTag(const TTagType aType, const TASN1Class aClass)
	{
	iType = aType;
	iClass = aClass;

	CalculateTagLengthDER();
	}


// Gives total number of octets in the DER encoding of this object
EXPORT_C TUint CASN1EncBase::LengthDER() const
	{
	return iTagLengthDER + iLengthLengthDER + iContentsLengthDER;
	}
	

// Writes entire DER encoding of this object into the given buffer
EXPORT_C void CASN1EncBase::WriteDERL(TDes8& aBuf, TUint& aPos) const
	{
	__ASSERT_ALWAYS(aBuf.Length() - STATIC_CAST(TInt, aPos) >= STATIC_CAST(TInt, LengthDER()),
		Panic(KErrDescriptorTooShort));

	WriteHelperL(aBuf, aPos, iTagLengthDER,      &CASN1EncBase::WriteTagDERL);
	WriteHelperL(aBuf, aPos, iLengthLengthDER,   &CASN1EncBase::WriteLengthDER);
	WriteHelperL(aBuf, aPos, iContentsLengthDER, &CASN1EncBase::WriteContentsDERL);
	}


// Internal helper method, used for efficiency
void CASN1EncBase::WriteHelperL(TDes8& aBuf, TUint& aPos, const TUint aLength,
									WriteFunc aWriteFunc) const
	{
	if (aLength)
		{
		// Get the middle bit of buffer we should be using
		TPtr8 ptr(&aBuf[aPos], aLength, aLength);
		
		// Call the function to actually write the data
		(this->*aWriteFunc)(ptr);
		
		aPos += aLength;
		}
	}


// Give number of octets in tag section
void CASN1EncBase::CalculateTagLengthDER()
	{
	if (iType <= KMaxLowNumberFormTag)
		{
		// Low tag number form
		iTagLengthDER = 1;
		}
	else
		{
		// High tag number form
		TASN1EncBase128DER encoder(iType);
		iTagLengthDER = encoder.LengthDER() + 1;
		}
	}


// Give number of octets in length section
// Only called after contents length is worked out
void CASN1EncBase::CalculateLengthLengthDER()
	{
	iLengthLengthDER = 1;
	if (iContentsLengthDER > KMaxShortFormLength)
		{
		// Long form - need number of octets in base256 representation
		// of length, plus 1.

		TUint working = iContentsLengthDER;
		do
			{
			++iLengthLengthDER;
			}
		while (working >>= 8);
		}
	}


// Writes the tag section into the given buffer
void CASN1EncBase::WriteTagDERL(TDes8& aBuf) const
	{
	// Write class - bits 7-8
	aBuf[0] = STATIC_CAST(TUint8, iClass << 6);

	// Write constructed/primitive flag in bit 6
	if (IsConstructed())
		{
		aBuf[0] |= 0x20;
		}

	// Write length
	if (iType <= KMaxLowNumberFormTag)
		{
		aBuf[0] |= iType;
		}
	else
		{
		// First byte signals more to come
		aBuf[0] |= 0x1F;

		// The rest we delegate
		TASN1EncBase128DER encoder(iType);
		TUint offset = 1;
		encoder.WriteDERL(aBuf, offset);
		}
	}


// Writes the length section into the given buffer
void CASN1EncBase::WriteLengthDER(TDes8& aBuf) const
	{
	if (iContentsLengthDER > KMaxShortFormLength)
		{
		// First byte is the number of other bytes, with top bit also set
		TUint octets = iLengthLengthDER - 1;

		// Cast takes 8 least sig bits
		aBuf[0] = STATIC_CAST(TUint8, octets);
		aBuf[0] |= 0x80;

		// Other bytes are base 256 representation of length, most significant first
		TUint working = iContentsLengthDER;
		do
			{
			// Cast to TUint8 takes 8 least sig bits
			aBuf[octets] = STATIC_CAST(TUint8, working);
			working >>= 8;
			}
		while (--octets);
		}
	else
		{
		// Short form, nice and easy - take least sig 8 bits with a cast
		aBuf[0] = STATIC_CAST(TUint8, iContentsLengthDER);
		}
	}


// Sets the parent (can only do this once)
void CASN1EncBase::SetParent(CASN1EncBase* aParent)
	{
	__ASSERT_ALWAYS(iParent == 0, Panic(KErrAlreadyHasParent));
	iParent = aParent;
	}
