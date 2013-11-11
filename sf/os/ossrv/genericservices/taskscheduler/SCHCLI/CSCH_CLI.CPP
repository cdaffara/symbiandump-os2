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
// Implements RScheduler functions: client-side session with the task scheduler
// 
//

#include "CSCH_CLI.H"	//definition of RScheduler
#include "CSCHCODE.H"	//opcodes
#include "SCHEXE.H"
#include <schinfointernal.h>

/** Default constructor. */
EXPORT_C RScheduler::RScheduler()
    {
    }

/** 
Connects a client to the Task Scheduler server, creating a session with that 
server.

@return KErrNone, if successful; otherwise one of the other system wide error 
codes. 
*/
EXPORT_C TInt RScheduler::Connect()
	{
	TInt r=CreateSession(KSchSvrName, Version(),0);
	if (r==KErrNotFound)
		{
		r=StartSch32();
		if (r!=KErrNone)
			return r;
		r=CreateSession(KSchSvrName, Version(),0);
	    }
	return(r); 
	}
	
/** 
Gets the client side version number.

@return The version number. 
*/
EXPORT_C TVersion RScheduler::Version() const
	{
	return(TVersion(KESchMajorVersionNumber,KESchMinorVersionNumber,KESchBuildVersionNumber));
	}

/** 
Registers a client with the Task Scheduler.

A client must register with the Task Scheduler before scheduling any tasks, 
but does not need to register just to create and edit schedules.

@param aFileName The name and full path of a program that encapsulates the 
client-specific knowledge for implementing tasks. On the emulator, the program 
should be a DLL; on an ARM processor, it should be an executable.
@param aPriority A priority value.
@return KErrNone, if successful; otherwise one of the other system wide error 
codes. 
*/
EXPORT_C TInt RScheduler::Register(const TFileName& aFileName,const TInt aPriority)
	{
	return SendReceive(ERegisterClient,TIpcArgs(&aFileName,aPriority));
	}

/** 
Creates a persistent time based schedule.

This schedule has no tasks associated with it but merely contains information
about start and finish times.

@capability WriteDeviceData
@param aRef Definition of the new schedule.  On return this contains a valid
handle to the newly created schedule.
@param aEntryList The set of schedule entries that make up the new schedule.
@return KErrNone, if successful, KErrArgument if the condition array is 
empty, or an entry has an interval less than 1, KErrServerBusy, if a backup or 
restore operation is taking place, KErrPermissionDenied if the client 
does not have WriteDeviceData capability. 
Otherwise one of the other system wide error codes.
@see TScheduleEntryInfo2
*/
EXPORT_C TInt RScheduler::CreatePersistentSchedule(TSchedulerItemRef& aRef, 
							const CArrayFixFlat<TScheduleEntryInfo2>& aEntryList)
	{
	TScheduleSettings2 settings;
	settings.iPersists = ETrue;
	settings.iEntryCount = aEntryList.Count();
	settings.iName = aRef.iName;
	return CreateSchedule(aRef, aEntryList, settings);
	}

/** 
Creates a persistent condition based schedule.

This schedule has no tasks associated with it but merely contains information
about conditions that must be satified to complete this schedule.

@capability WriteDeviceData
@param aRef Definition of the new schedule.  On return this contains a valid
handle to the newly created schedule.
@param aConditions An array of system agent conditons 
that make up the schedule.
@param aDefaultRunTime The time at which the schedule with run if no
conditions are met. If this is a local time based value, the schedule will remain
at that local time regardless of timezone and DST changes (ie. it will float)
If the value is UTC based, the schedule will remain at that UTC time (will not float).
@return KErrNone, if successful, KErrArgument if the condition array is 
empty or if the publish and subscribe variables have an invalid category, 
KErrServerBusy, if a backup or restore operation is taking place,
KErrPermissionDenied if the client does not have WriteDeviceData capability. 
Otherwise one of the other system wide error codes.
@see TTaskSchedulerCondition 
@internalAll
*/
EXPORT_C TInt RScheduler::CreatePersistentSchedule(TSchedulerItemRef& aRef, 
							const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							const TTsTime& aDefaultRunTime)
	{
	TScheduleSettings2 settings;
	settings.iPersists = ETrue;
	settings.iEntryCount = aConditions.Count();
	settings.iName = aRef.iName;
	return CreateSchedule(aRef, aConditions, aDefaultRunTime, settings);	
	}

static TBool ScheduleEntriesAreBad(const CArrayFixFlat<TScheduleEntryInfo2>& aEntries)
	{//must have at least 1 entry, each entry's interval must be at least 1
	TInt count = aEntries.Count();
	if (count==0)
		return ETrue;
	for (TInt i=0; i < count; i++)
		{
		const TScheduleEntryInfo2* entry = &(aEntries.At(i));
		if (entry->Interval() < 1)
			return ETrue;
		}
	return EFalse;
	}

TInt RScheduler::CreateSchedule(TSchedulerItemRef& aRef, 
			const CArrayFixFlat<TScheduleEntryInfo2>& aEntryList, 
			const TScheduleSettings2& aSettings)
	{//check critical aspects of input here (client-side)
	if (ScheduleEntriesAreBad(aEntryList))
		return KErrArgument;
	
	//write settings (entry count + persists flag + name)
	TPckgC<TScheduleSettings2> pSettings(aSettings);
	//write entries
	TPtrC8 pA((TUint8*)&aEntryList.At(0), sizeof(TScheduleEntryInfo2)*aSettings.iEntryCount); 
	
	//read back generated ID
	TPckg<TInt> id(aRef.iHandle);
	return SendReceive(ECreateTimeSchedule, TIpcArgs(&pSettings, &pA, &id));
	}
	
static TBool ScheduleEntriesAreBad(const CArrayFixFlat<TTaskSchedulerCondition>& aConditions)
	{//must have at least 1 condition in array. The category for a condition cannot be KNullUid.
	TInt count = aConditions.Count();
	if (count==0)
		return ETrue;
	for (TInt i=0; i < count; i++)
		{
		const TTaskSchedulerCondition* entry = &(aConditions.At(i));
		if (entry->iCategory == KNullUid)
			return ETrue;
		}
	return EFalse;
	}

TInt RScheduler::CreateSchedule(TSchedulerItemRef& aRef, 
			const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
			const TTsTime& aDefaultRunTime, 
			const TScheduleSettings2& aSettings)
	{//check critical aspects of input here (client-side)
	if (ScheduleEntriesAreBad(aConditions))
		return KErrArgument;
	
	//write settings (entry count + persists flag + name)
	TPckgC<TScheduleSettings2> pSettings(aSettings);
	//write entries
	TPtrC8 pA((TUint8*)&aConditions.At(0), sizeof(TTaskSchedulerCondition)*aSettings.iEntryCount); 
	//write Time
	TPckgC<TTsTime> pTime(aDefaultRunTime);
	
	//read back generated ID
	TPckg<TInt> id(aRef.iHandle);
	return SendReceive(ECreateConditionSchedule, TIpcArgs(&pSettings, &pA, &pTime, &id));
	}

/** 
Changes a time based schedule.

Note that changing a schedule is implemented by supplying a replacement set 
of schedule entries.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aScheduleHandle The Id that identifies the schedule.
@param aEntryList The set of schedule entries that will make up the new schedule.
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrArgument if the schedule is not a time based one, 
KErrPermissionDenied if the client does not have the same SID as the schedules
creator or has WriteDeviceData capability, KErrServerBusy, if a backup or 
restore operation is taking place or any of the other system wide error 
codes.
@see TScheduleEntryInfo2
*/
EXPORT_C TInt RScheduler::EditSchedule(const TInt aScheduleHandle, const CArrayFixFlat<TScheduleEntryInfo2>& aEntryList)
	{
	if (ScheduleEntriesAreBad(aEntryList))
		return KErrArgument;
	TInt count = aEntryList.Count();
	TPtrC8 pA((TUint8*) &aEntryList.At(0), sizeof(TScheduleEntryInfo2)*count);
	return SendReceive(EEditTimeSchedule, TIpcArgs(count, aScheduleHandle, &pA));
	}

/** 
Changes a condition based schedule.

Note that changing a schedule is implemented by supplying a replacement set 
of schedule conditons and time.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aScheduleHandle The Id that identifies the schedule.
@param aConditions An array of system agent conditons 
that make up the schedule.
@param aDefaultRunTime The time at which the schedule with run if no
conditions are met. If this is a local time based value, the schedule will remain
at that local time regardless of timezone and DST changes (ie. it will float)
If the value is UTC based, the schedule will remain at that UTC time (will not float).
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrArgument if the schedule is not a condition based one
or if the publish and subscribe variables, defined by aConditions category 
and key values, do not exist or are not of integral type, 
KErrPermissionDenied if the client does not have the same SID as the schedules
creator or has WriteDeviceData capability, KErrServerBusy, if a backup or 
restore operation is taking place, or any of the other system wide error 
codes.

@internalAll
*/	
EXPORT_C TInt RScheduler::EditSchedule(TInt aScheduleHandle, 
						const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
						const TTsTime& aDefaultRunTime)
	{
	TInt count = aConditions.Count();
	TPtrC8 pA((TUint8*) &aConditions.At(0), sizeof(TTaskSchedulerCondition)*count);
	//write Time
	TPckgC<TTsTime> pTime(aDefaultRunTime);
	return SendReceive(EEditConditionSchedule, TIpcArgs(count, aScheduleHandle, &pA, &pTime));	
	}


/** 
Deletes the specified schedule.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

Note that a schedule cannot be deleted if there are tasks associated with 
it; the tasks must be explicitly deleted first.

@param aScheduleHandle The Id that identifies the schedule.
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrArgument if there are outstanding tasks associated with 
the schedule, KErrPermissionDenied if the client does not have the same SID as 
the schedules creator or has WriteDeviceData capability, KErrServerBusy, if a 
backup or restore operation is taking place or any of the other 
system wide error codes.
*/
EXPORT_C TInt RScheduler::DeleteSchedule(const TInt aScheduleHandle) const
	{
	return SendReceive(EDeleteSchedule, TIpcArgs(aScheduleHandle));
	}

/** 
Disables the specified schedule.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aScheduleHandle The Id that identifies the schedule.
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrPermissionDenied if the client does not have the same SID 
as the schedules creator or has WriteDeviceData capability, KErrServerBusy, if a 
backup or restore operation is taking place or any of the other 
system wide error codes.
*/
EXPORT_C TInt RScheduler::DisableSchedule(const TInt aScheduleHandle) const
	{
	return SendReceive(EDisableSchedule, TIpcArgs(aScheduleHandle));
	}

/** 
Enables the specified schedule.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aScheduleHandle The Id that identifies the schedule.
@return KErrNone if successful. KErrNotFound if there is no schedule with 
the specified Id, KErrPermissionDenied if the client does not have the same SID 
as the schedules creator or has WriteDeviceData capability, KErrServerBusy, if a 
backup or restore operation is taking place or any of the other 
system wide error codes.
*/
EXPORT_C TInt RScheduler::EnableSchedule(const TInt aScheduleHandle) const
	{
	return SendReceive(EEnableSchedule, TIpcArgs(aScheduleHandle));
	}

/** 
Adds a task to an existing persistent schedule.

Behaviour of execution after a Backup and Restore operation should be 
considered when adding tasks to a persistent schedule. Persistent schedules might be 
backed up when a one-off task might be pending, and become due and executed some time 
after the backup operation. When the backup is restored, the tasks might be executed 
again if they are still valid (restore done during the validation period for time-based 
schedules, or conditions satisfied after restore for condition-based schedules). Clients 
should refrain from creating tasks that might have undesired effects under these 
conditions (e.g. by incurring a cost to the user by sending an SMS twice). 

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aTaskInfo Information about the task to be added to the schedule. On return
the task Id is written into this class.
@param aTaskData Data that is passed to the task on execution.
@param aScheduleHandle The Id that identifies the schedule to be used.
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrArgument if a task with a repeat vale other than 0 is 
being tried to be assigned to a condition based schedule, 
KErrPermissionDenied if the client does not have the same SID as the schedules
creator or has WriteDeviceData capability, KErrServerBusy, if a backup or 
restore operation is taking place or any of the other system wide error 
codes.
@panic CTaskScheduler 0 The client has not registered. The client must register 
before adding tasks to the schedule. 
*/
EXPORT_C TInt RScheduler::ScheduleTask(TTaskInfo& aTaskInfo, 
									HBufC& aTaskData, 
									const TInt aScheduleHandle)
	{
	TPckgC<TTaskInfo> pI(aTaskInfo);
	//scheduler writes back task id
	TPckg<TInt> id(aTaskInfo.iTaskId);
	TPtr pD(aTaskData.Des());
	return SendReceive(EScheduleTask, TIpcArgs(&pI,aScheduleHandle,&id, &pD));
	}

