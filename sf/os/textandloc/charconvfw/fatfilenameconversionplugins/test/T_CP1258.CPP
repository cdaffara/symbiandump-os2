/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "T_CP1258.h"
#include <e32des8.h>

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


_LIT16(Uni_1, "\x006D\xAAAA\x00DC\x0111\x20AC\xFFFF\x0070");
_LIT8(CP1258_1, "\x6D\x5F\xDC\xF0\x80\x5F\x70");
_LIT16(Uni_2, "\x0017\x005F\x201A\x00A0\x0303");
_LIT8(CP1258_2, "\x17\x5F\x82\xA0\xDE");


_LIT(KName,"CP1258");
const TUid KPluginUid={0x10206A94};


// Used for supressing warning in OOM tests
#define __UNUSED_VAR(var) var = var

/**
@SYMTestCaseID          SYSLIB-FATCHARSETCONV-CT-1781
@SYMTestCaseDesc	    Tests API behaviours of UnicodeConv class
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for conversions from/to Unicode, using a function pointer
@SYMTestExpectedResults Test must not fail
*/
void CT_CP1258::TestL()
	{
	RLibrary lib;

	const TUidType serverUid(KNullUid,KNullUid,KPluginUid);
	// load the dll
	TInt returnValue = lib.Load(KName,serverUid);
	test(returnValue==0);

	// get a pointer to the specified ordinal function and call it
	TLibraryFunction function1 = lib.Lookup(1);
	TLibraryFunction function2 = lib.Lookup(2);
	TLibraryFunction function3 = lib.Lookup(3);

	//cast the function pointer f to a function of type void with two arguments
	typedef void (*TConvertFromUnicodeL)(TDes8&, const TDesC16&);
	TConvertFromUnicodeL aConvertFromUnicodeL = reinterpret_cast <TConvertFromUnicodeL> (function1);

	typedef void (*TConvertToUnicodeL)(TDes16&, const TDesC8&);
	TConvertToUnicodeL aConvertToUnicodeL = reinterpret_cast <TConvertToUnicodeL> (function2);

	typedef TBool (*TIsLegalShortNameCharacter)(TUint);
	TIsLegalShortNameCharacter aIsLegalShortNameCharacter = reinterpret_cast <TIsLegalShortNameCharacter> (function3);


	TBuf8<20> foreign1;
	TBuf16<20> unicode2;

	const TDesC16& unicode1(Uni_1);
	(*aConvertFromUnicodeL)(foreign1, unicode1); 	//testing conversion from Unicode
	TInt error = foreign1.Compare(CP1258_1);
	test(error==0);
	foreign1.Zero();

	const TDesC8& foreign2(CP1258_2);
	(*aConvertToUnicodeL)(unicode2,foreign2); 	//testing conversion to Unicode
	error = unicode2.Compare(Uni_2);
	test(error==0);
	unicode2.Zero();


	//testing for legal short name character
	TInt result = (*aIsLegalShortNameCharacter)(0x005F); //testing for existent character
	test(result==1);
	result = (*aIsLegalShortNameCharacter)(0x003F); //testing for illegal character
	test(result==0);
	result = (*aIsLegalShortNameCharacter)(0x2999); //testing for non-existent character
	test(result==0);

	lib.Close();
	}

void CT_CP1258::OOMTestL()
	{
    INFO_PRINTF1(_L("OOM testing"));
	TInt err, tryCount = 0;
	do
		{
			__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

			// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err,TestL());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		}while (err == KErrNoMemory);

	test(err == KErrNone);
	INFO_PRINTF2(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}


CT_CP1258::CT_CP1258()
    {
    SetTestStepName(KTestStep_T_CP1258);
    }


TVerdict CT_CP1258::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestL());
    TRAPD(error2, OOMTestL());

    __UHEAP_MARKEND;

    if(error1 == KErrNone && error2 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
