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
#include <schinfo.h>
#include <schinfointernal.h>

#include "Thelpers.h"  // KDefaultTimeout
#include "INC072424_step.h"

_LIT(KLaunch, "Launch");
_LIT(KNotLaunch, "NotLaunch");
_LIT(KStartupState, "StartupState");
_LIT(KExpectedResult, "ExpectedResult");

_LIT(KMissingInputParam, "Missing input parameter");
_LIT(KBadInput, "Fail to parse %S");

// This TEF test has to wait for MinimalTaskHandler to signal a semaphore.
// The 2 minutes wait time is chosen to minimize possibility of early
// timeout. The kernel will terminate the wait when the semaphore is 
// signaled. Thus in reality the test will only wait for a few seconds.
const TInt KSuccessfulTaskLaunchTimeout = KDefaultTimeout;

// In the case where task is not expected to launch, a shorter timeout
// is needed because the test has to wait the full amount of the
// timeout value.
const TInt KNoTaskLaunchTimeout = 10000000;

CSSA_StateTransitionStep::CSSA_StateTransitionStep()
	{
	SetTestStepName(KSSA_StateTransitionStep);
	}

CSSA_StateTransitionStep::~CSSA_StateTransitionStep()
	{
	}

TVerdict CSSA_StateTransitionStep::doTestStepL()
	{
	TStartupStateIdentifier nextState;
	TBool expectingTaskToLaunch;
	GetParametersL(nextState, expectingTaskToLaunch);

	INFO_PRINTF3(_L("SS Transition test, next state 0x%X, expect launch %d"), nextState, expectingTaskToLaunch);

	// Start Schsvr in critical dynamic state. Set current time
	// to 9:55 am
	BeginTestSetupL();

	// Creates a time based schedule    
    TSchedulerItemRef ref;
    // This is the time when we want the time-based schedule to fire
    TDateTime datetime1(2000, EJanuary, 1, 10, 0, 0, 0);
    TTsTime dueTime(datetime1, EFalse);

	_LIT(KSchTestName, "SSA Transition Test");
	AddTransientScheduleL(KSchTestName, dueTime, ref);

	INFO_PRINTF2(_L("Set clock to 4 s before task due and change SS to 0x%X"), nextState);
    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 56, 0))); // 9:59.56 am

    User::LeaveIfError(iScheduler.EnableSchedule(ref.iHandle));
	TInt ret = RequestDomainStateTransition(nextState);

	// If next state is EStartupStateUndefined, then badhandle is returened.
    TEST(ret == KErrNone || ret == KErrBadHandle);

    TInt timeOutMicroSeconds = (expectingTaskToLaunch) ? KSuccessfulTaskLaunchTimeout : KNoTaskLaunchTimeout;

    ret = STaskSemaphore::WaitL(timeOutMicroSeconds);

    INFO_PRINTF2(_L("Sema wait return code is %d"), ret);
	TBool pass = (expectingTaskToLaunch && (ret == KErrNone)) ||
		         (!expectingTaskToLaunch && (ret == KErrTimedOut));
    TEST(pass);
	return TestStepResult();
	}

