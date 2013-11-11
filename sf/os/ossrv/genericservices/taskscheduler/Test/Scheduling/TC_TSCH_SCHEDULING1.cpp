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

#include <e32base.h>
#include <e32test.h>
#include <csch_cli.h>
#include <f32file.h>
#include "Thelpers.h"

#include "TestUtils.h"

// Globals
RTest					TheTest(_L("TC_TSCH_SCHEDULING1"));
static RScheduler		TheScheduler;
static RFs				TheFsSession;

typedef CArrayFixFlat<TScheduleEntryInfo>	CScheduleEntryInfoArray;
typedef CArrayFixFlat<TTaskInfo>			CTaskInfoArray;
typedef CArrayFixFlat<TSchedulerItemRef>    CSchItemRefArray;

_LIT(KMinimalTaskHandler, "MinimalTaskHandler");

_LIT(KTimeFormatString,						"%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");
_LIT(KCurrentDateTimeChanged,				"Date & Time changed to: [%S]\n");

const TInt KTimeToWait = 4*1000*1000;


//***********************************************************************************

// Sets time to before specified time by aTimeBeforeInSeconds
static void SetTimeBeforeL(RTest& aTest, TTime& aTime, TInt aTimeBeforeInSeconds)
	{
	TTimeIntervalSeconds secs(aTimeBeforeInSeconds);
	TTime time = aTime-secs;
	SchSvrHelpers::SetHomeTimeL(time);
	TBuf<30> dateString;
	time.FormatL(dateString, KTimeFormatString);
	aTest.Printf(KCurrentDateTimeChanged, &dateString);
	}

// gets the due time for this schedule
static TTime GetDueTimeL(TInt aScheduleId)
	{
	TTime nextTimeScheduleIsDue;
	TScheduleState state;
	CScheduleEntryInfoArray* entries 
		= new (ELeave) CScheduleEntryInfoArray(3);
	CleanupStack::PushL(entries);
	CTaskInfoArray* tasks = new (ELeave) CTaskInfoArray(3);
	CleanupStack::PushL(tasks);

	TInt res = TheScheduler.GetScheduleL(aScheduleId, state, *entries, *tasks, nextTimeScheduleIsDue);
	TEST2(res, KErrNone);

	CleanupStack::PopAndDestroy(2); // entries, tasks
	return state.iDueTime;
	}
	
// Forces the task to be exectued aCount times.
static void ForceTaskExecutionForSpecifiedIdL(TInt aId, TInt aCount)
	{
	TheTest.Printf(_L("Executing %d times\n"), aCount);
	TTime time;
	for (TInt i=0; i<aCount; ++i)
		{
		TheTest.Printf(_L("Execution %d\n"), i+1);
		time = GetDueTimeL(aId);	
		
		SetTimeBeforeL(TheTest, time, 5 /*seconds*/);		
		
		// Wait for notification that schedule has executed.
		TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone);
		CleanupHelpers::KillProcess(KMinimalTaskHandler);
		}
	}

