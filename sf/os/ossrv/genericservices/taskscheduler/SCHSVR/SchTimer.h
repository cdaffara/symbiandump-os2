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

#ifndef __SCHTIMER_H__
#define __SCHTIMER_H__

// System includes
#include <e32base.h>

// Classes referenced
class TTsTime;
class CTaskScheduler;
class CScheduleTimer;
class CConditionManager;
class TTaskSchedulerCondition;
class CSchedule;

/**
Defines the types of schedules changes (condition and/or a time)
@internalComponent
*/
enum TSchChangeType
	{
	//Update both condition and a time based schedule
	EConditionAndTime,
	
	//Update only time based schedule
	EOnlyTime,	
	};

// class CScheduleCriteriaManager
// This class manages and determines when a schedule is due, based on 
// either a set of condition and/or a time.  The conditions and time
// are set and overridden via the ReplaceScheduleL method.
NONSHARABLE_CLASS(CScheduleCriteriaManager) : public CActive
	{
public:
	static CScheduleCriteriaManager* NewL(CTaskScheduler& aOwner);
	~CScheduleCriteriaManager();

public:	
	void ReplaceScheduleL(CSchedule& aSchedule, TSchChangeType aSchChange = EConditionAndTime, TBool aNotFirstTime = EFalse);
	void ReplaceScheduleL(const TTsTime& aNextTime, 
						TInt aSchedule);
	void RemoveSchedule(TInt aSchedule);
	void DueSchedule(TInt aScheduleHandle);

private:
	CScheduleCriteriaManager(CTaskScheduler& aOwner);
	// From CActive
	void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
	
private:
	void CompleteRequest();
	// Timer handling
	void RemoveTimer(CScheduleTimer* aTimer);		
	void RemoveTimers();
	CScheduleTimer* Find(TInt aSchedule);		
	// Condtion handling
	void RemoveCondition(CConditionManager* aTimer);		
	void RemoveConditions();
	CConditionManager* FindCondition(TInt aSchedule);		

private:
	CTaskScheduler& iTaskScheduler;
	TSglQue<CScheduleTimer> iTimers;
	TSglQue<CConditionManager> iConditions;
	TInt iDueScheduleHandle;
	};

#endif
