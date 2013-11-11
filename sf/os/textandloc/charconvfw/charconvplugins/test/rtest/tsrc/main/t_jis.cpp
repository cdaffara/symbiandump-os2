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
#include "t_jis.h"

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
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0529
@SYMTestCaseDesc        Tests for truncated conversion from Unicode to JIS
@SYMTestPriority        Medium
@SYMTestActions         Tests for truncated conversion from Unicode to JIS and back to Unicode
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_JIS::TestTruncatedConversionFromUnicodeToJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0529 "));
	for (TInt i=aOriginalUnicode.Length(); i>=0; --i)
		{
		TBuf8<KBufferLength> generatedJis;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedJis, aOriginalUnicode.Left(i))>=0);
		TBuf8<KBufferLength> generatedsecondPartOfJis;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedsecondPartOfJis, aOriginalUnicode.Mid(i))==0);
		generatedJis.Append(generatedsecondPartOfJis);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedJis, state)==0);
		test(generatedUnicode==aOriginalUnicode);
		}
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0530
@SYMTestCaseDesc        Splitting and converting from Unicode to JIS test
@SYMTestPriority        Medium
@SYMTestActions         Tests for conversion after splitting, from Unicode to JIS and back to Unicode
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_JIS::TestSplittingConvertingFromUnicodeToJis(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, const TDesC8& aExpectedFirstPartOfJis, const TDesC8& aExpectedSecondPartOfJis, const TDesC16& aOriginalUnicode)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0530 "));
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint8 jisBuffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr8 generatedFirstPartOfJis(jisBuffer, i);
		test(aCharacterSetConverter.ConvertFromUnicode(generatedFirstPartOfJis, aOriginalUnicode)==aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit);
		test(generatedFirstPartOfJis==aExpectedFirstPartOfJis);
		TBuf8<KBufferLength> generatedSecondPartOfJis;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfJis, aOriginalUnicode.Right(aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit))==0);
		test(generatedSecondPartOfJis==aExpectedSecondPartOfJis);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedFirstPartOfJis, state)==0);
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, generatedSecondPartOfJis, state)==0);
		generatedUnicode.Append(generatedSecondPartOfUnicode);
		test(generatedUnicode==aOriginalUnicode);
		}
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0531
@SYMTestCaseDesc        Tests for truncated conversion from JIS to Unicode
@SYMTestPriority        Medium
@SYMTestActions         Tests for truncated conversion from JIS to Unicode and back to JIS
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_JIS::TestTruncatedConversionToUnicodeFromJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalJis)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0531 "));
	for (TInt i=aOriginalJis.Length(); i>=6; --i) // 6 is the length of JIS' longest escape sequence
		{
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, aOriginalJis.Left(i), state);
		test(returnValue>=0);
		TBuf16<KBufferLength> generatedsecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedsecondPartOfUnicode, aOriginalJis.Mid(i-returnValue), state)==0);
		generatedUnicode.Append(generatedsecondPartOfUnicode);
		test(generatedUnicode==aExpectedUnicode);
		}
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0532
@SYMTestCaseDesc        Splitting and converting from JIS to Unicode test
@SYMTestPriority        Medium
@SYMTestActions         Tests for conversion after splitting, from JIS to Unicode and back to JIS
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_JIS::TestSplittingConvertingToUnicodeFromJis(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfJisBytesNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalJis)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0532 "));
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint16 unicodeBuffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr16 generatedFirstPartOfUnicode(unicodeBuffer, i);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedFirstPartOfUnicode, aOriginalJis, state);
		test(generatedFirstPartOfUnicode==aExpectedUnicode.Left(aExpectedLengthOfFirstPartOfUnicode));
		test(returnValue==aExpectedNumberOfJisBytesNotConvertedAtSplit);
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, aOriginalJis.Right(aExpectedNumberOfJisBytesNotConvertedAtSplit), state)==0);
		test(generatedSecondPartOfUnicode==aExpectedUnicode.Mid(aExpectedLengthOfFirstPartOfUnicode));
		TBuf8<KBufferLength> generatedJis;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedJis, generatedFirstPartOfUnicode)==0);
		TBuf8<KBufferLength> generatedSecondPartOfJis;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfJis, generatedSecondPartOfUnicode)==0);
		generatedJis.Append(generatedSecondPartOfJis);
		TBuf16<KBufferLength> regeneratedUnicode;
		state=CCnvCharacterSetConverter::KStateDefault;
		test(aCharacterSetConverter.ConvertToUnicode(regeneratedUnicode, generatedJis, state)==0);
		test(regeneratedUnicode==aExpectedUnicode);
		state=CCnvCharacterSetConverter::KStateDefault;
		test(aCharacterSetConverter.ConvertToUnicode(regeneratedUnicode, aOriginalJis, state)==0);
		test(regeneratedUnicode==aExpectedUnicode);
		}
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0533
@SYMTestCaseDesc        Conversion of bad JIS format to Unicode test
@SYMTestPriority        Medium
@SYMTestActions         Tests to convert bad formatted JIS input to Unicode.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_JIS::TestIsIllFormedJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aJis)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0533 "));
	TBuf16<50> generatedUnicode;
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TPtrC8 remainderOfJis(aJis);
	TInt lastReturnValue=KMaxTInt;
	FOREVER
		{
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, remainderOfJis, state);
		if (returnValue==CCnvCharacterSetConverter::EErrorIllFormedInput)
			{
			break;
			}
		test(returnValue>0);
		test(returnValue<lastReturnValue);
		lastReturnValue=returnValue;
		remainderOfJis.Set(remainderOfJis.Right(returnValue));
		}
	}