//creates a daily schedule with StartTime of aStartTime
static TInt CreateScheduleL(TSchedulerItemRef& aRef, 
							RScheduler& aScheduler, 
							const TTime& aStartTime)
	{
	CScheduleEntryInfoArray* entryList 
		= new (ELeave) CScheduleEntryInfoArray(1);
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

// counts the number of scheduled items based on the supplied filter.
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

// Extract task references from the schedule server based on a ID
static void GetTaskInfoL(CTaskInfoArray& aTaskInfoArray,
						TInt aScheduleId)
	{
	aTaskInfoArray.Reset();
	TTime nextTimeScheduleIsDue;
	TScheduleState state;
	CScheduleEntryInfoArray* entries 
		= new (ELeave) CScheduleEntryInfoArray(3);
	CleanupStack::PushL(entries);
	TInt res = TheScheduler.GetScheduleL(aScheduleId, 
										state, 
										*entries, 
										aTaskInfoArray, 
										nextTimeScheduleIsDue);
	TEST2(res, KErrNone);
	CleanupStack::PopAndDestroy(entries);
	}




// schedules a transient task	
static TInt ScheduleTransientTaskL(TInt& aTaskId, 
								TSchedulerItemRef& aRef, 
								TInt aRepeat, 
								RScheduler& aScheduler)
	{
	CScheduleEntryInfoArray* entryList = new(ELeave) CScheduleEntryInfoArray(3);
	CleanupStack::PushL(entryList);

	aRef.iName = _L("transient one");

	// SCEHDULES
	TScheduleEntryInfo entry1;
	entry1.iStartTime = SchSvrHelpers::TimeBasedOnOffset(0, 1); // 1 min in the future
	entry1.iInterval = 1;
	entry1.iIntervalType = EDaily;
	entry1.iValidityPeriod = 20;
	entryList->AppendL(entry1);
	
	TScheduleEntryInfo entry2;
	entry2.iStartTime = SchSvrHelpers::TimeBasedOnOffset(0, 2); // 2 mins in the future
	entry2.iInterval = 1;
	entry2.iIntervalType = EDaily;
	entry2.iValidityPeriod = 500;
	entryList->AppendL(entry2);

	TScheduleEntryInfo entry3;
	entry3.iStartTime = SchSvrHelpers::TimeBasedOnOffset(0, 3); // 3 mins in the future
	entry3.iInterval = 1;
	entry3.iIntervalType = EDaily;
	entry3.iValidityPeriod = 5;
	entryList->AppendL(entry3);

	// TASK
	TTaskInfo taskInfo;
	taskInfo.iName = _L("mail");
	taskInfo.iTaskId = aTaskId;
	taskInfo.iRepeat = aRepeat;
	taskInfo.iPriority = 2;
	HBufC* data = _L("the data, some strange new name ").AllocLC();

	// Schedule the item
	TInt res = aScheduler.ScheduleTask(taskInfo, *data, aRef, *entryList);
	CleanupStack::PopAndDestroy(2); // data, entryList

	aTaskId = taskInfo.iTaskId;
	return res;
	}

/*
 * Add Persistent and long term schedules to SchedulessBackup.dat files. this function 
 * is part of the DEF108026 defect testing called by DEF108026() 		
 * It is part of the manual test SYSLIB-SCHSVR-CT-4003
 * should be commented and will run as manual test only.
 * 
 * 
  
 static void DEF108026_AddPersistentSchedulesL( TBool aImmediate )
	{	
	
	_LIT(KTaskData1, "This is some really exciting task data (number 1)");
	_LIT(KTaskData2, "This is some really exciting task data (number 2)");	
	_LIT(KTestName,	"Def108026_test");

	TheTest.Next(_L("Def108026_test: Adding schedules"));
	
	RDebug::Print(_L("DEF108026_AddPersistentSchedulesL"));
	
	TheTest.Next(_L("Connect to Scheduler"));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	

	TTime startTimeForSchedule;
	if ( aImmediate )
		{
		// This is the time when we want the schedule to fire
		startTimeForSchedule = TTime( (TDateTime(2007, EJune, 20, 10, 00, 0, 0)) ); 
		RDebug::Print( _L("Added immediate schedule"));
		}
	else
		{
		startTimeForSchedule = TTime( (TDateTime(2037, EJune, 20, 10, 00, 0, 0)) ); // These schedules are due in the future.
		RDebug::Print( _L("Added long-term schedule"));	
		}


	// Prepare a schedule describing when we want the tasks to run (10:00 am)
	TSchedulerItemRef ref;
	User::LeaveIfError(CreateScheduleL(ref, TheScheduler, startTimeForSchedule));

	// Disable the schedule whilst we set it up
	User::LeaveIfError(TheScheduler.DisableSchedule(ref.iHandle));

	// Associate a task with the schedule
	TTaskInfo taskInfo1;
	taskInfo1.iRepeat = 0;
	taskInfo1.iName = KTestName;
	taskInfo1.iPriority = 2;

	// Create some data associated with this task
	HBufC* taskData1 = KTaskData1().AllocLC();
	User::LeaveIfError(TheScheduler.ScheduleTask(taskInfo1, *taskData1, ref.iHandle));
	CleanupStack::PopAndDestroy(); // taskData1

	// Associate a task (2) with the schedule
	TTaskInfo taskInfo2;
	taskInfo2.iRepeat = 0;
	taskInfo2.iName = KTestName;
	taskInfo2.iPriority = 2;

	// Create some data associated with this task
	HBufC* taskData2 = KTaskData2().AllocLC();
	User::LeaveIfError(TheScheduler.ScheduleTask(taskInfo2, *taskData2, ref.iHandle));
	CleanupStack::PopAndDestroy(); // taskData2
	
	// We should now have two tasks scheduled at exactly the same time...
	User::LeaveIfError(TheScheduler.EnableSchedule(ref.iHandle));
	
	CleanupHelpers::KillProcess(KMinimalTaskHandler);
	}
	
*
** end of DEF108026_AddPersistentSchedulesL() **/
	

/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-1031
@SYMTestCaseDesc	    Test code for defect "Sending SMS to multiple recipients
						sends message to only one recipient" (EDNEMHE-4Q69BG)
@SYMTestPriority 	    High
@SYMTestActions  	    Schedule two tasks to run at same time
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
static void Test1L()
	{	
	_LIT(KTaskData1, "This is some really exciting task data (number 1)");
	_LIT(KTaskData2, "This is some really exciting task data (number 2)");
	_LIT(KTestName,	"SmsTest");

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-1031 TheTest3: SMS:Sending to multiple recipients"));
	
	TheTest.Next(_L("Connect to Scheduler"));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	// Set the time to a known value, since this makes testing much easier (and more
	// repeatable).	
	SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 55, 0, 0))); // 9:55 am

	// This is the time when we want the schedule to fire
	TTime startTimeForSchedule(TDateTime(2000, EJanuary, 1, 10, 0, 0, 0)); // 10:00 am

	// Prepare a schedule describing when we want the tasks to run (10:00 am)
	TSchedulerItemRef ref;
	User::LeaveIfError(CreateScheduleL(ref, TheScheduler, startTimeForSchedule));

	// Disable the schedule whilst we set it up
	User::LeaveIfError(TheScheduler.DisableSchedule(ref.iHandle));

	// Associate a task with the schedule
	TTaskInfo taskInfo1;
	taskInfo1.iRepeat = 0;
	taskInfo1.iName = KTestName;
	taskInfo1.iPriority = 2;

	// Create some data associated with this task
	HBufC* taskData1 = KTaskData1().AllocLC();
	User::LeaveIfError(TheScheduler.ScheduleTask(taskInfo1, *taskData1, ref.iHandle));
	CleanupStack::PopAndDestroy(); // taskData1

	// Associate a task (2) with the schedule
	TTaskInfo taskInfo2;
	taskInfo2.iRepeat = 0;
	taskInfo2.iName = KTestName;
	taskInfo2.iPriority = 2;

	// Create some data associated with this task
	HBufC* taskData2 = KTaskData2().AllocLC();
	User::LeaveIfError(TheScheduler.ScheduleTask(taskInfo2, *taskData2, ref.iHandle));
	CleanupStack::PopAndDestroy(); // taskData2

	// We should now have two tasks scheduled at exactly the same time...
	User::LeaveIfError(TheScheduler.EnableSchedule(ref.iHandle));

	// Set the time to 5 minutes *AFTER* the schedule was due to run (10:05am). In this instance,
	// based on the new fixed Schedule Server, the schedule should still execute since 
	// it falls within the validity period (30 mins), however, in the old scheme of things,
	// the schedule would not be valid again until tomorrow (2/1/2000) at 10:00am and hence
	// would not execute until then.	
	SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 10, 5, 0, 0))); // 10:05 am

	// Now wait for something to happen...
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone);
	CleanupHelpers::KillProcess(KMinimalTaskHandler);
	}

