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

_LIT(KTestName,	"Task Scheduler Condition Scheduling Test - UTC");

RTest	TheTest(KTestName);

typedef CArrayFixFlat<TTaskInfo>			CTaskInfoArray;
typedef CArrayFixFlat<TSchedulerItemRef>    CSchItemRefArray;
typedef CArrayFixFlat<TTaskSchedulerCondition>	CSchConditionArray;

static RScheduler	TheScheduler;
static CTrapCleanup*	TheCleanup;
static RFs			TheFsSession;

const TInt KTestKey1 = 1;
const TInt KTestKey2 = 2;
const TInt KTestKey3 = 3;

_LIT(KMinimalTaskHandler, "MinimalTaskHandler");
_LIT(KSeparator, "|"); // Invalid filepath char used to separate filenames

// This function launches the TPropertyDefine process which
//	has WriteDeviceData Capabilities enabling it to create the P&S 
//	variables used by this test.
static void LaunchHelperL(TUid aCategory, TInt aKey, TInt aAttr)
	{
	_LIT(KConditionHelper, "TPropertyDefine");
	TRequestStatus stat;
	RProcess p;
	TBuf<32> args;
	args.AppendNum(aCategory.iUid);
	args.Append(KSeparator);
	args.AppendNum(aKey);
	args.Append(KSeparator);
	args.AppendNum(aAttr);
	User::LeaveIfError(p.Create(KConditionHelper, args,EOwnerProcess));
	
	// Asynchronous logon: completes when process terminates with process exit code
	p.Logon(stat);
	p.Resume();

	User::WaitForRequest(stat);
	TInt exitReason = p.ExitReason();
	p.Close();
	User::LeaveIfError(exitReason);
	}
static void CreateTestVariables()
	{
	LaunchHelperL(KUidSystemCategory, KTestKey1,RProperty::EInt);
	LaunchHelperL(KUidSystemCategory, KTestKey2,RProperty::EInt);
	LaunchHelperL(KUidSystemCategory, KTestKey3,RProperty::EInt);	
	}
	
static void ResetVariablesL(TInt aKey1Val,
							TInt aKey2Val,
							TInt aKey3Val)
	{
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KTestKey1,aKey1Val));
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KTestKey2,aKey2Val));
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KTestKey3,aKey3Val));		
	}
	
static void GetTaskInfoL(CTaskInfoArray& aTaskInfoArray,
						TInt aScheduleId)
	// Extract schedule references from the schedule server based on a filter. If
	{
	aTaskInfoArray.Reset();
	TTsTime defaultTime;
	TScheduleState2 state;
	CSchConditionArray* conditionList 
		= new (ELeave) CSchConditionArray(3);
	CleanupStack::PushL(conditionList);
	TInt res = TheScheduler.GetScheduleL(aScheduleId, 
										state, 
										*conditionList, 
										defaultTime,
										aTaskInfoArray);
	TEST2(res, KErrNone);
	CleanupStack::PopAndDestroy(conditionList);
	}

static TInt CountTasksL(TInt aScheduleId)	
	{
	CTaskInfoArray* tasks = new (ELeave) CTaskInfoArray(3);
	CleanupStack::PushL(tasks);
	GetTaskInfoL(*tasks, aScheduleId);
	TInt ret = tasks->Count();
	CleanupStack::PopAndDestroy(tasks);
	return ret;
	}
	

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

static CSchConditionArray* CreateSingleConditionLC(const TUid& aConditionUID,
									TUint aConditionUInt)
	{
	CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(3);
	CleanupStack::PushL(conditionList);

	{
	TTaskSchedulerCondition condition1;
	condition1.iCategory = aConditionUID;
	condition1.iKey		= aConditionUInt;
	condition1.iState	= 10;
	condition1.iType	= TTaskSchedulerCondition::EEquals;
	conditionList->AppendL(condition1);
	}
	return conditionList;			
	}

