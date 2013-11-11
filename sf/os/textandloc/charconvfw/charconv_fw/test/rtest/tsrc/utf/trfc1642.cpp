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
#include "t_rfc1642.h"
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
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0564
@SYMTestCaseDesc        Tests for conversion from UTF-7 to Unicode character set
@SYMTestPriority        Medium
@SYMTestActions        	Tests for CnvUtfConverter::ConvertToUnicodeFromUtf7() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_RFC1642::TestRFC1642()
	{

    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0564 Testing the examples given in RFC 1642 "));
	TBuf16<256> generatedUnicode;
	TInt state=CnvUtfConverter::KStateDefault;
	//
	INFO_PRINTF1(_L("Empty descriptor"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8(""), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==_L16(""));
	INFO_PRINTF1(_L("\"A<NOT IDENTICAL TO><ALPHA>.\""));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("A+ImIDkQ."), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==4);
	test(generatedUnicode[0]==0x0041);
	test(generatedUnicode[1]==0x2262);
	test(generatedUnicode[2]==0x0391);
	test(generatedUnicode[3]==0x002e);
	INFO_PRINTF1(_L("\"Hi Mom <WHITE SMILING FACE>!\""));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("Hi Mom +Jjo-!"), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==9);
	test(generatedUnicode[0]==0x0048);
	test(generatedUnicode[1]==0x0069);
	test(generatedUnicode[2]==0x0020);
	test(generatedUnicode[3]==0x004d);
	test(generatedUnicode[4]==0x006f);
	test(generatedUnicode[5]==0x006d);
	test(generatedUnicode[6]==0x0020);
	test(generatedUnicode[7]==0x263a);
	test(generatedUnicode[8]==0x0021);
	INFO_PRINTF1(_L("The japanese word \"nihongo\""));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+ZeVnLIqe-"), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode.Length()==3);
	test(generatedUnicode[0]==0x65e5);
	test(generatedUnicode[1]==0x672c);
	test(generatedUnicode[2]==0x8a9e);
	INFO_PRINTF1(_L("The special case of \"+-\""));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("+-"), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==_L16("+"));
	INFO_PRINTF1(_L("The special case of \"+-\" in a context"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, _L8("2+-3 is 5"), state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==_L16("2+3 is 5"));
	//

	}

CT_RFC1642::CT_RFC1642()
    {
    SetTestStepName(KTestStep_T_RFC1642);
    }

TVerdict CT_RFC1642::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestRFC1642());

    __UHEAP_MARKEND;

    if(error1 == KErrNone )
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }

