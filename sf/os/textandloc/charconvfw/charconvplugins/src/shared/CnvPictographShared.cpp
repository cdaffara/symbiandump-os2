/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:         This class is a utitlity class for conversion of Vodafone
*                pictograph.
*
*/








// PICTOGRAPH_SHARED.CPP
//
//

#include "CnvPictographShared.h"
// #include "CnvPictograph1.h"
#include "CnvShiftJisDirectmap.h"
#include <convgeneratedcpp.h>


// CONSTANTS
// Escape sequence for Vodafone pictograph
_LIT8(KLit8EscapeSequenceForPictograph, "\x1b\x24");
// _LIT8(KLit8EscapeSequenceForPictograph1, "\x1b\x24\x47");

const TInt KEscape = 0x1B;
const TInt KShiftIn = 0x0F;

// Intermidiate buffers for pictograph
const TInt KIntermediateBuffer1 = 'G';

const TUint KSingleByteRangeFirstBlockStart = 0xF0;
const TUint KSingleByteRangeFirstBlockEnd = 0xF9;
const TUint KSingleByteRangeSecondBlockStart = 0x40;
const TUint KSingleByteRangeSecondBlockEnd = 0xFC;

const TUint KFirstByteRangeFirstBlockStart = 0xF5;
const TUint KFirstByteRangeFirstBlockEnd = 0xFE;
const TUint KSecondByteRangeSecondBlockStart = 0xA1;
const TUint KSecondByteRangeSecondBlockEnd = 0xFE;

const TUint KEUCJPSecondBlockStart = 0xF5;
const TUint KEUCJPBlockSize = 0x5D;
const TUint KSingleShift3=0x8f;
// used for EucJp
const TUint KPictographStartFirstByte = 0xF0;
const TUint KPictographStartSecondByte = 0x40;
const TUint KShiftJisTrailByteIllegal = 0x7F;

// ============================ LOCAL FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// DummyConvertToIntermediateBufferInPlace ?description.
//
// -----------------------------------------------------------------------------
//
LOCAL_C void DummyConvertToIntermediateBufferInPlace(TDes8&)
    {
    }

// -----------------------------------------------------------------------------
// DummyConvertFromIntermediateBufferInPlace
//
// -----------------------------------------------------------------------------
//
LOCAL_C void DummyConvertFromIntermediateBufferInPlace(TInt, TDes8&,
                                TInt& aNumberOfCharactersThatDroppedOut)
    {
    aNumberOfCharactersThatDroppedOut=0;
    }

