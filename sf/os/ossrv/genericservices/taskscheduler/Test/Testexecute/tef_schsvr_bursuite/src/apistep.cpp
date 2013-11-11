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

/**
 @file
*/
#include "apistep.h"
#include "tef_schsvr_bursuitedefs.h"

CAPIStep::~CAPIStep()
/**
 * Destructor
 */
	{
	}

CAPIStep::CAPIStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KAPIStep);
	}

void CAPIStep::CreateScheduleForTestL(TSchedulerItemRef& aRef1, TSchedulerItemRef& aRef2)
	{
	// Creates a condition based schedule    
    // If condition not met, task is run one year from now.
    TDateTime datetime1(2001, EJanuary, 1, 10, 0, 0, 0);
    TTsTime dueTime(datetime1, EFalse);

    CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(2);
    CleanupStack::PushL(conditionList);

	AddTestConditionL(*conditionList, KTestKey1);	

    CScheduleEntryInfoArray* entryList = new (ELeave) CScheduleEntryInfoArray(1);
    CleanupStack::PushL(entryList);

    TScheduleEntryInfo2 entry (dueTime, EDaily, 1, 30);
    entryList->AppendL(entry);

	TInt ret;
	//ECreateTimeSchedule
    ret = iScheduler.CreatePersistentSchedule(aRef1, *entryList);
    TESTL(ret==KErrNone);

    //ECreateConditionSchedule
    ret = iScheduler.CreatePersistentSchedule(aRef2, *conditionList, dueTime);
    TESTL(ret==KErrNone);
    
    CleanupStack::PopAndDestroy(entryList);    
	CleanupStack::PopAndDestroy(conditionList);     
	}
	
void CAPIStep::CallPersistingFunctionsL(TSchedulerItemRef& aRef1, TSchedulerItemRef& aRef2)
	{
	// Creates a condition based schedule    
    // If condition not met, task is run one year from now.
    TDateTime datetime1(2001, EJanuary, 1, 10, 0, 0, 0);
    TTsTime dueTime(datetime1, EFalse);

	_LIT(KSchTestName, "API Test");
	
    CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(2);
    CleanupStack::PushL(conditionList);

	AddTestConditionL(*conditionList, KTestKey1);	

    CScheduleEntryInfoArray* entryList = new (ELeave) CScheduleEntryInfoArray(1);
    CleanupStack::PushL(entryList);

    TScheduleEntryInfo2 entry (dueTime, EDaily, 1, 30);
    entryList->AppendL(entry);

	TInt ret;
	//ECreateTimeSchedule
    ret = iScheduler.CreatePersistentSchedule(aRef1, *entryList);
    TESTL(ret==KErrServerBusy);
    
    //EEditTimeSchedule
    ret = iScheduler.EditSchedule(aRef1.iHandle, *entryList);
    TESTL(ret==KErrServerBusy);

    //ECreateConditionSchedule
    ret = iScheduler.CreatePersistentSchedule(aRef2, *conditionList, dueTime);
    TESTL(ret==KErrServerBusy);

    //EEditConditionSchedule
    ret = iScheduler.EditSchedule(aRef2.iHandle, *conditionList, dueTime);
    TESTL(ret==KErrServerBusy);
    
    //EDisableSchedule
    ret = iScheduler.DisableSchedule(aRef1.iHandle);
	TESTL(ret==KErrServerBusy);
	
	//EEnableSchedule
    ret = iScheduler.EnableSchedule(aRef1.iHandle);
	TESTL(ret==KErrServerBusy);
	    	
    // Associate a task with the time-based schedule
    TTaskInfo taskInfo;
    taskInfo.iName = KSchTestName;
    taskInfo.iPriority = 2;
    taskInfo.iRepeat = 0;
    // Create some data associated with this task    
    HBufC* taskData = KTaskData().AllocLC();
    
    //EScheduleTask
    ret = iScheduler.ScheduleTask(taskInfo, *taskData, aRef1.iHandle);
	TESTL(ret==KErrServerBusy);

    //EDeleteTask
    ret = iScheduler.DeleteTask(aRef1.iHandle);
	TESTL(ret==KErrServerBusy);
	
	//EDeleteSchedule
	ret = iScheduler.DeleteSchedule(aRef1.iHandle);
	TESTL(ret==KErrServerBusy);
	
	// following API calls still work during backup/restore because they are working on a 
	// non-persisting (transient) schedule which is not affected 

	//EScheduleTask (transient)
	TSchedulerItemRef transientRef;
    ret = iScheduler.ScheduleTask(taskInfo, *taskData, transientRef, *entryList);
    TESTL(ret==KErrNone);

    //EDisableSchedule
    ret = iScheduler.DisableSchedule(transientRef.iHandle);
	TESTL(ret==KErrNone);
	
	//EEnableSchedule
    ret = iScheduler.EnableSchedule(transientRef.iHandle);
	TESTL(ret==KErrNone);

    //EDeleteTask
    ret = iScheduler.DeleteTask(transientRef.iHandle);
	TESTL(ret==KErrNone);

    CleanupStack::PopAndDestroy(taskData);
    CleanupStack::PopAndDestroy(entryList);    
	CleanupStack::PopAndDestroy(conditionList);    
	}

TVerdict CAPIStep::doTestStepPreambleL()
	{
    SetTestStepResult(EFail);
        
	CTefSchsvrBURSuiteSimStepBase::doTestStepPreambleL();        
    
	// Start Schsvr in critical dynamic state. Set current time
	// to 9:55 am
	BeginTestSetupL();

    SetTestStepResult(EPass);
    return TestStepResult();
	}

TVerdict CAPIStep::doTestStepL()
	{
    SetTestStepResult(EFail);
    
	INFO_PRINTF1(_L("API Behaviour test during BUR"));

    TSchedulerItemRef ref1,ref2;
	// Create a schedule for test
	CreateScheduleForTestL(ref1, ref2);
	
	// Simulate backup
	Simulate_StartBackupL();
    CallPersistingFunctionsL(ref1, ref2);
	Simulate_EndBackupL();

	// Simulate restore
	Simulate_StartRestoreL();
    CallPersistingFunctionsL(ref1, ref2);
	Simulate_EndRestoreL(ETrue);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

