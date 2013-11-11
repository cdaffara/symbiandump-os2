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

// User includes
#include "SSCH_STD.H"
#include "CSCHCODE.H"
#include "SCHCLI.H"
#include "SchLogger.h"
#include "SCHMAN.H"
#include "SCHEDULE.H"
#include <schtask.h>

// Constants
const TInt KSchedulerArrayGranularity = 1; 
_LIT(KSchedulerPanic, "Task Scheduler");

void PanicClient(TInt aPanic, const RMessage2& aMessage)
//
// Toast the client
//
	{
	//Note that panic also completes the message so don't complete anywhere else
	aMessage.Panic(KSchedulerPanic,aPanic);
	}
	
CSchSession::CSchSession(CTaskScheduler& aScheduler)
//
// C'Tor - must pass client to CSession
//
	:	CSession2(), 
		iTaskScheduler(&aScheduler), 
		iClient(NULL)
	{
	}

CSchSession::~CSchSession()
	{
	if (iClient)
		{
		iClient->DecUsers();
		if (!iClient->Users())
			{
			iClient->Remove();
			delete iClient;
			iClient = NULL;
			}
		}
	if(!iPendingNotification.IsNull())
		iPendingNotification.Complete(KErrCancel);	
	}
	
void CSchSession::ServiceError(const RMessage2& aMessage,TInt aError)
//
// Handle an error or leave from CMySession::ServiceL()
// A bad descriptor error implies a badly programmed client, so panic it;
// otherwise use the default handling (report the error to the client)
//
	{
	if (aError==KErrBadDescriptor)
		PanicClient(EPanicBadDescriptor,aMessage);
	CSession2::ServiceError(aMessage,aError);
	}

void CSchSession::ServiceL(const RMessage2& aMessage)
//
//	Handle messages for this session.
//	all API functions are synchronous so complete here too
//
	{
	
	iClientMessage = BSUL::CClientMessage::NewL(aMessage);
	
	//Validate the message
	TRAPD(error, iClientMessage->ValidateL());
	
	if(error == KErrNone)
		{
		TRAP(error, DoServiceL());
		}
	
   	// don't complete message if we have paniced client (ie message is NULL)
   	iClientMessage->CompleteRequestL(error);

   	delete iClientMessage;
   	iClientMessage = NULL;   	
	}

void CSchSession::DoServiceL()
//
//	Handle messages for this session.
//	all API functions are synchronous so complete here too
//
	{
	switch (iClientMessage->Function())
		{
#if defined (_DEBUG)
	case ESchDbgMarkHeap:
		__UHEAP_MARK;
		break;
	case ESchDbgCheckHeap:
		__UHEAP_CHECK(iClientMessage->GetIntL(0));
		break;
	case ESchDbgMarkEnd:
		{
		TInt count = iClientMessage->GetIntL(0);
		
		//Three allocations are made for this message so add 3 to count
		//CClientMessage::NewL
		//CIntParameter::NewL
		//RPointerArray::AppendL	
		__UHEAP_MARKENDC(count + 3);
		}
		break;
	case ESchDbgFailNext:
		__UHEAP_FAILNEXT(iClientMessage->GetIntL(0));
		break;
	case ESchDbgResetHeap:
		__UHEAP_RESET;
		break;
	case ESchFaultServer:
		{
		// make sure we complete the message first before Killing this server
		iClientMessage->CompleteRequestL(KErrNone);
		RProcess().Kill(0);
		return;
		}
#endif
    case ERegisterClient:
		RegisterClientL();
    	break;
	case ECreateTimeSchedule:
		CreateTimeScheduleL();
    	break;
	case ECreateConditionSchedule:
		CreateConditionScheduleL();
    	break;
	case EScheduleTask:
		ScheduleTaskL();
    	break;
	case EDeleteTask:
		DeleteTaskL();
		break;
	case EDeleteSchedule:
		{
		CSchedule* schedule = iTaskScheduler->FindL(iClientMessage->GetIntL(0));
		CheckPersistsInBackupL(*schedule);
		//check that user has permission to delete this schedule.
		schedule->CheckAccessAllowedL(iClientMessage->Message());
		iTaskScheduler->RemoveScheduleL(iClientMessage->GetIntL(0));
    	break;
		}
	case EDisableSchedule:
		{
		CSchedule* schedule = iTaskScheduler->FindL(iClientMessage->GetIntL(0));
		CheckPersistsInBackupL(*schedule);
		//check that user has permission to disable this schedule.
		schedule->CheckAccessAllowedL(iClientMessage->Message());
		iTaskScheduler->DisableScheduleL(iClientMessage->GetIntL(0));
    	break;
		}
	case EEnableSchedule:
		{
		CSchedule* schedule = iTaskScheduler->FindL(iClientMessage->GetIntL(0));
		CheckPersistsInBackupL(*schedule);
		//check that user has permission to enable this schedule.
		schedule->CheckAccessAllowedL(iClientMessage->Message());
		iTaskScheduler->EnableScheduleL(iClientMessage->GetIntL(0));
    	break;
		}
	case EEditTimeSchedule:
		EditTimeScheduleL();
    	break;
	case EEditConditionSchedule:
		EditConditionScheduleL();
    	break;
	case EGetScheduleRefs:
		GetScheduleRefsL();
		break;
	case ECountSchedules:
		CountSchedulesL();
		break;
	case EGetScheduleInfo:
		GetScheduleInfoL();
		break;
	case EGetTimeScheduleData:
		GetTimeScheduleDataL();
		break;
	case EGetConditionScheduleData:
		GetConditionScheduleDataL();
		break;
	case EGetTaskData:
		GetTaskDataL();
		break;
	case EGetTaskRefs:
		GetTaskRefsL();
    	break;	
	case ECountTasks:
		CountTasksL();
		break;
	case EGetTask:
		GetTaskInfoL();
		break;
	case EGetSchedulerItemRefAndNextDueTime:
		GetScheduleItemRefAndDueTimeL();
		break;
	case EGetTaskDataSize:
		GetTaskDataSizeL();
		break;
	case EGetScheduleType:
		GetScheduleTypeL();
		break;
	default:
		User::Leave(EPanicIllegalFunction);
		break;
		
        }//switch 

	}

