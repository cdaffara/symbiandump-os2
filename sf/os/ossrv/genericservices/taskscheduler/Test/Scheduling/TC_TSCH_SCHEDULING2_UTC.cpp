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
#include <s32file.h>

#include "TestUtils.h"


//If suddenly all SCHSVR tests begin failing, the OOM conditions might be the reason.
//TScheduling test tries to create KNumberOfSchedulesToCreate tasks, loading enormously 
//the task scheduling server. The task scheduling server fails and stays loaded in memory
//with many scheduling tasks holding large amounts of allocated memory in this way.
//The next SCHSVR tests may fail because of OOM.
//KNumberOfSchedulesToCreate value was 100 originally. Now it is 20.

//
// Literal constants
//
_LIT(KTestName,								"TC_TSCH_SCHEDULING2 - UTC");

//
// Type definitions
//
typedef CArrayFixFlat<TScheduleEntryInfo2>	CSchEntryInfoArray;
typedef CArrayFixFlat<TTaskInfo>			CTaskInfoArray;
typedef CArrayFixFlat<TSchedulerItemRef>    CSchItemRefArray;

//
// Global data
//
RTest										TheTest(KTestName);
static TInt64								TheSeed;
static RScheduler							TheScheduler;
static CTrapCleanup*						TheCleanup;
static RFs									TheFsSession;

_LIT(KMinimalTaskHandler, "MinimalTaskHandler");


//***********************************************************************************
// Extract task info from the schedule server based on a schedule ID
static void GetTaskInfoL(CTaskInfoArray& aTaskInfoArray,
						TInt aScheduleId)
	{
	aTaskInfoArray.Reset();
	TTsTime nextTimeScheduleIsDue;
	TScheduleState2 state;
	CSchEntryInfoArray* entries 
		= new (ELeave) CSchEntryInfoArray(3);
	CleanupStack::PushL(entries);
	TInt res = TheScheduler.GetScheduleL(aScheduleId, 
										state, 
										*entries, 
										aTaskInfoArray, 
										nextTimeScheduleIsDue);
	TEST2(res, KErrNone);
	CleanupStack::PopAndDestroy(entries);
	}

// count the number of tasks associated with this schedule
static TInt CountTasksL(TInt aScheduleId)	
	{
	CTaskInfoArray* tasks = new (ELeave) CTaskInfoArray(3);
	CleanupStack::PushL(tasks);
	GetTaskInfoL(*tasks, aScheduleId);
	TInt ret = tasks->Count();
	CleanupStack::PopAndDestroy(tasks);
	return ret;
	}

// count the number of schedules based on a filter.
static TInt CountScheduledItemsL(TScheduleFilter aFilter, 
								RScheduler& aScheduler)
	{
	CSchItemRefArray* refs = new (ELeave) CSchItemRefArray(3);
	CleanupStack::PushL(refs);

	TInt res = aScheduler.GetScheduleRefsL(*refs, aFilter);
	TEST2(res, KErrNone);

	TInt count = refs->Count();
	CleanupStack::PopAndDestroy(); // refs
	return count;
	}

//creates a daily schedule with StartTime of aStartTime
static TInt CreateScheduleL(TSchedulerItemRef& aRef, 
							RScheduler& aScheduler, 
							const TTsTime& aStartTime)
	{
	CSchEntryInfoArray* entryList 
		= new (ELeave) CSchEntryInfoArray(1);
	CleanupStack::PushL(entryList);

	TScheduleEntryInfo2 entry1 (aStartTime, EDaily, 1, 30);
	entryList->AppendL(entry1);
	TInt res = aScheduler.CreatePersistentSchedule(aRef, *entryList);
	CleanupStack::PopAndDestroy(); // entryList
	return res;
	}
	
// creates a schedule with numerous entries	
static TInt CreateSchedule1L(TSchedulerItemRef& aRef, RScheduler& aScheduler)
	{
	aRef.iName = _L("Schedule created using \"CreateSchedule1L\"");

	CSchEntryInfoArray* entryList = new (ELeave) CSchEntryInfoArray(3);
	CleanupStack::PushL(entryList);

	{
	TTsTime startTime1(SchSvrHelpers::UtcTimeBasedOnOffset(10, 0), ETrue); // 0m:10s from "now"
	TScheduleEntryInfo2 entry1 (startTime1, EDaily, 1, 20);
	entryList->AppendL(entry1);
	}
	{
	TTsTime startTime2(SchSvrHelpers::UtcTimeBasedOnOffset(20, 1), ETrue); // 0m:20s from "now"
	TScheduleEntryInfo2 entry2 (startTime2, EDaily, 1, 20);
	entryList->AppendL(entry2);
	}
	{
	TTsTime startTime3(SchSvrHelpers::UtcTimeBasedOnOffset(0, 0, 0, 0, 0, -1), ETrue); // -1 year from "now"
	TScheduleEntryInfo2 entry3 (startTime3, EDaily, 1, 20);
	entryList->AppendL(entry3);
	}

	TInt res = aScheduler.CreatePersistentSchedule(aRef, *entryList);
	CleanupStack::PopAndDestroy(); // entryList
	return res;
	}

// creates a schedule with numerous entries	
static TInt CreateSchedule2L(TSchedulerItemRef& aRef, RScheduler& aScheduler)
	{
	CSchEntryInfoArray* entryList = new (ELeave) CSchEntryInfoArray(3);
	CleanupStack::PushL(entryList);

	aRef.iName = _L("Schedule created using \"CreateSchedule2L\"");

	{
	TTsTime startTime1(SchSvrHelpers::UtcTimeBasedOnOffset(30, 2), ETrue); // 2m:30s from "now"
	TScheduleEntryInfo2 entry1 (startTime1, EDaily, 1, 20);
	entryList->AppendL(entry1);
	}
	{
	TTsTime startTime2(SchSvrHelpers::UtcTimeBasedOnOffset(0, 5), ETrue);  // 5m:00s from "now"
	TScheduleEntryInfo2 entry2 (startTime2, EDaily, 1, 20);
	entryList->AppendL(entry2);
	}

	TInt res = aScheduler.CreatePersistentSchedule(aRef, *entryList);
	CleanupStack::PopAndDestroy(); // entryList
	return res;
	}

// creates a schedule with numerous entries	
static TInt CreateSchedule3L(TSchedulerItemRef& aRef, RScheduler& aScheduler)
	{
	CSchEntryInfoArray* entryList = new (ELeave) CSchEntryInfoArray(3);
	CleanupStack::PushL(entryList);

	aRef.iName = _L("Schedule created using \"CreateSchedule3L\"");

	{
	TTsTime startTime1(SchSvrHelpers::TimeBasedOnOffset(0, 9, 0, 20), ETrue); // 9mins and 20days in the future
	TScheduleEntryInfo2 entry1 (startTime1, EDaily, 1, 5); // repeat every day and valid for only 5 minutes
	entryList->AppendL(entry1);
	}
	
	{
	TTsTime startTime2(SchSvrHelpers::TimeBasedOnOffset(0, 11, 0, 20), ETrue); // 11mins and 20days in the future
	TScheduleEntryInfo2 entry2 (startTime2, EDaily, 1, 5);
	entryList->AppendL(entry2);
	}

	TInt res = aScheduler.CreatePersistentSchedule(aRef, *entryList);
	CleanupStack::PopAndDestroy(); // entryList

	return res;
	}

// schedules a persistent task associated with the supplied schedule ID
static TInt SchedulePersistentTaskL(const TName& aName, 
									TInt& aNewId, 
									TInt aScheduleId, 
									TInt aRepeat, 
									RScheduler& aScheduler)
	{
	TTaskInfo taskInfo;
	taskInfo.iTaskId = aNewId;
	taskInfo.iName = aName;
	taskInfo.iPriority = 2;
	taskInfo.iRepeat = aRepeat;
	HBufC* data = _L("the data").AllocLC();
	TInt res = aScheduler.ScheduleTask(taskInfo, *data, aScheduleId);
	aNewId = taskInfo.iTaskId;

	CleanupStack::PopAndDestroy(); // data
	return res;
	}

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0265
@SYMTestCaseDesc 			Replicated test for for defect (EDNAWIR-4FQJ6A) - UTC
@SYMTestPriority 			High
@SYMTestActions  			Register twice with SchSvr and check that there are no memery leaks
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void Test1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0265 TheTest1: Registering with the tasks scheduler without disconnecting "));
	
	__UHEAP_MARK;
	TheTest.Next(_L("Connect to Scheduler"));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	TheScheduler.Close();
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	TheTest.Next(_L("Connect to Scheduler again"));
	res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering client again"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);
	TheScheduler.__DbgMarkHeap();

	TheTest.Next(_L("Register when already registered"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	TheTest.Next(_L("Cancel registering client and check for memory leak"));
	TheScheduler.__DbgMarkEnd(0);
	TheScheduler.Close();
	__UHEAP_MARKEND;
	}


//***********************************************************************************

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0266
@SYMTestCaseDesc 			Replicated test for for defect (EDNHLJT-4TRAAE) - UTC
@SYMTestPriority 			High
@SYMTestActions  			Create schedule, kill server (simulate re-boot), and make sure task still completes
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void Test2L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0266 TheTest2: Resend after hard reset (simulation) "));
	TheTest.Next(_L("Connect to Scheduler"));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	// Create a schedule
	TheTest.Next(_L("Creating schedule"));
	TSchedulerItemRef scheduleHandle;
	TTime time;	
	time.UniversalTime();
	time += TTimeIntervalSeconds(5); //Task to go off five seconds from now
	TTsTime time2 (time, ETrue);
	User::LeaveIfError(CreateScheduleL(scheduleHandle, TheScheduler, time2));  

	// Add a task to the schedule
	TheTest.Next(_L("Creating task for schedule"));
	{
	TTaskInfo taskInfo;
	taskInfo.iName = _L("MyTaskName");
	taskInfo.iPriority = 2;
	taskInfo.iTaskId = 0;
	taskInfo.iRepeat = 1;
	HBufC* data = _L("Task Data").AllocLC();
	TInt res = TheScheduler.ScheduleTask(taskInfo, *data, scheduleHandle.iHandle);
	CleanupStack::PopAndDestroy(); // data
	TEST2(res, KErrNone);
	}

	// Kill the server !!
	TheTest.Next(_L("Killing server"));
	// Need to turn off JIT dubugging as we are panicking server and we 
	// want test to keep running.
	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse);

	TheScheduler.__FaultServer();
	User::After(100000);
	
	// Turn on JIT again.
	User::SetJustInTime(jit);
	
	// Connect to the server again
	TheTest.Next(_L("Re-connect to Scheduler"));
	res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	// Re-register
	TheTest.Next(_L("Re-register Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	TheTest.Next(_L("Check schedule count and task count"));
	TInt scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1);
	TInt taskCount = CountTasksL(scheduleHandle.iHandle);
	TEST(taskCount == 1);
	
	// Wait for task to fire... It should happen in about 5 seconds
	TheTest.Next(_L("Waiting for task to complete"));	
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone);
	CleanupHelpers::KillProcess(KMinimalTaskHandler);
	}
	
