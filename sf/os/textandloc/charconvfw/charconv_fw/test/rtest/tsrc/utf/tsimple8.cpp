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

#include <utf.h>
#include "t_simple8.h"
///////////////////////////////////////////////////////////////////////////////////////

#define test(cond)                                  \
    TEST((cond));                                   \
    if (!(cond))                                    \
        {                                           \
        ERR_PRINTF1(_L("ERROR: Test Failed"));      \
        User::Leave(1);                             \
        }

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-4002
@SYMTestCaseDesc        Test conversion of a UTF-8 BOM between Unicode and 8-bit 
                        encoding.
@SYMTestPriority        Medium
@SYMTestActions        	Tests for CnvUtfConverter::ConvertFromUnicodeToUtf8(),
                        CnvUtfConverter::ConvertToUnicodeFromUtf8L() function  
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
//INC118714L
void CT_SIMPLE8::TestForUtf8BomL()
	{
    INFO_PRINTF1(_L("@SYMTestCaseID:SYSLIB-CHARCONV-CT-4002 Testing converting to and from UTF-8 BOM.")); 	

	TBuf8<100> generatedUtf8NoGKBWithBom(_L8("\xef\xbb\xbf\x7F\x48\x65\x6c\x6c\x6f\x20\x57\x6f\x72\x6c\x64"));
	
	//convert to unicode and verify BOM converted properly.
	INFO_PRINTF1(_L("BOM, valid UTF-8 to Unicode conversion test."));
	TBuf16<256> generatedUnicode;
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8NoGKBWithBom)==0);
	test(generatedUnicode==_L16("\xFEFF\x007fHello World"));
   
	INFO_PRINTF1(_L("BOM, valid Unicode to UTF-8 conversion test."));
	TBuf8<256> generatedUtf8;
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, generatedUnicode)==0);
	test(generatedUtf8==generatedUtf8NoGKBWithBom);
	
	}


/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0563
@SYMTestCaseDesc        Conversion tests from Unicode to UTF-8 character set
@SYMTestPriority        Medium
@SYMTestActions        	Tests for CnvUtfConverter::ConvertFromUnicodeToUtf8(),
                        CnvUtfConverter::ConvertToUnicodeFromUtf8L() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_SIMPLE8::TestSimple8()
	{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0563 Testing simple UTF-8 round trips "));
	TBuf16<256> originalUnicode;
	TBuf8<256> generatedUtf8;
	TBuf16<256> generatedUnicode;
	//
	INFO_PRINTF1(_L("Empty descriptor"));
	originalUnicode=_L16("");
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, originalUnicode)==0);
	test(generatedUtf8==_L8(""));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==originalUnicode);
	// Testing the new API
	HBufC8* testbuffer8;
	HBufC* testbuffer;
	testbuffer8= CnvUtfConverter::ConvertFromUnicodeToUtf8L(originalUnicode);
	TPtr8 comparebuffer8 = testbuffer8->Des();
	test(comparebuffer8==generatedUtf8);
	testbuffer = CnvUtfConverter::ConvertToUnicodeFromUtf8L(generatedUtf8);
	TPtr compareBuffer = testbuffer->Des();
	test(compareBuffer == originalUnicode);
	delete testbuffer8;
	delete testbuffer;

	INFO_PRINTF1(_L("English \"Hello!\""));
	originalUnicode=_L16("Hello!");
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, originalUnicode)==0);
	test(generatedUtf8==_L8("Hello!"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==originalUnicode);
	// Testing the new API
	testbuffer8= CnvUtfConverter::ConvertFromUnicodeToUtf8L(originalUnicode);
	comparebuffer8.Set(testbuffer8->Des());
	test(comparebuffer8==generatedUtf8);
	testbuffer = CnvUtfConverter::ConvertToUnicodeFromUtf8L(generatedUtf8);
	compareBuffer.Set(testbuffer->Des());
	test(compareBuffer == originalUnicode);
	delete testbuffer8;
	delete testbuffer;

	INFO_PRINTF1(_L("Russian \"Hello!\""));
	originalUnicode.Format(_L16("%c%c%c%c%c%c%c%c%c%c%c%c%c"), 0x0417, 0x0434, 0x0440, 0x0430, 0x0432, 0x0441, 0x0442, 0x0432, 0x0443, 0x0439, 0x0442, 0x0435, 0x0021);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, originalUnicode)==0);
	test(generatedUtf8==_L8("\xd0\x97\xd0\xb4\xd1\x80\xd0\xb0\xd0\xb2\xd1\x81\xd1\x82\xd0\xb2\xd1\x83\xd0\xb9\xd1\x82\xd0\xb5\x21"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==originalUnicode);
	// Testing the new API
	testbuffer8= CnvUtfConverter::ConvertFromUnicodeToUtf8L(originalUnicode);
	comparebuffer8.Set(testbuffer8->Des());
	test(comparebuffer8==generatedUtf8);
	testbuffer = CnvUtfConverter::ConvertToUnicodeFromUtf8L(generatedUtf8);
	compareBuffer.Set(testbuffer->Des());
	test(compareBuffer == originalUnicode);
	delete testbuffer8;
	delete testbuffer;

	INFO_PRINTF1(_L("Greek \"Hello!\""));
	originalUnicode.Format(_L16("%c%c%c%c%c%c"), 0x0393, 0x03b1, 0x03c3, 0x03bf, 0x03c5, 0x0021);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, originalUnicode)==0);
	test(generatedUtf8==_L8("\xce\x93\xce\xb1\xcf\x83\xce\xbf\xcf\x85\x21"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==originalUnicode);
	// Testing the new API
	testbuffer8= CnvUtfConverter::ConvertFromUnicodeToUtf8L(originalUnicode);
	comparebuffer8.Set(testbuffer8->Des());
	test(comparebuffer8==generatedUtf8);
	testbuffer = CnvUtfConverter::ConvertToUnicodeFromUtf8L(generatedUtf8);
	compareBuffer.Set(testbuffer->Des());
	test(compareBuffer == originalUnicode);
	delete testbuffer8;
	delete testbuffer;

	INFO_PRINTF1(_L("Chinese \"Hello!\""));
	originalUnicode.Format(_L16("%c%c%c"), 0x4f60, 0x597d, 0x0021);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, originalUnicode)==0);
	test(generatedUtf8==_L8("\xe4\xbd\xa0\xe5\xa5\xbd\x21"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==originalUnicode);
	// Testing the new API
	testbuffer8= CnvUtfConverter::ConvertFromUnicodeToUtf8L(originalUnicode);
	comparebuffer8.Set(testbuffer8->Des());
	test(comparebuffer8==generatedUtf8);
	testbuffer = CnvUtfConverter::ConvertToUnicodeFromUtf8L(generatedUtf8);
	compareBuffer.Set(testbuffer->Des());
	test(compareBuffer == originalUnicode);
	delete testbuffer8;
	delete testbuffer;

	INFO_PRINTF1(_L("Japanese \"Hello!\""));
	originalUnicode.Format(_L16("%c%c%c%c"), 0x4eca, 0x65e5, 0x306f, 0x0021);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(generatedUtf8, originalUnicode)==0);
	test(generatedUtf8==_L8("\xe4\xbb\x8a\xe6\x97\xa5\xe3\x81\xaf\x21"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(generatedUnicode, generatedUtf8)==0);
	test(generatedUnicode==originalUnicode);
	// Testing the new API
	testbuffer8= CnvUtfConverter::ConvertFromUnicodeToUtf8L(originalUnicode);
	comparebuffer8.Set(testbuffer8->Des());
	test(comparebuffer8==generatedUtf8);
	testbuffer = CnvUtfConverter::ConvertToUnicodeFromUtf8L(generatedUtf8);
	compareBuffer.Set(testbuffer->Des());
	test(compareBuffer == originalUnicode);
	delete testbuffer8;
	delete testbuffer;
	
	
	}

CT_SIMPLE8::CT_SIMPLE8()
    {
    SetTestStepName(KTestStep_T_SIMPLE8);
    }

TVerdict CT_SIMPLE8::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestSimple8());
    TRAPD(error2, TestForUtf8BomL());


    __UHEAP_MARKEND;

    if(error1 == KErrNone && error2 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }

