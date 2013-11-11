// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

// User includes
#include "SchTimer.h"
#include "SCHMAN.H"
#include "SCHEDULE.H"
#include "schlogger.h"

// system includes
#include <e32property.h>

/**
An instance of this class is used for each different time that a
schedule needs to be run. It is created solely in the 
CScheduleCriteriaManager class.  The timer is updated via the 
SetNext method.  When the time has been reached it notifies the schedule
manager via the CScheduleCriteriaManager::DueSchedule() method.

@internalComponent
*/
_LIT(KTaskSchedulerPanic, "TaskScheduler ");

NONSHARABLE_CLASS(CScheduleTimer) : public CTimer
	{
public:
	~CScheduleTimer();
	static CScheduleTimer* NewL(TInt aSchedule, CScheduleCriteriaManager& aManager);
	
	void SetNext(const TTsTime& aNextTime);
	TInt Id();

	//list capability
	static TInt Offset();
private:
	// From CTimer
	void RunL();

	CScheduleTimer(TInt aSchedule, CScheduleCriteriaManager& aManager);
	void ConstructL();
	
private:
	TTsTime iDueTime;
	TInt iScheduleHandle;
	TSglQueLink iLink;
	CScheduleCriteriaManager& iConditonManager;
	};

CScheduleTimer* CScheduleTimer::NewL(TInt aSchedule, CScheduleCriteriaManager& aManager)
	{
	CScheduleTimer* self = new(ELeave) CScheduleTimer(aSchedule, aManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CScheduleTimer::CScheduleTimer(TInt aSchedule, CScheduleCriteriaManager& aManager)
:	CTimer(EPriorityStandard),
	iScheduleHandle(aSchedule),
	iConditonManager(aManager)
	{
	}

CScheduleTimer::~CScheduleTimer()
	{
	Cancel();
	}

void CScheduleTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CScheduleTimer::SetNext(const TTsTime& aNewTime)
	{
	// Can't handle (unlikely but theoretical) situation when year is BC (not AD)
	__ASSERT_ALWAYS(aNewTime.GetUtcTime().DateTime().Year()>0,User::Invariant());
	if (IsActive())
		Cancel();

	iDueTime=aNewTime;
	TTime currentTime;
	currentTime.UniversalTime();

	if	(aNewTime.GetUtcTime()>currentTime)
		AtUTC(aNewTime.GetUtcTime());
	else
		AtUTC(currentTime);
	}

//	Respond to an task being due.  RunL is only called once!
void CScheduleTimer::RunL()
	{
	if(iStatus != KErrAbort)
		iConditonManager.DueSchedule(iScheduleHandle);

	// RunL() will also be triggered if the system time is changed, with iStatus
	// set to KErrAbort. In this case DueSchedule() should not be called.
	// If the system time has been changed, the schedule needs to be requeued. 
	// This has already been done automatically by CTaskScheduler::HandleEnvironmentChange()
	// [called by the AO CEnvironmentChangeNotifier],
	// as the active object CEnvironmentChangeNotifier has a higher priority than CScheduleTimer.
	}	

TInt CScheduleTimer::Offset()
	{
	return (_FOFF(CScheduleTimer, iLink));
	}

TInt CScheduleTimer::Id()
	{
	return iScheduleHandle;
	}

//		
// class CPropertyNotifier
// This class handles changes to P&S variables and notifies the
// CConditionManager class when a condition is satisfied.
NONSHARABLE_CLASS(CPropertyNotifier) : public CActive
	{
public:
	~CPropertyNotifier();
	static CPropertyNotifier* NewL(CConditionManager& aManager);
	
private:	
	CPropertyNotifier(CConditionManager& aManager);
	void AttachL();
	// From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

public:
	void SetPropertyL(const TUid& aCategory, TUint aKey);

private:
	TUid iCategory;
	TUint iKey;
	RProperty iProperty;
	CConditionManager& iConditionManager;
	};

//		
// class CConditionManager
// This class manages a set of conditions for each schedule.  It is used
// solely by the CScheduleCriteriaManager class. When the set of conditions 
// is met, a the schedule manager is notified.
NONSHARABLE_CLASS(CConditionManager) : public CActive
	{
public:
	~CConditionManager();
	static CConditionManager* NewL(TInt aSchedule, CScheduleCriteriaManager& aManager);
	
private:	
	CConditionManager(TInt aSchedule, CScheduleCriteriaManager& aManager);
	TBool MatchAllConditionsL() const;
	TBool HasVariable(const TUid& aCategory, TUint aKey) const;
	void CompleteRequest();
	// From CActive
	void RunL();
	void DoCancel();

public:
	void ReplaceL(const RArray<TTaskSchedulerCondition>& aConditions);
	TInt Id();
	void VariableChangedL(const TUid& aCategory, TUint aKey);
	//list capability
	static TInt Offset();	
	
private:
	RArray<TTaskSchedulerCondition> iConditions;
	RPointerArray<CPropertyNotifier> iPropertyNotifiers;
	TInt iScheduleHandle;
	TSglQueLink iLink;
	CScheduleCriteriaManager& iManager;
	};

CConditionManager* CConditionManager::NewL(TInt aSchedule, CScheduleCriteriaManager& aManager)
	{
	CConditionManager* self = new(ELeave) CConditionManager(aSchedule, aManager);
	return self;
	}

CConditionManager::CConditionManager(TInt aSchedule, CScheduleCriteriaManager& aManager)
:	CActive(EPriorityStandard+1), //make priority higher that propertynotifier AO
	iScheduleHandle(aSchedule),
	iManager(aManager)
	{
	CActiveScheduler::Add(this);
	}

CConditionManager::~CConditionManager()
	{
	Cancel();
	iPropertyNotifiers.ResetAndDestroy();
	iConditions.Reset();		
	}

//This function evaluates (aValue <op> aState) expression, where "op" could be
//"==", "!=", ">", "<", depending on aType value, and returns the result of expression.
static TBool DoMatchCondition(TTaskSchedulerCondition::TConditionType aType,
							  TInt aValue, 
							  TInt aState)
	{
	if(aType == TTaskSchedulerCondition::EEquals)
		{
		if(aValue == aState)
			{
			return ETrue;
			}
		}
	else if(aType == TTaskSchedulerCondition::ENotEquals)
		{
		if(aValue != aState)
			{
			return ETrue;
			}
		}
	else if(aType == TTaskSchedulerCondition::EGreaterThan)
		{
		if(aValue > aState)
			{
			return ETrue;
			}
		}
	else if(aType == TTaskSchedulerCondition::ELessThan)
		{
		if(aValue < aState)
			{
			return ETrue;
			}
		}
	else
		{
		__ASSERT_ALWAYS(0, User::Invariant());
		}
	return EFalse;
	}

void CConditionManager::ReplaceL(const RArray<TTaskSchedulerCondition>& aConditions)
	{
	// Ensure any active requests are cancelled
	if(IsActive()) 
	{ 
		Cancel(); 
	}

	//destroying existing ones will cancel outstanding requests
	iPropertyNotifiers.ResetAndDestroy(); 
	iConditions.Reset();
	const TInt count = aConditions.Count();
	TInt i;
	//Check that the properties already exist
	for(i=0;i<count;++i)
		{
		TInt value;
		TInt err = RProperty::Get(aConditions[i].iCategory, aConditions[i].iKey, value);
		if(err != KErrNone)
			{
			if(err == KErrNotFound)
				{
				err = KErrArgument; //use KErrArgument error code to signify bad conditions.
				}
			User::Leave(err);	
			}
		}
	//Add the new conditions and notifiers.
	for(i=0;i<count;++i)
		{
		//Create local CPropertyNotifier object
		CPropertyNotifier* notifier = CPropertyNotifier::NewL(*this);
		CleanupStack::PushL(notifier);
		const TTaskSchedulerCondition& condition = aConditions[i];
		notifier->SetPropertyL(condition.iCategory, condition.iKey); 
		//Add condition
		User::LeaveIfError(iConditions.Append(condition));
		//Add notifier
		TInt err = iPropertyNotifiers.Append(notifier);
		if(err != KErrNone)
			{
			iConditions.Remove(iConditions.Count() - 1);//Remove the condition we've just added
			User::Leave(err);
			}
		CleanupStack::Pop(notifier);
		}
	//Check to see that conditions are not already satisfied.
	if(MatchAllConditionsL())
		{
		SetActive(); //we need to set AO active here, otherwise RunL wont be called.	
		CompleteRequest();
		}
	}

void CConditionManager::CompleteRequest()
	{
	TRequestStatus *status = &iStatus;
	User::RequestComplete(status, KErrNone); // now compete request so RunL is triggered
	}
	
//Respond to a condition changing.
//Called from CPropertyNotifier::RunL().
void CConditionManager::VariableChangedL(const TUid& aCategory, TUint aKey)
	{
	//We have been notified that the value of one of the variables has been changed.
	//It is not enough to check that the variable's value satisfies its condition!
	//We have to check that all CConditionManager::iPropertyNotifiers satisfy their conditions.
	//----------------
	//If this is a variable, which is a part of the variables, monitored by the 
	//current CConditionManager object, only then do check variables values against
	//requested conditions	
	if(HasVariable(aCategory, aKey))
		{
		if(MatchAllConditionsL())
			{
			SetActive(); //we need to set AO active here, otherwise RunL wont be called.	
			CompleteRequest();
			}
		}
	}
	
void CConditionManager::RunL()
	{
	// cancel outstanding notification requests by destroying AO's
	iPropertyNotifiers.ResetAndDestroy();
	iManager.DueSchedule(iScheduleHandle);		
	}

void CConditionManager::DoCancel()
	{
	CompleteRequest();  		
	}

//The method returns ETrue, if all monitored variables (aConditions array) 
//satisfy their conditions, EFalse otherwise.
TBool CConditionManager::MatchAllConditionsL() const
	{
	TInt satisfiedConditionsCnt = 0;
	TInt count = iConditions.Count();
	for(TInt i=0;i<count;++i)
		{
		const TTaskSchedulerCondition& condition = iConditions[i];
		TInt value;
		// errors here typically indicate that the P&S variables is not of 
		// integer type (ie its changed) or its been deleted
		User::LeaveIfError(RProperty::Get(condition.iCategory, condition.iKey, value));
		if(::DoMatchCondition(condition.iType, value, condition.iState))
			{
			++satisfiedConditionsCnt;
			}
		}
	return satisfiedConditionsCnt == count;
	}
	
//This method checks if the variable, identified by (aCategory, aKey) pair, is a part
//of CConditionManager::iPropertyNotifiers array and returns ETrue, if that's true.
//EFalse otherwise.
TBool CConditionManager::HasVariable(const TUid& aCategory, TUint aKey) const
	{
	for(TInt i=iConditions.Count()-1;i>-1;--i)
		{
		if(iConditions[i].iCategory == aCategory && iConditions[i].iKey == aKey)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TInt CConditionManager::Offset()
	{
	return (_FOFF(CConditionManager, iLink));
	}

TInt CConditionManager::Id()
	{
	return iScheduleHandle;
	}

//

CPropertyNotifier* CPropertyNotifier::NewL(CConditionManager& aManager)
	{
	CPropertyNotifier* self = new(ELeave) CPropertyNotifier(aManager);
	return self;
	}

CPropertyNotifier::CPropertyNotifier(CConditionManager& aManager)
:	CActive(EPriorityStandard),
	iConditionManager(aManager)
	{
	CActiveScheduler::Add(this);
	}

CPropertyNotifier::~CPropertyNotifier()
	{
	Cancel();
	iProperty.Close();
	}

void CPropertyNotifier::AttachL()
	{
	User::LeaveIfError(iProperty.Attach(iCategory, iKey));
	iProperty.Subscribe(iStatus);
	SetActive();
	}

void CPropertyNotifier::SetPropertyL(const TUid& aCategory, TUint aKey)
	{
	if (IsActive())
	    {
		Cancel();
		iProperty.Close();
	    }
	iCategory = aCategory;
	iKey = aKey;
	AttachL();	
	}

//	Respond to a condition changing
void CPropertyNotifier::RunL()
	{
	if (iStatus.Int() >= KErrNone)
		{
		iConditionManager.VariableChangedL(iCategory, iKey);
		AttachL();
		}
	// if status is KErrNotFound then P&S variable has been deleted!  By 
	// resubscribing we wait for it to be created.  If it never gets
	// created then TRequestStatus never completes so this condition 
	// never gets met and iConditionManager.VariableChanged which 
	// makes sense.
	else if (iStatus.Int() == KErrNotFound)
		AttachL();			
	// If status is another error we have a problem!!! Whatever the case 
	// we should just ignore this condition from now on by doing nothing.
	}

void CPropertyNotifier::DoCancel()
	{
	iProperty.Cancel();
	}
	
TInt CPropertyNotifier::RunError(TInt aError)
    {
    if (aError)
        {
        LOGSTRING("CPropertyNotifier::RunL() leaves.");
        User::Panic(KTaskSchedulerPanic, aError);
        }
    return KErrNone;
    }
	
//

CScheduleCriteriaManager* CScheduleCriteriaManager::NewL(CTaskScheduler& aOwner)
	{
	CScheduleCriteriaManager* self = new(ELeave) CScheduleCriteriaManager(aOwner);
	return self;
	}

CScheduleCriteriaManager::~CScheduleCriteriaManager()
	{
	Cancel();
	RemoveTimers();
	RemoveConditions();
	}

CScheduleCriteriaManager::CScheduleCriteriaManager(CTaskScheduler& aOwner)
:	CActive(EPriorityStandard+2), //make priority higher than condition AO
	iTaskScheduler(aOwner),
	iTimers(CScheduleTimer::Offset()),
	iConditions(CConditionManager::Offset())	
	{
	CActiveScheduler::Add(this);
	}

void CScheduleCriteriaManager::CompleteRequest()
	{
	TRequestStatus *status = &iStatus;
	User::RequestComplete(status, KErrNone); // now compete request so RunL is triggered	
	}
	
void CScheduleCriteriaManager::DueSchedule(TInt aScheduleHandle)
	{
	iDueScheduleHandle = aScheduleHandle;
	SetActive(); // need to set AO active so RunL will subsequently be called.
	CompleteRequest();
	}

void CScheduleCriteriaManager::RunL()
	{
	// remove schedule and then notify task scheduler manager	
	RemoveSchedule(iDueScheduleHandle);
	iTaskScheduler.DueTaskNotifyL(iDueScheduleHandle);
	}

void CScheduleCriteriaManager::DoCancel()
	{
	CompleteRequest();
	}
	
// If schedule timer for this ID doesnt exist then create and add new timer.  If schedule 
// timer does exist then just amend existing timer.
//When one of the schedule entries in this schedule has become due,
//this function will be called with aNotFirstTime = ETrue
//If this function is called because of environment changes then aSchChange   = EOnlyTime and only update time based schedule
void CScheduleCriteriaManager::ReplaceScheduleL(CSchedule& aSchedule, TSchChangeType aSchChange , TBool aNotFirstTime)
	{
	aSchedule.CalculateDueTime(aNotFirstTime);

	TInt  scheduleId 		=  	aSchedule.Id();
	const TTsTime nextTime 	= 	aSchedule.DueTime();
	ReplaceScheduleL(nextTime,scheduleId);
	
	//If this function is called because of environment changes then
	//leave conditions unchanged
	if(aSchChange == EOnlyTime)
		return;
	CConditionManager* condition = FindCondition(scheduleId);
	// no point in doing work for 
	if(aSchedule.Conditions().Count() > 0)
		{
		if(!condition)
			{
			condition = CConditionManager::NewL(scheduleId, *this);
			iConditions.AddLast(*condition);
			}
		condition->ReplaceL(aSchedule.Conditions());
		}
	else if(condition)
		RemoveCondition(condition);		
	}
	
// If schedule timer for this ID doesnt exist then create and add new timer.  If schedule 
// timer does exist then just amend existing timer.
void CScheduleCriteriaManager::ReplaceScheduleL(const TTsTime& aNextTime, 
								TInt aSchedule)
	{
	CScheduleTimer* timer = Find(aSchedule);
	// if time is set to MaxTTime then we don't want to set a timer
	// off as it will complete straight away.
	if((aNextTime.GetUtcTime() != Time::MaxTTime())
	&& 	(aNextTime.GetLocalTime() != Time::MaxTTime()))
		{
		if(!timer)
			{	
			timer = CScheduleTimer::NewL(aSchedule, *this);
			iTimers.AddLast(*timer);
			}
		timer->SetNext(aNextTime);
		}
	else if(timer)	
		{
		RemoveTimer(timer); // make sure we remove the old one!
		}
	}

void CScheduleCriteriaManager::RemoveSchedule(TInt aSchedule)
	{
	CScheduleTimer* timer = Find(aSchedule);
	if(timer)
		RemoveTimer(timer); // remove timer also terminates AO

	CConditionManager* condition = FindCondition(aSchedule);
	if(condition)
		RemoveCondition(condition); // remove condition also terminates AO
	
	}

//Timer methods	
void CScheduleCriteriaManager::RemoveTimers()
	{
	CScheduleTimer* timer;
	TSglQueIter<CScheduleTimer> timerIter(iTimers);
    timerIter.SetToFirst();
    while ((timer = timerIter++) != NULL)
		{
		RemoveTimer(timer);
		}
	}

void CScheduleCriteriaManager::RemoveTimer(CScheduleTimer* aTimer)
	{
	iTimers.Remove(*aTimer);
	delete aTimer;
	}
	
CScheduleTimer* CScheduleCriteriaManager::Find(TInt aSchedule)
	{
	CScheduleTimer* timer = NULL;
	TSglQueIter<CScheduleTimer> timerIter(iTimers);
    timerIter.SetToFirst();
    while ((timer = timerIter++) != NULL)
		{
		if	(timer->Id() == aSchedule)
			break;
		}	
	return timer;
	}	

// condition methods
void CScheduleCriteriaManager::RemoveConditions()
	{
	CConditionManager* condition;
	TSglQueIter<CConditionManager> conditionIter(iConditions);
    conditionIter.SetToFirst();
    while ((condition = conditionIter++) != NULL)
		{
		RemoveCondition(condition);
		}
	}

void CScheduleCriteriaManager::RemoveCondition(CConditionManager* aCondition)
	{
	iConditions.Remove(*aCondition);
	delete aCondition;
	}
	
CConditionManager* CScheduleCriteriaManager::FindCondition(TInt aSchedule)
	{
	CConditionManager* condition = NULL;
	TSglQueIter<CConditionManager> conditionIter(iConditions);
    conditionIter.SetToFirst();
    while ((condition = conditionIter++) != NULL)
		{
		if	(condition->Id() == aSchedule)
			break;
		}	
	return condition;
	}	
	
TInt CScheduleCriteriaManager::RunError(TInt aError)
    {
    if (aError)
        {
        LOGSTRING("CScheduleCriteriaManager::RunL() leaves.");
        User::Panic(KTaskSchedulerPanic, aError);
        }    
    return KErrNone;
    }


