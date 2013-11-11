/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  GB18030 converter implementation
*
*/


#include <e32std.h>
#include <charconv.h>
#include <convutils.h>
#include <convdata.h>
#include "gb2312.h"
#include "gbk.h"
#include "gb18030_4byte.h"
#include "gb18030_diff_gbk.h"
#include <ecom/implementationproxy.h>
#include <charactersetconverter.h>

class CGB18030ConverterImpl : public CCharacterSetConverterPluginInterface
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

	static CGB18030ConverterImpl* NewL();
	virtual ~CGB18030ConverterImpl();

private:
	CGB18030ConverterImpl();
	TInt ConstructL();
	SCnvConversionData * completeGb18030_2byteConversionData; // a merged conversion data including Gb18030-diff-g2312, GB2312, Gb18030-diff-gbk and Gbk
	TUint8 * workingMemory;
	};

// Implement gb18030 plug-in using cnvutils framework in which gb2312 and gbk conversion data is re-used for memory saving 
// 1) foreign->unicode:
//    1.1) 1 byte->unicode bmp: use gb2312 mapping table;
//
//    1.2) 2 byte->unicode bmp: use gb18030-2byte mapping table (gb18030_diff_gb2312ConversionData + gb2312ConversionData + gb18030_diff_gbkConversionData + gbkConversionData);
//
//    1.3) 4 byte->unicode bmp: use gb18030-4byte mapping table;
//
//    1.4) 4 byte->unicode non-bmp: calculate with formula.
//
// 2) unicode->foreign:
//    2.1) firstly check gb18030-2byte mapping table (gb18030_diff_gb2312ConversionData + gb2312ConversionData + gb18030_diff_gbkConversionData + gbkConversionData);
//
//    2.2) if not found in 2.1), check gb18030-4byte mapping table;
//
//    2.3) if not found in 2.2), calculate with formula (gb18030-4byte non BMP);
//


// GB18030-diff-gb2312 defines 1 foreign-to-Unicode range and 2 unicode-to-Foreign range
// GB2312.CTL defines 21 foreign-to-Unicode ranges and 21 Unicode-to-foreign ranges
// GB18030-diff-gbk defines 1 foreign-to-Unicode ranges and 2 Unicode-to-foreign range
// GBK.CTL defines 2 foreign-to-Unicode ranges and 2 Unicode-to-foreign range
const TInt KNumberOfBytesOfWorkingMemory=(1+2+21+21+1+2+2+2)*sizeof(SCnvConversionData::SOneDirectionData::SRange);  //totally 1040 bytes

const TDesC8& CGB18030ConverterImpl::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return CnvGb18030_diff_gbk::ReplacementForUnconvertibleUnicodeCharacters();
	}

_LIT(KLitPanicText, "GB18030");
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
	EPanicSingleShift3Expected,
	EPanicTooManyBytesOfWorkingMemoryUsed1,
	EPanicTooManyBytesOfWorkingMemoryUsed2
	};

LOCAL_C void Panic(TPanic aPanic)
	{
	User::Panic(KLitPanicText, aPanic);
	}

#define ARRAY_LENGTH(aArray) (sizeof(aArray)/sizeof((aArray)[0]))

LOCAL_C void Step12DummyConvertFromIntermediateBufferInPlace(TInt, TDes8&, TInt& aNumberOfCharactersThatDroppedOut)
	{
	aNumberOfCharactersThatDroppedOut=0;
	}

