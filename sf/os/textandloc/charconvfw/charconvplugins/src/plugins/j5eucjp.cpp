/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* J5 charconv character converter
* converts from EUC_JP to unicode
* based on code in EUCJP_PACKED
*
*/


#include <e32std.h>
#include <charconv.h>
#include <ecom/implementationproxy.h>
#include <convutils.h>
#include "shiftjis.h"
#include "jisbase.h"
#include "jisx0201.h"
#include "jisx0208.h"
#include "jisx0212.h"

const TUint KSingleShift2=0x8e;
const TUint KSingleShift3=0x8f;

		
#if defined(_DEBUG)

_LIT(KLitPanicText, "EUCJP_PACKED");

enum TPanic
	{
	EPanicNothingToConvert1=1,
	EPanicNothingToConvert2,
	EPanicNothingToConvert3,
	EPanicNothingToConvert4,
	EPanicNothingToConvert5,
	EPanicNothingToConvert6,
	EPanicOddNumberOfBytes1,
	EPanicOddNumberOfBytes2,
	EPanicOddNumberOfBytes3,
	EPanicOddNumberOfBytes4,
	EPanicOddNumberOfBytes5,
	EPanicOddNumberOfBytes6,
	EPanicBadHighBit1,
	EPanicBadHighBit2,
	EPanicBadHighBit3,
	EPanicBadHighBit4,
	EPanicBadHighBit5,
	EPanicBadHighBit6,
	EPanicBadHighBit7,
	EPanicBadPointers1,
	EPanicBadPointers2,
	EPanicBadPointers3,
	EPanicBadPointers4,
	EPanicBadPointers5,
	EPanicBadPointers6,
	EPanicBadPointers7,
	EPanicBadPointers8,
	EPanicBadPointers9,
	EPanicBadPointers10,
	EPanicBadPointers11,
	EPanicBadPointers12,
	EPanicBadPointers13,
	EPanicBadPointers14,
	EPanicBadPointers15,
	EPanicBadPointers16,
	EPanicBadPointers17,
	EPanicBadPointers18,
	EPanicBadPointers19,
	EPanicBadPointers20,
	EPanicBadPointers21,
	EPanicBadPointers22,
	EPanicBadPointers23,
	EPanicBadPointers24,
	EPanicBadPointers25,
	EPanicBadPointers26,
	EPanicBadPointers27,
	EPanicBadPointers28,
	EPanicBadPointers29,
	EPanicBadPointers30,
	EPanicBadPointers31,
	EPanicBadPointers32,
	EPanicBadPointers33,
	EPanicBadPointers34,
	EPanicBadPointers35,
	EPanicBadPointers36,
	EPanicBadCalculation1,
	EPanicBadCalculation2,
	EPanicNumberOfBytesIsNotMultipleOfThree1,
	EPanicNumberOfBytesIsNotMultipleOfThree2,
	EPanicSingleShift2Expected,
	EPanicSingleShift3Expected
	};

LOCAL_C void Panic(TPanic aPanic)
	{
	User::Panic(KLitPanicText, aPanic);
	}

#endif
	
/**
 @return The number of bytes that can be converted from aDescriptor to JisRoman
 @internalTechnology 
 */
TInt NumberOfBytesAbleToConvertToJisRoman(const TDesC8& aDescriptor)
	{
	const TUint8* pointerToPreviousByte=aDescriptor.Ptr()-1;
	const TUint8* const pointerToLastByte=pointerToPreviousByte+aDescriptor.Length();
	if (pointerToPreviousByte==pointerToLastByte)
		{
		return 0;
		}
		
	FOREVER
		{
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers14));
		const TUint currentByte=*(pointerToPreviousByte+1);
		if (currentByte&0x80)
			{
			break;
			}
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers15));
		++pointerToPreviousByte;
		__ASSERT_DEBUG(pointerToPreviousByte<=pointerToLastByte, Panic(EPanicBadPointers16));
		if (pointerToPreviousByte>=pointerToLastByte)
			{
			break;
			}
		}
	return (pointerToPreviousByte+1)-aDescriptor.Ptr();
	}

