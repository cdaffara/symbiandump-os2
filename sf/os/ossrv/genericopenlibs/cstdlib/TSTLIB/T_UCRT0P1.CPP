// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32cons.h>
#include <e32test.h>


LOCAL_D RTest test(_L("T_UCRT0P1"));
LOCAL_D RProcess me;


/**
@SYMTestCaseID 			SYSLIB-STDLIB-UT-1669
@SYMTestCaseDesc	    Testing whether increasing KMaxArgC from 20 to 25 results in the extra arguments
						passed in to a new process to be handled correctly.
@SYMTestPriority 	    High
@SYMTestActions  	    Starts a new process (T_UCRT0P2) passing in a set of arguments and this new process
						checks all the arguements were handled correctly.
@SYMTestExpectedResults The test should not fail.
@SYMDEF 				DEF074278
*/
void Defect_DEF074278()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STDLIB-UT-1669 - Defect_DEF074278... "));
	
	TBuf<180> arguments(_L("one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen sixteen seventeen eighteen nineteen twenty twenty-one twenty-two twenty-three twenty-four "));

	test.Printf(_L("Create a new process passing in a large number of arguements\n"));
	//Creating another process to run T_UCRT0P2 passing in 24 arguments (25th will be the
	//path and name of the exe). T_UCRT0P2 should only be run via this test and not on it's own.
	TInt err=me.Create(_L("T_UCRT0P2"),arguments);
	if (err==KErrNotFound)
		err=me.Create(_L("z:\\test\\T_UCRT0P2"),arguments);
    if (err==KErrNotFound)
        err=me.Create(_L("c:\\test\\T_UCRT0P2"),arguments);
    if (err==KErrNotFound)
        err=me.Create(_L("z:\\sys\\bin\\T_UCRT0P2"),arguments);
	test(err==KErrNone);

	//Checking that the child proces T_UCRT0P2 executes properly with no panic
	TRequestStatus status;
	me.Logon(status);
	me.Resume();
	User::WaitForRequest(status);
	//Test to make sure the child process ended correctly
	test(status==KErrNone);
	test(me.ExitReason()==KErrNone);
	me.Close();
	}


/**
@SYMTestCaseID           SYSLIB-STDLIB-UT-4003
@SYMTestCaseDesc	     When there are heap memory allocation failures in __crt0, any locally allocated
                         heap memory should be cleaned up before the pointers go out of scope, preventing a memory leak.
@SYMTestPriority         Medium
@SYMTestActions  	     Start new process (T_UCRT0P3) passing arguments. Set heap allocation to fail at certain
                         points in __crt0. Clean up any locally allocated before routine returns. Check pointers
                         are set to NULL and there are no memory leaks. 
@SYMTestExpectedResults  No memory is leaked and pointer are NULL.
@SYMDEF                  DEF124477
*/
void Defect_DEF124477()
	{
	test.Next(_L("@SYMTestCaseID:SYSLIB-STDLIB-UT-4003 - Defect_DEF124477..."));
	
	TBuf<20> arguments(_L("random arguements"));

	test.Printf(_L("Create a new process passing in the arguements\n"));
	//Creating another process to run T_UCRT0P3 passing in the arguments. 
	// T_UCRT0P3 should only be run via this test and not on it's own.
	TInt err=me.Create(_L("T_UCRT0P3"),arguments);
	if (err==KErrNotFound)
		err=me.Create(_L("z:\\test\\T_UCRT0P3"),arguments);
	test(err==KErrNone);

	//Checking that the child proces T_UCRT0P3 executes properly with no panic
	TRequestStatus status;
	me.Logon(status);
	me.Resume();
	User::WaitForRequest(status);
	//Test to make sure the child process ended correctly
	test(status==KErrNone);
	test(me.ExitReason()==KErrNone);
	me.Close();
	}

/**
Invoke the tests
*/
LOCAL_C void RunTestsL()
    {
	Defect_DEF074278();
	#ifdef _DEBUG	
		Defect_DEF124477();  // OOM related test.
	#endif
	}


GLDEF_C TInt E32Main()
    {
	test.Title();
	test.Start(_L(" Running T_UCRT0P1 tests..."));
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	test(cleanup != NULL);
	TRAPD(error,RunTestsL());
	test(error==KErrNone);
	test.End();
	test.Close();
	delete cleanup;
	__UHEAP_MARKEND;
	return 0;
    }