/** 
Creates a new, transient, time based schedule and adds a task to it.

Note that a transient schedule is destroyed when the task is destroyed or 
power is lost.

@param aTaskInfo Information about the task to be added to the transient schedule. 
On return the task Id is written into this class.
@param aTaskData Data that is passed to the task on execution.
@param aRef Definition of the new transient schedule.
@param aEntryList The set of schedule entries that make up the new transient 
schedule.
@return KErrNone, if successful, or any of the other system wide error codes.
@panic CTaskScheduler 0 The client has not registered. The client must register 
before adding tasks to the schedule.
@see TScheduleEntryInfo2 
*/
EXPORT_C TInt RScheduler::ScheduleTask(TTaskInfo& aTaskInfo, 
									HBufC& aTaskData, 
									TSchedulerItemRef& aRef,
									const CArrayFixFlat<TScheduleEntryInfo2>& aEntryList)
	{
	TScheduleSettings2 settings;
	settings.iPersists = EFalse;
	settings.iEntryCount = aEntryList.Count();
	settings.iName = aRef.iName;
	TInt res = CreateSchedule(aRef, aEntryList, settings);
	if (res==KErrNone)
		{
		res = ScheduleTask(aTaskInfo, aTaskData, aRef.iHandle);
		//the schedule here needs a task: so if we can't schedule a task,
		//need to delete schedule
		if (res!=KErrNone)
			DeleteSchedule(aRef.iHandle);
		}
	return res;
	}

/** 
Creates a new, transient, condition based schedule and adds a task to it.

Note that a transient schedule is destroyed when the task is destroyed or
power is lost.

@param aTaskInfo Information about the task to be added to the transient schedule. 
On return the task Id is written into this class.
@param aTaskData Data that is passed to the task on execution.
@param aRef Definition of the new transient schedule.
@param aConditions An array of schedule conditons that make up the schedule.
@param aDefaultRunTime The time at which the schedule will run if no
conditions are met. If this is a local time based value, the schedule will remain
at that local time regardless of timezone and DST changes (ie. it will float)
If the value is UTC based, the schedule will remain at that UTC time (will not float).
@return KErrNone, if successful, KErrArgument if the publish and subscribe 
variables, defined by aConditions category and key values, do not exist or 
are not of integral type, or any of the other system wide error codes.
@panic CTaskScheduler 0 The client has not registered. The client must register 
before adding tasks to the schedule.

@internalAll
*/
EXPORT_C TInt RScheduler::ScheduleTask(TTaskInfo& aTaskInfo, 
								HBufC& aTaskData, 
								TSchedulerItemRef& aRef, 
								const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
								const TTsTime& aDefaultRunTime)
	{
	TScheduleSettings2 settings;
	settings.iPersists = EFalse;
	settings.iEntryCount = aConditions.Count();
	settings.iName = aRef.iName;
	TInt res = CreateSchedule(aRef, aConditions, aDefaultRunTime, settings);
	if (res==KErrNone)
		{
		res = ScheduleTask(aTaskInfo, aTaskData, aRef.iHandle);
		//the schedule here needs a task: so if we can't schedule a task,
		//need to delete schedule
		if (res!=KErrNone)
			DeleteSchedule(aRef.iHandle);
		}
	return res;
	}

/** 
Deletes the specified task.

@capability Note Only clients with the same SID as the relevant schedule
creator, or WriteDeviceData capability can sucessfully call this API.

@param aTaskId The Id that identifies the task.
@return KErrNone if successful, KErrNotFound if there is no task with the 
specified Id, KErrPermissionDenied if the client does not have the same SID as 
the schedules creator or has WriteDeviceData capability, KErrServerBusy, if a 
backup or restore operation is taking place or any of the 
other system wide error codes.
*/
EXPORT_C TInt RScheduler::DeleteTask(const TInt aTaskId) const
	{
	return SendReceive(EDeleteTask, TIpcArgs(aTaskId));
	}

/** 
Gets a list of all schedules, or a subset of schedules.

@capability Note A call to this API will only retrieve schedules created with
the caller's SID.  If the caller has WriteDeviceData capability all schedules
will be retrieved.

@param aScheduleRefArray On return, a populated array of schedule definitions. 
Note that populating the array could cause this function to leave because 
of an out of memory condition.
@param aFilter The schedule filter.
@return KErrNone if successful otherwise one of the other system wide error 
codes.
@see TSchedulerItemRef 
*/
EXPORT_C TInt RScheduler::GetScheduleRefsL(CArrayFixFlat<TSchedulerItemRef>& aScheduleRefArray,
									const TScheduleFilter aFilter)
	{
	//1) get number of refs
	TInt count;//scheduler writes back count
	TPckg<TInt> pCount(count);
	TInt res = SendReceive(ECountSchedules, TIpcArgs(&pCount, aFilter));
	if (res!=KErrNone)
		return res;
	aScheduleRefArray.ResizeL(count);
	//2) get refs, if there are any
	if (count>0)
		{
		TPtr8 sPtr((TUint8*)&(aScheduleRefArray.At(0)), count*sizeof(TSchedulerItemRef));
		res = SendReceive(EGetScheduleRefs, TIpcArgs(count, aFilter, &sPtr));
		}
	return res;
	}

/** 
Gets information relating to a specified time based schedule.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aScheduleHandle The Id that identifies the schedule.
@param aState On return, the state of the specified schedule.
@param aEntries On return, a populated array of schedule entries that make 
up the schedule. Note that populating the array could cause this function 
to leave because of an out of memory condition.
@param aTasks On return, a populated array of tasks associated with the schedule. 
Note that populating the array could cause this function to leave because 
of an out of memory condition.
@param aDueTime On return, the time that the schedule is next due. This value may
be local time or UTC based,dependent on the type of time used for schedule entries.
Comparisons used within the scheduler to find the next due time are UTC based.
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrArgument if the schedule is not a time based one, 
KErrPermissionDenied if the client does not have the same SID as the schedules
creator or has WriteDeviceData capability, or any of the other system wide error 
codes.
@see TScheduleEntryInfo2
@see TTaskInfo 
*/
EXPORT_C TInt RScheduler::GetScheduleL(const TInt aScheduleHandle, 
									TScheduleState2& aState, 
									CArrayFixFlat<TScheduleEntryInfo2>& aEntries, 
									CArrayFixFlat<TTaskInfo>& aTasks, 
									TTsTime& aDueTime)
	{
	TScheduleInfo info;
	TInt res = GetScheduleInfo(aScheduleHandle, info, aDueTime);
	if (res != KErrNone)
		return res;
	aState = info.iState;
	res = GetScheduleDataL(aScheduleHandle, info, aEntries);
	if (res != KErrNone)
		return res;
	res = GetTaskDataL(aScheduleHandle, info, aTasks);							
	return res;
	}

/** 
Gets information relating to a specified condition based schedule.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aScheduleHandle The Id that identifies the schedule.
@param aState On return, the state of the specified schedule.
@param aConditions On return, a populated array of schedule conditons 
that make up the schedule. Note that populating the array could cause 
this function to leave because of an out of memory condition.
@param aDefaultRunTime On return, the time at which the schedule with run if
no conditions are met. If this is a local time based value, the schedule will remain
at that local time regardless of timezone and DST changes (ie. it will float)
If the value is UTC based, the schedule will remain at that UTC time (will not float).
@param aTasks On return, a populated array of tasks associated with the schedule. 
Note that populating the array could cause this function to leave because 
of an out of memory condition.
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrArgument if the schedule is not a condition based one, 
KErrPermissionDenied if the client does not have the same SID as the schedules
creator or has WriteDeviceData capability, or any of the other system wide error 
codes.
@see TTaskSchedulerCondition
@see TTaskInfo 

@internalAll
*/
EXPORT_C TInt RScheduler::GetScheduleL(TInt aScheduleHandle, 
							TScheduleState2& aState, 
							CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							TTsTime& aDefaultRunTime,
							CArrayFixFlat<TTaskInfo>& aTasks)
	{
	TScheduleInfo info;
	TTsTime dummyTime;
	TInt res = GetScheduleInfo(aScheduleHandle, info, dummyTime);
	if (res != KErrNone)
		return res;
	aState = info.iState;
	res = GetScheduleDataL(aScheduleHandle, 
							info, 
							aConditions, 
							aDefaultRunTime);
	if (res != KErrNone)
		return res;
	res = GetTaskDataL(aScheduleHandle, info, aTasks);							
	return res;
	}

/**
Gets the schedule type.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aScheduleHandle The Id that identifies the schedule
@param aScheduleType On return the type of schedule relating to the handle.
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrPermissionDenied if the client does not have the same SID 
as the schedules creator or has WriteDeviceData capability, or any of the other 
system wide error codes.

@internalAll
*/
EXPORT_C TInt RScheduler::GetScheduleTypeL(TInt aScheduleHandle,
											TScheduleType& aScheduleType)
	{
	TPckg<TScheduleType> id(aScheduleType);
	return SendReceive(EGetScheduleType, TIpcArgs(aScheduleHandle, &id));
	}

/** 
Gets a list of all tasks, or a subset of tasks.

Note that if more than one client has supplied the same information when registering, 
then it is possible for a list of tasks to be returned, which the calling 
client has not scheduled.

@capability Note A call to this API will only retrieve tasks created with
the caller's SID.  If the caller has WriteDeviceData capability all tasks
will be retrieved.

@param aTasks On return, a populated array of schedule definitions. The schedule 
definitions are those whose associated tasks satisfy the selection criteria. 
Note that populating the array could cause this function to leave because 
of an out of memory condition.
@param aScheduleFilter The schedule filter.
@param aTaskFilter The task filter.
@return KErrNone if successful otherwise one of the other system wide error 
codes.
@see TSchedulerItemRef 
*/
EXPORT_C TInt RScheduler::GetTaskRefsL(CArrayFixFlat<TSchedulerItemRef>& aTasks,
										const TScheduleFilter aScheduleFilter,
										const TTaskFilter aTaskFilter)
	{
	//1)get number of tasks
	TInt count;
	TPckg<TInt> pCount(count);
	TInt res = SendReceive(ECountTasks, TIpcArgs(&pCount, aScheduleFilter, aTaskFilter));
	if (res!=KErrNone)
		return res;
	aTasks.ResizeL(count);
	//2)get taskrefs
	if (count >0)
		{
		TPtr8 pTasks(REINTERPRET_CAST(TUint8*, &(aTasks.At(0))), count*sizeof(TSchedulerItemRef));
		res = SendReceive(EGetTaskRefs, TIpcArgs(count, aScheduleFilter, aTaskFilter, &pTasks));
		}
	return res;
	}

/** 
Gets information relating to a specified task.

@capability Note Only clients with the same SID as the relevant schedule
creator, or WriteDeviceData capability can sucessfully call this API.

@param aTaskId The Id that identifies the task.
@param aTaskInfo On return, information about the task.
@param aTaskData On return, a pointer descriptor representing the data that 
is passed to the program to be executed. The caller must set up this pointer 
descriptor before calling the function. The required length of the descriptor 
can be found by calling GetTaskDataSize().
@param aRef On return, the associated schedule definition.
@param aNextDue On return, the time that the task is next due. This value may
be local time or UTC based. Comparisons used to find the next due time are based on UTC.
@return KErrNone, if successful. KErrNotFound, if there is no task with the 
specified Id, KErrPermissionDenied if the client does not have the same SID as 
the schedules creator or has WriteDeviceData capability. 
*/
EXPORT_C TInt RScheduler::GetTaskInfoL(const TInt aTaskId, TTaskInfo& aTaskInfo, TPtr& aTaskData, TSchedulerItemRef& aRef, TTsTime& aNextDue)
	{
	// First of all retrieve the normal stuff
	TPtr8 pInfo((TUint8*)&aTaskInfo,sizeof(TTaskInfo));
	TInt size = aTaskData.MaxLength();
	TInt res = SendReceive(EGetTask, TIpcArgs(aTaskId, &pInfo, size, &aTaskData));
	if (res != KErrNone)
		return res;

	// Next retrieve the TSchedulerItemRef and next due time
	TPtr8 pItemRef((TUint8*)&aRef, sizeof(TSchedulerItemRef));
	TPtr8 pDueTime((TUint8*)&aNextDue, sizeof(TTsTime));
	return SendReceive(EGetSchedulerItemRefAndNextDueTime, TIpcArgs(aTaskId, &pItemRef, &pDueTime));
	}

