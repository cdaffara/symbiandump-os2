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
#include "t_imap.h"


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
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0574
@SYMTestCaseDesc        Tests for converting to Unicode from UTF- 7, from UTF-7 to Unicode
@SYMTestPriority        Medium
@SYMTestActions        	Tests for CCnvCharacterSetConverter::ConvertToUnicode(),
						CCnvCharacterSetConverter::ConvertFromUnicode() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_IMAP::TestIMAP()
	{
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();
	TInt state=CCnvCharacterSetConverter::KStateDefault;
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0574 Testing IMAP UTF-7 "));
	test(characterSetConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierImapUtf7, fileServerSession)==CCnvCharacterSetConverter::EAvailable);
	TBuf16<256> originalUnicode;
	TBuf8<256> generatedUtf7;
	TBuf16<256> generatedUnicode;
	//
	INFO_PRINTF1(_L("Empty descriptor"));
	originalUnicode=_L16("");
	test(characterSetConverter->ConvertFromUnicode(generatedUtf7, originalUnicode)==0);
	test(generatedUtf7==_L8(""));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf7, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Characters \" +&-~\\\""));
	originalUnicode=_L16(" +&-~\\");
	test(characterSetConverter->ConvertFromUnicode(generatedUtf7, originalUnicode)==0);
	test(generatedUtf7==_L8(" +&--~\\"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf7, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Example quoted in RFC 2060 (Section 5.1.3)"));
	originalUnicode.Format(_L16("~peter/mail/%c%c%c/%c%c"), 0x65e5, 0x672c, 0x8a9e, 0x53f0, 0x5317);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf7, originalUnicode)==0);
	test(generatedUtf7==_L8("~peter/mail/&ZeVnLIqe-/&U,BTFw-"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf7, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	INFO_PRINTF1(_L("Testing fix for defect EDNDSEF-4KMEUH in \"Symbian Defect Tracking v3.0\""));
	originalUnicode.Format(_L16("%c%c%c%c%c%c%c%c%c"), 0x30ad, 0x30e3, 0x30d3, 0x30cd, 0x30c3, 0x30c8, 0x3068, 0x306f, 0xff1f);
	test(characterSetConverter->ConvertFromUnicode(generatedUtf7, originalUnicode)==0);
	test(generatedUtf7==_L8("&MK0w4zDTMM0wwzDIMGgwb,8f-"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf7, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode);
	TBuf8<0x14> generatedUtf7_0x14;
	test(characterSetConverter->ConvertFromUnicode(generatedUtf7_0x14, originalUnicode)==3);
	test(generatedUtf7_0x14==_L8("&MK0w4zDTMM0wwzDI-"));
	test(characterSetConverter->ConvertToUnicode(generatedUnicode, generatedUtf7_0x14, state)==0);
	test(state==CCnvCharacterSetConverter::KStateDefault);
	test(generatedUnicode==originalUnicode.Left(6));
	CleanupStack::PopAndDestroy(2); // characterSetConverter and fileServerSession
	}

CT_IMAP::CT_IMAP()
    {
    SetTestStepName(KTestStep_T_IMAP);
    }

TVerdict CT_IMAP::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestIMAP());

    __UHEAP_MARKEND;

    if(error1 == KErrNone )
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
