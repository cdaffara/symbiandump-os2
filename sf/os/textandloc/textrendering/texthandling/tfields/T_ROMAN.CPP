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


#include "../sfields/FLDNUMS.H"
#include "T_ROMAN.h"

#define test(cond)											\
	{														\
	TBool __bb = (cond);									\
	TEST(__bb);												\
	if (!__bb)												\
		{													\
		ERR_PRINTF1(_L("ERROR: Test Failed"));				\
		User::Leave(1);										\
		}													\
	}

#define UNUSED_VAR(a) a = a

const TInt KTestCleanupStack=0x40;

LOCAL_D CTrapCleanup* TheTrapCleanup;

//

void CT_ROMAN::GetValue(HBufC* aBuf,TInt aDenery,TDeneryToCharBase& aNumeral)
	{
	TPtr ptr=aBuf->Des();
	TInt size=aNumeral.DeneryToChar(ptr,aDenery);
	if (size>0)
		{
		aBuf->ReAlloc(size);
		ptr=aBuf->Des();
		size = aNumeral.DeneryToChar(ptr,aDenery);
			test(size==KErrNone);
		}
	}


void CT_ROMAN::DisplayValue(HBufC* aBuf,TInt aDenery,TDeneryToCharBase& aNumeral)
	{
	GetValue(aBuf,aDenery,aNumeral);
	INFO_PRINTF2(_L("%D: "),aDenery);
	INFO_PRINTF1(*aBuf);
	INFO_PRINTF1(_L("\n"));
	}


void CT_ROMAN::test1()
	{
	INFO_PRINTF1(_L("Testing with buffer of adequate size"));
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-ETEXT-LEGACY-T_ROMAN-0001 "));
	HBufC* hbuf = HBufC::NewL(10); 
	CleanupStack::PushL(hbuf);
	TRomanNumeral roman;
	GetValue(hbuf,4,roman);
	TPtr buf = hbuf->Des();
		test(buf==_L("IV"));
	CleanupStack::PopAndDestroy();
	//
	INFO_PRINTF1(_L("Testing with inadequate buffer"));
	HBufC* minibuf = HBufC::NewL(1); 
	CleanupStack::PushL(minibuf);
	GetValue(minibuf,4,roman);
	TPtr buf2 = minibuf->Des();
		test(buf2==_L("IV"));
	CleanupStack::PopAndDestroy();
	}


void CT_ROMAN::test2()
	{
	INFO_PRINTF1(_L("Testing Roman numerals for a range of denery values"));
	HBufC* hbuf = HBufC::NewL(10); 
	CleanupStack::PushL(hbuf);
	TRomanNumeral roman;

	DisplayValue(hbuf,1,roman);
	test(0 == hbuf->CompareC(_L("i"), 1, 0));
	DisplayValue(hbuf,4,roman);
	test(0 == hbuf->CompareC(_L("iv"), 1, 0));
	DisplayValue(hbuf,9,roman);
	test(0 == hbuf->CompareC(_L("ix"), 1, 0));
	DisplayValue(hbuf,17,roman);
	test(0 == hbuf->CompareC(_L("xvii"), 1, 0));
	DisplayValue(hbuf,34,roman);
	test(0 == hbuf->CompareC(_L("xxxiv"), 1, 0));
	DisplayValue(hbuf,99,roman);
	test(0 == hbuf->CompareC(_L("xcix"), 1, 0));
	DisplayValue(hbuf,143,roman);
	test(0 == hbuf->CompareC(_L("cxliii"), 1, 0));
	DisplayValue(hbuf,1982,roman);
	test(0 == hbuf->CompareC(_L("mcmlxxxii"), 1, 0));

	CleanupStack::PopAndDestroy();
	}


void CT_ROMAN::test3()
	{
	INFO_PRINTF1(_L("Testing alphabetic numerals for a range of denery values"));
	HBufC* hbuf = HBufC::NewL(10); 
	CleanupStack::PushL(hbuf);
	TAlphabeticNumeral alpha;

	DisplayValue(hbuf,1,alpha);
	test(0 == hbuf->CompareC(_L("a"), 1, 0));
	DisplayValue(hbuf,4,alpha);
	test(0 == hbuf->CompareC(_L("d"), 1, 0));
	DisplayValue(hbuf,9,alpha);
	test(0 == hbuf->CompareC(_L("i"), 1, 0));
	DisplayValue(hbuf,17,alpha);
	test(0 == hbuf->CompareC(_L("q"), 1, 0));
	DisplayValue(hbuf,34,alpha);
	test(0 == hbuf->CompareC(_L("ah"), 1, 0));
	DisplayValue(hbuf,99,alpha);
	test(0 == hbuf->CompareC(_L("cu"), 1, 0));
	DisplayValue(hbuf,143,alpha);
	test(0 == hbuf->CompareC(_L("em"), 1, 0));
	DisplayValue(hbuf,1982,alpha);
	test(0 == hbuf->CompareC(_L("bxf"), 1, 0));

	CleanupStack::PopAndDestroy();
	}


void CT_ROMAN::runTests()
// Test the fields dll.
//
    {
	// test with HBufs both large enough and too small
	test1();

	// test for a range of denery values
	test2();

	// Test alphabetics 
	test3();
    }


void CT_ROMAN::setupCleanup()
//
// Initialise the cleanup stack.
//
    {

	TheTrapCleanup=CTrapCleanup::New();
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)1);\
		test(r==KErrNone);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	}

CT_ROMAN::CT_ROMAN()
    {
    SetTestStepName(KTestStep_T_ROMAN);
    }

TVerdict CT_ROMAN::doTestStepL()
    {
	INFO_PRINTF1(_L("Testing denery to Roman numeral converter"));
    SetTestStepResult(EFail);

    __UHEAP_MARK;

	setupCleanup();
    
    TRAPD(error1, runTests());

	delete TheTrapCleanup;

    __UHEAP_MARKEND;

    if(error1 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
