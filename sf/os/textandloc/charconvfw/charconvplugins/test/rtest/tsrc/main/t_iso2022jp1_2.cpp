/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32base.h>
#include <f32file.h>
#include <charconv.h>
#include "t_iso2022jp1_2.h"

#define test(cond)                                  \
    {                                               \
    TBool __bb = (cond);                            \
    TEST(__bb);                                     \
    if (!__bb)                                      \
        {                                           \
        ERR_PRINTF1(_L("ERROR: Test Failed"));      \
        User::Leave(1);                             \
        }                                           \
    }

const TUint KCharacterSetTestIso2022jp1_2=0x01000009;
const TInt KBufferLength=100;
const TInt KNotEnoughBufferLength=17;

void CT_ISO2022JP1_2::TestConversionFromUnicodeToIso(
                CCnvCharacterSetConverter& aCharacterSetConverter,
                const TDesC16& aOriginalUnicode)
 	{
	INFO_PRINTF1(_L(" TestConversionFromUnicodeToIso "));
 	TBuf8<KNotEnoughBufferLength> generatedIso2022Jp;
	generatedIso2022Jp.FillZ(KNotEnoughBufferLength);
 	const TInt returnValue=aCharacterSetConverter.ConvertFromUnicode(generatedIso2022Jp, aOriginalUnicode);
 	test(returnValue>=0);
 	TInt state=CCnvCharacterSetConverter::KStateDefault;
 	TBuf16<KBufferLength> generatedUnicode;
	TInt testresult;
	testresult=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedIso2022Jp, state);
 	test(testresult==0);
 //	test(generatedUnicode==aOriginalUnicode);
 	}

void CT_ISO2022JP1_2::TestTruncatedConversionFromUnicodeToIso2022Jp(
                CCnvCharacterSetConverter& aCharacterSetConverter,
                const TDesC16& aOriginalUnicode)
	{
	INFO_PRINTF1(_L(" TestTruncatedConversionFromUnicodeToIso2022Jp "));
	for (TInt i=aOriginalUnicode.Length(); i>=0; --i)
		{
		TBuf8<KBufferLength> generatedIso2022Jp;
		const TInt returnValue=aCharacterSetConverter.ConvertFromUnicode(generatedIso2022Jp, aOriginalUnicode.Left(i));
		test(returnValue>=0);
		TBuf8<KBufferLength> generatedsecondPartOfIso2022Jp;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedsecondPartOfIso2022Jp, aOriginalUnicode.Mid(i-returnValue))==0);
		generatedIso2022Jp.Append(generatedsecondPartOfIso2022Jp);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedIso2022Jp, state)==0);
		test(generatedUnicode==aOriginalUnicode);
		}
	}

void CT_ISO2022JP1_2::TestSplittingConvertingFromUnicodeToIso2022Jp(
                CCnvCharacterSetConverter& aCharacterSetConverter,
                TInt aMaximumLengthLowerLimit,
                TInt aMaximumLengthUpperLimit,
                TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit,
                const TDesC8& aExpectedFirstPartOfIso2022Jp,
                const TDesC8& aExpectedSecondPartOfIso2022Jp,
                const TDesC16& aOriginalUnicode)
	{
	INFO_PRINTF1(_L(" TestTruncatedConversionFromUnicodeToIso2022Jp "));
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint8 iso2022JpBuffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr8 generatedFirstPartOfIso2022Jp(iso2022JpBuffer, i);
		test(aCharacterSetConverter.ConvertFromUnicode(generatedFirstPartOfIso2022Jp, aOriginalUnicode)==aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit);
		test(generatedFirstPartOfIso2022Jp==aExpectedFirstPartOfIso2022Jp);
		TBuf8<KBufferLength> generatedSecondPartOfIso2022Jp;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfIso2022Jp, aOriginalUnicode.Right(aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit))==0);
		test(generatedSecondPartOfIso2022Jp==aExpectedSecondPartOfIso2022Jp);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedFirstPartOfIso2022Jp, state)==0);
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, generatedSecondPartOfIso2022Jp, state)==0);
		generatedUnicode.Append(generatedSecondPartOfUnicode);
		test(generatedUnicode==aOriginalUnicode);
		}
	}

