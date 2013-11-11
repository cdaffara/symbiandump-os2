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
#include "shiftjis.h"
#include <utf.h>
#include "t_shiftjis_docomo.h"

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
@SYMTestCaseID          SYSLIB-CHARCONV-UT-1903
@SYMTestCaseDesc        Tests for truncated conversion from Unicode to ShiftJIS
@SYMTestPriority        Medium
@SYMTestActions         Tests for truncated conversion from Unicode to ShiftJIS and back to Unicode
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_SHIFTJIS_DOCOMO::TestTruncatedConversionFromUnicodeToShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode, const TDesC16& aUnicodeAfterRoundTrip)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-1903 "));
	for (TInt i=aOriginalUnicode.Length(); i>=0; --i)
		{
		TBuf8<KBufferLength> generatedShiftJis;
		const TInt returnValue=aCharacterSetConverter.ConvertFromUnicode(generatedShiftJis, aOriginalUnicode.Left(i));
		test(returnValue>=0);
		TBuf8<KBufferLength> generatedsecondPartOfShiftJis;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedsecondPartOfShiftJis, aOriginalUnicode.Mid(i-returnValue))==0);
		generatedShiftJis.Append(generatedsecondPartOfShiftJis);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedShiftJis, state)==0);
		test(generatedUnicode==aUnicodeAfterRoundTrip);
		}
	}

void CT_SHIFTJIS_DOCOMO::TestTruncatedConversionFromUnicodeToShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode)
	{
	TestTruncatedConversionFromUnicodeToShiftJis(aCharacterSetConverter, aOriginalUnicode, aOriginalUnicode);
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-UT-1904
@SYMTestCaseDesc        Splitting and converting from Unicode to ShiftJIS test
@SYMTestPriority        Medium
@SYMTestActions         Tests for conversion after splitting from Unicode to ShiftJIS and back to Unicode
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_SHIFTJIS_DOCOMO::TestSplittingConvertingFromUnicodeToShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfShiftJis, const TDesC8& aExpectedShiftJis, const TDesC16& aOriginalUnicode, const TDesC16& aUnicodeAfterRoundTrip)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-1904 "));
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint8 shiftJisBuffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr8 generatedFirstPartOfShiftJis(shiftJisBuffer, i);
		TInt unconverted = aCharacterSetConverter.ConvertFromUnicode(generatedFirstPartOfShiftJis, aOriginalUnicode) ;
		test(unconverted == aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit);
		test(generatedFirstPartOfShiftJis==aExpectedShiftJis.Left(aExpectedLengthOfFirstPartOfShiftJis));
		TBuf8<KBufferLength> generatedSecondPartOfShiftJis;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfShiftJis, aOriginalUnicode.Right(aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit))==0);
		test(generatedSecondPartOfShiftJis==aExpectedShiftJis.Mid(aExpectedLengthOfFirstPartOfShiftJis));
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedFirstPartOfShiftJis, state)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, generatedSecondPartOfShiftJis, state)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		generatedUnicode.Append(generatedSecondPartOfUnicode);
		test(generatedUnicode==aUnicodeAfterRoundTrip);
		}
	}