static CSchConditionArray* CreateMultipleConditionsLC()
	{
	CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(3);
	CleanupStack::PushL(conditionList);
	{
	TTaskSchedulerCondition condition1;
	condition1.iCategory = KUidSystemCategory;
	condition1.iKey		= KTestKey1;
	condition1.iState	= 10;
	condition1.iType	= TTaskSchedulerCondition::EEquals;
	conditionList->AppendL(condition1);
	}
	{
	TTaskSchedulerCondition condition2;
	condition2.iCategory = KUidSystemCategory;
	condition2.iKey		= KTestKey2;
	condition2.iState	= 10;
	condition2.iType	= TTaskSchedulerCondition::ENotEquals;
	conditionList->AppendL(condition2);
	}
	{
	TTaskSchedulerCondition condition3;
	condition3.iCategory = KUidSystemCategory;
	condition3.iKey		= KTestKey3;
	condition3.iState	= 10;
	condition3.iType	= TTaskSchedulerCondition::ELessThan;
	conditionList->AppendL(condition3);
	}
	return conditionList;		
	}
	
// single condition with default time set to 1 year in the future
// As this is a valid time a CTimer object actually gets set unlike 
// if Time::TTimeMax() is used, hence we need to test for both cases.
static TInt CreateScheduleSingle1L(TSchedulerItemRef& aRef, 
									RScheduler& aScheduler,
									const TUid& aConditionUID,
									TUint aConditionUInt)
	{
	aRef.iName = _L("Schedule created using CreateScheduleSingle");

	CSchConditionArray* conditionList 
		= CreateSingleConditionLC(aConditionUID, aConditionUInt);
		
	TTime ttime(SchSvrHelpers::UtcTimeBasedOnOffset(0, 0, 0, 0, 0, 1));
	TTsTime time(ttime, ETrue); //1 year in the future
	TInt res = aScheduler.CreatePersistentSchedule(aRef, *conditionList, time);
	CleanupStack::PopAndDestroy(); // conditionList
	return res;
	}

// single condition with default time set to Time::MaxTTime()
static TInt CreateScheduleSingle2L(TSchedulerItemRef& aRef, 
									RScheduler& aScheduler,
									const TUid& aConditionUID,
									TUint aConditionUInt)
	{
	aRef.iName = _L("Schedule created using CreateScheduleSingle");

	CSchConditionArray* conditionList 
		= CreateSingleConditionLC(aConditionUID, aConditionUInt);
	TTime ttime(Time::MaxTTime());
	TTsTime time(ttime, ETrue);
	TInt res = aScheduler.CreatePersistentSchedule(aRef, *conditionList, time);
	CleanupStack::PopAndDestroy(); // conditionList
	return res;
	}

// An empty schedule list.
static TInt CreateScheduleEmpty3L(TSchedulerItemRef& aRef, 
									RScheduler& aScheduler,
									const TUid&,
									TUint )
	{
	aRef.iName = _L("Empty Schedule list created");

	CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(3);
	CleanupStack::PushL(conditionList);
	TTime ttime(SchSvrHelpers::UtcTimeBasedOnOffset(0, 0, 0, 0, 0, 1));
	TTsTime time(ttime, ETrue); //1 year in the future
	TInt res = aScheduler.CreatePersistentSchedule(aRef, *conditionList, time);
	CleanupStack::PopAndDestroy(); // conditionList
	
	return res;
	}

// A null schedule.
static TInt CreateScheduleSingleNull4L(TSchedulerItemRef& aRef, 
									RScheduler& aScheduler,
									const TUid&,
									TUint )
	{
	aRef.iName = _L("One schedule in the list with a NULL uid");
	
	CSchConditionArray* conditionList 
		= CreateSingleConditionLC(KNullUid, 0);
	TTime ttime(SchSvrHelpers::UtcTimeBasedOnOffset(0, 0, 0, 0, 0, 1));
	TTsTime time(ttime, ETrue); //1 year in the future
	TInt res = aScheduler.CreatePersistentSchedule(aRef, *conditionList, time);
	CleanupStack::PopAndDestroy(); // conditionList

	return res;
	}

static TInt CreateScheduleMultipleL(TSchedulerItemRef& aRef, RScheduler& aScheduler)
	{
	aRef.iName = _L("Schedule created using CreateScheduleMultiple");
	
	CSchConditionArray* conditionList = CreateMultipleConditionsLC();
	TTime ttime(SchSvrHelpers::UtcTimeBasedOnOffset(0, 0, 0, 0, 0, 1));
	TTsTime time(ttime, ETrue); //1 year in the future
	TInt res = aScheduler.CreatePersistentSchedule(aRef, *conditionList, time);
	CleanupStack::PopAndDestroy(); // conditionList
	return res;
	}