/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-1032
@SYMTestCaseDesc	    Tests for creation of transient schedule with task repeating 5 times
@SYMTestPriority 	    High
@SYMTestActions  	    There should be no schedules as its a transient one and last schedule
						should have deleted by itself.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
static void Test2L()
	{	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-1032 Testing creation of transient schedule with task repeating 5 times "));

	TInt tTask = 0;
	TSchedulerItemRef ref;
	// schedule has 3 entries - +1min, +2min and +3min
	TInt res = ScheduleTransientTaskL(tTask, ref, 5, TheScheduler);//5 repeats
	TEST2(res, KErrNone);
	
	TheTest.Printf(_L("Get Task count and repeat count\n"));

	CTaskInfoArray* tasks = new (ELeave) CTaskInfoArray(3);
	CleanupStack::PushL(tasks);
	GetTaskInfoL(*tasks, ref.iHandle);
	TEST(tasks->Count() == 1);
	TTaskInfo info = tasks->At(0);
	TEST(info.iRepeat == 5);
	ForceTaskExecutionForSpecifiedIdL(ref.iHandle, 3);
	GetTaskInfoL(*tasks, ref.iHandle);
	TEST(tasks->Count() == 1);
	info = tasks->At(0);
	TEST(info.iRepeat == 2); // still 2 repeats to go.
	ForceTaskExecutionForSpecifiedIdL(ref.iHandle, 2);

	CleanupStack::PopAndDestroy(tasks);
	
	TInt scheduleCount = CountScheduledItemsL(EPendingSchedules, TheScheduler);
	// There should be no schedules as its a transient one and last schedule
	// should have deleted itself.
	TEST(scheduleCount == 0); 
	SchSvrHelpers::Pause(TheTest);
	}

