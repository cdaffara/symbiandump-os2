// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

_LIT(KTestName,	"Task Scheduler Robustness Test");
_LIT(KTaskScheduler, "schexe");

RTest	TheTest(KTestName);

typedef CArrayFixFlat<TTaskInfo>			CTaskInfoArray;
typedef CArrayFixFlat<TSchedulerItemRef>    CSchItemRefArray;
typedef CArrayFixFlat<TTaskSchedulerCondition>	CSchConditionArray;

static RScheduler	TheScheduler;
static CTrapCleanup*	TheCleanup;
static RFs			TheFsSession;

const TInt KTestKey1 = 1;

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
	}	

static void ResetVariablesL(TInt aKey1Val)
	{
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KTestKey1,aKey1Val));		
	}
	
	
// single condition with default time set to 1 year in the future
static TInt CreateScheduleL(TSchedulerItemRef& aRef, 
									RScheduler& aScheduler,
									const TUid& aConditionUID,
									TUint aConditionUInt)
	{
	aRef.iName = _L("Schedule created using CreateScheduleSingle");

	CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(1);
	CleanupStack::PushL(conditionList);
	
	//create a single condition
	TTaskSchedulerCondition condition1;
	condition1.iCategory = aConditionUID;
	condition1.iKey		= aConditionUInt;
	condition1.iState	= 10;
	condition1.iType	= TTaskSchedulerCondition::EEquals;
	
	conditionList->AppendL(condition1);
		
	//create a persistent schedule
	TTime time = SchSvrHelpers::TimeBasedOnOffset(0, 0, 0, 0, 0, 1); //1 year in the future
	TInt res = aScheduler.CreatePersistentSchedule(aRef, *conditionList, time);
	CleanupStack::PopAndDestroy(); // conditionList
	return res;
	}

//Add a single task to a schedule	
static TInt AddTaskToScheduleL(const TDesC& aName, 
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
	
	
static TInt ScheduleTaskL()
	{
	//reset the p&s variables before creating the schedule
	ResetVariablesL(0);
	
	//Create a schedule
	TSchedulerItemRef ref1;
	TheTest.Printf(_L("Create a schedule\n"));
	TInt res = CreateScheduleL(ref1, TheScheduler, KUidSystemCategory, KTestKey1);
	TEST2(res, KErrNone);
	
	//Add task to the schedule
	TInt task1 = 0;
	_LIT(KName1, "Test Task");
	TheTest.Printf(_L("Schedule a task\n"));

	res = AddTaskToScheduleL(KName1, task1, ref1.iHandle, TheScheduler);
	TEST2(res, KErrNone);
	 
	return res;	
	}
	
static void ExecuteTaskL()
	{
	TheTest.Printf(_L("Execute Task\n"));
	//Set property causing schedule to be run
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KTestKey1,10));
	
	//Pause to wait for the task to be executed
	SchSvrHelpers::Pause(TheTest, 2);
	
	}

static TInt ScheduleAndExecuteTaskL()
	{
	TInt res = ScheduleTaskL();

	ExecuteTaskL();
	 
	return res;	
	}

