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
// TESTFIXES.CPP
// 
//

#include <e32base.h>
#include <e32test.h>
#include <csch_cli.h>
#include <f32file.h>
#include "Thelpers.h"
#include "TestUtils.h"

// Globals
static RTest			TheTest(_L("TestFixes"));
static RScheduler		TheScheduler;
static RFs				TheFsSession;

// Type definitions
typedef CArrayFixFlat<TScheduleEntryInfo>	CScheduleEntryInfoArray;
typedef CArrayFixFlat<TTaskInfo>			CTaskInfoArray;

//***********************************************************************************
static TInt RegisterClient(RScheduler& aScheduler)
//
//	Utility function to simplify registering a client with the task scheduler
//
	{
	TFileName filename = _L("Z:\\System\\Bin\\MinimalTaskHandler.exe");
	return aScheduler.Register(filename, 27);
	}

//***********************************************************************************

/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-1023
@SYMTestCaseDesc	    Tests for defect EDNHLJT-4WDEJV
						A Kern Exec3 panic occurs 30 minutes after scheduling 300 tasks to complete at once.
@SYMTestPriority 	    High
@SYMTestActions  	    Create the schedule for the task.Create 300 tasks and enable the schedule and wait for 30 seconds.Check if any panic occurs
@SYMTestExpectedResults Check if any panic occurs
@SYMREQ                 REQ0000
*/
static void Test1L()
	{	
	// Test title
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-1023 \nTest for defect EDNHLJT-4WDEJV "));
	// A Kern Exec3 panic occurs after 30ish minutes after scheduling 300 tasks to complete at once.
	
	// Constants / vars used in this function
	TSchedulerItemRef itemRef;

	TheTest.Next(_L("Connect to Scheduler"));
	TInt res = TheScheduler.Connect();
	TheTest (res==KErrNone);

	TheTest.Next(_L("Registering Client"));
	TheTest (RegisterClient(TheScheduler) == KErrNone);
	
	// Create some scheduling entries
	CArrayFixFlat<TScheduleEntryInfo>* entries = new(ELeave) CArrayFixFlat<TScheduleEntryInfo>(10);
	CleanupStack::PushL(entries);

	TScheduleEntryInfo entry1;
	entry1.iIntervalType	= EHourly;
	entry1.iStartTime		= SchSvrHelpers::TimeBasedOnOffset(1);
	entry1.iInterval		= 1;
	entry1.iValidityPeriod	= 20;
	entries->AppendL(entry1);

	// Create the schedule for the task...
	res = TheScheduler.CreatePersistentSchedule(itemRef, *entries);
	TheTest(res == KErrNone);

	// Disable the schedule straight away
	res = TheScheduler.DisableSchedule(itemRef.iHandle);
	TheTest(res == KErrNone);
	
	if	(res == KErrNone)
		{
		for(TInt i=0; i<300; i++)
			{
			// Create the tasks themselves..
			TTaskInfo task;
			task.iRepeat	= 1; // repeat once
			task.iName		= _L("Test Task For Defect Verification");
			task.iPriority	= 100;
			//
			HBufC* taskData = HBufC::NewMaxLC(100);
			taskData->Des().Repeat(_L(" "));
			//
			res = TheScheduler.ScheduleTask(task, *taskData, itemRef.iHandle);
			TheTest(res == KErrNone);

			CleanupStack::PopAndDestroy(taskData);
			}
		}

	res = TheScheduler.EnableSchedule(itemRef.iHandle);
	TheTest(res == KErrNone);

	CleanupStack::PopAndDestroy(entries);

	TheTest.Printf(_L("Waiting for 30 minutes\n"));
	// Wait for thirty minutes to see if a Kern Exec3 panic  occurs 
	User::After(1000000 * 60 * 30); // thirty mins

	}

//***********************************************************************************

GLDEF_C TInt DoTheTestsL()
//
//	Kickoff method
//
	{
	// Add tests here:-
	Test1L();

	return KErrNone;
	}

//***********************************************************************************
GLDEF_C TInt E32Main()
//	
// Entry point
//
    {
	__UHEAP_MARK;
	TheTest.Title();
	TheTest.Start(_L("TheTest Task Scheduler Fixes"));

	TInt error = KErrNone;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		return KErrNoMemory;

	TheFsSession.Connect();
	TRAP(error, DoTheTestsL());
	TEST(error == KErrNone);
	TheFsSession.Close();
	TheTest(error == KErrNone);
	delete cleanup;	

	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
