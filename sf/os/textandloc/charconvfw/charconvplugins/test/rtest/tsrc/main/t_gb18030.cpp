/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "t_gb18030.h"
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

const TInt KMibValue = 114;
const TInt KBufferLength=100;

void CT_GB18030::TestTruncatedConversionFromUnicodeToGb18030(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode)
	{
	for (TInt i=aOriginalUnicode.Length(); i>=0; --i)
		{
		TBuf8<KBufferLength> generatedGb18030;
		const TInt returnValue=aCharacterSetConverter.ConvertFromUnicode(generatedGb18030, aOriginalUnicode.Left(i));
		test(returnValue>=0);
		TBuf8<KBufferLength> generatedsecondPartOfGb18030;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedsecondPartOfGb18030, aOriginalUnicode.Mid(i-returnValue))==0);
		generatedGb18030.Append(generatedsecondPartOfGb18030);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedGb18030, state)==0);
		test(generatedUnicode==aOriginalUnicode);
		}
	}

void CT_GB18030::TestSplittingConvertingFromUnicodeToGb18030(CCnvCharacterSetConverter& aCharacterSetConverter,
		TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit,
		TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfGb18030,
		const TDesC8& aExpectedGb18030, const TDesC16& aOriginalUnicode)
	{
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint8 gb18030Buffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr8 generatedFirstPartOfGb18030(gb18030Buffer, i);
		test(aCharacterSetConverter.ConvertFromUnicode(generatedFirstPartOfGb18030, aOriginalUnicode)==aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit);
		test(generatedFirstPartOfGb18030==aExpectedGb18030.Left(aExpectedLengthOfFirstPartOfGb18030));
		TBuf8<KBufferLength> generatedSecondPartOfGb18030;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfGb18030, aOriginalUnicode.Right(aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit))==0);
		test(generatedSecondPartOfGb18030==aExpectedGb18030.Mid(aExpectedLengthOfFirstPartOfGb18030));
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedFirstPartOfGb18030, state)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, generatedSecondPartOfGb18030, state)==0);
		test(state==CCnvCharacterSetConverter::KStateDefault);
		generatedUnicode.Append(generatedSecondPartOfUnicode);
		test(generatedUnicode==aOriginalUnicode);
		}
	}

void CT_GB18030::TestTruncatedConversionToUnicodeFromGb18030(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalGb18030)
	{
	for (TInt i=aOriginalGb18030.Length(); i>=4; --i) // 4 is the length of GB18030's longest multi-byte characters
		{
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		TBuf16<KBufferLength> generatedUnicode;
		const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, aOriginalGb18030.Left(i), state);
		test(returnValue>=0);
		TBuf16<KBufferLength> generatedsecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedsecondPartOfUnicode, aOriginalGb18030.Mid(i-returnValue), state)==0);
		generatedUnicode.Append(generatedsecondPartOfUnicode);
		test(generatedUnicode==aExpectedUnicode);
		}
	}

