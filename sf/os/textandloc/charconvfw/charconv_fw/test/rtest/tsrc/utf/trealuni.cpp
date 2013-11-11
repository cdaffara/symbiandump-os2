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
#include "t_realuni.h"

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
@SYMTestCaseID          SYSLIB-CHARCONV-CT-0568
@SYMTestCaseDesc        Testing a real Unicode file's round trip
@SYMTestPriority        Medium
@SYMTestActions        	Tests for CnvUtfConverter::ConvertFromUnicodeToUtf8() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_REALUNI::TestREALUNI()
	{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0568 Testing a \"real\" Unicode file's round trip "));
	RFs fs;
	RFile file;
	test(fs.Connect() == KErrNone);
	TFindFile findFile=fs;
	test(findFile.FindByDir(_L("DAXUE.UNI"), _L("\\system\\data\\"))==KErrNone);
	test(file.Open(fs, findFile.File(), EFileShareExclusive|EFileStream|EFileRead)==KErrNone);
	TInt size=0;
	test(file.Size(size) == KErrNone);
	test((size>0) && (size%2==0));
	HBufC16* originalUnicode=HBufC16::New(size/2);
	HBufC8* generatedUtf8=HBufC8::New(size*2);
	HBufC16* generatedUnicode=HBufC16::New(size/2);
	test(originalUnicode!=NULL);
	test(generatedUtf8!=NULL);
	test(generatedUnicode!=NULL);
	TPtr8 ptr1(REINTERPRET_CAST(TUint8*, CONST_CAST(TUint16*, originalUnicode->Ptr())), 0, size);
	test(file.Read(ptr1) == KErrNone);
	test(ptr1.Length()==size);
	TPtr16 ptr2=originalUnicode->Des();
	ptr2.SetLength(size/2);
	test(originalUnicode->Size()==size);
	TPtr8 ptr3=generatedUtf8->Des();
	test(CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr3, *originalUnicode)==0);
	TPtr16 ptr4=generatedUnicode->Des();
	test(CnvUtfConverter::ConvertToUnicodeFromUtf8(ptr4, *generatedUtf8)==0);
	test(*generatedUnicode==*originalUnicode);
	delete originalUnicode;
	delete generatedUtf8;
	delete generatedUnicode;
	file.Close();
	fs.Close();

	}

CT_REALUNI::CT_REALUNI()
    {
    SetTestStepName(KTestStep_T_REALUNI);
    }

TVerdict CT_REALUNI::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestREALUNI());

    __UHEAP_MARKEND;

    if(error1 == KErrNone )
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
