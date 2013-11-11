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
#include "SchLogger.h"
#include "SCHEDULE.H"
#include <schtask.h>
#include "SCHCLI.H"
#include "SCHENTRY.H"
#include "SchTimer.h"
#include "SCHEXEC.H"
// Constants
const TInt KMaxTasksPerSchedule = 9999;

//TScheduledTask
TScheduledTask::TScheduledTask(CScheduledTask& aTask, CClientProxy& aClient)
	:	iTask(aTask), 
		iClient(aClient)
	{
	}

void TScheduledTask::OnDue(const TTsTime& aValidUntil)
	{
	LOGSTRING("TScheduledTask::OnDue - start");
	iTask.OnDue(aValidUntil);
	iClient.ReadyToExecute();
	LOGSTRING("TScheduledTask::OnDue - end");
	}

const HBufC& TScheduledTask::Data() const
	{
	return iTask.Data();
	}

const TTaskInfo& TScheduledTask::Info() const
	{
	return iTask.Info();
	}

const CClientProxy& TScheduledTask::Client() const
	{
	return iClient;
	}

void TScheduledTask::RemoveInfo()
	{
	LOGSTRING("TScheduledTask::RemoveInfo - start");
	iClient.RemoveTask(&iTask);
	LOGSTRING("TScheduledTask::RemoveInfo - end");
	}
	
void TScheduledTask::DecRepeat()
	{
	iTask.DecRepeat();
	}

TInt TScheduledTask::Offset()
	{
	return (_FOFF(TScheduledTask, iLink));
	}
/*************************************************************************/
//CSchedule functions
/*************************************************************************/
CSchedule* CSchedule::NewLC(TInt aHandle, 
						const TDesC& aName, 
						TBool aPersists, 
						const CArrayFixFlat<TScheduleEntryInfo2>& aEntries,
						const TSecurityInfo& aSecurityInfo)
	{
	CSchedule* self = new(ELeave) CSchedule(aSecurityInfo, aHandle, aPersists);
	CleanupStack::PushL(self);
	self->ConstructL(aName, aEntries);
	return self;
	}

CSchedule* CSchedule::NewLC(TInt aHandle, 
						const TDesC& aName, 
						TBool aPersists, 
						const CArrayFixFlat<TTaskSchedulerCondition>& aEntries,
						const TTsTime& aDefaultRunTime,
						const TSecurityInfo& aSecurityInfo)
	{
	CSchedule* self = new(ELeave) CSchedule(aSecurityInfo, aHandle, aPersists);
	CleanupStack::PushL(self);
	self->ConstructL(aName, aEntries,aDefaultRunTime);
	return self;
	}
	
CSchedule* CSchedule::NewL(CFileStore& aStore, TStreamId& aStreamId)
	{
	CSchedule* self = new(ELeave) CSchedule;
	CleanupStack::PushL(self);
	self->RestoreL(aStore, aStreamId);//get self from root
	CleanupStack::Pop();//self
	return self;
	}

CSchedule::CSchedule(const TSecurityInfo& aSecurityInfo, TInt aHandle, TBool aPersists)
:	iId(aHandle), 
	iPersists(aPersists), 
	iEnabled(ETrue),
	iTaskList(TScheduledTask::Offset()),
	iEntryList(TScheduleEntry::Offset()),
	iSecurityInfo(aSecurityInfo)
	{
	}

CSchedule::CSchedule()
:	iTaskList(TScheduledTask::Offset()),
	iEntryList(TScheduleEntry::Offset())
	{
	}

CSchedule::~CSchedule()
	{
	LOGSTRING("CSchedule::~CSchedule - start");
		
	delete iName;
	RemoveTasks(EFalse);
	RemoveEntries();
	RemoveConditions();

	LOGSTRING("CSchedule::~CSchedule - end");
	}

void CSchedule::ConstructL(const TDesC& aName,
						const CArrayFixFlat<TScheduleEntryInfo2>& aEntries)
	{
	iName = aName.AllocL();
	AddEntriesL(aEntries);
	}
	
void CSchedule::ConstructL(const TDesC& aName,
					const CArrayFixFlat<TTaskSchedulerCondition>& aEntries,
					const TTsTime& aDefaultRunTime)
	{
	iName = aName.AllocL();
	AddConditionsL(aEntries);
	
	// we plug the default time in as the start time of a schedule entry
	TScheduleEntryInfo2 info(aDefaultRunTime, EDaily, 1, 60*24);
	TScheduleEntry* entry = ScheduleEntryFactory::CreateL(info);
	iEntryList.AddLast(*entry);		
	}	
	
void CSchedule::RestoreL(CFileStore& aStore, TStreamId& aId)
	{
	RStoreReadStream scheduleStream;
	scheduleStream.OpenLC(aStore, aId);
	InternalizeL(scheduleStream);
	CleanupStack::PopAndDestroy(&scheduleStream);
	}
	