void CT_GB18030::TestSplittingConvertingToUnicodeFromGb18030(CCnvCharacterSetConverter& aCharacterSetConverter,
        
				TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit,
				TInt aExpectedNumberOfGb18030BytesNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode,
				const TDesC16& aExpectedUnicode, const TDesC8& aOriginalGb18030)
	{
	test(aMaximumLengthLowerLimit<=aMaximumLengthUpperLimit);
	test(aMaximumLengthUpperLimit<=KBufferLength);
	TUint16 unicodeBuffer[KBufferLength];
	for (TInt i=aMaximumLengthLowerLimit; i<=aMaximumLengthUpperLimit; ++i)
		{
		TPtr16 generatedFirstPartOfUnicode(unicodeBuffer, i);
		TInt state=CCnvCharacterSetConverter::KStateDefault;
		test(aCharacterSetConverter.ConvertToUnicode(generatedFirstPartOfUnicode, aOriginalGb18030, state)==aExpectedNumberOfGb18030BytesNotConvertedAtSplit);
		test(generatedFirstPartOfUnicode==aExpectedUnicode.Left(aExpectedLengthOfFirstPartOfUnicode));
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf16<KBufferLength> generatedSecondPartOfUnicode;
		test(aCharacterSetConverter.ConvertToUnicode(generatedSecondPartOfUnicode, aOriginalGb18030.Right(aExpectedNumberOfGb18030BytesNotConvertedAtSplit), state)==0);
		test(generatedSecondPartOfUnicode==aExpectedUnicode.Mid(aExpectedLengthOfFirstPartOfUnicode));
		test(state==CCnvCharacterSetConverter::KStateDefault);
		TBuf8<KBufferLength> generatedGb18030;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedGb18030, generatedFirstPartOfUnicode)==0);
		TBuf8<KBufferLength> generatedSecondPartOfGb18030;
		test(aCharacterSetConverter.ConvertFromUnicode(generatedSecondPartOfGb18030, generatedSecondPartOfUnicode)==0);
		generatedGb18030.Append(generatedSecondPartOfGb18030);
		test(generatedGb18030==aOriginalGb18030);
		}
	}


void CT_GB18030::CharacterSetValueAndMIBTests(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFileServerSession)
{
	// check that the character set value of '114' is converted to the GB18030 UID (0x10287038)
	test(aCharacterSetConverter.ConvertMibEnumOfCharacterSetToIdentifierL(KMibValue,aFileServerSession)==KCharacterSetIdentifierGb18030);
	INFO_PRINTF1(_L("\nMIB->Char Set UID - OK"));

	// check that the UCS2 GUID (0x10287038) is converted to the character set value of '114'
	test(aCharacterSetConverter.ConvertCharacterSetIdentifierToMibEnumL(KCharacterSetIdentifierGb18030,aFileServerSession)==KMibValue);
	INFO_PRINTF1(_L("\nChar Set UID->MIB - OK"));
}


/**
*/
void CT_GB18030::TestConversionToUnicodeFromGb18030(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalGb18030, TInt aExpectedResult)
	{
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TBuf16<KBufferLength> generatedUnicode;
	const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(generatedUnicode, aOriginalGb18030, state);
	test(returnValue == aExpectedResult );
	test(generatedUnicode==aExpectedUnicode);
	}


/**
*/
void CT_GB18030::TestConversionFromUnicodeToGb18030(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode, TInt aExpectedResult)
	{
	TBuf8<KBufferLength> generatedGb18030;
	const TInt returnValue=aCharacterSetConverter.ConvertFromUnicode(generatedGb18030, aOriginalUnicode);
	test(returnValue == aExpectedResult);
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	TBuf16<KBufferLength> generatedUnicode;
	test(aCharacterSetConverter.ConvertToUnicode(generatedUnicode, generatedGb18030, state)==0);
	test(generatedUnicode==aOriginalUnicode);
	}

/**
@SYMTestCaseID          TI18N-CHARCONV-CT-4051
@SYMTestCaseDesc        Check GB18030 plugin support  conversion between one byte GB18030 character and Unicode
@SYMTestPriority        High
@SYMTestActions         1. Select characters from one-byte character set of GB2312-80
						2. Pass it to charconv and convert it to Unicode
						3. Pass the above Unicode to charconv and convert it to GB18030
@SYMTestExpectedResults Conversion is successful, and the returned Unicode/GB18030 is as defined
@SYMREQ                 REQ12065 PREQ12066
*/

