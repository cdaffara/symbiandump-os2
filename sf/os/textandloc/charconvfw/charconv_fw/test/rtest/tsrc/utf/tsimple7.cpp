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

///////////////////////////////////////////////////////////////////////////////////////

#include "t_simple7.h"

#define test(cond)                                  \
    TEST((cond));                                   \
    if (!(cond))                                    \
        {                                           \
        ERR_PRINTF1(_L("ERROR: Test Failed"));      \
        User::Leave(1);                             \
        }

/**
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0562
@SYMTestCaseDesc        Conversion tests from Unicode to UTF-7 character set
@SYMTestPriority        Medium
@SYMTestActions        	Tests for CnvUtfConverter::ConvertFromUnicodeToUtf7() function
						Tests for CnvUtfConverter::ConvertToUnicodeFromUtf7() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_SIMPLE7::TestSIMPLE7()
	{
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0562 Testing simple UTF-7 round trips "));
	TBuf16<256> originalUnicode;
	TBuf8<256> generatedUtf7;
	TBuf16<256> generatedUnicode;
	TInt state=CnvUtfConverter::KStateDefault;
	//
	INFO_PRINTF1(_L("Empty descriptor"));
	originalUnicode=_L16("");
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, ETrue)==0);
	test(generatedUtf7==_L8(""));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, EFalse)==0);
	test(generatedUtf7==_L8(""));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("English \"Hello!\""));
	originalUnicode=_L16("Hello!");
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, ETrue)==0);
	test(generatedUtf7==_L8("Hello+ACE-"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, EFalse)==0);
	test(generatedUtf7==_L8("Hello!"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Russian \"Hello!\""));
	originalUnicode.Format(_L16("%c%c%c%c%c%c%c%c%c%c%c%c!"), 0x0417, 0x0434, 0x0440, 0x0430, 0x0432, 0x0441, 0x0442, 0x0432, 0x0443, 0x0439, 0x0442, 0x0435);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, ETrue)==0);
	test(generatedUtf7==_L8("+BBcENARABDAEMgRBBEIEMgRDBDkEQgQ1ACE-"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, EFalse)==0);
	test(generatedUtf7==_L8("+BBcENARABDAEMgRBBEIEMgRDBDkEQgQ1-!"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Greek \"Hello!\""));
	originalUnicode.Format(_L16("%c%c%c%c%c!"), 0x0393, 0x03b1, 0x03c3, 0x03bf, 0x03c5);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, ETrue)==0);
	test(generatedUtf7==_L8("+A5MDsQPDA78DxQAh-"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, EFalse)==0);
	test(generatedUtf7==_L8("+A5MDsQPDA78DxQ-!"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Chinese \"Hello!\""));
	originalUnicode.Format(_L16("%c%c!"), 0x4f60, 0x597d);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, ETrue)==0);
	test(generatedUtf7==_L8("+T2BZfQAh-"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, EFalse)==0);
	test(generatedUtf7==_L8("+T2BZfQ-!"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Japanese \"Hello!\""));
	originalUnicode.Format(_L16("%c%c%c!"), 0x4eca, 0x65e5, 0x306f);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, ETrue)==0);
	test(generatedUtf7==_L8("+Tspl5TBvACE-"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, EFalse)==0);
	test(generatedUtf7==_L8("+Tspl5TBv-!"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Trailing \"-\" character"));
	originalUnicode=_L16(":-");
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, ETrue)==0);
	test(generatedUtf7==_L8(":-"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, EFalse)==0);
	test(generatedUtf7==_L8(":-"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode=_L16("=-");
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, ETrue)==0);
	test(generatedUtf7==_L8("+AD0--"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, EFalse)==0);
	test(generatedUtf7==_L8("=-"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	originalUnicode.Format(_L16("%c-"), 0x1e77);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, ETrue)==0);
	test(generatedUtf7==_L8("+Hnc--"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, EFalse)==0);
	test(generatedUtf7==_L8("+Hnc--"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Interspersed \"+\" characters"));
	originalUnicode.Format(_L16("+%c+&+a+"), 0x52ff);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, ETrue)==0);
	test(generatedUtf7==_L8("+-+Uv8-+-+ACY-+-a+-"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(generatedUtf7, originalUnicode, EFalse)==0);
	test(generatedUtf7==_L8("+-+Uv8-+-&+-a+-"));
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(generatedUnicode, generatedUtf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);

	}

CT_SIMPLE7::CT_SIMPLE7()
    {
    SetTestStepName(KTestStep_T_SIMPLE7);
    }

TVerdict CT_SIMPLE7::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestSIMPLE7());


    __UHEAP_MARKEND;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