// -----------------------------------------------------------------------------
// ConvertFromPictogaphToEucJpDirectmapInPlace
// Converts from EucJp packed Pictograph to Unicode
// -----------------------------------------------------------------------------
//
LOCAL_C void ConvertFromPictogaphToEucJpDirectmapInPlace(
        TInt aStartPositionInDescriptor, TDes8& aDescriptor,
        TInt& aNumberOfCharactersThatDroppedOut)
    {
    TInt descriptorLength=aDescriptor.Length();
    TInt bytesPerCharacter = 2;
    if (aDescriptor[0] >= KEUCJPSecondBlockStart)
        {
        bytesPerCharacter = 3;
        }

    aNumberOfCharactersThatDroppedOut =
        Max(0, ((descriptorLength - aStartPositionInDescriptor) / 2) -
        ((aDescriptor.MaxLength() - aStartPositionInDescriptor) /
        bytesPerCharacter));
    descriptorLength -= aNumberOfCharactersThatDroppedOut * 2;
    if (descriptorLength <= aStartPositionInDescriptor)
        {
        aDescriptor.SetLength(descriptorLength);
        }
    else
        {
        // pointerToTargetByte is initialized properly when descriptorLength
        // has been offset to the actual final length of aDescriptor
        TUint8* pointerToTargetByte = CONST_CAST(TUint8*, aDescriptor.Ptr());
        const TUint8* const pointerToFirstByte =
            pointerToTargetByte + aStartPositionInDescriptor;
        const TUint8* pointerToSourceByte =
            pointerToTargetByte + (descriptorLength - 1);
        descriptorLength = (((descriptorLength - aStartPositionInDescriptor)
            * bytesPerCharacter) / 2) + aStartPositionInDescriptor;
        aDescriptor.SetLength(descriptorLength);
        // pointerToTargetByte is is initialized properly here
        pointerToTargetByte += descriptorLength - 1;
        for (;pointerToTargetByte > pointerToFirstByte; )
            {
            TInt secondByte = *pointerToSourceByte;
            TInt firstByte = *(pointerToSourceByte - 1);

            if (bytesPerCharacter == 3)
                {
                firstByte = (firstByte - KEUCJPSecondBlockStart) * 2 +
                    KFirstByteRangeFirstBlockStart;
                }
            else
                {
                firstByte = (firstByte - KPictographStartFirstByte) * 2 +
                    KFirstByteRangeFirstBlockStart;
                }
            if (static_cast<TInt>(KEUCJPBlockSize + KPictographStartSecondByte + 1)
                 < secondByte)
                {
                if (secondByte > KShiftJisTrailByteIllegal)
                    secondByte -= 1;
                secondByte = secondByte -(KPictographStartSecondByte +
                    KEUCJPBlockSize + 1) + KSecondByteRangeSecondBlockStart;
                firstByte++;
                }
            else
                {
                if (secondByte > KShiftJisTrailByteIllegal)
                    secondByte -= 1;
                secondByte += KSecondByteRangeSecondBlockStart - KPictographStartSecondByte;
                }
            *pointerToTargetByte = static_cast<TUint8>(secondByte);
            --pointerToTargetByte;
            *pointerToTargetByte = static_cast<TUint8>(firstByte);
            if (bytesPerCharacter == 3)
                {
                --pointerToTargetByte;
                *pointerToTargetByte = KSingleShift3;
                }
            --pointerToTargetByte;
            pointerToSourceByte -= 2;
            }
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CnvPictographShared::SetCharacterSetsForPictograph
// It sets parameters to SCharacterSet array. It called in ConvertFromUnicode().
// It makes possible that pictograph conversion is called from other conversion
// plug-ins.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CnvPictographShared::SetCharacterSetsForPictograph(
        RArray<CnvUtilities::SCharacterSet>& /*aArrayOfCharacterSets*/)
    {
    // SAMPLE CODE
    // If this module needs to support escape sequence pictograph,
    // Remove the comment from the following code.

    // CnvUtilities::SCharacterSet characterSet;
    // characterSet.iConversionData = &CnvPictograph1::ConversionData();
    // characterSet.iConvertFromIntermediateBufferInPlace =
    //     CnvPictographShared::ConvertToPictograph1InPlace;
    // characterSet.iEscapeSequence = &KLit8EscapeSequenceForPictograph1;
    // aArrayOfCharacterSets.Append(characterSet);

    }

// -----------------------------------------------------------------------------
// CnvPictographShared::SetMethodsForPictograph
// It sets parameters to SMethod array. It called in ConvertToUnicode().
// It makes possible that pictograph conversion is called from other conversion
// plug-ins.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CnvPictographShared::SetMethodsForPictograph(
        RArray<CnvUtilities::SMethod>& aArrayOfMethods, TOriginalCharset aOriginalCharset)
    {
    TInt ret = KErrNone;
    CnvUtilities::SMethod method;
    switch (aOriginalCharset)
        {
        case ECharsetShiftJis:
            method.iNumberOfBytesAbleToConvert =
                CnvPictographShared::NumberOfBytesAbleToConvertSJisDirectMap;
            method.iConvertToIntermediateBufferInPlace =
                DummyConvertToIntermediateBufferInPlace;
            method.iConversionData =
                &CnvShiftJisDirectmap::ConversionData();
            method.iNumberOfBytesPerCharacter = 2;
            method.iNumberOfCoreBytesPerCharacter = 2;
            ret |= aArrayOfMethods.Append(method);
            break;
        case ECharsetEucJp:
            method.iNumberOfBytesAbleToConvert =
                CnvPictographShared::NumberOfBytesAbleToConvertToEucJpDirectMap1;
            method.iConvertToIntermediateBufferInPlace =
                CnvPictographShared::ConvertToPictographFromEucJpPackedInPlace;
            method.iConversionData =
                &CnvShiftJisDirectmap::ConversionData();
            method.iNumberOfBytesPerCharacter = 2;
            method.iNumberOfCoreBytesPerCharacter = 2;
            ret |= aArrayOfMethods.Append(method);
            method.iNumberOfBytesAbleToConvert =
                CnvPictographShared::NumberOfBytesAbleToConvertToEucJpDirectMap2;
            method.iConvertToIntermediateBufferInPlace =
                CnvPictographShared::ConvertToPictographFromEucJpPackedInPlace;
            method.iConversionData =
                &CnvShiftJisDirectmap::ConversionData();
            method.iNumberOfBytesPerCharacter = 3;
            method.iNumberOfCoreBytesPerCharacter = 2;
            ret |= aArrayOfMethods.Append(method);
            break;
        default:
            break;
        }
    __ASSERT_DEBUG(!ret, User::Panic(_L("RArray append failure"), ret));
    // SAMPLE CODE
    // If this module needs to support escape sequence pictograph,
    // Remove the comment from the following code.

    // method.iNumberOfBytesAbleToConvert =
    //     CnvPictographShared::NumberOfBytesAbleToConvertToPicto1;
    // method.iConvertToIntermediateBufferInPlace =
    //     CnvPictographShared::ConvertToPictographFromUnicode;
    // method.iConversionData = &CnvPictograph1::ConversionData();
    // method.iNumberOfBytesPerCharacter = 1;
    // method.iNumberOfCoreBytesPerCharacter = 1;
    // aArrayOfMethods.Append(method);
    }