//private functions
void CSchSession::RegisterClientL()
	{
	TFileName writeBuf;

	iClientMessage->ReadL(0,writeBuf);

	// should return existing client if there is one
	CClientProxy* client = iTaskScheduler->AddClientL(writeBuf, iClientMessage->GetIntL(1));
	client->IncUsers();
	if	(iClient)
		{
		iClient->DecUsers();
		if	(!iClient->Users())
			{
			LOGSTRING2("CSchSession::RegisterClientL - removing existing client %S", &iClient->ExecutorFileName());
			iClient->Remove();
			delete iClient;
			}
		}
	iClient = client;
	}

void CSchSession::CreateTimeScheduleL()
	{
	// get settings
	TScheduleSettings2 settings;
	TPScheduleSettings pS(settings);
	iClientMessage->ReadL(0, pS);
	
	//WriteDeviceData needed to create persistent schedules
	if(settings.iPersists)
		{
		// backup or restore in progress, so don't allow functions that might write to store
		if (iTaskScheduler->BUROperationInProgress() != EBUROperationNoActivity)
			{
			User::Leave(KErrServerBusy);
			}

		CheckCapabilityL();
		}	

	//Verify that the length of the condition list passed in is as expected
	if(iClientMessage->GetDesLengthL(1) != 
				(settings.iEntryCount * sizeof(TScheduleEntryInfo2)))
		{
		User::Leave(KErrBadDescriptor);
		}

	// get entries
	CArrayFixFlat<TScheduleEntryInfo2>* entries 
		= new(ELeave) CArrayFixFlat<TScheduleEntryInfo2>(settings.iEntryCount);
	CleanupStack::PushL(entries);
	entries->ResizeL(settings.iEntryCount);
	TPtr8 entriesPtr(REINTERPRET_CAST(TUint8*,&(entries->At(0))), settings.iEntryCount * sizeof(TScheduleEntryInfo2));
	iClientMessage->ReadL(1, entriesPtr);

	// create schedule
	const TInt handle = iTaskScheduler->GenerateId();
	if	(handle == KErrOverflow)
		User::Leave(KErrOverflow);

	// write back ID
	TPInt id(handle);
	iClientMessage->WriteL(2,id);

	// get security info
	TSecurityInfo securityInfo(iClientMessage->Message());	

	// Create schedule and add to scheduler
	CSchedule* newschedule = CSchedule::NewLC(handle, 
										settings.iName, 
										settings.iPersists, 
										*entries,
										securityInfo);
	iTaskScheduler->AddScheduleL(*newschedule);
	CleanupStack::Pop(newschedule);
	CleanupStack::PopAndDestroy(entries);
	}