void CT_GB18030::OneByteConversion( CCnvCharacterSetConverter& aCharacterSetConverter )
{
	TPtrC8 originalGb18030;
	TBuf16<KBufferLength> originalUnicode;

	//	One-byte gb
	originalGb18030.Set(_L8("\x40"));
	originalUnicode.Format(_L16("%c"), 0x40);

	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030);
	
	//	one-byte uincode
	_LIT16(Uni_0, "\x0000");
	_LIT8(Gb_0, "\x00");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_0);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_0, Gb_0);

	_LIT16(Uni_1, "\x0079");
	_LIT8(Gb_1, "\x79");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_1);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_1, Gb_1);

	_LIT16(Uni_2, "\x0080");
	_LIT8(Gb_2, "\x81\x30\x81\x30");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_2);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_2, Gb_2);

	_LIT16(Uni_3, "\x0081");
	_LIT8(Gb_3, "\x81\x30\x81\x31");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_3);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_3, Gb_3);
	
	_LIT16(Uni_4, "\x00fe");
	_LIT8(Gb_4, "\x81\x30\x8B\x36");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_4);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_4, Gb_4);

	_LIT16(Uni_5, "\x00ff");
	_LIT8(Gb_5, "\x81\x30\x8B\x37");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_5);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_5, Gb_5);
}


/**
@SYMTestCaseID          TI18N-CHARCONV-CT-4052
@SYMTestCaseDesc        Check GB18030 plug-in support  conversion between two-byte GB18030 character and Unicode
@SYMTestPriority        High
@SYMTestActions         1. Select characters from two-byte character set from GB2312-80
						2. Pass it to charconv and convert it to Unicode
						3. Pass the above Unicode to charconv and convert it to GB18030
						4. Select characters from two-byte characters set from GBK, outside of GB2312-80, same with GBK
						5. Pass to charconv and convert to Unicode
						6. Pass the above Unicode to charconv, and conver it to GB18030
						7. Select characters from two-byte characters set from GBK, outside of GB2312-80, different from GBK
						8. Pass to charconv and convert to Unicode
						9. Pass the above Unicode to charconv, and conver it to GB18030
@SYMTestExpectedResults Conversion is successful, and the returned Unicode/GB18030 is as defined
@SYMREQ                 REQ12065 REQ12066
*/

void CT_GB18030::TwoByteConversion( CCnvCharacterSetConverter& aCharacterSetConverter )
{
	TPtrC8 originalGb18030;
	TBuf16<KBufferLength> originalUnicode;

	//	two-byte gb
	originalGb18030.Set(_L8("\xec\xe1"));
	originalUnicode.Format(_L16("%c"), 0x706c);
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030);

	originalGb18030.Set(_L8("\x81\x81"));
	originalUnicode.Format(_L16("%c"), 0x4e96);
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030);

	originalGb18030.Set(_L8("\xa2\xe3"));
	originalUnicode.Format(_L16("%c"), 0x20ac);
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030);
	originalGb18030.Set(_L8("\xa6\xd9"));
	originalUnicode.Format(_L16("%c"), 0xe78d);
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030);
	originalGb18030.Set(_L8("\xa8\xbc"));
	originalUnicode.Format(_L16("%c"), 0x1e3f);
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030);
	
	
	//	two-byte unicode
	_LIT16(Uni_6, "\x0100");
	_LIT8(Gb_6, "\x81\x30\x8B\x38");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_6);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_6, Gb_6);

	_LIT16(Uni_7, "\x0101");
	_LIT8(Gb_7, "\xA8\xA1");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_7);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_7, Gb_7);

	_LIT16(Uni_8, "\x0ffe");
	_LIT8(Gb_8, "\x81\x33\x83\x38");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_8);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_8, Gb_8);

	_LIT16(Uni_9, "\x0fff");
	_LIT8(Gb_9, "\x81\x33\x83\x39");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_9);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_9, Gb_9);

	_LIT16(Uni_10, "\x1000");
	_LIT8(Gb_10, "\x81\x33\x84\x30");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_10);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_10, Gb_10);

	_LIT16(Uni_11, "\x1001");
	_LIT8(Gb_11, "\x81\x33\x84\x31");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_11);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_11, Gb_11);

	_LIT16(Uni_12, "\xfffe");
	_LIT8(Gb_12, "\x84\x31\xA4\x38");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_12);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_12, Gb_12);

	_LIT16(Uni_13, "\xffff");
	_LIT8(Gb_13, "\x84\x31\xA4\x39");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_13);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_13, Gb_13);	
}

/**
@SYMTestCaseID          TI18N-CHARCONV-CT-4056
@SYMTestCaseDesc        Check GB18030 plug-in support  conversion between four-byte GB18030 character and Unicode
@SYMTestPriority        High
@SYMTestActions         1. Select characters from CJK A
						2. Pass it to charconv and convert it to Unicode 
						3. Pass the above Unicode to charconv and convert it to GB18030
						4. Select characters from CJK B
						5. Pass it to charconv and convert it to Unicode 
						6. Pass the above Unicode to charconv and convert it to GB18030
@SYMTestExpectedResults Conversion is successful, and the returned Unicode/GB18030 is as defined
@SYMREQ                 REQ12065 REQ12066
*/

void CT_GB18030::FourByteConversion( CCnvCharacterSetConverter& aCharacterSetConverter )
{
	TPtrC8 originalGb18030;
	TBuf16<KBufferLength> originalUnicode;

	//	four-byte gb
	originalGb18030.Set(_L8("\x82\x30\xA7\x30"));
	originalUnicode.Format(_L16("%c"), 0x3622);
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030);

	originalGb18030.Set(_L8("\x95\x32\xad\x35"));
	originalUnicode.Format(_L16("%c%c"), 0xd840, 0xddad );//0x201ad)
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030);
	
	//	four-byte unicode
	_LIT16(Uni_14, "\xd840\xdc00");
	_LIT8(gb_14, "\x95\x32\x82\x36");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_14);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_14, gb_14);

	_LIT16(Uni_15, "\xd840\xdc01");
	_LIT8(gb_15, "\x95\x32\x82\x37");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_15);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_15, gb_15);
	
	_LIT16(Uni_16, "\xD87F\xdffe");
	_LIT8(gb_16, "\x9a\x34\x84\x30");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_16);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_16, gb_16);

	_LIT16(Uni_17, "\xD87F\xdfff");
	_LIT8(gb_17, "\x9a\x34\x84\x31");
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, Uni_17);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, Uni_17, gb_17);
}


/**
@SYMTestCaseID          TI18N-CHARCONV-CT-4054
@SYMTestCaseDesc        Check GB18030 plug-in support  conversion between one-byte GB18030 character and Unicode, negative input 
@SYMTestPriority        High
@SYMTestActions         1. Select one-byte characters which are not mapped to Unicode
						2. Pass it to charconv and convert it to Unicode 
						3. Select one-byte code point which is not valid in GB18030
						4. Pass it to charconv and convert it to Unicode 
@SYMTestExpectedResults No side effect
@SYMREQ                 REQ12065 REQ12066
*/

void CT_GB18030::OneByteConversionNegative( CCnvCharacterSetConverter& aCharacterSetConverter )
{
	TPtrC8 originalGb18030;
	TBuf16<KBufferLength> originalUnicode;

	originalGb18030.Set(_L8("\x0e"));
	originalUnicode.Format(_L16("%c"), 0x0e);
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030);

	originalGb18030.Set(_L8("\xa0"));
	originalUnicode.Copy( _L("") );
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030, CCnvCharacterSetConverter::EErrorIllFormedInput );
}


/**
@SYMTestCaseID          TI18N-CHARCONV-CT-4055
@SYMTestCaseDesc        Check GB18030 plug-in support  conversion between two-byte GB18030 character and Unicode, negative input 
@SYMTestPriority        High
@SYMTestActions         1. Select two-byte characters which are not mapped to Unicode
						2. Pass it to charconv and convert it to Unicode 
						3. Select two-byte code points which are not valid in GB18030
						4. Pass it to charconv and convert it to Unicode
@SYMTestExpectedResults No side effect
@SYMREQ                 REQ12065 REQ12066
*/

void CT_GB18030::TwoByteConversionNegative( CCnvCharacterSetConverter& aCharacterSetConverter )
{
	TPtrC8 originalGb18030;
	TBuf16<KBufferLength> originalUnicode;

	originalGb18030.Set(_L8("\xa0\x7f"));
	originalUnicode.Copy( _L("") );
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030, CCnvCharacterSetConverter::EErrorIllFormedInput );
}


/**
@SYMTestCaseID          TI18N-CHARCONV-CT-4056
@SYMTestCaseDesc        Check GB18030 plug-in support  conversion between four-byte GB18030 character and Unicode, negative input 
@SYMTestPriority        High
@SYMTestActions         1. Select four-byte characters which are not mapped to Unicode
						2. Pass it to charconv and convert it to Unicode 
						3. Select four-byte code points which are not valid in GB18030
						4. Pass it to charconv and convert it to Unicode
@SYMTestExpectedResults No side effect
@SYMREQ                 REQ12065 REQ12066
*/

void CT_GB18030::FourByteConversionNegative( CCnvCharacterSetConverter& aCharacterSetConverter )
{
	TPtrC8 originalGb18030;
	TBuf16<KBufferLength> originalUnicode;

	originalGb18030.Set(_L8("\x81\x30\x81\x3a"));
	originalUnicode.Copy( _L("") );
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030, CCnvCharacterSetConverter::EErrorIllFormedInput );
}


/**
@SYMTestCaseID          TI18N-CHARCONV-CT-4053
@SYMTestCaseDesc        Check GB18030 plug-in support  conversion between problematic GB18030 character and Unicode, negative input 
@SYMTestPriority        High
@SYMTestActions         1. Select characters whose trailing byte is in the range of leading bytes
						2. Pass it to charconv and convert it to Unicode 
						3. Select Unicode whose leading/trailing byte is in the range of low-ANSI 
						4. Pass it to charconv and convert it to GB18030
						5. Select characters whose trailing byte is special symbol like backslash
						6. Pass it to charconv and convert it to Unicode
@SYMTestExpectedResults No side effect
@SYMREQ                 REQ12065 REQ12066
*/

void CT_GB18030::ProblemProneCharcters( CCnvCharacterSetConverter& aCharacterSetConverter )
{
	TPtrC8 originalGb18030;
	TBuf16<KBufferLength> originalUnicode;

	originalGb18030.Set(_L8("\x81\x81\xba\xba\xa0\xa0\xf7\xf7"));
	originalUnicode.Format(_L16("%c%c%c%c"), 0x4e96, 0x6c49, 0x724b, 0x9f22);
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030 );
	
	originalGb18030.Set(_L8("\xc4\x40\x83\xa0\xa0\x7c\xa0\x86"));
	originalUnicode.Format(_L16("%c%c%c%c"), 0x8140, 0x512c, 0x7218, 0x7222);
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030 );

	originalGb18030.Set(_L8("\x81\x43\x81\x63\xf7\x4d\xf7\x6d\xa0\x53\xa0\x73"));
	originalUnicode.Format(_L16("%c%c%c%c%c%c"), 0x4E06, 0x4E67, 0x9C49, 0x9C69, 0x71EC, 0x720F);
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030 );

	originalGb18030.Set(_L8("\x9f\xaa\x81\xae\xf7\xbf\xbf\xaa\xb0\x5b\xb1\x5b\xb1\x5c\xb2\x5c\xb2\x5d\xc3\x5d\xd4\x5f\xe5\x5f\xd6\x7b\xe7\x7b\xF7\x7C\xFD\x7C\xAA\x7D\xFE\x7D"));
	originalUnicode.Format(_L16("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"), 0x716A, 0x4F04, 0x9ABA, 0x5F00, 0x7667, 0x76F5, 0x76F6, 0x7788, 0x7789, 0x80C5, 0x8a3D, 0x9329, 0x8B20, 0x940A, 0x9C78, 0x9F76, 0x7347, 0x464c);
	TestConversionFromUnicodeToGb18030(aCharacterSetConverter, originalUnicode);
	TestConversionToUnicodeFromGb18030(aCharacterSetConverter, originalUnicode, originalGb18030 );
}


void CT_GB18030::DoE32MainL()
	{
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
	TPtrC8 originalGb18030;
	TBuf16<KBufferLength> originalUnicode;
	TBuf8<KBufferLength> generatedGb18030;
	TBuf16<KBufferLength> generatedUnicode;
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	
	// test that we can get MIB and Charset values
	CharacterSetValueAndMIBTests(*characterSetConverter, fileServerSession);

	characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierGb18030, *arrayOfCharacterSetsAvailable, fileServerSession);

	INFO_PRINTF1( _L("GB18030 one byte support") );
	OneByteConversion( *characterSetConverter );
	
	INFO_PRINTF1( _L("GB18030 two byte support") );
	TwoByteConversion( *characterSetConverter );
	
	INFO_PRINTF1( _L("GB18030 four byte support") );
	FourByteConversion( *characterSetConverter );
	
	INFO_PRINTF1( _L("GB18030 one byte support with negative input") );
	OneByteConversionNegative( *characterSetConverter );

	INFO_PRINTF1( _L("GB18030 two byte support with negative input") );
	TwoByteConversionNegative( *characterSetConverter );
	
	INFO_PRINTF1( _L("GB18030 four byte support with negative input") );
	FourByteConversionNegative( *characterSetConverter );
	
	INFO_PRINTF1( _L("Problem prone characters") );
	ProblemProneCharcters( *characterSetConverter );
	
	INFO_PRINTF1(_L("Testing characters shared with GB 2312-80 and characters only in GBK"));

	originalGb18030.Set(_L8("A\xfd\x7d\xdd\xb6\xb1\xc9\xe9\x8e\xe8\x9d""E b\xd3\x59\xd2\x40\x95\xc0"));		// 20 byte, 12 char
	originalUnicode.Format(_L16("A%c%c%c%c%cE b%c%c%c"), 0x9f77, 0x837b, 0x9119, 0x95b9, 0x94e6, 0x89bb, 0x8938, 0x66b2);
	TestTruncatedConversionFromUnicodeToGb18030(*characterSetConverter, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 0, 0, 12, 0, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 1, 2, 11, 1, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 3, 4, 10, 3, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 5, 6, 9, 5, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 7, 8, 8, 7, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 9, 10, 7, 9, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 11, 11, 6, 11, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 12, 12, 5, 12, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 13, 13, 4, 13, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 14, 15, 3, 14, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 16, 17, 2, 16, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 18, 19, 1, 18, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 20, 40, 0, 20, originalGb18030, originalUnicode);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode, originalGb18030);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(1, 2), originalGb18030.Mid(1, 4));
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 0, 0, 20, 0, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 1, 1, 19, 1, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 2, 2, 17, 2, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 3, 3, 15, 3, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 4, 4, 13, 4, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 5, 5, 11, 5, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 6, 6, 9, 6, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 7, 7, 8, 7, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 8, 8, 7, 8, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 9, 9, 6, 9, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 10, 10, 4, 10, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 11, 11, 2, 11, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 12, 30, 0, 12, originalUnicode, originalGb18030);

	INFO_PRINTF1(_L("Testing GB18030 characters where the first byte has the high-bit set and the second byte doesn't"));
	originalGb18030.Set(_L8("\x20\x5d\xa0\x5d\xa0\xdd"));
	originalUnicode.Format(_L16(" ]%c%c"), 0x71f7, 0x72a6);
	TestTruncatedConversionFromUnicodeToGb18030(*characterSetConverter, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 0, 0, 4, 0, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 1, 1, 3, 1, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 2, 3, 2, 2, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 4, 5, 1, 4, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 6, 20, 0, 6, originalGb18030, originalUnicode);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 0, 0, 6, 0, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 1, 1, 5, 1, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 2, 2, 4, 2, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 3, 3, 2, 3, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 4, 20, 0, 4, originalUnicode, originalGb18030);

	INFO_PRINTF1(_L("Testing truncated GB18030 sequences"));
	originalGb18030.Set(_L8("qwe\xb5"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, originalGb18030, state)==1);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==_L16("qwe"));

	originalGb18030.Set(_L8("qwe\x81\x30"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, originalGb18030, state)==2);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==_L16("qwe"));

	INFO_PRINTF1(_L("Testing 4 byte characters, including surrogate pair"));
	originalGb18030.Set(_L8("C\x81\x30\x81\x30\x82\x30\x81\x30\x95\x32\x82\x36"));		// 13 byte
	originalUnicode.Format(_L16("C%c%c%c%c"), 0x0080, 0x34A3, 0xD840, 0xDC00);			// 4 Char (3 UCS2, 1 surrogate pair)
	TestTruncatedConversionFromUnicodeToGb18030(*characterSetConverter, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 0, 0, 5, 0, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 1, 4, 4, 1, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 5, 8, 3, 5, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 9, 12, 2, 9, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 13, 40, 0, 13, originalGb18030, originalUnicode);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode, originalGb18030);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(1, 2), originalGb18030.Mid(1, 8));
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(3, 2), originalGb18030.Mid(9, 4));
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 0, 0, 13, 0, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 1, 1, 12, 1, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 2, 2, 8, 2, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 3, 4, 4, 3, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 5, 30, 0, 5, originalUnicode, originalGb18030);
	
	INFO_PRINTF1(_L("Testing GB18030 characters which have different mapping with GB2312-80"));
	originalGb18030.Set(_L8("\xa1\xaa\xa1\xa4\xa8\x44\x81\x39\xa7\x39"));		// 10 byte
	originalUnicode.Format(_L16("%c%c%c%c"), 0x2014, 0x00B7, 0x2015, 0x30FB);			// 4 char (UCS2)
	TestTruncatedConversionFromUnicodeToGb18030(*characterSetConverter, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 0, 1, 4, 0, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 2, 3, 3, 2, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 4, 5, 2, 4, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 6, 9, 1, 6, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 10, 20, 0, 10, originalGb18030, originalUnicode);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode, originalGb18030);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(0, 2), originalGb18030.Mid(0, 4));
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(2, 2), originalGb18030.Mid(4, 6));
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 0, 0, 10, 0, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 1, 1, 8, 1, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 2, 2, 6, 2, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 3, 3, 4, 3, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 4, 10, 0, 4, originalUnicode, originalGb18030);
	
	INFO_PRINTF1(_L("Testing 2 byte GB18030 characters which have different mapping with GBK"));
	originalGb18030.Set(_L8("\xa1\xa4\xa1\xaa\xa8\xbc\xa8\xbf\xa9\x8a"));		// 10 byte
	originalUnicode.Format(_L16("%c%c%c%c%c"), 0x00b7, 0x2014, 0x1e3f, 0x01f9, 0x2ff0);			// 5 char (UCS2)
	TestTruncatedConversionFromUnicodeToGb18030(*characterSetConverter, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 0, 1, 5, 0, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 2, 3, 4, 2, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 4, 5, 3, 4, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 10, 20, 0, 10, originalGb18030, originalUnicode);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode, originalGb18030);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(0, 2), originalGb18030.Mid(0, 4));
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(2, 3), originalGb18030.Mid(4, 6));
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 0, 0, 10, 0, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 1, 1, 8, 1, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 2, 2, 6, 2, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 5, 10, 0, 5, originalUnicode, originalGb18030);
	
	INFO_PRINTF1(_L("Testing 2/4 byte GB18030 characters mapping to PUA codes"));
	originalGb18030.Set(_L8("\xFE\xFE\xFE\xD4\xFD\xE8\xA7\xFE\xA3\xEB\x84\x31\xA4\x39\x83\x36\xF0\x37\xA3\xB1"));		// 20 byte
	originalUnicode.Format(_L16("%c%c%c%c%c%c%c%c"), 0xE4C5, 0xE49B, 0xE451, 0xE7BB, 0xFF4B, 0xFFFF, 0xE9AC, 0xFF11);		// 8 char (UCS2)
	TestTruncatedConversionFromUnicodeToGb18030(*characterSetConverter, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 0, 1, 8, 0, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 2, 3, 7, 2, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 4, 5, 6, 4, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 10, 13, 3, 10, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 14, 17, 2, 14, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 18, 19, 1, 18, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 20, 40, 0, 20, originalGb18030, originalUnicode);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode, originalGb18030);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(0, 2), originalGb18030.Mid(0, 4));
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(2, 3), originalGb18030.Mid(4, 6));
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(5, 2), originalGb18030.Mid(10, 8));
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(7, 1), originalGb18030.Mid(18, 2));
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 0, 0, 20, 0, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 1, 1, 18, 1, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 2, 2, 16, 2, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 6, 6, 6, 6, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 7, 7, 2, 7, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 8, 10, 0, 8, originalUnicode, originalGb18030);	
	
	INFO_PRINTF1(_L("Testing combined GB18030 characters including 1/2/4 byte"));
	originalGb18030.Set(_L8("A\x95\x32\x82\x36\x32\x81\x40\xC2\xB7\x8E\xEB\x95\x33\x96\x30\x81\x37\x81\x30\xFE\xF1")); //22 bytes
	originalUnicode.Format(_L16("A%c%c%c%c%c%c%c%c%c%c"), 0xD840, 0xDC00, 0x0032, 0x4E02, 0x8DEF, 0x5EAA, 0xD841, 0xDDAE, 0x23EC, 0xE4B8 ); //11 chars (UCS2)
	TestTruncatedConversionFromUnicodeToGb18030(*characterSetConverter, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 0, 0, 11, 0, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 1, 4, 10, 1, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 5, 5, 8, 5, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 6, 6, 7, 6, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 8, 9, 6, 8, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 10, 11, 5, 10, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 12, 15, 4, 12, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 16, 19, 2, 16, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 20, 21, 1, 20, originalGb18030, originalUnicode);
	TestSplittingConvertingFromUnicodeToGb18030(*characterSetConverter, 22, 30, 0, 22, originalGb18030, originalUnicode);
	
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode, originalGb18030);
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(0, 1), originalGb18030.Mid(0, 1));
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(1, 2), originalGb18030.Mid(1, 4));
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(3, 1), originalGb18030.Mid(5, 1));
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(4, 3), originalGb18030.Mid(6, 6));
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(7, 3), originalGb18030.Mid(12, 8));
	TestTruncatedConversionToUnicodeFromGb18030(*characterSetConverter, originalUnicode.Mid(10, 1), originalGb18030.Mid(20, 2));
	
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 0, 0, 22, 0, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 1, 2, 21, 1, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 3, 3, 17, 3, originalUnicode, originalGb18030);
	
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 4, 4, 16, 4, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 7, 8, 10, 7, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 9, 9, 6, 9, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 10, 10, 2, 10, originalUnicode, originalGb18030);
	TestSplittingConvertingToUnicodeFromGb18030(*characterSetConverter, 11, 20, 0, 11, originalUnicode, originalGb18030);		
	
	CleanupStack::PopAndDestroy(3); // arrayOfCharacterSetsAvailable and characterSetConverter and fileServerSession
	}

CT_GB18030::CT_GB18030()
    {
    SetTestStepName(KTestStep_T_GB18030);
    }

TVerdict CT_GB18030::doTestStepL()
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
