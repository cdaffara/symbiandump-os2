/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    This file is a source code file for a charconv plug-in.
*                This plug-in supports EUC-JP with direct mapped pictograph.
*
*/







// INCLUDE FILES

#include <e32std.h>

#include <charconv.h>



#ifndef EKA2

#include <CONVPLUG.H>

#else

#include <convgeneratedcpp.h>

#include <ecom/implementationproxy.h>

#include "charactersetconverter.h"

#endif // !EKA2



#include <convutils.h>

#include "jisx0201.h"

#include "jisx0208.h"

#include "jisx0212.h"

#include "CnvShiftJisDirectmap.h"

// LOCAL CONSTANTS AND MACROS

const TUint KSingleShift2=0x8e;

const TUint KSingleShift3=0x8f;

const TUint KFirstByteRangeFirstBlockStart = 0xF5;

const TUint KFirstByteRangeFirstBlockEnd = 0xFE;

const TUint KSecondByteRangeSecondBlockStart = 0xA1;

const TUint KSecondByteRangeSecondBlockEnd = 0xFE;

const TUint KPictographStartFirstByte = 0xF0;

const TUint KPictographStartSecondByte = 0x40;

const TUint KEUCJPSecondBlockStart = 0xF5;

const TUint KEUCJPBlockSize = 0x5D;

const TUint KShiftJisTrailByteIllegal = 0x7F;



// SecureID for Brower app

const TUint32 KBrowserSecureId = 0x10008D39;

// Define for converting from YenSign to BackSlash

const TUint KCharacterCodeForYenSign = 0x00A5;

const TUint KCharacterCodeForBackSlash = 0x005C;



// fullwidth question mark

_LIT8(KLit8EucJpPackedReplacementForUnconvertibleUnicodeCharacters, "\xa1\xa9");

#if defined(_DEBUG)

_LIT(KLitPanicText, "EucJpDirectmap");

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



// -----------------------------------------------------------------------------

// DummyConvertFromIntermediateBufferInPlace

//

// -----------------------------------------------------------------------------

//

LOCAL_C void DummyConvertFromIntermediateBufferInPlace(TInt, TDes8&,

        TInt& aNumberOfCharactersThatDroppedOut)

    {

    aNumberOfCharactersThatDroppedOut = 0;

    }



// -----------------------------------------------------------------------------

// ConvertFromJisX0208ToEucJpPackedInPlace

// Converts from JIS code to EUC-JP

// -----------------------------------------------------------------------------

//

LOCAL_C void ConvertFromJisX0208ToEucJpPackedInPlace(

        TInt aStartPositionInDescriptor, TDes8& aDescriptor,

        TInt& aNumberOfCharactersThatDroppedOut)

    {

    aNumberOfCharactersThatDroppedOut = 0;

    const TInt descriptorLength = aDescriptor.Length();

    __ASSERT_DEBUG(descriptorLength > aStartPositionInDescriptor,

        Panic(EPanicNothingToConvert1));

    __ASSERT_DEBUG((descriptorLength - aStartPositionInDescriptor) % 2 == 0,

        Panic(EPanicOddNumberOfBytes1));

    TUint8* pointerToCurrentByte = CONST_CAST(TUint8*, aDescriptor.Ptr());

    const TUint8* const pointerToLastByte =

        pointerToCurrentByte + (descriptorLength - 1);

    pointerToCurrentByte += aStartPositionInDescriptor;

    FOREVER

        {

        const TUint currentByte = *pointerToCurrentByte;

        __ASSERT_DEBUG((currentByte & 0x80) == 0, Panic(EPanicBadHighBit1));

        *pointerToCurrentByte = STATIC_CAST(TUint8, currentByte | 0x80);

        __ASSERT_DEBUG(pointerToCurrentByte <= pointerToLastByte,

            Panic(EPanicBadPointers1));

        if (pointerToCurrentByte >= pointerToLastByte)

            {

            break;

            }

        ++pointerToCurrentByte;

        }

    }



// -----------------------------------------------------------------------------

// ConvertFromHalfWidthKatakana8ToEucJpPackedInPlace

// Converts from half width Katakana code to EUC-JP

// -----------------------------------------------------------------------------

//

