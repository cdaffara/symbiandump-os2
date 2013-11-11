/*
* Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_hz.h"

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

const TInt KBufferLength=100;
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0503
@SYMTestCaseDesc        Tests for truncated conversion from Unicode to HZ character set
@SYMTestPriority        Medium
@SYMTestActions         Tests for generated Unicode with the original Unicode
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_HZ::TestTruncatedConversionFromUnicodeToHz(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0503 "));
	for (TInt i=aOriginalUnicode.Length(); i>=0; --i)
		{
		TBuf8<KBufferLength> generatedHz;
		const TInt returnValue=aCharacterSetConverter.ConvertFromUnicode(generatedHz, aOriginalUnicode.Left(i));
		test(returnValue>=0);
		TBuf8<KBufferLength> generatedsecondPartOfHz;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedsecondPartOfHz, aOriginalUnicode.Mid(i-returnValue))==0);
		generatedHz.Append(generatedsecondPartOfHz);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedHz, state)==0);
		test(generatedUnicode==aOriginalUnicode);
		}
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0504
@SYMTestCaseDesc        Splitting and converting from Unicode to HZ test
@SYMTestPriority        Medium
@SYMTestActions         Tests for conversion after splitting from Unicode to Hz and back to Unicode
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_HZ::TestSplittingConvertingFromUnicodeToHz(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, const TDesC8& aExpectedFirstPartOfHz, const TDesC8& aExpectedSecondPartOfHz, const TDesC16& aOriginalUnicode)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0504 "));
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint8 hzBuffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr8 generatedFirstPartOfHz(hzBuffer, i);
		test(aCharacterSetConverter.ConvertFromUnicode(generatedFirstPartOfHz, aOriginalUnicode)==aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit);
		test(generatedFirstPartOfHz==aExpectedFirstPartOfHz);
		TBuf8<KBufferLength> generatedSecondPartOfHz;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfHz, aOriginalUnicode.Right(aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit))==0);
		test(generatedSecondPartOfHz==aExpectedSecondPartOfHz);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedFirstPartOfHz, state)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, generatedSecondPartOfHz, state)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		generatedUnicode.Append(generatedSecondPartOfUnicode);
		test(generatedUnicode==aOriginalUnicode);
		}
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0505
@SYMTestCaseDesc        Tests for truncated conversion from HZ to Unicode
@SYMTestPriority        Medium
@SYMTestActions         Tests for conversion after truncation from HZ to Unicode and back to HZ
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_HZ::TestTruncatedConversionToUnicodeFromHz(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalHz)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0505 "));
	for (TInt i=aOriginalHz.Length(); i>=2; --i) // 2 is the length of HZ's "tilde" sequences
		{
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, aOriginalHz.Left(i), state);
		test(returnValue>=0);
		TBuf16<KBufferLength> generatedsecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedsecondPartOfUnicode, aOriginalHz.Mid(i-returnValue), state)==0);
		generatedUnicode.Append(generatedsecondPartOfUnicode);
		test(generatedUnicode==aExpectedUnicode);
		}
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0506
@SYMTestCaseDesc        Splitting and converting to Unicode from HZ test
@SYMTestPriority        Medium
@SYMTestActions         Tests for conversion after splitting from Hz to Unicode and back to Hz
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_HZ::TestSplittingConvertingToUnicodeFromHz(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfHzBytesNotConvertedAtSplit, TInt aExpectedStateAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalHz)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0506 "));
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint16 unicodeBuffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr16 generatedFirstPartOfUnicode(unicodeBuffer, i);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedFirstPartOfUnicode, aOriginalHz, state);
		test(generatedFirstPartOfUnicode==aExpectedUnicode.Left(aExpectedLengthOfFirstPartOfUnicode));
		test((returnValue==aExpectedNumberOfHzBytesNotConvertedAtSplit) && (state==aExpectedStateAtSplit));
		{
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		state=aExpectedStateAtSplit;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, aOriginalHz.Right(aExpectedNumberOfHzBytesNotConvertedAtSplit), state)==0);
		test(generatedSecondPartOfUnicode==aExpectedUnicode.Mid(aExpectedLengthOfFirstPartOfUnicode));
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf8<KBufferLength> generatedHz;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedHz, generatedFirstPartOfUnicode)==0);
		TBuf8<KBufferLength> generatedSecondPartOfHz;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfHz, generatedSecondPartOfUnicode)==0);
		generatedHz.Append(generatedSecondPartOfHz);
		TBuf16<KBufferLength> regeneratedUnicode;
		state=CCnvCharacterSetConverter::KStateDefault;
		test(aCharacterSetConverter.ConvertToUnicode(regeneratedUnicode, generatedHz, state)==0);
		test(regeneratedUnicode==aExpectedUnicode);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		test(aCharacterSetConverter.ConvertToUnicode(regeneratedUnicode, aOriginalHz, state)==0);
		test(regeneratedUnicode==aExpectedUnicode);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		}
		{
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		state=aExpectedStateAtSplit;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, aOriginalHz.Right(aExpectedNumberOfHzBytesNotConvertedAtSplit), state)==0);
		test(generatedSecondPartOfUnicode==aExpectedUnicode.Mid(aExpectedLengthOfFirstPartOfUnicode));
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf8<KBufferLength> generatedHz;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedHz, generatedFirstPartOfUnicode)==0);
		TBuf8<KBufferLength> generatedSecondPartOfHz;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfHz, generatedSecondPartOfUnicode)==0);
		generatedHz.Append(generatedSecondPartOfHz);
		TBuf16<KBufferLength> regeneratedUnicode;
		state=CCnvCharacterSetConverter::KStateDefault;
		test(aCharacterSetConverter.ConvertToUnicode(regeneratedUnicode, generatedHz, state)==0);
		test(regeneratedUnicode==aExpectedUnicode);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		test(aCharacterSetConverter.ConvertToUnicode(regeneratedUnicode, aOriginalHz, state)==0);
		test(regeneratedUnicode==aExpectedUnicode);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		}
		}
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0507
@SYMTestCaseDesc        Conversion of bad HZ format to Unicode test
@SYMTestPriority        Medium
@SYMTestActions         Tests to convert bad Hz format to Unicode.
                        Check the status of CCnvCharacterSetConverter::EErrorIllFormedInput error flag
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_HZ::TestIsIllFormedHz(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aHz)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0507 "));
	TBuf16<50> generatedUnicode;
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TPtrC8 remainderOfHz(aHz);
	TInt lastReturnValue=KMaxTInt;
	FOREVER
		{
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, remainderOfHz, state);
		if (returnValue==CCnvCharacterSetConverter::EErrorIllFormedInput)
			{
			break;
			}
		test(returnValue>0);
		test(returnValue<lastReturnValue);
		lastReturnValue=returnValue;
		remainderOfHz.Set(remainderOfHz.Right(returnValue));
		}
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0508
@SYMTestCaseDesc        HZ to Unicode and Unicode to HZ conversion tests
@SYMTestPriority        Medium
@SYMTestActions         Wrapper function to call HZ conversion test functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_HZ::DoE32MainL()
	{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0508 "));
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableLC(fileServerSession);
	INFO_PRINTF1(_L("Available:\n"));
	for (TInt i=arrayOfCharacterSetsAvailable->Count()-1; i>=0; --i)
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
	INFO_PRINTF1(_L("Testing HZ conversions"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierHz, *arrayOfCharacterSetsAvailable, fileServerSession);
	//
	INFO_PRINTF1(_L("Empty descriptor"));
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 0, 10, 0, KNullDesC8, KNullDesC8, KNullDesC16);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 0, 10, 0, CCnvCharacterSetConverter::KStateDefault, 0, KNullDesC16, KNullDesC8);
	INFO_PRINTF1(_L("Testing converting to HZ"));
	TBuf16<50> originalUnicode;
	originalUnicode.Format(_L16("%c%cxyz%c%c~%c~~%ce~a%c"), 0x4e2d, 0x56fd, 0x5979, 0x4ed6, 0x4eba, 0x4e09, 0x7259);
	TestTruncatedConversionFromUnicodeToHz(*characterSetConverter, originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 0, 5, 16, KNullDesC8, _L8("~{VP9z~}xyz~{K}K{~}~~~{HK~}~~~~~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 6, 7, 15, _L8("~{VP~}"), _L8("~{9z~}xyz~{K}K{~}~~~{HK~}~~~~~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 8, 8, 14, _L8("~{VP9z~}"), _L8("xyz~{K}K{~}~~~{HK~}~~~~~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 9, 9, 13, _L8("~{VP9z~}x"), _L8("yz~{K}K{~}~~~{HK~}~~~~~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 10, 10, 12, _L8("~{VP9z~}xy"), _L8("z~{K}K{~}~~~{HK~}~~~~~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 11, 16, 11, _L8("~{VP9z~}xyz"), _L8("~{K}K{~}~~~{HK~}~~~~~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 17, 18, 10, _L8("~{VP9z~}xyz~{K}~}"), _L8("~{K{~}~~~{HK~}~~~~~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 19, 20, 9, _L8("~{VP9z~}xyz~{K}K{~}"), _L8("~~~{HK~}~~~~~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 21, 26, 8, _L8("~{VP9z~}xyz~{K}K{~}~~"), _L8("~{HK~}~~~~~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 27, 28, 7, _L8("~{VP9z~}xyz~{K}K{~}~~~{HK~}"), _L8("~~~~~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 29, 30, 6, _L8("~{VP9z~}xyz~{K}K{~}~~~{HK~}~~"), _L8("~~~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 31, 36, 5, _L8("~{VP9z~}xyz~{K}K{~}~~~{HK~}~~~~"), _L8("~{H}~}e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 37, 37, 4, _L8("~{VP9z~}xyz~{K}K{~}~~~{HK~}~~~~~{H}~}"), _L8("e~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 38, 39, 3, _L8("~{VP9z~}xyz~{K}K{~}~~~{HK~}~~~~~{H}~}e"), _L8("~~a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 40, 40, 2, _L8("~{VP9z~}xyz~{K}K{~}~~~{HK~}~~~~~{H}~}e~~"), _L8("a~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 41, 46, 1, _L8("~{VP9z~}xyz~{K}K{~}~~~{HK~}~~~~~{H}~}e~~a"), _L8("~{Q@~}"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 47, 50, 0, _L8("~{VP9z~}xyz~{K}K{~}~~~{HK~}~~~~~{H}~}e~~a~{Q@~}"), KNullDesC8, originalUnicode);
	INFO_PRINTF1(_L("Testing converting to Unicode"));
	const TPtrC8 originalHz(_S8("My name is ~{3W~} ~~~~{~~I ~{'X'Z'S'e~} in ~{11>)~}~~~~}~~~~"));
	TBuf16<50> expectedUnicode;
	expectedUnicode.Format(_L16("My name is %c ~~{~I %c%c%c%c in %c%c~~}~~"), 0x5319, 0x0436, 0x0438, 0x0432, 0x0443, 0x5317, 0x4eac);
	TestTruncatedConversionToUnicodeFromHz(*characterSetConverter, expectedUnicode, originalHz);
	TestTruncatedConversionToUnicodeFromHz(*characterSetConverter, _L16("Hello"), _L8("~{~}~{~}~{~}Hello"));
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 0, 0, 60, CCnvCharacterSetConverter::KStateDefault, 0, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 1, 1, 59, CCnvCharacterSetConverter::KStateDefault, 1, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 2, 2, 58, CCnvCharacterSetConverter::KStateDefault, 2, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 3, 3, 57, CCnvCharacterSetConverter::KStateDefault, 3, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 4, 4, 56, CCnvCharacterSetConverter::KStateDefault, 4, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 5, 5, 55, CCnvCharacterSetConverter::KStateDefault, 5, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 6, 6, 54, CCnvCharacterSetConverter::KStateDefault, 6, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 7, 7, 53, CCnvCharacterSetConverter::KStateDefault, 7, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 8, 8, 52, CCnvCharacterSetConverter::KStateDefault, 8, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 9, 9, 51, CCnvCharacterSetConverter::KStateDefault, 9, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 10, 10, 50, CCnvCharacterSetConverter::KStateDefault, 10, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 11, 11, 47, CCnvCharacterSetConverter::KStateDefault+1, 11, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 12, 12, 43, CCnvCharacterSetConverter::KStateDefault, 12, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 13, 13, 42, CCnvCharacterSetConverter::KStateDefault, 13, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 14, 14, 40, CCnvCharacterSetConverter::KStateDefault, 14, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 15, 15, 38, CCnvCharacterSetConverter::KStateDefault, 15, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 16, 16, 37, CCnvCharacterSetConverter::KStateDefault, 16, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 17, 17, 35, CCnvCharacterSetConverter::KStateDefault, 17, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 18, 18, 34, CCnvCharacterSetConverter::KStateDefault, 18, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 19, 19, 31, CCnvCharacterSetConverter::KStateDefault+1, 19, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 20, 20, 29, CCnvCharacterSetConverter::KStateDefault+1, 20, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 21, 21, 27, CCnvCharacterSetConverter::KStateDefault+1, 21, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 22, 22, 25, CCnvCharacterSetConverter::KStateDefault+1, 22, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 23, 23, 21, CCnvCharacterSetConverter::KStateDefault, 23, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 24, 24, 20, CCnvCharacterSetConverter::KStateDefault, 24, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 25, 25, 19, CCnvCharacterSetConverter::KStateDefault, 25, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 26, 26, 18, CCnvCharacterSetConverter::KStateDefault, 26, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 27, 27, 15, CCnvCharacterSetConverter::KStateDefault+1, 27, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 28, 28, 13, CCnvCharacterSetConverter::KStateDefault+1, 28, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 29, 29, 9, CCnvCharacterSetConverter::KStateDefault, 29, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 30, 30, 7, CCnvCharacterSetConverter::KStateDefault, 30, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 31, 31, 5, CCnvCharacterSetConverter::KStateDefault, 31, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 32, 32, 4, CCnvCharacterSetConverter::KStateDefault, 32, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 33, 33, 2, CCnvCharacterSetConverter::KStateDefault, 33, expectedUnicode, originalHz);
	TestSplittingConvertingToUnicodeFromHz(*characterSetConverter, 34, 40, 0, CCnvCharacterSetConverter::KStateDefault, 34, expectedUnicode, originalHz);
	INFO_PRINTF1(_L("Having '~' as the second byte of a Chinese character"));
	originalUnicode.Format(_L16("1%c2"), 0x4e8c);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 0, 0, 3, KNullDesC8, _L8("1~{6~~}2"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 1, 6, 2, _L8("1"), _L8("~{6~~}2"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 7, 7, 1, _L8("1~{6~~}"), _L8("2"), originalUnicode);
	TestSplittingConvertingFromUnicodeToHz(*characterSetConverter, 8, 20, 0, _L8("1~{6~~}2"), KNullDesC8, originalUnicode);
	INFO_PRINTF1(_L("Testing ill-formed HZ"));
	TestIsIllFormedHz(*characterSetConverter, _L8("abc ~{C~} def"));
	TestIsIllFormedHz(*characterSetConverter, _L8("abc ~{C;C~} def"));
	TestIsIllFormedHz(*characterSetConverter, _L8("abc ~{C;C;C~} def"));
	TestIsIllFormedHz(*characterSetConverter, _L8("abc ~"));
	TestIsIllFormedHz(*characterSetConverter, _L8("abc ~{~"));
	TestIsIllFormedHz(*characterSetConverter, _L8("abc ~{C"));
	// note that...................................._L8("abc ~{C~") is actually well-formed
	TestIsIllFormedHz(*characterSetConverter, _L8("abc ~{C;~"));
	CleanupStack::PopAndDestroy(3); // arrayOfCharacterSetsAvailable and characterSetConverter and fileServerSession
	}

CT_HZ::CT_HZ()
    {
    SetTestStepName(KTestStep_T_HZ);
    }

TVerdict CT_HZ::doTestStepL()
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