// -----------------------------------------------------------------------------
// CnvPictographShared::SetCharacterSetsForPictograph
// It sets parameters to SState array. It called in ConvertToUnicode().
// It makes possible that pictograph conversion is called from other
// conversion plug-ins.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CnvPictographShared::SetCharacterSetsForPictograph(
        RArray<CnvUtilities::SCharacterSet>& aArrayOfStates,
        TOriginalCharset aOriginalCharset)
    {
    TInt ret = KErrNone;
    CnvUtilities::SCharacterSet characterSet;
    switch (aOriginalCharset)
        {
        case ECharsetShiftJis:
            // Append the character set for pictograph
            characterSet.iConversionData =
                &CnvShiftJisDirectmap::ConversionData();
            characterSet.iConvertFromIntermediateBufferInPlace =
                DummyConvertFromIntermediateBufferInPlace;
            characterSet.iEscapeSequence = &KNullDesC8;
            ret |= aArrayOfStates.Append(characterSet);
            break;
        case ECharsetEucJp:
            // Append the character set for pictograph1
            characterSet.iConversionData = &CnvShiftJisDirectmap::ConversionData();
            characterSet.iConvertFromIntermediateBufferInPlace =
                ConvertFromPictogaphToEucJpDirectmapInPlace;
            characterSet.iEscapeSequence = &KNullDesC8;
            ret |= aArrayOfStates.Append(characterSet);

            // Append the character set for pictograph2
            characterSet.iConversionData = &CnvShiftJisDirectmap::ConversionData();
            characterSet.iConvertFromIntermediateBufferInPlace =
                ConvertFromPictogaphToEucJpDirectmapInPlace;
            characterSet.iEscapeSequence = &KNullDesC8;
            ret |= aArrayOfStates.Append(characterSet);
            break;
        default:
            break;
        }
    __ASSERT_DEBUG(!ret, User::Panic(_L("RArray append failure"), ret));
    }

// -----------------------------------------------------------------------------
// CnvPictographShared::SetStatesForPictograph
// It sets parameters to SState array. It called in ConvertToUnicode().
// It makes possible that pictograph conversion is called from other
// conversion plug-ins.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CnvPictographShared::SetStatesForPictograph(
        RArray<CnvUtilities::SState>& /*aArrayOfStates*/)
    {
    // SAMPLE CODE
    // If this module needs to support escape sequence pictograph,
    // Remove the comment from the following code.

    // CnvUtilities::SState state;
    // state.iEscapeSequence = &KLit8EscapeSequenceForPictograph1;
    // state.iConversionData = &CnvPictograph1::ConversionData();
    // aArrayOfStates.Append(state);
    }

// -----------------------------------------------------------------------------
// CnvPictographShared::ConvertToPictograph1InPlace
// It converts from an intermediate buffer to a pictograph code.
// After that, it adds shift-in code(0x0F).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
// void CnvPictographShared::ConvertToPictograph1InPlace(
//         TInt aStartPositionInDescriptor,
//         TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
//     {
//     CnvUtilities::ConvertFromIntermediateBufferInPlace(
//         aStartPositionInDescriptor, aDescriptor,
//         aNumberOfCharactersThatDroppedOut,
//         KLit8EscapeSequenceForPictograph1, 1);
//     AppendShiftIn(aDescriptor, aNumberOfCharactersThatDroppedOut);
//     }