/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-1341
@SYMTestCaseDesc	    Tests for defect number DEF055586L
@SYMTestPriority 	    High
@SYMTestActions  	    Check to ensure that last element in schedule entries
                        array not skipped by schedule checking functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
static void DEF055586L()
	{	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-1041 DEF055586 - Last element in array missed by loop"));
	
	TheTest.Next(_L("Connect to Scheduler"));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	// Set the time to a known value, since this makes testing much easier (and more
	// repeatable).
	SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 55, 0, 0))); // 9:55 am

	// This is the time when we want the schedule to fire
	TTime startTimeForSchedule(TDateTime(2000, EJanuary, 1, 10, 0, 0, 0)); // 10:00 am

	// Prepare a schedule describing when we want the tasks to run (10:00 am)
	TSchedulerItemRef ref;

	CScheduleEntryInfoArray* entryList = new (ELeave) CScheduleEntryInfoArray(1);
	CleanupStack::PushL(entryList);

	TScheduleEntryInfo entry1;
	entry1.iStartTime		= startTimeForSchedule;
	entry1.iInterval		= 0; // TTimeIntervalDays: set to 0 to induce an error
	entry1.iIntervalType	= EDaily;
	entry1.iValidityPeriod	= 30; // minutes
	entryList->AppendL(entry1);
	TInt err = TheScheduler.CreatePersistentSchedule(ref, *entryList);

	TEST2(err, KErrArgument);

	TheTest.Next(_L("DEF055586 - Now checking 0 entries in schedule"));
	entryList->Reset();
	
	err = TheScheduler.CreatePersistentSchedule(ref, *entryList);
	
	TEST2(err, KErrArgument);
	
	CleanupStack::PopAndDestroy(); // entryList
	}
	
