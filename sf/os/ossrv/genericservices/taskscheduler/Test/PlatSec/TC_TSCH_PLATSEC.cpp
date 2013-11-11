// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <csch_cli.h>
#include "Thelpers.h"

#include <e32base.h>
#include <e32test.h>
#include <f32file.h>

#include "TestUtils.h"
#include "platsectaskcommon.h"

_LIT(KTestName,	"Task Scheduler platform security Test");

_LIT(KProcess1, "tschsvrclient1");
_LIT(KProcess2, "tschsvrclient2");
_LIT(KProcess3, "tschsvrclient3");

RTest	TheTest(KTestName);

typedef CArrayFixFlat<TScheduleEntryInfo>	CSchEntryInfoArray;
typedef CArrayFixFlat<TTaskInfo>			CTaskInfoArray;
typedef CArrayFixFlat<TSchedulerItemRef>    CSchItemRefArray;

static RScheduler	TheScheduler;
static CTrapCleanup*	TheCleanup;
static RFs			TheFsSession;

//creates a daily schedule with StartTime of aStartTime
static TInt CreateScheduleL(TSchedulerItemRef& aRef, 
							RScheduler& aScheduler, 
							const TTime& aStartTime)
	{
	CSchEntryInfoArray* entryList 
		= new (ELeave) CSchEntryInfoArray(1);
	CleanupStack::PushL(entryList);

	TScheduleEntryInfo entry1;
	entry1.iStartTime		= aStartTime;
	entry1.iInterval		= 1; // TTimeIntervalDays
	entry1.iIntervalType	= EDaily;
	entry1.iValidityPeriod	= 30; // minutes
	entryList->AppendL(entry1);
	TInt res = aScheduler.CreatePersistentSchedule(aRef, *entryList);
	CleanupStack::PopAndDestroy(); // entryList
	return res;
	}

static void LaunchClient(TInt aScheduleHandle, 
						TInt aTaskHandle,
						const TDesC& aClient,
						TInt aErrorCondition,
						TInt aScheduleCount,
						TInt aTaskCount)
	{
	// now launch process to try and access schedule
	TRequestStatus stat;
	RProcess client;

	TBuf<30> id;
	id.Append(':');
	id.AppendNum(aScheduleHandle);
	id.Append(':');
	id.AppendNum(aTaskHandle);
	id.Append(':');
	id.AppendNum(aErrorCondition);
	id.Append(':');
	id.AppendNum(aScheduleCount);
	id.Append(':');
	id.AppendNum(aTaskCount);
	id.Append(':');
	
	TInt res = client.Create(aClient, id);
	TEST2(res, KErrNone);
	// Asynchronous logon: completes when process terminates with process exit code
	client.Logon(stat);
	client.Resume();

	User::WaitForRequest(stat);

	TInt exitReason = client.ExitReason();
	TEST2(exitReason, KErrNone);
	client.Close();	
	}

/**
@SYMTestCaseID SYSLIB-SCHSVR-CT-0025
@SYMTestCaseDesc Check that schedules/tasks cannot be manipulated by unauthorised clients
@SYMTestPriority High
@SYMTestActions  Ensure permission denied for client with the wrong SID and without WriteDeviceData
                 Ensure permission granted for client with the wrong SID but with WriteDeviceData
@SYMTestExpectedResults Only the schedule creator or clients with WriteDeviceData can manipulate schedules/tasks.
@SYMPREQ 277 Ensure integrity of Symbian OS handsets
*/
static void DoTest1L()
	{
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0025 platformsec - API policing "));

	TheTest.Next(_L("Connect to Scheduler"));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	// Create a schedule
	TheTest.Next(_L("Creating schedule"));
	TSchedulerItemRef scheduleHandle;
	TTime time;
	time.HomeTime();
	time += TTimeIntervalHours(1);  //Task to go off one hour from now (ie we dont 
									//want it going off for the purposes of this test)
	User::LeaveIfError(CreateScheduleL(scheduleHandle, TheScheduler, time));  

	// Add a task to the schedule
	TheTest.Next(_L("Creating task for schedule"));
	TTaskInfo taskInfo;
	taskInfo.iName = _L("MyTaskName");
	taskInfo.iPriority = 2;
	taskInfo.iTaskId = 0;
	taskInfo.iRepeat = 1;
	HBufC* data = _L("Tasks Data").AllocLC();
	res = TheScheduler.ScheduleTask(taskInfo, *data, scheduleHandle.iHandle);
	CleanupStack::PopAndDestroy(); // data
	TEST2(res, KErrNone);
	
	// Now launch client process with a different SID to ours
	// and all capabilities excluding WriteDeviceData (and TCB)
	// Client operations should fail with KErrPermissionDenied
	TheTest.Next(_L("Launch Client1"));
	LaunchClient(scheduleHandle.iHandle, 
					taskInfo.iTaskId, 
					KProcess1, 
					KErrPermissionDenied,
					0,
					0);
	
	TheScheduler.Close();
	// Now shut down the server and restart.
	TheTest.Next(_L("shuting down and restarting server"));
	CleanupHelpers::TestCleanupL();
	res = TheScheduler.Connect();
	TEST2(res, KErrNone);
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);
	
	//Check we still have 1 schedule and 1 task.
	TheTest.Next(_L("Checking schedule info and launching client"));
	CSchItemRefArray* refs = new (ELeave) CSchItemRefArray(3);
	CleanupStack::PushL(refs);
	res = TheScheduler.GetScheduleRefsL(*refs, EAllSchedules); 
	TEST2(res, KErrNone);
	TInt count = refs->Count();
	TEST2(count, 1); 
	scheduleHandle.iHandle = refs->At(0).iHandle;
	
	res = TheScheduler.GetTaskRefsL(*refs, EAllSchedules, EAllTasks);
	TEST2(res, KErrNone);
	count = refs->Count();
	TEST2(count, 1); 
	taskInfo.iTaskId = refs->At(0).iHandle;
	
	CleanupStack::PopAndDestroy(refs);
	
	// Client operations should fail with KErrPermissionDenied
	TheTest.Next(_L("Launch client1 again"));
	LaunchClient(scheduleHandle.iHandle, 
					taskInfo.iTaskId, 
					KProcess1, 
					KErrPermissionDenied,
					0,
					0);

	// Try with client with no 3rd UID.
	// Client operations should fail with KErrPermissionDenied
	TheTest.Next(_L("Launch client3"));
	LaunchClient(scheduleHandle.iHandle, 
					taskInfo.iTaskId, 
					KProcess3, 
					KErrPermissionDenied,
					0,
					0);
	
	// Now try with a client with WriteDeviceData.
	// All operations should succeed
	TheTest.Next(_L("Launch client2"));
	LaunchClient(scheduleHandle.iHandle, 
					taskInfo.iTaskId, 
					KProcess2, 
					KErrNone,
					1,
					1);
	
	TheScheduler.Close();
	
	SchSvrHelpers::Pause(TheTest);
	}

/**
@SYMTestCaseID SYSLIB-SCHSVR-CT-0026
@SYMTestCaseDesc Check that the scheduled exe receives the security info of the schedule creator.
@SYMTestPriority High
@SYMTestActions  Check that the scheduled exe receives the security info of the schedule creator.
@SYMTestExpectedResults The scheduled exe receives the security info of the schedule creator.
@SYMPREQ 277 Ensure integrity of Symbian OS handsets
*/
static void DoTest2L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0026 platformsec - TSecurityInfo validity "));
	TheTest.Next(_L("Connect to Scheduler"));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering Client"));
	TFileName filename;
	filename = _L("PlatSecTaskHandler");
	TEST2(TheScheduler.Register(filename, 27), KErrNone);

	// Create a schedule
	TheTest.Next(_L("Creating schedule"));
	TSchedulerItemRef scheduleHandle;
	TTime time;
	time.HomeTime();
	time += TTimeIntervalSeconds(2);
	User::LeaveIfError(CreateScheduleL(scheduleHandle, TheScheduler, time));  

	// Add a task to the schedule
	TheTest.Next(_L("Creating task for schedule"));
	TTaskInfo taskInfo;
	taskInfo.iName = KPlatSecTaskName();
	taskInfo.iPriority = 2;
	taskInfo.iTaskId = 0;
	taskInfo.iRepeat = 1;
	HBufC* data = KPlatSecTaskData().AllocLC();
	res = TheScheduler.ScheduleTask(taskInfo, *data, scheduleHandle.iHandle);
	CleanupStack::PopAndDestroy(); // data
	TEST2(res, KErrNone);
	
	TheTest.Next(_L("Wait for task to fire"));
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone);
		
	SchSvrHelpers::Pause(TheTest);
	}