LOCAL_C void AddTaskFunctionL()
	{
	RScheduler	localScheduler;
	// Connect to the server
	TInt res = localScheduler.Connect();
	TEST2(res, KErrNone);
	//Schedule a task and execute it
	//reset the p&s variables before creating the schedule
	ResetVariablesL(0);
		
	//Create a schedule
	TSchedulerItemRef ref1;
	ref1.iName = _L("Schedule created using CreateScheduleSingle");
	CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(1);
	CleanupStack::PushL(conditionList);
		
	//create a single condition
	TTaskSchedulerCondition condition1;
	condition1.iCategory = KUidSystemCategory;
	condition1.iKey		= KTestKey1;
	condition1.iState	= 10;
	condition1.iType	= TTaskSchedulerCondition::EEquals;
			
	conditionList->AppendL(condition1);
				
	//create a persistent schedule
	TTime time = SchSvrHelpers::TimeBasedOnOffset(0, 0, 0, 0, 0, 1); //1 year in the future
	res = localScheduler.CreatePersistentSchedule(ref1, *conditionList, time);
	CleanupStack::PopAndDestroy(); // conditionList
			
	//Add task to the schedule
	TInt task1 = 0;
	_LIT(KName1, "Test Task");
		
	TTaskInfo taskInfo;
	taskInfo.iTaskId = task1;
	taskInfo.iName = KName1;
	taskInfo.iPriority = 2;
	taskInfo.iRepeat = 0;
	HBufC* data = _L("the data").AllocLC();

	TInt ret = localScheduler.ScheduleTask(taskInfo, *data, ref1.iHandle);
	TEST2(ret, 0);//EPanicNotRegistered == 0
		
	task1 = taskInfo.iTaskId;

	CleanupStack::PopAndDestroy(); // data
		
	//Tidying up so next test will be clear.
	SchSvrHelpers::DeleteAllSchedulesL(localScheduler);

	localScheduler.Close();
	}

// Helper function for DEF124488 
LOCAL_C TInt TestPanicThread(TAny*)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;
	

	TRAPD(err,AddTaskFunctionL())
	TEST2(err,KErrNone);
	
	delete cleanup;
	return KErrNone;
	}

/**	
@SYMTestCaseID 	SYSLIB-SCHSVR-CT-3369
@SYMTestCaseDesc	Test deletion of temporary files with non existent client
@SYMTestPriority	High
@SYMTestActions 	Schedule a task with a client that does not exist.  
					Ensure that all temporary files are deleted after 
					schedule excecutes
@SYMTestExpectedResults All temporary files should be deleted by task scheduler
@SYMDEF 		 PDEF101876
*/
static void DoTest1L()
	{	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-3369 Test handling of non existent client "));	
	// Connect to the server
	TheTest.Next(_L("===== Connect to Scheduler ====="));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);
	
	// Register a client with the server - this client does not exist
	TheTest.Next(_L("===== Registering Client ====="));
	res = SchSvrHelpers::RegisterNonExistentClient(TheScheduler);
	TEST2(res, KErrNone);

	//Schedule a task and execute it
	ScheduleAndExecuteTaskL();
	
	// Check for left task files after scheduled tasks completed
	// To access private data cage, uses SchSvrHelplers::CheckTaskFilesL()
	TheTest.Next(_L("Now checking no files left when tasks completed"));
	TInt err = SchSvrHelpers::CheckTaskFilesL();
	
	// If there's any task files left, test fails with error code KErrGeneral
	TEST(err == KErrNone);
	TheTest.Next(_L("All files deleted as expected..."));

	//Tidying up so next test will be clear.
	TheTest.Next(_L("Delete all schedules"));
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);

	TheScheduler.Close();

	}
	
/**
@SYMTestCaseID 	SYSLIB-SCHSVR-CT-3370
@SYMTestCaseDesc	Test deletion of temporary files with faulty client
@SYMTestPriority	High
@SYMTestActions 	Schedule a task with a client that panics and does not 
					release the temporary file handle. 
					Ensure that all temporary files are deleted after schedule excecutes
@SYMTestExpectedResults All temporary files should be deleted by task scheduler
@SYMDEF 		 PDEF101876
*/	
static void DoTest2L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-3370 Test handling of panicing client "));
	
	// Connect to the server
	TheTest.Next(_L("===== Connect to Scheduler ====="));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);
	
	// Register a client with the server - this client panics
	//after calling RFile::AdoptFromClient
	TheTest.Next(_L("===== Registering Client ====="));
	res = SchSvrHelpers::RegisterPanicingClient(TheScheduler);
	TEST2(res, KErrNone);
		
	//Schedule a task and execute it - we expect the client to panic
	ScheduleAndExecuteTaskL();
	
	// Check for left task files after scheduled tasks completed
	// To access private data cage, uses SchSvrHelplers::CheckTaskFilesL()
	TheTest.Next(_L("Now checking no files left when tasks completed"));
	TInt err = SchSvrHelpers::CheckTaskFilesL();

	// If there's any task files left, test fails with error code KErrGeneral
	TEST(err == KErrNone);	
	
	TheTest.Next(_L("All files deleted as expected..."));

	//Tidying up so next test will be clear.
	TheTest.Next(_L("Delete all schedules"));
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);

	TheScheduler.Close();
	}

