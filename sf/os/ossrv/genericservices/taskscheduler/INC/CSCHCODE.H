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
// Scheduler opcodes
// 
//

#if !defined (__CSCHCODE_H__)
#define __CSCHCODE_H__

#include <schinfo.h>
enum TSchMess
	{//opcodes
	ERegisterClient,
	ECreateTimeSchedule,
	ECreateConditionSchedule,
	EScheduleTask,
	ERemoveTask,
	EDisableSchedule,
	EEnableSchedule,
	EDeleteSchedule,
	EEditTimeSchedule,
	EEditConditionSchedule,
	EGetScheduleInfo,
	EGetTimeScheduleData,
	EGetConditionScheduleData,
	EGetTaskData,
	ECountSchedules,
	ECountTasks,
	EGetTaskRefs,
	EGetTask,
	EGetTaskDataSize,
	EGetScheduleRefs,
	EDeleteTask,
	EGetSchedulerItemRefAndNextDueTime,
	EGetScheduleType,

	//debug
	ESchDbgMarkHeap = 100,
	ESchDbgCheckHeap,
	ESchDbgMarkEnd,
	ESchDbgFailNext,
	ESchDbgResetHeap,
	ESchFaultServer, 
	ESchDbgSetTime
	};

class TScheduleSettings2
	{
public:
	TBool iPersists;
	TInt iEntryCount;
	TName iName;
	};

/**
@internalComponent
*/
class TScheduleInfo // Move to 
	{
public:
	TScheduleState2 iState;
	TInt iEntryCount;
	TInt iTaskCount;
	};

/**
@internalAll
@deprecated replaced with TScheduleSettings2
*/
class TScheduleSettings 
	{
public:
	TBool iPersists;
	TInt iEntryCount;
	};


	
// Forward Declarations	
class TTsTime;
class TTaskInfo;
class TSchedulerItemRef;
	
	// Typedefs for TPckg's used for IPC
typedef TPckg<TInt>					TPInt;
typedef TPckg<TTaskInfo>			TPTaskInfo;
typedef TPckg<TScheduleInfo>		TPScheduleInfo;
typedef TPckg<TScheduleSettings2>	TPScheduleSettings;
typedef TPckgBuf<TTsTime>				TPBTime;
typedef TPckgBuf<TTaskInfo>			TPBTaskInfo;
typedef TPckgBuf<TScheduleInfo>		TPBScheduleInfo;
typedef TPckgBuf<TSchedulerItemRef>	TPBSchedulerItemRef;

const TUint KESchSvrMajorVersionNumber=1;
const TUint KESchSvrMinorVersionNumber=0;
const TUint KESchSvrBuildVersionNumber=50;

_LIT(KSchSvrName, "!TaskScheduler");


#endif