void CSSA_StateTransitionStep::GetParametersL(TStartupStateIdentifier& aState, TBool& aLaunch)
	{
	_LIT(KUndefinedState, "Undefined");
	_LIT(KBeforeCSState, "BeforeCS");
	_LIT(KCSState, "CS");
	_LIT(KCDState, "CD");
	_LIT(KNCState, "NC");
	_LIT(KNCPlusState, "NCPlus");

	TPtrC data;
	if (!GetStringFromConfig(ConfigSection(), KStartupState, data))
		{
		ERR_PRINTF1(KMissingInputParam);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	if (data == KUndefinedState)
		{
		aState = EStartupStateUndefined;
		}
	else if (data == KBeforeCSState)
		{
		aState = EReservedStartUpState1;
		}
	else if (data == KCSState)
		{
		aState = EStartupStateCriticalStatic;
		}
	else if (data == KCDState)
		{
		aState = EStartupStateCriticalDynamic;
		}
	else if (data == KNCState)
		{
		aState = EStartupStateNonCritical;
		}
	else if (data == KNCPlusState)
		{
		aState = EReservedStartUpState4;
		}
	else
		{
		ERR_PRINTF2(KBadInput, &data);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	// Next parameter
	if (!GetStringFromConfig(ConfigSection(), KExpectedResult, data))
		{
		ERR_PRINTF1(KMissingInputParam);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	if (data == KLaunch)
		{
		aLaunch = ETrue;
		}
	else if (data == KNotLaunch)
		{
		aLaunch = EFalse;
		}
	else
		{
		ERR_PRINTF2(KBadInput, &data);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}
	}

//--------------------------------------------------------
CSSA_PersistentScheduleStep::CSSA_PersistentScheduleStep()
	{
	SetTestStepName(KSSA_PersistentScheduleStep);
	}

CSSA_PersistentScheduleStep::~CSSA_PersistentScheduleStep()
	{
	}

TVerdict CSSA_PersistentScheduleStep::doTestStepL()
	{
	TInt goToNonCritical;
	TBool expectingTaskToLaunch;
	GetParametersL(goToNonCritical, expectingTaskToLaunch);

	INFO_PRINTF1(_L("Persistent schedule test"));
	// Start Schsvr in critical dynamic state. Set current time
	// to 9:55 am
	BeginTestSetupL();

	// Creates a time based schedule    
    TSchedulerItemRef ref;
    // This is the time when we want the time-based schedule to fire
    TDateTime datetime1(2000, EJanuary, 1, 10, 0, 0, 0);
    TTsTime dueTime(datetime1, EFalse);

	_LIT(KSchTestName, "Persistent Schedule Test");
	AddPersistentScheduleL(KSchTestName, dueTime, ref);

	// Set startup state to critical dynamic before re-starting server
	User::LeaveIfError(RequestDomainStateTransition(EStartupStateCriticalDynamic));

	INFO_PRINTF1(_L("Set clock to 4 s before task due"));
    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 56, 0))); // 9:59.56 am

    User::LeaveIfError(iScheduler.EnableSchedule(ref.iHandle));

	// Setup complete. Re-start schsvr and observe outcome.
	RestartSchedulerL();

	if (goToNonCritical)
		{
		User::LeaveIfError(RequestDomainStateTransition(EStartupStateNonCritical));
		}

    TInt timeOutMicroSeconds = (expectingTaskToLaunch) ? KSuccessfulTaskLaunchTimeout : KNoTaskLaunchTimeout;

    TInt ret = STaskSemaphore::WaitL(timeOutMicroSeconds);

    INFO_PRINTF2(_L("Sema wait return code is %d"), ret);
	TBool pass = (expectingTaskToLaunch && (ret == KErrNone)) ||
		         (!expectingTaskToLaunch && (ret == KErrTimedOut));
    TEST(pass);
	return TestStepResult();
	}

void CSSA_PersistentScheduleStep::GetParametersL(TInt& aDoNCTransition, TBool& aLaunch)
	{
	_LIT(KGoToNC, "GoToNC");
	TPtrC strdata;
	TInt intdata, r1, r2;

	r1 = GetIntFromConfig(ConfigSection(), KGoToNC, intdata);
	r2 = GetStringFromConfig(ConfigSection(), KExpectedResult, strdata);
	if (!r1 || !r2)
		{
		ERR_PRINTF1(KMissingInputParam);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}
	aDoNCTransition = intdata;
	aLaunch = (strdata == KLaunch);
	}

//--------------------------------------------------------
CSSA_ConditionScheduleStep::CSSA_ConditionScheduleStep()
	{
	SetTestStepName(KSSA_ConditionScheduleStep);
	}

CSSA_ConditionScheduleStep::~CSSA_ConditionScheduleStep()
	{
	// Reset the value so as not to affect next test
	RProperty::Set(KTestUid, KTestKey1, 0);
	}