/**
@SYMTestCaseID 	SYSLIB-SCHSVR-CT-3371
@SYMTestCaseDesc	Test deletion of temporary files on task scheduler startup
@SYMTestPriority	High
@SYMTestActions 	Create temporary files in the task schedulers private data cage.
					Start the task scheduler and verify that these files are deleted.
@SYMTestExpectedResults All temporary files should be deleted by task scheduler on startup
@SYMDEF 		 PDEF101876
*/	
static void DoTest3L()
	{	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-3371 Test deletion of temporary files on startup "));
	
	//Connect to the scheduler
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);

 	// Kill the server to ensure we restart it when we connect
	res = CleanupHelpers::KillProcess(KTaskScheduler);
	TEST2(res, KErrNone);
 	TheScheduler.Close();
 	
	// Create task files to test cleanup
	// To access private data cage, uses SchSvrHelplers::CreateTaskFilesL()
	TheTest.Next(_L("Creating dummy task files"));
	res = SchSvrHelpers::CreateTaskFilesL();

	//Restart the scheduler which should clean up temp files on startup
	TheTest.Next(_L("===== Connect to Scheduler ====="));
	res = TheScheduler.Connect();
	TEST2(res, KErrNone);
	
	//wait for the server to start up
	SchSvrHelpers::Pause(TheTest, 2);
		
	TheScheduler.Close();
	
	// Check for left task files after scheduled tasks completed
	// To access private data cage, uses SchSvrHelplers::CheckTaskFilesL()
	TheTest.Next(_L("Now checking no files left after task scheduler starts"));
	res = SchSvrHelpers::CheckTaskFilesL();
	
	TEST2(res, KErrNone);
	
	TheTest.Next(_L("All files deleted as expected..."));
	}

/**
@SYMTestCaseID 		SYSLIB-SCHSVR-CT-3402
@SYMTestCaseDesc	Test memory cleanup on Task Scheduler exit
@SYMTestPriority	High
@SYMTestActions 	Start the scheduler and register a client.
					Execute a schedule and then terminate the scheduler.
					When the scheduler is restarted it should exit as there are no
					pending schedules. On exit all allocated memory should be freed	
@SYMTestExpectedResults All allocated memory should be freed when the scheduler exits
@SYMDEF 		 DEF102414
*/	
static void DoTest4L()
	{	
	__UHEAP_MARK;
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-3402 Test memory cleanup on Task Scheduler exit "));
	
	// Connect to the server
	TheTest.Next(_L("===== Connect to Scheduler ====="));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);
	
	// Register a client with the server
	TheTest.Next(_L("===== Registering Client ====="));
	res = SchSvrHelpers::RegisterClientL(TheScheduler);
	TEST2(res, KErrNone);
	
	//Schedule a task and execute it
	ScheduleAndExecuteTaskL();
	
 	// Kill the server
	res = CleanupHelpers::KillProcess(KTaskScheduler);
	TEST2(res, KErrNone);
	
 	TheScheduler.Close();
	
	//Restarting with a registered client and no schedule should
	//cause the server to exit.  Ther server should free all allocated
	//memory.  If all memory is not freed, heap check macros within
	// the task scheduler code will cause a panic 
	SchSvrHelpers::LaunchTaskSchedulerL();
	
	//wait for the server to exit
	SchSvrHelpers::Pause(TheTest, 2);
	
	//Verify that the server has already exited - there are two valid 
	//error codes depending on how quickly the process is cleaned up
	//KErrDied - Process is dead but hasn't been cleaned up yet by the kernel
	//KErrNotFound - Process has been cleaned up
	res = CleanupHelpers::KillProcess(KTaskScheduler);
	
	TEST((res == KErrDied)||(res == KErrNotFound));
	
	__UHEAP_MARKEND;

	TheTest.Next(_L("All memory freed..."));
	}
	
	
