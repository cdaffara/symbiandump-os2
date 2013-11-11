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
// RScheduler class
// Client Interface to Scheduler
// 
//

#if !defined(__CSCH_CLI_H__)
#define __CSCH_CLI_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__SCHINFO_H__)
#include <schinfo.h>
#endif

#include <e32cons.h>


class TTaskSchedulerCondition;
class TScheduleSettings2;
class TScheduleInfo;


/** 
Constants
@publishedAll
@deprecated
*/
const TUint KESchMajorVersionNumber=1;
const TUint KESchMinorVersionNumber=0;
const TUint KESchBuildVersionNumber=50;



/**
Client side interface to the Task Scheduler.
@publishedAll	
@released
*/
class RScheduler : public RSessionBase
	{
public:
	IMPORT_C RScheduler();
	IMPORT_C TInt Connect();
	IMPORT_C TVersion Version() const;
	
	// client registration
	IMPORT_C TInt Register(const TFileName& aFileName,const TInt aPriority);
	
	// schedule editing
	IMPORT_C TInt CreatePersistentSchedule(TSchedulerItemRef& aRef, 
							const CArrayFixFlat<TScheduleEntryInfo2>& aEntryList);

	IMPORT_C TInt CreatePersistentSchedule(TSchedulerItemRef& aRef, 
							const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							const TTsTime& aDefaultRunTime);

	IMPORT_C TInt DeleteSchedule(const TInt aScheduleHandle) const;

	IMPORT_C TInt DisableSchedule(const TInt aScheduleHandle) const;

	IMPORT_C TInt EnableSchedule(const TInt aScheduleHandle) const;

	// This will return KErrArgument if the schedule is not a time based one.
	// (consistent behaviour with GetScheduleL below).
	IMPORT_C TInt EditSchedule(const TInt aScheduleHandle, 
							const CArrayFixFlat<TScheduleEntryInfo2>& aEntryList);

	// This will return KErrArgument if the schedule is not a condition based one.
	// (consistent behaviour with GetScheduleL below).
	IMPORT_C TInt EditSchedule(TInt aScheduleHandle, 
							const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							const TTsTime& aDefaultRunTime);	

	//task funtions
	// Schedule a task based on a persistent schedule.  Will return KErrArgument
	// if schedule is a condition based one and task repeat is not 0.
	IMPORT_C TInt ScheduleTask(TTaskInfo& aTaskInfo, 
							HBufC& aTaskData, 
							const TInt aScheduleHandle);

	//creates a transient schedule
	IMPORT_C TInt ScheduleTask(TTaskInfo& aTaskInfo, 
							HBufC& aTaskData, 
							TSchedulerItemRef& aRef,
							const CArrayFixFlat<TScheduleEntryInfo2>& aEntryList);

	// creates a transient schedule based on conditions.  Will return KErrArgument
	// if schedule is a condition based one and task repeat is not 0.
	IMPORT_C TInt ScheduleTask(TTaskInfo& aTaskInfo, 
							HBufC& aTaskData, 
							TSchedulerItemRef& aRef, 
							const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							const TTsTime& aDefaultRunTime);

	IMPORT_C TInt DeleteTask(const TInt aTaskId) const;
	
	//retrieving
	IMPORT_C TInt GetScheduleRefsL(CArrayFixFlat<TSchedulerItemRef>& aScheduleRefArray,
							const TScheduleFilter aFilter);

	// Gets time based schedule.  Will return KErrArgument if scheduleType 
	// is not time based.
	IMPORT_C TInt GetScheduleL(const TInt aScheduleHandle, 
							TScheduleState2& aState, 
							CArrayFixFlat<TScheduleEntryInfo2>& aEntries, 
							CArrayFixFlat<TTaskInfo>& aTasks, 
							TTsTime& aDueTime);

	// Gets condition based schedule.  Will return KErrArgument if scheduleType 
	// is not condition based. 
	IMPORT_C TInt GetScheduleL(TInt aScheduleHandle, 
							TScheduleState2& aState, 
							CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							TTsTime& aDefaultRunTime, 
							CArrayFixFlat<TTaskInfo>& aTasks);

	IMPORT_C TInt GetTaskRefsL(CArrayFixFlat<TSchedulerItemRef>& aTasks,
							const TScheduleFilter aScheduleFilter, 
							const TTaskFilter aTaskFilter);

	IMPORT_C TInt GetTaskInfoL(const TInt aTaskId, 
							TTaskInfo& aTaskInfo, 
							TPtr& aTaskData, 
							TSchedulerItemRef& aRef, 
							TTsTime& aNextDue);

	IMPORT_C TInt GetTaskDataSize(const TInt aTaskId, TInt& aSize);
	
	// Gets the scheduletype for a given schedule handle
	IMPORT_C TInt GetScheduleTypeL(TInt aScheduleHandle,
											TScheduleType& aScheduleType);
public:
	//deprecated API functions as of v9.2
	//clients of these APIs are encouraged to migrate to the equivalent APIs above.
	//see ../Documentation/Feature Documentation/SGL.GT0250.204 Task Scheduler Migration Guide to new APIs in Symbian OS v9.x.doc
	
	IMPORT_C TInt CreatePersistentSchedule(TSchedulerItemRef& aRef, 
							const CArrayFixFlat<TScheduleEntryInfo>& aEntryList);

	IMPORT_C TInt CreatePersistentSchedule(TSchedulerItemRef& aRef, 
							const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							const TTime& aDefaultRunTime);

	IMPORT_C TInt EditSchedule(const TInt aScheduleHandle, 
							const CArrayFixFlat<TScheduleEntryInfo>& aEntryList);

	IMPORT_C TInt EditSchedule(TInt aScheduleHandle, 
							const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							const TTime& aDefaultRunTime);	

	IMPORT_C TInt ScheduleTask(TTaskInfo& aTaskInfo, 
							HBufC& aTaskData, 
							TSchedulerItemRef& aRef,
							const CArrayFixFlat<TScheduleEntryInfo>& aEntryList);

	IMPORT_C TInt ScheduleTask(TTaskInfo& aTaskInfo, 
							HBufC& aTaskData, 
							TSchedulerItemRef& aRef, 
							const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							const TTime& aDefaultRunTime);
	
	IMPORT_C TInt GetScheduleL(const TInt aScheduleHandle, 
							TScheduleState& aState, 
							CArrayFixFlat<TScheduleEntryInfo>& aEntries, 
							CArrayFixFlat<TTaskInfo>& aTasks, 
							TTime& aDueTime);

	IMPORT_C TInt GetScheduleL(TInt aScheduleHandle, 
							TScheduleState& aState, 
							CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
							TTime& aDefaultRunTime, 
							CArrayFixFlat<TTaskInfo>& aTasks);

	IMPORT_C TInt GetTaskInfoL(const TInt aTaskId, 
							TTaskInfo& aTaskInfo, 
							TPtr& aTaskData, 
							TSchedulerItemRef& aRef, 
							TTime& aNextDue);
	
public://debug functions.
	//these functions are included in rel for rel/deb compatibility, 
	//but only do stuff in debug builds
	IMPORT_C TInt __DbgMarkHeap();
	IMPORT_C TInt __DbgCheckHeap(TInt aCount);
	IMPORT_C TInt __DbgMarkEnd(TInt aCount);
	IMPORT_C TInt __DbgFailNext(TInt aCount);
	IMPORT_C TInt __DbgResetHeap();
	IMPORT_C TInt __FaultServer();//kill the server!

private:
	TInt CreateSchedule(TSchedulerItemRef& aRef, 
						const CArrayFixFlat<TScheduleEntryInfo2>& aEntryList, 
						const TScheduleSettings2& aSettings);
	TInt CreateSchedule(TSchedulerItemRef& aRef, 
						const CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
						const TTsTime& aDefaultRunTime, 
						const TScheduleSettings2& aSettings);
	TInt GetScheduleInfo(const TInt aScheduleHandle, 
						TScheduleInfo& aInfo, 
						TTsTime& aNextDue);
	TInt GetScheduleDataL(const TInt aScheduleHandle, 
						const TScheduleInfo& aInfo,
						CArrayFixFlat<TScheduleEntryInfo2>& aEntries);
	TInt GetScheduleDataL(const TInt aScheduleHandle, 
						const TScheduleInfo& aInfo,
						CArrayFixFlat<TTaskSchedulerCondition>& aConditions,
						TTsTime& aDefaultRunTime);
	TInt GetTaskDataL(const TInt aScheduleHandle, 
						const TScheduleInfo& aInfo,
						CArrayFixFlat<TTaskInfo>& aTasks);					
	};

#endif