LOCAL_C void ConvertFromHalfWidthKatakana8ToEucJpPackedInPlace(

        TInt aStartPositionInDescriptor, TDes8& aDescriptor,

        TInt& aNumberOfCharactersThatDroppedOut)

    {

    TInt descriptorLength = aDescriptor.Length();

    __ASSERT_DEBUG(descriptorLength > aStartPositionInDescriptor,

        Panic(EPanicNothingToConvert2));

    aNumberOfCharactersThatDroppedOut = Max(0, (descriptorLength -

        aStartPositionInDescriptor) - ((aDescriptor.MaxLength() -

        aStartPositionInDescriptor) / 2));

    descriptorLength -= aNumberOfCharactersThatDroppedOut;

    __ASSERT_DEBUG(descriptorLength >= aStartPositionInDescriptor,

        Panic(EPanicBadCalculation1));

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

        descriptorLength =

            ((descriptorLength - aStartPositionInDescriptor) * 2) +

            aStartPositionInDescriptor;

        __ASSERT_DEBUG((descriptorLength - aStartPositionInDescriptor) % 2 == 0,

            Panic(EPanicOddNumberOfBytes2));

        aDescriptor.SetLength(descriptorLength);

        // pointerToTargetByte is is initialized properly here

        pointerToTargetByte += descriptorLength - 1;

        FOREVER

            {

            *pointerToTargetByte = *pointerToSourceByte;

            __ASSERT_DEBUG(pointerToTargetByte>pointerToFirstByte,

                Panic(EPanicBadPointers2));

            --pointerToTargetByte;

            *pointerToTargetByte = KSingleShift2;

            __ASSERT_DEBUG(pointerToTargetByte >= pointerToFirstByte,

                Panic(EPanicBadPointers3));

            if (pointerToTargetByte <= pointerToFirstByte)

                {

                break;

                }

            --pointerToTargetByte;

            __ASSERT_DEBUG(pointerToSourceByte > pointerToFirstByte,

                Panic(EPanicBadPointers4));

            --pointerToSourceByte;

            }

        __ASSERT_DEBUG(pointerToTargetByte == pointerToFirstByte,

            Panic(EPanicBadPointers5));

        __ASSERT_DEBUG(pointerToSourceByte == pointerToFirstByte,

            Panic(EPanicBadPointers6));

        }

    }



// -----------------------------------------------------------------------------

// ConvertFromJisX0212ToEucJpPackedInPlace

// Converts from JIS code to EUC-JP

// -----------------------------------------------------------------------------

//

LOCAL_C void ConvertFromJisX0212ToEucJpPackedInPlace(

        TInt aStartPositionInDescriptor, TDes8& aDescriptor,

        TInt& aNumberOfCharactersThatDroppedOut)

    {

    TInt descriptorLength=aDescriptor.Length();

    __ASSERT_DEBUG(descriptorLength > aStartPositionInDescriptor,

        Panic(EPanicNothingToConvert3));

    __ASSERT_DEBUG((descriptorLength - aStartPositionInDescriptor) % 2 == 0,

        Panic(EPanicOddNumberOfBytes3));

    aNumberOfCharactersThatDroppedOut =

        Max(0, ((descriptorLength - aStartPositionInDescriptor) / 2) -

        ((aDescriptor.MaxLength() - aStartPositionInDescriptor) / 3));

    descriptorLength -= aNumberOfCharactersThatDroppedOut * 2;

    __ASSERT_DEBUG(descriptorLength >= aStartPositionInDescriptor,

        Panic(EPanicBadCalculation2));

    if (descriptorLength <= aStartPositionInDescriptor)

        {

        aDescriptor.SetLength(descriptorLength);

        }

    else

        {

        __ASSERT_DEBUG((descriptorLength - aStartPositionInDescriptor) % 2 == 0,

            Panic(EPanicOddNumberOfBytes4));

        // pointerToTargetByte is initialized properly when descriptorLength

        // has been offset to the actual final length of aDescriptor

        TUint8* pointerToTargetByte = CONST_CAST(TUint8*, aDescriptor.Ptr());

        const TUint8* const pointerToFirstByte =

            pointerToTargetByte + aStartPositionInDescriptor;

        const TUint8* pointerToSourceByte =

            pointerToTargetByte + (descriptorLength - 1);

        descriptorLength = (((descriptorLength - aStartPositionInDescriptor)

            * 3) / 2) + aStartPositionInDescriptor;

        __ASSERT_DEBUG((descriptorLength - aStartPositionInDescriptor) % 3 ==0,

            Panic(EPanicNumberOfBytesIsNotMultipleOfThree1));

        aDescriptor.SetLength(descriptorLength);

        // pointerToTargetByte is is initialized properly here

        pointerToTargetByte += descriptorLength - 1;

        FOREVER

            {

            __ASSERT_DEBUG((*pointerToSourceByte & 0x80) == 0,

                Panic(EPanicBadHighBit2));

            *pointerToTargetByte =

                STATIC_CAST(TUint8, *pointerToSourceByte | 0x80);

            __ASSERT_DEBUG(pointerToTargetByte > pointerToFirstByte,

                Panic(EPanicBadPointers7));

            --pointerToTargetByte;

            __ASSERT_DEBUG(pointerToSourceByte > pointerToFirstByte,

                Panic(EPanicBadPointers8));

            --pointerToSourceByte;

            __ASSERT_DEBUG((*pointerToSourceByte & 0x80) == 0,

                Panic(EPanicBadHighBit3));

            *pointerToTargetByte =

                STATIC_CAST(TUint8, *pointerToSourceByte | 0x80);

            __ASSERT_DEBUG(pointerToTargetByte > pointerToFirstByte,

                Panic(EPanicBadPointers9));

            --pointerToTargetByte;

            *pointerToTargetByte = KSingleShift3;

            __ASSERT_DEBUG(pointerToTargetByte >= pointerToFirstByte,

                Panic(EPanicBadPointers10));

            if (pointerToTargetByte <= pointerToFirstByte)

                {

                break;

                }

            --pointerToTargetByte;

            __ASSERT_DEBUG(pointerToSourceByte > pointerToFirstByte,

                Panic(EPanicBadPointers11));

            --pointerToSourceByte;

            }

        __ASSERT_DEBUG(pointerToTargetByte == pointerToFirstByte,

            Panic(EPanicBadPointers12));

        __ASSERT_DEBUG(pointerToSourceByte == pointerToFirstByte,

            Panic(EPanicBadPointers13));

        }

    }



// -----------------------------------------------------------------------------

// NumberOfBytesAbleToConvertToJisRoman

// Counts the bytes of be able to convert to JIS

// -----------------------------------------------------------------------------

//

LOCAL_C TInt NumberOfBytesAbleToConvertToJisRoman(const TDesC8& aDescriptor)

    {

    const TUint8* pointerToPreviousByte = aDescriptor.Ptr() - 1;

    const TUint8* const pointerToLastByte =

        pointerToPreviousByte + aDescriptor.Length();

    if (pointerToPreviousByte == pointerToLastByte)

        {

        return 0;

        }

    FOREVER

        {

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers14));

        const TUint currentByte = *(pointerToPreviousByte + 1);

        if (currentByte & 0x80)

            {

            break;

            }

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers15));

        ++pointerToPreviousByte;

        __ASSERT_DEBUG(pointerToPreviousByte <= pointerToLastByte,

            Panic(EPanicBadPointers16));

        if (pointerToPreviousByte >= pointerToLastByte)

            {

            break;

            }

        }

    return (pointerToPreviousByte + 1) - aDescriptor.Ptr();

    }



// -----------------------------------------------------------------------------

// NumberOfBytesAbleToConvertToJisX0208

// Counts the bytes of be able to convert to JIS

// -----------------------------------------------------------------------------

//

LOCAL_C TInt NumberOfBytesAbleToConvertToJisX0208(const TDesC8& aDescriptor)

    {

    const TUint8* pointerToPreviousByte = aDescriptor.Ptr() - 1;

    const TUint8* const pointerToLastByte =

        pointerToPreviousByte + aDescriptor.Length();

    if (pointerToPreviousByte == pointerToLastByte)

        {

        return 0;

        }

    FOREVER

        {

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers17));

        TUint currentByte = *(pointerToPreviousByte + 1);

        if (currentByte < 0xa0)

            {

            break;

            }

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers18));

        if (pointerToLastByte - pointerToPreviousByte < 2)

            {

            break;

            }

        ++pointerToPreviousByte;

        currentByte = *(pointerToPreviousByte + 1);

        if (currentByte < 0xa0)

            {

            return CCnvCharacterSetConverter::EErrorIllFormedInput;

            }

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers19));

        ++pointerToPreviousByte;

        __ASSERT_DEBUG(pointerToPreviousByte <= pointerToLastByte,

            Panic(EPanicBadPointers20));

        if (pointerToPreviousByte >= pointerToLastByte)

            {

            break;

            }

        }

    return (pointerToPreviousByte + 1) - aDescriptor.Ptr();

    }



// -----------------------------------------------------------------------------

// NumberOfBytesAbleToConvertToHalfWidthKatakana8

// Counts the bytes of be able to convert to half width Katakana

// -----------------------------------------------------------------------------

//

LOCAL_C TInt NumberOfBytesAbleToConvertToHalfWidthKatakana8(const TDesC8& aDescriptor)

    {

    const TUint8* pointerToPreviousByte = aDescriptor.Ptr() - 1;

    const TUint8* const pointerToLastByte =

        pointerToPreviousByte + aDescriptor.Length();

    if (pointerToPreviousByte == pointerToLastByte)

        {

        return 0;

        }

    FOREVER

        {

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers21));

        TUint currentByte = *(pointerToPreviousByte + 1);

        if (currentByte != KSingleShift2)

            {

            break;

            }

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers22));

        if (pointerToLastByte - pointerToPreviousByte < 2)

            {

            break;

            }

        ++pointerToPreviousByte;

        currentByte = *(pointerToPreviousByte + 1);

        if (currentByte < 0xa0)

            {

            return CCnvCharacterSetConverter::EErrorIllFormedInput;

            }

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers23));

        ++pointerToPreviousByte;

        __ASSERT_DEBUG(pointerToPreviousByte <= pointerToLastByte,

            Panic(EPanicBadPointers24));

        if (pointerToPreviousByte >= pointerToLastByte)

            {

            break;

            }

        }

    return (pointerToPreviousByte + 1) - aDescriptor.Ptr();

    }



// -----------------------------------------------------------------------------

// NumberOfBytesAbleToConvertToJisX0212

// Counts the bytes of be able to convert to JIS

// -----------------------------------------------------------------------------

//

LOCAL_C TInt NumberOfBytesAbleToConvertToJisX0212(const TDesC8& aDescriptor)

    {

    const TUint8* pointerToPreviousByte = aDescriptor.Ptr() - 1;

    const TUint8* const pointerToLastByte =

        pointerToPreviousByte + aDescriptor.Length();

    if (pointerToPreviousByte == pointerToLastByte)

        {

        return 0;

        }

    FOREVER

        {

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers25));

        TUint currentByte = *(pointerToPreviousByte + 1);

        if (currentByte != KSingleShift3)

            {

            break;

            }

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers26));

        if (pointerToLastByte - pointerToPreviousByte < 3)

            {

            break;

            }

        ++pointerToPreviousByte;

        currentByte = *(pointerToPreviousByte + 1);

        if (currentByte < 0xa0)

            {

            return CCnvCharacterSetConverter::EErrorIllFormedInput;

            }

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers27));

        ++pointerToPreviousByte;

        currentByte = *(pointerToPreviousByte + 1);

        if (currentByte < 0xa0)

            {

            return CCnvCharacterSetConverter::EErrorIllFormedInput;

            }

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers28));

        ++pointerToPreviousByte;

        __ASSERT_DEBUG(pointerToPreviousByte <= pointerToLastByte,

            Panic(EPanicBadPointers29));

        if (pointerToPreviousByte >= pointerToLastByte)

            {

            break;

            }

        }

    return (pointerToPreviousByte + 1) - aDescriptor.Ptr();

    }



// -----------------------------------------------------------------------------

// DummyConvertToIntermediateBufferInPlace

//

// -----------------------------------------------------------------------------

//

LOCAL_C void DummyConvertToIntermediateBufferInPlace(TDes8&)

    {

    }



// -----------------------------------------------------------------------------

// ConvertToJisX0208FromEucJpPackedInPlace

// Converts from EUC-JP to JIS code

// -----------------------------------------------------------------------------

//

LOCAL_C void ConvertToJisX0208FromEucJpPackedInPlace(TDes8& aDescriptor)

    {

    const TInt descriptorLength = aDescriptor.Length();

    __ASSERT_DEBUG(descriptorLength > 0, Panic(EPanicNothingToConvert4));

    __ASSERT_DEBUG(descriptorLength % 2 == 0, Panic(EPanicOddNumberOfBytes5));

    TUint8* pointerToCurrentByte = CONST_CAST(TUint8*, aDescriptor.Ptr());

    const TUint8* const pointerToLastByte =

        pointerToCurrentByte + (descriptorLength - 1);

    FOREVER

        {

        const TUint currentByte = *pointerToCurrentByte;

        __ASSERT_DEBUG(currentByte & 0x80, Panic(EPanicBadHighBit4));

        *pointerToCurrentByte = STATIC_CAST(TUint8, currentByte & ~0x80);

        __ASSERT_DEBUG(pointerToCurrentByte <= pointerToLastByte,

            Panic(EPanicBadPointers30));

        if (pointerToCurrentByte >= pointerToLastByte)

            {

            break;

            }

        ++pointerToCurrentByte;

        }

    }



// -----------------------------------------------------------------------------

// ConvertToHalfWidthKatakana8FromEucJpPackedInPlace

// Converts from EUC-JP to half width Katakana

// -----------------------------------------------------------------------------

//