/**
@SYMTestCaseID 		SYSLIB-SCHSVR-CT-3412
@SYMTestCaseDesc	Test Task Scheduler startup with pending schedule
@SYMTestPriority	High
@SYMTestActions 	Start the scheduler and register a client.
					Create a scheduled task and then terminate the scheduler without executing the task.
					When the scheduler is restarted it should not exit as there is a 
					pending schedule. Verify that the scheduler is still active by executing
					the schedule
@SYMTestExpectedResults	The task scheduler should not exit and the schedule should execute
@SYMDEF 		 DEF102414
*/	
static void DoTest5L()
	{
	__UHEAP_MARK;
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-3412 Test Task Scheduler startup with pending schedule "));
	
	// Connect to the server
	TheTest.Next(_L("===== Connect to Scheduler ====="));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);
	
	// Register a client with the server
	TheTest.Next(_L("===== Registering Client ====="));
	res = SchSvrHelpers::RegisterClientL(TheScheduler);
	TEST2(res, KErrNone);
	
	//Schedule a task
	ScheduleTaskL();
	
 	// Kill the server
	res = CleanupHelpers::KillProcess(KTaskScheduler);
	TEST2(res, KErrNone);
 	TheScheduler.Close();
 	
 	TheTest.Next(_L("Create Task notification semaphore"));
	//initialise task notification semaphore
	STaskSemaphore sem;
	sem.CreateL();
	
	//Restart the scheduler - task scheduler should not exit as there is a 
	//pending schedule
	res = SchSvrHelpers::LaunchTaskSchedulerL();
	TEST2(res, KErrNone);
	
	//Execute task and wait for it to run - this would not succeed
	//if task scheduler had exited above
	ExecuteTaskL();
	TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone); 
	
	//Kill the process and verify that the scheduler was active
	//If the task scheduler isnt active when we try to kill it
	//KillProcess would return KErrDied
	res = CleanupHelpers::KillProcess(KTaskScheduler);
	TEST2(res, KErrNone);
	
	//close handle to semaphore
	sem.Close();

	__UHEAP_MARKEND;	
	
	}

/**	
@SYMTestCaseID 	SYSLIB-SCHSVR-CT-4010
@SYMTestCaseDesc	Test that adding a task using an unregistered client panics the client and does not crash the server.
@SYMTestPriority	High
@SYMTestActions 	Schedule a task with a client that has not been registered.  
@SYMTestExpectedResults Client should be panicked.
@SYMDEF 		DEF124488 
*/
static void DoTest6L()
	{	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-4010 Test handling of unregistered client Should Panic Client thread "));	
	
	RThread testThread;
	_LIT(KThreadName, "PanicClientThread");

	testThread.Create(KThreadName, TestPanicThread, KDefaultStackSize, 0x1000, 0x100000, NULL);

	TRequestStatus requestStatus;
	// Request notification when the thread terminates
	testThread.Logon(requestStatus);
	
	TBool justInTime=User::JustInTime(); 
	User::SetJustInTime(EFalse); 
	// Let the thread execute
	testThread.Resume();

	// Wait for termination
	User::WaitForRequest(requestStatus);
	User::SetJustInTime(justInTime); 

	TEST2(testThread.ExitReason(), 0);
	testThread.Close();

	}

static TInt RunTestsL()
	{
	TheTest.Next(_L("Delete old files"));
	SchSvrHelpers::DeleteScheduleFilesL();
	
	//create P&S variables for the test
	CreateTestVariables();
	
	TheTest.Next(_L("Start tests"));

	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
	DoTest1L();
	DoTest2L();
	DoTest3L();
	DoTest4L();
	DoTest5L();	
	DoTest6L();
	
	TheTest.Next(_L("Tidying up"));
	CleanupStack::PopAndDestroy(scheduler);

	return KErrNone;
	}

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	TheTest.Start(_L("TC_TSCH_ROBUSTNESS"));
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
