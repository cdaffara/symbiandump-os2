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


#include "PictographObserver.h"
#include "featmgr/featmgr.h"  

#include <e32std.h>
#include <charconv.h>
#include <convutils.h>
#include "jisx0201.h"
#include "jisx0208.h"
#include "SHIFTJIS_2.H"
#include <convdata.h>

//const TUint KSingleByteRangeFirstBlockStart=0x00;
const TUint KSingleByteRangeFirstBlockEnd=0x7f;
const TUint KSingleByteRangeSecondBlockStart=0xa1;
const TUint KSingleByteRangeSecondBlockEnd=0xdf;
const TUint KFirstByteRangeFirstBlockStart=0x81;
const TUint KFirstByteRangeFirstBlockEnd=0x9f;
const TUint KFirstByteRangeFirstBlockLength=(KFirstByteRangeFirstBlockEnd+1)-KFirstByteRangeFirstBlockStart;
const TUint KFirstByteRangeSecondBlockStart=0xe0;
// Ken Lunde's book "CJKV Information Processing" (page 176) says this
// constant should be 0xef, but no Shift-JIS characters in
// \charconv\data\JISX0208.TXT have a first byte greater than 0xea, and
// some extensions of Shift-JIS may use the 0xeb-0xef range for the first
// byte of their characters (e.g. "MOPERA"), and in order for users of this
// DLL to be able to implement these extensions using the
// aArrayOfAdditionalXxxxx parameters, this constant therefore needs to
// be set as low as possible, i.e. to 0xea
const TUint KFirstByteRangeSecondBlockEnd=0xea;
const TUint KFirstByteRangeOtherBlockEnd = 0xfc;

const TUint KPictoFirstByteStart = 0xF0;
const TUint KPictoFirstByteEnd = 0xF9;

const TUint KSecondByteRangeFirstBlockStart=0x40;
const TUint KSecondByteRangeFirstBlockEnd=0x7e;
const TUint KSecondByteRangeFirstBlockLength=(KSecondByteRangeFirstBlockEnd+1)-KSecondByteRangeFirstBlockStart;
const TUint KSecondByteRangeSecondBlockStart=0x80;
const TUint KSecondByteRangeSecondBlockEnd=0xfc;

// SecureID for Brower app
const TUint32 KBrowserSecureId = 0x10008D39;
// Define for converting from YenSign to BackSlash
const TUint KCharacterCodeForYenSign = 0x00A5;
const TUint KCharacterCodeForBackSlash = 0x005C;

TInt BytesInOtherDoubleByteAreas(const TDesC8& aShiftJis);
TInt OneIfNotValid(const TDesC8& aShiftJis);

TBool IsJisX0208LeadByte(TUint a)
    {
    return (KFirstByteRangeFirstBlockStart <= a
        && a <= KFirstByteRangeFirstBlockEnd)
        || (KFirstByteRangeSecondBlockStart <= a
        && a <= KFirstByteRangeSecondBlockEnd);
    }

TBool IsValidTrailByte(TUint a)
    {
    return KSecondByteRangeFirstBlockStart <= a
        && a <= KSecondByteRangeSecondBlockEnd
        && a != KSecondByteRangeFirstBlockEnd + 1;
    }

TBool IsOtherLeadByte(TUint a)
    {
    if ( FeatureManager::FeatureSupported(KFeatureIdJapanesePicto) )
        {        
        return ((KFirstByteRangeSecondBlockEnd < a && a <= KFirstByteRangeOtherBlockEnd)
                && (a < KPictoFirstByteStart || a > KPictoFirstByteEnd));
        }
    else
        {        
        return KFirstByteRangeSecondBlockEnd < a
                && a <= KFirstByteRangeOtherBlockEnd;
        }
    }

_LIT8(KLit8ShiftJisReplacementForUnconvertibleUnicodeCharacters, "\x81\x48"); // fullwidth question mark

#if defined(_DEBUG)

_LIT(KLitPanicText, "SHIFTJIS_SHARED");

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

LOCAL_C void Panic(TPanic aPanic)
    {
    User::Panic(KLitPanicText, aPanic);
    }

#endif

// TCombinedArrayOfCharacterSets

class TCombinedArrayOfCharacterSets
    {
public:
    TCombinedArrayOfCharacterSets(const TArray<CnvUtilities::SCharacterSet>* aArrayOfAdditionalCharacterSets);
    ~TCombinedArrayOfCharacterSets();
    TArray<CnvUtilities::SCharacterSet> Array() const;
private:
    static TInt CombinedCount(const CBase* aThis);
    static const TAny* CombinedAccessor(const CBase* aThis, TInt aIndex);
private:
    RArray<CnvUtilities::SCharacterSet> iArrayOfCoreCharacterSets;
    const TArray<CnvUtilities::SCharacterSet>* iArrayOfAdditionalCharacterSets;
    };

TCombinedArrayOfCharacterSets::TCombinedArrayOfCharacterSets(const TArray<CnvUtilities::SCharacterSet>* aArrayOfAdditionalCharacterSets)
    :iArrayOfAdditionalCharacterSets(aArrayOfAdditionalCharacterSets)
    {
    if ( FeatureManager::FeatureSupported(KFeatureIdJapanesePicto) )
        {      
        CnvUtilities::SCharacterSet characterSet;
        characterSet.iConversionData=&CnvJisX0201::ConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=CnvShiftJis::DummyConvertFromIntermediateBufferInPlace;
        characterSet.iEscapeSequence=&KNullDesC8;
        iArrayOfCoreCharacterSets.Append(characterSet);
        characterSet.iConversionData=&CnvJisX0208::ConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=CnvShiftJis::ConvertFromJisX0208ToShiftJisInPlace;
        characterSet.iEscapeSequence=&KNullDesC8;
        iArrayOfCoreCharacterSets.Append(characterSet);

        SetCharacterSetsForPictograph(iArrayOfCoreCharacterSets, ECharsetShiftJis);
        }
    else
        {            
        CnvUtilities::SCharacterSet characterSet;
        characterSet.iConversionData=&CnvJisX0201::ConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=CnvShiftJis::DummyConvertFromIntermediateBufferInPlace;
        characterSet.iEscapeSequence=&KNullDesC8;
        iArrayOfCoreCharacterSets.Append(characterSet);
        characterSet.iConversionData=&CnvJisX0208::ConversionData();
        characterSet.iConvertFromIntermediateBufferInPlace=CnvShiftJis::ConvertFromJisX0208ToShiftJisInPlace;
        characterSet.iEscapeSequence=&KNullDesC8;
        iArrayOfCoreCharacterSets.Append(characterSet);
        }
    }

TCombinedArrayOfCharacterSets::~TCombinedArrayOfCharacterSets()
    {
    iArrayOfCoreCharacterSets.Close();
    }


TArray<CnvUtilities::SCharacterSet> TCombinedArrayOfCharacterSets::Array() const
    {
    return TArray<CnvUtilities::SCharacterSet>(CombinedCount, CombinedAccessor, REINTERPRET_CAST(const CBase*, this));
    }

TInt TCombinedArrayOfCharacterSets::CombinedCount(const CBase* aThis)
    {
    const TCombinedArrayOfCharacterSets& thisReference=*REINTERPRET_CAST(const TCombinedArrayOfCharacterSets*, aThis);
    const TInt numberOfCoreCharacterSets=thisReference.iArrayOfCoreCharacterSets.Count();
    if (thisReference.iArrayOfAdditionalCharacterSets!=NULL)
        {
        return numberOfCoreCharacterSets+thisReference.iArrayOfAdditionalCharacterSets->Count();
        }
    return numberOfCoreCharacterSets;
    }

const TAny* TCombinedArrayOfCharacterSets::CombinedAccessor(const CBase* aThis, TInt aIndex)
    {
    const TCombinedArrayOfCharacterSets& thisReference=*REINTERPRET_CAST(const TCombinedArrayOfCharacterSets*, aThis);
    const TInt numberOfCoreCharacterSets=thisReference.iArrayOfCoreCharacterSets.Count();
    if (aIndex>=numberOfCoreCharacterSets)
        {
        __ASSERT_DEBUG(thisReference.iArrayOfAdditionalCharacterSets!=NULL, Panic(EPanicIndexOverflow1));
        return &(*thisReference.iArrayOfAdditionalCharacterSets)[aIndex-numberOfCoreCharacterSets];
        }
    return &thisReference.iArrayOfCoreCharacterSets[aIndex];
    }

// TCombinedArrayOfMethods