//***********************************************************************************

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0267
@SYMTestCaseDesc 			Replicated test for CR (AALR-4EDG75) - UTC
@SYMTestPriority 			High
@SYMTestActions  			Test changes to Schedule Server API
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void Test3L()
	{
	//
	//
	// Test changes to Schedule Server API as of Change Request document AALR-4EDG75
	// (GT change requests database)
	//
	//
	//
	// This test establishes that the change to...
	//
	//	RScheduler::GetScheduleL(const TInt aScheduleHandle,
	//								 TScheduleState& aState,
	//								 CArrayFixFlat<TScheduleEntryInfo>& aEntries,
	//								 CArrayFixFlat<TTaskInfo>& aTasks,
	//								 TTime& aNextDue)
	//
	// ...is functionally correct.
	// 
	//

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0267 Test change request AALR-4EDG75 implementation"));
	
	TheTest.Next(_L("Connect to Scheduler"));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	const TDateTime KTimeToStartTask(2000, EJanuary, 10, 15, 30, 0, 0);

	TSchedulerItemRef schedulerItemReference;
	CSchEntryInfoArray* entryArray = new(ELeave) CSchEntryInfoArray(1);
	CleanupStack::PushL(entryArray);
	
	HBufC* taskData = _L("This is some dummy task data created for testing").AllocL();
	CleanupStack::PushL(taskData);
	
	// Prepare the task info - this describes the tasks that are contained within the task
	// entry array
	TTaskInfo taskInfo = SchSvrHelpers::TaskInfo(_L("A transient test task"), 100);
		
	// Create a schedule entry and append it to the entry array
	{
	TScheduleEntryInfo2 scheduleEntry = SchSvrHelpers::ScheduleEntryInfo(EDaily, TTsTime(TTime(KTimeToStartTask), ETrue), 7, 2);
	entryArray->AppendL(scheduleEntry);
	}

	// Create the transient task
	TInt ret = TheScheduler.ScheduleTask(taskInfo, *taskData, schedulerItemReference, *entryArray);
	TEST2(ret, KErrNone);

	// Check that the task Id after scheduling the event is not 
	// the same as it was prior to the requesting the service
	TEST(taskInfo.iTaskId != -1);

	//
	// Now obtain info about the scheduled transient task...
	//
	TScheduleState2 scheduleState;
	TTsTime nextTaskDueTime;

	// Reset the schedule entry info array as the server now has copies of this and it is
	// no longer required client-side
	entryArray->Reset();
	CTaskInfoArray* taskInfoArray = new(ELeave) CTaskInfoArray(4);
	CleanupStack::PushL(taskInfoArray);

	// Request task schedule information from the server
	ret = TheScheduler.GetScheduleL(schedulerItemReference.iHandle, scheduleState, *entryArray, *taskInfoArray, nextTaskDueTime);
	TEST2(ret, KErrNone);

	// Because this is a daily task which is scheduled to occur at a specific time (but the date
	// cannot necessarily be established, we can perform a simple check to ensure that the
	// time when the task is next scheduled to run falls within the 15:30 - 17:30 bracket.
	TEST(SchSvrHelpers::IsTimeTheSame(nextTaskDueTime, TTsTime(TTime(KTimeToStartTask), ETrue)) == (TInt) ETrue);

	// Establish and test the size of the task data for the specified task object
	TInt sizeOfTaskData = 0;
	TEST2(TheScheduler.GetTaskDataSize(taskInfo.iTaskId, sizeOfTaskData), KErrNone);
	TEST(sizeOfTaskData == taskData->Length());

	// Now check the information return from the server pertaining to a specific task...
	{
	TTaskInfo taskFromServer;
	HBufC* taskDataFromServer					= HBufC::NewLC(sizeOfTaskData);
	TPtr pTaskDataFromServer					= taskDataFromServer->Des();
	TTime nullTime								= Time::NullTTime();
	TTsTime nextDueTimeFromServer (nullTime, ETrue);
	TSchedulerItemRef schedulerRefFromServer;
	TEST2(TheScheduler.GetTaskInfoL(taskInfo.iTaskId, taskFromServer, pTaskDataFromServer, schedulerRefFromServer, nextDueTimeFromServer), KErrNone);
	TEST(SchSvrHelpers::IsTimeTheSame(nextTaskDueTime, TTsTime(TTime(KTimeToStartTask), ETrue)) == (TInt) ETrue);
	
	TEST(SchSvrHelpers::IsTaskInfoTheSame(taskFromServer, taskInfo) == (TInt) ETrue);
	TEST(SchSvrHelpers::IsItemRefTheSame(schedulerRefFromServer, schedulerItemReference) == (TInt) ETrue);
	CleanupStack::PopAndDestroy(); // taskDataFromServer
	}

	// Disable the schedule and check when it is next schedule to run
	TEST2(TheScheduler.DisableSchedule(schedulerItemReference.iHandle), KErrNone);

	// Get the new schedule info - check that the nextDueTime is still reported even
	// though the schedule has been disabled
	
	nextTaskDueTime.SetUtcTime(Time::NullTTime());
	TEST2(TheScheduler.GetScheduleL(schedulerItemReference.iHandle, scheduleState, *entryArray, *taskInfoArray, nextTaskDueTime), KErrNone);
	TEST(SchSvrHelpers::IsTimeTheSame(nextTaskDueTime, TTsTime(TTime(KTimeToStartTask), ETrue)) == (TInt) ETrue);
	TEST(SchSvrHelpers::IsTaskInfoTheSame(taskInfoArray->At(0), taskInfo) == (TInt) ETrue);

	// Re-enable the schedule
	TEST2(TheScheduler.EnableSchedule(schedulerItemReference.iHandle), KErrNone);

	// Delete the only task (relating to this test) from the server
	TEST2(TheScheduler.DeleteTask(taskInfo.iTaskId), KErrNone);
	ret = TheScheduler.GetScheduleL(schedulerItemReference.iHandle, scheduleState, *entryArray, *taskInfoArray, nextTaskDueTime);
	TEST2(ret, KErrNotFound); // there is no longer any tasks associated with this schedule

	CleanupStack::PopAndDestroy(3); // taskInfoArray, entryArray, taskData
	}