// Perform the actual conversion (unicode -> gb18030 4byte non-BMP) using formula in this function
LOCAL_C void Step3ConvertFromIntermediateBufferInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	aNumberOfCharactersThatDroppedOut = 0;		// no drop out, because all GB18030 outside BMP are exactly 4-bytes
	
	const TInt descriptorLength=aDescriptor.Length();
	TUint8* pVeryFrom = CONST_CAST(TUint8*, aDescriptor.Ptr());
	const TUint8* pEnd = pVeryFrom + descriptorLength;
	TUint8* pFrom = pVeryFrom + aStartPositionInDescriptor;
	FOREVER
		{
		if (pFrom + 4 > pEnd)
			{
			__ASSERT_DEBUG(pFrom==pEnd, Panic(EPanicBadPointers25));
			break;
			}
		TUint characterCode = 0;
		for (TInt i=0; i<4; i++)
			{
			characterCode <<= 8;
			characterCode += pFrom[i];
			}
		
		// to gb18030
		characterCode -= 0x10000;
		TUint b4 = characterCode % 10 + 0x30;
		characterCode /= 10;
		TUint b3 = characterCode % 126 + 0x81;
		characterCode /= 126;
		TUint b2 = characterCode % 10 + 0x30;
		TUint b1 = characterCode / 10 + 0x90;
		
		*pFrom++ = b1;
		*pFrom++ = b2;
		*pFrom++ = b3;
		*pFrom++ = b4;
		}
	aDescriptor.SetLength(pFrom-pVeryFrom);
	}

// gb2312-1byte ->unicode (0x00 - 0x7F)
LOCAL_C TInt Step0NumberOfBytesAbleToConvertToUnicode(const TDesC8& aDescriptor)
	{
	const TInt descriptorLength=aDescriptor.Length();
	const TUint8* pointerToPreviousByte=aDescriptor.Ptr()-1;
	const TUint8* const pointerToLastByte=pointerToPreviousByte+aDescriptor.Length();
	
	TInt numOfBytes = 0;
	FOREVER
		{
		if (pointerToPreviousByte>=pointerToLastByte)
			{
			break;
			}
		// byte 1
		TUint b1 = pointerToPreviousByte[1];
		if (b1 <= 0x7F)
			{
			pointerToPreviousByte++;
			numOfBytes++;
			}
		else
			break;
		}
		return numOfBytes;
	}

// gb18030-2byte --> unicode (0x8140 - 0xFE7E, 0x8180 - 0xFEFE)
LOCAL_C TInt Step1NumberOfBytesAbleToConvertToUnicode(const TDesC8& aDescriptor)
	{
	const TInt descriptorLength=aDescriptor.Length();
	const TUint8* pointerToPreviousByte=aDescriptor.Ptr()-1;
	const TUint8* const pointerToLastByte=pointerToPreviousByte+aDescriptor.Length();

	TInt numOfBytes = 0;
	FOREVER
		{
		if (pointerToPreviousByte>=pointerToLastByte)
			{
			break;
			}
		// byte 1
		TUint b1 = pointerToPreviousByte[1];
		if (b1 <= 0x80 || b1 > 0xFE)
			break;

		// byte 2
		if (pointerToPreviousByte+1 >= pointerToLastByte)
			break;
		TUint b2 = pointerToPreviousByte[2];
		if (b2 >= 0x40 && b2 <= 0xFE && b2 != 0x7F)		// all gb18030 2-byte code
			{
			pointerToPreviousByte = pointerToPreviousByte + 2;
			numOfBytes = numOfBytes + 2;
			}
		else if (b2 < 0x30 || b2 > 0x39)
			{
			if (numOfBytes <= 0)
				return CCnvCharacterSetConverter::EErrorIllFormedInput;
			else
				break;
			}
		else
			break;
		}
		return numOfBytes;
	}