TInt CSchedule::Offset()
	{
	return (_FOFF(CSchedule, iLink));
	}
	
TBool CSchedule::ClientInSchedule(const TDesC& aClientName)
//
// returns true if the client is part of a task associated with this schedule
//
	{
	TSglQueIter<TScheduledTask> tasks(iTaskList);
	tasks.SetToFirst();
	//
	TScheduledTask* task;
	while ((task=tasks++)!=NULL)
		{
		if (task->Client().ExecutorFileName().MatchF(aClientName) == 0)
			return ETrue;
		}
	return EFalse;
	}
	
//
//Task methods
//

void CSchedule::AddTask(TScheduledTask& aTask)
	{
	iTaskList.AddFirst(aTask);
	}

void CSchedule::RemoveTasks(TBool aFromClient)
	{
	LOGSTRING("CSchedule::RemoveTasks - start");

	TScheduledTask*  task;
	TSglQueIter<TScheduledTask> taskIter(iTaskList);
    taskIter.SetToFirst();
    while ((task = taskIter++) != NULL)
		{
		if (aFromClient)
			{
			task->RemoveInfo();
			}
		RemoveTask(task);
		}

	LOGSTRING("CSchedule::RemoveTasks - end");
	}

void CSchedule::RemoveTask(TScheduledTask* aTask)
	{
	LOGSTRING("CSchedule::RemoveTask - start");

	LOGSTRING2("CSchedule::RemoveTask - Schedule id: %d", iId);
	iTaskList.Remove(*aTask);
	delete aTask;
	LOGSTRING("CSchedule::RemoveTask - end");
	}

void CSchedule::NotifyTasks()
	{
	LOGSTRING("CSchedule::NotifyTasks - start");

	TScheduledTask*  task;
	TSglQueIter<TScheduledTask> taskIter(iTaskList);
    taskIter.SetToFirst();
    TTsTime time;
    while((task = taskIter++) != NULL)
		{
		if (iDueTime.IsUtc())
			time.SetUtcTime(iDueTime.GetUtcTime() + iValidityPeriod);
		else
			time.SetLocalTime(iDueTime.GetLocalTime()+iValidityPeriod);
		
		task->OnDue(time);
		if	(task->Info().iRepeat > 0)
			task->DecRepeat();
		if	(task->Info().iRepeat == 0)
			RemoveTask(task);
		}

	LOGSTRING("CSchedule::NotifyTasks - end");
	}

TInt CSchedule::GenerateTaskId()
	{
	LOGSTRING("CSchedule::GenerateTaskId - start");

	TInt id = iId;
	TScheduledTask* task = Task(id);
	while (task!=NULL) 
		{
		if	((id-iId) > KMaxTasksPerSchedule)
			return KErrOverflow;
		id++;
		task = Task(id);
		}
	LOGSTRING("CSchedule::GenerateTaskId - end");
	return id;
	}

TScheduledTask* CSchedule::Task(const TInt aTaskId)
	{
	TSglQueIter<TScheduledTask> tasks(iTaskList);
	tasks.SetToFirst();
	//
	TScheduledTask* task;
	while ((task=tasks++)!=NULL)
		{
		if	(task->Info().iTaskId == aTaskId)
			return task;
		}
	return NULL;
	}

void CSchedule::TasksL(CArrayFixFlat<TTaskInfo>& aTasks)
	{
	LOGSTRING("CSchedule::TasksL - start");
	TSglQueIter<TScheduledTask> taskIter(iTaskList);
	taskIter.SetToFirst();
	TScheduledTask* task;
	while ((task = taskIter++) != NULL)
		{
		aTasks.AppendL(task->Info());
		}
	LOGSTRING("CSchedule::TasksL - end");
	}

//
//Externalize/Internalize methods
//
void CSchedule::ExternalizeL(RWriteStream& aWriteStream) const
	{
	LOGSTRING("CSchedule::ExternalizeL - start");

	aWriteStream.WriteInt32L(iId);
	aWriteStream << *iName;
	aWriteStream.WriteInt32L(iPersists);
	aWriteStream.WriteInt32L(iEnabled);

	TInt count=0;
	// Count the number of schedule entries so that
	// we can write the count (in the stream) in advance
	// of the entries themselves.
	TSglQueIter<TScheduleEntry> iter(iEntryList);
	iter.SetToFirst();

	TScheduleEntry* entry;
	while((entry=iter++)!=NULL)
		count++;
	aWriteStream.WriteInt32L(count);

	// Write the entries
	iter.SetToFirst();
	while((entry=iter++)!=NULL)
		{
		entry->Info().ExternalizeL(aWriteStream);
		}
	
	//write conditions
	count = iConditions.Count();
	aWriteStream.WriteInt32L(count);
	for(TInt ii = 0; ii<count; ++ii)
		{
		aWriteStream.WriteInt32L(iConditions[ii].iCategory.iUid);
		aWriteStream.WriteUint32L(iConditions[ii].iKey);
		aWriteStream.WriteInt32L(iConditions[ii].iState);
		aWriteStream.WriteInt32L(iConditions[ii].iType);		
		}		
		
	//write security Info
	aWriteStream << iSecurityInfo;
		
	LOGSTRING("CSchedule::ExternalizeL - end");
	}

