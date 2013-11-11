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

// System includes
#include <bacntf.h>

// User includes
#include "SCHMAN.H"
#include "SchTimer.h"
#include "SchLogger.h"
#include "SCHEDULE.H"
#include "SCHCLI.H"
#include "SCHSTORE.H"
#include <schtask.h>
#include "SCHLOG.h"

// Constants
const TInt KMinScheduleId = 0;
const TInt KMaxSchedules = 25000;

//Command line argument  
_LIT(KCommandLine, "SYSSTARTSCHEXE");

//SID of SysStart
const TInt KSysStartSID = 0x10205C44; 


#define UNUSED_VAR(a) a = a

//
// Construction/Destruction functions
//

CTaskScheduler::CTaskScheduler()
:	iSchedules(CSchedule::Offset()), 
	iClients(CClientProxy::Offset()),
	iStartupStatePassNonCritical(EFalse)
	{
	}

CTaskScheduler::~CTaskScheduler()
	{
	if (iBackupNotification)
		{
		iBackupNotification->DeRegisterBackupOperationObserver(*this);
		}
	delete iBackupNotification;
	delete iBackupManager;
	delete iNotifier;
	delete iScheduleCriteriaManager;
	delete iSchLogManager;
	
	//remove clients and schedules as well!!
	TDblQueIter<CClientProxy> clientIter(iClients);
	
	clientIter.SetToFirst();
	CClientProxy* client=NULL;
	while ((client=clientIter++)!=NULL)
		{
			client->Remove();
			delete client;
		}
	
	TSglQueIter<CSchedule> schedIter(iSchedules);
	
	schedIter.SetToFirst();
	CSchedule* schedule=NULL;
	while ((schedule=schedIter++)!=NULL)
		{
			iSchedules.Remove(*schedule);
			delete schedule;
		}	
	}

void CTaskScheduler::ConstructL()
	{
	LOGSTRING("CTaskScheduler::ConstructL - Creating new schedule server log entry");

	User::LeaveIfError(iFsSession.Connect());
	iBackupManager = new(ELeave) CSchBackupManager(iFsSession);
	iBackupManager->ConstructL();

	iNotifier = CEnvironmentChangeNotifier::NewL(CActive::EPriorityHigh, TCallBack(EnvironmentChanged, this));
	iNotifier->Start();

	iScheduleCriteriaManager = CScheduleCriteriaManager::NewL(*this);

	iSchLogManager = CSchLogManager::NewL(iFsSession);

	LOGSTRING("CTaskScheduler::ConstructL - Restoring clients and schedules");
	TRAPD(err, iBackupManager->RestoreL(iClients, iSchedules,*iSchLogManager));
	if	(err != KErrNone) // the file's corrupt or something...
		{
		LOGSTRING2("CTaskScheduler::ConstructL - had to create new store because of error: %d", err);
		iBackupManager->CreateEmptyBackupL();
		}
		
	//checking the SID of the process which started the Task scheduler	
 	if (User::CreatorSecureId() == KSysStartSID)
	 	{
	    TInt argLen = User::CommandLineLength();
     	if (argLen)
	         {
	         HBufC* arg = HBufC::NewLC(argLen);
	         TPtr argPtr = arg->Des();
	         User::CommandLine(argPtr);
	         argPtr.UpperCase();
	         
	 		 //Checking Comman dLine arg passed to it is same as in SSCForStartupMode0.rss
	 		 //and checking for persisted schedules
	         if((argPtr.Compare(KCommandLine) == 0) && iSchedules.IsEmpty())
	             {
	             //if no schedule leave
				 User::Leave(KErrNone);
	             }
	 	      CleanupStack::PopAndDestroy(arg);
	 	      }
         }

	// Each client now contains a list of associated tasks. We need
	// to now associate those tasks with specific schedules
	CClientProxy* client;
	TDblQueIter<CClientProxy> clientIter(iClients);
	clientIter.SetToFirst();
	while ((client = clientIter++) != NULL)
		{
		// Fetch an iterator for each task owned by this client
		CScheduledTask* task;
		TDblQueIter<CScheduledTask> taskIterator = client->TaskIterator();
		taskIterator.SetToFirst();

		// Iterate through all the tasks owned by this client, trying to find
		// the corresponding schedules.
		while ((task = taskIterator++) != NULL)
			{
			CSchedule* schedule = NULL;
			schedule = Find(task->ScheduleId());
			if (schedule)
				{
				TScheduledTask* taskRef = new(ELeave) TScheduledTask(*task,*client);
				schedule->AddTask(*taskRef);
				}
			}
		}
	iBackupNotification = CBaBackupSessionWrapper::NewL();
	iBackupNotification->RegisterBackupOperationObserverL(*this);
	}