/**
 @return The number of bytes that can be converted from aDescriptor to Jis X208
 @internalTechnology 
 */
 TInt NumberOfBytesAbleToConvertToJisX0208(const TDesC8& aDescriptor)
	{
	const TUint8* pointerToPreviousByte=aDescriptor.Ptr()-1;
	const TUint8* const pointerToLastByte=pointerToPreviousByte+aDescriptor.Length();
	if (pointerToPreviousByte==pointerToLastByte)
		{
		return 0;
		}
	FOREVER
		{
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers17));
		TUint currentByte=*(pointerToPreviousByte+1);
		if (currentByte<0xa0)
			{
			break;
			}
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers18));
		if (pointerToLastByte-pointerToPreviousByte<2)
			{
			break;
			}
		++pointerToPreviousByte;
		currentByte=*(pointerToPreviousByte+1);
		if (currentByte<0xa0)
			{
			return CCnvCharacterSetConverter::EErrorIllFormedInput;
			}
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers19));
		++pointerToPreviousByte;
		__ASSERT_DEBUG(pointerToPreviousByte<=pointerToLastByte, Panic(EPanicBadPointers20));
		if (pointerToPreviousByte>=pointerToLastByte)
			{
			break;
			}
		}
	return (pointerToPreviousByte+1)-aDescriptor.Ptr();
	}
	
/**
 @return The number of bytes that can be converted from aDescriptor to HalfWidthKatakana
 @internalTechnology 
 */
 TInt NumberOfBytesAbleToConvertToHalfWidthKatakana8(const TDesC8& aDescriptor)
	{
	const TUint8* pointerToPreviousByte=aDescriptor.Ptr()-1;
	const TUint8* const pointerToLastByte=pointerToPreviousByte+aDescriptor.Length();
	if (pointerToPreviousByte==pointerToLastByte)
		{
		return 0;
		}
	FOREVER
		{
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers21));
		TUint currentByte=*(pointerToPreviousByte+1);
		if (currentByte!=KSingleShift2)
			{
			break;
			}
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers22));
		if (pointerToLastByte-pointerToPreviousByte<2)
			{
			break;
			}
		++pointerToPreviousByte;
		currentByte=*(pointerToPreviousByte+1);
		if (currentByte<0xa0)
			{
			return CCnvCharacterSetConverter::EErrorIllFormedInput;
			}
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers23));
		++pointerToPreviousByte;
		__ASSERT_DEBUG(pointerToPreviousByte<=pointerToLastByte, Panic(EPanicBadPointers24));
		if (pointerToPreviousByte>=pointerToLastByte)
			{
			break;
			}
		}
	return (pointerToPreviousByte+1)-aDescriptor.Ptr();
	}

/**
 @return The number of bytes that can be converted from aDescriptor to JISX0212
 @internalTechnology 
 */
TInt NumberOfBytesAbleToConvertToJisX0212(const TDesC8& aDescriptor)
	{
	const TUint8* pointerToPreviousByte=aDescriptor.Ptr()-1;
	const TUint8* const pointerToLastByte=pointerToPreviousByte+aDescriptor.Length();
	if (pointerToPreviousByte==pointerToLastByte)
		{
		return 0;
		}
		
	FOREVER
		{
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers25));
		TUint currentByte=*(pointerToPreviousByte+1);
		if (currentByte!=KSingleShift3)
			{
			break;
			}
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers26));
		if (pointerToLastByte-pointerToPreviousByte<3)
			{
			break;
			}
		++pointerToPreviousByte;
		currentByte=*(pointerToPreviousByte+1);
		if (currentByte<0xa0)
			{
			return CCnvCharacterSetConverter::EErrorIllFormedInput;
			}
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers27));
		++pointerToPreviousByte;
		currentByte=*(pointerToPreviousByte+1);
		if (currentByte<0xa0)
			{
			return CCnvCharacterSetConverter::EErrorIllFormedInput;
			}
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers28));
		++pointerToPreviousByte;
		__ASSERT_DEBUG(pointerToPreviousByte<=pointerToLastByte, Panic(EPanicBadPointers29));
		if (pointerToPreviousByte>=pointerToLastByte)
			{
			break;
			}
		}
	return (pointerToPreviousByte+1)-aDescriptor.Ptr();
	}

void DummyConvertToIntermediateBufferInPlace(TDes8&)
	{
	}
	
/**
 Converts to JISX0212
 @internalTechnology 
 */