class TCombinedArrayOfMethods
    {
public:
    TCombinedArrayOfMethods(const TArray<CnvUtilities::SMethod>* aArrayOfAdditionalMethods);
    ~TCombinedArrayOfMethods();
    TArray<CnvUtilities::SMethod> Array() const;
private:
    static TInt CombinedCount(const CBase* aThis);
    static const TAny* CombinedAccessor(const CBase* aThis, TInt aIndex);
private:
    RArray<CnvUtilities::SMethod> iArrayOfCoreMethods;
    const TArray<CnvUtilities::SMethod>* iArrayOfAdditionalMethods;
    };

TCombinedArrayOfMethods::TCombinedArrayOfMethods(const TArray<CnvUtilities::SMethod>* aArrayOfAdditionalMethods)
    :iArrayOfAdditionalMethods(aArrayOfAdditionalMethods)
    {
    if ( FeatureManager::FeatureSupported(KFeatureIdJapanesePicto) )
        {      
        SetMethodsForPictograph(iArrayOfCoreMethods, ECharsetShiftJis);
        CnvUtilities::SMethod method;
        method.iNumberOfBytesAbleToConvert=CnvShiftJis::NumberOfBytesAbleToConvertToJisX0201;
        method.iConvertToIntermediateBufferInPlace=CnvShiftJis::DummyConvertToIntermediateBufferInPlace;
        method.iConversionData=&CnvJisX0201::ConversionData();
        method.iNumberOfBytesPerCharacter=1;
        method.iNumberOfCoreBytesPerCharacter=1;
        iArrayOfCoreMethods.Append(method);
        method.iNumberOfBytesAbleToConvert=CnvShiftJis::NumberOfBytesAbleToConvertToJisX0208;
        method.iConvertToIntermediateBufferInPlace=CnvShiftJis::ConvertToJisX0208FromShiftJisInPlace;
        method.iConversionData=&CnvJisX0208::ConversionData();
        method.iNumberOfBytesPerCharacter=2;
        method.iNumberOfCoreBytesPerCharacter=2;
        iArrayOfCoreMethods.Append(method);
        }
    else
        {        
        CnvUtilities::SMethod method;
        method.iNumberOfBytesAbleToConvert=CnvShiftJis::NumberOfBytesAbleToConvertToJisX0201;
        method.iConvertToIntermediateBufferInPlace=CnvShiftJis::DummyConvertToIntermediateBufferInPlace;
        method.iConversionData=&CnvJisX0201::ConversionData();
        method.iNumberOfBytesPerCharacter=1;
        method.iNumberOfCoreBytesPerCharacter=1;
        iArrayOfCoreMethods.Append(method);
        method.iNumberOfBytesAbleToConvert=CnvShiftJis::NumberOfBytesAbleToConvertToJisX0208;
        method.iConvertToIntermediateBufferInPlace=CnvShiftJis::ConvertToJisX0208FromShiftJisInPlace;
        method.iConversionData=&CnvJisX0208::ConversionData();
        method.iNumberOfBytesPerCharacter=2;
        method.iNumberOfCoreBytesPerCharacter=2;
        iArrayOfCoreMethods.Append(method);    
        }
    }

TCombinedArrayOfMethods::~TCombinedArrayOfMethods()
    {
    iArrayOfCoreMethods.Close();
    }


TArray<CnvUtilities::SMethod> TCombinedArrayOfMethods::Array() const
    {
    return TArray<CnvUtilities::SMethod>(CombinedCount, CombinedAccessor, REINTERPRET_CAST(const CBase*, this));
    }

TInt TCombinedArrayOfMethods::CombinedCount(const CBase* aThis)
    {
    const TCombinedArrayOfMethods& thisReference=*REINTERPRET_CAST(const TCombinedArrayOfMethods*, aThis);
    const TInt numberOfCoreMethods=thisReference.iArrayOfCoreMethods.Count();
    if (thisReference.iArrayOfAdditionalMethods!=NULL)
        {
        return numberOfCoreMethods+thisReference.iArrayOfAdditionalMethods->Count();
        }
    return numberOfCoreMethods;
    }

