/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <convdata.h>
#include <convgeneratedcpp.h>
#include "gb2312.h"
#include "gbk.h"

#define ARRAY_LENGTH(aArray) (sizeof(aArray)/sizeof((aArray)[0]))

const TInt KNumberOfBytesOfWorkingMemory=(21+21+2+1)*sizeof(SCnvConversionData::SOneDirectionData::SRange); // GB2312.CTL defines 21 foreign-to-Unicode ranges and 21 Unicode-to-foreign ranges, and GBK.CTL defines 2 foreign-to-Unicode ranges and 1 Unicode-to-foreign range

_LIT(KLitPanicText, "GBK_SHARED");

enum TPanic
	{
	EPanicTooManyBytesOfWorkingMemoryUsed1=1,
	EPanicTooManyBytesOfWorkingMemoryUsed2
	};

LOCAL_D const SCnvConversionData::SVariableByteData::SRange foreignVariableByteDataRanges[]=
	{
		{
		0x00,
		0x7f,
		0,
		0
		},
		{
		0x80,
		0xff,
		1,
		0
		}
	};

LOCAL_C void Panic(TPanic aPanic)
	{
	User::Panic(KLitPanicText, aPanic);
	}

LOCAL_C void SetUpCompleteGbkConversionData(SCnvConversionData& aCompleteGbkConversionData, TUint8* aWorkingMemory)
	{
	const SCnvConversionData& gb2312ConversionData=CnvGb2312::ConversionData();
	const SCnvConversionData& supplementaryGbkConversionData=conversionData;
	// create a SCnvConversionData that is the combination of gb2312ConversionData and supplementaryGbkConversionData
	aCompleteGbkConversionData.iEndiannessOfForeignCharacters=SCnvConversionData::EFixedBigEndian;
	aCompleteGbkConversionData.iForeignVariableByteData.iNumberOfRanges=ARRAY_LENGTH(foreignVariableByteDataRanges);
	aCompleteGbkConversionData.iForeignVariableByteData.iRangeArray=foreignVariableByteDataRanges;
	TInt numberOfBytesOfWorkingMemoryUsed=0;
	// set up the foreign-to-Unicode data
	const TInt numberOfForeignToUnicodeDataRanges=gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges+supplementaryGbkConversionData.iForeignToUnicodeData.iNumberOfRanges;
	aCompleteGbkConversionData.iForeignToUnicodeData.iNumberOfRanges=numberOfForeignToUnicodeDataRanges;
	SCnvConversionData::SOneDirectionData::SRange* foreignToUnicodeDataRangeArray=REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange*, aWorkingMemory+numberOfBytesOfWorkingMemoryUsed);
	numberOfBytesOfWorkingMemoryUsed+=(numberOfForeignToUnicodeDataRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	__ASSERT_ALWAYS(numberOfBytesOfWorkingMemoryUsed<=KNumberOfBytesOfWorkingMemory, Panic(EPanicTooManyBytesOfWorkingMemoryUsed1));
	aCompleteGbkConversionData.iForeignToUnicodeData.iRangeArray=foreignToUnicodeDataRangeArray;
	Mem::Copy(foreignToUnicodeDataRangeArray, gb2312ConversionData.iForeignToUnicodeData.iRangeArray, gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	Mem::Copy(foreignToUnicodeDataRangeArray+gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges, supplementaryGbkConversionData.iForeignToUnicodeData.iRangeArray, supplementaryGbkConversionData.iForeignToUnicodeData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	// set up the Unicode-to-foreign data
	const TInt numberOfUnicodeToForeignDataRanges=gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges+supplementaryGbkConversionData.iUnicodeToForeignData.iNumberOfRanges;
	aCompleteGbkConversionData.iUnicodeToForeignData.iNumberOfRanges=numberOfUnicodeToForeignDataRanges;
	SCnvConversionData::SOneDirectionData::SRange* unicodeToForeignDataRangeArray=REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange*, aWorkingMemory+numberOfBytesOfWorkingMemoryUsed);
	numberOfBytesOfWorkingMemoryUsed+=(numberOfUnicodeToForeignDataRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	__ASSERT_ALWAYS(numberOfBytesOfWorkingMemoryUsed<=KNumberOfBytesOfWorkingMemory, Panic(EPanicTooManyBytesOfWorkingMemoryUsed2));
	aCompleteGbkConversionData.iUnicodeToForeignData.iRangeArray=unicodeToForeignDataRangeArray;
	Mem::Copy(unicodeToForeignDataRangeArray, gb2312ConversionData.iUnicodeToForeignData.iRangeArray, gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	Mem::Copy(unicodeToForeignDataRangeArray+gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges, supplementaryGbkConversionData.iUnicodeToForeignData.iRangeArray, supplementaryGbkConversionData.iUnicodeToForeignData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	}

EXPORT_C const TDesC8& CnvGbk::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return ReplacementForUnconvertibleUnicodeCharacters_internal();
	}

EXPORT_C const SCnvConversionData& CnvGbk::ConversionData()
	{
	return conversionData;
	}

EXPORT_C TInt CnvGbk::ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, TDes8& aForeign, const TDesC16& aUnicode, CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	TUint notUsed;
	return ConvertFromUnicode(aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters, notUsed, 0);
	}

EXPORT_C TInt CnvGbk::ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, TDes8& aForeign, const TDesC16& aUnicode, CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters, TUint& aOutputConversionFlags, TUint aInputConversionFlags)
	{
	SCnvConversionData completeGbkConversionData;
	TUint8 workingMemory[KNumberOfBytesOfWorkingMemory]; // 900 bytes on the stack is just about tolerable (no other functions called from here will make any significant use of the stack)
	SetUpCompleteGbkConversionData(completeGbkConversionData, workingMemory);
	return CCnvCharacterSetConverter::DoConvertFromUnicode(completeGbkConversionData, aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters, aOutputConversionFlags, aInputConversionFlags);
	}

EXPORT_C TInt CnvGbk::ConvertToUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aForeign, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	TUint notUsed;
	return ConvertToUnicode(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter, notUsed, 0);
	}

EXPORT_C TInt CnvGbk::ConvertToUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aForeign, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, TUint& aOutputConversionFlags, TUint aInputConversionFlags)
	{
	SCnvConversionData completeGbkConversionData;
	TUint8 workingMemory[KNumberOfBytesOfWorkingMemory]; // 900 bytes on the stack is just about tolerable (no other functions called from here will make any significant use of the stack)
	SetUpCompleteGbkConversionData(completeGbkConversionData, workingMemory);
	return CCnvCharacterSetConverter::DoConvertToUnicode(completeGbkConversionData, aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter, aOutputConversionFlags, aInputConversionFlags);
	}

EXPORT_C TBool CnvGbk::IsCharGBBased(TInt& aConfidenceLevel, const TDesC8& aSample)
	{
	return CnvGb2312::IsCharGBBased(aConfidenceLevel, aSample);
	}