/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-3159
@SYMTestCaseDesc	    Tests for defect number DEF094149
@SYMTestPriority 	    Normal
@SYMTestActions  	    Create one Schedule and two task in it, then delete one task
						check whether Task Scheduler execute disabled Schedule 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void DEF094149L()
	{	
	// Create  scheduling entries
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-3159 DEF094149: RF Task Scheduler execute deleted tasks (in stress conditions) "));
	TTime time;
	time.HomeTime();
	time += TTimeIntervalSeconds(2); // Scheduler will run task in 2 seconds

	TScheduleEntryInfo entry;
	entry.iIntervalType	= EHourly;
	entry.iStartTime		= time;
	entry.iInterval		= 1;
	entry.iValidityPeriod	= 20;
	
	CArrayFixFlat<TScheduleEntryInfo>* entries = new(ELeave) CArrayFixFlat<TScheduleEntryInfo>(10);
	CleanupStack::PushL(entries);

	entries->AppendL(entry);
	TSchedulerItemRef ref;
	// Create the schedule for the task... and disable it
	TheScheduler.Connect();
	TheScheduler.CreatePersistentSchedule(ref, *entries);
	TheScheduler.DisableSchedule(ref.iHandle);
	
// CreateNewTask
	TTaskInfo task;
	task.iRepeat	= 1; // repeat once
	task.iName		= _L("Test Task For Defect Verification");
	task.iPriority	= 100;
	
	
	TBuf<255>fileName = _L("Some task");
	TheScheduler.Register( fileName, 1 );
	HBufC* taskData = HBufC::NewLC(1);

	//first task
	TheScheduler.ScheduleTask(task, *taskData,ref.iHandle);
	TInt taskid1;
	taskid1 = task.iTaskId;
	
	//Second task
	TheScheduler.ScheduleTask(task, *taskData,ref.iHandle);
	TInt taskid2;
	taskid2 = task.iTaskId;

	TInt err = TheScheduler.DeleteTask(taskid1);
	
	User::After(KTimeToWait);
	err = TheScheduler.DeleteTask(taskid2);
	
	//check whether Task Scheduler execute the second task after deleting first tasks 
	//If the Schedule executed then err == KErrNotFound
	TEST2(err, KErrNone);
	CleanupStack::PopAndDestroy(taskData);	
	CleanupStack::PopAndDestroy(entries);	

	}

/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-3358
@SYMTestCaseDesc	    Tests for defect number  INC098909 
@SYMTestPriority 	    High
@SYMTestActions  	    Mark heap of Scheduler then create a schedule & task wait for its 
						execution then check heap again for memory leaks
@SYMTestExpectedResults Test must not fail
@SYMDEF                 INC098909: Process !TaskScheluder leaks memory in mail polls.
*/
static void INC098909()
	{
	const TInt KTimeToWait = 5*1000*1000; 
	//SchSvrHelpers::DeleteScheduleFilesL();
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-3358 INC098909: Process !TaskScheluder leaks memory in mail polls "));
	TheTest.Next(_L("Connect to Scheduler"));
	TEST2(TheScheduler.Connect(),KErrNone);

	TheTest.Next(_L("Registering Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);
	
	User::LeaveIfError(TheScheduler.__DbgMarkHeap());

	//Create Schedule
	CArrayFixFlat<TScheduleEntryInfo>* entryList = new(ELeave) CArrayFixFlat<TScheduleEntryInfo>(1);
	CleanupStack::PushL(entryList);
	TSchedulerItemRef ref;
	
	TTime time;
	time.HomeTime();
	time += TTimeIntervalSeconds(0); // Scheduler will run task in  5 sec

	TScheduleEntryInfo entry1;
	entry1.iIntervalType	= EHourly;
	entry1.iStartTime		= time;
	entry1.iInterval		= 1;
	entry1.iValidityPeriod	= 20;
	entryList->AppendL(entry1);
	
	// Create the schedule for the task...
	TEST2(TheScheduler.CreatePersistentSchedule(ref, *entryList),KErrNone);
	
	//Create Task
	TTaskInfo task;
	task.iRepeat	= 1; // repeat once
	task.iName		= _L("Test ");
	task.iPriority	= 100;
	
	HBufC* taskData = HBufC::NewLC(1);
	TEST2(TheScheduler.ScheduleTask(task, *taskData,ref.iHandle), KErrNone);
	
	CleanupStack::PopAndDestroy(taskData);
	CleanupStack::PopAndDestroy(entryList);	
	
	//Wait schedule to complete
	User::After(KTimeToWait);

	User::LeaveIfError(TheScheduler.__DbgMarkEnd(0));

	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);
	
	// really clean out the scheduler (get rid of all the files and process)
	TheScheduler.Close();
	SchSvrHelpers::DeleteScheduleFilesL();
	CleanupHelpers::KillProcess(KMinimalTaskHandler);
	// Now wait for something to happen...
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone);
	}


	