/** 
Gets the size of the data to be passed to the task's program.

This function should be called before calling GetTaskInfoL() so that a descriptor 
of the correct size can be set up.

@capability Note Only clients with the same SID as the relevant schedule
creator, or WriteDeviceData capability can sucessfully call this API.

@param aTaskId The Id that identifies the task.
@param aSize The size of the task's data.
@return KErrNone if successful, KErrNotFound if there is no task with the 
specified Id, KErrPermissionDenied if the client does not have the same SID as 
the schedules creator or has WriteDeviceData capability, otherwise any of the 
other system wide error codes.
*/
EXPORT_C TInt RScheduler::GetTaskDataSize(const TInt aTaskId, TInt& aSize)
	{
	TPckg<TInt> pSize(aSize);
	return SendReceive(EGetTaskDataSize, TIpcArgs(aTaskId, &pSize));
	}

//Converts array of TScheduleEntryInfos to an array of TScheduleEntryInfo2s.
void ConvertArrayToEntryInfo2L(const CArrayFixFlat<TScheduleEntryInfo>& aEntryList,
							CArrayFixFlat<TScheduleEntryInfo2>& aEntry2List)
	{
	aEntry2List.ResizeL(aEntryList.Count());
	
	TScheduleEntryInfo entryInfo;
	for (TInt i=0; i < aEntryList.Count(); i++)
		{
		entryInfo = aEntryList.At(i);
		//create new TScheduleEntryInfo2 object from old TScheduleEntryInfo object
		//assume local time for backwards compatibility
		TScheduleEntryInfo2 entryInfo2(entryInfo);	
		aEntry2List.At(i) = entryInfo2;
		}	
	}

//Converts array of TScheduleEntryInfo2s to an array of TScheduleEntryInfos.
void ConvertArrayFromEntryInfo2L(const CArrayFixFlat<TScheduleEntryInfo2>& aEntry2List,
							CArrayFixFlat<TScheduleEntryInfo>& aEntryList)
	{
	TScheduleEntryInfo entryInfo;
	TScheduleEntryInfo2 entryInfo2;
	
	aEntryList.ResizeL(aEntry2List.Count());
	for(TInt i=0; i<aEntry2List.Count(); i++)
		{
		entryInfo2 = aEntry2List.At(i);
		
		//use local time for backwards compatibility
		entryInfo.iStartTime = entryInfo2.StartTime().GetLocalTime(); 
		entryInfo.iIntervalType = entryInfo2.IntervalType();
		entryInfo.iInterval = entryInfo2.Interval();
		entryInfo.iValidityPeriod = entryInfo2.ValidityPeriod();
		
		aEntryList.At(i) = entryInfo;
		}
	}

/** 
Creates a persistent time based schedule.

This schedule has no tasks associated with it but merely contains information
about start and finish times.

@capability WriteDeviceData
@param aRef Definition of the new schedule.  On return this contains a valid
handle to the newly created schedule.
@param aEntryList The set of schedule entries that make up the new schedule.
Start times of all entries are assumed to be local time based.
@return KErrNone, if successful, KErrArgument if the condition array is 
empty, or an entry has an interval less than 1, KErrServerBusy, if a backup or 
restore operation is taking place, KErrPermissionDenied if the 
client does not have WriteDeviceData capability. 
Otherwise one of the other system wide error codes.
@see TScheduleEntryInfo 
@deprecated See note in CSch_Cli.h
*/
EXPORT_C TInt RScheduler::CreatePersistentSchedule(TSchedulerItemRef& aRef, 
							const CArrayFixFlat<TScheduleEntryInfo>& aEntryList)
	{
	//create schedule settings object
	TScheduleSettings2 settings;
	settings.iPersists = ETrue;
	settings.iName = aRef.iName;
	settings.iEntryCount = aEntryList.Count();
	
	//create array of TScheduleEntryInfo2's from array of TScheduleEntryInfo's.
	//assume local time for backwards compatibility.
	CArrayFixFlat<TScheduleEntryInfo2>* entryInfo2List = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(1);
	CleanupStack::PushL(entryInfo2List);
	ConvertArrayToEntryInfo2L(aEntryList, *entryInfo2List);

	//create schedule using new array of TScheduleEntryInfo2s.
	TInt err = CreateSchedule(aRef, *entryInfo2List, settings);
	
	CleanupStack::PopAndDestroy(entryInfo2List);
	return err;
	}
	

	
/** 
Creates a persistent condition based schedule.

This schedule has no tasks associated with it but merely contains information
about conditions that must be satified to complete this schedule.

@capability WriteDeviceData
@param aRef Definition of the new schedule.  On return this contains a valid
handle to the newly created schedule.
@param aConditions An array of system agent conditons 
that make up the schedule.
@param aDefaultRunTime The time at which the schedule with run if no
conditions are met. Default run times of all schedules created 
using this API are assumed to be local time based.
@return KErrNone, if successful, KErrArgument if the condition array is 
empty or if the publish and subscribe variables have an invalid category.
KErrServerBusy, if a backup or restore operation is taking place.
KErrPermissionDenied if the client does not have WriteDeviceData capability. 
Otherwise one of the other system wide error codes.
@see TTaskSchedulerCondition 
@internalAll
@deprecated See note in CSch_Cli.h 
*/
EXPORT_C TInt RScheduler::CreatePersistentSchedule(TSchedulerItemRef& aRef, 
							const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							const TTime& aDefaultRunTime)
	{
	//create schedule settings object
	TScheduleSettings2 settings;
	settings.iPersists = ETrue;
	settings.iEntryCount = aConditions.Count();
	settings.iName = aRef.iName;
	
	// create TTsTime object from TTime argument aDefaultRunTime.
	// assume aDefaultTime is local time based for backwards compatibility
	TTsTime defaultRunTime(aDefaultRunTime, EFalse);	
	
	return CreateSchedule(aRef, aConditions, defaultRunTime, settings);	
	}


/** 
Changes a time based schedule.

Note that changing a schedule is implemented by supplying a replacement set 
of schedule entries.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aScheduleHandle The Id that identifies the schedule.
@param aEntryList The set of schedule entries that will make up the new schedule.
Start times of all entries are assumed to be local time based.
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrArgument if the schedule is not a time based one, 
KErrPermissionDenied if the client does not have the same SID as the schedules
creator or has WriteDeviceData capability, KErrServerBusy, if a backup or 
restore operation is taking place or any of the other system wide error 
codes.
@see TScheduleEntryInfo 
@deprecated See note in CSch_Cli.h
*/
EXPORT_C TInt RScheduler::EditSchedule(const TInt aScheduleHandle, const CArrayFixFlat<TScheduleEntryInfo>& aEntryList)
	{
	TInt count = aEntryList.Count();
	
	CArrayFixFlat<TScheduleEntryInfo2>* entryInfo2List = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(1);
	CleanupStack::PushL(entryInfo2List);
	
	//creates array of TScheduleEntryInfo2's from array of TScheduleEntryInfo's.
	//assumes local time for backwards compatibility.
	ConvertArrayToEntryInfo2L(aEntryList, *entryInfo2List);
	
	if (ScheduleEntriesAreBad(*entryInfo2List))
		return KErrArgument;
		
	TPtrC8 pA((TUint8*) &entryInfo2List->At(0), sizeof(TScheduleEntryInfo2)*count);
	TInt err = SendReceive(EEditTimeSchedule, TIpcArgs(count, aScheduleHandle, &pA));
	CleanupStack::PopAndDestroy(entryInfo2List);
	return err;
	}

