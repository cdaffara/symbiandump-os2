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
#include <schinfo.h>
#include <schinfointernal.h>

#include "tef_schsvr_bursuitestepbase.h"
#include "tef_schsvr_bursuitedefs.h"

CTefSchsvrBURSuiteStepBase::CTefSchsvrBURSuiteStepBase()
	{
	}

CTefSchsvrBURSuiteStepBase::~CTefSchsvrBURSuiteStepBase()
	{
	iScheduler.Close();
	}

void CTefSchsvrBURSuiteStepBase::RestartSchedulerL()
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

void CTefSchsvrBURSuiteStepBase::AddTestConditionL(CSchConditionArray& aConditionList, TInt aKeyId)
    {
    TTaskSchedulerCondition condition1;
    condition1.iCategory = KTestUid;
    condition1.iKey        = aKeyId;
    condition1.iState    = KKeyValue;
    condition1.iType    = TTaskSchedulerCondition::EEquals;
    aConditionList.AppendL(condition1);
    }

void CTefSchsvrBURSuiteStepBase::AddBackupConditionL(CSchConditionArray& aConditionList)
    {
    TTaskSchedulerCondition condition1;
    condition1.iCategory = KTestUid; 
    condition1.iKey      = conn::KUidBackupRestoreKey;
    condition1.iState    = conn::EBackupBase|conn::EBURBackupPartial;
    condition1.iType     = TTaskSchedulerCondition::EEquals;
    aConditionList.AppendL(condition1);
    }

void CTefSchsvrBURSuiteStepBase::AddRestoreConditionL(CSchConditionArray& aConditionList)
    {
    TTaskSchedulerCondition condition1;
    condition1.iCategory = KTestUid; 
    condition1.iKey      = conn::KUidBackupRestoreKey;
    condition1.iState    = conn::EBackupBase|conn::EBURRestorePartial;
    condition1.iType     = TTaskSchedulerCondition::EEquals;
    aConditionList.AppendL(condition1);
    }

void CTefSchsvrBURSuiteStepBase::AddTimeScheduleL(const TDesC& aName,
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

void CTefSchsvrBURSuiteStepBase::AddTransientTimeScheduleL(const TDesC& aName,
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
    }
    
void CTefSchsvrBURSuiteStepBase::AddConditionScheduleL(const TDesC& aName,
							   			const TTsTime& aDefaultRunTime,
							   			TSchedulerItemRef& aRef,
							   			CSchConditionArray& aConditionList)
    {
    // Associate a task with the condition-based schedule
    TTaskInfo taskInfo;
    taskInfo.iName = aName;
    taskInfo.iPriority = 2;
    taskInfo.iRepeat = 0;
    // Create some data associated with this task    
    HBufC* taskData = KTaskData().AllocLC();

	TInt ret;
	// want persistent condition schedule
	ret = iScheduler.CreatePersistentSchedule(aRef, aConditionList, aDefaultRunTime);
	TESTL(ret==KErrNone);

	User::LeaveIfError(iScheduler.ScheduleTask(taskInfo, *taskData, aRef.iHandle));
        
    CleanupStack::PopAndDestroy(taskData);
    }
    
void CTefSchsvrBURSuiteStepBase::AddTransientConditionScheduleL(const TDesC& aName,
							   			const TTsTime& aDefaultRunTime,
							   			CSchConditionArray& aConditionList)
    {
    // Associate a task with the condition-based schedule
    TTaskInfo taskInfo;
    taskInfo.iName = aName;
    taskInfo.iPriority = 2;
    taskInfo.iRepeat = 0;
    // Create some data associated with this task    
    HBufC* taskData = KTaskData().AllocLC();

    //also add a transient schedule here
    TSchedulerItemRef ref2;
    User::LeaveIfError(iScheduler.ScheduleTask(taskInfo,*taskData,ref2,aConditionList,aDefaultRunTime));
        
    CleanupStack::PopAndDestroy(taskData);
    }    
    

void CTefSchsvrBURSuiteStepBase::CreatePSKeyL(TInt aKeyId)
	{
    // create P&S variables for condition based tests
    TInt ret = RProperty::Define(KTestUid, aKeyId, RProperty::EInt);
	if (ret != KErrNone && ret != KErrAlreadyExists)
		{
		User::Leave(ret);
		}
    User::LeaveIfError(RProperty::Set(KTestUid, aKeyId, 0));
	}

void CTefSchsvrBURSuiteStepBase::BeginTestSetupL()
	{
    RestartSchedulerL();

    // Set the time to a known value
    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 45, 0, 0))); // 9:45 am
	}

void CTefSchsvrBURSuiteStepBase::GetParameterL(const TDesC& aKey, TInt& aValue)
	{
	TInt r1;
	
	r1 = GetIntFromConfig(ConfigSection(), aKey, aValue);
	if (!r1)
		{
		ERR_PRINTF1(KMissingInputParam);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}
	}