TVerdict CSSA_ConditionScheduleStep::doTestStepL()
	{
	TInt matchCondFirst, useTransient;
	GetParametersL(matchCondFirst, useTransient);

	INFO_PRINTF1(_L("Condition schedule test"));
	// Start Schsvr in critical dynamic state. Set current time
	// to 9:55 am
	BeginTestSetupL();

	// Creates a condition based schedule    
    TSchedulerItemRef ref;
    // If condition not met, task is run one year from now.
    TDateTime datetime1(2001, EJanuary, 1, 10, 0, 0, 0);
    TTsTime dueTime(datetime1, EFalse);

	_LIT(KSchTestName, "Condition Schedule Test");
	AddConditionScheduleL(KSchTestName, dueTime, ref, KTestKey1, useTransient);
    User::LeaveIfError(iScheduler.EnableSchedule(ref.iHandle));

	if (matchCondFirst)
		{
		User::LeaveIfError(RProperty::Set(KTestUid, KTestKey1, KKeyValue));
		}
	else
		{
		User::LeaveIfError(RequestDomainStateTransition(EStartupStateNonCritical));
		}

    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 56, 0))); // 9:59.56 am

    TInt ret = STaskSemaphore::WaitL(KNoTaskLaunchTimeout);
    INFO_PRINTF2(_L("Expecting timeout, return code is %d"), ret);
	TESTL(ret == KErrTimedOut);

	if (matchCondFirst)
		{
		User::LeaveIfError(RequestDomainStateTransition(EStartupStateNonCritical));
		}
	else
		{
		User::LeaveIfError(RProperty::Set(KTestUid, KTestKey1, KKeyValue));
		}

    ret = STaskSemaphore::WaitL(KSuccessfulTaskLaunchTimeout);
    INFO_PRINTF2(_L("Expecting ok, return code is %d"), ret);
	TESTL(ret == KErrNone);

	return TestStepResult();
	}

void CSSA_ConditionScheduleStep::GetParametersL(TInt& aMatchCondFirst,
												TInt& aUseTransient)
	{
	_LIT(KMatchCondBeforeNC, "MatchCondBeforeNC");
	_LIT(KUseTransient, "UseTransient");
	TInt r1, r2;

	r1 = GetIntFromConfig(ConfigSection(), KMatchCondBeforeNC, aMatchCondFirst);
	r2 = GetIntFromConfig(ConfigSection(), KUseTransient, aUseTransient);
	if (!r1 || !r2)
		{
		ERR_PRINTF1(KMissingInputParam);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}
	}

//---------------------------------------------------------------------
CSSA_EnvironmentChangeStep::CSSA_EnvironmentChangeStep()
	{
	SetTestStepName(KSSA_EnvironmentChangeStep);
	}

CSSA_EnvironmentChangeStep::~CSSA_EnvironmentChangeStep()
	{
	}

TVerdict CSSA_EnvironmentChangeStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Transient schedule with System time rewind backward"));

	// Start Schsvr in critical dynamic state. Set current time
	// to 9:55 am
	BeginTestSetupL();

	// Creates a time based schedule    
    TSchedulerItemRef ref;
    // This is the time when we want the time-based schedule to fire
    TDateTime datetime1(2000, EJanuary, 1, 10, 0, 0, 0);
    TTsTime dueTime(datetime1, EFalse);

	_LIT(KSchTestName, "Environment Change Test");
	AddTransientScheduleL(KSchTestName, dueTime, ref);
    User::LeaveIfError(iScheduler.EnableSchedule(ref.iHandle));

	INFO_PRINTF1(_L("Set clock to 4 s before task due"));
    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 56, 0))); // 9:59.56 am

    TInt ret = STaskSemaphore::WaitL(KNoTaskLaunchTimeout);
    INFO_PRINTF2(_L("Expecting timeout, return code is %d"), ret);
	TESTL(ret == KErrTimedOut);

	INFO_PRINTF1(_L("Rewind clock back to 4 s before task due and set SS to NC"));
    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 56, 0))); // 9:59.56 am
	User::LeaveIfError(RequestDomainStateTransition(EStartupStateNonCritical));

    ret = STaskSemaphore::WaitL(KSuccessfulTaskLaunchTimeout);
    INFO_PRINTF2(_L("Sema wait return code is %d"), ret);
    TEST(ret == KErrNone);
	return TestStepResult();
	}