const TAny* TCombinedArrayOfMethods::CombinedAccessor(const CBase* aThis, TInt aIndex)
    {
    const TCombinedArrayOfMethods& thisReference=*REINTERPRET_CAST(const TCombinedArrayOfMethods*, aThis);
    const TInt numberOfCoreMethods=thisReference.iArrayOfCoreMethods.Count();
    if (aIndex>=numberOfCoreMethods)
        {
        __ASSERT_DEBUG(thisReference.iArrayOfAdditionalMethods!=NULL, Panic(EPanicIndexOverflow2));
        return &(*thisReference.iArrayOfAdditionalMethods)[aIndex-numberOfCoreMethods];
        }
    return &thisReference.iArrayOfCoreMethods[aIndex];
    }

// CnvShiftJis

EXPORT_C const TDesC8& CnvShiftJis::ReplacementForUnconvertibleUnicodeCharacters()
    {
    return KLit8ShiftJisReplacementForUnconvertibleUnicodeCharacters;
    }

EXPORT_C TInt CnvShiftJis::ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, TDes8& aForeign, const TDesC16& aUnicode, CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
    {
    return DoConvertFromUnicode(aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters, NULL);
    }

/**
 * Converts text from Unicode to Shift-JIS (aArrayOfAdditionalCharacterSets provides support
 * for Shift-JIS extensions)
 *
 * @since Internationalization_6.2
 */
EXPORT_C TInt CnvShiftJis::ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, TDes8& aForeign, const TDesC16& aUnicode, CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters, const TArray<CnvUtilities::SCharacterSet>& aArrayOfAdditionalCharacterSets)
    {
    return DoConvertFromUnicode(aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters, &aArrayOfAdditionalCharacterSets);
    }

EXPORT_C TInt CnvShiftJis::ConvertToUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,
    TDes16& aUnicode, const TDesC8& aForeign,
    TInt& aNumberOfUnconvertibleCharacters,
    TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
    {
    // DummyData converts any pair of bytes into FFFD
    static const SCnvConversionData::SVariableByteData::SRange
        DummyByteLengths = {0, 255, 1, 0};
    static const SCnvConversionData::SOneDirectionData::SRange DummyConv =
        { 0xFFFD, 0xFFFD,
        SCnvConversionData::SOneDirectionData::SRange::EDirect, 2, 0 };
    static const SCnvConversionData DummyData =
        {
        SCnvConversionData::EFixedBigEndian,
        {1, &DummyByteLengths},
        {1, &DummyConv},
        {1, &DummyConv}
        };
    TFixedArray<CnvUtilities::SMethod, 2> nullExtension;
    // First extension converts unrecognised double byte characters to FFFD
    nullExtension[0].iNumberOfBytesAbleToConvert
        = BytesInOtherDoubleByteAreas;
    nullExtension[0].iConvertToIntermediateBufferInPlace
        = CnvShiftJis::DummyConvertToIntermediateBufferInPlace;
    // This conversion data won't do anything for us: it will just get us
    // a load of FFFD, which is what we want.
    nullExtension[0].iConversionData = &DummyData;
    nullExtension[0].iNumberOfBytesPerCharacter = 2;
    nullExtension[0].iNumberOfCoreBytesPerCharacter = 2;
    // Second extension converts any other single byte to FFFD,
    // but only ever one!
    nullExtension[1].iNumberOfBytesAbleToConvert
        = OneIfNotValid;
    nullExtension[1].iConvertToIntermediateBufferInPlace
        = CnvShiftJis::DummyConvertToIntermediateBufferInPlace;
    // This conversion data won't do anything for us: it will just get us
    // a load of FFFD, which is what we want.
    nullExtension[1].iConversionData = &CnvJisX0201::ConversionData();
    nullExtension[1].iNumberOfBytesPerCharacter = 1;
    nullExtension[1].iNumberOfCoreBytesPerCharacter = 1;
    const TArray<CnvUtilities::SMethod> nullExtensionArray = nullExtension.Array();
    return DoConvertToUnicode(aDefaultEndiannessOfForeignCharacters,
        aUnicode, aForeign, aNumberOfUnconvertibleCharacters,
        aIndexOfFirstByteOfFirstUnconvertibleCharacter,
        &nullExtensionArray);
    }

/**
 * Converts text from Shift-JIS to Unicode (aArrayOfAdditionalMethods provides support
 * for Shift-JIS extensions)
 *
 * @since Internationalization_6.2
 */
EXPORT_C TInt CnvShiftJis::ConvertToUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aForeign, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, const TArray<CnvUtilities::SMethod>& aArrayOfAdditionalMethods)
    {
    return DoConvertToUnicode(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter, &aArrayOfAdditionalMethods);
    }