//***********************************************************************************

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0268
@SYMTestCaseDesc 			Replicated test for defect (EDNAALR-4JKEFC) - UTC
@SYMTestPriority 			High
@SYMTestActions  			Create 2 schedules with repeating tasks, kill server and check that tasks complete
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void Test4L()
	{
	// Test title
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0268 \nTest for defect EDNAALR-4JKEFC"));
	
	TheTest.Next(_L("Connect to Scheduler"));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	// Constants / vars used in this function
	TSchedulerItemRef itemRef;

	// Create some scheduling entries
	CArrayFixFlat<TScheduleEntryInfo2>* entries = new(ELeave) CArrayFixFlat<TScheduleEntryInfo2>(10);
	CleanupStack::PushL(entries);

	TTsTime startTime1 (SchSvrHelpers::TimeBasedOnOffset(1, 1), ETrue); // 1m:01s from "now"
	TScheduleEntryInfo2 entry1 (startTime1, EHourly, 1, 20);
	entries->AppendL(entry1);

	TTsTime startTime2 (SchSvrHelpers::TimeBasedOnOffset(5, 5), ETrue); // 5m:05s from "now"
	TScheduleEntryInfo2 entry2 (startTime2, EHourly, 1, 20);
	entries->AppendL(entry2);

	// Create the schedule for the task...
	res = TheScheduler.CreatePersistentSchedule(itemRef, *entries);
	TEST2(res, KErrNone);

	// Create the tasks themselves..
	TTaskInfo task;
	task.iRepeat	= 10; // repeat once
	task.iName		= _L("Test Task For Defect Verification");
	task.iPriority	= 100;
	HBufC* taskData = task.iName.AllocLC();
	res = TheScheduler.ScheduleTask(task, *taskData, itemRef.iHandle);
	CleanupStack::PopAndDestroy(); // taskData

	{
	CArrayFixFlat<TSchedulerItemRef>* refs = new CArrayFixFlat<TSchedulerItemRef>(3);
	CleanupStack::PushL(refs);
	TInt res = TheScheduler.GetScheduleRefsL(*refs, EAllSchedules); 
	TEST2(res, KErrNone);
	CleanupStack::PopAndDestroy(); // refs
	}

	// Re-register theclient with the server
	for(TInt i=0; i<5; i++)
		{
		// Log off from the task scheduler
		TheScheduler.Close();
		res = TheScheduler.Connect();
		TEST2(res, KErrNone);

		User::After(1000000);

		TheTest.Next(_L("===== Re-registering Client (wait a second) ====="));
		res = SchSvrHelpers::RegisterClientL(TheScheduler);
		TEST2(res, KErrNone);
			{
			CArrayFixFlat<TSchedulerItemRef>* refs = new CArrayFixFlat<TSchedulerItemRef>(3);
			CleanupStack::PushL(refs);
			TInt res = TheScheduler.GetScheduleRefsL(*refs, EAllSchedules);
			TEST2(res, 0);
			CleanupStack::PopAndDestroy(); // refs
			}

		// Check the information that the scheduler knows about...
		TInt taskDataSize = 0;
		res = TheScheduler.GetTaskDataSize(task.iTaskId, taskDataSize);
		TEST2(res, KErrNone);
		TEST(taskDataSize == task.iName.Length());
		TTaskInfo taskInfoFromServer;
		taskData = HBufC::NewLC(taskDataSize);
		TPtr pTaskData = taskData->Des();

		TTsTime nextDueTime(Time::NullTTime(), ETrue);
		res = TheScheduler.GetTaskInfoL(task.iTaskId, taskInfoFromServer, pTaskData, itemRef, nextDueTime);
		TEST2(res, KErrNone);
		TEST(pTaskData == task.iName);
		CleanupStack::PopAndDestroy(); // taskData
		}

	CleanupStack::PopAndDestroy(); // entries

	}