//---------------------------------------------------------------------
CSSA_EditTransientStep::CSSA_EditTransientStep()
	{
	SetTestStepName(KSSA_EditTransientStep);
	}

CSSA_EditTransientStep::~CSSA_EditTransientStep()
	{
	}

TVerdict CSSA_EditTransientStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Edit Transient schedule"));

	// Start Schsvr in critical dynamic state. Set current time
	// to 9:55 am
	BeginTestSetupL();

	// Creates a time based schedule    
    TSchedulerItemRef ref;
    // This is the time when we want the time-based schedule to fire
    TDateTime datetime1(2000, EJanuary, 1, 10, 0, 0, 0);
    TTsTime dueTime(datetime1, EFalse);

	_LIT(KSchTestName, "Edit Transient Schedule Test");
	AddTransientScheduleL(KSchTestName, dueTime, ref);
    User::LeaveIfError(iScheduler.EnableSchedule(ref.iHandle));

    TDateTime datetime2(2000, EJanuary, 1, 10, 2, 0, 0); // 10:02.00 am
    TTsTime newduetime(datetime2, EFalse);
    CScheduleEntryInfoArray* entryList = new (ELeave) CScheduleEntryInfoArray(1);
    CleanupStack::PushL(entryList);

    TScheduleEntryInfo2 entry (newduetime, EDaily, 1, 30);
    entryList->AppendL(entry);

    User::LeaveIfError(iScheduler.EditSchedule(ref.iHandle, *entryList));
    CleanupStack::PopAndDestroy(entryList);

	INFO_PRINTF1(_L("Set clock to 4 s before old due time and set SS = NC"));
    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 56, 0))); // 9:59.56 am

	User::LeaveIfError(RequestDomainStateTransition(EStartupStateNonCritical));

    TInt ret = STaskSemaphore::WaitL(KNoTaskLaunchTimeout);
    INFO_PRINTF2(_L("Expecting timeout, return code is %d"), ret);
	TESTL(ret == KErrTimedOut);


	INFO_PRINTF1(_L("Set clock to 4 s before new due time."));
    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 10, 1, 56, 0))); // 10:01.56 am

    ret = STaskSemaphore::WaitL(KSuccessfulTaskLaunchTimeout);
    INFO_PRINTF2(_L("Sema wait return code is %d"), ret);
    TEST(ret == KErrNone);
	return TestStepResult();
	}

//---------------------------------------------------------------------
CSSA_EditPersistentStep::CSSA_EditPersistentStep()
	{
	SetTestStepName(KSSA_EditPersistentStep);
	}

CSSA_EditPersistentStep::~CSSA_EditPersistentStep()
	{
	}

