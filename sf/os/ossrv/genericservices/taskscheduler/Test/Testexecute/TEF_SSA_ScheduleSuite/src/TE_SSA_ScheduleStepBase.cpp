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
//

#include <domaindefs.h>
#include <startupdomaindefs.h>
#include <schinfointernal.h>
#include "TE_SSA_ScheduleStepBase.h"

CSSA_ScheduleStepBase::CSSA_ScheduleStepBase()
	{
	}

CSSA_ScheduleStepBase::~CSSA_ScheduleStepBase()
	{
	iDomainManager.Close();

    // Delete all schedules
    TRAP_IGNORE(SchSvrHelpers::DeleteAllSchedulesL(iScheduler));
    SchSvrHelpers::Pause(2);
    
    // Delete old files
    TRAP_IGNORE(SchSvrHelpers::DeleteScheduleFilesL());

	iScheduler.Close();
    iSem.Close();
	}

TVerdict CSSA_ScheduleStepBase::doTestStepPreambleL()
	{
    SetTestStepResult(EFail);
        
    // Delete old files.
    SchSvrHelpers::DeleteScheduleFilesL();

    iSem.CreateL();

    TInt ret = iDomainManager.Connect(KDmHierarchyIdStartup);
    iConnectedToDM = (ret==KErrNone);

	User::LeaveIfError(iScheduler.Connect());
        
    // Registering Client
	User::LeaveIfError(SchSvrHelpers::RegisterClientL(iScheduler));

	// Tests with timezone set to Europe, London
    RTz tz;
    tz.Connect();
    CTzId* tzId = CTzId::NewL(2592); //set the timezone to Europe/London
    CleanupStack::PushL(tzId);
    tz.SetTimeZoneL(*tzId);
	CleanupStack::PopAndDestroy(tzId);
	tz.Close();

    SetTestStepResult(EPass);
    return TestStepResult();
	}

TVerdict CSSA_ScheduleStepBase::doTestStepPostambleL()
	{
	TInt err = CleanupHelpers::KillProcess(KMinimalTaskHandler);
	if((err != KErrDied) && (err != KErrNotFound))
		{
		User::LeaveIfError(err);	
		}
	return TestStepResult();
	}

