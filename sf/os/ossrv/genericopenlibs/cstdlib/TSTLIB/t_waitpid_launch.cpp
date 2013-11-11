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
// T_WAITPIDLAUNCH.CPP
// 
//

#include <e32cons.h>
#include <e32test.h>


LOCAL_D RTest test(_L("WAITPID DEF073739"));
LOCAL_D RProcess me;


/**
@SYMTestCaseID 			SYSLIB-STDLIB-UT-1575
@SYMTestCaseDesc	    Testing if POSIX waitpid function conforms to POSIX standard
@SYMTestPriority 	    High
@SYMTestActions  	    Starts a new process (T_WAITPID) this new process
						creates some processes and tests the POSIX waitpid function behaves correctly
						by waiting for these processes.
@SYMTestExpectedResults The test should not fail.
@SYMDEF 				INC073739
*/

//INC073739 Incorrect error codes returned from waitpid() in STDLIB
void Defect_073739()
	{


	test.Printf(_L("Create a new process t_waitpid to test POSIX waitpid\n"));
	//Creating another process to run T_WAITPID and test if it completed correctly
	TInt err=me.Create(_L("T_WAITPID"),_L(""));
	if (err==KErrNotFound)
		err=me.Create(_L("z:\\test\\T_WAITPID"),_L(""));
	if (err==KErrNotFound)
		err=me.Create(_L("c:\\test\\T_WAITPID"),_L(""));
	if (err==KErrNotFound)
		err=me.Create(_L("c:\\sys\\bin\\T_WAITPID"),_L(""));
	test(err==KErrNone);

	//Checking that the child proces T_WAITPID executes properly with no panic
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
LOCAL_C void RunTestsL ()
    {
	Defect_073739();
	}


GLDEF_C TInt E32Main()
    {
	test.Title();
	test.Start(_L(" @SYMTestCaseID:SYSLIB-STDLIB-UT-1575 Starting the T_WAITPID tests... "));
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