/**  
Changes a condition based schedule.

Note that changing a schedule is implemented by supplying a replacement set 
of schedule conditons and time.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aScheduleHandle The Id that identifies the schedule.
@param aConditions An array of system agent conditons 
that make up the schedule.
@param aDefaultRunTime The time at which the schedule with run if no
conditions are met. Note that this parameter is assumed to be local time based.
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrArgument if the schedule is not a condition based one
or if the publish and subscribe variables, defined by aConditions category 
and key values, do not exist or are not of integral type, 
KErrPermissionDenied if the client does not have the same SID as the schedules
creator or has WriteDeviceData capability, KErrServerBusy, if a backup or 
restore operation is taking place or any of the other system wide error 
codes.

@internalAll
@deprecated See note in CSch_Cli.h
*/	
EXPORT_C TInt RScheduler::EditSchedule(TInt aScheduleHandle, 
						const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
						const TTime& aDefaultRunTime)
	{
	TInt count = aConditions.Count();
	TPtrC8 pA((TUint8*) &aConditions.At(0), sizeof(TTaskSchedulerCondition)*count);
	
	// create TTsTime object from aDefaultRunTime. 
	// aDefaultRunTime is assumed to be local time based for backwards compatibility
	TTsTime defaultRunTime(aDefaultRunTime, EFalse);	
	//write Time
	TPckgC<TTsTime> pTime(defaultRunTime);
	return SendReceive(EEditConditionSchedule, TIpcArgs(count, aScheduleHandle, &pA, &pTime));	
	}


/** 
Creates a new, transient, time based schedule and adds a task to it.

Note that a transient schedule is destroyed when the task is destroyed or 
power is lost.

@param aTaskInfo Information about the task to be added to the transient schedule. 
On return the task Id is written into this class.
@param aTaskData Data that is passed to the task on execution.
@param aRef Definition of the new transient schedule.
@param aEntryList The set of schedule entries that make up the new transient 
schedule. All entry start times are assumed to be local time based.
@return KErrNone, if successful, otherwise one of the other system wide error 
codes.
@panic CTaskScheduler 0 The client has not registered. The client must register 
before adding tasks to the schedule.
@see TScheduleEntryInfo 
@deprecated See note in CSch_Cli.h
*/
EXPORT_C TInt RScheduler::ScheduleTask(TTaskInfo& aTaskInfo, 
									HBufC& aTaskData, 
									TSchedulerItemRef& aRef,
									const CArrayFixFlat<TScheduleEntryInfo>& aEntryList)
	{
	TScheduleSettings2 settings;
	settings.iPersists = EFalse;
	settings.iEntryCount = aEntryList.Count();
	settings.iName = aRef.iName;
	
	CArrayFixFlat<TScheduleEntryInfo2>* entryInfo2List = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(1);
	CleanupStack::PushL(entryInfo2List);

	//creates array of TScheduleEntryInfo2's from array of TScheduleEntryInfo's.
	//assumes local time for backwards compatibility.
	ConvertArrayToEntryInfo2L(aEntryList, *entryInfo2List);

	TInt res = CreateSchedule(aRef, *entryInfo2List, settings);
	CleanupStack::PopAndDestroy(entryInfo2List);
	
	if (res==KErrNone)
		{
		res = ScheduleTask(aTaskInfo, aTaskData, aRef.iHandle);
		//the schedule here needs a task: so if we can't schedule a task,
		//need to delete schedule
		if (res!=KErrNone)
			DeleteSchedule(aRef.iHandle);
		}
	return res;
	}


/** 
Creates a new, transient, condition based schedule and adds a task to it.

Note that a transient schedule is destroyed when the task is destroyed or
power is lost.

@param aTaskInfo Information about the task to be added to the transient schedule. 
On return the task Id is written into this class.
@param aTaskData Data that is passed to the task on execution.
@param aRef Definition of the new transient schedule.
@param aConditions An array of schedule conditons that make up the schedule.
@param aDefaultRunTime The time at which the schedule with run if no
conditions are met. aDefaultRunTime is assumed to be local time based.
@return KErrNone, if successful, KErrArgument if the publish and subscribe 
variables, defined by aConditions category and key values, do not exist or 
are not of integral type, otherwise one of the other system wide error codes.
@panic CTaskScheduler 0 The client has not registered. The client must register 
before adding tasks to the schedule.

@internalAll
@deprecated See note in CSch_Cli.h
*/
EXPORT_C TInt RScheduler::ScheduleTask(TTaskInfo& aTaskInfo, 
								HBufC& aTaskData, 
								TSchedulerItemRef& aRef, 
								const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
								const TTime& aDefaultRunTime)
	{
	// create schedule settings object
	TScheduleSettings2 settings;
	settings.iPersists = EFalse;
	settings.iEntryCount = aConditions.Count();
	settings.iName = aRef.iName;
	
	// create TTsTime object from aDefaultRunTime.
	// assumes aDefaultRunTime is local time based for backwards compatibility
	TTsTime defaultRunTime(aDefaultRunTime, EFalse);
	
	TInt res = CreateSchedule(aRef, aConditions, defaultRunTime, settings);
	if (res==KErrNone)
		{
		res = ScheduleTask(aTaskInfo, aTaskData, aRef.iHandle);
		//the schedule here needs a task: so if we can't schedule a task,
		//need to delete schedule
		if (res!=KErrNone)
			DeleteSchedule(aRef.iHandle);
		}
	return res;
	}


