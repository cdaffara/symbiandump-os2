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
#include <f32file.h>
#include <utf.h>
#include "t_realut8.h"

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
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0567
@SYMTestCaseDesc        Testing a real UTF8 file's round trip
@SYMTestPriority        Medium
@SYMTestActions        	Tests for CnvUtfConverter::ConvertToUnicodeFromUtf8() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_REALUT8::TestREALUT8()
	{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0567 Testing a \"real\" UTF8 file's round trip "));
	RFs fs;
	RFile inputFile;
	test(fs.Connect()== KErrNone);
	TFindFile findFile=fs;
	test(findFile.FindByDir(_L("ETHIOPIC.UT8"), _L("\\system\\data\\"))==KErrNone);
	test(inputFile.Open(fs, findFile.File(), EFileShareExclusive|EFileStream|EFileRead)==KErrNone);
	TInt size=0;
	test(inputFile.Size(size) == KErrNone);
	HBufC8* originalUtf8=HBufC8::New(size);
	HBufC16* generatedUnicode=HBufC16::New(size);
	HBufC8* generatedUtf8=HBufC8::New(size);
	RFile outputFile;
	test(originalUtf8!=NULL);
	test(generatedUnicode!=NULL);
	test(generatedUtf8!=NULL);
	TPtr8 ptr1=originalUtf8->Des();
	test(inputFile.Read(ptr1) == KErrNone);
	test(outputFile.Replace(fs, _L("c:\\ETHIOPIC.UNI"), EFileShareExclusive|EFileStream|EFileWrite)==KErrNone);
	TPtr16 ptr2=generatedUnicode->Des();
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(ptr2, *originalUtf8)==0);
	TPtr8 ptr3=generatedUtf8->Des();
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr3, *generatedUnicode)==0);
	test(*generatedUtf8==*originalUtf8);
	test(outputFile.Write(TPtrC8(REINTERPRET_CAST(const TUint8*, generatedUnicode->Ptr()), generatedUnicode->Size()))==KErrNone);
	outputFile.Close();
	delete generatedUtf8;
	delete generatedUnicode;
	delete originalUtf8;
	inputFile.Close();
	fs.Close();
	}

CT_REALUT8::CT_REALUT8()
    {
    SetTestStepName(KTestStep_T_REALUT8);
    }

TVerdict CT_REALUT8::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestREALUT8());

    __UHEAP_MARKEND;

    if(error1 == KErrNone )
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
