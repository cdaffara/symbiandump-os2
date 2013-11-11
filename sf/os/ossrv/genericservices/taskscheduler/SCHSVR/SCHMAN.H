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
//

#ifndef __SCHMAN_H__
#define __SCHMAN_H__

// System includes
#include <f32file.h>

// User includes
#include "SCHINFO.H"
#include "SchSSAObserver.h"

#include <babackup.h>

// Classes referenced
class CSchedule;
class TScheduledTask;
class CScheduleCriteriaManager;
class CClientProxy;
class TScheduleEntryInfo2;
class CSchBackupManager;
class CEnvironmentChangeNotifier;
class CSchLogManager;
class TTaskSchedulerCondition;

enum TBUROperation
	{
	EBUROperationNoActivity = 0,
	EBUROperationBackup,
	EBUROperationRestore
	};

enum TBUROperationResult
	{
	EBUROperationAbort = 0,
	EBUROperationSuccess
	};

NONSHARABLE_CLASS(CTaskScheduler) : public CBase, public MSchStartupStateObserver, private MBackupOperationObserver
	{
public:
	static CTaskScheduler* NewL();
	static CTaskScheduler* NewLC();
	~CTaskScheduler();

public: 
	// Client, Schedule and Task functions
	CClientProxy* AddClientL(const TDesC& aFilename, TInt aPriority);
	void AddScheduleL(CSchedule& aSchedule);
	void RemoveScheduleL(TInt aHandle);
	void EditScheduleL(TInt aScheduleHandle, 
					CArrayFixFlat<TScheduleEntryInfo2>& aEntryList);
	void EditScheduleL(TInt aScheduleHandle, 
					CArrayFixFlat<TTaskSchedulerCondition>& aEntryList,
					const TTsTime& aDefaultTime);
	void EnableScheduleL(TInt aHandle);
	void DisableScheduleL(TInt aHandle);
	void ScheduleTaskL(CSchedule& aSchedule,
					CClientProxy& aClient);
	void DeleteTaskL(TInt aScheduleHandle, TInt aTaskHandle);

	TInt GetScheduleRefsL(CArrayFixFlat<TSchedulerItemRef>* aRefArray, 
									TScheduleFilter aFilter,
									const RMessagePtr2& aMessage);
	TInt GetTaskRefsL(CArrayFixFlat<TSchedulerItemRef>* aRefArray, 
									TScheduleFilter aScheduleFilter,
									TTaskFilter aTaskFilter,
									CClientProxy* aClient,
									const RMessagePtr2& aMessage);
	// Utility functions
	CSchedule* FindL(TInt aHandle);
	TInt GenerateId();
	void DueTaskNotifyL(TInt aScheduleHandle);	//called when some task is due

	// from MSchStartupStateObserver
	void ProcessSSAEventL(TStartupStateIdentifier aKnownState);
	TBool IsScheduleReadyForUpdate(CSchedule& aSchedule);
	TBool IsStartupStateNonCritical();

	inline TBUROperation BUROperationInProgress() { return iBUROperationInProgress; }

	// from MBackupOperationObserver
	virtual void HandleBackupOperationEventL(const TBackupOperationAttributes &aBackupOperationAttributes);	

private: 
	// Construction functions
	CTaskScheduler();
	void ConstructL();
	
	//Environment change functions
	static TInt EnvironmentChanged(TAny* aScheduler);
	void HandleEnvironmentChange();
	
	// Utility functions
	CSchedule* Find(TInt aHandle);
	void DoRemoveL(CSchedule* aSchedule);
	void ExecuteClients(TBool aUpdateClient=ETrue);
	void DoEditScheduleL(CSchedule& aSchedule, 
							CArrayFixFlat<TTaskSchedulerCondition>& aConditionList,
							const TTsTime& aDefaultTime);
	void AddSchedulesToTimerL();
	void CleanupScheduledTasksL();
	void UpdateClients();

	void BURBeginningL(TBUROperation aOperationType); 
	void BURCompleteL(TBUROperation aOperationType, TBUROperationResult aBURResult); 

private:
	RFs iFsSession;
	//
	TSglQue<CSchedule> iSchedules;
	TPriQue<CClientProxy> iClients;
	//
	CScheduleCriteriaManager* iScheduleCriteriaManager;
	CSchBackupManager* iBackupManager;
	CEnvironmentChangeNotifier* iNotifier;
	CSchLogManager* iSchLogManager;
	TBool iStartupStatePassNonCritical;

	TBUROperation iBUROperationInProgress;
	TBool iTaskExecutedDuringBUR;
	CBaBackupSessionWrapper* iBackupNotification;
	};

#endif
