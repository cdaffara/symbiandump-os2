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
#include "t_newlin7.h"

///////////////////////////////////////////////////////////////////////////////////////

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
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0570
@SYMTestCaseDesc        Testing base64 sequences combined with line-breaks
@SYMTestPriority        Medium
@SYMTestActions        	Tests for conversion from UTF-7 to Unicode with line-breaks
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_NEWLIN7::TestNEWLIN7()
	{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0570 Testing base64 sequences combined with line-breaks "));
	TBuf16<256> unicode;
	TBuf8<256> utf7;
	TInt state=CnvUtfConverter::KStateDefault;
	utf7=_L8("+U0E\nU0E");
	test(CnvUtfConverter::ConvertToUnicodeFromUtf7(unicode, utf7, state)==0);
	test(state==CnvUtfConverter::KStateDefault);
	test(unicode.Length()==5);
	test(unicode[0]==0x5341);
	test(unicode[1]==0x000a);
	test(unicode[2]==0x0055);
	test(unicode[3]==0x0030);
	test(unicode[4]==0x0045);
	unicode=_L16(")\n)\n*\n*\n+\n+\n)\n)\n*\n*\n+\n+");
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(utf7, unicode, ETrue)==0);
	test(utf7==_L8(")\n)\n+ACo-\n+ACo-\n+-\n+-\n)\n)\n+ACo-\n+ACo-\n+-\n+-"));
	test(CnvUtfConverter::ConvertFromUnicodeToUtf7(utf7, unicode, EFalse)==0);
	test(utf7==_L8(")\n)\n*\n*\n+-\n+-\n)\n)\n*\n*\n+-\n+-"));
	}

CT_NEWLIN7::CT_NEWLIN7()
    {
    SetTestStepName(KTestStep_T_NEWLIN7);
    }

TVerdict CT_NEWLIN7::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestNEWLIN7());

    __UHEAP_MARKEND;

    if(error1 == KErrNone )
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