TVerdict CSSA_EditPersistentStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Edit Persistent schedule"));

	// Start Schsvr in critical dynamic state. Set current time
	// to 9:55 am
	BeginTestSetupL();

	// Creates a time based schedule    
    TSchedulerItemRef ref;
    // This is the time when we want the time-based schedule to fire
    TDateTime datetime1(2000, EJanuary, 1, 10, 0, 0, 0);
    TTsTime dueTime(datetime1, EFalse);

	_LIT(KSchTestName, "Edit Persistent Schedule Test");
	AddPersistentScheduleL(KSchTestName, dueTime, ref);
    User::LeaveIfError(iScheduler.EnableSchedule(ref.iHandle));

    TDateTime datetime2(2000, EJanuary, 1, 10, 2, 0, 0); // 10:02.00 am
    TTsTime newduetime(datetime2, EFalse);
    CScheduleEntryInfoArray* entryList = new (ELeave) CScheduleEntryInfoArray(1);
    CleanupStack::PushL(entryList);

    TScheduleEntryInfo2 entry (newduetime, EDaily, 1, 30);
    entryList->AppendL(entry);

    User::LeaveIfError(iScheduler.EditSchedule(ref.iHandle, *entryList));
    CleanupStack::PopAndDestroy(entryList);

	User::LeaveIfError(RequestDomainStateTransition(EStartupStateNonCritical));
	RestartSchedulerL();


	INFO_PRINTF1(_L("Schsvr re-started. Set clock to 4 s before old due time and set SS = NC"));
    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 9, 59, 56, 0))); // 9:59.56 am

    TInt ret = STaskSemaphore::WaitL(KNoTaskLaunchTimeout);
    INFO_PRINTF2(_L("Expecting timeout, return code is %d"), ret);
	TESTL(ret == KErrTimedOut);


	INFO_PRINTF1(_L("Set clock to 4 s before new due time."));
    SchSvrHelpers::SetHomeTimeL(TTime(TDateTime(2000, EJanuary, 1, 10, 1, 56, 0))); // 10:01.56 am

    ret = STaskSemaphore::WaitL(KSuccessfulTaskLaunchTimeout);
    INFO_PRINTF2(_L("Sema wait return code is %d"), ret);
    TEST(ret == KErrNone);
	return TestStepResult();
	}

//---------------------------------------------------------------------
CSSA_EditConditionStep::CSSA_EditConditionStep()
	{
	SetTestStepName(KSSA_EditConditionStep);
	}

CSSA_EditConditionStep::~CSSA_EditConditionStep()
	{
 	// reset RProperty value
	RProperty::Set(KTestUid, KTestKey1, 0);
	RProperty::Set(KTestUid, KTestKey2, 0);
	}

TVerdict CSSA_EditConditionStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Edit Condition schedule"));

	// Start Schsvr in critical dynamic state. Set current time
	// to 9:55 am
	BeginTestSetupL();

	// Creates a condition based schedule    
    TSchedulerItemRef ref;
    // If condition not met, task is run one year from now.
    TDateTime datetime1(2001, EJanuary, 1, 10, 0, 0, 0);
    TTsTime dueTime(datetime1, EFalse);

	_LIT(KSchTestName, "Edit Persistent Condition Schedule Test");
	AddConditionScheduleL(KSchTestName, dueTime, ref, KTestKey1, EFalse);
    User::LeaveIfError(iScheduler.EnableSchedule(ref.iHandle));

	CreatePSKeyL(KTestKey2);

    CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(1);
    CleanupStack::PushL(conditionList);
    TTaskSchedulerCondition condition1;
    condition1.iCategory = KTestUid;
    condition1.iKey        = KTestKey2;
    condition1.iState    = KKeyValue;
    condition1.iType    = TTaskSchedulerCondition::EEquals;
    conditionList->AppendL(condition1);

    User::LeaveIfError(iScheduler.EditSchedule(ref.iHandle, *conditionList, dueTime));
    CleanupStack::PopAndDestroy(conditionList);

	User::LeaveIfError(RequestDomainStateTransition(EStartupStateNonCritical));
	RestartSchedulerL();


	INFO_PRINTF1(_L("Schsvr re-started in NC. Set old property to cond met."));
	User::LeaveIfError(RProperty::Set(KTestUid, KTestKey1, KKeyValue));

    TInt ret = STaskSemaphore::WaitL(KNoTaskLaunchTimeout);
    INFO_PRINTF2(_L("Expecting timeout, return code is %d"), ret);
	TESTL(ret == KErrTimedOut);

	INFO_PRINTF1(_L("Set new property to cond met."));
	User::LeaveIfError(RProperty::Set(KTestUid, KTestKey2, KKeyValue));

    ret = STaskSemaphore::WaitL(KSuccessfulTaskLaunchTimeout);
    INFO_PRINTF2(_L("Sema wait return code is %d"), ret);
    TEST(ret == KErrNone);
	return TestStepResult();
	}