/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-4003
@SYMTestCaseDesc	    Test for DEF108026:
						The aim of this test is to create enough persistent schedules. This reproduce the defect, when 
						SysStart startup. task scheduler will see that these schedules are due. It will then add them 
						immediately to the timer during SSA phase. As this operation consumes a long time and CSchStartupStateMgr::RunL() 
						is blocking, the domain manager fails to be ackowleged about the state transition, therefore certain processes like 
						Esock workers does not complete.  These schedules are due immediately (at the time we run this test), we need then to 
						set the time of the device at the time where the schedules are due. We set the home time of the device in CSchServer::ConstructL(). 
						I add in comment the modified CSchServer::ConstructL().
						ssch_svr.dll must be rebuilt.
						I could reproduces this defect only on Uiq. I could not reproduce it on techview!
@SYMTestPriority 	    Normal
@SYMTestActions  	    Create persistent and long-term schedules.
@SYMTestExpectedResults Test must create a 
@SYMDEF                 DEF108026:SCHSVR performs SSA-driven actions before acknowledging transition.
*/
/*********************** constructL() modified.Needed to explain the manual test DEF108026() ********************************
**
**
void CSchServer::ConstructL()
	{
#ifdef __SCHLOGGING__
	iTheLog = CSheduleServerLog::NewL(_L("SchSvr"));
	Dll::SetTls(iTheLog);
#endif

 	// Create server storage path
 	RFs fs;
 	User::LeaveIfError(fs.Connect());

#ifdef __SECURE_DATA__
	TInt err = fs.CreatePrivatePath(EDriveC);
#else
	_LIT(KDirPath, "C:\\System\\Schedules\\");
	TInt err = fs.MkDirAll(KDirPath);
#endif

	if(err != KErrNone && err != KErrAlreadyExists)
		User::Leave(err);
	
	fs.Close();
	
	
	
	
	// The process that we need to add to set the time of the device.
	// We need to set it to the same time that the schedules added are due.
TTime newTime ( (TDateTime(2007, EJune, 20, 10, 00, 00, 00)) ); // 10:00 am
	{

	_LIT(KSetHomeTime, "TSetHomeTime");
	_LIT(KTimeFormat, "%F%Y%M%D:%H%T%S.%*C6");
	
	RProcess p;	
	TRequestStatus stat;
	TBuf<128> bufLocalTime;	
	
	newTime.FormatL(bufLocalTime, KTimeFormat);
	
	User::LeaveIfError(p.Create(KSetHomeTime, bufLocalTime));
	
	// Asynchronous logon: completes when process terminates with process exit code
	p.Logon(stat);
	p.Resume();

	User::WaitForRequest(stat);
	TInt exitReason = p.ExitReason();
	p.Close();
   //return (exitReason);
	}	
	
	
	
	
	iTaskScheduler = CTaskScheduler::NewL();

	iSSAMgr = new(ELeave) CSchStartupStateMgr(KDmHierarchyIdStartup, KBaseServicesDomain3);
	iSSAMgr->RegisterObserverL(iTaskScheduler);
	iSSAMgr->InitialiseL();

	StartL(KSchSvrName);
	}
**
** end of the changes we need to do in ssch_svr.cpp **/

