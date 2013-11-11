/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* General test code for Charconv
* @internalComponent This is test code
*
*/


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <charconv.h>
#include <convdata.h>
#include <utf.h>


///////////////////////////////////////////////////////////////////////////////////////


#include "t_memory.h"

#define test(cond)                                  \
    TEST((cond));                                   \
    if (!(cond))                                    \
        {                                           \
        ERR_PRINTF1(_L("ERROR: Test Failed"));      \
        User::Leave(1);                             \
        }


///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

class CSimple : public CBase
	{
public:
	CSimple(){};
	static CSimple* NewLC();
	};


CSimple* CSimple::NewLC()
	{
	CSimple* This = new (ELeave) CSimple;
	CleanupStack::PushL(This);
	return This;
	}


/**
 * CreateAndDestroyConverterL is run for increasing values of __UHEAP_FAILNEXT
 */
void CreateAndDestroyConverterL(void)
	{
	// put some dummy items on the stack
	TInt numDummies=5;
	for (TInt i=0; i<numDummies; i++)
		{
		CSimple* obj = CSimple::NewLC();

		// use to obj to fix compilier warning
		obj = (CSimple *)NULL;
		}

	CCnvCharacterSetConverter* charConverter2 = 0;
	charConverter2 = CCnvCharacterSetConverter::NewLC();

	CleanupStack::PopAndDestroy(charConverter2);
	CleanupStack::PopAndDestroy(numDummies);
	}

/**
Test code for DEF041901 see teamtrack for more details
Description of DEF041901:
If a memory allocation fails during CleanupStack::PushL in the NewLC, then
the destructor will be called. The destructor calls
TTlsData::CharacterSetConverterIsBeingDestroyed(); which incorrectly frees
the thread local storage

@SYMTestCaseID          SYSLIB-CHARCONV-CT-0561
@SYMTestCaseDesc        Tests for defect number DEF041901
@SYMTestPriority        Medium
@SYMTestActions        	Tests for memory allocation failure.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void CT_MEMORY::TestDef41901L()
	{
	RFs fileServerSession;
	CleanupClosePushL(fileServerSession);
	User::LeaveIfError(fileServerSession.Connect());
	INFO_PRINTF1(_L(" @SYMTestCaseID:SYSLIB-CHARCONV-CT-0561 T_memory Charconv test for DEF041901\n "));

	// create an instance of CCnvCharacterSetConverter
	CCnvCharacterSetConverter* charConverter1;
	charConverter1 = CCnvCharacterSetConverter::NewLC();

	// conversion from native
	_LIT(KSourceString, "Skookumchuck");
	TBuf8<80> outputString;
	CCnvCharacterSetConverter::TAvailability availability;
	TInt unconvertedCount;

	outputString.SetLength(0);
	availability = charConverter1->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit, fileServerSession);
	test(availability == CCnvCharacterSetConverter::EAvailable);
	unconvertedCount = charConverter1->ConvertFromUnicode(outputString, KSourceString);
	test(unconvertedCount == 0);
	INFO_PRINTF1(_L("After first charConverter1 conversion.\n"));

	// perfrom heap checking on a second instance of CCnvCharacterSetConverter
	TInt pass=1;
	TInt ret=KErrNoMemory;
 	while (ret != KErrNone)
		{
		__UHEAP_FAILNEXT(pass);
		TRAP(ret,CreateAndDestroyConverterL());
		__UHEAP_RESET;
		INFO_PRINTF3(_L("CCnvCharacterSetConverter OOM test: pass[%d] ret[%d]\n"),pass,ret);
		pass++;
		}

 	// now testing of charConverter2 is complete, test that charConverter1 is still ok.
 	// Note: this is the test code DEF041901 see teamtrack for more details.
	availability = charConverter1->PrepareToConvertToOrFromL(KCharacterSetIdentifierSms7Bit, fileServerSession);
	test(availability == CCnvCharacterSetConverter::EAvailable);
	unconvertedCount = charConverter1->ConvertFromUnicode(outputString, KSourceString);
	test(unconvertedCount == 0);

	INFO_PRINTF1(_L("Charconv test for DEF041901 passed ok\n"));

	CleanupStack::PopAndDestroy(charConverter1);
	CleanupStack::PopAndDestroy();	// RFs
	}


CT_MEMORY::CT_MEMORY()
    {
    SetTestStepName(KTestStep_T_MEMORY);
    }


TVerdict CT_MEMORY::doTestStepL()
    {
    SetTestStepResult(EFail);

    __UHEAP_MARK;

    TRAPD(error1, TestDef41901L());


    __UHEAP_MARKEND;

    if(error1 == KErrNone )
        {
        SetTestStepResult(EPass);
        }

    return TestStepResult();
    }
