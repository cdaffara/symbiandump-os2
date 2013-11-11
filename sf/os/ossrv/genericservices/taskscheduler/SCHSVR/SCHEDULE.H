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
// class TScheduledTask
// -a task which has been scheduled with a schedule
// -maintains non-owning pointer to proxy for client which scheduled it
// class CSchedule
// -has >=1 entries, >=0 tasks
// -when one of its entries is due, all its tasks are due
// 
//

#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

// System includes
#include <e32base.h>

// Forward Declarations
class CFileStore;
class TStreamId;
class TScheduleEntry;
class TScheduleEntryInfo2;
class TScheduleInfo;
class TScheduledTask;
class CScheduledTask;
class CClientProxy;
class TTaskInfo;
class RWriteStream;
class CConditionManager;
class TTaskSchedulerCondition;

//#include <schinfo.h>
#include <schinfointernal.h>
#include "CSCHCODE.H"	//opcodes
/**
Constants
@internalComponent
*/
const TInt KScheduleIdDifferential=10000;

/**
@internalComponent
*/
class TScheduledTask
	{
public:
	TScheduledTask(CScheduledTask& aTask, CClientProxy& iClient);

	// ?? 
	void OnDue(const TTsTime& iValidUntil);

	// Access data
	const HBufC& Data() const;
	const TTaskInfo& Info() const;
	const CClientProxy& Client() const;
	
	// Ask client to remove & delete the TTaskInfo member
	void RemoveInfo();
	void DecRepeat();

	// For queue support
	static TInt Offset();

private:
	CScheduledTask& iTask;//doesn't own this: the client does
	CClientProxy& iClient;//doesn't own this: CTaskScheduler owns all clients
	TSglQueLink iLink;
	};

/**
@internalComponent
*/
NONSHARABLE_CLASS(CSchedule) : public CBase
	{
public:
	static CSchedule* NewLC(TInt aHandle, 
						const TDesC& aName, 
						TBool aPersists,  
						const CArrayFixFlat<TScheduleEntryInfo2>& aEntries,
						const TSecurityInfo& aSecurityInfo);
	static CSchedule* NewLC(TInt aHandle, 
						const TDesC& aName, 
						TBool aPersists,  
						const CArrayFixFlat<TTaskSchedulerCondition>& aEntries,
						const TTsTime& aDefaultRunTime,
						const TSecurityInfo& aSecurityInfo);
	static CSchedule* NewL(CFileStore& aStore, TStreamId& aId);
	~CSchedule();

private: 
	void ConstructL(const TDesC& aName,
					const CArrayFixFlat<TScheduleEntryInfo2>& aEntries);
	void ConstructL(const TDesC& aName,
					const CArrayFixFlat<TTaskSchedulerCondition>& aEntries,
					const TTsTime& aDefaultRunTime);
	void RestoreL(CFileStore& aStore, TStreamId& aId);
	CSchedule();
	CSchedule(const TSecurityInfo& aSecurityInfo, TInt aHandle, TBool aPersists);

private:
	void AddEntriesL(const CArrayFixFlat<TScheduleEntryInfo2>& aEntries);
	TTsTime ValidUntil() const; //from base class
	void AddConditionsL(const CArrayFixFlat<TTaskSchedulerCondition>& aConditions);

public:
	// externalize/internalize
	void InternalizeL(RReadStream& aReadStream);
	void ExternalizeL(RWriteStream& aWriteStream) const;
	//call this whenever schedule becomes due: tells tasks they need to be executed 
	void NotifyTasks();
	// called whenever a schedule changes.  If aNotFirstTime = ETrue then this means
	// the schedule has just run so we need a minor modification to the next due time calculation
	void CalculateDueTime(TBool aNotFirstTime = EFalse);
	//Time entries
	void ReplaceEntriesL(const CArrayFixFlat<TScheduleEntryInfo2>& aEntries);
	void EntriesL(CArrayFixFlat<TScheduleEntryInfo2>& aEntries);
	void GetInfo(TScheduleInfo& aInfo, TBool aCalculateForConditions);
	inline const TTsTime& DueTime() const;
	//Condition entries
	void ReplaceConditionsL(const CArrayFixFlat<TTaskSchedulerCondition>& aConditions);
	void ConditionsL(CArrayFixFlat<TTaskSchedulerCondition>& aConditions);
	const TTsTime& DefaultRunTimeL() const;
	//editing
	void AddTask(TScheduledTask& aTask);
	void RemoveTask(TScheduledTask* aTask);
	void RemoveTasks(TBool aFromClient);
	TInt GenerateTaskId();
	TScheduledTask* Task(const TInt aTaskId);
	//accessors
	void RemoveEntries();
	void RemoveConditions();
	
	TBool ClientInSchedule(const TDesC& aClientName);
	inline TSglQue<TScheduledTask>* Tasks();
	void TasksL(CArrayFixFlat<TTaskInfo>& aTasks);
	inline TInt Id() const;
	inline const TDesC& Name() const;
	inline TBool Persists() const;
	inline TBool Enabled() const;
	inline void SetEnabled(TBool aEnabled);
	inline TBool HasTasks() const;
	inline TScheduleType Type() const;
	const RArray<TTaskSchedulerCondition>& Conditions() const;
	
	//security Info related stuff
	TBool IsAccessAllowed(const RMessagePtr2& aMessage) const;
	inline void CheckAccessAllowedL(const RMessagePtr2& aMessage) const;
	
	const TSecurityInfo& SecurityInfo() const;
	//list capability
	static TInt Offset();
	TBool IsUpdatable();

private://data
	TSglQueLink iLink;			//scheduler keeps all schedules in owning queue
	TInt iId;
	HBufC* iName;
	TBool iPersists;
	TBool iEnabled;
	TTsTime iDueTime;
	TTimeIntervalMinutes iValidityPeriod;
	// list handling
	RArray<TTaskSchedulerCondition> iConditions;
	TSglQue<TScheduledTask> iTaskList;		//things which happen at those times	
	mutable TSglQue<TScheduleEntry> iEntryList;	//list of times at which things happen
	
	//Creating process SecurityInfo
	TSecurityInfo iSecurityInfo;
	};
	
#include "SCHEDULE.INL"
#endif