/**
* Utility for DEF063276 fix.
*/

_LIT(KOriginalJisFilename, "z:\\test\\data\\originalJis.dat");

void CT_JIS::ReadDesc(TDes8& aDes, const TDesC& aFilename, RFs& aFs)
	{
	RFile file;
	TInt err = file.Open(aFs, aFilename, EFileRead);
	test(err == KErrNone);
	CleanupClosePushL(file);

	err = file.Read(aDes);
	test(err == KErrNone);
	CleanupStack::PopAndDestroy(&file);
	}

/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0534
@SYMTestCaseDesc        JIS to Unicode and Unicode to JIS conversion tests
@SYMTestPriority        Medium
@SYMTestActions         Calls up all conversion test functions from JIS to Unicode
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

void CT_JIS::DoE32MainL()
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0534 "));
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
	INFO_PRINTF1(_L("Testing JIS conversions"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierJis, *arrayOfCharacterSetsAvailable, fileServerSession);
	//
	INFO_PRINTF1(_L("Empty descriptor"));
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 0, 10, 0, KNullDesC8, KNullDesC8, KNullDesC16);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 0, 10, 0, 0, KNullDesC16, KNullDesC8);
	INFO_PRINTF1(_L("Testing converting to JIS"));
	TBuf16<50> originalUnicode;
	originalUnicode.Format(_L16("I %c%c%c%c%c%c%c%c"), 0x611b, 0x3059, 0x308b, 0xff74, 0xff9a, 0xff68, 0xff9d, 0x4e04);
	TestTruncatedConversionFromUnicodeToJis(*characterSetConverter, originalUnicode);
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 0, 3, 10, KNullDesC8, _L8("\x1b\x28\x4aI \x1b\x24\x42\x30\x26\x24\x39\x24\x6b\x1b\x28\x49\x34\x5a\x28\x5d\x1b\x24\x28\x44\x30\x22"), originalUnicode);
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 4, 4, 9, _L8("\x1b\x28\x4aI"), _L8("\x1b\x28\x4a \x1b\x24\x42\x30\x26\x24\x39\x24\x6b\x1b\x28\x49\x34\x5a\x28\x5d\x1b\x24\x28\x44\x30\x22"), originalUnicode);
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 5, 9, 8, _L8("\x1b\x28\x4aI "), _L8("\x1b\x24\x42\x30\x26\x24\x39\x24\x6b\x1b\x28\x49\x34\x5a\x28\x5d\x1b\x24\x28\x44\x30\x22"), originalUnicode);
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 10, 11, 7, _L8("\x1b\x28\x4aI \x1b\x24\x42\x30\x26"), _L8("\x1b\x24\x42\x24\x39\x24\x6b\x1b\x28\x49\x34\x5a\x28\x5d\x1b\x24\x28\x44\x30\x22"), originalUnicode);
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 12, 13, 6, _L8("\x1b\x28\x4aI \x1b\x24\x42\x30\x26\x24\x39"), _L8("\x1b\x24\x42\x24\x6b\x1b\x28\x49\x34\x5a\x28\x5d\x1b\x24\x28\x44\x30\x22"), originalUnicode);
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 14, 17, 5, _L8("\x1b\x28\x4aI \x1b\x24\x42\x30\x26\x24\x39\x24\x6b"), _L8("\x1b\x28\x49\x34\x5a\x28\x5d\x1b\x24\x28\x44\x30\x22"), originalUnicode);
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 18, 18, 4, _L8("\x1b\x28\x4aI \x1b\x24\x42\x30\x26\x24\x39\x24\x6b\x1b\x28\x49\x34"), _L8("\x1b\x28\x49\x5a\x28\x5d\x1b\x24\x28\x44\x30\x22"), originalUnicode);
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 19, 19, 3, _L8("\x1b\x28\x4aI \x1b\x24\x42\x30\x26\x24\x39\x24\x6b\x1b\x28\x49\x34\x5a"), _L8("\x1b\x28\x49\x28\x5d\x1b\x24\x28\x44\x30\x22"), originalUnicode);
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 20, 20, 2, _L8("\x1b\x28\x4aI \x1b\x24\x42\x30\x26\x24\x39\x24\x6b\x1b\x28\x49\x34\x5a\x28"), _L8("\x1b\x28\x49\x5d\x1b\x24\x28\x44\x30\x22"), originalUnicode);
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 21, 26, 1, _L8("\x1b\x28\x4aI \x1b\x24\x42\x30\x26\x24\x39\x24\x6b\x1b\x28\x49\x34\x5a\x28\x5d"), _L8("\x1b\x24\x28\x44\x30\x22"), originalUnicode);
	TestSplittingConvertingFromUnicodeToJis(*characterSetConverter, 27, 40, 0, _L8("\x1b\x28\x4aI \x1b\x24\x42\x30\x26\x24\x39\x24\x6b\x1b\x28\x49\x34\x5a\x28\x5d\x1b\x24\x28\x44\x30\x22"), KNullDesC8, originalUnicode);
	INFO_PRINTF1(_L("Testing converting to Unicode"));

	/**
	* Work around for DEF063276.
	* This literal is now loaded from a z:\test\data\originalJis.dat
	* Bullseye Coverage corrupts this literal to avoid this it is stored in a file as to not be touched by Bullseye Coverage.
	*/
	// const TPtrC8 originalJis(_S8("\\\x1b\x28\x42\\\xb4\\\x0e\x31\x0f\\\x0e\x4a\x5e\x1b\x26\x40\x1b\x24\x42\x30\x24\x1b\x24\x28\x44\x30\x24\x1b\x28\x49\x21\x0e\x22\x0f\x30\x24\x0e"));

	TBuf8<64> buf;
	ReadDesc(buf, KOriginalJisFilename, fileServerSession);

	TBuf16<50> expectedUnicode;
	expectedUnicode.Format(_L16("\xa5\\%c\xa5%c\xa5%c%c%c%c%c%c0$"), 0xff74, 0xff71, 0xff8a, 0xff9e, 0x963f, 0x4e0c, 0xff61, 0xff62);
	TestTruncatedConversionToUnicodeFromJis(*characterSetConverter, expectedUnicode, buf);
	TestTruncatedConversionToUnicodeFromJis(*characterSetConverter, _L16(" Hello"), _L8("\x1b\x24\x42\x1b\x28\x4a\x1b\x24\x42\x1b\x28\x4a\x1b\x26\x40\x1b\x24\x42\x1b\x28\x4a Hello"));
	TestTruncatedConversionToUnicodeFromJis(*characterSetConverter, _L16(" Hello"), _L8("\x1b\x26\x40\x1b\x24\x42\x1b\x28\x4a Hello"));
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 0, 0, 38, 0, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 1, 1, 34, 1, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 2, 2, 33, 2, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 3, 3, 32, 3, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 4, 4, 30, 4, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 5, 5, 28, 5, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 6, 6, 26, 6, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 7, 7, 25, 7, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 8, 8, 24, 8, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 9, 9, 12, 9, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 10, 10, 7, 10, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 11, 11, 5, 11, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 12, 12, 3, 12, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 13, 13, 2, 13, expectedUnicode, buf);
	TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, 14, 30, 0, 14, expectedUnicode, buf);

	// End fix
	INFO_PRINTF1(_L("Testing the default JIS state"));
	for (i=0; i<=6; ++i)
		{
		TestSplittingConvertingToUnicodeFromJis(*characterSetConverter, i, i, 6-i, i, _L16("Hello\xa5"), _L8("Hello\\"));
		}
	INFO_PRINTF1(_L("Testing ill-formed JIS"));
	TestIsIllFormedJis(*characterSetConverter, _L8("abc \x1b\x26\x40\x1b\x24\x42\x21\x1b\x28\x4a def"));
	TestIsIllFormedJis(*characterSetConverter, _L8("abc \x1b\x26\x40\x1b\x24\x42\x21\x21\x21\x1b\x28\x4a def"));
	TestIsIllFormedJis(*characterSetConverter, _L8("abc \x1b\x26\x40\x1b\x24\x42\x21\x21\x21\x21\x21\x1b\x28\x4a def"));
	TestIsIllFormedJis(*characterSetConverter, _L8("abc \x1b"));
	TestIsIllFormedJis(*characterSetConverter, _L8("abc \x1b\x24"));
	TestIsIllFormedJis(*characterSetConverter, _L8("abc \x1b\x24\xff"));
	CleanupStack::PopAndDestroy(3); // arrayOfCharacterSetsAvailable and characterSetConverter and fileServerSession
	}

CT_JIS::CT_JIS()
    {
    SetTestStepName(KTestStep_T_JIS);
    }

TVerdict CT_JIS::doTestStepL()
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
