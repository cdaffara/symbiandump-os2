/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <utf.h>
#include "t_java.h"

#define test(cond)                                          \
    {                                                       \
    TBool __bb = (cond);                                    \
    TEST(__bb);                                             \
    if (!__bb)                                              \
        {                                                   \
        ERR_PRINTF1(_L("ERROR: Test Failed"));              \
        User::Leave(1);                                     \
        }                                                   \
    }

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0571
@SYMTestCaseDesc        CnvUtfConverter class functionality test
@SYMTestPriority        Medium
@SYMTestActions        	Checking that Java-conformant UTF-8 is generated and handled correctly
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_JAVA::Test_JAVA()
	{
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0571 Checking that Java-conformant UTF-8 is generated and handled correctly "));
	test(characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierJavaConformantUtf8, fileServerSession)==CCnvCharacterSetConverter::EAvailable);
	TBuf16<32> unicode;
	TBuf16<32> generatedUnicode;
	TBuf8<64> generatedUtf8;
	TInt convertToUnicodeState = CCnvCharacterSetConverter::KStateDefault;

	//
	INFO_PRINTF1(_L("Testing the behaviour of the Unicode character 0x0000"));

	unicode.Format(_L16("%c"), 0x0000);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==TPtrC8(REINTERPRET_CAST(const TText8*, "\x00"), 1)); // can't use _L8 as that gets the length by assuming it's a zero-terminated string
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xc0\x80"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode==unicode);

	unicode.Format(_L16("%c%c"), 0x0000, 0x0026);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==TPtrC8(REINTERPRET_CAST(const TText8*, "\x00\x26"), 2)); // can't use _L8 as that gets the length by assuming it's a zero-terminated string
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xc0\x80\x26"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode==unicode);

	unicode.Format(_L16("%c%c"), 0x0000, 0x05d9);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==TPtrC8(REINTERPRET_CAST(const TText8*, "\x00\xd7\x99"), 3)); // can't use _L8 as that gets the length by assuming it's a zero-terminated string
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xc0\x80\xd7\x99"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode==unicode);

	unicode.Format(_L16("%c%c"), 0x0000, 0x81ea);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==TPtrC8(REINTERPRET_CAST(const TText8*, "\x00\xe8\x87\xaa"), 4)); // can't use _L8 as that gets the length by assuming it's a zero-terminated string
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xc0\x80\xe8\x87\xaa"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode==unicode);

	unicode.Format(_L16("%c%c%c"), 0x0000, 0xd9e0, 0xdda3);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==TPtrC8(REINTERPRET_CAST(const TText8*, "\x00\xf2\x88\x86\xa3"), 5)); // can't use _L8 as that gets the length by assuming it's a zero-terminated string
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xc0\x80\xed\xa7\xa0\xed\xb6\xa3"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==7);
	test(generatedUnicode[0]==0x0000);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);
	test(generatedUnicode[6]==0xFFFD);

	unicode.Format(_L16("%c%c"), 0x0071, 0x0000);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==TPtrC8(REINTERPRET_CAST(const TText8*, "\x71\x00"), 2)); // can't use _L8 as that gets the length by assuming it's a zero-terminated string
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\x71\xc0\x80"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode==unicode);

	unicode.Format(_L16("%c%c"), 0x06aa, 0x0000);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==TPtrC8(REINTERPRET_CAST(const TText8*, "\xda\xaa\x00"), 3)); // can't use _L8 as that gets the length by assuming it's a zero-terminated string
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xda\xaa\xc0\x80"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode==unicode);

	unicode.Format(_L16("%c%c"), 0xc427, 0x0000);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==TPtrC8(REINTERPRET_CAST(const TText8*, "\xec\x90\xa7\x00"), 4)); // can't use _L8 as that gets the length by assuming it's a zero-terminated string
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xec\x90\xa7\xc0\x80"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode==unicode);

	unicode.Format(_L16("%c%c%c"), 0xd801, 0xdef1, 0x0000);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==TPtrC8(REINTERPRET_CAST(const TText8*, "\xf0\x90\x9b\xb1\x00"), 5)); // can't use _L8 as that gets the length by assuming it's a zero-terminated string
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xa0\x81\xed\xbb\xb1\xc0\x80"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==7);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);
	test(generatedUnicode[6]==0x0000);

	INFO_PRINTF1(_L("Testing the generation of UTF-8 from surrogate pairs"));
	unicode.Format(_L16("%c"), 0xd7ff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\x9f\xbf"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\x9f\xbf"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode==unicode);

	unicode.Format(_L16("%c"), 0xe000);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xee\x80\x80"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xee\x80\x80"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode==unicode);

	unicode.Format(_L16("%c%c"), 0xd800, 0xdc00);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf0\x90\x80\x80"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xa0\x80\xed\xb0\x80"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==6);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);

	unicode.Format(_L16("%c%c"), 0xd800, 0xdfff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf0\x90\x8f\xbf"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xa0\x80\xed\xbf\xbf"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==6);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);

	unicode.Format(_L16("%c%c"), 0xdbff, 0xdc00);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf4\x8f\xb0\x80"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xaf\xbf\xed\xb0\x80"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==6);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);

	unicode.Format(_L16("%c%c"), 0xdbff, 0xdfff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf4\x8f\xbf\xbf"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xaf\xbf\xed\xbf\xbf"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==6);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);

	unicode.Format(_L16("%c%c"), 0xd8e9, 0xdcd4);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf1\x8a\x93\x94"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xa3\xa9\xed\xb3\x94"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==6);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);

	unicode.Format(_L16("%c%c%c"), 0xd8e9, 0xdcd4, 0x003e);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf1\x8a\x93\x94\x3e"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xa3\xa9\xed\xb3\x94\x3e"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==7);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);
	test(generatedUnicode[6]==0x003e);

	unicode.Format(_L16("%c%c%c"), 0xd8e9, 0xdcd4, 0x01e9);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf1\x8a\x93\x94\xc7\xa9"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xa3\xa9\xed\xb3\x94\xc7\xa9"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==7);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);
	test(generatedUnicode[6]==0x01e9);

	unicode.Format(_L16("%c%c%c"), 0xd8e9, 0xdcd4, 0x1f9a);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xf1\x8a\x93\x94\xe1\xbe\x9a"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xed\xa3\xa9\xed\xb3\x94\xe1\xbe\x9a"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==7);
	test(generatedUnicode[0]==0xFFFD);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);
	test(generatedUnicode[6]==0x1f9a);

	unicode.Format(_L16("%c%c%c"), 0x0025, 0xd8e9, 0xdcd4);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\x25\xf1\x8a\x93\x94"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\x25\xed\xa3\xa9\xed\xb3\x94"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==7);
	test(generatedUnicode[0]==0x0025);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);
	test(generatedUnicode[6]==0xFFFD);

	unicode.Format(_L16("%c%c%c"), 0x00a5, 0xd8e9, 0xdcd4);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xc2\xa5\xf1\x8a\x93\x94"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xc2\xa5\xed\xa3\xa9\xed\xb3\x94"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==7);
	test(generatedUnicode[0]==0x00a5);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);
	test(generatedUnicode[6]==0xFFFD);

	unicode.Format(_L16("%c%c%c"), 0x0ca6, 0xd8e9, 0xdcd4);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xe0\xb2\xa6\xf1\x8a\x93\x94"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==unicode);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf8, unicode)==0);
	test(generatedUtf8==_L8("\xe0\xb2\xa6\xed\xa3\xa9\xed\xb3\x94"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==0);
	test(generatedUnicode.Length()==7);
	test(generatedUnicode[0]==0x0ca6);
	test(generatedUnicode[1]==0xFFFD);
	test(generatedUnicode[2]==0xFFFD);
	test(generatedUnicode[3]==0xFFFD);
	test(generatedUnicode[4]==0xFFFD);
	test(generatedUnicode[5]==0xFFFD);
	test(generatedUnicode[6]==0xFFFD);

	// Additional tests added for INC117111...

	generatedUtf8.Format(_L8("\xe0\xb2\xa6\xc0"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==1);
	test(generatedUnicode[0]==0x0ca6);

	generatedUtf8.Format(_L8("\xc0"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf8, convertToUnicodeState)==CnvUtfConverter::EErrorIllFormedInput);

	CleanupStack::PopAndDestroy(2); // characterSetConverter and fileServerSession
	}


CT_JAVA::CT_JAVA()
    {
    SetTestStepName(KTestStep_T_JAVA);
    }

TVerdict CT_JAVA::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, Test_JAVA());

    __UHEAP_MARKEND;

    if(error1 == KErrNone )
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