/*
 * This should be called by DoTheTestsL () . It is part of the manual test SYSLIB-SCHSVR-CT-4003
 * should be commented and will run as manual test only.
 *
 * 
static void DEF108026()
	{
	TheTest.Next(_L("DEF108026: Creating enough persistent and long-term schedules to make task scheduler slower while adding them to the timer"));
	
	for ( TInt i = 0; i < 500; i++ )
		{
		DEF108026_AddPersistentSchedulesL( ETrue );
		HBufC* debugTxt = HBufC::NewLC( 256 );
		debugTxt->Des().Append( _L( "Added Test... " ) );
		debugTxt->Des().AppendNum( i++ );				
		RDebug::Print( *debugTxt );	
		CleanupStack::PopAndDestroy( debugTxt );		
		}
	
	
	for ( TInt i = 0; i < 1000; i++ )
		{
		DEF108026_AddPersistentSchedulesL( EFalse );
		HBufC* debugTxt = HBufC::NewLC( 256 );
		debugTxt->Des().Append( _L( "Added Test... " ) );
		debugTxt->Des().AppendNum( i++ );				
		RDebug::Print( *debugTxt );	
		CleanupStack::PopAndDestroy( debugTxt );		
		}			
		
	}
** end of DEF108026 () **/

GLDEF_C TInt DoTheTestsL()
	{
	
	//Delete old files.
	SchSvrHelpers::DeleteScheduleFilesL();
	
	STaskSemaphore sem;
	sem.CreateL();
	
	TheTest.Next(_L("Start tests"));
	// Add tests here:-
	
	//DEF108026(); //This is a manual test. This test runs only under certain condition,
				 //Read the condition required to run this test in the implementation of the DEF108026() comments.
	
	INC098909();	// Should be commented while running def108026
	DEF094149L();   // Should be commented while running def108026
	Test1L();		//Should be commented while running def108026
	Test2L();		//Should be commented while running def108026
	DEF055586L();	//Should be commented while running def108026
	
	sem.Close();
	
	//Tidying up so next test will be clear.
	TheTest.Next(_L("Delete all schedules"));
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler); // While running def108026() manual test, this should be commented. We
														// want keep the persistent schedules.
	SchSvrHelpers::Pause(TheTest, 2);
	TheTest.Next(_L("Delete old files\n"));
	SchSvrHelpers::DeleteScheduleFilesL(); // While running def108026() manual test, this should be commented. We
											// want keep the persistent schedules.
	TheScheduler.Close();

	return KErrNone;
	}


//***********************************************************************************
GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	TheTest.Title();
	TheTest.Start(_L("TC_TSCH_SCHEDULING1"));

	TInt error = KErrNone;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		return KErrNoMemory;
	//If the previous test fails, SCHSVR.exe may stay in memory.
	TRAP(error,CleanupHelpers::TestCleanupL());
	TEST2(error, KErrNone);

	TEST2(TheFsSession.Connect(), KErrNone);
	TheTest.Next(_L("Do the tests"));
	TRAP(error, DoTheTestsL());
	TEST2(error,KErrNone); // While running DEF108026 this line should be commented. If not, error = -4 (no memory) and this leaves. 
						   //The reason is that our generated schedules take a lot of space. But this is the aim of this manual test:
						   // The SchedulesBackup.dat must be bigger as possible to reproduce the defect
	TheFsSession.Close();
	TRAP(error,CleanupHelpers::TestCleanupL());
	TEST2(error, KErrNone);
	delete cleanup;	

	TheTest.End();
	TheTest.Close();
	__UHEAP_MARKEND;
	return KErrNone;
	}