//***********************************************************************************

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0269
@SYMTestCaseDesc 			Persistant schedule test - UTC
@SYMTestPriority 			High
@SYMTestActions  			Mark heap, create persistent schedules, schedule tasks, transient schedules, delete tasks, delete remaing schedules, check heap
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void Test5L()
	{
	TInt res = KErrNone;
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0269 ===== Starting test 1 ====="));
	
	__UHEAP_MARK;
	
	TSchedulerItemRef ref1;
	TSchedulerItemRef ref2;
	TSchedulerItemRef ref3;

	// Remove all existing schedules before starting the test
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);
	
	TInt scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 0); // check that no schedules are present.
	
	TheTest.Printf(_L("Create some schedules\n"));
	res = CreateSchedule1L(ref1, TheScheduler); // +10sec, +20sec, -1year
	TEST2(res, KErrNone);	
	res = CreateSchedule2L(ref2, TheScheduler); // +2min 30sec, +5min
	TEST2(res, KErrNone);
	res = CreateSchedule3L(ref3, TheScheduler); // +20day 9min, +20day 11min
	TEST2(res, KErrNone);

	TSchedulerItemRef ref4;
	TSchedulerItemRef ref5;
	res = CreateSchedule2L(ref4, TheScheduler); // +2min 30sec, 5min
	TEST2(res, KErrNone);
	res = CreateSchedule3L(ref5, TheScheduler); // +20day 9min, +20day 11min
	TEST2(res, KErrNone);

	TInt task1 = 0;
	TInt task2 = 0;
	TInt task3 = 0;
	TInt task4 = 0;
	TName name1 = (_L("web subscription"));
	TName name2 = (_L("another web subscription"));
	TName name3 = (_L("third web subscription"));

	TheTest.Printf(_L("Schedule some tasks\n"));

	// NOTE: have to put repeats here of > 0 otherwise the task will run immediately
	// (because it's schedule specifies a date of 1 year in the past!) and be
	// removed (by the server) before we have a chance to delete it....
	res = SchedulePersistentTaskL(name1, task1, ref1.iHandle, 3, TheScheduler);
	TEST2(res, KErrNone);
	res = SchedulePersistentTaskL(name2, task2, ref2.iHandle, 3, TheScheduler);
	TEST2(res, KErrNone);
	res = SchedulePersistentTaskL(name3, task3, ref3.iHandle, 3, TheScheduler);
	TEST2(res, KErrNone);
	res = SchedulePersistentTaskL(name3, task4, ref3.iHandle, 3, TheScheduler);
	TEST2(res, KErrNone);

	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 5); // 5 persistant
	CleanupHelpers::KillProcess(KMinimalTaskHandler);

	TheTest.Printf(_L("Deleting task with id %d\n"), task1);
	res = TheScheduler.DeleteTask(task1);
	TEST2(res, KErrNone);
	TheTest.Printf(_L("Deleting schedule with id %d\n"), ref1.iHandle);
	res = TheScheduler.DeleteSchedule(ref1.iHandle);
	TEST2(res, KErrNone);
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	// 4 persistant expected as we have deleted one
	TEST(scheduleCount == 4); 

	TheTest.Printf(_L("Deleting task with id %d\n"), task2);
	res = TheScheduler.DeleteTask(task2);
	TEST2(res, KErrNone);
	TheTest.Printf(_L("Deleting schedule with id %d\n"), ref2.iHandle);
	res = TheScheduler.DeleteSchedule(ref2.iHandle);
	TEST2(res, KErrNone);
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	// 3 persistant  expected as we have deleted one
	TEST(scheduleCount == 3); 

	TheTest.Printf(_L("Deleting task with id %d\n"), task3);
	res = TheScheduler.DeleteTask(task3);
	TEST2(res, KErrNone);
	TheTest.Printf(_L("Deleting task with id %d\n"), task4);
	res = TheScheduler.DeleteTask(task4);
	TEST2(res, KErrNone);
	TheTest.Printf(_L("Deleting schedule with id %d\n"), ref3.iHandle);
	res = TheScheduler.DeleteSchedule(ref3.iHandle);
	TEST2(res, KErrNone);
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	// 2 persistant  expected as we have deleted one
	TEST(scheduleCount == 2); 

	TheTest.Printf(_L("Deleting schedule with id %d\n"), ref4.iHandle);
	res = TheScheduler.DeleteSchedule(ref4.iHandle);
	TEST2(res, KErrNone);
	TheTest.Printf(_L("Deleting schedule with id %d\n"), ref5.iHandle);
	res = TheScheduler.DeleteSchedule(ref5.iHandle);
	TEST2(res, KErrNone);

	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 0); 
	
	SchSvrHelpers::Pause(TheTest,1);
	__UHEAP_MARKEND;
	}