/** 
Gets information relating to a specified time based schedule.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aScheduleHandle The Id that identifies the schedule.
@param aState On return, the state of the specified schedule.
On return, aState will have a local time based iDueTime member, regardless 
of whether the schedule is UTC or local time based. If the schedule is UTC 
based, the due time will be converted to a local time based value before returning.
@param aEntries On return, a populated array of schedule entries that make 
up the schedule. All entry start times returned will be local time based, 
though they may be UTC or local time based within the scheduler. The scheduler will
convert any UTC based times to local time before returning.
Note that populating the array could cause this function 
to leave because of an out of memory condition.
@param aTasks On return, a populated array of tasks associated with the schedule. 
Note that populating the array could cause this function to leave because 
of an out of memory condition.
@param aDueTime On return, the time that the schedule is next due. This value
will be local time based, regardless of whether the schedule is UTC or local 
time based. If the schedule is UTC based, the due time will be converted to 
a local time based value before returning.
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrArgument if the schedule is not a time based one, 
KErrPermissionDenied if the client does not have the same SID as the schedules
creator or has WriteDeviceData capability, or any of the other system wide error 
codes.
@see TScheduleEntryInfo
@see TTaskInfo 
@deprecated See note in CSch_Cli.h
*/
EXPORT_C TInt RScheduler::GetScheduleL(const TInt aScheduleHandle, 
									TScheduleState& aState, 
									CArrayFixFlat<TScheduleEntryInfo>& aEntries, 
									CArrayFixFlat<TTaskInfo>& aTasks, 
									TTime& aDueTime)
	{
	// set up vars required->scheduleState, entries, and time
	TTsTime dueTime;
	TScheduleState2 state;
	CArrayFixFlat<TScheduleEntryInfo2>* entryInfo2List = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(1);
	CleanupStack::PushL(entryInfo2List);
	
	// call new API	
	TInt res = GetScheduleL(aScheduleHandle,
							state,
							*entryInfo2List,
							aTasks, 
							dueTime);
							
	// convert vars back to old versions
	// use local time for backwards compatibility
	aDueTime = dueTime.GetLocalTime();	
	aState = TScheduleState(state);
	ConvertArrayFromEntryInfo2L(*entryInfo2List, aEntries);
			
	CleanupStack::PopAndDestroy(entryInfo2List);
	return res;
	
	}

/** 
Gets information relating to a specified condition based schedule.

@capability Note Only clients with the same SID as the schedule creator, or 
WriteDeviceData capability can sucessfully call this API.

@param aScheduleHandle The Id that identifies the schedule.
@param aState On return, the state of the specified schedule.
On return, aState will have a local time based iDueTime member, regardless 
of whether the schedule is UTC or local time based. If the schedule is UTC 
based, the due time will be converted to a local time based value before returning.
@param aConditions On return, a populated array of schedule conditons 
that make up the schedule. Note that populating the array could cause 
this function to leave because of an out of memory condition.
@param aDefaultRunTime On return, the time at which the schedule with run if
no conditions are met. This value will be local time based, regardless of 
whether the schedule is UTC or local time based. If the schedule is UTC based, 
the due time will be converted to a local time based value before returning.
@param aTasks On return, a populated array of tasks associated with the schedule. 
Note that populating the array could cause this function to leave because 
of an out of memory condition.
@return KErrNone if successful, KErrNotFound if there is no schedule with 
the specified Id, KErrArgument if the schedule is not a condition based one, 
KErrPermissionDenied if the client does not have the same SID as the schedules
creator or has WriteDeviceData capability, or any of the other system wide error 
codes.
@see TTaskSchedulerCondition
@see TTaskInfo 

@internalAll
@deprecated See note in CSch_Cli.h
*/
EXPORT_C TInt RScheduler::GetScheduleL(TInt aScheduleHandle, 
							TScheduleState& aState, 
							CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							TTime& aDefaultRunTime,
							CArrayFixFlat<TTaskInfo>& aTasks)
	{
	TScheduleInfo info;
	TTsTime dummyTime;
	TInt res = GetScheduleInfo(aScheduleHandle, info, dummyTime);
	if (res != KErrNone)
		return res;
	aState = TScheduleState(info.iState);
	TTsTime defaultRunTime;
	res = GetScheduleDataL(aScheduleHandle, 
							info, 
							aConditions, 
							defaultRunTime);
							
	//uses local time for backwards compatibility							
	aDefaultRunTime = defaultRunTime.GetLocalTime();
	
	if (res != KErrNone)
		return res;
	res = GetTaskDataL(aScheduleHandle, info, aTasks);							
	return res;
	}
 
	
/** 
Gets information relating to a specified task.

@capability Note Only clients with the same SID as the relevant schedule
creator, or WriteDeviceData capability can sucessfully call this API.

@param aTaskId The Id that identifies the task.
@param aTaskInfo On return, information about the task.
@param aTaskData On return, a pointer descriptor representing the data that 
is passed to the program to be executed. The caller must set up this pointer 
descriptor before calling the function. The required length of the descriptor 
can be found by calling GetTaskDataSize().
@param aRef On return, the associated schedule definition.
@param aNextDue On return, the time that the task is next due. This value
will be local time based, regardless of whether the schedule is UTC or local 
time based. If the schedule is UTC based, the due time will be converted to 
a local time based value before returning.
@return KErrNone, if successful. KErrNotFound, if there is no task with the 
specified Id, KErrPermissionDenied if the client does not have the same SID as 
the schedules creator or has WriteDeviceData capability. 
@deprecated See note in CSch_Cli.h
*/
EXPORT_C TInt RScheduler::GetTaskInfoL(const TInt aTaskId, TTaskInfo& aTaskInfo, TPtr& aTaskData, TSchedulerItemRef& aRef, TTime& aNextDue)
	{
	// First of all retrieve the normal stuff
	TPtr8 pInfo((TUint8*)&aTaskInfo,sizeof(TTaskInfo));
	TInt size = aTaskData.MaxLength();
	TInt res = SendReceive(EGetTask, TIpcArgs(aTaskId, &pInfo, size, &aTaskData));
	if (res != KErrNone)
		return res;

	// Next retrieve the TSchedulerItemRef and next due time
	TTsTime nextDueTime;
	TPtr8 pItemRef((TUint8*)&aRef, sizeof(TSchedulerItemRef));
	TPtr8 pDueTime((TUint8*)&nextDueTime, sizeof(TTsTime));
	res = SendReceive(EGetSchedulerItemRefAndNextDueTime, TIpcArgs(aTaskId, &pItemRef, &pDueTime));
	
	// use local time for backwards compatibility
	aNextDue = nextDueTime.GetLocalTime();
	return res;
	}


