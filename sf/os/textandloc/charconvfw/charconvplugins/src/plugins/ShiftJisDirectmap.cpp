/*
* Copyright (c) 2022 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     This module is a plug-in module for Shift-JIS with Pictograph.
*                Basicaly, Vodafone Pictograph is encoded by ISO2022,
*                but Japanese FEP needs a pictograph as one character code
*                in Shift-JIS character code set.
*
*/






// INCLUDE FILES
#include "CnvShiftJisDirectmap.h"
#include <e32std.h>
#include <charconv.h>
#include "jisx0201.h"
#include "jisx0208.h"
#include <convutils.h>

#ifdef EKA2
#include <convgeneratedcpp.h>
#include <ecom/implementationproxy.h>
#include "charactersetconverter.h"
#endif // EKA2

const TUint KSingleByteRangeFirstBlockEnd = 0x7f;
const TUint KSingleByteRangeSecondBlockStart = 0xa1;
const TUint KSingleByteRangeSecondBlockEnd = 0xdf;
const TUint KFirstByteRangeFirstBlockStart = 0x81;
const TUint KFirstByteRangeFirstBlockEnd = 0x9f;
const TUint KFirstByteRangeFirstBlockLength = (KFirstByteRangeFirstBlockEnd+1)-KFirstByteRangeFirstBlockStart;
const TUint KFirstByteRangeSecondBlockStart = 0xe0;
const TUint KFirstByteRangeSecondBlockEnd = 0xfc;
const TUint KSecondByteRangeFirstBlockStart = 0x40;
const TUint KSecondByteRangeFirstBlockEnd = 0x7e;
const TUint KSecondByteRangeFirstBlockLength = (KSecondByteRangeFirstBlockEnd+1)-KSecondByteRangeFirstBlockStart;
const TUint KSecondByteRangeSecondBlockStart = 0x80;
const TUint KSecondByteRangeSecondBlockEnd = 0xfc;

const TUint KPictoFirstByteStart = 0xF0;
const TUint KPictoFirstByteEnd = 0xF9;
const TUint KPictoSecondByteStart = 0x40;
//const TUint KPictoSecondByteEnd = 0xFB;
const TUint KPictoSecondByteEnd = 0xFC;

// SecureID for Brower app
const TUint32 KBrowserSecureId = 0x10008D39;
// Define for converting from YenSign to BackSlash
const TUint KCharacterCodeForYenSign = 0x00A5;
const TUint KCharacterCodeForBackSlash = 0x005C;

#ifdef _DEBUG

_LIT(KLitPanicText, "SHIFTJIS_FORFEP");

enum TPanic
    {
    EPanicIndexOverflow1=1,
    EPanicIndexOverflow2,
    EPanicNothingToConvert1,
    EPanicNothingToConvert2,
    EPanicOddNumberOfBytes1,
    EPanicOddNumberOfBytes2,
    EPanicBadPointers1,
    EPanicBadPointers2,
    EPanicBadPointers3,
    EPanicBadPointers4,
    EPanicBadPointers5,
    EPanicBadPointers6,
    EPanicBadPointers7,
    EPanicBadPointers8,
    EPanicBadPointers9
    };

// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// Panic ?description.
// ?description
// -----------------------------------------------------------------------------
//
LOCAL_C void Panic(TPanic aPanic)
    {
    User::Panic(KLitPanicText, aPanic);
    }
#endif

// -----------------------------------------------------------------------------
// DummyConvertFromIntermediateBufferInPlace ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
void DummyConvertFromIntermediateBufferInPlace(TInt, TDes8&, TInt& aNumberOfCharactersThatDroppedOut)
    {
    aNumberOfCharactersThatDroppedOut = 0;
    }

// -----------------------------------------------------------------------------
// ConvertFromJisX0208ToShiftJisInPlace ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
void ConvertFromJisX0208ToShiftJisInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
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
        TUint firstByte=*pointerToCurrentByte-0x21;
        TUint secondByte=*(pointerToCurrentByte+1)-0x21;
        if (firstByte%2!=0)
            {
            secondByte+=94;
            }
        firstByte/=2;
        if (firstByte<KFirstByteRangeFirstBlockLength)
            {
            firstByte+=KFirstByteRangeFirstBlockStart;
            }
        else
            {
            firstByte+=KFirstByteRangeSecondBlockStart-KFirstByteRangeFirstBlockLength;
            }
        if (secondByte<KSecondByteRangeFirstBlockLength)
            {
            secondByte+=KSecondByteRangeFirstBlockStart;
            }
        else
            {
            secondByte+=KSecondByteRangeSecondBlockStart-KSecondByteRangeFirstBlockLength;
            }
        *pointerToCurrentByte=STATIC_CAST(TUint8, firstByte);
        ++pointerToCurrentByte;
        *pointerToCurrentByte=STATIC_CAST(TUint8, secondByte);
        __ASSERT_DEBUG(pointerToCurrentByte<=pointerToLastByte, Panic(EPanicBadPointers1));
        if (pointerToCurrentByte>=pointerToLastByte)
            {
            break;
            }
        ++pointerToCurrentByte;
        }
    }