/**
@SYMTestCaseID SYSLIB-SCHSVR-CT-1885
@SYMTestCaseDesc Check that TTaskInfo states correctly persisted
@SYMTestPriority High
@SYMTestActions Setup a schedule and task and then close the server, reconnect and check that the schedule and task
				info has been persisted correctly.
@SYMTestExpectedResults The test must not panic or fail.
@SYMDEF INC093573
*/	
static void DoTest3L()
	{	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-1885 INC093573: Symbian provided task scheduler (RScheduler) loses parts of pending schedule inf "));
	TheTest.Next(_L("Connect to Scheduler"));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	// Create a schedule
	TheTest.Next(_L("Creating schedule"));
	TSchedulerItemRef scheduleHandle;
	TTime time;
	time.HomeTime();
	time += TTimeIntervalHours(1);  //Task to go off one hour from now (ie we dont 
									//want it going off for the purposes of this test)
	User::LeaveIfError(CreateScheduleL(scheduleHandle, TheScheduler, time));  

	// Add a task to the schedule
	TheTest.Next(_L("Creating task for schedule"));
	TTaskInfo taskInfo;
	taskInfo.iName = _L("INC093573");
	taskInfo.iPriority = 2;
	taskInfo.iTaskId = 0;
	taskInfo.iRepeat = -1;
	HBufC* data = _L("Tasks Data").AllocLC();
	res = TheScheduler.ScheduleTask(taskInfo, *data, scheduleHandle.iHandle);
	CleanupStack::PopAndDestroy(); // data
	TEST2(res, KErrNone);
	
	TheScheduler.Close();

	// Now shut down the server and restart.
	TheTest.Next(_L("shuting down and restarting server"));
	CleanupHelpers::TestCleanupL();
	res = TheScheduler.Connect();
	TEST2(res, KErrNone);
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);
	
	//Check that the task is still there with all the correct settings
	TheTest.Next(_L("Checking schedule info and launching client"));
	TTaskInfo returnedInfo;
	TSchedulerItemRef returnedItemRef;
	TTsTime returnedTsTime;
	TInt size=0;
	res=TheScheduler.GetTaskDataSize(0,size);
	TEST2(res,KErrNone);	
	HBufC* buffer=HBufC::NewL(size);
	TPtr modifiableBuffer=buffer->Des();

	//now get the task info and compare to the original submitted to ensure that it is properly
	//externalized when server shut down.
	res=TheScheduler.GetTaskInfoL(0,returnedInfo,modifiableBuffer,returnedItemRef,returnedTsTime);
	TEST2(res,KErrNone);
	TEST2(returnedInfo.iPriority,taskInfo.iPriority);
	TEST2(returnedInfo.iRepeat,taskInfo.iRepeat);	
	TEST2(returnedInfo.iTaskId,taskInfo.iTaskId);
	TEST2(returnedInfo.iName.Compare(taskInfo.iName),0);			

	//now clear the buffer
	delete buffer;
	}

static TInt RunTestsL()
	{
	TheTest.Next(_L("Delete old files"));
	SchSvrHelpers::DeleteScheduleFilesL();

	TheTest.Next(_L("Create Task notification semaphore"));
	//initialise task notification semaphore
	STaskSemaphore sem;
	sem.CreateL();

	// Connect to the server
	TheTest.Next(_L("===== Connect to Scheduler ====="));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	// Register a client with the server
	TheTest.Next(_L("===== Registering Client ====="));
	res = SchSvrHelpers::RegisterClientL(TheScheduler);
	TEST2(res, KErrNone);
	
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
	TheTest.Next(_L("Start tests"));
	
	if(PlatSec::IsCapabilityEnforced(ECapabilityWriteDeviceData))
	 	DoTest1L();
 	DoTest2L();
	DoTest3L();
	TheTest.Next(_L("Tidying up"));
	CleanupStack::PopAndDestroy(scheduler);
	//close handle to semaphore
	sem.Close();
	
	//Tidying up so next test will be clear.
	TheTest.Next(_L("Delete all schedules"));
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);
	SchSvrHelpers::Pause(TheTest, 2);
	TheTest.Next(_L("Delete old files\n"));
	SchSvrHelpers::DeleteScheduleFilesL();

	TheScheduler.Close();
	return KErrNone;
	}

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	TheTest.Start(_L("TC_TSCH_PLATSEC"));
	TheTest.Title();

	TheCleanup = CTrapCleanup::New();
	//If the previous test fails, SCHSVR.exe may stay in memory.
	TRAPD(error, CleanupHelpers::TestCleanupL());
	TEST2(error, KErrNone);

	TheTest(TheFsSession.Connect() == KErrNone);
	
	TRAP(error, RunTestsL());	
	TEST2(error,KErrNone);
	
	TRAP(error,CleanupHelpers::TestCleanupL());
	TEST2(error, KErrNone);
	delete TheCleanup;	
	
	TheFsSession.Close();
	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;

	return KErrNone;
	}