CTaskScheduler* CTaskScheduler::NewL()
	{
	CTaskScheduler* self = CTaskScheduler::NewLC();
	CleanupStack::Pop();
	return self;	
	}

CTaskScheduler* CTaskScheduler::NewLC()
	{
	CTaskScheduler* self = new(ELeave) CTaskScheduler();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

//
// Client, Schedule and Task functions
//

CClientProxy* CTaskScheduler::AddClientL(const TDesC& aFilename, TInt aPriority)
	{
	//check we don't already have a client that will do...
	TDblQueIter<CClientProxy> clientIter(iClients);
	clientIter.SetToFirst();
	CClientProxy* client=NULL;
	while ((client=clientIter++)!=NULL)
		{
		if	(client->IsEqual(aFilename, aPriority))
			return client;
		}
	client = CClientProxy::NewL(iFsSession, aFilename, aPriority,*iSchLogManager); 
	iClients.Add(*client);
	return client;
	}

void CTaskScheduler::AddScheduleL(CSchedule& aSchedule)
	{
	LOGSTRING3("CTaskScheduler::AddScheduleL - schedule: %S, %d", &aSchedule.Name(), aSchedule.Id());
	iBackupManager->PerformStoreOperationL(CSchBackupManager::ESchBackupOperationAdd, aSchedule);
	iSchedules.AddLast(aSchedule);
	LOGSTRING("CTaskScheduler::AddScheduleL - schedule added");
	}

void CTaskScheduler::EditScheduleL(TInt aScheduleHandle, CArrayFixFlat<TScheduleEntryInfo2>& aEntryList)
	{
	CSchedule* schedule = FindL(aScheduleHandle);

	// remove schedule from condition manager before replacing entries to ensure 
	// its deleted properly.
	iScheduleCriteriaManager->RemoveSchedule(schedule->Id());
	schedule->ReplaceEntriesL(aEntryList);

	TRAPD(err, iBackupManager->PerformStoreOperationL(CSchBackupManager::ESchBackupOperationEdit, *schedule));
	if(err)
		{
		schedule->RemoveEntries();
		User::Leave(err);
		}
	// recalculate due time only if schedule is enabled and has tasks to run	
	if (IsScheduleReadyForUpdate(*schedule))
		{
		iScheduleCriteriaManager->ReplaceScheduleL(*schedule);
		}
	}

void CTaskScheduler::DoEditScheduleL(CSchedule& aSchedule, 
							CArrayFixFlat<TTaskSchedulerCondition>& aConditionList,
							const TTsTime& aDefaultTime)
	{
	aSchedule.ReplaceConditionsL(aConditionList);
	
	//Default Time is represented by a single entry class
	CArrayFixFlat<TScheduleEntryInfo2>* entries 
		= new(ELeave) CArrayFixFlat<TScheduleEntryInfo2>(1);
	CleanupStack::PushL(entries);
	TScheduleEntryInfo2 info;
	info.SetStartTime(aDefaultTime);
	info.SetInterval(1);
	info.SetIntervalType(EDaily);
	//validityperiod of 24 hours will ensure task is always run
	info.SetValidityPeriod(60*24); 
	entries->AppendL(info);
	aSchedule.ReplaceEntriesL(*entries);
	CleanupStack::Pop(entries);
	
	iBackupManager->PerformStoreOperationL(CSchBackupManager::ESchBackupOperationEdit, aSchedule);
	}

void CTaskScheduler::EditScheduleL(TInt aScheduleHandle, 
							CArrayFixFlat<TTaskSchedulerCondition>& aConditionList,
							const TTsTime& aDefaultTime)
	{
	CSchedule* schedule = FindL(aScheduleHandle);

	// remove schedule from condition manager before replacing entries to ensure 
	// its deleted properly.
	iScheduleCriteriaManager->RemoveSchedule(schedule->Id());

	TRAPD(err, DoEditScheduleL(*schedule, aConditionList, aDefaultTime));
	if(err)
		{
		schedule->RemoveEntries();
		schedule->RemoveConditions();
		User::Leave(err);
		}

	// recalculate due time only if schedule is enabled and has tasks to run	
	if (IsScheduleReadyForUpdate(*schedule))
		{
		iScheduleCriteriaManager->ReplaceScheduleL(*schedule);
		}
	}	

void CTaskScheduler::RemoveScheduleL(TInt aHandle)
	{
	CSchedule* schedule = FindL(aHandle);
	LOGSTRING3("CTaskScheduler::RemoveScheduleL - schedule: %S, %d", &schedule->Name(), schedule->Id());
	if (!schedule->HasTasks())
		{
		LOGSTRING("CTaskScheduler::RemoveScheduleL - schedule doesn't have any tasks, removing");
		//remove schedule from timer
		iScheduleCriteriaManager->RemoveSchedule(schedule->Id());
		DoRemoveL(schedule);
		}
	else
		{
		// Can't delete a schedule which has tasks
		LOGSTRING("CTaskScheduler::RemoveScheduleL - schedule has tasks, can't delete");
		User::Leave(KErrArgument);
		}
	}

void CTaskScheduler::DisableScheduleL(TInt aHandle)
	{
	CSchedule* schedule = FindL(aHandle);
	schedule->SetEnabled(EFalse);
	iBackupManager->PerformStoreOperationL(CSchBackupManager::ESchBackupOperationEdit, *schedule);
	//remove schedule from timer as its disabled
	iScheduleCriteriaManager->RemoveSchedule(schedule->Id());
	}

void CTaskScheduler::EnableScheduleL(TInt aHandle)
	{
	CSchedule* schedule = FindL(aHandle);
	schedule->SetEnabled(ETrue);
	iBackupManager->PerformStoreOperationL(CSchBackupManager::ESchBackupOperationEdit, *schedule);

	// recalculate due time only if schedule has tasks to run	
	if ( IsScheduleReadyForUpdate(*schedule))
		{
		iScheduleCriteriaManager->ReplaceScheduleL(*schedule);
		}
	}

void CTaskScheduler::ScheduleTaskL(CSchedule& aSchedule, CClientProxy& aClient)
	{
	// Backup the task
	if	(aSchedule.Persists())
		{
		iBackupManager->PerformStoreOperationL(CSchBackupManager::ESchBackupOperationEdit, aClient);
		}
	// if schedule is enabled then add schedule to timer.
	if (aSchedule.Enabled() && IsStartupStateNonCritical())
		{
		iScheduleCriteriaManager->ReplaceScheduleL(aSchedule);
		}		
	}

void CTaskScheduler::DeleteTaskL(TInt aScheduleHandle, TInt aTaskHandle)
	{
	CSchedule* schedule = FindL(aScheduleHandle);

	TScheduledTask* task = schedule->Task(aTaskHandle);
	if (!task)
		{
		LOGSTRING("CTaskScheduler::DeleteTaskL - task wasn't found");
		User::Leave(KErrNotFound);
		}

	const CClientProxy& clientForTask = task->Client();
	
	// This deletes the task and removes the CScheduledTask
	// from the CClientProxy's queue of tasks
	task->RemoveInfo();

	// This deletes the TScheduledTask defined above and removes it
	// from CSchedule's queue of TScheduledTask's.
	schedule->RemoveTask(task);
	if (!schedule->HasTasks())	//i.e. it was the last task 
		{
		LOGSTRING("CTaskScheduler::DeleteTaskL - schedule doesn't have any more tasks left");
		//remove scheule from timer as there are no more tasks
		iScheduleCriteriaManager->RemoveSchedule(schedule->Id());
		// If the schedule isn't persistent then we delete it (transient schedules only
		// have one task).
		if (!schedule->Persists())
			DoRemoveL(schedule);
		else
			iBackupManager->PerformStoreOperationL(CSchBackupManager::ESchBackupOperationEdit, clientForTask);		
		}
	else
		{
		// Backup the changes to the tasks. Although we are deleting a task, we are not actually
		// deleting the client, so this is actually an edit operation.
		if (schedule->Persists())
			iBackupManager->PerformStoreOperationL(CSchBackupManager::ESchBackupOperationEdit, clientForTask);
		}
	}

//
// Utility Functions
//

TInt CTaskScheduler::GenerateId()
	{
	TInt id = KMinScheduleId;
	CSchedule* schedule = Find(id);
	while (schedule!=NULL) 
		{
		id+=KScheduleIdDifferential;//=10 000
		if ((id/KScheduleIdDifferential) > KMaxSchedules)
			return KErrOverflow;
		schedule = Find(id);
		}
	return id;
	}

void CTaskScheduler::DoRemoveL(CSchedule* aSchedule)
	{
	TRAPD(err, iBackupManager->PerformStoreOperationL(CSchBackupManager::ESchBackupOperationDelete, *aSchedule));
	if	(err < KErrNone && err != KErrNotFound)
		User::Leave(err);
	iSchedules.Remove(*aSchedule);
	delete aSchedule;
	aSchedule = NULL;
	}

CSchedule* CTaskScheduler::FindL(TInt aHandle)
	{
	CSchedule* schedule = Find(aHandle);
	if (!schedule)
		User::Leave(KErrNotFound);
	return schedule;
	}

CSchedule* CTaskScheduler::Find(TInt aHandle)
	{
	TSglQueIter<CSchedule> scheduleIter(iSchedules);
	scheduleIter.SetToFirst();
	CSchedule* schedule;
	while ((schedule = scheduleIter++)!=NULL)
		{
		if (schedule->Id() == aHandle)
			return schedule;
		}
	return NULL;
	}

// If aRefArray is NULL then only count it returned.
TInt CTaskScheduler::GetScheduleRefsL(CArrayFixFlat<TSchedulerItemRef>* aRefArray, 
									TScheduleFilter aFilter,
									const RMessagePtr2& aMessage)
	{
	TInt count = 0;
	TSglQueIter<CSchedule> iter(iSchedules);
	iter.SetToFirst();
	CSchedule* schedule = NULL;
	while ((schedule = iter++) != NULL)
		{
		if(aFilter == EAllSchedules || (schedule->Enabled() && schedule->HasTasks()))
			{
			//only add information for schedules that the client has permission to alter
			if(schedule->IsAccessAllowed(aMessage))
				{
				if(aRefArray)
					{
					TSchedulerItemRef ref;
					ref.iHandle = schedule->Id();
					ref.iName = schedule->Name();
					aRefArray->AppendL(ref);	
					}
				count++;		
				}
			}
		}
	return count;				
	}

// If aRefArray is NULL then only count it returned.
TInt CTaskScheduler::GetTaskRefsL(CArrayFixFlat<TSchedulerItemRef>* aRefArray, 
									TScheduleFilter aScheduleFilter,
									TTaskFilter aTaskFilter,
									CClientProxy* aClient,
									const RMessagePtr2& aMessage)
	{
	TInt count = 0;
	TSglQueIter<CSchedule> iter(iSchedules);
	iter.SetToFirst();
	CSchedule* schedule = NULL;
	while ((schedule = iter++) != NULL)
		{
		if(aScheduleFilter == EAllSchedules || (schedule->Enabled() && schedule->HasTasks()))
			{
			//only add information for schedules that the client has permission to alter
			if(schedule->IsAccessAllowed(aMessage))
				{
				TSglQueIter<TScheduledTask> taskIter(*(schedule->Tasks()));
				taskIter.SetToFirst();
				TScheduledTask* task;
				while ((task=taskIter++)!=NULL)
					{
					if (aTaskFilter==EAllTasks||&task->Client() == aClient) // This pointer comparison is a bit rubbish.  Change?
						{
						if(aRefArray)
							{
							TTaskInfo info = task->Info();
							TSchedulerItemRef ref;
							ref.iHandle = info.iTaskId;
							ref.iName = info.iName;
							aRefArray->AppendL(ref);
							}
						count++;
						}
					}
				}
			}
		}
	return count;				
	}
	

//
// Schedule Execution functions
//

// A schedule is ready to be run
void CTaskScheduler::DueTaskNotifyL(TInt aScheduleHandle)
	{
	CSchedule* schedule = FindL(aScheduleHandle);
	//NotifyTasks() also removes tasks from the schedule if there are no 
	//repeats left.
	schedule->NotifyTasks(); 

	if (!schedule->HasTasks())
		{
		// remove schedule.
		TRAPD(ignore, DoRemoveL(schedule));
		//??error only occurs in relation to persistence!! Do something.  
        UNUSED_VAR(ignore);
		}
	else
		{
		__ASSERT_ALWAYS(IsStartupStateNonCritical(), User::Invariant());
		iScheduleCriteriaManager->ReplaceScheduleL(*schedule,EConditionAndTime,ETrue);
		}

	// Execute all clients.  This method doesn't leave as all errors are either
	// logged in the log engine or handled elsewhere.
	ExecuteClients();
	}

//	Go through all clients, executing their tasks
void CTaskScheduler::ExecuteClients(TBool aUpdateClient)
	{
	if ((BUROperationInProgress() == EBUROperationNoActivity) || !aUpdateClient)
		{
		TDblQueIter<CClientProxy> clientIter(iClients);
		clientIter.SetToFirst();
		CClientProxy* client;
		while( (client = clientIter++) != NULL)
			{
			// Does this client have anything ready to run?
			if	(client->IsReadyToExecute())
				{
				client->ExecuteTasks();
				// Clears the 'IsReadyToExecute' flag...
				client->RemoveDueTasks();
				}
			}
		if (aUpdateClient)	
			{
			// Update the store file now
			UpdateClients();	
			}
		}
	else
		{
		// Sets the flag to trigger delayed store operation when BUR ends
		iTaskExecutedDuringBUR = ETrue;
		}
	}

//	Go through all clients, update the store file with modified client info
void CTaskScheduler::UpdateClients()
	{
	// iterate the client list to perform delayed update of the store file
	TDblQueIter<CClientProxy> clientIter(iClients);
	clientIter.SetToFirst();
	CClientProxy* client;
	while( (client = clientIter++) != NULL)
		{
		if	(!client->Users())
			{
			// Remove from store
			TRAPD(ignore, iBackupManager->PerformStoreOperationL(CSchBackupManager::ESchBackupOperationDelete, *client));
			//?? if ignore is not KErrNone then there is a problem with the store
            UNUSED_VAR(ignore);
			// Remove client & delete it
			client->Remove();
			delete client;
			}
		else
			{
			// Update this clients data in the store...
			TRAPD(ignore, iBackupManager->PerformStoreOperationL(CSchBackupManager::ESchBackupOperationEdit, *client));

			//?? if ignore is not KErrNone then there is a problem with the store
            UNUSED_VAR(ignore);
			}
		}
	}
	
//
// Environment change functions
//

TInt CTaskScheduler::EnvironmentChanged(TAny* aScheduler)
	{
	CTaskScheduler* self = reinterpret_cast<CTaskScheduler*>(aScheduler);
	self->HandleEnvironmentChange();
	return KErrNone;
	}

void CTaskScheduler::HandleEnvironmentChange()
	{
	// If staged startup still in critical region, can safely
	// ignore system time change.
	if (!IsStartupStateNonCritical())
		{
		return;
		}

	TInt changes=iNotifier->Change();
	if	(changes & EChangesSystemTime)
		{
#ifdef __SCHLOGGING__
		{
		TTime time;
		time.HomeTime();
		TDateTime due(time.DateTime());
		LOGSTRING7("CTaskScheduler::HandleEnvironmentChangeL - system time is now: [%02d/%02d/%d] @ %02d:%02d:%02d", due.Day(), (TInt) due.Month() + 1, due.Year(), due.Hour(), due.Minute(), due.Second());
		}
#endif


		// Cannot use AddSchedulesToTimerL() because this method
		// uses the non-condition version of ReplaceScheduleL.
		TSglQueIter<CSchedule> scheduleIter(iSchedules);
		scheduleIter.SetToFirst();
		CSchedule* schedule = NULL;
		while ((schedule=scheduleIter++)!=NULL)
			{
			if(IsScheduleReadyForUpdate(*schedule))
				{
				TRAPD(err, iScheduleCriteriaManager->ReplaceScheduleL(*schedule, EOnlyTime));
                UNUSED_VAR(err);
				}
			}
		}
	}

void CTaskScheduler::AddSchedulesToTimerL()
	{
	if (!IsStartupStateNonCritical())
		{
		return; // not ready
		}

	TInt ret = KErrNone;

	TSglQueIter<CSchedule> scheduleIter(iSchedules);
	scheduleIter.SetToFirst();
	CSchedule* schedule = NULL;
	while ((schedule=scheduleIter++)!=NULL)
		{
		if(IsScheduleReadyForUpdate(*schedule))
			{
			TRAPD(err, iScheduleCriteriaManager->ReplaceScheduleL(*schedule));
			if (err != KErrNone)
				{
				ret = err;
				}
			}
		}
	User::LeaveIfError(ret);
	}
	
void CTaskScheduler::CleanupScheduledTasksL()
	{
	RFs fs;
	_LIT(KTempFilePath, "_:\\private\\10005399\\*.tmp");
	TBuf<32> filePath(KTempFilePath);

	filePath[0] = RFs::GetSystemDriveChar();	
	
	fs.Connect();
	CleanupClosePushL(fs);
	
	CFileMan* fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan);

	//Delete all temporary files in the private folder	
	fileMan->Delete(filePath,0);
	
	//Pop and destroy fs and fileMan  
	//This will call fs.Close() so no need to call it explicitly
	CleanupStack::PopAndDestroy(2); 
	}

/**
CSchStartupStateMgr calls this to notify startup state changes of
interest.

@internalComponent
*/
void CTaskScheduler::ProcessSSAEventL(TStartupStateIdentifier aKnownState)
	{
	LOGSTRING2("ProcessSSAEventL receive SS 0x%x", aKnownState);

	if (! IsStartupStateNonCritical() &&
		(aKnownState >= KSchFinalStartupState))
		{
		iStartupStatePassNonCritical = ETrue;
		CleanupScheduledTasksL();
		AddSchedulesToTimerL();
		}
	}

/**
Returns ETrue if Start-up State is NonCritical
*/
TBool CTaskScheduler::IsStartupStateNonCritical()
	{
	return iStartupStatePassNonCritical;
	}

/**
Check schedule is valid
*/
TBool CTaskScheduler::IsScheduleReadyForUpdate(CSchedule& aSchedule)
	{
	if(aSchedule.IsUpdatable() && IsStartupStateNonCritical())
		return 	ETrue;		
	else
		return EFalse;
	}

/**
babackup server calls this to notify backup operations. The attributes are read and translated to determine
which operation is actually in progress.  
@internalComponent
*/
void CTaskScheduler::HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes)
	{
	TBUROperation type;

	// determine the operation type (backup or restore)
	switch(aBackupOperationAttributes.iFileFlag)	
		{
		case MBackupObserver::EReleaseLockReadOnly:
			type = EBUROperationBackup;
			break;
		case MBackupObserver::EReleaseLockNoAccess:
			type = EBUROperationRestore;
			break;
		case MBackupObserver::ETakeLock:
			// No information is passed from babackup server, so we need to depend on our own memory
			type = iBUROperationInProgress;
			break;
		default:
			type = EBUROperationNoActivity;
			break;
		}
		
	// determine the operation status (e.g. starting, ending)
	switch(aBackupOperationAttributes.iOperation)
		{
		case EStart:
			BURBeginningL(type);
			break;
		case EEnd:
			BURCompleteL(type, EBUROperationSuccess);
			break;
		case EAbort:
			BURCompleteL(type, EBUROperationAbort);
			break;
		default:
			break;
		}
	}


