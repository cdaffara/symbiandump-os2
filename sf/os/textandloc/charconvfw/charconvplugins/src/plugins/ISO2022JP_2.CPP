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
#include <ecom/implementationproxy.h>
#include "charactersetconverter.h"


_LIT8(KLit8EscapeSequenceForJisRoman, "\x1b\x28\x4a");
_LIT8(KLit8EscapeSequenceForAscii, "\x1b\x28\x42");     
_LIT8(KLit8EscapeSequenceForJisX0208_1983, "\x1b\x24\x42");
//_LIT8(KLit8EscapeSequenceForJisC6226_1978, "\x1b\x24\x40");
//_LIT8(KLit8EscapeSequenceForJisX0212_1990, "\x1b\x24\x28\x44"); 
//_LIT8(KLit8EscapeSequenceForHalfWidthKatakana, "\x1b\x28\x49");
_LIT8(KLit8Iso2022JpReplacementForUnconvertibleUnicodeCharacters, "\x1b\x24\x42\x21\x29"); // fullwidth question mark


class CISO2022JPConverterImpl : public CCharacterSetConverterPluginInterface
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

    static CISO2022JPConverterImpl* NewL();
    virtual ~CISO2022JPConverterImpl();

private:
    CISO2022JPConverterImpl();
    void ConstructL();

    };



