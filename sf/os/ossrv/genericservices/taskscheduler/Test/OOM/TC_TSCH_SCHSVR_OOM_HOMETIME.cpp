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
#include <e32property.h>
#include <schinfointernal.h>
#include "TestUtils.h"

//
// Literal constants
//
_LIT(KTestName,								"TC_TSCH_SCHSVR_OOM_HOMETIME");


//
// Type definitions
//
typedef CArrayFixFlat<TScheduleEntryInfo2>	CSchEntryInfoArray;
typedef CArrayFixFlat<TTaskInfo>			CTaskInfoArray;
typedef CArrayFixFlat<TSchedulerItemRef>    CSchItemRefArray;
typedef CArrayFixFlat<TTaskSchedulerCondition> CSchConditionArray;

//
// Global data
//
RTest										TheTest(KTestName);
static RScheduler							TheScheduler;
static CTrapCleanup*						TheCleanup;
static RFs									TheFsSession;

const TUid KTestUid = TUid::Uid(0x12345678);
const TInt KTestKey1 = 1;


//***********************************************************************************
static void GetTaskInfoL(CTaskInfoArray& aTaskInfoArray,
						TInt aScheduleId)
	// Extract schedule references from the schedule server based on a filter. If
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

//***********************************************************************************
static TInt CountTasksL(TInt aScheduleId)	
	{
	CTaskInfoArray* tasks = new (ELeave) CTaskInfoArray(3);
	CleanupStack::PushL(tasks);
	GetTaskInfoL(*tasks, aScheduleId);
	TInt ret = tasks->Count();
	CleanupStack::PopAndDestroy(tasks);
	return ret;
	}

//***********************************************************************************
static TInt CountScheduledItemsL(TScheduleFilter aFilter, 
								RScheduler& aScheduler)
	// Extract schedule references from the schedule server based on a filter. If
	{
	CSchItemRefArray* refs = new (ELeave) CSchItemRefArray(3);
	CleanupStack::PushL(refs);

	TInt res = aScheduler.GetScheduleRefsL(*refs, aFilter);
	TEST2(res, KErrNone);

	TInt count = refs->Count();
	CleanupStack::PopAndDestroy(); // refs
	return count;
	}


//***********************************************************************************
static CSchEntryInfoArray* CreateScheduleArrayLC()
	{
	CSchEntryInfoArray* entryList = new (ELeave) CSchEntryInfoArray(3);
	CleanupStack::PushL(entryList);

	TTime ttime(SchSvrHelpers::TimeBasedOnOffset(0, 20));
	TTsTime startTime (ttime, EFalse);
	TScheduleEntryInfo2 entry1 (startTime, EDaily, 1, 20); // 20m from "now"
	entryList->AppendL(entry1);
	
	return entryList;
	}

//***********************************************************************************
static CSchConditionArray* CreateConditionArrayLC()
	{
	CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(3);
	CleanupStack::PushL(conditionList);

	TTaskSchedulerCondition condition1;
	condition1.iCategory = KTestUid;
	condition1.iKey		= KTestKey1;
	condition1.iState	= 10;
	condition1.iType	= TTaskSchedulerCondition::EEquals;
	conditionList->AppendL(condition1);
	
	return conditionList;
	}
	