void CSchSession::CreateConditionScheduleL()
	{
	// get settings
	TScheduleSettings2 settings;
	TPScheduleSettings pS(settings);
	iClientMessage->ReadL(0, pS);

	//WriteDeviceData needed to create persistent schedules
	if(settings.iPersists)
		{
		// backup or restore in progress, so don't allow functions that might write to store
		if (iTaskScheduler->BUROperationInProgress() != EBUROperationNoActivity)
			{
			User::Leave(KErrServerBusy);
			}

		CheckCapabilityL();
		}

	//Verify that the length of the condition list passed in is as expected
	if(iClientMessage->GetDesLengthL(1) != 
				(settings.iEntryCount * sizeof(TTaskSchedulerCondition)))
		{
		User::Leave(KErrBadDescriptor);
		}

	// get entries
	CArrayFixFlat<TTaskSchedulerCondition>* entries 
		= new(ELeave) CArrayFixFlat<TTaskSchedulerCondition>(settings.iEntryCount);
	CleanupStack::PushL(entries);
	entries->ResizeL(settings.iEntryCount);
	TPtr8 entriesPtr(REINTERPRET_CAST(TUint8*,&(entries->At(0))), settings.iEntryCount * sizeof(TTaskSchedulerCondition));
	iClientMessage->ReadL(1, entriesPtr);
	
	// get time
	TTsTime defaultRunTime;
	TPckg<TTsTime> pTime(defaultRunTime);
	iClientMessage->ReadL(2, pTime);

	// create schedule
	const TInt handle = iTaskScheduler->GenerateId();
	if	(handle == KErrOverflow)
		User::Leave(KErrOverflow);

	// write back ID
	TPInt id(handle);
	iClientMessage->WriteL(3,id);
	
	// get security info
	TSecurityInfo securityInfo(iClientMessage->Message());	

	// Create schedule and add to scheduler
	CSchedule* newschedule 
		= CSchedule::NewLC(handle, 
							settings.iName, 
							settings.iPersists, 
							*entries,
							defaultRunTime,
							securityInfo);
	iTaskScheduler->AddScheduleL(*newschedule);
	CleanupStack::Pop(newschedule);
	CleanupStack::PopAndDestroy(entries);
	}

void CSchSession::EditTimeScheduleL()
	{
	CSchedule* schedule = iTaskScheduler->FindL(iClientMessage->GetIntL(1));
	
	CheckPersistsInBackupL(*schedule);
	//check that user has permission to edit this schedule.
	schedule->CheckAccessAllowedL(iClientMessage->Message());
	
	//check that this is a time schedule!
	if(schedule->Type() != ETimeSchedule)
		User::Leave(KErrArgument);
		
	// Get entry count
	const TInt count = iClientMessage->GetIntL(0);
	
	//Verify that the length of the condition list passed in is as expected
	if(iClientMessage->GetDesLengthL(2) != (count * sizeof(TScheduleEntryInfo2)))
		{
		User::Leave(KErrBadDescriptor);
		}

	// Get new entry list
	CArrayFixFlat<TScheduleEntryInfo2>* entries 
		= new(ELeave) CArrayFixFlat<TScheduleEntryInfo2> (KSchedulerArrayGranularity);	
	CleanupStack::PushL(entries);
	entries->ResizeL(count);
	TPtr8 entriesPtr(REINTERPRET_CAST(TUint8*, &(entries->At(0))), count*sizeof(TScheduleEntryInfo2));

	iClientMessage->ReadL(2, entriesPtr);

	// Give it to scheduler along with schedule id
	iTaskScheduler->EditScheduleL(iClientMessage->GetIntL(1), *entries);
	CleanupStack::PopAndDestroy(entries);
	}