//private
// TScheduleInfo.iEntryCount is number of schedule entries for time schedules
// and number of conditions for condition based schedules.
TInt RScheduler::GetScheduleInfo(const TInt aScheduleHandle, TScheduleInfo& aInfo, TTsTime& aNextDue)
	{	
	TPtr8 pInfo((TUint8*)&aInfo,sizeof(TScheduleInfo));//scheduler writes back info
	TPtr8 pDueTime((TUint8*)&aNextDue,sizeof(TTsTime));//scheduler writes back dueTime
	// get info for number of entries & tasks
	TInt res = SendReceive(EGetScheduleInfo, TIpcArgs(aScheduleHandle, &pInfo, &pDueTime));
	return res;
	}
	
TInt RScheduler::GetTaskDataL(const TInt aScheduleHandle, 
								const TScheduleInfo& aInfo,
								CArrayFixFlat<TTaskInfo>& aTasks)
	{
	// resize array
	aTasks.ResizeL(aInfo.iTaskCount);

	// get entries & taskrefs & flags
	TPBScheduleInfo pckgInfo(aInfo);
	TInt res = KErrNone;
	if (aInfo.iTaskCount > 0)
		{
		TPtr8 pTasks((TUint8*)&(aTasks.At(0)), aInfo.iTaskCount*sizeof(TTaskInfo));
		res = SendReceive(EGetTaskData, TIpcArgs(aScheduleHandle, &pckgInfo, &pTasks));
		}
	return res;
	}

TInt RScheduler::GetScheduleDataL(const TInt aScheduleHandle, 
								const TScheduleInfo& aInfo,
								CArrayFixFlat<TScheduleEntryInfo2>& aEntries)
	{
	// resize array	
	aEntries.ResizeL(aInfo.iEntryCount);

	// get entries
	TPBScheduleInfo pckgInfo(aInfo);
	TPtr8 pEntries((TUint8*)&(aEntries.At(0)), aInfo.iEntryCount*sizeof(TScheduleEntryInfo2));
	return SendReceive(EGetTimeScheduleData, TIpcArgs(aScheduleHandle, &pckgInfo, &pEntries));
	}
	
TInt RScheduler::GetScheduleDataL(const TInt aScheduleHandle, 
								const TScheduleInfo& aInfo,
								CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
								TTsTime& aDefaultRunTime)
	{
	// resize arrays	
	aConditions.ResizeL(aInfo.iEntryCount);

	// get entries
	TPtr8 pDefaultTime((TUint8*)&aDefaultRunTime,sizeof(TTsTime));//scheduler writes back defaultTime
	TPBScheduleInfo pckgInfo(aInfo);
	TPtr8 pEntries((TUint8*)&(aConditions.At(0)), aInfo.iEntryCount*sizeof(TTaskSchedulerCondition));
	return SendReceive(EGetConditionScheduleData, TIpcArgs(aScheduleHandle, &pckgInfo, &pEntries, &pDefaultTime));
	}	
	

#if defined (_DEBUG)
/** 
Marks the start of checking the current thread's heap.

This is used for debugging, and is only implemented in debug builds.

@return In debug builds, KErrNone, if successful, otherwise one of the other 
system wide error codes. In release builds, KErrNone always.
@see __UHEAP_MARK 
@internalComponent
*/
EXPORT_C TInt RScheduler::__DbgMarkHeap()
	{
	return SendReceive(ESchDbgMarkHeap);
	}

/** 
Checks that the number of allocated cells on the current thread's heap is the 
same as the specified value.

This is used for debugging, and is only implemented in debug builds.

@param aCount In debug builds, the number of heap cells expected to be allocated. 
In release builds, this parameter is not used.
@return In debug builds, KErrNone, if successful, otherwise one of the other 
system wide error codes. In release builds, KErrNone always.
@see __UHEAP_CHECK 
@internalComponent
*/
EXPORT_C TInt RScheduler::__DbgCheckHeap(TInt aCount)
	{
	return SendReceive(ESchDbgCheckHeap,TIpcArgs(aCount));
	}

/** 
Marks the end of checking the current thread's heap.

This is used for debugging, and is only implemented in debug builds.

@param aCount In debug builds, the number of heap cells expected to remain 
allocated. In release builds, this parameter is not used.
@return In debug builds, KErrNone, if successful, otherwise one of the other 
system wide error codes. In release builds, KErrNone always.
@see __UHEAP_MARKENDC 
@internalComponent
*/
EXPORT_C TInt RScheduler::__DbgMarkEnd(TInt aCount)
	{
	return SendReceive(ESchDbgMarkEnd,TIpcArgs(aCount));
	}

/** 
Simulates heap allocation failure.

This is used for debugging, and is only implemented in debug builds.

@param aCount In debug builds, the rate of failure - heap allocation fails 
every aCount attempts. In release builds, this parameter is not used.
@return In debug builds, KErrNone, if successful, otherwise one of the other 
system wide error codes. In release builds, KErrNone always.
@see __UHEAP_FAILNEXT 
@internalComponent
*/
EXPORT_C TInt RScheduler::__DbgFailNext(TInt aCount)
	{
	return SendReceive(ESchDbgFailNext,TIpcArgs(aCount));
	}

/** 
Cancels simulated heap allocation failure for the current thread's heap.

This is used for debugging, and is only implemented in debug builds.

@return In debug builds, KErrNone, if successful, otherwise one of the other 
system wide error codes. In release builds, KErrNone always.
@see __UHEAP_RESET 
@internalComponent
*/
EXPORT_C TInt RScheduler::__DbgResetHeap()
	{
	return SendReceive(ESchDbgResetHeap);
	}


/** 
It tries to kill the server (schsvr).

This is used for debugging, and is only implemented in debug builds.

@return In debug builds, KErrNone, if successful, otherwise one of the other 
system wide error codes. In release builds, KErrNone always. 
@internalComponent
*/
EXPORT_C TInt RScheduler::__FaultServer()
	{
	return SendReceive(ESchFaultServer);
	}

#else
//release build exports empty versions of these for rel/deb compatibility
EXPORT_C TInt RScheduler::__DbgMarkHeap()
	{
	return KErrNone;
	}

EXPORT_C TInt RScheduler::__DbgCheckHeap(TInt /*aCount*/)
	{
	return KErrNone;
	}

EXPORT_C TInt RScheduler::__DbgMarkEnd(TInt /*aCount*/)
	{
	return KErrNone;
	}

EXPORT_C TInt RScheduler::__DbgFailNext(TInt /*aCount*/)
	{
	return KErrNone;
	}

EXPORT_C TInt RScheduler::__DbgResetHeap()
	{
	return KErrNone;
	}

EXPORT_C TInt RScheduler::__FaultServer()
	{
	return KErrNone;
	}

#endif // _DEBUG





