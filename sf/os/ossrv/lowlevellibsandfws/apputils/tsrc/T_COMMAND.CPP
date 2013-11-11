// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

LOCAL_D RTest test(_L("t_command"));
LOCAL_D RProcess me;

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0485
@SYMTestCaseDesc        Tests the behaviour of long command line arguments
@SYMTestPriority        High
@SYMTestActions         Tests for long command line arguments by creating another process.
@SYMTestExpectedResults Tests must not fail
@SYMREQ                 REQ0000
*/
void RunChildProcessL()
	{
	//passing a very long command line argument with more than 256 characters
	TBuf<300> longargument(_L("verylongargument verylongargumentverylongargumentverylongargumentverylongargumentverylongargumentverylongargumentverylongargumentverylongargumentverylongargumentverylongargumentverylongargumentverylongargumentverylongargumentverylongargumentverylongargumentverylongargument"));
	test.Printf(_L("Test for command line argument with %d characters \n"),longargument.Length());

	//Creating another process to run T_CLINE with very long command line arguments
	TInt err=me.Create(_L("T_CLINE"),longargument);
	if (err==KErrNotFound)
		err=me.Create(_L("z:\\test\\T_CLINE"),longargument);
	test(err==KErrNone);

	//Checking that the child proces T_CLINE executes properly with no panic
	TRequestStatus status;
	me.Logon(status);
	me.Resume();
	User::WaitForRequest(status);
	test(status==KErrNone);

	test(me.ExitType()==EExitKill);
	test(me.ExitReason()==KErrNone);
	me.Close();


	}



GLDEF_C TInt E32Main()
    {
	test.Title();
	test.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0485 Starting tests... "));
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAPD(error,RunChildProcessL());
	test(error==KErrNone);
	test.End();
	test.Close();
	delete cleanup;
	__UHEAP_MARKEND;
	return 0;
    }