void CSchSession::EditConditionScheduleL()
	{
	CSchedule* schedule = iTaskScheduler->FindL(iClientMessage->GetIntL(1));
	
	CheckPersistsInBackupL(*schedule);
	//check that user has permission to edit this schedule.
	schedule->CheckAccessAllowedL(iClientMessage->Message());
	
	//check that this is a condition schedule!
	if(schedule->Type() != EConditionSchedule)
		User::Leave(KErrArgument);

	// Get entry count
	const TInt count = iClientMessage->GetIntL(0);
	
	//Verify that the length of the condition list passed in is as expected
	if(iClientMessage->GetDesLengthL(2) != (count * sizeof(TTaskSchedulerCondition)))
		{
		User::Leave(KErrBadDescriptor);
		}

	// Get new entry list
	CArrayFixFlat<TTaskSchedulerCondition>* entries 
		= new(ELeave) CArrayFixFlat<TTaskSchedulerCondition> (KSchedulerArrayGranularity);	
	CleanupStack::PushL(entries);
	entries->ResizeL(count);
	TPtr8 entriesPtr(REINTERPRET_CAST(TUint8*, &(entries->At(0))), count*sizeof(TTaskSchedulerCondition));
	iClientMessage->ReadL(2, entriesPtr);

	// get time
	TTsTime defaultRunTime;
	TPckg<TTsTime> pTime(defaultRunTime);
	iClientMessage->ReadL(3, pTime);

	// Give it to scheduler along with schedule id
	iTaskScheduler->EditScheduleL(iClientMessage->GetIntL(1), *entries, defaultRunTime);
	CleanupStack::PopAndDestroy(entries);
	}

void CSchSession::ScheduleTaskL()
	{
	// If client is registered, it is allowed to schedule tasks
	if(iClient != NULL)
		{
		TTaskInfo taskInfo;
		TPTaskInfo pI(taskInfo);
		iClientMessage->ReadL(0, pI);

		CSchedule* schedule = iTaskScheduler->FindL(iClientMessage->GetIntL(1));
	
		CheckPersistsInBackupL(*schedule);

		//check that user has permission to schedule a task using this schedule.
		schedule->CheckAccessAllowedL(iClientMessage->Message());
	
		taskInfo.iTaskId = schedule->GenerateTaskId();

		if (taskInfo.iTaskId == KErrOverflow)
			{
			User::Leave(KErrOverflow);
			}
		// panic client if this is a condition schedule and client is asking for 
		// repeats.  This doesnt make sense.	
		if (schedule->Type() == EConditionSchedule && taskInfo.iRepeat != 0)
			User::Leave(KErrArgument); 

		// Create a new scheduled task with associated task data
		TInt len = iClientMessage->GetDesLengthL(3);
		HBufC* taskdata = HBufC::NewLC(len);

		TPtr pData(taskdata->Des());
		iClientMessage->ReadL(3, pData);	
	
		CScheduledTask* newTask = new(ELeave) CScheduledTask(taskInfo, 
											taskdata, 
											schedule->Type(), 
											schedule->SecurityInfo());
		CleanupStack::Pop(taskdata); //taskdata now owned by newTask
		CleanupStack::PushL(newTask);
		// Have to store the associated task, so that in the case where the
		// tasks need to be restored from a backup, we can ensure that the
		// task is associated with the correct schedule.
		newTask->SetScheduleId(schedule->Id());

		//If the schedule that the task is associated with is persistent then set the flag
		if(schedule->Persists())
			{
			newTask->SetPersists();
			}
		LOGSTRING("CSchSession::ScheduleTaskL - writing back task id to client");
		TPInt id(taskInfo.iTaskId);
		iClientMessage->WriteL(2,id);
	
		TScheduledTask* task = new(ELeave) TScheduledTask(*newTask,*iClient);
	
		// Now that task has been created sucessfully, we can add newTask to Client
		iClient->AddTask(*newTask); // task info is owned by client: schedule just keeps a reference
		CleanupStack::Pop(newTask);

		schedule->AddTask(*task);	//add new task - its now owned by schedule

		LOGSTRING("CSchSession::ScheduleTaskL - scheduling task with task scheduler");
		// If this leaves we need to still remove task from client and schedule. 
		// No easy way to do this via CleanupStack
		TRAPD(err, iTaskScheduler->ScheduleTaskL(*schedule, *iClient));
		if(err)
			{
			schedule->RemoveTask(task);
			iClient->RemoveTask(newTask);
			User::Leave(err);	
			}
		}
	else
		//Client hasnt been registered so it isn't allowed to schedule task, panic client
		{
		iClientMessage->PanicClient(KSchedulerPanic,EPanicNotRegistered);
		}
	}