void CSchedule::InternalizeL(RReadStream& aReadStream)
	{
	LOGSTRING("CSchedule::InternalizeL - start");
	iId = aReadStream.ReadInt32L();
	HBufC* name = HBufC::NewL(aReadStream, KMaxName);
	delete iName;
	iName = name;
	iPersists = aReadStream.ReadInt32L();
	iEnabled = aReadStream.ReadInt32L();

	//Make sure we remove the entries first!
	RemoveEntries();

	TInt entries = aReadStream.ReadInt32L();
	for (TInt i=0; i<entries;i++)
		{
		TScheduleEntryInfo2 info;
		info.InternalizeL(aReadStream);		
		TScheduleEntry* entry = ScheduleEntryFactory::CreateL(info);
		iEntryList.AddLast(*entry);
		}
	
	// now read in conditions
	RemoveConditions();
	TInt conditions = aReadStream.ReadInt32L();
	for (TInt ii=0; ii<conditions;++ii)
		{
		TUid category;
		category.iUid = aReadStream.ReadInt32L();
		TUint key = aReadStream.ReadUint32L();
		TInt state = aReadStream.ReadInt32L();
		TTaskSchedulerCondition::TConditionType type 
			= static_cast<TTaskSchedulerCondition::TConditionType>(aReadStream.ReadInt32L());
		TTaskSchedulerCondition condition(category, key, state, type);
		User::LeaveIfError(iConditions.Append(condition));
		}
	
	// read in security Info
	aReadStream >> iSecurityInfo;
	
	LOGSTRING("CSchedule::InternalizeL - end");
	}
	
//
//Entries methods
//

void CSchedule::EntriesL(CArrayFixFlat<TScheduleEntryInfo2>& aEntries)
	{
	LOGSTRING("CSchedule::EntriesL - start");
	TSglQueIter<TScheduleEntry> entryIter(iEntryList);
	entryIter.SetToFirst();
	TScheduleEntry* entry;
	while ((entry = entryIter++) != NULL)
		{
		aEntries.AppendL(entry->Info());
		}
	LOGSTRING("CSchedule::EntriesL - end");
	}

void CSchedule::AddEntriesL(const CArrayFixFlat<TScheduleEntryInfo2>& aEntries)
	{
	LOGSTRING("CSchedule::AddEntriesL - start");
	TInt count = aEntries.Count();
	TScheduleEntryInfo2 entryInfo2;
	TTsTime ttsTime;	//temporary needed due to gccxml compiler
	TDateTime dateTime;
	
	for (TInt i = 0;i<count;i++)
		{
		entryInfo2 = aEntries.At(i);
		
		// Zero out time to the nearest ms.
		if(entryInfo2.StartTime().IsUtc())
			{
			dateTime = entryInfo2.StartTime().GetUtcTime().DateTime();
			dateTime.SetMicroSecond(0);
			ttsTime.SetUtcTime(dateTime);
			entryInfo2.SetStartTime(ttsTime);
			}
		else
			{
			dateTime = entryInfo2.StartTime().GetLocalTime().DateTime();
			dateTime.SetMicroSecond(0);
			ttsTime.SetLocalTime(dateTime);
			entryInfo2.SetStartTime(ttsTime);
			}
			
		TScheduleEntry* entry = ScheduleEntryFactory::CreateL(entryInfo2);
		iEntryList.AddLast(*entry);
		}

	LOGSTRING("CSchedule::AddEntriesL - end");
	}

void CSchedule::ReplaceEntriesL(const CArrayFixFlat<TScheduleEntryInfo2>& aEntries)
	{
	// remove the original entries
	RemoveEntries();
	AddEntriesL(aEntries);
	}

void CSchedule::RemoveEntries()
	{
	LOGSTRING("CSchedule::RemoveEntries - start");

	TScheduleEntry* entry;
    TSglQueIter<TScheduleEntry> iter(iEntryList);
	iter.SetToFirst();
    while ((entry = iter++) != NULL)
		{
		iEntryList.Remove(*entry);
		delete entry;
		}

	LOGSTRING("CSchedule::RemoveEntries - end");
	}
	
//Condition methods
void CSchedule::AddConditionsL(const CArrayFixFlat<TTaskSchedulerCondition>& aConditions)
	{
	TInt count = aConditions.Count();
	for (TInt i = 0;i<count;++i)
		User::LeaveIfError(iConditions.Append(aConditions[i]));
	}
	