void CT_SHIFTJIS_DOCOMO::TestSplittingConvertingFromUnicodeToShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfShiftJis, const TDesC8& aExpectedShiftJis, const TDesC16& aOriginalUnicode)
	{
	TestSplittingConvertingFromUnicodeToShiftJis(aCharacterSetConverter, aMaximumLengthLowerLimit, aMaximumLengthUpperLimit, aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, aExpectedLengthOfFirstPartOfShiftJis, aExpectedShiftJis, aOriginalUnicode, aOriginalUnicode);
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-UT-1905
@SYMTestCaseDesc        Tests for truncated conversion from ShiftJIS to Unicode
@SYMTestPriority        Medium
@SYMTestActions         Tests for truncated conversion from ShiftJIS to Unicode and back to ShiftJIS
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_SHIFTJIS_DOCOMO::TestTruncatedConversionToUnicodeFromShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalShiftJis)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-1905 "));
	for (TInt i=aOriginalShiftJis.Length(); i>=2; --i) // 2 is the length of GBK's longest multi-byte characters
		{
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, aOriginalShiftJis.Left(i), state);
		test(returnValue>=0);
		TBuf16<KBufferLength> generatedsecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedsecondPartOfUnicode, aOriginalShiftJis.Mid(i-returnValue), state)==0);
		generatedUnicode.Append(generatedsecondPartOfUnicode);
		test(generatedUnicode==aExpectedUnicode);
		}
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-UT-1906
@SYMTestCaseDesc        Splitting and converting from ShiftJIS to Unicode test
@SYMTestPriority        Medium
@SYMTestActions         Tests for conversion after splitting from ShiftJIS to Unicode and back to ShiftJIS
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_SHIFTJIS_DOCOMO::TestSplittingConvertingToUnicodeFromShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfShiftJisBytesNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalShiftJis)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-1906 "));
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint16 unicodeBuffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr16 generatedFirstPartOfUnicode(unicodeBuffer, i);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		test(aCharacterSetConverter.ConvertToUnicode(generatedFirstPartOfUnicode, aOriginalShiftJis, state)==aExpectedNumberOfShiftJisBytesNotConvertedAtSplit);
		test(generatedFirstPartOfUnicode==aExpectedUnicode.Left(aExpectedLengthOfFirstPartOfUnicode));
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, aOriginalShiftJis.Right(aExpectedNumberOfShiftJisBytesNotConvertedAtSplit), state)==0);
		test(generatedSecondPartOfUnicode==aExpectedUnicode.Mid(aExpectedLengthOfFirstPartOfUnicode));
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf8<KBufferLength> generatedShiftJis;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedShiftJis, generatedFirstPartOfUnicode)==0);
		TBuf8<KBufferLength> generatedSecondPartOfShiftJis;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfShiftJis, generatedSecondPartOfUnicode)==0);
		generatedShiftJis.Append(generatedSecondPartOfShiftJis);
		test(generatedShiftJis==aOriginalShiftJis);
		}
	}
/**
@SYMTestCaseID          SYSLIB-CHARCONV-UT-1907
@SYMTestCaseDesc        Conversion of bad ShiftJIS format to Unicode test
@SYMTestPriority        Medium
@SYMTestActions         Tests for converting bad format ShiftJIS
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_SHIFTJIS_DOCOMO::TestIsIllFormedShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aShiftJis)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-1907 "));
	TBuf16<50> generatedUnicode;
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TPtrC8 remainderOfShiftJis(aShiftJis);
	TInt lastReturnValue=KMaxTInt;
	FOREVER
		{
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, remainderOfShiftJis, state);
		if (returnValue==CCnvCharacterSetConverter::EErrorIllFormedInput)
			{
			break;
			}
		test(returnValue>0);
		test(returnValue<lastReturnValue);
		lastReturnValue=returnValue;
		remainderOfShiftJis.Set(remainderOfShiftJis.Right(returnValue));
		}
	}

/**
@SYMTestCaseID          SYSLIB-CHARCONV-UT-3611
@SYMTestCaseDesc        Tests conversion of ShiftJIS text longer than 0xFFFF
@SYMTestPriority        High
@SYMTestActions         Attempts to convert the first part the text passed in and verifies that
						at least some characters were converted and the remaining text length is as expected.
@SYMTestExpectedResults The first 20 characters should be converted and the remaining text length
						should be the original length - the number of chars converted
@SYMDEF                 INC110495
*/
void CT_SHIFTJIS_DOCOMO::DoTestConvertingLargeText(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aShiftJisText)
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-3611 "));

	TBuf16<20> outputBuffer;
	TPtrC8 remainderOfForeignText(aShiftJisText);

	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TInt returnValue = aCharacterSetConverter.ConvertToUnicode(outputBuffer,
													remainderOfForeignText,
													state);
	TInt charsProcessed = outputBuffer.Length();
	TInt charsToConvert = aShiftJisText.Length();
	test(charsProcessed == 20);
	test(returnValue == (charsToConvert - charsProcessed));
	}

