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
* Description:       
*
*/

#include "PictographObserver.h"
#include "featmgr/featmgr.h"

#include <e32std.h>
#include <charconv.h>
#include <convutils.h>
#include "jisbase.h"
#include "jisx0201.h"
#include "jisx0208.h"
#include "jisx0212.h"
#include <ecom/implementationproxy.h>
#include "charactersetconverter.h"

_LIT8(KLit8EscapeSequenceForJisRoman, "\x1b\x28\x4a");
_LIT8(KLit8EscapeSequenceForAscii, "\x1b\x28\x42");
//_LIT8(KLit8EscapeSequenceForJisC6226_1978, "\x1b\x24\x40");
_LIT8(KLit8EscapeSequenceForJisX0208_1983, "\x1b\x24\x42");
_LIT8(KLit8EscapeSequenceForJisX0212_1990, "\x1b\x24\x28\x44");
//_LIT8(KLit8EscapeSequenceForHalfWidthKatakana, "\x1b\x28\x49");
_LIT8(KLit8Iso2022Jp1ReplacementForUnconvertibleUnicodeCharacters, "\x1b\x24\x42\x21\x29"); // fullwidth question mark


class CISO2022JP1ConverterImpl : public CCharacterSetConverterPluginInterface
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

	static CISO2022JP1ConverterImpl* NewL();
	virtual ~CISO2022JP1ConverterImpl();

private:
	CISO2022JP1ConverterImpl();
	void ConstructL();

	};


const TDesC8& CISO2022JP1ConverterImpl::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return KLit8Iso2022Jp1ReplacementForUnconvertibleUnicodeCharacters;
	}

LOCAL_C void ConvertFromJisRomanToJisInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	CnvUtilities::ConvertFromIntermediateBufferInPlace(aStartPositionInDescriptor, aDescriptor, aNumberOfCharactersThatDroppedOut, KLit8EscapeSequenceForJisRoman, 1);
	}

LOCAL_C void ConvertFromAsciiToJisInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	CnvUtilities::ConvertFromIntermediateBufferInPlace(aStartPositionInDescriptor, aDescriptor, aNumberOfCharactersThatDroppedOut, KLit8EscapeSequenceForAscii, 1);
	}

LOCAL_C void ConvertFromJisX0208ToJisInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	CnvUtilities::ConvertFromIntermediateBufferInPlace(aStartPositionInDescriptor, aDescriptor, aNumberOfCharactersThatDroppedOut, KLit8EscapeSequenceForJisX0208_1983, 2);
	}

LOCAL_C void ConvertFromJisX0212ToJisInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
	{
	CnvUtilities::ConvertFromIntermediateBufferInPlace(aStartPositionInDescriptor, aDescriptor, aNumberOfCharactersThatDroppedOut, KLit8EscapeSequenceForJisX0212_1990, 2);
	}

TInt CISO2022JP1ConverterImpl::ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
    TInt ret = KErrNone;
    RArray<CnvUtilities::SCharacterSet> characterSets;
    if ( FeatureManager::FeatureSupported(KFeatureIdJapanesePicto) )
        {        
        CnvUtilities::SCharacterSet characterSet;
        characterSet.iConversionData=&CnvJisRoman::ConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromJisRomanToJisInPlace;
        characterSet.iEscapeSequence=&KLit8EscapeSequenceForJisRoman;
        ret |= characterSets.Append(characterSet);
        characterSet.iConversionData=&CCnvCharacterSetConverter::AsciiConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromAsciiToJisInPlace;
        characterSet.iEscapeSequence=&KLit8EscapeSequenceForAscii;
        ret |= characterSets.Append(characterSet);
        characterSet.iConversionData=&CnvJisX0208::ConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromJisX0208ToJisInPlace;
        characterSet.iEscapeSequence=&KLit8EscapeSequenceForJisX0208_1983;
        ret |= characterSets.Append(characterSet);
        characterSet.iConversionData=&CnvJisX0212::ConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromJisX0212ToJisInPlace;
        characterSet.iEscapeSequence=&KLit8EscapeSequenceForJisX0212_1990;
        ret |= characterSets.Append(characterSet);

        SetCharacterSetsForPictograph(characterSets);
        }
    else
        {        
        CnvUtilities::SCharacterSet characterSet;
	    characterSet.iConversionData=&CnvJisRoman::ConversionData();
	    characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromJisRomanToJisInPlace;
	    characterSet.iEscapeSequence=&KLit8EscapeSequenceForJisRoman;
	    ret |= characterSets.Append(characterSet);
    	characterSet.iConversionData=&CCnvCharacterSetConverter::AsciiConversionData();
    	characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromAsciiToJisInPlace;
    	characterSet.iEscapeSequence=&KLit8EscapeSequenceForAscii;
    	ret |= characterSets.Append(characterSet);
    	characterSet.iConversionData=&CnvJisX0208::ConversionData();
    	characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromJisX0208ToJisInPlace;
    	characterSet.iEscapeSequence=&KLit8EscapeSequenceForJisX0208_1983;
    	ret |= characterSets.Append(characterSet);
    	characterSet.iConversionData=&CnvJisX0212::ConversionData();
    	characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromJisX0212ToJisInPlace;
    	characterSet.iEscapeSequence=&KLit8EscapeSequenceForJisX0212_1990;
    	ret |= characterSets.Append(characterSet);
        }
    __ASSERT_DEBUG(!ret, User::Panic(_L("RArray append failure"), ret));
	TUint notUsed;
	TUint inputConversionFlags=CCnvCharacterSetConverter::EInputConversionFlagMustEndInDefaultCharacterSet;
    TInt unconvert = CnvUtilities::ConvertFromUnicode(aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters, 
        characterSets.Array(),notUsed,inputConversionFlags);

    characterSets.Close();
    return unconvert;
	}

TInt CISO2022JP1ConverterImpl::ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& aState, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	return CnvJisBase::ConvertToUnicode(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aState, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
	}

TBool CISO2022JP1ConverterImpl::IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample)
	{
	aSetToTrue=ETrue;
	CnvJisBase::IsCharacterJISBased(aConfidenceLevel, aSample);
	return ETrue;
	}

CISO2022JP1ConverterImpl* CISO2022JP1ConverterImpl::NewL()
	{
	CISO2022JP1ConverterImpl* self = new(ELeave) CISO2022JP1ConverterImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CISO2022JP1ConverterImpl::~CISO2022JP1ConverterImpl()
	{
	FeatureManager::UnInitializeLib();
	}

CISO2022JP1ConverterImpl::CISO2022JP1ConverterImpl()
	{
	}

void CISO2022JP1ConverterImpl::ConstructL()
	{
	FeatureManager::InitializeLibL();
	}

const TImplementationProxy ImplementationTable[] = 
	{
#ifdef S60_TEST
    IMPLEMENTATION_PROXY_ENTRY(0x01000009,  CISO2022JP1ConverterImpl::NewL)
#else
	IMPLEMENTATION_PROXY_ENTRY(0x100066A3,	CISO2022JP1ConverterImpl::NewL)
#endif	
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	