void CSchSession::DeleteTaskL()
	{
	const TInt taskId = iClientMessage->GetIntL(0);
	
	const TInt scheduleId = (taskId/KScheduleIdDifferential)*KScheduleIdDifferential;

	//check that user has permission to delete a task using this schedule.
	CSchedule* schedule = iTaskScheduler->FindL(scheduleId);
	CheckPersistsInBackupL(*schedule);
	schedule->CheckAccessAllowedL(iClientMessage->Message());
	
	iTaskScheduler->DeleteTaskL(scheduleId, taskId);
	}

//retrieval functions
void CSchSession::CountSchedulesL()
	{
	TScheduleFilter filter = STATIC_CAST(TScheduleFilter, iClientMessage->GetIntL(1));
	//GetScheduleRefs filters on clients schedule permssion as well.
	TInt count = iTaskScheduler->GetScheduleRefsL(NULL,filter, iClientMessage->Message());
	TPInt pCount(count);
	iClientMessage->WriteL(0,pCount);
	}

void CSchSession::GetScheduleRefsL()
	{
	//client calls CountSchedules() first to get expected count. still 
	//need to check it's valid though...
	
	const TInt expectedCount = iClientMessage->GetIntL(0);
	TScheduleFilter filter = STATIC_CAST(TScheduleFilter, iClientMessage->GetIntL(1));

	CArrayFixFlat<TSchedulerItemRef>* refArray 
		= new(ELeave) CArrayFixFlat<TSchedulerItemRef>(KSchedulerArrayGranularity);
	CleanupStack::PushL(refArray);
	
	//GetScheduleRefs filters on clients schedule permssion as well.
	TInt count = iTaskScheduler->GetScheduleRefsL(refArray, filter, iClientMessage->Message());
	if	((expectedCount<count) || (count == 0))
		User::Leave(KErrArgument);

	TPtrC8 pS(REINTERPRET_CAST(TUint8*, &refArray->At(0)),sizeof(TSchedulerItemRef)*expectedCount);
	iClientMessage->WriteL(2, pS);
	CleanupStack::PopAndDestroy();//info array
	}

void CSchSession::GetTimeScheduleDataL()
	{
	//client input:handle at info at aMessage.Int0()
	//info at aMessage.Ptr1()
	//info contains: exepcted entry count
	//scheduler output: entries at aM.Ptr2()
	TInt handle = iClientMessage->GetIntL(0);

	TScheduleInfo info;
	TPScheduleInfo pS(info);
	iClientMessage->ReadL(1, pS);

	CSchedule* schedule = iTaskScheduler->FindL(handle);

	//check that user has permission to get this schedules info.
	schedule->CheckAccessAllowedL(iClientMessage->Message());
	
	if (schedule->Type() != ETimeSchedule)
		User::Leave(KErrArgument); //calling wrong API

	CArrayFixFlat<TScheduleEntryInfo2>* entries 
		= new(ELeave) CArrayFixFlat<TScheduleEntryInfo2> (KSchedulerArrayGranularity);
	CleanupStack::PushL(entries);
	schedule->EntriesL(*entries);
	if ((entries->Count())!=info.iEntryCount)
		User::Leave(KErrArgument);
	//write entries
	TPtrC8 pE(REINTERPRET_CAST(TUint8*, &entries->At(0)),sizeof(TScheduleEntryInfo2)*info.iEntryCount);
	iClientMessage->WriteL(2, pE);	

	CleanupStack::PopAndDestroy(entries);//entry array
	}