LOCAL_C void ConvertToHalfWidthKatakana8FromEucJpPackedInPlace(

        TDes8& aDescriptor)

    {

    const TInt descriptorLength = aDescriptor.Length();

    __ASSERT_DEBUG(descriptorLength > 0, Panic(EPanicNothingToConvert5));

    __ASSERT_DEBUG(descriptorLength % 2 == 0, Panic(EPanicOddNumberOfBytes6));

    TUint8* pointerToTargetByte = CONST_CAST(TUint8*, aDescriptor.Ptr());

    const TUint8* pointerToSourceByte = pointerToTargetByte;

    const TUint8* const pointerToLastByte = pointerToSourceByte +

        (descriptorLength - 1);

    FOREVER

        {

        __ASSERT_DEBUG(*pointerToSourceByte == KSingleShift2,

            Panic(EPanicSingleShift2Expected));

        __ASSERT_DEBUG(pointerToSourceByte < pointerToLastByte,

            Panic(EPanicBadPointers31));

        ++pointerToSourceByte;

        const TUint sourceByte = *pointerToSourceByte;

        __ASSERT_DEBUG(sourceByte & 0x80, Panic(EPanicBadHighBit5));

        *pointerToTargetByte = STATIC_CAST(TUint8, sourceByte);

        __ASSERT_DEBUG(pointerToSourceByte <= pointerToLastByte,

            Panic(EPanicBadPointers32));

        if (pointerToSourceByte >= pointerToLastByte)

            {

            break;

            }

        ++pointerToSourceByte;

        ++pointerToTargetByte;

        }

    aDescriptor.SetLength(descriptorLength / 2);

    }



// -----------------------------------------------------------------------------

// ConvertToJisX0212FromEucJpPackedInPlace

// Converts from EUC-JP to JIS

// -----------------------------------------------------------------------------

//

LOCAL_C void ConvertToJisX0212FromEucJpPackedInPlace(TDes8& aDescriptor)

    {

    const TInt descriptorLength = aDescriptor.Length();

    __ASSERT_DEBUG(descriptorLength > 0, Panic(EPanicNothingToConvert6));

    __ASSERT_DEBUG(descriptorLength % 3 == 0,

        Panic(EPanicNumberOfBytesIsNotMultipleOfThree2));

    TUint8* pointerToTargetByte = CONST_CAST(TUint8*, aDescriptor.Ptr());

    const TUint8* pointerToSourceByte = pointerToTargetByte;

    const TUint8* const pointerToLastByte =

        pointerToSourceByte + (descriptorLength - 1);

    FOREVER

        {

        __ASSERT_DEBUG(*pointerToSourceByte == KSingleShift3,

            Panic(EPanicSingleShift3Expected));

        __ASSERT_DEBUG(pointerToSourceByte < pointerToLastByte,

            Panic(EPanicBadPointers33));

        ++pointerToSourceByte;

        TUint sourceByte = *pointerToSourceByte;

        __ASSERT_DEBUG(sourceByte & 0x80, Panic(EPanicBadHighBit6));

        *pointerToTargetByte = STATIC_CAST(TUint8, sourceByte & ~0x80);

        __ASSERT_DEBUG(pointerToSourceByte<pointerToLastByte,

            Panic(EPanicBadPointers34));

        ++pointerToSourceByte;

        sourceByte = *pointerToSourceByte;

        __ASSERT_DEBUG(sourceByte&0x80, Panic(EPanicBadHighBit7));

        __ASSERT_DEBUG(pointerToTargetByte < pointerToLastByte,

            Panic(EPanicBadPointers35));

        ++pointerToTargetByte;

        *pointerToTargetByte = STATIC_CAST(TUint8, sourceByte & ~0x80);

        __ASSERT_DEBUG(pointerToSourceByte <= pointerToLastByte,

            Panic(EPanicBadPointers36));

        if (pointerToSourceByte >= pointerToLastByte)

            {

            break;

            }

        ++pointerToSourceByte;

        ++pointerToTargetByte;

        }

    aDescriptor.SetLength((descriptorLength / 3) * 2);

    }



// -----------------------------------------------------------------------------

// NumberOfBytesAbleToConvertToPictograph

//

// -----------------------------------------------------------------------------

//

LOCAL_C TInt NumberOfBytesAbleToConvertToPictograph1(const TDesC8& aDescriptor)

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

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers14));

        TUint currentByte = *(pointerToPreviousByte + 1);

        const TUint nextByte = *(pointerToPreviousByte + 2);

        if (((currentByte < KFirstByteRangeFirstBlockStart) ||

             (currentByte > KFirstByteRangeFirstBlockEnd)) ||

            ((nextByte < KSecondByteRangeSecondBlockStart) ||

             (nextByte > KSecondByteRangeSecondBlockEnd)))

            {

            break;

            }

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers15));

        pointerToPreviousByte += 2;

        }

    return (pointerToPreviousByte + 1) - aDescriptor.Ptr();

    }



// -----------------------------------------------------------------------------

// NumberOfBytesAbleToConvertToPictograph

//

// -----------------------------------------------------------------------------

//

LOCAL_C TInt NumberOfBytesAbleToConvertToPictograph2(const TDesC8& aDescriptor)

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

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers14));

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

        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte,

            Panic(EPanicBadPointers15));

        pointerToPreviousByte += 2;

        }

    return (pointerToPreviousByte + 1) - aDescriptor.Ptr();

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

    __ASSERT_DEBUG(descriptorLength > aStartPositionInDescriptor,

        Panic(EPanicNothingToConvert3));

    __ASSERT_DEBUG((descriptorLength - aStartPositionInDescriptor) % 2 == 0,

        Panic(EPanicOddNumberOfBytes3));

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

    __ASSERT_DEBUG(descriptorLength >= aStartPositionInDescriptor,

        Panic(EPanicBadCalculation2));

    if (descriptorLength <= aStartPositionInDescriptor)

        {

        aDescriptor.SetLength(descriptorLength);

        }

    else

        {

        __ASSERT_DEBUG((descriptorLength - aStartPositionInDescriptor) % 2 == 0,

            Panic(EPanicOddNumberOfBytes4));

        // pointerToTargetByte is initialized properly when descriptorLength

        // has been offset to the actual final length of aDescriptor

        TUint8* pointerToTargetByte = CONST_CAST(TUint8*, aDescriptor.Ptr());

        const TUint8* const pointerToFirstByte =

            pointerToTargetByte + aStartPositionInDescriptor;

        const TUint8* pointerToSourceByte =

            pointerToTargetByte + (descriptorLength - 1);

        descriptorLength = (((descriptorLength - aStartPositionInDescriptor)

            * bytesPerCharacter) / 2) + aStartPositionInDescriptor;

        __ASSERT_DEBUG((descriptorLength - aStartPositionInDescriptor) %

            bytesPerCharacter == 0, Panic(EPanicNumberOfBytesIsNotMultipleOfThree1));

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

            __ASSERT_DEBUG(pointerToTargetByte >= pointerToFirstByte,

                Panic(EPanicBadPointers10));

            --pointerToTargetByte;

            pointerToSourceByte -= 2;

            }

        }

    }



// -----------------------------------------------------------------------------

// ConvertToPictographFromEucJpPackedInPlace1

// Converts from EucJp packed Pictograph to Unicode

// -----------------------------------------------------------------------------

//

LOCAL_C void ConvertToPictographFromEucJpPackedInPlace(TDes8& aDescriptor)

    {

    const TInt descriptorLength = aDescriptor.Length();

    __ASSERT_DEBUG(descriptorLength > 0, Panic(EPanicNothingToConvert6));

    TUint8* pointerToTargetByte = CONST_CAST(TUint8*, aDescriptor.Ptr());

    const TUint8* pointerToSourceByte = pointerToTargetByte;

    const TUint8* const pointerToLastByte =

        pointerToSourceByte + (descriptorLength - 1);

    TInt bytesPerCharacter = 2;

    TInt sjisStartbyte = KPictographStartFirstByte;

    if (*pointerToSourceByte == KSingleShift3)

        {

        ++pointerToSourceByte;

        bytesPerCharacter = 3;

        sjisStartbyte = KEUCJPSecondBlockStart;

        }

    for (;pointerToSourceByte < pointerToLastByte; )

        {

        __ASSERT_DEBUG(pointerToSourceByte < pointerToLastByte,

            Panic(EPanicBadPointers33));

        TUint firstByte = (*pointerToSourceByte

            - KFirstByteRangeFirstBlockStart) / 2 + sjisStartbyte;

        TUint secondByte = *(pointerToSourceByte + 1);

        if ((*pointerToSourceByte % 2) == 0)

            {

            secondByte += KPictographStartSecondByte -

                KSecondByteRangeSecondBlockStart + KEUCJPBlockSize + 1;

            if (secondByte >= KShiftJisTrailByteIllegal)

                secondByte++;

            }

        else

            {

            secondByte += KPictographStartSecondByte - KSecondByteRangeSecondBlockStart;

            if (secondByte >= KShiftJisTrailByteIllegal)

                secondByte++;

            }

        *pointerToTargetByte = static_cast<TUint8>(firstByte);



        __ASSERT_DEBUG(pointerToTargetByte < pointerToLastByte,

            Panic(EPanicBadPointers35));

        ++pointerToTargetByte;

        *pointerToTargetByte = static_cast<TUint8>(secondByte);

        __ASSERT_DEBUG(pointerToSourceByte <= pointerToLastByte,

            Panic(EPanicBadPointers36));

        pointerToSourceByte += 2;

        ++pointerToTargetByte;

        }

    aDescriptor.SetLength((descriptorLength / bytesPerCharacter) * 2);

    }



// New Interface class

class CEucJpDirectmapImplementation : public CCharacterSetConverterPluginInterface

{

    public:

        virtual const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();



        virtual TInt ConvertFromUnicode(

            CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,

            const TDesC8& aReplacementForUnconvertibleUnicodeCharacters,

            TDes8& aForeign,

            const TDesC16& aUnicode,

            CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters );



        virtual TInt ConvertToUnicode(

            CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,

            TDes16& aUnicode,

            const TDesC8& aForeign,

            TInt&,

            TInt& aNumberOfUnconvertibleCharacters,

            TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter );



        virtual TBool IsInThisCharacterSetL(

            TBool& aSetToTrue,

            TInt& aConfidenceLevel,

            const TDesC8& );



        static CEucJpDirectmapImplementation* NewL();



        virtual ~CEucJpDirectmapImplementation();

    private:

        CEucJpDirectmapImplementation();

};



// -----------------------------------------------------------------------------

// ReplacementForUnconvertibleUnicodeCharacters

// Retruns the character to replacement for unconvertible unicode character.

// Returns: '?':

// -----------------------------------------------------------------------------

//

const TDesC8& CEucJpDirectmapImplementation::ReplacementForUnconvertibleUnicodeCharacters()

    {

    return KLit8EucJpPackedReplacementForUnconvertibleUnicodeCharacters;

    }



// -----------------------------------------------------------------------------

// ConvertFromUnicode

// Converts from Unicode to EUC-JP

// Returns: The number of unconverted characters

// -----------------------------------------------------------------------------

//

TInt CEucJpDirectmapImplementation::ConvertFromUnicode(

        CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,

        const TDesC8& aReplacementForUnconvertibleUnicodeCharacters,

        TDes8& aForeign, const TDesC16& aUnicode,

        CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)

    {

    TFixedArray<CnvUtilities::SCharacterSet, 6> characterSets;

    characterSets[0].iConversionData = &CnvJisRoman::ConversionData();

    characterSets[0].iConvertFromIntermediateBufferInPlace =

        DummyConvertFromIntermediateBufferInPlace;

    characterSets[0].iEscapeSequence = &KNullDesC8;

    characterSets[1].iConversionData = &CnvJisX0208::ConversionData();

    characterSets[1].iConvertFromIntermediateBufferInPlace =

        ConvertFromJisX0208ToEucJpPackedInPlace;

    characterSets[1].iEscapeSequence = &KNullDesC8;

    characterSets[2].iConversionData = &CnvHalfWidthKatakana8::ConversionData();

    characterSets[2].iConvertFromIntermediateBufferInPlace =

        ConvertFromHalfWidthKatakana8ToEucJpPackedInPlace;

    characterSets[2].iEscapeSequence = &KNullDesC8;

    characterSets[3].iConversionData = &CnvJisX0212::ConversionData();

    characterSets[3].iConvertFromIntermediateBufferInPlace =

        ConvertFromJisX0212ToEucJpPackedInPlace;

    characterSets[3].iEscapeSequence = &KNullDesC8;

    characterSets[4].iConversionData = &CnvShiftJisDirectmap::ConversionData();

    characterSets[4].iConvertFromIntermediateBufferInPlace =

        ConvertFromPictogaphToEucJpDirectmapInPlace;

    characterSets[4].iEscapeSequence = &KNullDesC8;

    characterSets[5].iConversionData = &CnvShiftJisDirectmap::ConversionData();

    characterSets[5].iConvertFromIntermediateBufferInPlace =

        ConvertFromPictogaphToEucJpDirectmapInPlace;

    characterSets[5].iEscapeSequence = &KNullDesC8;



    TInt unconvert = CnvUtilities::ConvertFromUnicode(

        aDefaultEndiannessOfForeignCharacters,

        aReplacementForUnconvertibleUnicodeCharacters,

        aForeign, aUnicode, aIndicesOfUnconvertibleCharacters,

        characterSets.Array());



    return unconvert;

    }



// -----------------------------------------------------------------------------

// ConvertToUnicode

// Converts from EUC-JP to Unicode.

// Returns: The number of unconverted bytes

// -----------------------------------------------------------------------------

//