TInt CnvShiftJis::DoConvertFromUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,
    const TDesC8& aReplacementForUnconvertibleUnicodeCharacters,
    TDes8& aForeign, const TDesC16& aUnicode,
    CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters,
    const TArray<CnvUtilities::SCharacterSet>* aArrayOfAdditionalCharacterSets)
    {
    TCombinedArrayOfCharacterSets combinedArrayOfCharacterSets(aArrayOfAdditionalCharacterSets);
    return CnvUtilities::ConvertFromUnicode(aDefaultEndiannessOfForeignCharacters, aReplacementForUnconvertibleUnicodeCharacters, aForeign, aUnicode, aIndicesOfUnconvertibleCharacters, combinedArrayOfCharacterSets.Array());
    }

TInt CnvShiftJis::DoConvertToUnicode(
    CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,
    TDes16& aUnicode, const TDesC8& aForeign,
    TInt& aNumberOfUnconvertibleCharacters,
    TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter,
    const TArray<CnvUtilities::SMethod>* aArrayOfAdditionalMethods)
    {
    TCombinedArrayOfMethods combinedArrayOfMethods(aArrayOfAdditionalMethods);
    TInt unconvert = CnvUtilities::ConvertToUnicodeFromHeterogeneousForeign(
        aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign,
        aNumberOfUnconvertibleCharacters,
        aIndexOfFirstByteOfFirstUnconvertibleCharacter,
        combinedArrayOfMethods.Array());

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

void CnvShiftJis::DummyConvertFromIntermediateBufferInPlace(TInt, TDes8&, TInt& aNumberOfCharactersThatDroppedOut)
    {
    aNumberOfCharactersThatDroppedOut=0;
    }

void CnvShiftJis::ConvertFromJisX0208ToShiftJisInPlace(TInt aStartPositionInDescriptor, TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut)
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

TInt CnvShiftJis::NumberOfBytesAbleToConvertToJisX0201(const TDesC8& aDescriptor)
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
        const TUint currentByte=*(pointerToPreviousByte+1);
        if (((currentByte>KSingleByteRangeFirstBlockEnd) && (currentByte<KSingleByteRangeSecondBlockStart)) ||
            (currentByte>KSingleByteRangeSecondBlockEnd))
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

TInt CnvShiftJis::NumberOfBytesAbleToConvertToJisX0208(const TDesC8& aDescriptor)
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
        if (!IsJisX0208LeadByte(currentByte))
            {
            if (!IsOtherLeadByte(currentByte))
                {
                break;
                }
            }
        __ASSERT_DEBUG(pointerToPreviousByte<pointerToLastByte, Panic(EPanicBadPointers6));
        if (pointerToPreviousByte+1>=pointerToLastByte)
            {
            break;
            }
        __ASSERT_DEBUG(pointerToPreviousByte+2<=pointerToLastByte, Panic(EPanicBadPointers7));
        currentByte=*(pointerToPreviousByte+2);
        if (!IsValidTrailByte(currentByte))
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

void CnvShiftJis::DummyConvertToIntermediateBufferInPlace(TDes8&)
    {
    }

void CnvShiftJis::ConvertToJisX0208FromShiftJisInPlace(TDes8& aDescriptor)
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

TInt BytesInOtherDoubleByteAreas(const TDesC8& aShiftJis)
    {
    const TText8* start = aShiftJis.Ptr();
    const TText8* end = start + aShiftJis.Length() - 1;
    const TText8* p = start;
    while (p < end && IsOtherLeadByte(p[0]) && IsValidTrailByte(p[1]))
        p += 2;
    return p - start;
    }

TInt OneIfNotValid(const TDesC8& aShiftJis)
    {
    TInt length = aShiftJis.Length();
    if (length == 0)
        return 0;
    TInt c = aShiftJis[0];
    // If the next byte is not a lead byte, we can swallow it. The
    // double-byte methods obviously can't be waiting for more
    // information.
    if (!IsOtherLeadByte(c) && !IsJisX0208LeadByte(c))
        return 1;
    // Otherwise we have a lead byte. If it is on its own we cannot
    // swallow it as the double-byte converters will be waiting for
    // more information. If they have the information (i.e. if length > 1)
    // then they have passed on it and we can swallow it.
    return length == 1? 0 : 1;
    }