void CSchSession::GetConditionScheduleDataL()
	{
	//client input:handle at info at aMessage.Int0()
	//info at aMessage.Ptr1()
	//info contains: exepcted entry count
	//scheduler output: entries at aM.Ptr2()
	TInt handle = iClientMessage->GetIntL(0);

	TScheduleInfo info;
	TPScheduleInfo pS(info);
	iClientMessage->ReadL(1, pS);

	CSchedule* schedule = iTaskScheduler->FindL(handle);
	
	//check that user has permission to get this schedules info.
	schedule->CheckAccessAllowedL(iClientMessage->Message());
	
	if (schedule->Type() != EConditionSchedule)
		User::Leave(KErrArgument); //calling wrong API

	CArrayFixFlat<TTaskSchedulerCondition>* conditions 
		= new(ELeave) CArrayFixFlat<TTaskSchedulerCondition> (KSchedulerArrayGranularity);
	CleanupStack::PushL(conditions);
	schedule->ConditionsL(*conditions);
	if ((conditions->Count())!=info.iEntryCount)
		User::Leave(KErrArgument);
	//write entries
	TPtrC8 pE(REINTERPRET_CAST(TUint8*, &conditions->At(0)),sizeof(TTaskSchedulerCondition)*info.iEntryCount);
	iClientMessage->WriteL(2, pE);	

	//write time
	TTsTime defaultTime(schedule->DefaultRunTimeL());
	TPBTime pDefaultTime(defaultTime);
	iClientMessage->WriteL(3, pDefaultTime);

	CleanupStack::PopAndDestroy(conditions);//condition array
	}

void CSchSession::GetScheduleTypeL()
	{
	TScheduleType type;
	TInt handle = iClientMessage->GetIntL(0);
	CSchedule* schedule = iTaskScheduler->FindL(handle);
	
	//check that user has permission to get this schedules info.
	schedule->CheckAccessAllowedL(iClientMessage->Message());
	
	type = schedule->Type();	
	TPInt pType(type);
	iClientMessage->WriteL(1,pType);
	}
	
void CSchSession::GetTaskDataL()
	{//client input:handle at info at aMessage.Int0()
	//info at aMessage.Ptr1()
	//info contains: exepcted entry count, expected task count
	//scheduler output: tasks at aM.Ptr2()
	TInt handle = iClientMessage->GetIntL(0);

	TScheduleInfo info;
	TPScheduleInfo pS(info);
	iClientMessage->ReadL(1, pS);

	CSchedule* schedule = iTaskScheduler->FindL(handle);

	//check that user has permission to get this task info (based on owning
	// schedules info).
	schedule->CheckAccessAllowedL(iClientMessage->Message());
	
	CArrayFixFlat<TTaskInfo>* tasks 
		= new(ELeave) CArrayFixFlat<TTaskInfo> (KSchedulerArrayGranularity);
	CleanupStack::PushL(tasks);
	schedule->TasksL(*tasks);
	if ((tasks->Count())!=info.iTaskCount)
		User::Leave(KErrArgument);
	//write tasks if there are any
	if (info.iTaskCount>0)
		{
		TPtrC8 pT(REINTERPRET_CAST(TUint8*, &tasks->At(0)),sizeof(TTaskInfo)*info.iTaskCount);
		iClientMessage->WriteL(2, pT);	
		}
	CleanupStack::PopAndDestroy(tasks);//task array
	}

void CSchSession::GetScheduleInfoL()
	{
	CSchedule* schedule = iTaskScheduler->FindL(iClientMessage->GetIntL(0));
	
	//check that user has permission to get this schedules info.
	schedule->CheckAccessAllowedL(iClientMessage->Message());
	
	TScheduleInfo info;
	if (schedule->Type() == ETimeSchedule)
		schedule->GetInfo(info, EFalse);
	else
		schedule->GetInfo(info, ETrue);
		
	TPBScheduleInfo pInfo(info);
	iClientMessage->WriteL(1, pInfo);
	TTsTime dueTime(schedule->DueTime());
	TPBTime pDueTime(dueTime);
	iClientMessage->WriteL(2, pDueTime);
	}

void CSchSession::GetTaskRefsL()
	{
	//taskrefs for all, 
	//or just the ones I scheduled, 
	//or just the pending ones, 
	//or just the pending ones I scheduled
	
	TInt expectedCount = iClientMessage->GetIntL(0);
	TScheduleFilter sFilter = STATIC_CAST(TScheduleFilter, iClientMessage->GetIntL(1));
	TTaskFilter tFilter = STATIC_CAST(TTaskFilter, iClientMessage->GetIntL(2));
	CArrayFixFlat<TSchedulerItemRef>* refs 
		= new (ELeave) CArrayFixFlat<TSchedulerItemRef> (KSchedulerArrayGranularity);
	CleanupStack::PushL(refs);
	
	//GetTaskRefs filters on clients schedule permssion as well.
	TInt actualCount = iTaskScheduler->GetTaskRefsL(refs, sFilter, tFilter,iClient, iClientMessage->Message());
	
	// Must find enough tasks.
	if((actualCount != expectedCount) || (refs->Count() == 0))
		User::Leave(KErrArgument);
	
	// Write array back to client
	TPtrC8 pR(REINTERPRET_CAST(TUint8*,&refs->At(0)),sizeof(TSchedulerItemRef)*expectedCount);
	iClientMessage->WriteL(3, pR);
	CleanupStack::PopAndDestroy();//refs
	}