//***********************************************************************************

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0270
@SYMTestCaseDesc 			Transient schedule test - UTC
@SYMTestPriority 			High
@SYMTestActions  			Create transient schedule with non-repeating task
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void Test6L()
	{
	// Heap testing removed because this is a flakey bit of code.
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0270 Transient, non-repeating - waits for schedule to activate "));
	
	// Remove all existing schedules before starting the test
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);

	// Create transient schedule
	TheTest.Printf(_L("Create transient schedule with non-repeating task\n"));
	TSchedulerItemRef ref;
	CSchEntryInfoArray* entryList = new(ELeave) CSchEntryInfoArray(1);
	CleanupStack::PushL(entryList);
	ref.iName = _L("A Transient Schedule");
	
	// CREATE SCHEDULE ENTRY:
	// starttime 5 secs in the future, hometime based
	TTsTime startTime(SchSvrHelpers::TimeBasedOnOffset(5), EFalse);
	// constructor takes: starttime, interval type, interval, validity period
	TScheduleEntryInfo2 entry(startTime, EDaily, 1, 20);
	entryList->AppendL(entry);
	 
	// CREATE SCHEDULE TASK:
	TTaskInfo taskInfo;
	taskInfo.iName = _L("mail");
	taskInfo.iTaskId = 0;
	taskInfo.iRepeat = 1;
	taskInfo.iPriority = 2;
	HBufC* data = _L("This is the data for the task").AllocLC();
	
	// Schedule the item
	TInt res = TheScheduler.ScheduleTask(taskInfo, *data, ref, *entryList);
	TEST2(res, KErrNone);

	CleanupStack::PopAndDestroy(2, entryList); // data, entryList
	
	// Should be one item
	TEST(CountScheduledItemsL(EAllSchedules, TheScheduler) == 1);

	// Waiting for entry to complete
	TheTest.Next(_L("Waiting for task to complete"));
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone);	

	CleanupHelpers::KillProcess(KMinimalTaskHandler);
	
	// Should be no items as schedule deletes itself after task has completed
	TEST(CountScheduledItemsL(EAllSchedules, TheScheduler) == 0);

	SchSvrHelpers::Pause(TheTest,1);
	}


//***********************************************************************************

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0271
@SYMTestCaseDesc 			Persistant schedule test with repeating task - UTC
@SYMTestPriority 			High
@SYMTestActions  			Persistent schedule, repeating task, non-repeating task, go off, check task's still there, go off again, check it's still there, delete task, delete schedule, 
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void Test7L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0271 Test persistent scheduling, repeating and non-repeating task schedules"));
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);

	// Transient
	TSchedulerItemRef ref;

	// We shouldn't have any outstanding schedules registered with the server
	TInt count = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(count == 0);

	// This creates 3 schedule entries, each with a validity period of 20 minutes, and are
	// due to run in 10s, 20s, and over a year ago (a year in the past)
	TheTest.Printf(_L("Create Persistent schedule\n"));
	TInt res = CreateSchedule1L(ref, TheScheduler);
	TEST2(res, KErrNone);

	// We should now have one registered schedule
	count = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(count == 1);

	//
	TheTest.Printf(_L("\nSchedule two tasks: one repeating....\n"));
	//
	TInt task1 = 0;
	TName name1 = (_L("web subscription(rpts)"));
	// -1 indicates repeating schedule
	res = SchedulePersistentTaskL(name1, task1, ref.iHandle, -1, TheScheduler); // -1 indicates repeat until explicitly deleted
	TEST2(res, KErrNone);

	// List those schedules that are pending
	count = CountScheduledItemsL(EPendingSchedules, TheScheduler);
	TEST(count == 1);

	//
	TheTest.Printf(_L("\n... and one non-repeating\n"));
	//
	TInt task2 = 0;
	TName name2 = (_L("non-repeating"));
	res = SchedulePersistentTaskL(name2, task2, ref.iHandle, 1, TheScheduler); // only runs once
	TEST2(res, KErrNone);

	TheTest.Printf(_L("Waiting for tasks to run\n"));
	// Wait for notification that schedule has executed.	
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone);

	CleanupHelpers::KillProcess(KMinimalTaskHandler);

	TheTest.Printf(_L("...and wait again for repeating one to execute again\n"));
	// Wait for notification that schedule has executed.
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone);

	CleanupHelpers::KillProcess
	(KMinimalTaskHandler);

	TheTest.Printf(_L("Delete the repeating task, and the schedule \n"));
	res = TheScheduler.DeleteTask(task1);
	TEST2(res, KErrNone);
	res = TheScheduler.DeleteTask(task2);
	TEST2(res, KErrNotFound); //Should be not found since its only executed once.
	res = TheScheduler.DeleteSchedule(ref.iHandle);
	TEST2(res, KErrNone);

	count = CountScheduledItemsL(EPendingSchedules, TheScheduler);
	TEST(count == 0);
	SchSvrHelpers::Pause(TheTest,1);
	}


static CSchEntryInfoArray* CreateSchEntryInfoArrayLC(TInt aGranularity)
	{
	CSchEntryInfoArray*	scheduleEntries	= new (ELeave) CSchEntryInfoArray(aGranularity);
	CleanupStack::PushL(scheduleEntries);
	return scheduleEntries;
	}