TInt CEucJpDirectmapImplementation::ConvertToUnicode(

        CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,

        TDes16& aUnicode, const TDesC8& aForeign, TInt&,

        TInt& aNumberOfUnconvertibleCharacters,

        TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)

    {

    TFixedArray<CnvUtilities::SMethod, 6> methods;



    methods[0].iNumberOfBytesAbleToConvert =

        NumberOfBytesAbleToConvertToPictograph1;

    methods[0].iConvertToIntermediateBufferInPlace =

        ConvertToPictographFromEucJpPackedInPlace;

    methods[0].iConversionData = &CnvShiftJisDirectmap::ConversionData();

    methods[0].iNumberOfBytesPerCharacter = 2;

    methods[0].iNumberOfCoreBytesPerCharacter = 2;

    methods[1].iNumberOfBytesAbleToConvert =

        NumberOfBytesAbleToConvertToPictograph2;

    methods[1].iConvertToIntermediateBufferInPlace =

        ConvertToPictographFromEucJpPackedInPlace;

    methods[1].iConversionData = &CnvShiftJisDirectmap::ConversionData();

    methods[1].iNumberOfBytesPerCharacter = 3;

    methods[1].iNumberOfCoreBytesPerCharacter = 2;



    methods[2].iNumberOfBytesAbleToConvert = NumberOfBytesAbleToConvertToJisRoman;

    methods[2].iConvertToIntermediateBufferInPlace =

        DummyConvertToIntermediateBufferInPlace;

    methods[2].iConversionData = &CnvJisRoman::ConversionData();

    methods[2].iNumberOfBytesPerCharacter = 1;

    methods[2].iNumberOfCoreBytesPerCharacter = 1;

    methods[3].iNumberOfBytesAbleToConvert = NumberOfBytesAbleToConvertToJisX0208;

    methods[3].iConvertToIntermediateBufferInPlace =

        ConvertToJisX0208FromEucJpPackedInPlace;

    methods[3].iConversionData = &CnvJisX0208::ConversionData();

    methods[3].iNumberOfBytesPerCharacter = 2;

    methods[3].iNumberOfCoreBytesPerCharacter = 2;

    methods[4].iNumberOfBytesAbleToConvert =

        NumberOfBytesAbleToConvertToHalfWidthKatakana8;

    methods[4].iConvertToIntermediateBufferInPlace =

        ConvertToHalfWidthKatakana8FromEucJpPackedInPlace;

    methods[4].iConversionData = &CnvHalfWidthKatakana8::ConversionData();

    methods[4].iNumberOfBytesPerCharacter = 2;

    methods[4].iNumberOfCoreBytesPerCharacter = 1;

    methods[5].iNumberOfBytesAbleToConvert =

        NumberOfBytesAbleToConvertToJisX0212;

    methods[5].iConvertToIntermediateBufferInPlace =

        ConvertToJisX0212FromEucJpPackedInPlace;

    methods[5].iConversionData = &CnvJisX0212::ConversionData();

    methods[5].iNumberOfBytesPerCharacter = 3;

    methods[5].iNumberOfCoreBytesPerCharacter = 2;



    TInt unconvert = CnvUtilities::ConvertToUnicodeFromHeterogeneousForeign(

        aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign,

        aNumberOfUnconvertibleCharacters,

        aIndexOfFirstByteOfFirstUnconvertibleCharacter, methods.Array());



    // The following is specific impelementation for brower.

    // If brower app calls this API, the yen sign code(0xA5)

    // must be converted to backslash code(0x5C).

    // Becasue Javascript supports backslash code ony.

    TBool browserProcess = (RProcess().SecureId().iId == KBrowserSecureId);

    if (browserProcess && aUnicode.Length() > 0)

        {

        const TUint16* pB = aUnicode.Ptr();

        const TUint16* pbase = pB;

        const TUint16* pE = pB + aUnicode.Length() -1;

        while (pE>=pbase)

            {

            if (*pbase == KCharacterCodeForYenSign)

                {

                aUnicode[pbase - pB] = KCharacterCodeForBackSlash;

                }

            pbase++;

            }

        }



    return unconvert;

    }



// -----------------------------------------------------------------------------

// IsInThisCharacterSetL

// Detects whether the text is the character code or not.

// Returns: ETrue:

// -----------------------------------------------------------------------------

//

TBool CEucJpDirectmapImplementation::IsInThisCharacterSetL(TBool& /*aSetToTrue*/, TInt& /*aConfidenceLevel*/,

        const TDesC8& /*aSample*/)

    {

    return EFalse;

    }



CEucJpDirectmapImplementation* CEucJpDirectmapImplementation::NewL()

    {

    CEucJpDirectmapImplementation* self = new(ELeave) CEucJpDirectmapImplementation;

    return self;

    }



CEucJpDirectmapImplementation::CEucJpDirectmapImplementation()

    {

    //default constructor.. do nothing

    }



CEucJpDirectmapImplementation::~CEucJpDirectmapImplementation()

    {

    //default destructor .. do nothing

    }



// ECOM CREATION FUNCTION

const TImplementationProxy ImplementationTable[] =

    {

    // Note: This is the same UID as defined in old mmp-file

    // Used also in 12221212.rss ( implementation_uid )

    IMPLEMENTATION_PROXY_ENTRY( 0x101F86A6, CEucJpDirectmapImplementation::NewL )

    };



EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )

    {

    aTableCount = sizeof( ImplementationTable ) / sizeof(TImplementationProxy);

    return ImplementationTable;

    }



// End of file