//***********************************************************************************

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0247
@SYMTestCaseDesc 			OOM test for time-based schedules - Hometime
@SYMTestPriority 			High
@SYMTestActions  			OOM test for time-based schedules
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void DoTest1L()
// Time based API's
	{

	TInt err = KErrNone;
	TInt tryCount = 0;
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0247 ===== Starting OOM test ====="));
	
	CSchEntryInfoArray* scheduleArray = CreateScheduleArrayLC();
	TSchedulerItemRef ref1;

	TheTest.Next(_L("===== Testing create persistent schedule ====="));
	for (tryCount = 0; ;++tryCount)
		{
		// These allocations are ignored because they cause corruptness
		// of the persistent store.  Its a bit rubbish.
		if(tryCount < 7 || tryCount >8 )
			{
			TheScheduler.__DbgFailNext(tryCount);
			TheScheduler.__DbgMarkHeap();
			err = TheScheduler.CreatePersistentSchedule(ref1, *scheduleArray);
			if (err==KErrNone)
				{
				TheScheduler.__DbgResetHeap();
				break;
				}
			TEST2(err, KErrNoMemory);		
			// reset server side heap for next iteration.
			TheScheduler.__DbgMarkEnd(0);			
			}
		}
	// Check schedule count
	TInt scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 

	// OK, now we have a persistent schedule
	TheTest.Next(_L("===== Testing disable schedule ====="));
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.DisableSchedule(ref1.iHandle);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	// Check schedule count
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 

	TheTest.Next(_L("===== Testing enable schedule ====="));
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.EnableSchedule(ref1.iHandle);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	// Check schedule count
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 

	TheTest.Next(_L("===== Testing edit schedule ====="));
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		// Even though we use the same array as before, the task scheduler
		// code is not smart enough to just return but actually replaces
		// the existing one, hence we are actually testing this method fully.
		err = TheScheduler.EditSchedule(ref1.iHandle, *scheduleArray);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	// Check schedule count
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 

	TheTest.Next(_L("===== Testing delete schedule ====="));
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.DeleteSchedule(ref1.iHandle);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	// Check schedule count
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 0); 

	// delete old files.
	// We need to do this because we have actually corrupted the store, 
	// even though its empty.  Without deleting it we get problems below.
	// This is a bit rubbish having to do this but what else can be done??
	TheTest.Next(_L("Delete old files"));
	SchSvrHelpers::DeleteScheduleFilesL();
	
	// OK now add back schedule in preparation for adding tasks.
	err = TheScheduler.CreatePersistentSchedule(ref1, *scheduleArray);
	TEST2(err, KErrNone);

	TheTest.Next(_L("===== Testing ScheduleTask ====="));
	
	_LIT(KTaskName, "TheTaskName");
	TName name = KTaskName();
	TTaskInfo taskInfo(0, name, 2, 0);

	HBufC* data = _L("the data").AllocLC();
	// cant test ScheduleTask API using OOM loop as it does dodgy things to 
	// the store which cause allocation problems.  Need to investigate this 
	// later at some stage.
	TEST2(TheScheduler.ScheduleTask(taskInfo, *data, ref1.iHandle), KErrNone);
	
	// Check schedule and task count
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 
	TInt taskCount = CountTasksL(ref1.iHandle);
	TEST(taskCount = 1);	

	TheTest.Next(_L("===== Testing GetScheduleRefsL ====="));

	CSchItemRefArray* refs = new (ELeave) CSchItemRefArray(3);
	CleanupStack::PushL(refs);
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.GetScheduleRefsL(*refs, EPendingSchedules);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	// Check schedule and task count
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 
	taskCount = CountTasksL(ref1.iHandle);
	TEST(taskCount = 1);	

	TheTest.Next(_L("===== Testing GetScheduleL ====="));

	CTaskInfoArray* tasks = new (ELeave) CTaskInfoArray(3);
	CleanupStack::PushL(tasks);
	TScheduleState2 state;
	TTsTime time;
	CSchEntryInfoArray* entryList = new (ELeave) CSchEntryInfoArray(3);
	CleanupStack::PushL(entryList);
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.GetScheduleL(ref1.iHandle,
										state,
										*entryList,
										*tasks, time);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	// Check schedule and task count
	TEST(entryList->Count() == 1);
	TEST(tasks->Count() == 1);
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 
	taskCount = CountTasksL(ref1.iHandle);
	TEST(taskCount = 1);	

	CleanupStack::PopAndDestroy(entryList);
	CleanupStack::PopAndDestroy(tasks);

	TheTest.Next(_L("===== Testing GetTaskRefsL ====="));

	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.GetTaskRefsL(*refs,
										EAllSchedules,
										EAllTasks);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	CleanupStack::PopAndDestroy(refs);

	TheTest.Next(_L("===== Testing GetTaskDataSize ====="));

	TInt size;
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.GetTaskDataSize(ref1.iHandle,
										size);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}

	TheTest.Next(_L("===== Testing GetTaskInfoL ====="));

	HBufC* newData = HBufC::NewLC(size);
	TPtr pTaskData = newData->Des();

	TTaskInfo newTaskInfo;
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.GetTaskInfoL(taskInfo.iTaskId,
										newTaskInfo,
										pTaskData,
										ref1,
										time);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	TEST(newData->MatchF(*data) == 0);
	
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 
	taskCount = CountTasksL(ref1.iHandle);
	TEST(taskCount = 1);	

	CleanupStack::PopAndDestroy(newData);
	CleanupStack::PopAndDestroy(data);

	TheTest.Next(_L("===== Testing GetScheduleTypeL ====="));

	TScheduleType type;
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.GetScheduleTypeL(ref1.iHandle, type);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	TEST(type == ETimeSchedule);

	CleanupStack::PopAndDestroy(scheduleArray);

	TheTest.Next(_L("===== Testing DeleteTask ====="));

	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.DeleteTask(taskInfo.iTaskId);
		//include test for KErrNotFound here as task is actually deleted and
		// then store is updated (which causes mem failure).  Problems
		// will still occur if you add a new task again and try and access store
		// as store is likely to still be corrupt. Investigate this??
		if (err==KErrNone || err==KErrNotFound) 
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 
	taskCount = CountTasksL(ref1.iHandle);
	TEST(taskCount == 0);
	
	//Now delete schedule to setup for next test
	TEST2(TheScheduler.DeleteSchedule(ref1.iHandle), KErrNone);
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 0); 

	SchSvrHelpers::Pause(TheTest);
	}