static CArrayFixFlat<TTaskInfo>* CreateTaskInfoLC(TInt aGranularity)
	{
	CArrayFixFlat<TTaskInfo>* taskInfos	= new (ELeave) CArrayFixFlat<TTaskInfo>(aGranularity);
	CleanupStack::PushL(taskInfos);
	return taskInfos;
	}

static CSchItemRefArray* CreateScheduleRefsLC(TInt aGranularity)
	{
	CSchItemRefArray* scheduleReferences = new (ELeave) CSchItemRefArray(aGranularity);
	CleanupStack::PushL(scheduleReferences);
	return scheduleReferences;
	}

static void CreateTransientScheduledTasksL(TInt aNumScheduleEntries,
										   TInt aNumSchedules,
										   CSchEntryInfoArray* aScheduleEntries,
										   CArrayFixFlat<TTaskInfo>* aTaskInfos,
										   CSchItemRefArray* aScheduleReferences)
	{
	const TTimeIntervalMicroSeconds timeToAdd = 10000000; //10 seconds
	const TTimeIntervalMicroSeconds timeLimit = 5000000; // 5 seconds
	_LIT(KTaskDataPrefix, "Task Data Entry: ");
	// Prepare keys required
	TKeyArrayFix KTaskInfoSortKey(_FOFF(TTaskInfo, iTaskId), ECmpTInt);

	for(TInt i=0;i<aNumSchedules;i++)
		{	
		// Remove any existing schedule entry info's
		aScheduleEntries->Reset();
		//
		// Populate the schedule entry array with a varying list of 
		// start-times, intervals, etc for this particular task
		//
		for(TInt j=0; j<aNumScheduleEntries; ++j)
			{
			TScheduleEntryInfo2 scheduleEntry = SchSvrHelpers::RandomScheduleEntryInfoUtc(TheSeed);
			TTime now;
			now.HomeTime(); // sets now to home time
			TTime sTime = scheduleEntry.StartTime().GetUtcTime();
			//if scheduleEntry.StartTime() is set lower then 5 sec into the future, postpone StartTime 10 sec into the future
			if(sTime < now + timeLimit) 
				{
				TTsTime newTime (now + timeToAdd, ETrue);
				scheduleEntry.SetStartTime(newTime); 					
				}
			aScheduleEntries->AppendL(scheduleEntry);
			}
		//
		// Create some dummy task data
		//
		HBufC* taskData = HBufC::NewLC(KTaskDataPrefix().Length()+4);
		taskData->Des().Copy(KTaskDataPrefix());
		taskData->Des().AppendNum(i);
		//
		// Munge the task name
		//
		TTaskInfo taskInfo;
		taskInfo.iName = *taskData;
		taskInfo.iRepeat = 1;
		taskInfo.iPriority = 1;
		// Schedule the transient task
		TSchedulerItemRef scheduleReference;
		TInt result = TheScheduler.ScheduleTask(taskInfo, 
												*taskData, 
												scheduleReference, 
												*aScheduleEntries);
		TEST2(result, KErrNone);
		TheTest.Printf(_L("TaskId: %d => TaskName: %S\n"), taskInfo.iTaskId, &taskInfo.iName);
		CleanupStack::PopAndDestroy(taskData);
		//
		// Save the taskInfo so we can check it later - this inserts the taskinfo into
		// the array (preserves sorted order by TTaskInfo.iTaskId) but also has the
		// added bonus of preventing duplicate task ids...
		//
		aTaskInfos->InsertIsqL(taskInfo, KTaskInfoSortKey);
		// Disable all schedules once added, just to stop them going off
		// and therefore being deleted when we are trying to compare them 
		result = TheScheduler.DisableSchedule(scheduleReference.iHandle);
		TEST2(result, KErrNone);
		// Save the sever generated schedule reference and taskId for checking later
		aScheduleReferences->AppendL(scheduleReference);
		}
	}

static void CheckScheduledRefs(TInt aNumSchedules)
	{
	CSchItemRefArray* refs = new (ELeave) CSchItemRefArray(3);
	CleanupStack::PushL(refs);
	TInt res = TheScheduler.GetScheduleRefsL(*refs, EAllSchedules); 
	TEST2(res, KErrNone);
	TInt count = refs->Count();
	TEST(count == aNumSchedules);
	CleanupStack::PopAndDestroy(refs);
	}

static void TestScheduledTasksL(TInt aNumSchedules,
								CArrayFixFlat<TTaskInfo>* aTaskInfos)
	{
	for(TInt n=0; n<aNumSchedules; ++n)
		{
		const TTaskInfo& taskInfo = aTaskInfos->At(n);

		// First retrieve the task size
		TInt taskSize;
		TInt result = TheScheduler.GetTaskDataSize(taskInfo.iTaskId, taskSize);
		TEST2(result, KErrNone);
		TEST(taskSize > 0);

		// Next retrieve the task info associated with a particular task Id
		HBufC* taskData = HBufC::NewLC(taskSize);
		TPtr pTaskData = taskData->Des();

		TTsTime scheduleNextDueTime;
		TTaskInfo taskFromServer;
		TSchedulerItemRef scheduleReference;

		result = TheScheduler.GetTaskInfoL(taskInfo.iTaskId,
										   taskFromServer, 
										   pTaskData, 
										   scheduleReference, 
										   scheduleNextDueTime);
		TEST2(result, KErrNone);
		TEST(taskData->Length() == taskSize);

		// Now check that the task returned by the server matches that held locallly....
		TBool bbb = SchSvrHelpers::IsTaskInfoTheSame(taskFromServer, taskInfo);
		if(!bbb)
			{
			RDebug::Print(_L("TaskInfo1. repeat=%x, id=%d, name=%S, priority=%x\n"),
				taskFromServer.iRepeat, taskFromServer.iTaskId, &taskFromServer.iName, taskFromServer.iPriority);
			RDebug::Print(_L("TaskInfo2. repeat=%x, id=%d, name=%S, priority=%x\n"),
				taskInfo.iRepeat, taskInfo.iTaskId, &taskInfo.iName, taskInfo.iPriority);
			}
		TEST(bbb);
		
		// Check taskData is the same (was originally held in the TTaskInfo.iName field)
		const TDesC& des1 = taskInfo.iName;
		TDes& des2 = pTaskData;
		TEST(des1 == des2);
		CleanupStack::PopAndDestroy(taskData);
		}
	}