/**
This function is called to notify when a Backup or Restore operation is commencing.

@internalComponent
*/
void CTaskScheduler::BURBeginningL(TBUROperation aOperationType)
{
	// This will stop the API calls that directly modify the store file
	iBUROperationInProgress	= aOperationType;
	
	//cancel background compaction of store during backup/restore	
	iBackupManager->Cancel();
}


/**
This function is called to notify when a Backup or Restore operation is finished.

@internalComponent
*/
void CTaskScheduler::BURCompleteL(TBUROperation aOperationType, TBUROperationResult aBURResult)
{
	// If there is a successful restore, this means that we have a different store file then we were using 
	// so we have to internalize and use that file. In any other case, we can proceed with the delayed 
	// updates to the old file
	if ((aOperationType == EBUROperationRestore)&&(aBURResult == EBUROperationSuccess))
		{
		LOGSTRING("CTaskScheduler::BURCompleteL - Restoring clients and schedules after successful restore");

		//First check whether any task expires during the restore process now that this is completed
		if (iTaskExecutedDuringBUR)	
			{
			// performed the delayed task execution but with no externalizing as we dont want to modify
			// the just restored file as after the delayed execution, all persistent schedule will be removed
			ExecuteClients(EFalse);
			iTaskExecutedDuringBUR=EFalse;			
			}
		
		// Now remove existing persistent schedules, tasks and clients
		TSglQueIter<CSchedule> scheduleIter(iSchedules);
		scheduleIter.SetToFirst();
		CSchedule* schedule;
		while ((schedule = scheduleIter++)!=NULL)
			{
			if (schedule->Persists())
				{
				iSchedules.Remove(*schedule);
				schedule->RemoveTasks(ETrue);
				delete schedule; 
				}
			}

		CClientProxy* client;
		TDblQueIter<CClientProxy> clientIter(iClients);
		
		// remove clients which don't have any associated tasks left (tasks in persistent schedules are
		// already removed, but the client might have transient schedules as well)
		clientIter.SetToFirst();
		while ((client = clientIter++) != NULL)
			{
			TDblQueIter<CScheduledTask> taskIter = client->TaskIterator();
			taskIter.SetToFirst();
			
			// remove client if no more tasks
			if (taskIter++ == NULL)
				{
				client->Remove();
				delete client; // removes associated tasks
				}
			else
				{
				//remove any persisted task and if the client only has persisted task, remove client as well
				taskIter.SetToFirst();
				CScheduledTask* task;
				TInt taskCount=0;
				TInt persistCount=0;
				while ((task = taskIter++) != NULL)
					{
					taskCount++;
					if (task->Persists())
						{
						persistCount++;
						client->RemoveTask(task);
						}
					}
				//if after removing the persist tasks, there are no more other tasks, we can remove the client too						
				if (taskCount==persistCount)
					{
					client->Remove();
					delete client;
					}					
				}
			}
		
		// now re-read the clients and schedules from the restored store file
		TRAPD(err, iBackupManager->RestoreL(iClients, iSchedules,*iSchLogManager,ETrue));

		if	(err != KErrNone) // the file's corrupt or something...
			{
			LOGSTRING2("CTaskScheduler::BURCompleteL - had to create new store because of error: %d", err);
			iBackupManager->CreateEmptyBackupL();
			}

			
		// Each client now contains a list of associated tasks. We need
		// to now associate those tasks with specific schedules
		clientIter.SetToFirst();
		while ((client = clientIter++) != NULL)
			{
			// Fetch an iterator for each task owned by this client
			CScheduledTask* task;
			TDblQueIter<CScheduledTask> taskIterator = client->TaskIterator();
			taskIterator.SetToFirst();

			// Iterate through all the tasks owned by this client, trying to find
			// the corresponding schedules.
			while ((task = taskIterator++) != NULL)
				{
				TSglQueIter<CSchedule> persScheduleIter(iSchedules);
				persScheduleIter.SetToFirst();
				CSchedule* persSchedule;
				while ((persSchedule = persScheduleIter++)!=NULL)
					{
					if ((persSchedule->Persists())&&(persSchedule->Id() == task->ScheduleId()))
						{
						TScheduledTask* taskRef = new(ELeave) TScheduledTask(*task,*client);
						persSchedule->AddTask(*taskRef);
						}
					}
				}
			}
		
		// Activate the scheduler with the new schedules
		AddSchedulesToTimerL();	
		}
	else
		{
		if (iTaskExecutedDuringBUR)	
			{
			iBUROperationInProgress	= EBUROperationNoActivity;		
			iTaskExecutedDuringBUR = EFalse;
			// performed the delayed task execution
			ExecuteClients();
			}	
		}	
	
	// BUR operation is completed
	iBUROperationInProgress	= EBUROperationNoActivity;		
	iTaskExecutedDuringBUR = EFalse;
}