TInt CSSA_ScheduleStepBase::RequestDomainStateTransition(TStartupStateIdentifier aTargetState)
    {
    TRequestStatus requeststatus;

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
    iDomainManager.RequestDomainTransition(KSM2OSServicesDomain3,
#else
    iDomainManager.RequestDomainTransition(KBaseServicesDomain3,
#endif //SYMBIAN_SYSTEM_STATE_MANAGEMENT
        aTargetState, ETraverseParentsFirst, requeststatus);

    User::WaitForRequest(requeststatus);
    return requeststatus.Int();
    }

void CSSA_ScheduleStepBase::RestartSchedulerL()
    {
    TBool jit = User::JustInTime();
    User::SetJustInTime(EFalse);
	CleanupHelpers::KillProcess(KSchexe);
    User::After(1000000);

    // Restore original JIT
    User::SetJustInTime(jit);

    TESTL(iScheduler.Connect() == KErrNone);

    // Re-register
    TESTL(SchSvrHelpers::RegisterClientL(iScheduler)==KErrNone);
    }

void CSSA_ScheduleStepBase::AddPersistentScheduleL(const TDesC& aName,
												const TTsTime& aDueTime,
												TSchedulerItemRef& aRef)
    {
    CScheduleEntryInfoArray* entryList = new (ELeave) CScheduleEntryInfoArray(1);
    CleanupStack::PushL(entryList);

    TScheduleEntryInfo2 entry (aDueTime, EDaily, 1, 30);
    entryList->AppendL(entry);

    TInt ret = iScheduler.CreatePersistentSchedule(aRef, *entryList);
    TESTL(ret==KErrNone);
    CleanupStack::PopAndDestroy(entryList);

    // Disable the schedule whilst we set it up
    User::LeaveIfError(iScheduler.DisableSchedule(aRef.iHandle));

    // Associate a task with the time-based schedule
    TTaskInfo taskInfo;
    taskInfo.iName = aName;
    taskInfo.iPriority = 2;
    taskInfo.iRepeat = 0;
    // Create some data associated with this task    
    HBufC* taskData = KTaskData().AllocLC();
    User::LeaveIfError(iScheduler.ScheduleTask(taskInfo, *taskData, aRef.iHandle));

    CleanupStack::PopAndDestroy(taskData);
    }

void CSSA_ScheduleStepBase::AddTransientScheduleL(const TDesC& aName,
							   					const TTsTime& aDueTime,
							   					TSchedulerItemRef& aRef)
    {
    CScheduleEntryInfoArray* entryList = new (ELeave) CScheduleEntryInfoArray(1);
    CleanupStack::PushL(entryList);

    TScheduleEntryInfo2 entry (aDueTime, EDaily, 1, 30);
    entryList->AppendL(entry);

    // Associate a task with the time-based schedule
    TTaskInfo taskInfo;
    taskInfo.iName = aName;
    taskInfo.iPriority = 2;
    taskInfo.iRepeat = 0;
    // Create some data associated with this task    
    HBufC* taskData = KTaskData().AllocLC();

    User::LeaveIfError(iScheduler.ScheduleTask(taskInfo, *taskData, aRef, *entryList));

    CleanupStack::PopAndDestroy(taskData);
    CleanupStack::PopAndDestroy(entryList);

    // Disable the schedule as persistent and condition-base do that.
    User::LeaveIfError(iScheduler.DisableSchedule(aRef.iHandle));
    }

void CSSA_ScheduleStepBase::AddConditionScheduleL(const TDesC& aName,
							   			const TTsTime& aDefaultRunTime,
							   			TSchedulerItemRef& aRef,
							   			TInt aKeyId,
										TBool aUseTransient)
    {
	CreatePSKeyL(aKeyId);

    CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(1);
    CleanupStack::PushL(conditionList);
    TTaskSchedulerCondition condition1;
    condition1.iCategory = KTestUid;
    condition1.iKey        = aKeyId;
    condition1.iState    = KKeyValue;
    condition1.iType    = TTaskSchedulerCondition::EEquals;
    conditionList->AppendL(condition1);

    // Associate a task with the condition-based schedule
    TTaskInfo taskInfo;
    taskInfo.iName = aName;
    taskInfo.iPriority = 2;
    taskInfo.iRepeat = 0;
    // Create some data associated with this task    
    HBufC* taskData = KTaskData().AllocLC();

	TInt ret;
	if (!aUseTransient)
		{
		// want persistent condition schedule
    	ret = iScheduler.CreatePersistentSchedule(aRef, *conditionList, aDefaultRunTime);
    	TESTL(ret==KErrNone);

    	User::LeaveIfError(iScheduler.DisableSchedule(aRef.iHandle));
		User::LeaveIfError(iScheduler.ScheduleTask(taskInfo, *taskData, aRef.iHandle));
		}
	else
		{
		// doing transient condition schedule
		User::LeaveIfError(iScheduler.ScheduleTask(taskInfo, *taskData, aRef, *conditionList, aDefaultRunTime));
    	User::LeaveIfError(iScheduler.DisableSchedule(aRef.iHandle));
		}

    CleanupStack::PopAndDestroy(taskData);
    CleanupStack::PopAndDestroy(conditionList);
    }

void CSSA_ScheduleStepBase::CreatePSKeyL(TInt aKeyId)
	{
    // create P&S variables for condition based tests
    TInt ret = RProperty::Define(KTestUid, aKeyId, RProperty::EInt);
	if (ret != KErrNone && ret != KErrAlreadyExists)
		{
		User::Leave(ret);
		}
    User::LeaveIfError(RProperty::Set(KTestUid, aKeyId, 0));
	}

void CSSA_ScheduleStepBase::BeginTestSetupL()
	{

	TInt ret(KErrTimedOut);
	TInt i;
	for (i = 0; (ret == KErrTimedOut) && (i < 3); i++)
		{
		ret = RequestDomainStateTransition(EStartupStateCriticalDynamic);
		if (ret == KErrTimedOut)
			{
			User::AfterHighRes(100000); // a tenth of a second
			}
		} 

    TESTL (ret==KErrNone);
    RestartSchedulerL();

    // Set the time to a known value
    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 55, 0, 0))); // 9:55 am
	}