// -----------------------------------------------------------------------------
// CnvPictographShared::NumberOfBytesAbleToConvertToPicto1
// It returns a number of bytes that the conversion to pictograph is
// possible in the descriptor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CnvPictographShared::NumberOfBytesAbleToConvertSJisDirectMap(
        const TDesC8& aDescriptor)
    {
    const TUint8* pointerToPreviousByte = aDescriptor.Ptr()-1;
    const TUint8* const pointerToLastByte =
            pointerToPreviousByte + aDescriptor.Length();
    if (pointerToPreviousByte == pointerToLastByte)
        {
        return 0;
        }
    for (; pointerToPreviousByte < pointerToLastByte; )
        {
        const TUint currentByte = *(pointerToPreviousByte + 1);
        const TUint nextByte = *(pointerToPreviousByte + 2);
        if (((currentByte < KSingleByteRangeFirstBlockStart) ||
             (currentByte > KSingleByteRangeFirstBlockEnd)) ||
            ((nextByte < KSingleByteRangeSecondBlockStart) ||
             (nextByte > KSingleByteRangeSecondBlockEnd)))
            {
            break;
            }
        pointerToPreviousByte += 2;
        }
    return (pointerToPreviousByte + 1) - aDescriptor.Ptr();
    }

// -----------------------------------------------------------------------------
// NumberOfBytesAbleToConvertToEucJpDirectMap1
//
// -----------------------------------------------------------------------------
//
TInt CnvPictographShared::NumberOfBytesAbleToConvertToEucJpDirectMap1(
        const TDesC8& aDescriptor)
    {
    const TUint8* pointerToPreviousByte = aDescriptor.Ptr() - 1;
    const TUint8* const pointerToLastByte =
        pointerToPreviousByte + aDescriptor.Length();
    if (pointerToPreviousByte == pointerToLastByte)
        {
        return 0;
        }
    for (;pointerToPreviousByte < pointerToLastByte;)
        {
        TUint currentByte = *(pointerToPreviousByte + 1);
        const TUint nextByte = *(pointerToPreviousByte + 2);
        if (((currentByte < KFirstByteRangeFirstBlockStart) ||
             (currentByte > KFirstByteRangeFirstBlockEnd)) ||
            ((nextByte < KSecondByteRangeSecondBlockStart) ||
             (nextByte > KSecondByteRangeSecondBlockEnd)))
            {
            break;
            }
        pointerToPreviousByte += 2;
        }
    return (pointerToPreviousByte + 1) - aDescriptor.Ptr();
    }

// -----------------------------------------------------------------------------
// NumberOfBytesAbleToConvertToPictograph
//
// -----------------------------------------------------------------------------
//
TInt CnvPictographShared::NumberOfBytesAbleToConvertToEucJpDirectMap2(
        const TDesC8& aDescriptor)
    {
    const TUint8* pointerToPreviousByte = aDescriptor.Ptr() - 1;
    const TUint8* const pointerToLastByte =
        pointerToPreviousByte + aDescriptor.Length();
    if (pointerToPreviousByte == pointerToLastByte)
        {
        return 0;
        }
    for (;pointerToPreviousByte < pointerToLastByte;)
        {
        TUint currentByte = *(pointerToPreviousByte + 1);
        if (currentByte == KSingleShift3)
            {
            pointerToPreviousByte++;
            currentByte = *(pointerToPreviousByte + 1);
            }
        else
            {
            break;
            }
        if (currentByte < 0xa0)
            {
            return CCnvCharacterSetConverter::EErrorIllFormedInput;
            }
        const TUint nextByte = *(pointerToPreviousByte + 2);
        if (nextByte < 0xa0)
            {
            return CCnvCharacterSetConverter::EErrorIllFormedInput;
            }
        if (((currentByte < KFirstByteRangeFirstBlockStart) ||
             (currentByte > KFirstByteRangeFirstBlockEnd)) ||
            ((nextByte < KSecondByteRangeSecondBlockStart) ||
             (nextByte > KSecondByteRangeSecondBlockEnd)))
            {
            // return the previous byte to the beginning of loop.
            pointerToPreviousByte--;
            break;
            }
        pointerToPreviousByte += 2;
        }
    return (pointerToPreviousByte + 1) - aDescriptor.Ptr();
    }

