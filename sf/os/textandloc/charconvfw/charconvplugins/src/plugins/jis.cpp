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
#include "jisbase.h"
#include "jisx0201.h"
#include "jisx0208.h"
#include "jisx0212.h"
#include <ecom/implementationproxy.h>
#include <charactersetconverter.h>
#include "featmgr/featmgr.h" 

_LIT8(KLit8EscapeSequenceForJisRoman, "\x1b\x28\x4a");
_LIT8(KLit8EscapeSequenceForAscii, "\x1b\x28\x42");
_LIT8(KLit8EscapeSequenceForHalfWidthKatakana, "\x1b\x28\x49");
_LIT8(KLit8EscapeSequenceForJisX0208_1983, "\x1b\x24\x42");
_LIT8(KLit8EscapeSequenceForJisX0212_1990, "\x1b\x24\x28\x44");
_LIT8(KLit8JisReplacementForUnconvertibleUnicodeCharacters, "\x1b\x24\x42\x21\x29"); // fullwidth question mark

class CJISConverterImpl : public CCharacterSetConverterPluginInterface
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

	static CJISConverterImpl* NewL();
	virtual ~CJISConverterImpl();

private:
	CJISConverterImpl();
	void ConstructL();

	};



const TDesC8& CJISConverterImpl::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return KLit8JisReplacementForUnconvertibleUnicodeCharacters;
	}

LOCAL_C void ConvertFromJisRomanToJisInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	CnvUtilities::ConvertFromIntermediateBufferInPlace(aStartPositionInDescriptor, aDescriptor, aNumberOfCharactersThatDroppedOut, KLit8EscapeSequenceForJisRoman, 1);
	}

LOCAL_C void ConvertFromAsciiToJisInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	CnvUtilities::ConvertFromIntermediateBufferInPlace(aStartPositionInDescriptor, aDescriptor, aNumberOfCharactersThatDroppedOut, KLit8EscapeSequenceForAscii, 1);
	}

LOCAL_C void ConvertFromHalfWidthKatakanaToJisInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	CnvUtilities::ConvertFromIntermediateBufferInPlace(aStartPositionInDescriptor, aDescriptor, aNumberOfCharactersThatDroppedOut, KLit8EscapeSequenceForHalfWidthKatakana, 1);
	}

LOCAL_C void ConvertFromJisX0208ToJisInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	CnvUtilities::ConvertFromIntermediateBufferInPlace(aStartPositionInDescriptor, aDescriptor, aNumberOfCharactersThatDroppedOut, KLit8EscapeSequenceForJisX0208_1983, 2);
	}

LOCAL_C void ConvertFromJisX0212ToJisInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	CnvUtilities::ConvertFromIntermediateBufferInPlace(aStartPositionInDescriptor, aDescriptor, aNumberOfCharactersThatDroppedOut, KLit8EscapeSequenceForJisX0212_1990, 2);
	}

TInt CJISConverterImpl::ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	TFixedArray<CnvUtilities::SCharacterSet, 5> characterSets;
	characterSets[0].iConversionData=&CnvJisRoman::ConversionData();
	characterSets[0].iConvertFromIntermediateBufferInPlace=ConvertFromJisRomanToJisInPlace;
	characterSets[0].iEscapeSequence=&KLit8EscapeSequenceForJisRoman;
	characterSets[1].iConversionData=&CCnvCharacterSetConverter::AsciiConversionData();
	characterSets[1].iConvertFromIntermediateBufferInPlace=ConvertFromAsciiToJisInPlace;
	characterSets[1].iEscapeSequence=&KLit8EscapeSequenceForAscii;
	characterSets[2].iConversionData=&CnvJisBase::HalfWidthKatakana7ConversionData();
	characterSets[2].iConvertFromIntermediateBufferInPlace=ConvertFromHalfWidthKatakanaToJisInPlace;
	characterSets[2].iEscapeSequence=&KLit8EscapeSequenceForHalfWidthKatakana;
	characterSets[3].iConversionData=&CnvJisX0208::ConversionData();
	characterSets[3].iConvertFromIntermediateBufferInPlace=ConvertFromJisX0208ToJisInPlace;
	characterSets[3].iEscapeSequence=&KLit8EscapeSequenceForJisX0208_1983;
	characterSets[4].iConversionData=&CnvJisX0212::ConversionData();
	characterSets[4].iConvertFromIntermediateBufferInPlace=ConvertFromJisX0212ToJisInPlace;
	characterSets[4].iEscapeSequence=&KLit8EscapeSequenceForJisX0212_1990;
	return CnvUtilities::ConvertFromUnicode(aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters, characterSets.Array());
	}

TInt CJISConverterImpl::ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& aState, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	return CnvJisBase::ConvertToUnicode(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aState, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
	}

TBool CJISConverterImpl::IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample)
	{
	aSetToTrue=ETrue;
	CnvJisBase::IsCharacterJISBased(aConfidenceLevel, aSample);
	return ETrue;
	}

CJISConverterImpl* CJISConverterImpl::NewL()
	{
	CJISConverterImpl* self = new(ELeave) CJISConverterImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
	return self;
	}

CJISConverterImpl::~CJISConverterImpl()
	{
	FeatureManager::UnInitializeLib();
	}

CJISConverterImpl::CJISConverterImpl()
	{
	}

void CJISConverterImpl::ConstructL()
    {
    FeatureManager::InitializeLibL();
    }

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x10006066,	CJISConverterImpl::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	
