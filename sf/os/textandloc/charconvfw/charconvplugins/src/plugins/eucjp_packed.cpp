/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <e32std.h>
#include <charconv.h>
#include <convutils.h>
#include "jisx0201.h"
#include "jisx0208.h"
#include "jisx0212.h"
#include <ecom/implementationproxy.h>
#include <charactersetconverter.h>

const TUint KSingleShift2=0x8e;
const TUint KSingleShift3=0x8f;

_LIT8(KLit8EucJpPackedReplacementForUnconvertibleUnicodeCharacters, "\xa1\xa9"); // fullwidth question mark

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


class CEucjpPackedConverterImpl : public CCharacterSetConverterPluginInterface
	{

public:
	virtual const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();

	virtual TInt ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters);

	virtual TInt ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& aState, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter);

	virtual TBool IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample);

	static CEucjpPackedConverterImpl* NewL();
	virtual ~CEucjpPackedConverterImpl();

private:
	CEucjpPackedConverterImpl();

	};





const TDesC8& CEucjpPackedConverterImpl::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return KLit8EucJpPackedReplacementForUnconvertibleUnicodeCharacters;
	}

LOCAL_C void DummyConvertFromIntermediateBufferInPlace(TInt, TDes8&, TInt& aNumberOfCharactersThatDroppedOut)
	{
	aNumberOfCharactersThatDroppedOut=0;
	}

LOCAL_C void ConvertFromJisX0208ToEucJpPackedInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	aNumberOfCharactersThatDroppedOut=0;
	const TInt descriptorLength=aDescriptor.Length();
	__ASSERT_DEBUG(descriptorLength>aStartPositionInDescriptor, Panic(EPanicNothingToConvert1));
	__ASSERT_DEBUG((descriptorLength-aStartPositionInDescriptor)%2==0, Panic(EPanicOddNumberOfBytes1));
	TUint8* pointerToCurrentByte=CONST_CAST(TUint8*, aDescriptor.Ptr());
	const TUint8* const pointerToLastByte=pointerToCurrentByte+(descriptorLength-1);
	pointerToCurrentByte+=aStartPositionInDescriptor;
	FOREVER
		{
		const TUint currentByte=*pointerToCurrentByte;
		__ASSERT_DEBUG((currentByte&0x80)==0, Panic(EPanicBadHighBit1));
		*pointerToCurrentByte=STATIC_CAST(TUint8, currentByte|0x80);
		__ASSERT_DEBUG(pointerToCurrentByte<=pointerToLastByte, Panic(EPanicBadPointers1));
		if (pointerToCurrentByte>=pointerToLastByte)
			{
			break;
			}
		++pointerToCurrentByte;
		}
	}

LOCAL_C void ConvertFromHalfWidthKatakana8ToEucJpPackedInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	TInt descriptorLength=aDescriptor.Length();
	__ASSERT_DEBUG(descriptorLength>aStartPositionInDescriptor, Panic(EPanicNothingToConvert2));
	aNumberOfCharactersThatDroppedOut=Max(0, (descriptorLength-aStartPositionInDescriptor)-((aDescriptor.MaxLength()-aStartPositionInDescriptor)/2));
	descriptorLength-=aNumberOfCharactersThatDroppedOut;
	__ASSERT_DEBUG(descriptorLength>=aStartPositionInDescriptor, Panic(EPanicBadCalculation1));
	if (descriptorLength<=aStartPositionInDescriptor)
		{
		aDescriptor.SetLength(descriptorLength);
		}
	else
		{
		TUint8* pointerToTargetByte=CONST_CAST(TUint8*, aDescriptor.Ptr()); // pointerToTargetByte is initialized properly when descriptorLength has been offset to the actual final length of aDescriptor
		const TUint8* const pointerToFirstByte=pointerToTargetByte+aStartPositionInDescriptor;
		const TUint8* pointerToSourceByte=pointerToTargetByte+(descriptorLength-1);
		descriptorLength=((descriptorLength-aStartPositionInDescriptor)*2)+aStartPositionInDescriptor;
		__ASSERT_DEBUG((descriptorLength-aStartPositionInDescriptor)%2==0, Panic(EPanicOddNumberOfBytes2));
		aDescriptor.SetLength(descriptorLength);
		pointerToTargetByte+=descriptorLength-1; // pointerToTargetByte is is initialized properly here
		FOREVER
			{
			*pointerToTargetByte=*pointerToSourceByte;
			__ASSERT_DEBUG(pointerToTargetByte>pointerToFirstByte, Panic(EPanicBadPointers2));
			--pointerToTargetByte;
			*pointerToTargetByte=KSingleShift2;
			__ASSERT_DEBUG(pointerToTargetByte>=pointerToFirstByte, Panic(EPanicBadPointers3));
			if (pointerToTargetByte<=pointerToFirstByte)
				{
				break;
				}
			--pointerToTargetByte;
			__ASSERT_DEBUG(pointerToSourceByte>pointerToFirstByte, Panic(EPanicBadPointers4));
			--pointerToSourceByte;
			}
		__ASSERT_DEBUG(pointerToTargetByte==pointerToFirstByte, Panic(EPanicBadPointers5));
		__ASSERT_DEBUG(pointerToSourceByte==pointerToFirstByte, Panic(EPanicBadPointers6));
		}
	}

LOCAL_C void ConvertFromJisX0212ToEucJpPackedInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	TInt descriptorLength=aDescriptor.Length();
	__ASSERT_DEBUG(descriptorLength>aStartPositionInDescriptor, Panic(EPanicNothingToConvert3));
	__ASSERT_DEBUG((descriptorLength-aStartPositionInDescriptor)%2==0, Panic(EPanicOddNumberOfBytes3));
	aNumberOfCharactersThatDroppedOut=Max(0, ((descriptorLength-aStartPositionInDescriptor)/2)-((aDescriptor.MaxLength()-aStartPositionInDescriptor)/3));
	descriptorLength-=aNumberOfCharactersThatDroppedOut*2;
	__ASSERT_DEBUG(descriptorLength>=aStartPositionInDescriptor, Panic(EPanicBadCalculation2));
	if (descriptorLength<=aStartPositionInDescriptor)
		{
		aDescriptor.SetLength(descriptorLength);
		}
	else
		{
		__ASSERT_DEBUG((descriptorLength-aStartPositionInDescriptor)%2==0, Panic(EPanicOddNumberOfBytes4));
		TUint8* pointerToTargetByte=CONST_CAST(TUint8*, aDescriptor.Ptr()); // pointerToTargetByte is initialized properly when descriptorLength has been offset to the actual final length of aDescriptor
		const TUint8* const pointerToFirstByte=pointerToTargetByte+aStartPositionInDescriptor;
		const TUint8* pointerToSourceByte=pointerToTargetByte+(descriptorLength-1);
		descriptorLength=(((descriptorLength-aStartPositionInDescriptor)*3)/2)+aStartPositionInDescriptor;
		__ASSERT_DEBUG((descriptorLength-aStartPositionInDescriptor)%3==0, Panic(EPanicNumberOfBytesIsNotMultipleOfThree1));
		aDescriptor.SetLength(descriptorLength);
		pointerToTargetByte+=descriptorLength-1; // pointerToTargetByte is is initialized properly here
		FOREVER
			{
			__ASSERT_DEBUG((*pointerToSourceByte&0x80)==0, Panic(EPanicBadHighBit2));
			*pointerToTargetByte=STATIC_CAST(TUint8, *pointerToSourceByte|0x80);
			__ASSERT_DEBUG(pointerToTargetByte>pointerToFirstByte, Panic(EPanicBadPointers7));
			--pointerToTargetByte;
			__ASSERT_DEBUG(pointerToSourceByte>pointerToFirstByte, Panic(EPanicBadPointers8));
			--pointerToSourceByte;
			__ASSERT_DEBUG((*pointerToSourceByte&0x80)==0, Panic(EPanicBadHighBit3));
			*pointerToTargetByte=STATIC_CAST(TUint8, *pointerToSourceByte|0x80);
			__ASSERT_DEBUG(pointerToTargetByte>pointerToFirstByte, Panic(EPanicBadPointers9));
			--pointerToTargetByte;
			*pointerToTargetByte=KSingleShift3;
			__ASSERT_DEBUG(pointerToTargetByte>=pointerToFirstByte, Panic(EPanicBadPointers10));
			if (pointerToTargetByte<=pointerToFirstByte)
				{
				break;
				}
			--pointerToTargetByte;
			__ASSERT_DEBUG(pointerToSourceByte>pointerToFirstByte, Panic(EPanicBadPointers11));
			--pointerToSourceByte;
			}
		__ASSERT_DEBUG(pointerToTargetByte==pointerToFirstByte, Panic(EPanicBadPointers12));
		__ASSERT_DEBUG(pointerToSourceByte==pointerToFirstByte, Panic(EPanicBadPointers13));
		}
	}