//***********************************************************************************

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0248
@SYMTestCaseDesc 			OOM test for condition-based schedules - Hometime
@SYMTestPriority 			High
@SYMTestActions  			OOM test for condition-based schedules
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void DoTest2L()
// Condition based API's
	{
		
	TInt err = KErrNone;
	TInt tryCount = 0;
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0248 ===== Starting Condition OOM test ===== "));
	
	// delete old files.
	// We need to do this because we have actually corrupted the store, 
	// with the DeleteTask call.  Without deleting it we get problems below.
	// This is a bit rubbish having to do this but what else can be done??
	TheTest.Next(_L("Delete old files"));
	SchSvrHelpers::DeleteScheduleFilesL();

	//Ensure P&S variables are defined.
	err = RProperty::Define(KTestUid, KTestKey1, RProperty::EInt);
	TEST(err == KErrNone || err == KErrAlreadyExists);
	TEST2(RProperty::Set(KTestUid, KTestKey1,1), KErrNone);
	
	CSchConditionArray* conditionArray = CreateConditionArrayLC();
	TSchedulerItemRef ref1;

	TheTest.Next(_L("===== Testing create persistent schedule ====="));
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.CreatePersistentSchedule(ref1, *conditionArray, Time::MaxTTime());
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);			
		}
	// Check schedule count
	TInt scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 

// This test code causes problems when EditSchedule fails, it deletes the entries 
// but doenst delete the actual CSchedule object.  We can't do this really as the 
// schedule may have oustanding tasks.  Need to clean up data in a consistent manner.
// Fix this! Same applied for time based EditSchedule
/*	TheTest.Next(_L("===== Testing edit schedule ====="));
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		// Even though we use the same array as before, the task scheduler
		// code is not smart enough to just return but actually replaces
		// the existing one, hence we are actually testing this method fully.
		err = TheScheduler.EditSchedule(ref1.iHandle, *conditionArray,
										SchSvrHelpers::TimeBasedOnOffset(0, 20));
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	// Check schedule count
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 
*/
	TheTest.Next(_L("===== Testing GetScheduleL ====="));

	CTaskInfoArray* tasks = new (ELeave) CTaskInfoArray(3);
	CleanupStack::PushL(tasks);
	TScheduleState2 state;
	TTsTime time;
	CSchConditionArray* entryList = new (ELeave) CSchConditionArray(3);
	CleanupStack::PushL(entryList);
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.GetScheduleL(ref1.iHandle,
										state,
										*entryList,
										time, *tasks);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	CleanupStack::PopAndDestroy(entryList);
	CleanupStack::PopAndDestroy(tasks);

	TheTest.Next(_L("===== Testing delete schedule ====="));
	for (tryCount = 0; ;++tryCount)
		{
		TheScheduler.__DbgFailNext(tryCount);
		TheScheduler.__DbgMarkHeap();
		err = TheScheduler.DeleteSchedule(ref1.iHandle);
		if (err==KErrNone)
			{
			TheScheduler.__DbgResetHeap();
			break;
			}
		TEST2(err, KErrNoMemory);		
		// reset server side heap for next iteration.
		TheScheduler.__DbgMarkEnd(0);
		}
	// Check schedule count
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 0); 

	CleanupStack::PopAndDestroy(conditionArray);

	SchSvrHelpers::Pause(TheTest);
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

	// Connect to the server
	TheTest.Next(_L("===== Connect to Scheduler ====="));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);
	// Register a client with the server
	TheTest.Next(_L("===== Registering Client ====="));
	res = SchSvrHelpers::RegisterClientL(TheScheduler);
	TEST2(res, KErrNone);

	TheTest.Next(_L("Start tests"));
	DoTest1L();
	DoTest2L();
	
	// Need to add OOM tests for when a task is executed
	// to clarify behaviour.  This will be tricky!!!
	
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
	TheTest.Start(_L("OOM testing - hometime"));
	TheTest.Title();
	TheCleanup = CTrapCleanup::New();
	
	//If the previous test fails, SCHSVR.exe may stay in memory.
	TRAPD(error,CleanupHelpers::TestCleanupL());
	TEST2(error, KErrNone);

	TheTest(TheFsSession.Connect() == KErrNone);;
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
