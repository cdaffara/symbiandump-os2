// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <e32test.h>
#include <f32file.h>
#include <s32file.h>
#include <bautils.h>
#include <hal.h>

#include "DowngradePath.h"
#include "../EcomTestUtils/EcomTestUtils.h"


/* test stuff */

RTest test(_L("EComServer: RDowngradePath test"));

// flag to indicate when OOM testing is in progress
TBool OomTest = EFalse;

// globals
typedef RArray<TLanguage> Languages;
LOCAL_D RFs	TheFs;


// --------------------------------------------------------------
// Begin: General helper functions & macros
// --------------------------------------------------------------

/** Test boolean result. */
#define TEST1(stmt)	test(stmt)

/** Print formatted string to output. */
#define TEST_PRINTF	test.Printf

/** Start next test. */
#define TEST_NEXT(title)	\
	{\
	if (!OomTest)\
		test.Next(title);\
	}

// Used for supressing warning in OOM tests
#define __UNUSED_VAR(var) var = var

/**
Conditionally test given statement.
if OOM testing is in progress then the error code is compared against KErrNoMemory.
If it matches then the code leaves with that error, otherwise the boolean result
of the statement will be tested.
*/
#define TEST2(err, stmt) \
	{\
	if (OomTest && err==KErrNoMemory) \
		{\
		User::LeaveNoMemory();\
		}\
	else\
		{\
		test(stmt);\
		}\
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-1857
@SYMTestCaseDesc 	Test RDowngradePath under OOM conditions.
@SYMTestPriority 	High
@SYMTestActions  	Test the use of RDowngradePath under OOM conditions.
@SYMTestExpectedResults No panics caused by leaking code.
@SYMDEF				 DEF090862
*/
typedef void (*FuncPtr8L) ();
static void DoOOMTestL(FuncPtr8L aTestFunctionL)
	{
	OomTest = ETrue;

	TInt err = KErrNone;
	TInt tryCount = 0;
	RThread thread;

	do
		{
		__UHEAP_MARK;

		// find out the number of open handles
		TInt startProcessHandleCount = 0;
		TInt startThreadHandleCount = 0;
		thread.HandleCount(startProcessHandleCount, startThreadHandleCount);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, aTestFunctionL());
		RDowngradePath::Reset();
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// check that no handles have leaked
		TInt endProcessHandleCount = 0;
		TInt endThreadHandleCount = 0;
		thread.HandleCount(endProcessHandleCount, endThreadHandleCount);
		TEST1(startProcessHandleCount == endProcessHandleCount);
		TEST1(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

	thread.Close();

 	TEST1(err==KErrNone);

	OomTest = EFalse;
	TEST_PRINTF(_L("- succeeded at heap failure rate of %i\n"), tryCount);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0656
@SYMTestCaseDesc	    Tests for RDowngradePath::HasChangedL() function
@SYMTestPriority 	    Medium
@SYMTestActions  	    Tests for downgradepath consistency
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDowngradePathL()
	{
	//we need to make sure that at start of the test the language is set to English
	EComTestUtils::SwitchToLanguageL(static_cast<TLanguage>(ELangEnglish));

	//the first time this is call should return language has changed
	TInt error;
	TBool hasLanguageChanged=EFalse;
	TRAP(error,hasLanguageChanged=RDowngradePath::HasChangedL(TheFs));
	if (error==KErrNoMemory)
		{
		User::LeaveNoMemory();
		}

	TEST1(hasLanguageChanged);

	//if second time called and no language changed yet,it should return false
	TRAP(error,hasLanguageChanged=RDowngradePath::HasChangedL(TheFs));
	if (error==KErrNoMemory)
		{
		User::LeaveNoMemory();
		}
	TEST1(!hasLanguageChanged);

	//now switch to another language, this will change the language downgrade path
	EComTestUtils::SwitchToLanguageL(static_cast<TLanguage>(ELangFrench));

	//this should detect language has changed now
	TRAP(error,hasLanguageChanged=RDowngradePath::HasChangedL(TheFs));
	if (error==KErrNoMemory)
		{
		User::LeaveNoMemory();
		}
	TEST1(hasLanguageChanged);

	//and finally call that function again it should return no language changed yet.
	TRAP(error,hasLanguageChanged=RDowngradePath::HasChangedL(TheFs));
	if (error==KErrNoMemory)
		{
		User::LeaveNoMemory();
		}
	TEST1(!hasLanguageChanged);

	//reset the cached language settings
	RDowngradePath::Reset();
	//and finally when tests are completed switch back to English
	EComTestUtils::SwitchToLanguageL(static_cast<TLanguage>(ELangEnglish));
	}

/**
Run all RDowngradePath tests.
*/
LOCAL_C void RunTestsL()
	{
	TEST_NEXT(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0656"));
	TestDowngradePathL();

	TEST_NEXT(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1857 Test RDowngradePath in OOM conditions. "));
	DoOOMTestL(TestDowngradePathL);
	}

/**
Executable entry point.
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Title();
	test.Start(_L("Running tests..."));

	test(TheFs.Connect() == KErrNone);
	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(err, RunTestsL());

	delete cleanup;
	TheFs.Close();

	// check for errors
	test(err == KErrNone);

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return(0);
	}
