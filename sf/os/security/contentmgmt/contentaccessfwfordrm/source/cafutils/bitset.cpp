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


#ifndef REMOVE_CAF1

#include <e32std.h>
#include <s32strm.h>
#include <stdarg.h>
#include <caf/caftypes.h>
#include <caf/bitset.h>

using namespace ContentAccess;

EXPORT_C CBitset* CBitset::NewLC(TInt aMaxBits)
	{
	CBitset* self = new(ELeave) CBitset();
	CleanupStack::PushL(self);
	self->ConstructL(aMaxBits);
	return self;
	}

EXPORT_C CBitset* CBitset::NewL(TInt aMaxBits)
	{
	CBitset* self=CBitset::NewLC(aMaxBits);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CBitset* CBitset::NewLC(const CBitset& aSource)
	{
	CBitset* self = new (ELeave) CBitset(aSource);
	CleanupStack::PushL(self);
	self->ConstructL(aSource);
	return self;
	}

EXPORT_C CBitset* CBitset::NewL(const CBitset& aSource)
	{
	CBitset* self=CBitset::NewLC(aSource);
	CleanupStack::Pop(self);
	return self;
	}

CBitset::~CBitset() 
	{ 
	delete [] iBitSet;
	}

CBitset::CBitset()
	{
	}

CBitset::CBitset(const CBitset& aSource)
	: iWidth(aSource.iWidth), iMaxBits(aSource.iMaxBits)
	{
	}

void CBitset::IdentifyBit(TInt aBitId, // in
						  TInt &aByte, // out
						  TInt &aBitInByte) const // out
	{
	// Identify the byte & bit corresponding to the Id
	aByte = aBitId / 8;
	aBitInByte  = aBitId % 8;
	}

EXPORT_C void CBitset::Invert()
	{
	// Invert array
	for (TInt j = 0; j < iWidth ; j++)
		{
		iBitSet[j] = static_cast<TUint8> (~iBitSet[j]);
		}
	}

EXPORT_C TInt CBitset::MaxBits() const
	{
	return iMaxBits;
	}

EXPORT_C void CBitset::Set(TInt aBit)
	{
	__ASSERT_ALWAYS(ValidBit(aBit), User::Panic(KCafUtilsPanic, EInvalidBit));
	
	TInt targetByte;
	TInt targetBit;
	IdentifyBit(aBit, targetByte, targetBit);

	// Or-in the relevant bit
	iBitSet[targetByte] |= (1 << targetBit);
	}

EXPORT_C void CBitset::Unset(TInt aBit)
	{
	__ASSERT_ALWAYS(ValidBit(aBit), User::Panic(KCafUtilsPanic, EInvalidBit));
	TInt targetByte;
	TInt targetBit;
	IdentifyBit(aBit, targetByte, targetBit);

	// And-in NOT(the relevant bit)
	iBitSet[targetByte] &= ~(1 << targetBit);
	}

EXPORT_C TBool CBitset::IsSet(TInt aBit) const
	{
	__ASSERT_ALWAYS(ValidBit(aBit), User::Panic(KCafUtilsPanic, EInvalidBit));
	
	TInt targetByte;
	TInt targetBit;
	IdentifyBit(aBit, targetByte, targetBit);

	// And-in the relevant bit and test for non-zero
	if (iBitSet[targetByte] & (1 << targetBit))
		{
		return(ETrue);
		}
	else
		{
		return(EFalse);
		}
	}

EXPORT_C void CBitset::Reset()
	{
	// Reset the array
	Mem::FillZ(iBitSet, iWidth);
	}

EXPORT_C void CBitset::SetAll()
	{
	Mem::Fill(iBitSet, iWidth, 0xFF);
	}

void CBitset::ConstructL(TInt aMaxBits)
	{
	// Calculate the required number of bytes
	iMaxBits = aMaxBits;
	iWidth          = 1 + aMaxBits / 8;

	iBitSet = new (ELeave) TUint8[iWidth];
	Reset(); // Zero the set
    }

void CBitset::ConstructL(const CBitset& aSource)
	{
	// Copy the bits of the source
	iBitSet = new (ELeave) TUint8[iWidth];
	Mem::Copy(iBitSet, aSource.iBitSet, iWidth);
    }

EXPORT_C void CBitset::ExternalizeL(RWriteStream& aStream) const
	{
	// Write the max-Id
	aStream.WriteInt32L(iMaxBits);
	// Create a descriptor to encapsulate the set
	TPtrC8 ptr(iBitSet, iWidth);
	// Write the descriptor
	aStream.WriteL(ptr);
	}

EXPORT_C void CBitset::InternalizeL(RReadStream& aStream)
	{
	// Delete the array 
	delete [] iBitSet;
	iBitSet = NULL; // safety
	// Call the constructor with the max-Id value
	ConstructL(aStream.ReadInt32L());
	// Create a descriptor to encapsulate the set
	TPtr8 ptr(iBitSet, 0, iWidth);
	// Load into the descriptor
	aStream.ReadL(ptr);
	}

//
// NB: leavescan will complain that this method calls a leaving
// function. The Leave is properly documented in the header
// file. Other than that, not much can be done, especially this
// class has already been deprecated.
EXPORT_C CBitset& CBitset::operator=(const CBitset& aSource)
	{
	// avoid self-assignment
	if (this == &aSource)
		{
		return *this;
		}
	
	// check for source/dest size mismatch
	if (aSource.iWidth != iWidth)
		{
		// always use largest width
		TInt widest = iWidth; 
		if (widest < aSource.iWidth)
			{
			widest = aSource.iWidth;
			}
		
		// Malloc the desired width
		TUint8 *temp = (TUint8 *) new (ELeave) TUint8[widest];
		
		// And zero
		Mem::FillZ(temp, widest);
			  
		// Now it is safe to re-assign the set
		delete [] iBitSet;
		iBitSet = temp;
		iWidth = widest;
		}

	// Width may be the same, but the Max-Id may not, so
	// align these here
	iMaxBits = aSource.iMaxBits;

	// Based on the source-size, copy the bitset
	Mem::Copy(iBitSet, aSource.iBitSet, aSource.iWidth);

	return *this;
	}

EXPORT_C TBool CBitset::operator==(const CBitset& aSource) const
	{
	TBool result = ETrue;
	if (this == &aSource)
		{
		return ETrue;
		}
	TInt narrowest = iMaxBits; 
	if (narrowest > aSource.iMaxBits)
		{
		narrowest = aSource.iMaxBits;
		}

	// Compare the first "narrowest/8" bytes - if the result is non-zero,
	// then they are not equal
	if (Mem::Compare(iBitSet, narrowest/8, aSource.iBitSet, narrowest/8))
		{
		return EFalse;
		}
	
	// Now, compare the last set of bits manually
	for (TInt i = narrowest/8 * 8; i < narrowest; ++i)
		{
		// Comparison of booleans - compare their negations
		if (!IsSet(i) != !aSource.IsSet(i))
			{
			return EFalse;
			}
		}
		
	return result;
	}

EXPORT_C void CBitset::SetListL(TInt aItems, ...)
	{
	va_list ap;
	va_start(ap, aItems);

	// Create a new set (of identical size)
	CBitset *tempSet = CBitset::NewLC(iMaxBits);

	// Duplicate this into temporary bitset
	*tempSet = *this;
	
	for (TInt i = 0; i < aItems; i++)
		{
		TInt x = va_arg(ap, TInt);
		tempSet->Set(x);
		}
	
	// No failure, so copy back into this
	*this = *tempSet;
	va_end(ap);
	
	CleanupStack::Pop(tempSet);
	delete tempSet;
	}

EXPORT_C TBool CBitset::IsSetList(TInt aItems, ...) const
	{
	va_list ap;
	va_start(ap, aItems);

	for (TInt i = 0; i < aItems; i++)
		{
		TInt x = va_arg(ap, TInt);
		if (!IsSet(x))
			{
			return (EFalse);
			}
		}
	va_end(ap);
	return (ETrue);
	}

#endif // #ifndef REMOVE_CAF1