void CT_SHIFTJIS_DOCOMO::TestConvertingLargeText(CCnvCharacterSetConverter& aCharacterSetConverter)
	{

	const TInt size = 0x10000;

	HBufC8* sjisBuf = HBufC8::NewL(size);
	TPtr8 sjisPtr = sjisBuf->Des();
	sjisPtr.Fill('a', sjisPtr.MaxLength());

	for(TInt i=0xFFFF; i<=size; i++)
		{
		sjisPtr.SetLength(i);
		DoTestConvertingLargeText(aCharacterSetConverter,sjisPtr);
		}

	delete sjisBuf;
	}


/**
@SYMTestCaseID          SYSLIB-CHARCONV-UT-1908
@SYMTestCaseDesc        ShiftJIS to Unicode and Unicode to ShiftJIS conversion tests
@SYMTestPriority        Medium
@SYMTestActions         Executes conversion tests of Unicode to ShiftJIS
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_SHIFTJIS_DOCOMO::DoE32MainL()
	{
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-UT-1908 Testing fix for defect EDNDBAR-4MCKCP in \"Symbian Defect Tracking v3.0\""));
	TBuf16<50> originalUnicode;
	TBuf8<50> generatedShiftjis;
	originalUnicode.Format(_L16("%c%c%c%ca"), 0x9999, 0x2029, 0xff61, 0x2028);
	CCnvCharacterSetConverter::TArrayOfAscendingIndices indicesOfUnconvertibleCharacters;
	INFO_PRINTF1(_L("Calling CnvShiftJis::ConvertFromUnicode before CHARCONV.DLL's thread-local storage has been set"));
	test(CnvShiftJis::ConvertFromUnicode(CCnvCharacterSetConverter::EBigEndian, _L8("?"), generatedShiftjis, originalUnicode, indicesOfUnconvertibleCharacters)==0);
	test(generatedShiftjis==_L8("\x8d\x81?\xa1?a"));
	test(indicesOfUnconvertibleCharacters.NumberOfIndices()==2);
	test(indicesOfUnconvertibleCharacters[0]==1);
	test(indicesOfUnconvertibleCharacters[1]==3);
	indicesOfUnconvertibleCharacters.RemoveAll();
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	INFO_PRINTF1(_L("Calling CnvShiftJis::ConvertFromUnicode after CHARCONV.DLL's thread-local storage has been set"));
	test(CnvShiftJis::ConvertFromUnicode(CCnvCharacterSetConverter::EBigEndian, _L8("?"), generatedShiftjis, originalUnicode, indicesOfUnconvertibleCharacters)==0); // call this after the CCnvCharacterSetConverter object has been created (i.e. after CHARCONV.DLL's thread-local storage has been set)
	test(generatedShiftjis==_L8("\x8d\x81?\xa1?a"));
	test(indicesOfUnconvertibleCharacters.NumberOfIndices()==2);
	test(indicesOfUnconvertibleCharacters[0]==1);
	test(indicesOfUnconvertibleCharacters[1]==3);
	indicesOfUnconvertibleCharacters.RemoveAll();
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
	INFO_PRINTF1(_L("Calling generic API's equivalent of CnvShiftJis::ConvertFromUnicode"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierShiftJis, *arrayOfCharacterSetsAvailable, fileServerSession);
	test(characterSetConverter->ConvertFromUnicode(generatedShiftjis, originalUnicode, indicesOfUnconvertibleCharacters)==0);
	test(generatedShiftjis==_L8("\x8d\x81\x0d\x0a\xa1\x0d\x0a""a"));
	test(indicesOfUnconvertibleCharacters.NumberOfIndices()==0);
	characterSetConverter->SetDowngradeForExoticLineTerminatingCharacters(CCnvCharacterSetConverter::EDowngradeExoticLineTerminatingCharactersToJustLineFeed);
	test(characterSetConverter->ConvertFromUnicode(generatedShiftjis, originalUnicode, indicesOfUnconvertibleCharacters)==0);
	test(generatedShiftjis==_L8("\x8d\x81\x0a\xa1\x0a""a"));
	test(indicesOfUnconvertibleCharacters.NumberOfIndices()==0);

	INFO_PRINTF1(_L("Testing fix for defect SIN-4XHE36 in \"Symbian Defect Tracking v4.0\""));
	_LIT8(inputShiftjis,"This \x09is a\x0d\x0atest\x09to see if Charconv\x0d\x0a""can process\x0d\x0a""control\x09""characters.");
	_LIT8(roundTripShiftjis,"This \x09is a\x0atest\x09to see if Charconv\x0a""can process\x0a""control\x09""characters.");
	TBuf16<100> outUnicode;
	TInt sta=CCnvCharacterSetConverter::KStateDefault;
	TInt unConvertibleNum;
	test(characterSetConverter->ConvertToUnicode(outUnicode, inputShiftjis, sta,unConvertibleNum)==0);
	test(unConvertibleNum==0);
	TBuf8<100> outUtf8;
	TBuf16<100> outUnicode1;
	TBuf8<100> outShifJis;
	TBuf8<100> idealShiftJis(roundTripShiftjis);
	CnvUtfConverter::ConvertFromUnicodeToUtf8(outUtf8, outUnicode);
	CnvUtfConverter::ConvertToUnicodeFromUtf8(outUnicode1, outUtf8);
	characterSetConverter->ConvertFromUnicode(outShifJis, outUnicode1);
	test(idealShiftJis==outShifJis);

	INFO_PRINTF1(_L("Testing Shift-JIS conversions"));
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierAscii, *arrayOfCharacterSetsAvailable, fileServerSession);
	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierShiftJis, *arrayOfCharacterSetsAvailable, fileServerSession);
	INFO_PRINTF1(_L("Empty descriptor"));
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 0, 10, 0, 0, KNullDesC8, KNullDesC16);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 0, 10, 0, 0, KNullDesC16, KNullDesC8);
	INFO_PRINTF1(_L("Testing converting to Shift-JIS"));
	originalUnicode.Format(_L16("%c%c%c is %c"), 0xff72, 0xff74, 0xff7d, 0x795e);
	const TPtrC8 expectedShiftJis(_S8("\xb2\xb4\xbd is \x90\x5f"));
	TestTruncatedConversionFromUnicodeToShiftJis(*characterSetConverter, originalUnicode);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 0, 0, 8, 0, expectedShiftJis, originalUnicode);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 1, 1, 7, 1, expectedShiftJis, originalUnicode);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 2, 2, 6, 2, expectedShiftJis, originalUnicode);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 3, 3, 5, 3, expectedShiftJis, originalUnicode);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 4, 4, 4, 4, expectedShiftJis, originalUnicode);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 5, 5, 3, 5, expectedShiftJis, originalUnicode);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 6, 6, 2, 6, expectedShiftJis, originalUnicode);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 7, 8, 1, 7, expectedShiftJis, originalUnicode);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 9, 20, 0, 9, expectedShiftJis, originalUnicode);

	INFO_PRINTF1(_L("Testing converting to Unicode"));
	const TPtrC8 originalShiftJis(_S8("My name is \xc3\xde\xa8\xcb\xde\xc2\xde or \x83\x66\x83\x42\x83\x72\x83\x64 in \x93\xfa\x96\x7b\x8c\xea"));
	TBuf16<50> expectedUnicode;
	expectedUnicode.Format(_L16("My name is %c%c%c%c%c%c%c or %c%c%c%c in %c%c%c"), 0xff83, 0xff9e, 0xff68, 0xff8b, 0xff9e, 0xff82, 0xff9e, 0x30c7, 0x30a3, 0x30d3, 0x30c5, 0x65e5, 0x672c, 0x8a9e);
	TestTruncatedConversionToUnicodeFromShiftJis(*characterSetConverter, expectedUnicode, originalShiftJis);
	TestTruncatedConversionToUnicodeFromShiftJis(*characterSetConverter, expectedUnicode.Right(3), originalShiftJis.Right(6));
	for (i=0; i<=21; ++i)
		{
		TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, i, i, 40-i, i, expectedUnicode, originalShiftJis);
		}
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 22, 22, 18, 22, expectedUnicode, originalShiftJis);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 23, 23, 16, 23, expectedUnicode, originalShiftJis);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 24, 24, 14, 24, expectedUnicode, originalShiftJis);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 25, 25, 12, 25, expectedUnicode, originalShiftJis);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 26, 26, 10, 26, expectedUnicode, originalShiftJis);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 27, 27, 9, 27, expectedUnicode, originalShiftJis);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 28, 28, 8, 28, expectedUnicode, originalShiftJis);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 29, 29, 7, 29, expectedUnicode, originalShiftJis);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 30, 30, 6, 30, expectedUnicode, originalShiftJis);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 31, 31, 4, 31, expectedUnicode, originalShiftJis);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 32, 32, 2, 32, expectedUnicode, originalShiftJis);
	TestSplittingConvertingToUnicodeFromShiftJis(*characterSetConverter, 33, 33, 0, 33, expectedUnicode, originalShiftJis);
	INFO_PRINTF1(_L("Testing ill-formed Shift-JIS"));
	TestIsIllFormedShiftJis(*characterSetConverter, _L8("abc \x83"));
	TestIsIllFormedShiftJis(*characterSetConverter, _L8("abc \x83\x41\xe7"));
	{
	INFO_PRINTF1(_L("Testing fix for defect EDNEMUA-4KGFEY in \"Symbian Defect Tracking v3.0\""));
	CCnvCharacterSetConverter* characterSetConverter2=CCnvCharacterSetConverter::NewLC();
	characterSetConverter2->PrepareToConvertToOrFromL(KCharacterSetIdentifierShiftJis, *arrayOfCharacterSetsAvailable, fileServerSession);
	characterSetConverter2->SetDowngradeForExoticLineTerminatingCharacters(CCnvCharacterSetConverter::EDowngradeExoticLineTerminatingCharactersToJustLineFeed);
	originalUnicode.Format(_L16("%c%c%cE%c%c%c%c%c%cE%c%c\x0d\x0a%c\x0a%c"), 0xff74, 0x9053, 0x2029, 0x9053, 0x2028, 0x9053, 0xff74, 0x2028, 0x9053, 0x2029, 0x9053, 0x9053, 0x9053);
	TBuf16<50> unicodeAfterRoundTrip2;
	unicodeAfterRoundTrip2.Format(_L16("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"), 0xff74, 0x9053, 0x000a, 0x0045, 0x9053, 0x000a, 0x9053, 0xff74, 0x000A, 0x9053, 0x0045, 0x000A, 0x9053, 0x2028, 0x9053, 0x000a,  0x9053);
	const TPtrC8 expectedShiftJis2(_S8("\xb4\x93\xb9\x0a""E\x93\xb9\x0a\x93\xb9\xb4\x0a\x93\xb9""E\x0a\x93\xb9\x0d\x0a\x93\xb9\x0a\x93\xb9"));
	TestTruncatedConversionFromUnicodeToShiftJis(*characterSetConverter2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 0, 0, 18, 0, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 1, 2, 17, 1, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 3, 3, 16, 3, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 4, 4, 15, 4, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 5, 6, 14, 5, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 7, 7, 13, 7, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 8, 9, 12, 8, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 10, 10, 11, 10, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 11, 11, 10, 11, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 12, 13, 9, 12, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 14, 14, 8, 14, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 15, 15, 7, 15, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 16, 17, 6, 16, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 18, 18, 5, 18, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);

	// At the request of Symbian KK, where possible the Shift-JIS converter now converts <cr><lf> sequences in Shift-JIS input to a single unicode
	// line feed. With the current CharConv API we can't do this in the case where converting input text in 2 parts results in a <cr><lf> pair being,
	// split and you'll get a unicode <cr><lf> pair.

	// Split at 18 characters will cause a problem as it splits a <cr><lf> sequence.
//	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 19, 19, 4, 19, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 20, 21, 3, 20, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 22, 22, 2, 22, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 23, 24, 1, 23, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter2, 25, 40, 0, 25, expectedShiftJis2, originalUnicode, unicodeAfterRoundTrip2);
	TBuf16<50> unicodeAfterRoundTrip1;
	unicodeAfterRoundTrip1.Format(_L16("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"), 0xff74, 0x9053, 0x2028, 0x0045, 0x9053, 0x2028, 0x9053, 0xff74, 0x2028, 0x9053, 0x0045, 0x2028, 0x9053, 0x2028, 0x9053, 0x000a,  0x9053);
	const TPtrC8 expectedShiftJis1(_S8("\xb4\x93\xb9\x0d\x0a""E\x93\xb9\x0d\x0a\x93\xb9\xb4\x0d\x0a\x93\xb9""E\x0d\x0a\x93\xb9\x0d\x0a\x93\xb9\x0a\x93\xb9"));
	TestTruncatedConversionFromUnicodeToShiftJis(*characterSetConverter, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 0, 0, 18, 0, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 1, 2, 17, 1, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 3, 4, 16, 3, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 5, 5, 15, 5, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 6, 7, 14, 6, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 8, 9, 13, 8, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 10, 11, 12, 10, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 12, 12, 11, 12, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 13, 14, 10, 13, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 15, 16, 9, 15, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 17, 17, 8, 17, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 18, 19, 7, 18, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 20, 21, 6, 20, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 22, 22, 5, 22, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);

	// Split at 23 characters will cause a problem as it splits a <cr><lf> sequence.
//	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 23, 23, 4, 23, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 24, 25, 3, 24, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 26, 26, 2, 26, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 27, 28, 1, 27, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	TestSplittingConvertingFromUnicodeToShiftJis(*characterSetConverter, 29, 40, 0, 29, expectedShiftJis1, originalUnicode, unicodeAfterRoundTrip1);
	CleanupStack::PopAndDestroy(characterSetConverter2);
	}

	//Test converting text longer than 0xFFFF
	INFO_PRINTF1(_L("Testing fix for defect INC110495"));
	CCnvCharacterSetConverter* characterSetConverter3 = CCnvCharacterSetConverter::NewLC();
	characterSetConverter3->PrepareToConvertToOrFromL( KCharacterSetIdentifierShiftJis, fileServerSession);

	TestConvertingLargeText(*characterSetConverter3);
	CleanupStack::PopAndDestroy(characterSetConverter3);

	// Test to see if the default replaceable character is 0x20, which it should be for the
	// docomo shiftjis converter.

	INFO_PRINTF1(_L("Testing fix for defect INC110965"));

		{
		CCnvCharacterSetConverter* characterSetConverter4 = CCnvCharacterSetConverter::NewLC();
		characterSetConverter4->PrepareToConvertToOrFromL(KCharacterSetIdentifierShiftJis, *arrayOfCharacterSetsAvailable, fileServerSession);
		characterSetConverter4->SetDefaultEndiannessOfForeignCharacters(CCnvCharacterSetConverter::EBigEndian);

		TBuf16<50> originalUnicode1;
		TBuf8<50> generatedShiftjis1;

		// test code - [ ur, ur, 0x7b, 0x5b ]
		originalUnicode1.Format(_L16("%c%c%c%c"), 0x00a5, 0x00e8, 0x007b, 0x005b);

		test(characterSetConverter4->ConvertFromUnicode(generatedShiftjis1,originalUnicode1,indicesOfUnconvertibleCharacters)==0);
		test(generatedShiftjis1==_L8("\x20\x20\x7b\x5b"));

		CleanupStack::PopAndDestroy(characterSetConverter4);
		}

	CleanupStack::PopAndDestroy(3);
	}

CT_SHIFTJIS_DOCOMO::CT_SHIFTJIS_DOCOMO()
    {
    SetTestStepName(KTestStep_T_SHIFTJIS_DOCOMO);
    }

TVerdict CT_SHIFTJIS_DOCOMO::doTestStepL()
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