// -----------------------------------------------------------------------------
// NumberOfBytesAbleToConvertToJisX0201 ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
TInt NumberOfBytesAbleToConvertToJisX0201(const TDesC8& aDescriptor)
    {
    const TUint8* pointerToPreviousByte=aDescriptor.Ptr()-1;
    const TUint8* const pointerToLastByte=pointerToPreviousByte+aDescriptor.Length();
    if (pointerToPreviousByte==pointerToLastByte)
        {
        return 0;
        }
    FOREVER
        {
        __ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers2));
        const TUint currentByte = *(pointerToPreviousByte+1);
        if (((currentByte > KSingleByteRangeFirstBlockEnd)
             && (currentByte < KSingleByteRangeSecondBlockStart)) ||
            (currentByte > KSingleByteRangeSecondBlockEnd))
            {
            break;
            }
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

// -----------------------------------------------------------------------------
// NumberOfBytesAbleToConvertToJisX0208 ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
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
        __ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers5));
        TUint currentByte=*(pointerToPreviousByte+1);
        if ((currentByte<KFirstByteRangeFirstBlockStart) ||
            ((currentByte>KFirstByteRangeFirstBlockEnd) && (currentByte<KFirstByteRangeSecondBlockStart)) ||
            (currentByte>KFirstByteRangeSecondBlockEnd) ||
            ((currentByte >= KPictoFirstByteStart) && (currentByte <= KPictoFirstByteEnd))
           )
            {
            break;
            }

        __ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers6));
        if (pointerToPreviousByte+1>=pointerToLastByte)
            {
            break;
            }
        __ASSERT_DEBUG(pointerToPreviousByte+2<=pointerToLastByte, Panic(EPanicBadPointers7));
        currentByte=*(pointerToPreviousByte+2);
        if ((currentByte<KSecondByteRangeFirstBlockStart) ||
            ((currentByte>KSecondByteRangeFirstBlockEnd) && (currentByte<KSecondByteRangeSecondBlockStart)) ||
            (currentByte>KSecondByteRangeSecondBlockEnd))
            {
            break;
            }
        pointerToPreviousByte+=2;
        __ASSERT_DEBUG(pointerToPreviousByte<=pointerToLastByte, Panic(EPanicBadPointers8));
        if (pointerToPreviousByte>=pointerToLastByte)
            {
            break;
            }
        }
    return (pointerToPreviousByte+1)-aDescriptor.Ptr();
    }

// -----------------------------------------------------------------------------
// NumberOfBytesAbleToConvertToPictograph ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
TInt NumberOfBytesAbleToConvertToPictograph(const TDesC8& aDescriptor)
    {
    const TUint8* pointerToPreviousByte = aDescriptor.Ptr() - 1;
    const TUint8* const pointerToLastByte = pointerToPreviousByte + aDescriptor.Length();

    for (; pointerToPreviousByte < pointerToLastByte;)
        {
        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte, Panic(EPanicBadPointers5));
        TUint currentByte = *(pointerToPreviousByte + 1);
        if ((currentByte < KPictoFirstByteStart) || (currentByte > KPictoFirstByteEnd))
            {
            break;
            }
        __ASSERT_DEBUG(pointerToPreviousByte < pointerToLastByte, Panic(EPanicBadPointers6));
        if (pointerToPreviousByte + 1 >= pointerToLastByte)
            {
            break;
            }
        __ASSERT_DEBUG(pointerToPreviousByte + 2 <= pointerToLastByte, Panic(EPanicBadPointers7));
        currentByte = *(pointerToPreviousByte + 2);
        if ((currentByte < KPictoSecondByteStart) || (currentByte> KPictoSecondByteEnd))
            {
            break;
            }
        pointerToPreviousByte += 2;
        __ASSERT_DEBUG(pointerToPreviousByte <= pointerToLastByte, Panic(EPanicBadPointers8));
        if (pointerToPreviousByte >= pointerToLastByte)
            {
            break;
            }
        }
    return (pointerToPreviousByte + 1)-aDescriptor.Ptr();
    }