static TInt SchedulePersistentTaskL(const TDesC& aName, 
									TInt& aNewId, 
									TInt aScheduleId, 
									RScheduler& aScheduler)
	{
	TTaskInfo taskInfo;
	taskInfo.iTaskId = aNewId;
	taskInfo.iName = aName;
	taskInfo.iPriority = 2;
	taskInfo.iRepeat = 0;
	HBufC* data = _L("the data").AllocLC();
	TInt res = aScheduler.ScheduleTask(taskInfo, *data, aScheduleId);
	aNewId = taskInfo.iTaskId;

	CleanupStack::PopAndDestroy(); // data
	return res;
	}

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0244
@SYMTestCaseDesc 			Single condition based test - UTC
@SYMTestPriority 			High
@SYMTestActions  			Create a condition based schedule & check schedule auto deletes
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void DoTest1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0244 "));
	// create a simple condition based schedule, and see if it runs.
	TInt res = KErrNone;
	TheTest.Next(_L("single condition based test"));
	__UHEAP_MARK;
	
	//reset the p&s variables before creating the schedule
	ResetVariablesL(0,0,0);
	TSchedulerItemRef ref1;
	TheTest.Printf(_L("Create a schedule\n"));
	res = CreateScheduleSingle1L(ref1, TheScheduler, KUidSystemCategory, KTestKey1);
	TEST2(res, KErrNone);
	
	TInt task1 = 0;
	_LIT(KName1, "toothpaste");
	TheTest.Printf(_L("Schedule some tasks\n"));

	res = SchedulePersistentTaskL(KName1, task1, ref1.iHandle, TheScheduler);
	TEST2(res, KErrNone);
	
	res = TheScheduler.__DbgMarkHeap();
	User::LeaveIfError(res); //#1
	TInt scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 
	
	// Check that schedule is of condition type
	TScheduleType scheduleType;
	res = TheScheduler.GetScheduleTypeL(ref1.iHandle, scheduleType);
	TEST2(res, KErrNone);
	TEST(scheduleType == EConditionSchedule );	

	res = TheScheduler.__DbgMarkEnd(0);
	User::LeaveIfError(res); //#1
	
	TheScheduler.Close();
	SchSvrHelpers::Pause(TheTest);

	// wait for condition to be satisfied
	res = RProperty::Set(KUidSystemCategory, KTestKey1,10);
	User::LeaveIfError(res);
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone); 
	CleanupHelpers::KillProcess(KMinimalTaskHandler);
	
	res = TheScheduler.Connect();
	TEST2(res, KErrNone);
	// Register a client with the server
	TheTest.Next(_L("===== Registering Client ====="));
	res = SchSvrHelpers::RegisterClientL(TheScheduler);
	TEST2(res, KErrNone);

	// can't check scheduler to see if any tasks left because it's been
	// deleted as last task has completed
	
	//Check that persistent schedule has auto-deleted

	TheTest.Printf(_L("DEF46200 - Check schedule has auto deleted\n"));
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 0);
	
	// Attempt to delete auto-deleted schedule, should fail
	 
	TheTest.Printf(_L("DEF46200 - Attempting to delete schedule with id %d\n"), ref1.iHandle);
	res = TheScheduler.DeleteSchedule(ref1.iHandle);
	TEST2(res, KErrNotFound);
	
	// now repeat process with singleschedule2
	ResetVariablesL(0,0,0);

	TheTest.Printf(_L("Create another schedule\n"));
	res = CreateScheduleSingle2L(ref1, TheScheduler, KUidSystemCategory, KTestKey1);
	TEST2(res, KErrNone);	

	TheTest.Printf(_L("Create an empty schedule list\n"));
	res = CreateScheduleEmpty3L(ref1, TheScheduler, KUidSystemCategory, KTestKey1);
	TEST2(res, KErrArgument);	

	TheTest.Printf(_L("Create an empty schedule list\n"));
	res = CreateScheduleSingleNull4L(ref1, TheScheduler, KUidSystemCategory, KTestKey1);
	TEST2(res, KErrArgument);	

	res = SchedulePersistentTaskL(KName1, task1, ref1.iHandle, TheScheduler);
	TEST2(res, KErrNone);
	SchSvrHelpers::Pause(TheTest);

	// we should have one outstanding task (without the check in
	// schtimer.cpp specifically for Time::MaxTTime() the timer would have
	// gone off immediately in the past.)
	TEST(CountTasksL(ref1.iHandle) == 1);

	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 
	// wait for condition to be satisfied
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KTestKey1,10));
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone); 
	CleanupHelpers::KillProcess(KMinimalTaskHandler);
	
	// can't check scheduler to see if any tasks left because it's been
	// deleted as last task has completed

	TheTest.Printf(_L("DEF46200 - Check schedule has auto deleted\n"));
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 0); 
	
	// Attempt to delete auto-deleted schedule, should fail
	 
	TheTest.Printf(_L("DEF46200 - Attempting to delete schedule with id %d\n"), ref1.iHandle);
	res = TheScheduler.DeleteSchedule(ref1.iHandle);
	TEST2(res, KErrNotFound);
		
	SchSvrHelpers::Pause(TheTest);
	__UHEAP_MARKEND;
	}

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0245
@SYMTestCaseDesc 			Multiple condition based test - UTC
@SYMTestPriority 			High
@SYMTestActions  	    	Create a condition based schedule with multiple entries, and see if it runs.
                        	Try auto deleting the last schedule and test for not found error. 
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void DoTest2L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0245 "));
	// create a condition based schedule with multiple entries, and see if it runs.
	TInt res = KErrNone;
	TheTest.Next(_L("multiple condition based test"));
	__UHEAP_MARK;
	
	//reset the p&s variables before creating the schedule
	ResetVariablesL(0,10,20);
	TSchedulerItemRef ref1;
	TheTest.Printf(_L("Create a schedule\n"));
	res = CreateScheduleMultipleL(ref1, TheScheduler);
	TEST2(res, KErrNone);	

	TInt task1 = 0;
	_LIT(KName1, "web subscription");
	TheTest.Printf(_L("Schedule some tasks\n"));
	res = SchedulePersistentTaskL(KName1, task1, ref1.iHandle, TheScheduler);
	TEST2(res, KErrNone);

	TInt scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 1); 

	// we should have one task scheduled to run
	CTaskInfoArray* tasks = new (ELeave) CTaskInfoArray(3);
	CleanupStack::PushL(tasks);
	GetTaskInfoL(*tasks, ref1.iHandle);
	TEST(tasks->Count() == 1);
	tasks->Reset();
	
	// wait for conditions to be satisfied
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KTestKey1,10));//"=="
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KTestKey2,1234));//"!="
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KTestKey3,9));//"<"
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone); 
	CleanupHelpers::KillProcess(KMinimalTaskHandler);

	// Can't check schedule for task info because it's gone

	
	// we should have no schedule, it has auto-deleted
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 0); 
			
	// Reset variables
	TheTest.Printf(_L("Reseting variables"));
	ResetVariablesL(0,10,20);

	TheTest.Printf(_L("DEF46200 - Attempting to delete schedule with id %d\n"), ref1.iHandle);
	res = TheScheduler.DeleteSchedule(ref1.iHandle);
	TEST2(res, KErrNotFound);
	scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 0); 
	
	CleanupStack::PopAndDestroy(tasks);
	
	SchSvrHelpers::Pause(TheTest);
	__UHEAP_MARKEND;
	}
	
/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0246
@SYMTestCaseDesc 			Error checking test - UTC
@SYMTestPriority 			High
@SYMTestActions  			Create a schedule with a P&S variables that doesnt exist & create a schedule with a P&S variable that isnt an integer.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/	
static void DoTest3L()	
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0246 "));
	// create a simple condition based schedule, and see if it runs.
	TInt res = KErrNone;
	TheTest.Next(_L("error checking test"));
	__UHEAP_MARK;
	
	//reset the p&s variables before creating the schedule
	ResetVariablesL(0,0,0);
	TSchedulerItemRef ref1;
	_LIT(KName1, "toothpaste");

	TheTest.Printf(_L("Create a schedule with a P&S variables that doesnt exist\n"));
	{
	const TUid KNonexistentUid = TUid::Uid(0x01234566);
	res = CreateScheduleSingle1L(ref1, TheScheduler, KNonexistentUid, KTestKey1);
	TEST2(res, KErrNone);
	TheTest.Printf(_L("Schedule some tasks - error should be returned\n"));

	TTaskInfo taskInfo;
	taskInfo.iName = KName1;
	taskInfo.iPriority = 2;
	taskInfo.iRepeat = 0;
	HBufC* data = _L("the data").AllocLC();
	res = TheScheduler.ScheduleTask(taskInfo, *data, ref1.iHandle);
	// since we have created the schedule using a UID which doesn't exist
	//we should get an error
	TEST2(res, KErrArgument);
	CleanupStack::PopAndDestroy(); // data
	
	TheTest.Printf(_L("Deleting schedule with id %d\n"), ref1.iHandle);
	res = TheScheduler.DeleteSchedule(ref1.iHandle);
	TEST2(res, KErrNone);
	TInt scheduleCount = CountScheduledItemsL(EAllSchedules, TheScheduler);
	TEST(scheduleCount == 0); 
	}

	TheTest.Printf(_L("Create a schedule\n"));
	res = CreateScheduleSingle1L(ref1, TheScheduler, KUidSystemCategory, KTestKey1);
	TEST2(res, KErrNone);	
	TheTest.Printf(_L("Schedule some tasks\n"));
	{
	TTaskInfo taskInfo;
	taskInfo.iName = KName1;
	taskInfo.iPriority = 2;
	taskInfo.iRepeat = 1;
	HBufC* data = _L("the data").AllocLC();
	User::LeaveIfError(TheScheduler.__DbgMarkHeap());
	res = TheScheduler.ScheduleTask(taskInfo, *data, ref1.iHandle);
	// since we have set repeat to something other than 0, we should get an error
	TEST2(res, KErrArgument);
	User::LeaveIfError(TheScheduler.__DbgMarkEnd(0));
	CleanupStack::PopAndDestroy(); // data
	}
	
	SchSvrHelpers::Pause(TheTest);
	__UHEAP_MARKEND;
	}
	
/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-3245
@SYMTestCaseDesc	    Persistent condition based schedule test
@SYMTestPriority 	    High
@SYMTestActions  	    Create a single persistent condition based schedule and then 
						terminate the task scheduler.
						Set the condition and then launch the task scheduler with the 
						condition satisfied.  Check that the schedule is executed.
@SYMTestExpectedResults Schedule must be executed and test must not panic or fail
@SYMREQ                 REQ0000
*/
static void DoTest4L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-3245 "));
	// create a persistent condition based schedule
	TInt res = KErrNone;
	TheTest.Next(_L("single condition based test"));
	__UHEAP_MARK;
	
	//reset the p&s variables before creating the schedule
	ResetVariablesL(0,0,0);
	TSchedulerItemRef ref1;
	TheTest.Printf(_L("Create a test schedule\n"));
	res = CreateScheduleSingle1L(ref1, TheScheduler, KUidSystemCategory, KTestKey1);
	TEST2(res, KErrNone);
	
	TInt task1 = 0;
	_LIT(KName1, "shutdown task 1");
	TheTest.Printf(_L("Schedule a persistant task\n"));

	res = SchedulePersistentTaskL(KName1, task1, ref1.iHandle, TheScheduler);
	TEST2(res, KErrNone);

	//Fault the server to force it to shut down
 	TheScheduler.__FaultServer();

	//close the Scheduler handle
	TheScheduler.Close();

	SchSvrHelpers::Pause(TheTest);
		
	// set condition
	res = RProperty::Set(KUidSystemCategory, KTestKey1,10);
	TEST2(res, KErrNone);
	
	//Restart the scheduler with the condition for this persistant task
	//satisfied
	res = TheScheduler.Connect();
	TEST2(res, KErrNone);	
	
	//wait for task to be executed
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone); 
	CleanupHelpers::KillProcess(KMinimalTaskHandler);
	
	SchSvrHelpers::Pause(TheTest);

	__UHEAP_MARKEND;
	
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
	
	// Launch helper process to create P&S variables
	CreateTestVariables();
	
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
	TheTest.Next(_L("Start tests"));
 	DoTest1L();
	DoTest2L();
	DoTest3L();
	DoTest4L();

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
	TheTest.Start(_L("TC_TSCH_CONDITION_UTC"));
	TheTest.Title();
	TheCleanup = CTrapCleanup::New();

	//If the previous test fails, SCHSVR.exe may stay in memory.
	TRAPD(error,CleanupHelpers::TestCleanupL());
	TEST2(error, KErrNone);
	TheTest(TheFsSession.Connect() == KErrNone);;
	TRAP(error, RunTestsL());
	TEST2(error, KErrNone);	
	TRAP(error,CleanupHelpers::TestCleanupL());
	TEST2(error, KErrNone);
	delete TheCleanup;
	
	TheFsSession.Close();
	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;

	return KErrNone;
	}