void CSchedule::ReplaceConditionsL(const CArrayFixFlat<TTaskSchedulerCondition>& aConditions)
	{
	// remove the original conditions
	RemoveConditions();
	AddConditionsL(aConditions);
	}

void CSchedule::ConditionsL(CArrayFixFlat<TTaskSchedulerCondition>& aConditions)
	{
	TInt count = iConditions.Count();
	for (TInt i = 0;i<count;++i)
		{
		aConditions.AppendL(iConditions[i]);
		}
	}

void CSchedule::RemoveConditions()
	{
	iConditions.Reset();
	}

const TTsTime& CSchedule::DefaultRunTimeL() const
	{
	TSglQueIter<TScheduleEntry> entryIter(iEntryList);
	entryIter.SetToFirst();
	TScheduleEntry* entry = entryIter;
	if (entry == NULL)
		User::Leave(KErrArgument);
	return entry->DueTime();
	}	
	
// This method is called when ever a new task is scheduled or a change to an 
// existing schedule is made. When one of the schedule entries in this schedule 
// has become due, this is called with aNotFirstTime = ETrue.  All this does is
// move the next due time into the next time frame.
void CSchedule::CalculateDueTime(TBool aNotFirstTime)
	{
	// Sort the list of entries
	TSglQueIter<TScheduleEntry> iter(iEntryList);
	iter.SetToFirst();
	TScheduleEntry* entry;
	TTime currentTTime;
	TTsTime currentTTsTime;
	//make sure we reset iDueTime to max so that only the minimum is calculated.
	TTsTime maxTime(Time::MaxTTime(), ETrue);
	TTsTime dueTime;
	iDueTime = maxTime; 
	while ((entry = iter++)!=NULL)
		{
		currentTTime.UniversalTime();
		currentTTsTime.SetUtcTime(currentTTime);
		// This works out when the schedule is next due based on the input time
		// and also updates the due time if it is home time based
		dueTime = entry->NextScheduledTime(currentTTsTime);
		if(aNotFirstTime && dueTime.GetUtcTime() <= currentTTime) 
			{
			// We don't want this schedule to run straight away so seed the 
			// next due initial time-frame by incrementing the validity
			currentTTime += entry->Info().ValidityPeriod();
			currentTTime += TTimeIntervalMicroSeconds32(1); // push into the next boundary
				
			if (dueTime.IsUtc())
				currentTTsTime.SetUtcTime(currentTTime);
			else
				currentTTsTime.SetLocalTime(currentTTime + dueTime.GetOffset());
				
			dueTime = entry->NextScheduledTime(currentTTsTime);
			}
		if(dueTime.GetUtcTime() < iDueTime.GetUtcTime()) //find earliest due time from all entries
			{	
			iDueTime = dueTime;
			iValidityPeriod = entry->Info().ValidityPeriod();
			}	 
		}
	}

// if aCalculateForConditions is true then entrycount corresponds
// to number of conditions;
void CSchedule::GetInfo(TScheduleInfo& aInfo, TBool aCalculateForConditions) 
	{
	aInfo.iState.SetName(Name());
	aInfo.iState.SetDueTime(iDueTime);
	aInfo.iState.SetPersists(Persists());
	aInfo.iState.SetEnabled(Enabled());
	TInt taskCount = 0;

	TSglQueIter<TScheduledTask> taskIter(*Tasks());
	taskIter.SetToFirst();
	while (taskIter++ != NULL)
		{
		taskCount++;
		}
	aInfo.iTaskCount = taskCount;
	TInt entryCount = 0;
	if(!aCalculateForConditions)
		{
		TSglQueIter<TScheduleEntry> entryIter(iEntryList);
		entryIter.SetToFirst();
		while (entryIter++ != NULL)
			{
			entryCount++;
			}
		}
	else
		entryCount = iConditions.Count();	
	aInfo.iEntryCount = entryCount;
	}

const RArray<TTaskSchedulerCondition>& CSchedule::Conditions() const
	{
	return iConditions;
	}
	
TBool CSchedule::IsAccessAllowed(const RMessagePtr2& aMessage) const
	{
	// Access allowed if message SID is the same as the schedule creator
	// or if client has WriteDeviceData
	return aMessage.SecureId()==iSecurityInfo.iSecureId 
		|| aMessage.HasCapability(ECapabilityWriteDeviceData)
		|| PlatSec::ConfigSetting(PlatSec::EPlatSecEnforcement)==0; // Enforcement off
	}
	
const TSecurityInfo& CSchedule::SecurityInfo() const
	{
	return iSecurityInfo;
	}

TBool CSchedule::IsUpdatable()
	{
	if(HasTasks() && Enabled() )
		return 	ETrue;	
	else
		return EFalse;	
	}
