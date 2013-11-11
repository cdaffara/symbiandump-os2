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
// Example CTestStep derived implementation
// 
//

#include <schinfo.h>
#include <schinfointernal.h>

#include "persist_hometimeStep.h"
#include "Te_floating_scheduleSuiteDefs.h"

STaskSemaphore sem1;

Cpersist_hometimeStep::~Cpersist_hometimeStep()
/**
 * Destructor
 */
	{
	}

Cpersist_hometimeStep::Cpersist_hometimeStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kpersist_hometimeStep);
	}

TVerdict Cpersist_hometimeStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EFail);
	CTe_floating_scheduleSuiteStepBase::doTestStepPreambleL();
		
	// Delete old files.
	SchSvrHelpers::DeleteScheduleFilesL();

	sem1.CreateL();

	TInt i = TheScheduler.Connect();
	TESTL (i==KErrNone);
		
	// Registering Client
	i = SchSvrHelpers::RegisterClientL(TheScheduler);
	TESTL (i==KErrNone);
	
	// create P&S variables for condition based tests
	User::LeaveIfError(RProperty::Define(KTestUid, KTestKey1, 0));
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CIT-0277
@SYMTestCaseDesc 			Persistant schedule - Hometime
@SYMTestPriority 			High
@SYMTestActions  			For time and condition based test schedule task and check it fires
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
TVerdict Cpersist_hometimeStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	SetTestStepResult(EFail);
	
	_LIT(KTestName1, "Persistant Hometime - Time-Based");
	_LIT(KTaskData1, "This is some really exciting task data (number 1)");
	_LIT(KTestName2, "Persistant Hometime - Condition-Based");
	_LIT(KTaskData2, "This is some really exciting task data (number 2)");
	
	// Tests with timezone set to Europe, London
	RTz tz;
	tz.Connect();
	CTzId* tzId = CTzId::NewL(2592); //set the timezone to Europe/London
	CleanupStack::PushL(tzId);
	tz.SetTimeZoneL(*tzId);
	
	// Set the time to a known value, since this makes testing much easier (and more
	// repeatable).	
	SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 55, 0, 0))); // 9:55 am

	// Prepare schedules describing when we want the tasks to run (10:00 am & 10.01)
	
	// Creates a time based daily persistant schedule	
	TSchedulerItemRef ref1;

	// This is the time when we want the time-based schedule to fire
	TDateTime datetime1(2000, EJanuary, 1, 10, 0, 0, 0);
	TTsTime startTimeForSchedule(datetime1, EFalse); // 10:00 am
	
	{
	CScheduleEntryInfoArray* entryList = new (ELeave) CScheduleEntryInfoArray(1);
	CleanupStack::PushL(entryList);
	TScheduleEntryInfo2 entry1 (startTimeForSchedule, EDaily, 1, 30);
	entryList->AppendL(entry1);

	TInt res = TheScheduler.CreatePersistentSchedule(ref1, *entryList);
	TESTL(res==KErrNone);
	CleanupStack::PopAndDestroy(); // entryList
	}
	
	// Disable the schedule whilst we set it up
	User::LeaveIfError(TheScheduler.DisableSchedule(ref1.iHandle));
	
	
	// Creates a condition based persistant schedule
	TSchedulerItemRef ref2;
	
	// This is the time when we want the condition-based schedule to fire
	TDateTime datetime2(2000, EJanuary, 1, 10, 1, 0, 0);
	TTsTime defaultRuntime(datetime2, EFalse); // 10:01 am

	{
	CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(1);
	CleanupStack::PushL(conditionList);
	TTaskSchedulerCondition condition1;
	condition1.iCategory = KTestUid;
	condition1.iKey		= KTestKey1;
	condition1.iState	= 10;
	condition1.iType	= TTaskSchedulerCondition::EEquals;
	conditionList->AppendL(condition1);

	TInt res = TheScheduler.CreatePersistentSchedule(ref2, *conditionList, defaultRuntime);
	TESTL(res==KErrNone);
	CleanupStack::PopAndDestroy(); // conditionList
	}	
	
	// Disable the schedule whilst we set it up
	User::LeaveIfError(TheScheduler.DisableSchedule(ref2.iHandle));
	
	// Kill the server to check if persisted schedules have been stored
	// Need to turn off JIT dubugging as we are panicking server and we 
	// want test to keep running.
	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse);

	TheScheduler.__FaultServer();
	User::After(1000000);
	
	// Turn on JIT again.
	User::SetJustInTime(jit);
	
	// Connect to the server again
	TInt res = TheScheduler.Connect();
	TESTL(res==KErrNone);

	// Re-register
	TESTL(SchSvrHelpers::RegisterClientL(TheScheduler)==KErrNone);


	// Associate a task with the time-based schedule
	TTaskInfo taskInfo1;
	taskInfo1.iName = KTestName1;
	taskInfo1.iPriority = 2;
	taskInfo1.iRepeat = 0;
	// Create some data associated with this task	
	HBufC* taskData1 = KTaskData1().AllocLC();
	User::LeaveIfError(TheScheduler.ScheduleTask(taskInfo1, *taskData1, ref1.iHandle));
	CleanupStack::PopAndDestroy();	// taskData1
		
	// Associate a task with the condition-based schedule
	TTaskInfo taskInfo2;
	taskInfo2.iName = KTestName2;
	taskInfo2.iPriority = 2;
	taskInfo2.iRepeat = 0;
	// Create some data associated with this task
	HBufC* taskData2 = KTaskData2().AllocLC();
	User::LeaveIfError(TheScheduler.ScheduleTask(taskInfo2, *taskData2, ref2.iHandle));
	CleanupStack::PopAndDestroy(); // taskData2	
	
	SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 50, 0))); // 9:59.50 am

	User::LeaveIfError(TheScheduler.EnableSchedule(ref1.iHandle));
	User::LeaveIfError(TheScheduler.EnableSchedule(ref2.iHandle));
	
	// Now wait for the time-based schedule to fire	
	TESTL(STaskSemaphore::WaitL(KDefaultTimeout) == KErrNone); 

	TTime timeNow;
	timeNow.HomeTime();
	TESTL(SchSvrHelpers::IsTimeTheSameNoSeconds(TTsTime(timeNow, EFalse), startTimeForSchedule));

	// Now wait for the condition-based schedule to fire	
	TESTL(STaskSemaphore::WaitL(KDefaultTimeout) == KErrNone); 

	timeNow.HomeTime();
	TESTL(SchSvrHelpers::IsTimeTheSameNoSeconds(TTsTime(timeNow, EFalse), defaultRuntime));
	
	CleanupStack::PopAndDestroy(); // timezone ID
	CleanupHelpers::KillProcess(KMinimalTaskHandler);
	SetTestStepResult(EPass);	
	return TestStepResult();
	}

TVerdict Cpersist_hometimeStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EFail);
  	CTe_floating_scheduleSuiteStepBase::doTestStepPostambleL();
  	
  	sem1.Close();
	
	// Delete all schedules
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);
	SchSvrHelpers::Pause(2);
	
	// Delete old files
	SchSvrHelpers::DeleteScheduleFilesL();
		
	TheScheduler.Close();
  	
  	SetTestStepResult(EPass);
	return TestStepResult();
	}