// -----------------------------------------------------------------------------
// CnvPictographShared::NumberOfBytesAbleToConvertToPicto1
// It returns a number of bytes that the conversion to pictograph is
// possible in the descriptor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CnvPictographShared::NumberOfBytesAbleToConvertToPicto1(
        const TDesC8& aDescriptor)
    {
    return NumberOfBytesAbleToConvertToPictograph(aDescriptor,
        KIntermediateBuffer1);
    }

// -----------------------------------------------------------------------------
// CnvPictographShared::NumberOfBytesAbleToConvertToPictograph
// It's a common function for checking the number of bytes that the conversion
// to pictograph
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CnvPictographShared::NumberOfBytesAbleToConvertToPictograph(
        const TDesC8& aDescriptor, TInt aIntermidiateCode)
    {
    TInt compLen(KLit8EscapeSequenceForPictograph().Length());
    TInt ret(0);
    if (aDescriptor.Left(compLen) == KLit8EscapeSequenceForPictograph)
        {
        const TInt intermidiateBuff = aDescriptor[compLen];
        if (intermidiateBuff == aIntermidiateCode)
            {
            ret = compLen + 2;

            const TInt descriptorLength = aDescriptor.Length();
            const TUint8* pointerToCurrentByte = aDescriptor.Ptr();
            const TUint8* pointerToLastByte = pointerToCurrentByte +
                (descriptorLength - 1);
            pointerToCurrentByte += compLen + 2;
            for (; pointerToCurrentByte <= pointerToLastByte;
                 pointerToCurrentByte++)
                {
                if (*pointerToCurrentByte == KShiftIn)
                    {
                    ret++;
                    break;
                    }
                if (*pointerToCurrentByte == KEscape)
                    break;
                ret++;
                }
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CnvPictographShared::ConvertToPictographFromUnicode
// It converts the string which includes a pictograph's escape sequence
// to unicode.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CnvPictographShared::ConvertToPictographFromUnicode(TDes8& aDescriptor)
    {
    const TInt descriptorLength = aDescriptor.Length();
    TUint8* pointerToCurrentByte = CONST_CAST(TUint8*, aDescriptor.Ptr());
    TUint8* pointerToDataByte = pointerToCurrentByte +
        KLit8EscapeSequenceForPictograph().Length();
    const TUint8* const pointerToLastByte = pointerToCurrentByte +
        (descriptorLength - 1);
    const TUint8* const pointerToFirstByte = pointerToCurrentByte;
    pointerToDataByte++;

    for (; pointerToDataByte <= pointerToLastByte; pointerToDataByte++)
        {
        if (*pointerToDataByte != KShiftIn)
            {
            *pointerToCurrentByte = *pointerToDataByte;
            pointerToCurrentByte++;
            }
        }
    aDescriptor.SetLength(pointerToCurrentByte - pointerToFirstByte);
    }

// -----------------------------------------------------------------------------
// CnvPictographShared::ConvertToPictographFromEucJpPackedInPlace
// Converts from EucJp packed Pictograph to Unicode
// -----------------------------------------------------------------------------
//
void CnvPictographShared::ConvertToPictographFromEucJpPackedInPlace(TDes8& aDescriptor)
    {
    const TInt descriptorLength = aDescriptor.Length();
    TUint8* pointerToTargetByte = CONST_CAST(TUint8*, aDescriptor.Ptr());
    const TUint8* pointerToSourceByte = pointerToTargetByte;
    const TUint8* const pointerToLastByte =
        pointerToSourceByte + (descriptorLength - 1);
    TInt bytesPerCharacter = 2;
    TInt sjisStartbyte = KSingleByteRangeFirstBlockStart;
    if (*pointerToSourceByte == KSingleShift3)
        {
        ++pointerToSourceByte;
        bytesPerCharacter = 3;
        sjisStartbyte = KEUCJPSecondBlockStart;
        }
    for (;pointerToSourceByte < pointerToLastByte; )
        {
        TUint firstByte = (*pointerToSourceByte
            - KFirstByteRangeFirstBlockStart) / 2 + sjisStartbyte;
        TUint secondByte = *(pointerToSourceByte + 1);
        if ((*pointerToSourceByte % 2) == 0)
            {
            secondByte += KSingleByteRangeSecondBlockStart - KSecondByteRangeSecondBlockStart
                + KEUCJPBlockSize + 1;
            if (secondByte >= KShiftJisTrailByteIllegal)
                secondByte++;
            }
        else
            {
            secondByte += KSingleByteRangeSecondBlockStart - KSecondByteRangeSecondBlockStart;
            if (secondByte >= KShiftJisTrailByteIllegal)
                secondByte++;
            }
        *pointerToTargetByte = static_cast<TUint8>(firstByte);

        ++pointerToTargetByte;
        *pointerToTargetByte = static_cast<TUint8>(secondByte);
        pointerToSourceByte += 2;
        ++pointerToTargetByte;
        }
    aDescriptor.SetLength((descriptorLength / bytesPerCharacter) * 2);
    }

// -----------------------------------------------------------------------------
// CnvPictographShared::AppendShiftIn
// It appends a shift-in code to the descriptor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CnvPictographShared::AppendShiftIn(TDes8& aDescriptor,
        TInt& aNumberOfCharactersThatDroppedOut)
    {
    if (aDescriptor.MaxLength() > aDescriptor.Length())
        {
        aDescriptor.Append(KShiftIn);
        }
    else
        {
        aNumberOfCharactersThatDroppedOut++;
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// SetCharacterSetsForPictograph()
// It sets parameters to SCharacterSet array.
// It called in ConvertFromUnicode().
// It makes possible that pictograph conversion is called from other
// conversion plug-ins.
// @since 2.6
// @param aArrayOfCharacterSets The conversion table array
// -----------------------------------------------------------------------------
//
EXPORT_C void SetCharacterSetsForPictograph(
            RArray<CnvUtilities::SCharacterSet>& aArrayOfCharacterSets)
    {
    CnvPictographShared::SetCharacterSetsForPictograph(aArrayOfCharacterSets);
    }

// -----------------------------------------------------------------------------
// SetCharacterSetsForPictograph()
// It sets parameters to SCharacterSet array.
// It called in ConvertFromUnicode().
// It makes possible that pictograph conversion is called from other
// conversion plug-ins.
// @since 2.6
// @param aArrayOfCharacterSets The conversion table array
// @param aOriginalCharset The character set
// -----------------------------------------------------------------------------
//
EXPORT_C void SetCharacterSetsForPictograph(
            RArray<CnvUtilities::SCharacterSet>& aArrayOfCharacterSets,
            TOriginalCharset aOriginalCharset)
    {
    CnvPictographShared::SetCharacterSetsForPictograph(aArrayOfCharacterSets,
                                                       aOriginalCharset);
    }

// -----------------------------------------------------------------------------
// SetMethodsForPictograph()
// It sets parameters to SMethod array.
// It called in ConvertToUnicode().
// It makes possible that pictograph conversion is called from other
// conversion plug-ins.
// @since 2.6
// @param aArrayOfMethods The conversion table array
// @param aOriginalCharset The character set
// -----------------------------------------------------------------------------
//
EXPORT_C void SetMethodsForPictograph(
            RArray<CnvUtilities::SMethod>& aArrayOfMethods,
            TOriginalCharset aOriginalCharset)
    {
    CnvPictographShared::SetMethodsForPictograph(aArrayOfMethods,
        aOriginalCharset);
    }

// -----------------------------------------------------------------------------
// SetStatesForPictograph()
// It sets parameters to SState array.
// It called in ConvertToUnicode().
// It makes possible that pictograph conversion is called from other
// conversion plug-ins.
// @since 2.6
// @param aArrayOfMethods The conversion table array
// -----------------------------------------------------------------------------
//
EXPORT_C void SetStatesForPictograph(
            RArray<CnvUtilities::SState>& aArrayOfStates)
    {
    CnvPictographShared::SetStatesForPictograph(aArrayOfStates);
    }

// -----------------------------------------------------------------------------
// CountOfPictographPagesFromUnicode()
// It returns count of pictograph pages.
// This method is used for the conversion from Unicode to foreign character
// set.
// @since 2.6
// @return count of pictograph pages.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CountOfPictographPagesFromUnicode()
    {
    return KCountOfPictoPages;
    }

// -----------------------------------------------------------------------------
// CountOfPictographPagesToUnicode()
// It returns count of pictograph pages.
// This method is used for the conversion from foreign character set to
// Unicode.
// @since 2.6
// @return count of pictograph pages.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CountOfPictographPagesToUnicode()
    {
    return KCountOfPictoPages + 1;
    }

//  End of File