// gb18030 4-bytes bmp --> unicode (0x81308130 - 0x8439FE39)
LOCAL_C TInt Step2NumberOfBytesAbleToConvertToUnicode(const TDesC8& aDescriptor)
	{
	const TUint8* pointerToPreviousByte=aDescriptor.Ptr()-1;
	const TUint8* const pointerToLastByte=pointerToPreviousByte+aDescriptor.Length();
	__ASSERT_DEBUG(pointerToPreviousByte<=pointerToLastByte, Panic(EPanicBadPointers25));
	
	TInt numOfBytes = 0;
	FOREVER
		{
		if (pointerToPreviousByte>=pointerToLastByte)
			{
			break;
			}
	
		// byte 1
		TUint b1 = pointerToPreviousByte[1];
		if ((b1 < 0x81) || (b1 > 0x84)){
			break;
		}
			
		// byte 2
		if (pointerToPreviousByte+1 >= pointerToLastByte)
			break;
		TUint b2 = pointerToPreviousByte[2];
		if (b2 >= 0x40 && b2 <= 0xFE && b2 != 0x7F)		// all gb18030 2-byte code
			break;
		else if (b2 < 0x30 || b2 > 0x39)
			{
			if (numOfBytes == 0)
				return CCnvCharacterSetConverter::EErrorIllFormedInput;
			else 
				break;
			}

		
		// byte 3
		if (pointerToPreviousByte+2 >= pointerToLastByte)
			break;
		TUint b3 = pointerToPreviousByte[3];
		if (b3 < 0x81 || b3 > 0xFE)
			{
			if (numOfBytes == 0)
				return CCnvCharacterSetConverter::EErrorIllFormedInput;
			else 
				break;
			}
		
		// byte 4
		if (pointerToPreviousByte+3 >= pointerToLastByte)
			break;
		TUint b4 = pointerToPreviousByte[4];
		if (b4 < 0x30 || b4 > 0x39)
			{
			if (numOfBytes == 0)
				return CCnvCharacterSetConverter::EErrorIllFormedInput;
			else 
				break;
			}
		else
			{
				numOfBytes = numOfBytes + 4;
				pointerToPreviousByte = pointerToPreviousByte+4;
			}
		}
		
		return numOfBytes;
	}

// gb18030 4-bytes non-bmp --> unicode (0x90308130~0xE339FE39)
LOCAL_C TInt Step3NumberOfBytesAbleToConvertToUnicode(const TDesC8& aDescriptor)
	{
	const TUint8* pointerToPreviousByte=aDescriptor.Ptr()-1;
	const TUint8* const pointerToLastByte=pointerToPreviousByte+aDescriptor.Length();
	__ASSERT_DEBUG(pointerToPreviousByte<=pointerToLastByte, Panic(EPanicBadPointers25));

	TInt numOfBytes = 0;
	FOREVER
		{
		if (pointerToPreviousByte>=pointerToLastByte)
			{
			break;
			}
		
		// byte 1
		TUint b1 = pointerToPreviousByte[1];
		if (b1 < 0x90 || b1 > 0xE3)
			break;
		
		// byte 2
		if (pointerToPreviousByte+1 >= pointerToLastByte)
			break;
		TUint b2 = pointerToPreviousByte[2];
		if (b2 >= 0x40 && b2 <= 0xFE && b2 != 0x7F)
			break;
		else if (b2 < 0x30 || b2 > 0x39)
			{
			if (numOfBytes == 0)
				return CCnvCharacterSetConverter::EErrorIllFormedInput;
			else 
				break;
			}
			
		// byte 3
		if (pointerToPreviousByte+2 >= pointerToLastByte)
			break;
		TUint b3 = pointerToPreviousByte[3];
		if (b3 < 0x81 || b3 > 0xFE)
			{
			if (numOfBytes == 0)
				return CCnvCharacterSetConverter::EErrorIllFormedInput;
			else 
				break;
			}
		
		// byte 4
		if (pointerToPreviousByte+3 >= pointerToLastByte)
			break;
		TUint b4 = pointerToPreviousByte[4];
		if (b4 < 0x30 || b4 > 0x39)
			{
			if (numOfBytes == 0)
				return CCnvCharacterSetConverter::EErrorIllFormedInput;
			else 
				break;
			}
		else 
			{
			numOfBytes = numOfBytes + 4;
			pointerToPreviousByte = pointerToPreviousByte + 4;
			}
		}
		return numOfBytes;
	}
	
void Step012DummyConvertToIntermediateBufferInPlace(TDes8&)
	{
	}