void CT_ISO2022JP1_2::TestTruncatedConversionToUnicodeFromIso2022Jp(
                CCnvCharacterSetConverter& aCharacterSetConverter,
                const TDesC16& aExpectedUnicode,
                const TDesC8& aOriginalIso2022Jp)
	{
	INFO_PRINTF1(_L(" TestTruncatedConversionToUnicodeFromIso2022Jp "));
	for (TInt i=aOriginalIso2022Jp.Length(); i>=3; --i) // 3 is the length of ISO-2022-JP's longest escape sequence
		{
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, aOriginalIso2022Jp.Left(i), state);
		test(returnValue>=0);
		TBuf16<KBufferLength> generatedsecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedsecondPartOfUnicode, aOriginalIso2022Jp.Mid(i-returnValue), state)==0);
		generatedUnicode.Append(generatedsecondPartOfUnicode);
		test(generatedUnicode==aExpectedUnicode);
		}
	}

void CT_ISO2022JP1_2::TestSplittingConvertingToUnicodeFromIso2022Jp(
                CCnvCharacterSetConverter& aCharacterSetConverter,
                TInt aMaximumLengthLowerLimit,
                TInt aMaximumLengthUpperLimit,
                TInt aExpectedNumberOfIso2022JpBytesNotConvertedAtSplit,
                TInt aExpectedLengthOfFirstPartOfUnicode,
                const TDesC16& aExpectedUnicode,
                const TDesC8& aOriginalIso2022Jp)
	{
	INFO_PRINTF1(_L(" TestSplittingConvertingToUnicodeFromIso2022Jp "));
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint16 unicodeBuffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr16 generatedFirstPartOfUnicode(unicodeBuffer, i);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedFirstPartOfUnicode, aOriginalIso2022Jp, state);
		test(generatedFirstPartOfUnicode==aExpectedUnicode.Left(aExpectedLengthOfFirstPartOfUnicode));
		test(returnValue==aExpectedNumberOfIso2022JpBytesNotConvertedAtSplit);
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, aOriginalIso2022Jp.Right(aExpectedNumberOfIso2022JpBytesNotConvertedAtSplit), state)==0);
		test(generatedSecondPartOfUnicode==aExpectedUnicode.Mid(aExpectedLengthOfFirstPartOfUnicode));
		TBuf8<KBufferLength> generatedIso2022Jp;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedIso2022Jp, generatedFirstPartOfUnicode)==0);
		TBuf8<KBufferLength> generatedSecondPartOfIso2022Jp;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfIso2022Jp, generatedSecondPartOfUnicode)==0);
		generatedIso2022Jp.Append(generatedSecondPartOfIso2022Jp);
		TBuf16<KBufferLength> regeneratedUnicode;
		state=CCnvCharacterSetConverter::KStateDefault;
		test(aCharacterSetConverter.ConvertToUnicode(regeneratedUnicode, generatedIso2022Jp, state)==0);
//		test(regeneratedUnicode==aExpectedUnicode);
		state=CCnvCharacterSetConverter::KStateDefault;
		test(aCharacterSetConverter.ConvertToUnicode(regeneratedUnicode, aOriginalIso2022Jp, state)==0);
		test(regeneratedUnicode==aExpectedUnicode);
		}
	}

void CT_ISO2022JP1_2::TestIsIllFormedIso2022Jp(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aIso2022Jp)
	{
	INFO_PRINTF1(_L(" TestIsIllFormedIso2022Jp "));
	TBuf16<50> generatedUnicode;
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TPtrC8 remainderOfIso2022Jp(aIso2022Jp);
	TInt lastReturnValue=KMaxTInt;
	FOREVER
		{
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, remainderOfIso2022Jp, state);
		if (returnValue==CCnvCharacterSetConverter::EErrorIllFormedInput)
			{
			break;
			}
		test(returnValue>0);
		test(returnValue<lastReturnValue);
		lastReturnValue=returnValue;
		remainderOfIso2022Jp.Set(remainderOfIso2022Jp.Right(returnValue));
		}
	}