TInt CEucjpPackedConverterImpl::ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	TFixedArray<CnvUtilities::SCharacterSet, 4> characterSets;
	characterSets[0].iConversionData=&CnvJisRoman::ConversionData();
	characterSets[0].iConvertFromIntermediateBufferInPlace=DummyConvertFromIntermediateBufferInPlace;
	characterSets[0].iEscapeSequence=&KNullDesC8;
	characterSets[1].iConversionData=&CnvJisX0208::ConversionData();
	characterSets[1].iConvertFromIntermediateBufferInPlace=ConvertFromJisX0208ToEucJpPackedInPlace;
	characterSets[1].iEscapeSequence=&KNullDesC8;
	characterSets[2].iConversionData=&CnvHalfWidthKatakana8::ConversionData();
	characterSets[2].iConvertFromIntermediateBufferInPlace=ConvertFromHalfWidthKatakana8ToEucJpPackedInPlace;
	characterSets[2].iEscapeSequence=&KNullDesC8;
	characterSets[3].iConversionData=&CnvJisX0212::ConversionData();
	characterSets[3].iConvertFromIntermediateBufferInPlace=ConvertFromJisX0212ToEucJpPackedInPlace;
	characterSets[3].iEscapeSequence=&KNullDesC8;
	return CnvUtilities::ConvertFromUnicode(aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters, characterSets.Array());
	}

LOCAL_C TInt NumberOfBytesAbleToConvertToJisRoman(const TDesC8& aDescriptor)
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

LOCAL_C TInt NumberOfBytesAbleToConvertToJisX0208(const TDesC8& aDescriptor)
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

LOCAL_C TInt NumberOfBytesAbleToConvertToHalfWidthKatakana8(const TDesC8& aDescriptor)
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

LOCAL_C TInt NumberOfBytesAbleToConvertToJisX0212(const TDesC8& aDescriptor)
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

LOCAL_C void DummyConvertToIntermediateBufferInPlace(TDes8&)
	{
	}

LOCAL_C void ConvertToJisX0208FromEucJpPackedInPlace(TDes8& aDescriptor)
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

LOCAL_C void ConvertToHalfWidthKatakana8FromEucJpPackedInPlace(TDes8& aDescriptor)
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

LOCAL_C void ConvertToJisX0212FromEucJpPackedInPlace(TDes8& aDescriptor)
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

TInt CEucjpPackedConverterImpl::ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& /*aState*/, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	TFixedArray<CnvUtilities::SMethod, 4> methods;
	methods[0].iNumberOfBytesAbleToConvert=NumberOfBytesAbleToConvertToJisRoman;
	methods[0].iConvertToIntermediateBufferInPlace=DummyConvertToIntermediateBufferInPlace;
	methods[0].iConversionData=&CnvJisRoman::ConversionData();
	methods[0].iNumberOfBytesPerCharacter=1;
	methods[0].iNumberOfCoreBytesPerCharacter=1;
	methods[1].iNumberOfBytesAbleToConvert=NumberOfBytesAbleToConvertToJisX0208;
	methods[1].iConvertToIntermediateBufferInPlace=ConvertToJisX0208FromEucJpPackedInPlace;
	methods[1].iConversionData=&CnvJisX0208::ConversionData();
	methods[1].iNumberOfBytesPerCharacter=2;
	methods[1].iNumberOfCoreBytesPerCharacter=2;
	methods[2].iNumberOfBytesAbleToConvert=NumberOfBytesAbleToConvertToHalfWidthKatakana8;
	methods[2].iConvertToIntermediateBufferInPlace=ConvertToHalfWidthKatakana8FromEucJpPackedInPlace;
	methods[2].iConversionData=&CnvHalfWidthKatakana8::ConversionData();
	methods[2].iNumberOfBytesPerCharacter=2;
	methods[2].iNumberOfCoreBytesPerCharacter=1;
	methods[3].iNumberOfBytesAbleToConvert=NumberOfBytesAbleToConvertToJisX0212;
	methods[3].iConvertToIntermediateBufferInPlace=ConvertToJisX0212FromEucJpPackedInPlace;
	methods[3].iConversionData=&CnvJisX0212::ConversionData();
	methods[3].iNumberOfBytesPerCharacter=3;
	methods[3].iNumberOfCoreBytesPerCharacter=2;
	return CnvUtilities::ConvertToUnicodeFromHeterogeneousForeign(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter, methods.Array());
	}

TBool CEucjpPackedConverterImpl::IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample)
	{
	aSetToTrue=ETrue;
	// check for the SS2 and SS3 which specifies Code Set 2 & 3 respectively from the Code space
	// between 00-7f only... then ambiguous 
	aSetToTrue = ETrue; 
	TInt sampleLength = aSample.Length();
	TInt eucjpPacked = 0;
	aConfidenceLevel = 0;

	for (TInt i = 0; i < sampleLength; ++i)
		{
		// Code set 1 JISX 0208 support
		TInt increment1 = i+1;
		if (increment1 >= sampleLength)
			break;
		if (((aSample[i] >= 0xa1) && (aSample[i] <= 0xfe)) && 
			((aSample[increment1] >= 0xa1) && (aSample[increment1] <= 0xfe)))
			{
			eucjpPacked = eucjpPacked +2;
			i++;
			}
		// Single Shift 2 (SS2) sequence - Code Set 2
		if (aSample[i]==0x8e)
			{
			TInt increment1 = i+1;
			if (increment1 >= sampleLength)
				break;
			if ((aSample[increment1] >= 0xa1) && (aSample[increment1] <= 0xdf))
				{
				eucjpPacked = eucjpPacked+2;
				i++;
				}
			else
				{
				eucjpPacked = 0;
				break;
				}
			}
		// Single Shift 3 (SS3) sequence - Code Set 3
		if (aSample[i]==0x8f)
			{
			TInt increment1 = i+1;
			TInt increment2 = i+2;
			if ((increment1 >= sampleLength) || (increment2 >= sampleLength))
				break;
			if (((aSample[increment1] >= 0xa1) && (aSample[increment1] <= 0xfe)) && 
				((aSample[increment2] >= 0xa1) && (aSample[increment2] <= 0xfe)))
				{
				eucjpPacked = eucjpPacked +3;
				i+=2;
				}
			else
				{
				eucjpPacked =0;
				break;
				}
			}
		}
	if (eucjpPacked)
		aConfidenceLevel = (eucjpPacked*100)/sampleLength;
	else
		aConfidenceLevel = 0;
	aConfidenceLevel=(aConfidenceLevel >100)?100:aConfidenceLevel; 
	return ETrue;
	}

CEucjpPackedConverterImpl* CEucjpPackedConverterImpl::NewL()
	{
	CEucjpPackedConverterImpl* self = new(ELeave) CEucjpPackedConverterImpl();
	return self;
	}

CEucjpPackedConverterImpl::~CEucjpPackedConverterImpl()
	{
	}

CEucjpPackedConverterImpl::CEucjpPackedConverterImpl()
	{
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10006067,CEucjpPackedConverterImpl::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	