void CSchSession::CountTasksL()
	{
	//count either all of 'em or just mine
	
	TScheduleFilter sFilter = STATIC_CAST(TScheduleFilter, iClientMessage->GetIntL(1));
	TTaskFilter tFilter = STATIC_CAST(TTaskFilter, iClientMessage->GetIntL(2));

	//GetTaskRefs filters on clients schedule permssion as well.
	TInt count = iTaskScheduler->GetTaskRefsL(NULL, sFilter, tFilter,iClient, iClientMessage->Message());

	TPInt pCount(count);
	iClientMessage->WriteL(0,pCount);
	}

void CSchSession::GetTaskInfoL()
	{
	// Get task
	TInt taskId				= iClientMessage->GetIntL(0);
	TInt scheduleId			= (taskId/KScheduleIdDifferential)*KScheduleIdDifferential;
	CSchedule* schedule		= iTaskScheduler->FindL(scheduleId);
	
	//check that user has permission to get this task info (based on owning 
	//schedules info).
	schedule->CheckAccessAllowedL(iClientMessage->Message());
	
	TScheduledTask* task = schedule->Task(taskId);
	if	(!task)
		User::Leave(KErrNotFound);

	TPBTaskInfo pI(task->Info());

	iClientMessage->WriteL(1, pI);//write info	
	
	// If there isn't enough room to hold the task data in the client buffer then
	// indicate by returning KErrArgument
	TInt clientBufferSize = iClientMessage->GetIntL(2);
	if	(clientBufferSize < task->Data().Length())
		User::Leave(KErrArgument);
	iClientMessage->WriteL(3, task->Data());
	}

void CSchSession::GetScheduleItemRefAndDueTimeL()
	{
	// Get CSchedule object
	TInt taskId = iClientMessage->GetIntL(0);
	TInt scheduleId = (taskId/KScheduleIdDifferential)*KScheduleIdDifferential;
	CSchedule* schedule = iTaskScheduler->FindL(scheduleId);

	//check that user has permission to get this schedules info.
	schedule->CheckAccessAllowedL(iClientMessage->Message());
	
	TSchedulerItemRef ref;
	ref.iHandle	= schedule->Id();
	ref.iName	= schedule->Name();

	TPBSchedulerItemRef pRef(ref);
	iClientMessage->WriteL(1, pRef);
	
	TTsTime dueTime(schedule->DueTime());
	TPBTime pDueTime(dueTime);
	iClientMessage->WriteL(2, pDueTime);
	}

void CSchSession::GetTaskDataSizeL()
	{
	TInt taskId = iClientMessage->GetIntL(0);
	TInt scheduleId = (taskId/KScheduleIdDifferential)*KScheduleIdDifferential;
	CSchedule* schedule = iTaskScheduler->FindL(scheduleId);

	//check that user has permission to get this task info (based on owning 
	//schedules info).
	schedule->CheckAccessAllowedL(iClientMessage->Message());
	
	TScheduledTask* task = schedule->Task(taskId);
	if	(!task)
		User::Leave(KErrNotFound);

	TInt size = task->Data().Length();
	TPInt pSize(size);
	iClientMessage->WriteL(1,pSize);
	}

void  CSchSession::CheckCapabilityL()
	{
	if( !( iClientMessage->Message().HasCapability(ECapabilityWriteDeviceData)
		|| (PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement)==0 ))) // Enforcement off
		User::Leave(KErrPermissionDenied);	
	}
	
void CSchSession::CheckPersistsInBackupL(const CSchedule& aSchedule)
	{
	// backup or restore in progress, so don't allow functions that might write to store	
	if ((iTaskScheduler->BUROperationInProgress() != EBUROperationNoActivity) && (aSchedule.Persists()))
		{
		User::Leave(KErrServerBusy);
		}
	}