// -----------------------------------------------------------------------------
// DummyConvertToIntermediateBufferInPlace ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
void DummyConvertToIntermediateBufferInPlace(TDes8&)
    {
    }

// -----------------------------------------------------------------------------
// ConvertToJisX0208FromShiftJisInPlace ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
void ConvertToJisX0208FromShiftJisInPlace(TDes8& aDescriptor)
    {
    const TInt descriptorLength=aDescriptor.Length();
    __ASSERT_DEBUG(descriptorLength>0, Panic(EPanicNothingToConvert2));
    __ASSERT_DEBUG(descriptorLength%2==0, Panic(EPanicOddNumberOfBytes2));
    TUint8* pointerToCurrentByte=CONST_CAST(TUint8*, aDescriptor.Ptr());
    const TUint8* const pointerToLastByte=pointerToCurrentByte+(descriptorLength-1);
    FOREVER
        {
        TUint firstByte=*pointerToCurrentByte;
        TUint secondByte=*(pointerToCurrentByte+1);
        if (firstByte<KFirstByteRangeSecondBlockStart)
            {
            firstByte-=KFirstByteRangeFirstBlockStart;
            }
        else
            {
            firstByte-=KFirstByteRangeSecondBlockStart-KFirstByteRangeFirstBlockLength;
            }
        if (secondByte<KSecondByteRangeSecondBlockStart)
            {
            secondByte-=KSecondByteRangeFirstBlockStart;
            }
        else
            {
            secondByte-=KSecondByteRangeSecondBlockStart-KSecondByteRangeFirstBlockLength;
            }
        firstByte*=2;
        if (secondByte>=94)
            {
            ++firstByte;
            secondByte-=94;
            }
        firstByte+=0x21;
        secondByte+=0x21;
        *pointerToCurrentByte=STATIC_CAST(TUint8, firstByte);
        ++pointerToCurrentByte;
        *pointerToCurrentByte=STATIC_CAST(TUint8, secondByte);
        __ASSERT_DEBUG(pointerToCurrentByte<=pointerToLastByte, Panic(EPanicBadPointers9));
        if (pointerToCurrentByte>=pointerToLastByte)
            {
            break;
            }
        ++pointerToCurrentByte;
        }
    }

// New Interface class
class ShiftJisDirectmapImplementation : public CCharacterSetConverterPluginInterface
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

        static ShiftJisDirectmapImplementation* NewL();

        virtual ~ShiftJisDirectmapImplementation();
    private:
        ShiftJisDirectmapImplementation();
};

// -----------------------------------------------------------------------------
// ReplacementForUnconvertibleUnicodeCharacters returns the character which
// which is used by default as the replacement for unconvertible Unicode
// characters.
// Returns: a character
// -----------------------------------------------------------------------------
//
_LIT8(KLit8ShiftJisReplacementForUnconvertibleUnicodeCharacters, "\x81\x48");
const TDesC8& ShiftJisDirectmapImplementation::ReplacementForUnconvertibleUnicodeCharacters()
    {
    return KLit8ShiftJisReplacementForUnconvertibleUnicodeCharacters;//CnvShiftJis::ReplacementForUnconvertibleUnicodeCharacters();
    }

// -----------------------------------------------------------------------------
// ConvertFromUnicode converts from an Unicode string to a Shift-Jis string
// with Pictograph.
// Returns: The number of unconverted characters left at the end of the input
//          descriptor
// -----------------------------------------------------------------------------
//
TInt ShiftJisDirectmapImplementation::ConvertFromUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,
    const TDesC8& aReplacementForUnconvertibleUnicodeCharacters,
    TDes8& aForeign, const TDesC16& aUnicode,
    CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
    {
    TFixedArray<CnvUtilities::SCharacterSet, 3> arrayOfCoreCharacterSets;
    arrayOfCoreCharacterSets[0].iConversionData = &CnvJisX0201::ConversionData();
    arrayOfCoreCharacterSets[0].iConvertFromIntermediateBufferInPlace =
        DummyConvertFromIntermediateBufferInPlace;
    arrayOfCoreCharacterSets[0].iEscapeSequence = &KNullDesC8;
    arrayOfCoreCharacterSets[1].iConversionData = &CnvJisX0208::ConversionData();
    arrayOfCoreCharacterSets[1].iConvertFromIntermediateBufferInPlace =
        ConvertFromJisX0208ToShiftJisInPlace;
    arrayOfCoreCharacterSets[1].iEscapeSequence = &KNullDesC8;
    arrayOfCoreCharacterSets[2].iConversionData = &CnvShiftJisDirectmap::ConversionData();
    arrayOfCoreCharacterSets[2].iConvertFromIntermediateBufferInPlace =
        DummyConvertFromIntermediateBufferInPlace;
    arrayOfCoreCharacterSets[2].iEscapeSequence = &KNullDesC8;

    return CnvUtilities::ConvertFromUnicode(aDefaultEndiannessOfForeignCharacters,
        aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode,
        aIndicesOfUnconvertibleCharacters, arrayOfCoreCharacterSets.Array());
    }

// -----------------------------------------------------------------------------
// ConvertToUnicode converts from a Shift-Jis string with Pictograph to
// an Unicode string .
// Returns: The number of unconverted bytes left at the end of the input
//          descriptor
// -----------------------------------------------------------------------------
//
TInt ShiftJisDirectmapImplementation::ConvertToUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,
    TDes16& aUnicode, const TDesC8& aForeign, TInt&,
    TInt& aNumberOfUnconvertibleCharacters,
    TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
    {
    TFixedArray<CnvUtilities::SMethod, 3> arrayOfCoreMethods;
    arrayOfCoreMethods[0].iNumberOfBytesAbleToConvert =
        NumberOfBytesAbleToConvertToJisX0201;
    arrayOfCoreMethods[0].iConvertToIntermediateBufferInPlace =
        DummyConvertToIntermediateBufferInPlace;
    arrayOfCoreMethods[0].iConversionData = &CnvJisX0201::ConversionData();
    arrayOfCoreMethods[0].iNumberOfBytesPerCharacter = 1;
    arrayOfCoreMethods[0].iNumberOfCoreBytesPerCharacter = 1;
    arrayOfCoreMethods[1].iNumberOfBytesAbleToConvert =
        NumberOfBytesAbleToConvertToPictograph;
    arrayOfCoreMethods[1].iConvertToIntermediateBufferInPlace =
        DummyConvertToIntermediateBufferInPlace;
    arrayOfCoreMethods[1].iConversionData = &CnvShiftJisDirectmap::ConversionData();
    arrayOfCoreMethods[1].iNumberOfBytesPerCharacter = 2;
    arrayOfCoreMethods[1].iNumberOfCoreBytesPerCharacter = 2;
    arrayOfCoreMethods[2].iNumberOfBytesAbleToConvert =
        NumberOfBytesAbleToConvertToJisX0208;
    arrayOfCoreMethods[2].iConvertToIntermediateBufferInPlace =
        ConvertToJisX0208FromShiftJisInPlace;
    arrayOfCoreMethods[2].iConversionData = &CnvJisX0208::ConversionData();
    arrayOfCoreMethods[2].iNumberOfBytesPerCharacter = 2;
    arrayOfCoreMethods[2].iNumberOfCoreBytesPerCharacter = 2;

    TInt unconvert = CnvUtilities::ConvertToUnicodeFromHeterogeneousForeign(
                        aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign,
                        aNumberOfUnconvertibleCharacters,
                        aIndexOfFirstByteOfFirstUnconvertibleCharacter,
                        arrayOfCoreMethods.Array());

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
// IsInThisCharacterSetL tests whether the aSample is Shift-JIS or not.
// But, This .cpl is only used for FEP.
// So, it's not need to correspond auto code detection.
// Returns: EFalse: ?description
// -----------------------------------------------------------------------------
//
TBool ShiftJisDirectmapImplementation::IsInThisCharacterSetL(TBool& /*aSetToTrue*/, TInt& /*aConfidenceLevel*/,
    const TDesC8& /*aSample*/)
    {
    return EFalse;
    }

ShiftJisDirectmapImplementation* ShiftJisDirectmapImplementation::NewL()
    {
    ShiftJisDirectmapImplementation* self = new(ELeave) ShiftJisDirectmapImplementation;
    return self;
    }

ShiftJisDirectmapImplementation::ShiftJisDirectmapImplementation()
    {
    //default constructor.. do nothing
    }

ShiftJisDirectmapImplementation::~ShiftJisDirectmapImplementation()
    {
    //default destructor .. do nothing
    }

// ECOM CREATION FUNCTION
const TImplementationProxy ImplementationTable[] =
    {
    // Note: This is the same UID as defined in old mmp-file
    // Used also in 12221212.rss ( implementation_uid )
    IMPLEMENTATION_PROXY_ENTRY( 0x101F8691, ShiftJisDirectmapImplementation::NewL )
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

//  End of File
