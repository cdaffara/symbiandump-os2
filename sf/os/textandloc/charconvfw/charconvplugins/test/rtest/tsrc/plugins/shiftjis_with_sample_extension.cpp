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
#include "shiftjis.h"
#include <ecom/implementationproxy.h>
#include <charactersetconverter.h>

#define ARRAY_LENGTH(aArray) (sizeof(aArray)/sizeof((aArray)[0]))

#if defined(_DEBUG)

_LIT(KLitPanicText, "SHIFTJIS_WITH_SAMPLE_EXTENSION");

enum TPanic
	{
	EPanicBadPointers1=1,
	EPanicBadPointers2,
	EPanicBadPointers3,
	EPanicBadPointers4
	};

LOCAL_C void Panic(TPanic aPanic)
	{
	User::Panic(KLitPanicText, aPanic);
	}

#endif


LOCAL_D const SCnvConversionData::SVariableByteData::SRange sampleExtensionVariableByteDataRanges[]=
	{
		{
		0xef,
		0xf0,
		1,
		0
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange sampleExtensionToUnicodeDataRanges[]=
	{
		{
		0xef00,
		0xf0ff,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		0,
		0,
			{
			STATIC_CAST(TUint, -987),
			0
			}
		}
	};

LOCAL_D const SCnvConversionData::SOneDirectionData::SRange unicodeToSampleExtensionDataRanges[]=
	{
		{
		0xeb25,
		0xed24,
		SCnvConversionData::SOneDirectionData::SRange::EOffset,
		2,
		0,
			{
			STATIC_CAST(TUint, 987),
			0
			}
		}
	};

LOCAL_D const SCnvConversionData sampleExtensionConversionData=
	{
	SCnvConversionData::EFixedBigEndian,
		{
		ARRAY_LENGTH(sampleExtensionVariableByteDataRanges),
		sampleExtensionVariableByteDataRanges
		},
		{
		ARRAY_LENGTH(sampleExtensionToUnicodeDataRanges),
		sampleExtensionToUnicodeDataRanges
		},
		{
		ARRAY_LENGTH(unicodeToSampleExtensionDataRanges),
		unicodeToSampleExtensionDataRanges
		},
	NULL,
	NULL
	};

class CExtensionTestConverterImpl : public CCharacterSetConverterPluginInterface
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

	static CExtensionTestConverterImpl* NewL();
	virtual ~CExtensionTestConverterImpl();

private:
	CExtensionTestConverterImpl();

	};


const TDesC8& CExtensionTestConverterImpl::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return CnvShiftJis::ReplacementForUnconvertibleUnicodeCharacters();
	}

LOCAL_C void DummyConvertFromIntermediateBufferInPlace(TInt, TDes8&, TInt& aNumberOfCharactersThatDroppedOut)
	{
	aNumberOfCharactersThatDroppedOut=0;
	}

TInt CExtensionTestConverterImpl::ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, TDes8& aForeign, const TDesC16& aUnicode, CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	TFixedArray<CnvUtilities::SCharacterSet, 1> arrayOfAdditionalCharacterSets;
	arrayOfAdditionalCharacterSets[0].iConversionData=&sampleExtensionConversionData;
	arrayOfAdditionalCharacterSets[0].iConvertFromIntermediateBufferInPlace=DummyConvertFromIntermediateBufferInPlace;
	arrayOfAdditionalCharacterSets[0].iEscapeSequence=&KNullDesC8;
	return CnvShiftJis::ConvertFromUnicode(aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters, arrayOfAdditionalCharacterSets.Array());
	}

LOCAL_C TInt NumberOfBytesAbleToConvertToSampleExtension(const TDesC8& aDescriptor)
	{
	const TUint8* pointerToPreviousByte=aDescriptor.Ptr()-1;
	const TUint8* const pointerToLastByte=pointerToPreviousByte+aDescriptor.Length();
	if (pointerToPreviousByte==pointerToLastByte)
		{
		return 0;
		}
	FOREVER
		{
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers1));
		TUint currentByte=*(pointerToPreviousByte+1);
		if ((currentByte<0xef) || (currentByte>0xf0))
			{
			break;
			}
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers2));
		if (pointerToLastByte-pointerToPreviousByte<2)
			{
			break;
			}
		++pointerToPreviousByte;
		__ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers3));
		++pointerToPreviousByte;
		__ASSERT_DEBUG(pointerToPreviousByte<=pointerToLastByte, Panic(EPanicBadPointers4));
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

TInt CExtensionTestConverterImpl::ConvertToUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aForeign, TInt&, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	TFixedArray<CnvUtilities::SMethod, 1> arrayOfAdditionalMethods;
	arrayOfAdditionalMethods[0].iNumberOfBytesAbleToConvert=NumberOfBytesAbleToConvertToSampleExtension;
	arrayOfAdditionalMethods[0].iConvertToIntermediateBufferInPlace=DummyConvertToIntermediateBufferInPlace;
	arrayOfAdditionalMethods[0].iConversionData=&sampleExtensionConversionData;
	arrayOfAdditionalMethods[0].iNumberOfBytesPerCharacter=2;
	arrayOfAdditionalMethods[0].iNumberOfCoreBytesPerCharacter=2;
	return CnvShiftJis::ConvertToUnicode(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter, arrayOfAdditionalMethods.Array());
	}

TBool CExtensionTestConverterImpl::IsInThisCharacterSetL(TBool& aSetToTrue, TInt& aConfidenceLevel, const TDesC8&)
	{
	aSetToTrue=ETrue;
	aConfidenceLevel=0;
	return EFalse;
	}

CExtensionTestConverterImpl* CExtensionTestConverterImpl::NewL()
	{
	CExtensionTestConverterImpl* self = new(ELeave) CExtensionTestConverterImpl();
	return self;
	}

CExtensionTestConverterImpl::~CExtensionTestConverterImpl()
	{
	}

CExtensionTestConverterImpl::CExtensionTestConverterImpl()
	{
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10009403,	CExtensionTestConverterImpl::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