void ConvertToJisX0212FromEucJpPackedInPlace(TDes8& aDescriptor)
	{
	const TInt descriptorLength=aDescriptor.Length();
	__ASSERT_DEBUG(descriptorLength>0, Panic(EPanicNothingToConvert6));
	__ASSERT_DEBUG(descriptorLength%3==0, Panic(EPanicNumberOfBytesIsNotMultipleOfThree2));
	TUint8* pointerToTargetByte=CONST_CAST(TUint8*, aDescriptor.Ptr());
	const TUint8* pointerToSourceByte=pointerToTargetByte;
	const TUint8* const pointerToLastByte=pointerToSourceByte+(descriptorLength-1);
	FOREVER
		{
		__ASSERT_DEBUG(*pointerToSourceByte==KSingleShift3, Panic(EPanicSingleShift3Expected));
		__ASSERT_DEBUG(pointerToSourceByte<pointerToLastByte, Panic(EPanicBadPointers33));
		++pointerToSourceByte;
		TUint sourceByte=*pointerToSourceByte;
		__ASSERT_DEBUG(sourceByte&0x80, Panic(EPanicBadHighBit6));
		*pointerToTargetByte=STATIC_CAST(TUint8, sourceByte&~0x80);
		__ASSERT_DEBUG(pointerToSourceByte<pointerToLastByte, Panic(EPanicBadPointers34));
		++pointerToSourceByte;
		sourceByte=*pointerToSourceByte;
		__ASSERT_DEBUG(sourceByte&0x80, Panic(EPanicBadHighBit7));
		__ASSERT_DEBUG(pointerToTargetByte<pointerToLastByte, Panic(EPanicBadPointers35));
		++pointerToTargetByte;
		*pointerToTargetByte=STATIC_CAST(TUint8, sourceByte&~0x80);
		__ASSERT_DEBUG(pointerToSourceByte<=pointerToLastByte, Panic(EPanicBadPointers36));
		if (pointerToSourceByte>=pointerToLastByte)
			{
			break;
			}
		++pointerToSourceByte;
		++pointerToTargetByte;
		}
	aDescriptor.SetLength((descriptorLength/3)*2);
	}
	
/**
 Converts to JISX0208
 @internalTechnology 
 */
void ConvertToJisX0208FromEucJpPackedInPlace(TDes8& aDescriptor)
	{
	const TInt descriptorLength=aDescriptor.Length();
	__ASSERT_DEBUG(descriptorLength>0, Panic(EPanicNothingToConvert4));
	__ASSERT_DEBUG(descriptorLength%2==0, Panic(EPanicOddNumberOfBytes5));
	TUint8* pointerToCurrentByte=CONST_CAST(TUint8*, aDescriptor.Ptr());
	const TUint8* const pointerToLastByte=pointerToCurrentByte+(descriptorLength-1);
	FOREVER
		{
		const TUint currentByte=*pointerToCurrentByte;
		__ASSERT_DEBUG(currentByte&0x80, Panic(EPanicBadHighBit4));
		*pointerToCurrentByte=STATIC_CAST(TUint8, currentByte&~0x80);
		__ASSERT_DEBUG(pointerToCurrentByte<=pointerToLastByte, Panic(EPanicBadPointers30));
		if (pointerToCurrentByte>=pointerToLastByte)
			{
			break;
			}
		++pointerToCurrentByte;
		}
	}
	
/**
 Converts to Half Width Katakana
 @internalTechnology 
 */
void ConvertToHalfWidthKatakana8FromEucJpPackedInPlace(TDes8& aDescriptor)
	{
	const TInt descriptorLength=aDescriptor.Length();
	__ASSERT_DEBUG(descriptorLength>0, Panic(EPanicNothingToConvert5));
	__ASSERT_DEBUG(descriptorLength%2==0, Panic(EPanicOddNumberOfBytes6));
	TUint8* pointerToTargetByte=CONST_CAST(TUint8*, aDescriptor.Ptr());
	const TUint8* pointerToSourceByte=pointerToTargetByte;
	const TUint8* const pointerToLastByte=pointerToSourceByte+(descriptorLength-1);
	FOREVER
		{
		__ASSERT_DEBUG(*pointerToSourceByte==KSingleShift2, Panic(EPanicSingleShift2Expected));
		__ASSERT_DEBUG(pointerToSourceByte<pointerToLastByte, Panic(EPanicBadPointers31));
		++pointerToSourceByte;
		const TUint sourceByte=*pointerToSourceByte;
		__ASSERT_DEBUG(sourceByte&0x80, Panic(EPanicBadHighBit5));
		*pointerToTargetByte=STATIC_CAST(TUint8, sourceByte);
		__ASSERT_DEBUG(pointerToSourceByte<=pointerToLastByte, Panic(EPanicBadPointers32));
		if (pointerToSourceByte>=pointerToLastByte)
			{
			break;
			}
		++pointerToSourceByte;
		++pointerToTargetByte;
		}
	aDescriptor.SetLength(descriptorLength/2);
	}
	
		
