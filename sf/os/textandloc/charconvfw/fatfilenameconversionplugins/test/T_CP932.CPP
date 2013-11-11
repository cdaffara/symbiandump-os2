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


#include "T_CP932.h"
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


_LIT16(Uni_1, "\x0053\x0059\x004D\x3125\x6349\xAAAA\x9673\x9ED1\x3000\xFF9F");
_LIT8(CP932_1, "\x53\x59\x4D\x5F\x91\xA8\x5F\x92\xC2\xEE\xEC\x81\x40\xDF");
_LIT16(Uni_2, "\x0032\x0070\xFFFD\xFF61\x8D77\xFFFD\xFFFD");
_LIT8(CP932_2, "\x32\x70\x80\xA1\x8B\x4E\xA0\x96\x7F");
_LIT16(Uni_3, "\x4E00\x5141\x674F\x95C7\x58F1");
_LIT8(CP932_3, "\x88\xEA\x88\xF2\x88\xC7\x88\xC5\x88\xEB");

_LIT(KName,"CP932");
const TUid KPluginUid={0x10206A92};


// Used for supressing warning in OOM tests
#define __UNUSED_VAR(var) var = var

/**
@SYMTestCaseID          SYSLIB-FATCHARSETCONV-CT-1779
@SYMTestCaseDesc	    Tests API behaviours of UnicodeConv class
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for conversions from/to Unicode, using a function pointer
@SYMTestExpectedResults Test must not fail 
*/
void CT_CP932::TestL()
	{ 
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-FATCHARSETCONV-CT-1779 "));
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
	
	
	TBuf8<15> foreign1;
	TBuf8<15> foreign3;
	TBuf16<15> unicode2;
	
	const TDesC16& unicode1(Uni_1);
	(*aConvertFromUnicodeL)(foreign1, unicode1); 	//testing conversion from Unicode
	TInt error = foreign1.Compare(CP932_1);
	test(error==0);
	foreign1.Zero();
	
	const TDesC16& unicode3(Uni_3);
	(*aConvertFromUnicodeL)(foreign3, unicode3); 	//testing conversion from Unicode for INC112715
	error = foreign3.Compare(CP932_3);
	test(error==0);
	foreign3.Zero();
	
	const TDesC8& foreign2(CP932_2);
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
	result = (*aIsLegalShortNameCharacter)(0xFF61); //testing for a double byte character
	test(result==1);

	lib.Close();
	}

/**
@SYMTestCaseID          SYSLIB-FATCHARSETCONV-CT-1847
@SYMTestCaseDesc	    Tests API behaviours of UnicodeConv class as part of INC090073
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for correct character conversion on certain chinese characters 
@SYMTestExpectedResults Test must not fail 
*/	
void CT_CP932::TestINC090073L()
	{
    INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-FATCHARSETCONV-CT-1847 "));
 	_LIT16(unicode, "\x6DFC\x715C\x9785\x7A37\x61A9\x80B1\x86A3\x7B71\x6615\x6600");
	_LIT8(CP932Code, "\xED\xE6\xED\xF6\xE8\xD7\xE2\x6C\x8C\x65\x8D\x6E\xE5\x6E\xE2\xAA\xED\xB3\xED\xB2");

	RLibrary lib;

	const TUidType serverUid(KNullUid,KNullUid,KPluginUid);	
	// load the dll	
	TInt returnValue = lib.Load(KName,serverUid);
	test(returnValue==0);

	// get a pointer to the specified ordinal function and call it	
	TLibraryFunction function1 = lib.Lookup(1);


	//cast the function pointer f to a function of type void with two arguments
	typedef void (*TConvertFromUnicodeL)(TDes8&, const TDesC16&);	
	TConvertFromUnicodeL aConvertFromUnicodeL = reinterpret_cast <TConvertFromUnicodeL> (function1);
	
	TBuf8<20> foreign1;
	
	const TDesC16& unicode1(unicode);
	(*aConvertFromUnicodeL)(foreign1, unicode1); 	//testing conversion from Unicode
	TInt error = foreign1.Compare(CP932Code);
	test(error==0);
	foreign1.Zero();

	lib.Close();
	}	

void CT_CP932::OOMTestL()
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


CT_CP932::CT_CP932()
    {
    SetTestStepName(KTestStep_T_CP932);
    }


TVerdict CT_CP932::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestL());
    TRAPD(error2, TestINC090073L());
    TRAPD(error3, OOMTestL());

    __UHEAP_MARKEND;

    if(error1 == KErrNone && error2 == KErrNone && error3 == KErrNone)
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