// Perform the actual conversion (gb18030 4byte non-BMP -> unicode) using formula in this function
LOCAL_C void Step3ConvertToIntermediateBufferInPlace(TDes8& aDescriptor)
	{
	const TInt descriptorLength=aDescriptor.Length();
	__ASSERT_DEBUG(descriptorLength%4 == 0, Panic(EPanicNothingToConvert5));
	TUint8* pointerToTargetByte=CONST_CAST(TUint8*, aDescriptor.Ptr());
	const TUint8* pointerToSourceByte=pointerToTargetByte;
	const TUint8* const pointerToLastByte=pointerToSourceByte+descriptorLength;
	
	FOREVER
		{
		if (pointerToLastByte - pointerToSourceByte < 4)
			break;
		
		// conversion
		TUint8 b1 = pointerToSourceByte[0];
		TUint8 b2 = pointerToSourceByte[1];
		TUint8 b3 = pointerToSourceByte[2];
		TUint8 b4 = pointerToSourceByte[3];
		
		TUint characterCode = 0x10000 + (b1 - 0x90) * 12600 +
										(b2 - 0x30) * 1260 +
										(b3 - 0x81) * 10 +
										(b4 - 0x30);
		
		pointerToTargetByte[0] = ((characterCode >> 24) & 0xFF);
		pointerToTargetByte[1] = ((characterCode >> 16) & 0xFF);
		pointerToTargetByte[2] = ((characterCode >> 8) & 0xFF);
		pointerToTargetByte[3] = (characterCode & 0xFF);
		
		pointerToSourceByte = pointerToSourceByte + 4;
		pointerToTargetByte = pointerToTargetByte + 4;
		}
	
		aDescriptor.SetLength(descriptorLength);
	}


// A dummy "direct" mapping table for non-Bmp chars in step 3
// Use 32-bit Unicode value as intermediate coding
LOCAL_D const SCnvConversionData::SVariableByteData::SRange step3ForeignVariableByteDataRanges[]=
	{
		{
		0x00,		// from 0x10000
		0x00,		// to  0x10FFFF
		3,			// total 4 bytes
		0
		},		
	};
LOCAL_D const SCnvConversionData::SOneDirectionData::SRange step3ForeignToUnicodeDataRanges[]=
	{
		{
		0x10000,	// from 0x10000
		0x10ffff,	// to  0x10FFFF
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		0,
		0,
			{
			0		// map from intermediate to unicode with offset = 0
			}
		},
	};
LOCAL_D const SCnvConversionData::SOneDirectionData::SRange step3UnicodeToForeignDataRanges[]=
	{
		{
		0x10000,	//from 0x10000
		0x10FFFF, //to 0x10FFFF
		SCnvConversionData::SOneDirectionData::SRange::EDirect,
		4,			// output byte count = 4
		0,
			{
			0		// offset = 0
			}
		},
	};
GLDEF_D const SCnvConversionData step3ConversionData=
	{
	SCnvConversionData::EFixedBigEndian,
		{
		ARRAY_LENGTH(step3ForeignVariableByteDataRanges),
		step3ForeignVariableByteDataRanges
		},
		{
		ARRAY_LENGTH(step3ForeignToUnicodeDataRanges),
		step3ForeignToUnicodeDataRanges
		},
		{
		ARRAY_LENGTH(step3UnicodeToForeignDataRanges),
		step3UnicodeToForeignDataRanges
		},
	NULL,
	NULL
	};


// An internal mapping table to reslove the conflict introduced in symbian GB2312-80 plug-in.
// It will be merged into the gb18030-2byte Conversion Data.
// It includes mapping: (0xA1A4 -> 0x00B7, 0xA1AA -> 0x2014, 0xA844 <- 0x2015, 0x8139A739 <- 0x30FB)
LOCAL_D const SCnvConversionData::SVariableByteData::SRange gb18030_diff_gb2312ForeignVariableByteDataRanges[]=
	{
		{
		0xA1, //from 0xA1A4
		0xA1, //to 0xA1AA
		1,			
		0
		},		
	};
LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_foreignToUnicode_1[]=
	{
			{
			0xA1A4,
			0x00B7
			},
			{
			0xA1AA,
			0x2014
			}
	};
LOCAL_D const SCnvConversionData::SOneDirectionData::SRange gb18030_diff_gb2312ForeignToUnicodeDataRanges[]=
	{
		{
		0xA1A4,
		0xA1AA,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		0,
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_foreignToUnicode_1)
			}
		},
	};
LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable1616::SEntry keyedTable1616_unicodeToForeign_1[]=
	{
			{
			0x2015,
			0xA844
			}
	};
LOCAL_D const SCnvConversionData::SOneDirectionData::SRange::UData::SKeyedTable3232::SEntry keyedTable3232_unicodeToForeign_1[]=
	{
			{
			0x30FB,
			0x8139A739
			}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange gb18030_diff_gb2312UnicodeToForeignDataRanges[]=
	{
		{
		0x2015,
		0x2015,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable1616,
		2,			// output byte count = 2
		0,
			{
			UData_SKeyedTable1616(keyedTable1616_unicodeToForeign_1)
			}
		},
		{
		0x30FB,
		0x30FB,
		SCnvConversionData::SOneDirectionData::SRange::EKeyedTable3232,
		4,			// output byte count = 4
		0,
			{
			UData_SKeyedTable3232(keyedTable3232_unicodeToForeign_1)
			}
		},
	};
GLDEF_D const SCnvConversionData gb18030_diff_gb2312ConversionData=
	{
	SCnvConversionData::EFixedBigEndian,
		{
		ARRAY_LENGTH(gb18030_diff_gb2312ForeignVariableByteDataRanges),
		gb18030_diff_gb2312ForeignVariableByteDataRanges
		},
		{
		ARRAY_LENGTH(gb18030_diff_gb2312ForeignToUnicodeDataRanges),
		gb18030_diff_gb2312ForeignToUnicodeDataRanges
		},
		{
		ARRAY_LENGTH(gb18030_diff_gb2312UnicodeToForeignDataRanges),
		gb18030_diff_gb2312UnicodeToForeignDataRanges
		},
	NULL,
	NULL
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

LOCAL_C void SetUpCompleteGb18030_2byteConversionData(SCnvConversionData& aCompleteGb18030_2byteConversionData, TUint8* aWorkingMemory)
	{
	const SCnvConversionData& gb2312ConversionData=CnvGb2312::ConversionData();
	const SCnvConversionData& gb18030_diff_gbkConversionData=CnvGb18030_diff_gbk::ConversionData();
	const SCnvConversionData& gbkConversionData=CnvGbk::ConversionData();
	// create a SCnvConversionData that is the combination of gb18030_diff_gb2312ConversionData, gb2312ConversionData, gb18030_diff_gbkConversionData and gbkConversionData;
	aCompleteGb18030_2byteConversionData.iEndiannessOfForeignCharacters=SCnvConversionData::EFixedBigEndian;
	aCompleteGb18030_2byteConversionData.iForeignVariableByteData.iNumberOfRanges=ARRAY_LENGTH(foreignVariableByteDataRanges);
	aCompleteGb18030_2byteConversionData.iForeignVariableByteData.iRangeArray=foreignVariableByteDataRanges;
	TInt numberOfBytesOfWorkingMemoryUsed=0;
	
	// set up the foreign-to-Unicode data
	const TInt numberOfForeignToUnicodeDataRanges=gb18030_diff_gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges + gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges + gb18030_diff_gbkConversionData.iForeignToUnicodeData.iNumberOfRanges + gbkConversionData.iForeignToUnicodeData.iNumberOfRanges;
	aCompleteGb18030_2byteConversionData.iForeignToUnicodeData.iNumberOfRanges=numberOfForeignToUnicodeDataRanges;
	SCnvConversionData::SOneDirectionData::SRange* foreignToUnicodeDataRangeArray=REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange*, aWorkingMemory+numberOfBytesOfWorkingMemoryUsed);
	numberOfBytesOfWorkingMemoryUsed+=(numberOfForeignToUnicodeDataRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	__ASSERT_ALWAYS(numberOfBytesOfWorkingMemoryUsed<=KNumberOfBytesOfWorkingMemory, Panic(EPanicTooManyBytesOfWorkingMemoryUsed1));
	aCompleteGb18030_2byteConversionData.iForeignToUnicodeData.iRangeArray=foreignToUnicodeDataRangeArray;
	Mem::Copy(foreignToUnicodeDataRangeArray, gb18030_diff_gb2312ConversionData.iForeignToUnicodeData.iRangeArray, gb18030_diff_gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	Mem::Copy(foreignToUnicodeDataRangeArray + gb18030_diff_gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges, gb2312ConversionData.iForeignToUnicodeData.iRangeArray, gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	Mem::Copy(foreignToUnicodeDataRangeArray + gb18030_diff_gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges + gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges, gb18030_diff_gbkConversionData.iForeignToUnicodeData.iRangeArray, gb18030_diff_gbkConversionData.iForeignToUnicodeData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	Mem::Copy(foreignToUnicodeDataRangeArray + gb18030_diff_gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges + gb2312ConversionData.iForeignToUnicodeData.iNumberOfRanges + gb18030_diff_gbkConversionData.iForeignToUnicodeData.iNumberOfRanges, gbkConversionData.iForeignToUnicodeData.iRangeArray, gbkConversionData.iForeignToUnicodeData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));

	// set up the Unicode-to-foreign data
	const TInt numberOfUnicodeToForeignDataRanges=gb18030_diff_gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges + gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges + gb18030_diff_gbkConversionData.iUnicodeToForeignData.iNumberOfRanges + gbkConversionData.iUnicodeToForeignData.iNumberOfRanges;
	aCompleteGb18030_2byteConversionData.iUnicodeToForeignData.iNumberOfRanges=numberOfUnicodeToForeignDataRanges;
	SCnvConversionData::SOneDirectionData::SRange* unicodeToForeignDataRangeArray=REINTERPRET_CAST(SCnvConversionData::SOneDirectionData::SRange*, aWorkingMemory+numberOfBytesOfWorkingMemoryUsed);
	numberOfBytesOfWorkingMemoryUsed+=(numberOfUnicodeToForeignDataRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	__ASSERT_ALWAYS(numberOfBytesOfWorkingMemoryUsed<=KNumberOfBytesOfWorkingMemory, Panic(EPanicTooManyBytesOfWorkingMemoryUsed2));
	aCompleteGb18030_2byteConversionData.iUnicodeToForeignData.iRangeArray=unicodeToForeignDataRangeArray;
	Mem::Copy(unicodeToForeignDataRangeArray, gb18030_diff_gb2312ConversionData.iUnicodeToForeignData.iRangeArray, gb18030_diff_gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	Mem::Copy(unicodeToForeignDataRangeArray + gb18030_diff_gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges, gb2312ConversionData.iUnicodeToForeignData.iRangeArray, gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	Mem::Copy(unicodeToForeignDataRangeArray + gb18030_diff_gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges + gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges, gb18030_diff_gbkConversionData.iUnicodeToForeignData.iRangeArray, gb18030_diff_gbkConversionData.iUnicodeToForeignData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	Mem::Copy(unicodeToForeignDataRangeArray + gb18030_diff_gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges + gb2312ConversionData.iUnicodeToForeignData.iNumberOfRanges + gb18030_diff_gbkConversionData.iUnicodeToForeignData.iNumberOfRanges, gbkConversionData.iUnicodeToForeignData.iRangeArray, gbkConversionData.iUnicodeToForeignData.iNumberOfRanges*sizeof(SCnvConversionData::SOneDirectionData::SRange));
	}


TInt CGB18030ConverterImpl::ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	TFixedArray<CnvUtilities::SCharacterSet, 3> characterSets;
	
	// step 1) gb18030-2byte
	characterSets[0].iConversionData						= completeGb18030_2byteConversionData;
	characterSets[0].iConvertFromIntermediateBufferInPlace	= Step12DummyConvertFromIntermediateBufferInPlace;
	characterSets[0].iEscapeSequence						= &KNullDesC8;
	
	// step 2) gb18030-4byte BMP
	characterSets[1].iConversionData						= &CnvGb18030_4byte::ConversionData();
	characterSets[1].iConvertFromIntermediateBufferInPlace	= Step12DummyConvertFromIntermediateBufferInPlace;
	characterSets[1].iEscapeSequence						= &KNullDesC8;
	
	// step 3) gb18030-4byte non-BMP
	characterSets[2].iConversionData						= &step3ConversionData;
	characterSets[2].iConvertFromIntermediateBufferInPlace	= Step3ConvertFromIntermediateBufferInPlace;
	characterSets[2].iEscapeSequence						= &KNullDesC8;
	
	return CnvUtilities::ConvertFromUnicode(aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters, characterSets.Array());
	}


TInt CGB18030ConverterImpl::ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& /*aState*/, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	TFixedArray<CnvUtilities::SMethod, 4> methods;
	// step 0) gb2312-1byte
	methods[0].iNumberOfBytesAbleToConvert			= Step0NumberOfBytesAbleToConvertToUnicode;
	methods[0].iConvertToIntermediateBufferInPlace	= Step012DummyConvertToIntermediateBufferInPlace;
	methods[0].iConversionData						= &CnvGb2312::ConversionData(); //only use one byte part
	methods[0].iNumberOfBytesPerCharacter			= 1;
	methods[0].iNumberOfCoreBytesPerCharacter		= 1;
	
	// step 1) gb18030-2byte
	methods[1].iNumberOfBytesAbleToConvert			= Step1NumberOfBytesAbleToConvertToUnicode;
	methods[1].iConvertToIntermediateBufferInPlace	= Step012DummyConvertToIntermediateBufferInPlace;
	methods[1].iConversionData						= completeGb18030_2byteConversionData;
	methods[1].iNumberOfBytesPerCharacter			= 2;
	methods[1].iNumberOfCoreBytesPerCharacter		= 2;
	
	// step 2) gb18030 4-byte BMP
	methods[2].iNumberOfBytesAbleToConvert			= Step2NumberOfBytesAbleToConvertToUnicode;
	methods[2].iConvertToIntermediateBufferInPlace	= Step012DummyConvertToIntermediateBufferInPlace;
	methods[2].iConversionData						= &CnvGb18030_4byte::ConversionData();
	methods[2].iNumberOfBytesPerCharacter			= 4;
	methods[2].iNumberOfCoreBytesPerCharacter		= 4;

	// step 3) gb18030 4-byte non-BMP
	methods[3].iNumberOfBytesAbleToConvert			= Step3NumberOfBytesAbleToConvertToUnicode;
	methods[3].iConvertToIntermediateBufferInPlace	= Step3ConvertToIntermediateBufferInPlace;
	methods[3].iConversionData						= &step3ConversionData;
	methods[3].iNumberOfBytesPerCharacter			= 4;
	methods[3].iNumberOfCoreBytesPerCharacter		= 4;
	
	return CnvUtilities::ConvertToUnicodeFromHeterogeneousForeign(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter, methods.Array());
	}

TBool CGB18030ConverterImpl::IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample)
	{
	aSetToTrue = ETrue;
	return CnvGb2312::IsCharGBBased(aConfidenceLevel, aSample);
	}

CGB18030ConverterImpl* CGB18030ConverterImpl::NewL()
	{
	CGB18030ConverterImpl* self = new(ELeave) CGB18030ConverterImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CGB18030ConverterImpl::~CGB18030ConverterImpl()
	{
	if (workingMemory)
		delete[] workingMemory;
	if (completeGb18030_2byteConversionData)
		delete completeGb18030_2byteConversionData;
	}

CGB18030ConverterImpl::CGB18030ConverterImpl()
	{
	}

TInt CGB18030ConverterImpl::ConstructL()
	{
	completeGb18030_2byteConversionData = new (ELeave)SCnvConversionData;
	CleanupStack::PushL(completeGb18030_2byteConversionData);
	workingMemory = new (ELeave) TUint8[KNumberOfBytesOfWorkingMemory]; //1040 bytes
	CleanupStack::Pop(); // completeGb18030_2byteConversionData
	SetUpCompleteGb18030_2byteConversionData(*completeGb18030_2byteConversionData, workingMemory);
	return 1;
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10287038,CGB18030ConverterImpl::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	