//***********************************************************************************

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0272
@SYMTestCaseDesc 			Large number of tasks test - UTC
@SYMTestPriority 			High
@SYMTestActions  			Create a large number of tasks, test retrieval of task data and task info
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void Test8L()
	{
	// Test title
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0272 Create a large number of tasks, test retrieval of task data and task info"));
	
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);

	// Constants used in this function
	const TInt KNumberOfSchedulesToCreate		= 20;
	const TInt KNumberOfScheduleEntriesToCreate = 5;

	// Prepare arrays required for the tests below
	CSchEntryInfoArray* scheduleEntries = ::CreateSchEntryInfoArrayLC(KNumberOfScheduleEntriesToCreate);
	CArrayFixFlat<TTaskInfo>* taskInfos = ::CreateTaskInfoLC(KNumberOfSchedulesToCreate);
	CSchItemRefArray* scheduleReferences = ::CreateScheduleRefsLC(KNumberOfSchedulesToCreate);

	//
	// Create a large number of transient scheduled tasks 
	// to test Id generation
	//
	::CreateTransientScheduledTasksL(KNumberOfScheduleEntriesToCreate,
									 KNumberOfSchedulesToCreate,
									 scheduleEntries,
									 taskInfos,
									 scheduleReferences);


	::CheckScheduledRefs(KNumberOfSchedulesToCreate);

	// Test tasks for a given taskid is the same
	::TestScheduledTasksL(KNumberOfSchedulesToCreate, taskInfos);

	// Test reference can be retrieved for a given handle.
	CleanupStack::PopAndDestroy(scheduleReferences);
	CleanupStack::PopAndDestroy(taskInfos);
	CleanupStack::PopAndDestroy(scheduleEntries);

	// now delete all schedules
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);
	TInt ccc = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(ccc == 0);

	SchSvrHelpers::Pause(TheTest,1);
	}

//***********************************************************************************
static TInt RunTestsL()
	{
	TheTest.Next(_L("Delete old files"));
	SchSvrHelpers::DeleteScheduleFilesL();
	
	TheTest.Next(_L("Create Task notification semaphore"));
	//initialise task notification semaphore
	STaskSemaphore sem;
	sem.CreateL();

	// Prepare random number seed
	TheTest.Next(_L("Prepare random number"));
	TTime now;
	now.UniversalTime();
	TheSeed = now.Int64();

	// Connect to the server
	TheTest.Next(_L("===== Connect to Scheduler ====="));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);
	
	// Register a client with the server
	TheTest.Next(_L("===== Registering Client ====="));
	res = SchSvrHelpers::RegisterClientL(TheScheduler);
	TEST2(res, KErrNone);

	TheTest.Next(_L("Start tests"));
	Test1L();
	Test2L();
	Test3L();
	Test4L();
	Test5L();
	Test6L();
	Test7L();
	Test8L();
	
	TheTest.Next(_L("Tidying up"));
	//Tidying up so next test will be clear.
	TheTest.Next(_L("Delete all schedules"));
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);
	SchSvrHelpers::Pause(TheTest, 2);
	TheTest.Next(_L("Delete old files\n"));
	SchSvrHelpers::DeleteScheduleFilesL();

	TheScheduler.Close();
	
	//close handle to semaphore
	sem.Close();

	return KErrNone;
	}

//***********************************************************************************
GLDEF_C TInt E32Main()
//	
// TheTest the scheduler
//
    {
	__UHEAP_MARK;
	TheTest.Start(_L("TC_TSCH_SCHEDULING2 - UTC"));
	TheTest.Title();

	TheCleanup = CTrapCleanup::New();
	//If the previous test fails, SCHSVR.exe may stay in memory.
	TRAPD(error,CleanupHelpers::TestCleanupL());
	TEST2(error, KErrNone);
	
	TTime now;
	now.HomeTime();
	// Used to Set the system UTC time and UTC offset
	// so that correct UTC Time values are returned while using time based APIs.
	SchSvrHelpers::SetHomeTimeL(now);

	TEST2(TheFsSession.Connect(), KErrNone);;
	TRAP(error, RunTestsL());	
	TRAP(error,CleanupHelpers::TestCleanupL());
	TEST2(error, KErrNone);
	delete TheCleanup;
	
	TheFsSession.Close();
	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
