/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Demonstration use of the TestExecuteUtils client API
* Relies on test config files being exported to z:\SampleTest
*
*/



/**
 @file SampleClient.cpp
*/
#include <test/testexecuteclient.h>

LOCAL_C void MainL()
	{
	// Create a root session with the SampleServer test server
	// User::After() 1/10th second delays between server calls because IPC calls fail with
	// KErrServerBusy if we bang them in to fast.
	RTestServ server;
	User::LeaveIfError(server.Connect(_L("SampleServer")));
	CleanupClosePushL(server);
	
	RTestSession session1;
	User::LeaveIfError(session1.Open(server,_L("SampleStep1")));
	CleanupClosePushL(session1);
	TRequestStatus status1;
	User::After(100000);
	// Run a test step then pick up completion
	TExitCategoryName panicString1;
	session1.RunTestStep(_L("z:\\Sampletest\\SampleTest.ini SectionOne"),panicString1,status1);
	User::WaitForRequest(status1);
	// Demonstration of another step run on the same session
	// We have to wait for the first step to complete otherwise we get KErrInUse
	// If we want to run multiple instances then we create another test session
	panicString1.Zero();
	session1.RunTestStep(_L("z:\\Sampletest\\SampleTest.ini SectionTwo"),panicString1,status1);
	
	// Get another session whilst the other one is still running
	RTestSession session2;
	User::After(100000);
	User::LeaveIfError(session2.Open(server,_L("SampleStep1")));
	CleanupClosePushL(session2);
	TRequestStatus status2;
	User::After(100000);
	TExitCategoryName panicString2;
	session2.RunTestStep(_L("z:\\Sampletest\\SampleTest.ini SectionTwo"),panicString2,status2);
	
	// Get another session whilst the other two are still running
	RTestSession session3;
	User::After(100000);
	User::LeaveIfError(session3.Open(server,_L("SampleStep1")));
	CleanupClosePushL(session3);
	TRequestStatus status3;
	User::After(100000);
	TExitCategoryName panicString3;
	session3.RunTestStep(_L("z:\\Sampletest\\SampleTest.ini SectionTwo"),panicString3,status3);

	// The test steps all ran concurrently so pick up the completions
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);

	session1.Close();
	session2.Close();
	session3.Close();

	CleanupStack::Pop(3);
	CleanupStack::Pop(&server);
	// Close the root server session
	server.Close();
	// Wait for flogger to log
	User::After(5000000);
	}

// Entry point for all Epoc32 executables
// See PSP Chapter 2 Getting Started
GLDEF_C TInt E32Main()
	{
	// Heap balance checking
	// See PSP Chapter 6 Error Handling
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	_LIT(KPanic,"SampleClient");
	__ASSERT_ALWAYS(!err, User::Panic(KPanic,err));
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }
