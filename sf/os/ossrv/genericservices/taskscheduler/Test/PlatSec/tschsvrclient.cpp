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
// tschsvrsclient1.cpp
// 
//

#include <e32base.h>
#include <e32debug.h>

#include <csch_cli.h>
#include <schinfointernal.h>
#include "Thelpers.h"
#include "TestUtils.h"

_LIT(KTestName,	"Client with different UID test");
RTest TheTest(KTestName);
static RScheduler	TheScheduler;


typedef CArrayFixFlat<TScheduleEntryInfo>	CSchEntryInfoArray;
typedef CArrayFixFlat<TTaskInfo>			CTaskInfoArray;
typedef CArrayFixFlat<TSchedulerItemRef>    CSchItemRefArray;
typedef CArrayFixFlat<TTaskSchedulerCondition>	CSchConditionArray;


static CSchEntryInfoArray* CreateScheduleArrayLC()
	{
	CSchEntryInfoArray* entryList = new (ELeave) CSchEntryInfoArray(3);
	CleanupStack::PushL(entryList);

	TScheduleEntryInfo entry1;
	entry1.iStartTime		= SchSvrHelpers::TimeBasedOnOffset(0, 20); // 20m from "now"
	entry1.iInterval		= 1;
	entry1.iIntervalType	= EDaily;
	entry1.iValidityPeriod	= 20;
	entryList->AppendL(entry1);
	
	return entryList;
	}

static CSchConditionArray* CreateSingleConditionLC()
	{
	CSchConditionArray* conditionList = new (ELeave) CSchConditionArray(3);
	CleanupStack::PushL(conditionList);

	const TUid KTestUid = TUid::Uid(0x12345678);
	
	TTaskSchedulerCondition condition1;
	condition1.iCategory = KTestUid;
	condition1.iKey		= 1;
	condition1.iState	= 10;
	condition1.iType	= TTaskSchedulerCondition::EEquals;
	conditionList->AppendL(condition1);
	
	return conditionList;			
	}	
/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-1343
@SYMTestCaseDesc	    Client with different UID test
@SYMTestPriority 	    High
@SYMTestActions  	    Connect to Scheduler,
                        Tests RScheduler::EditSchedule(),RScheduler::DisableSchedule(),RScheduler::EnableSchedule()
						RScheduler::ScheduleTask(),RScheduler::GetScheduleRefsL(),RScheduler::GetScheduleL(),
						RScheduler::GetTaskRefsL(),RScheduler::GetTaskDataSize(),RScheduler::GetTaskInfoL(),
						RScheduler::GetScheduleTypeL(),RScheduler::DeleteTask(),RScheduler::DeleteSchedule() functions
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/			
void DoExecuteL(TInt aScheduleHandle, 
				TInt aTaskHandle, 
				TInt aExpectedError,
				TInt aExpectedScheduleCount,
				TInt aExpectedTaskCount)
	{
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-1343 Connect to Scheduler "));
	TInt res = TheScheduler.Connect();
	TEST2(res, KErrNone);
	TheTest.Next(_L("Registering Client"));
	TEST2(SchSvrHelpers::RegisterClientL(TheScheduler), KErrNone);

	//Edit Schedule
	TheTest.Next(_L("Testing EditSchedule"));
	CSchEntryInfoArray* scheduleArray = CreateScheduleArrayLC();
	res = TheScheduler.EditSchedule(aScheduleHandle, *scheduleArray);
	CleanupStack::PopAndDestroy(); //scheduleArray
	TEST2(res, aExpectedError);
	//condition API -  only check if we are expecting an error
	if(aExpectedError != KErrNone)
		{
		CSchConditionArray* conditionList = CreateSingleConditionLC();
		TTime time = SchSvrHelpers::TimeBasedOnOffset(0, 0, 0, 0, 0, 1); //1 year in the future
		res = TheScheduler.EditSchedule(aScheduleHandle, *conditionList, time);
		CleanupStack::PopAndDestroy(); // conditionList
		TEST2(res, aExpectedError);
		}
	//Disable/Enable Schedule
	TheTest.Next(_L("Testing DisableSchedule and EnableSchedule"));
	res = TheScheduler.DisableSchedule(aScheduleHandle);
	TEST2(res, aExpectedError);
	res = TheScheduler.EnableSchedule(aScheduleHandle);
	TEST2(res, aExpectedError);
	
	//ScheduleTask
	TheTest.Next(_L("Testing ScheduleTask"));
	TTaskInfo taskInfo;
	taskInfo.iName = _L("MyTask");
	taskInfo.iPriority = 2;
	taskInfo.iTaskId = 0;
	taskInfo.iRepeat = 1;
	HBufC* data = _L("Task Data I cant schedule").AllocLC();
	res = TheScheduler.ScheduleTask(taskInfo, *data, aScheduleHandle);
	TEST2(res, aExpectedError);
	CleanupStack::PopAndDestroy(); // data
	
	if(res == KErrNone)
		{
		res = TheScheduler.DeleteTask(taskInfo.iTaskId);
		TEST2(res, KErrNone);
		}
		
	//GetScheduleRefs
	TheTest.Next(_L("Testing GetScheduleRefsL"));
	CSchItemRefArray* refs = new (ELeave) CSchItemRefArray(3);
	CleanupStack::PushL(refs);
	res = TheScheduler.GetScheduleRefsL(*refs, EAllSchedules); 
	TEST2(res, KErrNone);
	TInt count = refs->Count();
	TEST2(count, aExpectedScheduleCount); 
	
	//GetSchedule
	TheTest.Next(_L("Testing GetScheduleL"));
	CSchEntryInfoArray* entries = new (ELeave) CSchEntryInfoArray(3);
	CleanupStack::PushL(entries);
	CTaskInfoArray* tasks = new (ELeave) CTaskInfoArray(3);
	CleanupStack::PushL(tasks);
	
	TTime nextTimeScheduleIsDue;
	TScheduleState state;
	res = TheScheduler.GetScheduleL(aScheduleHandle, 
										state, 
										*entries, 
										*tasks, 
										nextTimeScheduleIsDue);
	TEST2(res, aExpectedError);
	//condition API -  only check if we are expecting an error
	if(aExpectedError != KErrNone)
		{
		CSchConditionArray* conditions = new (ELeave) CSchConditionArray(3);
		CleanupStack::PushL(conditions);
		res = TheScheduler.GetScheduleL(aScheduleHandle, 
											state,
											*conditions, 
											nextTimeScheduleIsDue, 
											*tasks);
		CleanupStack::PopAndDestroy(conditions);									
		TEST2(res, aExpectedError);
		}
	
	//GetTaskRefs
	TheTest.Next(_L("Testing GetScheduleL"));
	res = TheScheduler.GetTaskRefsL(*refs, EAllSchedules, EAllTasks);
	TEST2(res, KErrNone);
	count = refs->Count();
	TEST2(count, aExpectedTaskCount); 
	
	// TaskDataSize
	TheTest.Next(_L("Testing GetTaskDataSize"));
	TInt taskSize = 0;
	res = TheScheduler.GetTaskDataSize(aTaskHandle, taskSize);
	TEST2(res, aExpectedError);
	
	// GetTaskInfo
	TheTest.Next(_L("Testing GetTaskInfoL"));
	if(aExpectedError != KErrNone)
		//use dummy size for task as we actually didnt get it from above
		// if we were expected as error
		taskSize = 10; 
	HBufC* taskData = HBufC::NewLC(taskSize);
	TPtr pTaskData = taskData->Des();

	TTime scheduleNextDueTime;
	TTaskInfo taskFromServer;
	TSchedulerItemRef scheduleReference;

	res = TheScheduler.GetTaskInfoL(aTaskHandle,
										taskFromServer, 
									  	pTaskData, 
										scheduleReference, 
										scheduleNextDueTime);
	CleanupStack::PopAndDestroy(taskData);
	TEST2(res, aExpectedError);
	
	TScheduleType scheduleType;
	res = TheScheduler.GetScheduleTypeL(aScheduleHandle, scheduleType);
	TEST2(res, aExpectedError);

	//DeleteTask
	TheTest.Next(_L("Testing TaskSchedule"));
	res = TheScheduler.DeleteTask(aTaskHandle);
	TEST2(res, aExpectedError);

	//Delete Schedule
	TheTest.Next(_L("Testing DeleteSchedule"));
	res = TheScheduler.DeleteSchedule(aScheduleHandle);
	TEST2(res, aExpectedError);
											
	CleanupStack::PopAndDestroy(tasks);
	CleanupStack::PopAndDestroy(entries);
	CleanupStack::PopAndDestroy(refs);
	
	TheScheduler.Close();
	
	}

LOCAL_D TInt Execute(TInt aScheduleHandle, 
					TInt aTaskHandle, 
					TInt aExpectedError,
					TInt aExpectedScheduleCount,
					TInt aExpectedTaskCount)
	{
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup=CTrapCleanup::New();	//can fail
	if (cleanup)
		{
		
		TheTest.Start(_L("TSchSvrClient"));
		TheTest.Title();
		err = KErrNone;
		TRAP(err, DoExecuteL(aScheduleHandle, 
							aTaskHandle, 
							aExpectedError,
							aExpectedScheduleCount,
							aExpectedTaskCount))
		delete cleanup;

		TheTest.End();
		TheTest.Close();

		}
	return err;
	}


//***********************************************************************************
static TInt Str2Int(const TDesC& aStr)
	{
	TLex16 lex(aStr);
	TInt val = 0;
	lex.Val(val);
	return val;
	}

static TBuf<20> ExtractInt(const TDesC& aBuf, TInt& aExtractedInt)
	{
	TBuf<20> buf = aBuf.Right(aBuf.Length() - aBuf.Locate(':')-1);
	TBuf<5> handle;
	handle.FillZ();
	handle = buf.Left(buf.Locate(':'));
	aExtractedInt = Str2Int(handle);
	return buf;
	}
	
GLDEF_C TInt E32Main()
	{
	TBuf<30> cmd;
	User::CommandLine(cmd);
	// schedule Handle is first
	TInt scheduleHandle=0, taskHandle=0, errCode=0, schCount=0, taskCount=0;
	cmd = ExtractInt(cmd, scheduleHandle);
	// task Handle is second
	cmd = ExtractInt(cmd, taskHandle);
	// expected error code is third
	cmd = ExtractInt(cmd, errCode);
	// expected schedule count (based on this exe UID/Capability is fourth
	cmd = ExtractInt(cmd, schCount);
	// expected task count (based on this exe UID/Capability is fourth
	cmd = ExtractInt(cmd, taskCount);
	
	return Execute(scheduleHandle, taskHandle, errCode, schCount, taskCount);
	}
	