const TDesC8& CISO2022JPConverterImpl::ReplacementForUnconvertibleUnicodeCharacters()
    {
    return KLit8Iso2022JpReplacementForUnconvertibleUnicodeCharacters;
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

struct TConvTblFromHalfKanaToFullKana
    {
    TUint8  iHalfKana;
    TUint8  iHalfKanaMark;
    TUint16 iFullKana;
    };

LOCAL_D const TConvTblFromHalfKanaToFullKana convTblFromHalfKanaToFullKana[]=
    {
        { 0xA1, 0x00, 0x2123 },     // IDEOGRAPHIC FULL STOP
        { 0xA2, 0x00, 0x2156 },     // LEFT CORNER BRACKET
        { 0xA3, 0x00, 0x2157 },     // RIGHT CORNER BRACKET
        { 0xA4, 0x00, 0x2122 },     // IDEOGRAPHIC COMMA
        { 0xA5, 0x00, 0x2126 },     // KATAKANA MIDDLE DOT
        { 0xA6, 0x00, 0x2572 },     // KATAKANA LETTER WO
        { 0xA7, 0x00, 0x2521 },     // KATAKANA LETTER SMALL A
        { 0xA8, 0x00, 0x2523 },     // KATAKANA LETTER SMALL I
        { 0xA9, 0x00, 0x2525 },     // KATAKANA LETTER SMALL U
        { 0xAA, 0x00, 0x2527 },     // KATAKANA LETTER SMALL E
        { 0xAB, 0x00, 0x2529 },     // KATAKANA LETTER SMALL O
        { 0xAC, 0x00, 0x2563 },     // KATAKANA LETTER SMALL YA
        { 0xAD, 0x00, 0x2565 },     // KATAKANA LETTER SMALL YU
        { 0xAE, 0x00, 0x2567 },     // KATAKANA LETTER SMALL YO
        { 0xAF, 0x00, 0x2543 },     // KATAKANA LETTER SMALL TU
        { 0xB0, 0x00, 0x213C },     // KATAKANA-HIRAGANA PROLONGED SOUND MARK
        { 0xB1, 0x00, 0x2522 },     // KATAKANA LETTER A
        { 0xB2, 0x00, 0x2524 },     // KATAKANA LETTER I
        { 0xB3, 0x00, 0x2526 },     // KATAKANA LETTER U
        { 0xB3, 0xDE, 0x2574 },     // KATAKANA LETTER VU
        { 0xB4, 0x00, 0x2528 },     // KATAKANA LETTER E
        { 0xB5, 0x00, 0x252A },     // KATAKANA LETTER O
        { 0xB6, 0x00, 0x252B },     // KATAKANA LETTER KA
        { 0xB6, 0xDE, 0x252C },     // KATAKANA LETTER GA
        { 0xB7, 0x00, 0x252D },     // KATAKANA LETTER KI
        { 0xB7, 0xDE, 0x252E },     // KATAKANA LETTER GI
        { 0xB8, 0x00, 0x252F },     // KATAKANA LETTER KU
        { 0xB8, 0xDE, 0x2530 },     // KATAKANA LETTER GU
        { 0xB9, 0x00, 0x2531 },     // KATAKANA LETTER KE
        { 0xB9, 0xDE, 0x2532 },     // KATAKANA LETTER GE
        { 0xBA, 0x00, 0x2533 },     // KATAKANA LETTER KO
        { 0xBA, 0xDE, 0x2534 },     // KATAKANA LETTER GO
        { 0xBB, 0x00, 0x2535 },     // KATAKANA LETTER SA
        { 0xBB, 0xDE, 0x2536 },     // KATAKANA LETTER ZA
        { 0xBC, 0x00, 0x2537 },     // KATAKANA LETTER SI
        { 0xBC, 0xDE, 0x2538 },     // KATAKANA LETTER ZI
        { 0xBD, 0x00, 0x2539 },     // KATAKANA LETTER SU
        { 0xBD, 0xDE, 0x253A },     // KATAKANA LETTER ZU
        { 0xBE, 0x00, 0x253B },     // KATAKANA LETTER SE
        { 0xBE, 0xDE, 0x253C },     // KATAKANA LETTER ZE
        { 0xBF, 0x00, 0x253D },     // KATAKANA LETTER SO
        { 0xBF, 0xDE, 0x253E },     // KATAKANA LETTER ZO
        { 0xC0, 0x00, 0x253F },     // KATAKANA LETTER TA
        { 0xC0, 0xDE, 0x2540 },     // KATAKANA LETTER DA
        { 0xC1, 0x00, 0x2541 },     // KATAKANA LETTER TI
        { 0xC1, 0xDE, 0x2542 },     // KATAKANA LETTER DI
        { 0xC2, 0x00, 0x2544 },     // KATAKANA LETTER TU
        { 0xC2, 0xDE, 0x2545 },     // KATAKANA LETTER DU
        { 0xC3, 0x00, 0x2546 },     // KATAKANA LETTER TE
        { 0xC3, 0xDE, 0x2547 },     // KATAKANA LETTER DE
        { 0xC4, 0x00, 0x2548 },     // KATAKANA LETTER TO
        { 0xC4, 0xDE, 0x2549 },     // KATAKANA LETTER DO
        { 0xC5, 0x00, 0x254A },     // KATAKANA LETTER NA
        { 0xC6, 0x00, 0x254B },     // KATAKANA LETTER NI
        { 0xC7, 0x00, 0x254C },     // KATAKANA LETTER NU
        { 0xC8, 0x00, 0x254D },     // KATAKANA LETTER NE
        { 0xC9, 0x00, 0x254E },     // KATAKANA LETTER NO
        { 0xCA, 0x00, 0x254F },     // KATAKANA LETTER HA
        { 0xCA, 0xDE, 0x2550 },     // KATAKANA LETTER BA
        { 0xCA, 0xDF, 0x2551 },     // KATAKANA LETTER PA
        { 0xCB, 0x00, 0x2552 },     // KATAKANA LETTER HI
        { 0xCB, 0xDE, 0x2553 },     // KATAKANA LETTER BI
        { 0xCB, 0xDF, 0x2554 },     // KATAKANA LETTER PI
        { 0xCC, 0x00, 0x2555 },     // KATAKANA LETTER HU
        { 0xCC, 0xDE, 0x2556 },     // KATAKANA LETTER BU
        { 0xCC, 0xDF, 0x2557 },     // KATAKANA LETTER PU
        { 0xCD, 0x00, 0x2558 },     // KATAKANA LETTER HE
        { 0xCD, 0xDE, 0x2559 },     // KATAKANA LETTER BE
        { 0xCD, 0xDF, 0x255A },     // KATAKANA LETTER PE
        { 0xCE, 0x00, 0x255B },     // KATAKANA LETTER HO
        { 0xCE, 0xDE, 0x255C },     // KATAKANA LETTER BO
        { 0xCE, 0xDF, 0x255D },     // KATAKANA LETTER PO
        { 0xCF, 0x00, 0x255E },     // KATAKANA LETTER MA
        { 0xD0, 0x00, 0x255F },     // KATAKANA LETTER MI
        { 0xD1, 0x00, 0x2560 },     // KATAKANA LETTER MU
        { 0xD2, 0x00, 0x2561 },     // KATAKANA LETTER ME
        { 0xD3, 0x00, 0x2562 },     // KATAKANA LETTER MO
        { 0xD4, 0x00, 0x2564 },     // KATAKANA LETTER YA
        { 0xD5, 0x00, 0x2566 },     // KATAKANA LETTER YU
        { 0xD6, 0x00, 0x2568 },     // KATAKANA LETTER YO
        { 0xD7, 0x00, 0x2569 },     // KATAKANA LETTER RA
        { 0xD8, 0x00, 0x256A },     // KATAKANA LETTER RI
        { 0xD9, 0x00, 0x256B },     // KATAKANA LETTER RU
        { 0xDA, 0x00, 0x256C },     // KATAKANA LETTER RE
        { 0xDB, 0x00, 0x256D },     // KATAKANA LETTER RO
        { 0xDC, 0x00, 0x256F },     // KATAKANA LETTER WA
        { 0xDD, 0x00, 0x2573 },     // KATAKANA LETTER N
        { 0xDE, 0x00, 0x212B },     // HALFWIDTH KATAKANA VOICED SOUND MARK
        { 0xDF, 0x00, 0x212C }      // HALFWIDTH KATAKANA SEMI-VOICED SOUND MARK
    };
#define ARRAY_LENGTH(aArray) (sizeof(aArray)/sizeof((aArray)[0]))

LOCAL_C void ConvertFromHalfKatakanaToFullKatakana(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
    {
    // half-width Katakana is 1-byte, but full-width Katakana is 2-bytes.
    TInt convertlength = aDescriptor.Length()-aStartPositionInDescriptor;
    TInt numberOfMarkChar = 0;
    HBufC8* hBuf = HBufC8::New(convertlength*2);
    if (hBuf)
        {
        TPtr8 ptr = hBuf->Des();
        for (TInt i=aStartPositionInDescriptor; i < aDescriptor.Length(); i++)
            {
            const TUint8 convchar = (TUint8)(aDescriptor[i]|0x80);
            const TUint8 convnextchar = (TUint8)((aDescriptor.Length() > (i+1))?
                        (aDescriptor[i+1]|0x80) : TUint8(0x00));
            const TConvTblFromHalfKanaToFullKana* curTbl = &(convTblFromHalfKanaToFullKana[0]);
            for (TUint j=0; j < ARRAY_LENGTH(convTblFromHalfKanaToFullKana); curTbl++, j++)
                {
                if (convchar == curTbl->iHalfKana)
                    {
                    for (TInt k=0; (convchar == (curTbl+k)->iHalfKana); k++)
                        {
                        if (convnextchar == (curTbl+k)->iHalfKanaMark)
                            {
                            curTbl += k;
                            if (convnextchar == 0xDE || convnextchar == 0xDF)
                                {
                                i++;
                                numberOfMarkChar++;
                                }
                            break;
                            }
                        }
                    // set to buffer each 8 bit
                    const TUint8 highbit = (TUint8)(curTbl->iFullKana>>8);
                    const TUint8 lowbit = (TUint8)(curTbl->iFullKana|0xff00);
                    ptr.Append(highbit);
                    ptr.Append(lowbit);
                    break;
                    }
                }
            }
        // add ESC code
        CnvUtilities::ConvertFromIntermediateBufferInPlace(
                            aStartPositionInDescriptor, 
                            aDescriptor, aNumberOfCharactersThatDroppedOut, 
                            KLit8EscapeSequenceForJisX0208_1983, 1);
        if (!aNumberOfCharactersThatDroppedOut)
            {
            // delete half-width katakana
            aDescriptor.Delete(aStartPositionInDescriptor 
                                + KLit8EscapeSequenceForJisX0208_1983().Length(),
                               convertlength);

            TInt freelength = aDescriptor.MaxLength() - aDescriptor.Length();
            TInt copylength = ptr.Length();
            if (copylength > freelength)
                {
                copylength = freelength - (freelength%2);
                }
            if (copylength > 0)
                {
                // not convert, because there is no enough buffer
                aDescriptor.Append(ptr.Ptr(), copylength);
                aNumberOfCharactersThatDroppedOut=(convertlength-numberOfMarkChar) - (copylength/2);
                }
            else
                {
                // not convert, because there is no enough buffer.
                aNumberOfCharactersThatDroppedOut=convertlength;
                aDescriptor.SetLength(aStartPositionInDescriptor);
                }
            }
        delete hBuf;
        }
    else
        {
        // not convert, because there is no heap area.
        aNumberOfCharactersThatDroppedOut=convertlength;
        aDescriptor.SetLength(aStartPositionInDescriptor);
        }
    }

TInt CISO2022JPConverterImpl::ConvertFromUnicode(
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
        characterSet.iEscapeSequence=&KLit8EscapeSequenceForJisX0208_1983;
        characterSet.iConversionData=&CnvJisBase::HalfWidthKatakana7ConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromHalfKatakanaToFullKatakana;
        ret |= characterSets.Append(characterSet);

        SetCharacterSetsForPictograph(characterSets);
        }
    else
        {            
        CnvUtilities::SCharacterSet characterSet;
        characterSet.iConversionData=&CCnvCharacterSetConverter::AsciiConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromAsciiToJisInPlace;
        characterSet.iEscapeSequence=&KLit8EscapeSequenceForAscii;
        ret |= characterSets.Append(characterSet);
        characterSet.iConversionData=&CnvJisRoman::ConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromJisRomanToJisInPlace;
        characterSet.iEscapeSequence=&KLit8EscapeSequenceForJisRoman;
        ret |= characterSets.Append(characterSet);
        characterSet.iConversionData=&CnvJisX0208::ConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromJisX0208ToJisInPlace;
        characterSet.iEscapeSequence=&KLit8EscapeSequenceForJisX0208_1983;
        ret |= characterSets.Append(characterSet);
        characterSet.iEscapeSequence=&KLit8EscapeSequenceForJisX0208_1983;
        characterSet.iConversionData=&CnvJisBase::HalfWidthKatakana7ConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=ConvertFromHalfKatakanaToFullKatakana;
        ret |= characterSets.Append(characterSet);
        }
    __ASSERT_DEBUG(!ret, User::Panic(_L("RArray append failure"), ret));
    TUint notUsed;
//S60 30    TUint inputConversionFlags=CCnvCharacterSetConverter::EInputConversionFlagMustEndInDefaultCharacterSet;
    TUint inputConversionFlags=CCnvCharacterSetConverter::EInputConversionFlagMustEndInDefaultCharacterSet |
                               CCnvCharacterSetConverter::EInputConversionFlagAssumeStartInDefaultCharacterSet;
    TInt unconvert = CnvUtilities::ConvertFromUnicode(aDefaultEndiannessOfForeignCharacters,
                                                      aReplacementForUnconvertibleUnicodeCharacters, 
                                                      aForeign, 
                                                      aUnicode, 
                                                      aIndicesOfUnconvertibleCharacters, 
                                                      characterSets.Array(),
                                                      notUsed,
                                                      inputConversionFlags);
        
    characterSets.Close();

    return unconvert;
    }


TInt CISO2022JPConverterImpl::ConvertToUnicode(
        CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
        TDes16& aUnicode, 
        const TDesC8& aForeign, 
        TInt& aState, 
        TInt& aNumberOfUnconvertibleCharacters, 
        TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
    {
    return CnvJisBase::ConvertToUnicode(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aState, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
    }

TBool CISO2022JPConverterImpl::IsInThisCharacterSetL(
        TBool& aSetToTrue, 
        TInt& aConfidenceLevel, 
        const TDesC8& aSample)
    {
    aSetToTrue=ETrue;
    CnvJisBase::IsCharacterJISBased(aConfidenceLevel, aSample);
    return ETrue;
    }

CISO2022JPConverterImpl* CISO2022JPConverterImpl::NewL()
    {
    CISO2022JPConverterImpl* self = new(ELeave) CISO2022JPConverterImpl();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CISO2022JPConverterImpl::~CISO2022JPConverterImpl()
    {
    FeatureManager::UnInitializeLib();
    }

CISO2022JPConverterImpl::CISO2022JPConverterImpl()
    {
    }

void CISO2022JPConverterImpl::ConstructL()
    {
    FeatureManager::InitializeLibL();
    }

const TImplementationProxy ImplementationTable[] = 
    {
#ifdef S60_TEST
    IMPLEMENTATION_PROXY_ENTRY(0x01000006,  CISO2022JPConverterImpl::NewL)
#else
    IMPLEMENTATION_PROXY_ENTRY(0x100066A0,  CISO2022JPConverterImpl::NewL)
#endif        
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