void CT_ISO2022JP1_2::TestIso2022Jp(CCnvCharacterSetConverter * characterSetConverter, TBool isS60version)
    {
    //
    INFO_PRINTF1(_L("Empty descriptor"));
    TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 0, 10, 0, KNullDesC8, KNullDesC8, KNullDesC16);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 0, 10, 0, 0, KNullDesC16, KNullDesC8);
    INFO_PRINTF1(_L("Testing converting to ISO-2022-JP"));

    _LIT(KTestUnicode,">.@>0l90");
    _LIT(KTestUnicode2,"\x0393\x03b1\x03c3\x03bf\x03c5\x3055\x3088");
    TestConversionFromUnicodeToIso(*characterSetConverter, KTestUnicode2);
    TestConversionFromUnicodeToIso(*characterSetConverter, KTestUnicode);
    
    TBuf16<50> originalUnicode;
    originalUnicode.Format(_L16("%c%c%c%c%c\xa5%c%c%c%c%c"), 0x0393, 0x03b1, 0x03c3, 0x03bf, 0x03c5, 0x3055, 0x3088, 0x3046, 0x306a, 0x3089);
    TestTruncatedConversionFromUnicodeToIso2022Jp(*characterSetConverter, originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 3, 7, 12, _L8(""), _L8("\x1b\x24\x42\x26\x23\x26\x41\x26\x52\x26\x4f\x26\x54\x1b\x28\x4a\\\x1b\x28\x42\\\x1b\x24\x42\x24\x35\x24\x68\x24\x26\x24\x4a\x24\x69\x1b\x28\x42"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 8, 9, 11, _L8("\x1b\x24\x42\x26\x23\x1b\x28\x42"), _L8("\x1b\x24\x42\x26\x41\x26\x52\x26\x4f\x26\x54\x1b\x28\x4a\\\x1b\x28\x42\\\x1b\x24\x42\x24\x35\x24\x68\x24\x26\x24\x4a\x24\x69\x1b\x28\x42"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 10, 11, 10, _L8("\x1b\x24\x42\x26\x23\x26\x41\x1b\x28\x42"), _L8("\x1b\x24\x42\x26\x52\x26\x4f\x26\x54\x1b\x28\x4a\\\x1b\x28\x42\\\x1b\x24\x42\x24\x35\x24\x68\x24\x26\x24\x4a\x24\x69\x1b\x28\x42"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 12, 13, 9, _L8("\x1b\x24\x42\x26\x23\x26\x41\x26\x52\x1b\x28\x42"), _L8("\x1b\x24\x42\x26\x4f\x26\x54\x1b\x28\x4a\\\x1b\x28\x42\\\x1b\x24\x42\x24\x35\x24\x68\x24\x26\x24\x4a\x24\x69\x1b\x28\x42"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 14, 15, 8, _L8("\x1b\x24\x42\x26\x23\x26\x41\x26\x52\x26\x4f\x1b\x28\x42"), _L8("\x1b\x24\x42\x26\x54\x1b\x28\x4a\\\x1b\x28\x42\\\x1b\x24\x42\x24\x35\x24\x68\x24\x26\x24\x4a\x24\x69\x1b\x28\x42"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 16, 19, 7, _L8("\x1b\x24\x42\x26\x23\x26\x41\x26\x52\x26\x4f\x26\x54\x1b\x28\x42"), _L8("\x1b\x28\x4a\\\x1b\x28\x42\\\x1b\x24\x42\x24\x35\x24\x68\x24\x26\x24\x4a\x24\x69\x1b\x28\x42"), originalUnicode);
//    TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 20, 23, 6, _L8("\x1b\x24\x42\x26\x23\x26\x41\x26\x52\x26\x4f\x26\x54\x1b\x28\x4a\\\x1b\x28\x42"), _L8("\\\x1b\x24\x42\x24\x35\x24\x68\x24\x26\x24\x4a\x24\x69\x1b\x28\x42"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 24, 28, 5, _L8("\x1b\x24\x42\x26\x23\x26\x41\x26\x52\x26\x4f\x26\x54\x1b\x28\x4a\\\x1b\x28\x42\\"), _L8("\x1b\x24\x42\x24\x35\x24\x68\x24\x26\x24\x4a\x24\x69\x1b\x28\x42"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 29, 30, 4, _L8("\x1b\x24\x42\x26\x23\x26\x41\x26\x52\x26\x4f\x26\x54\x1b\x28\x4a\\\x1b\x28\x42\\\x1b\x24\x42\x24\x35\x1b\x28\x42"), _L8("\x1b\x24\x42\x24\x68\x24\x26\x24\x4a\x24\x69\x1b\x28\x42"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 31, 32, 3, _L8("\x1b\x24\x42\x26\x23\x26\x41\x26\x52\x26\x4f\x26\x54\x1b\x28\x4a\\\x1b\x28\x42\\\x1b\x24\x42\x24\x35\x24\x68\x1b\x28\x42"), _L8("\x1b\x24\x42\x24\x26\x24\x4a\x24\x69\x1b\x28\x42"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 33, 34, 2, _L8("\x1b\x24\x42\x26\x23\x26\x41\x26\x52\x26\x4f\x26\x54\x1b\x28\x4a\\\x1b\x28\x42\\\x1b\x24\x42\x24\x35\x24\x68\x24\x26\x1b\x28\x42"), _L8("\x1b\x24\x42\x24\x4a\x24\x69\x1b\x28\x42"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 35, 36, 1, _L8("\x1b\x24\x42\x26\x23\x26\x41\x26\x52\x26\x4f\x26\x54\x1b\x28\x4a\\\x1b\x28\x42\\\x1b\x24\x42\x24\x35\x24\x68\x24\x26\x24\x4a\x1b\x28\x42"), _L8("\x1b\x24\x42\x24\x69\x1b\x28\x42"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 37, 50, 0, _L8("\x1b\x24\x42\x26\x23\x26\x41\x26\x52\x26\x4f\x26\x54\x1b\x28\x4a\\\x1b\x28\x42\\\x1b\x24\x42\x24\x35\x24\x68\x24\x26\x24\x4a\x24\x69\x1b\x28\x42"), KNullDesC8, originalUnicode);
    originalUnicode.Format(_L16("%cX%cY%cZ"), 0x6153, 0x6376, 0x65d9);
    TestTruncatedConversionFromUnicodeToIso2022Jp(*characterSetConverter, originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 3, 7, 6, _L8(""), _L8("\x1b\x24\x42XX\x1b\x28\x42X\x1b\x24\x42YY\x1b\x28\x42Y\x1b\x24\x42ZZ\x1b\x28\x42Z"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 8, 11, 5, _L8("\x1b\x24\x42XX\x1b\x28\x42"), _L8("X\x1b\x24\x42YY\x1b\x28\x42Y\x1b\x24\x42ZZ\x1b\x28\x42Z"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 12, 16, 4, _L8("\x1b\x24\x42XX\x1b\x28\x42X"), _L8("\x1b\x24\x42YY\x1b\x28\x42Y\x1b\x24\x42ZZ\x1b\x28\x42Z"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 17, 20, 3, _L8("\x1b\x24\x42XX\x1b\x28\x42X\x1b\x24\x42YY\x1b\x28\x42"), _L8("Y\x1b\x24\x42ZZ\x1b\x28\x42Z"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 21, 25, 2, _L8("\x1b\x24\x42XX\x1b\x28\x42X\x1b\x24\x42YY\x1b\x28\x42Y"), _L8("\x1b\x24\x42ZZ\x1b\x28\x42Z"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 26, 29, 1, _L8("\x1b\x24\x42XX\x1b\x28\x42X\x1b\x24\x42YY\x1b\x28\x42Y\x1b\x24\x42ZZ\x1b\x28\x42"), _L8("Z"), originalUnicode);
//  TestSplittingConvertingFromUnicodeToIso2022Jp(*characterSetConverter, 30, 40, 0, _L8("\x1b\x24\x42XX\x1b\x28\x42X\x1b\x24\x42YY\x1b\x28\x42Y\x1b\x24\x42ZZ\x1b\x28\x42Z"), KNullDesC8, originalUnicode);
    INFO_PRINTF1(_L("Testing converting to Unicode"));
    const TPtrC8 originalIso2022Jp(_S8("\x1b\x24\x40\x1b\x28\x4aMy name is \x1b\x28\x4a\x1b\x28\x42\x1b\x24\x40\x25\x47\x25\x23\x25\x53\x25\x45\x1b\x28\x4a in \x1b\x24\x42\x46\x7c\x4b\x5c\x38\x6c\x1b\x28\x42\\~\x1b\x28\x4a\\~"));
    TBuf16<50> expectedUnicode;
    if (!isS60version)        
        expectedUnicode.Format(_L16("My name is %c%c%c%c in %c%c%c\\~%c%c"), 0x30c7, 0x30a3, 0x30d3, 0x30c5, 0x65e5, 0x672c, 0x8a9e, 0x00a5, 0x203e);
    else
        expectedUnicode.Format(_L16("My name is %c%c%c%c in %c%c%c\\~%c%c"), 0x30c7, 0x30a3, 0x30d3, 0x30c5, 0x65e5, 0x672c, 0x8a9e, 0x00a5, 0x7e);
    TestTruncatedConversionToUnicodeFromIso2022Jp(*characterSetConverter, expectedUnicode, originalIso2022Jp);
    TestTruncatedConversionToUnicodeFromIso2022Jp(*characterSetConverter, _L16(" Hello"), _L8("\x1b\x24\x42\x1b\x28\x4a\x1b\x24\x42\x1b\x28\x4a\x1b\x24\x42\x1b\x28\x4a Hello"));
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 0, 0, 60, 0, expectedUnicode, originalIso2022Jp);
    for (int i=1; i<=10; ++i)
        {
        TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, i, i, 54-i, i, expectedUnicode, originalIso2022Jp);
        }
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 11, 11, 34, 11, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 12, 12, 32, 12, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 13, 13, 30, 13, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 14, 14, 28, 14, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 15, 15, 23, 15, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 16, 16, 22, 16, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 17, 17, 21, 17, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 18, 18, 20, 18, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 19, 19, 16, 19, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 20, 20, 14, 20, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 21, 21, 12, 21, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 22, 22, 7, 22, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 23, 23, 6, 23, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 24, 24, 2, 24, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 25, 25, 1, 25, expectedUnicode, originalIso2022Jp);
    TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, 26, 40, 0, 26, expectedUnicode, originalIso2022Jp);
    INFO_PRINTF1(_L("Testing the default ISO-2022-JP state"));
    for (int i=0; i<=6; ++i)
        {
        TestSplittingConvertingToUnicodeFromIso2022Jp(*characterSetConverter, i, i, 6-i, i, _L16("Hello\xa5"), _L8("Hello\\"));
        }
    INFO_PRINTF1(_L("Testing ill-formed ISO-2022-JP"));
    TestIsIllFormedIso2022Jp(*characterSetConverter, _L8("abc \x1b\x24\x42\x21\x1b\x28\x4a def"));
    TestIsIllFormedIso2022Jp(*characterSetConverter, _L8("abc \x1b\x24\x42\x21\x21\x21\x1b\x28\x4a def"));
    TestIsIllFormedIso2022Jp(*characterSetConverter, _L8("abc \x1b\x24\x42\x21\x21\x21\x21\x21\x1b\x28\x4a def"));
    TestIsIllFormedIso2022Jp(*characterSetConverter, _L8("abc \x1b"));
    TestIsIllFormedIso2022Jp(*characterSetConverter, _L8("abc \x1b\x24"));
    TestIsIllFormedIso2022Jp(*characterSetConverter, _L8("abc \x1b\x24\xff"));
    TestIsIllFormedIso2022Jp(*characterSetConverter, _L8("abc \x1b\x26\x40"));
    }

void CT_ISO2022JP1_2::DoE32MainL()
	{
    INFO_PRINTF1(_L(" TISO2022JP_2 "));
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(fileServerSession);
	INFO_PRINTF1(_L("Available:\n"));
	TInt i;
	for (i=arrayOfCharacterSetsAvailable->Count()-1; i>=0; --i)
		{
		const CCnvCharacterSetConverter::SCharacterSet& charactersSet=(*arrayOfCharacterSetsAvailable)[i];
		characterSetConverter->PrepareToConvertToOrFromL(charactersSet.Identifier(), *arrayOfCharacterSetsAvailable, fileServerSession);
		TPtrC charactersSetName(charactersSet.Name());
		if (charactersSet.NameIsFileName())
			{
			charactersSetName.Set(TParsePtrC(charactersSetName).Name());
			}
		INFO_PRINTF2(_L("    %S\n"), &charactersSetName);
		}
	
	INFO_PRINTF1(_L("Testing ISO-2022-JP conversions"));
	
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetTestIso2022jp1_2, *arrayOfCharacterSetsAvailable, fileServerSession);
	TestIso2022Jp(characterSetConverter);

	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierIso2022Jp1, *arrayOfCharacterSetsAvailable, fileServerSession);
    TestIso2022Jp(characterSetConverter, EFalse);
	
	CleanupStack::PopAndDestroy(3); // arrayOfCharacterSetsAvailable and characterSetConverter and fileServerSession
	}

CT_ISO2022JP1_2::CT_ISO2022JP1_2()
    {
    SetTestStepName(KTestStep_T_ISO2022JP1_2);
    }

TVerdict CT_ISO2022JP1_2::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;
    TRAPD(error1, DoE32MainL());    
    __UHEAP_MARKEND;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }


