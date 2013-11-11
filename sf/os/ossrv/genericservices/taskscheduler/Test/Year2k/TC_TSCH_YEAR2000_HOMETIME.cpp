// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32test.h>
#include <csch_cli.h>
#include <f32file.h>
#include "Thelpers.h"
#include "TestUtils.h"

RTest TheTest(_L("TC_TSCH_YEAR2000 - hometime"));
LOCAL_D RFs TheFsSession;

LOCAL_D RScheduler TheScheduler;

_LIT(KMinimalTaskHandler, "MinimalTaskHandler");

LOCAL_D void SetTimeTo1SecBefore(TTsTime& aTime)
	{
	// TTsTime class can return UTC or Local time values.
	// to ensure these are calculated based on the correct offset,
	// use GetLocalTime() for local time based TTsTime values, 
	// and GetUtcTime() for UTC based TTsTime values.		
 	
	TTime time;
	TTimeIntervalSeconds secs(1);
	TBuf<30> dateString;
	
	// If time is UTC based, use UTC apis. If time is local time based, use local time APIs.
	if(aTime.IsUtc())
		{
		time = aTime.GetUtcTime() - secs;
		SchSvrHelpers::SetUTCTimeL(time);
		
		time.FormatL(dateString,(_L("%H%:1%T:%S %*E%*D %X%*N%Y %1 %2 %3")));
		TheTest.Printf(_L("current UTC time: %S\n"), &dateString);
		}
	else	// due time is local time based
		{
		time = aTime.GetLocalTime() - secs;
		SchSvrHelpers::SetHomeTimeL(time);
		
		time.FormatL(dateString,(_L("%H%:1%T:%S %*E%*D %X%*N%Y %1 %2 %3")));
		TheTest.Printf(_L("current local time: %S\n"), &dateString);
		}
	}

LOCAL_D void SetTime(TDateTime& aDateTime)
	{
	TTime time(aDateTime);	
	SchSvrHelpers::SetHomeTimeL(time);
	}

LOCAL_D TTsTime ShowDueTime(TInt aScheduleId)
	{
	TScheduleState2 state;
	TTsTime dueTime;
	CArrayFixFlat<TScheduleEntryInfo2>* entries = new CArrayFixFlat<TScheduleEntryInfo2> (3);
	CArrayFixFlat<TTaskInfo>* tasks = new CArrayFixFlat<TTaskInfo> (3);
	TInt res = TheScheduler.GetScheduleL(aScheduleId, state, *entries, *tasks, dueTime);
	TEST2(res, KErrNone);

	delete tasks;
	delete entries;

	TBuf<30> dateString;
	state.DueTime().GetLocalTime().FormatL(dateString,(_L("%H%:1%T%*E%*D%X%*N%Y %1 %2 %3")));
	TheTest.Printf(_L("due at:%S\n"), &dateString);
	return state.DueTime();
	}

LOCAL_D void AppendHourlyEntry(CArrayFixFlat<TScheduleEntryInfo2>& aEntries, TInt aInterval,TInt aYear,
							  TMonth aMonth, TInt aDay, TInt aHour, TInt aMinute)
	{
	TTime ttime(TDateTime(aYear, aMonth, aDay, aHour, aMinute, 0,0));
	TTsTime startTime (ttime, EFalse);
	TScheduleEntryInfo2 entry1 (startTime, EHourly, aInterval, 0);
	aEntries.AppendL(entry1);
	}

LOCAL_D void AppendDailyEntry(CArrayFixFlat<TScheduleEntryInfo2>& aEntries, TInt aInterval,TInt aYear,
							  TMonth aMonth, TInt aDay, TInt aHour, TInt aMinute)
	{
	TTime ttime(TDateTime(aYear, aMonth, aDay, aHour, aMinute, 0,0));
	TTsTime startTime (ttime,EFalse);
	TScheduleEntryInfo2 entry1 (startTime, EDaily, aInterval, 0);
	aEntries.AppendL(entry1);
	}

LOCAL_D void AppendMonthlyEntry(CArrayFixFlat<TScheduleEntryInfo2>& aEntries, TInt aInterval,
				TInt aYear, TMonth aMonth, TInt aDate, TInt aHour,TInt aMinute)
	{
	TTime ttime(TDateTime(aYear, aMonth, aDate, aHour, aMinute, 0,0));
	TTsTime startTime (ttime, EFalse);
	TScheduleEntryInfo2 entry1 (startTime, EMonthly, aInterval, 0);
	aEntries.AppendL(entry1);
	}

LOCAL_D void AppendYearlyEntry(CArrayFixFlat<TScheduleEntryInfo2>& aEntries, TInt aInterval,
				TInt aYear, TMonth aMonth,TInt aDate, TInt aHour, TInt aMinute)
	{
	TTime ttime(TDateTime(aYear, aMonth, aDate, aHour, aMinute, 0,0));
	TTsTime startTime (ttime,EFalse);
	TScheduleEntryInfo2 entry1 (startTime, EYearly, aInterval, 0);
	aEntries.AppendL(entry1);
	}


LOCAL_D TInt testCreateBoundarySchedule1(TSchedulerItemRef& aRef)
	{
	CArrayFixFlat<TScheduleEntryInfo2>* entryList;
	entryList = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(3);
	TName name(_L("and another off-peak"));
	aRef.iName = name;

//times post-boundary (to show boundary crossed properly)
	AppendYearlyEntry(*entryList, 10, 1999, EJanuary, 0, 0, 0);
	AppendYearlyEntry(*entryList, 10, 1999, EFebruary, 27, 0, 0);
	AppendYearlyEntry(*entryList, 10, 1999, EMarch, 0, 0, 0);
	AppendYearlyEntry(*entryList, 10, 1999, ESeptember, 0, 0, 0);
	AppendYearlyEntry(*entryList, 10, 1999, ESeptember, 8, 0, 0);
	AppendYearlyEntry(*entryList, 10, 1999, ESeptember, 9, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2000, EJanuary, 0, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2000, EFebruary, 27, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2000, EFebruary, 28, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2000, EMarch, 0, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2001, EJanuary, 0, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2001, EMarch, 0, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2004, EFebruary, 28, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2004, EMarch, 0, 0, 0);

	TInt res = TheScheduler.CreatePersistentSchedule(aRef, *entryList);
	TEST2(res, KErrNone);
	TInt count = entryList->Count();
	delete entryList;
	return count;
	}

LOCAL_D TInt testCreateBoundarySchedule2(TSchedulerItemRef& aRef)
	{
	CArrayFixFlat<TScheduleEntryInfo2>* entryList;
	entryList = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(3);
	TName name(_L("and another off-peak"));
	aRef.iName = name;

	//times on pre-boundary dates(to show they're recognized as valid)
	//commented-out lines are handled below
	AppendYearlyEntry(*entryList, 10, 1998, EDecember, 30, 0, 0);
	AppendYearlyEntry(*entryList, 10, 1999, EFebruary, 26, 0, 0);
//	AppendYearlyEntry(*entryList, 10, 1999, EFebruary, 27, 0, 0);
	AppendYearlyEntry(*entryList, 10, 1999, EAugust, 30, 0, 0);
	AppendYearlyEntry(*entryList, 10, 1999, ESeptember, 7, 0, 0);
//	AppendYearlyEntry(*entryList, 10, 1999, ESeptember, 8, 0, 0);
	AppendYearlyEntry(*entryList, 10, 1999, EDecember, 30, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2000, EFebruary, 26, 0, 0);
//	AppendYearlyEntry(*entryList, 10, 2000, EFebruary, 27, 0, 0);
//	AppendYearlyEntry(*entryList, 10, 2000, EFebruary, 28, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2000, EDecember, 30, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2001, EFebruary, 27, 0, 0);
	AppendYearlyEntry(*entryList, 10, 2004, EFebruary, 27, 0, 0);
	//AppendYearlyEntry(*entryList, 10, 2004, EFebruary, 28, 0, 0);
	TInt res = TheScheduler.CreatePersistentSchedule(aRef, *entryList);
	TEST2(res, KErrNone);

	TInt count = entryList->Count();
	delete entryList;
	return count;
	}

LOCAL_D TInt testCreateHourlyTimeSpanSchedule1(TSchedulerItemRef& aRef)
	{
	CArrayFixFlat<TScheduleEntryInfo2>* entryList;
	entryList = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(3);
	TName name(_L("and another off-peak"));
	aRef.iName = name;

	AppendHourlyEntry(*entryList, 213*24, 1999, EJune, 0, 0, 0);
	AppendHourlyEntry(*entryList, 214*24, 1999, EJune, 0, 0, 0);
	AppendHourlyEntry(*entryList, 273*24, 1999, EJune, 0, 0, 0);
	AppendHourlyEntry(*entryList, 274*24, 1999, EJune, 0, 0, 0);
	AppendHourlyEntry(*entryList, 305*24, 1999, EJune, 0, 0, 0);
	AppendHourlyEntry(*entryList, 366*24, 1999, EJuly, 0, 0, 0);

	TInt res = TheScheduler.CreatePersistentSchedule(aRef, *entryList);
	TEST2(res, KErrNone);

	TInt count = entryList->Count();
	delete entryList;
	return count;
	}

LOCAL_D TInt testCreateHourlyTimeSpanSchedule2(TSchedulerItemRef& aRef)
	{
	CArrayFixFlat<TScheduleEntryInfo2>* entryList;
	entryList = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(3);
	TName name(_L("and another off-peak"));
	aRef.iName = name;

	AppendHourlyEntry(*entryList, 365*24, 2000, EAugust, 0, 0, 0);

	TInt res = TheScheduler.CreatePersistentSchedule(aRef, *entryList);
	TEST2(res, KErrNone);
	
	TInt count = entryList->Count();
	delete entryList;
	return count;
	}

LOCAL_D TInt testCreateDailyTimeSpanSchedule1(TSchedulerItemRef& aRef)
	{
	CArrayFixFlat<TScheduleEntryInfo2>* entryList;
	entryList = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(3);
	TName name(_L("and another off-peak"));
	aRef.iName = name;

	AppendDailyEntry(*entryList, 213, 1999, EJune, 0, 0, 0);
	AppendDailyEntry(*entryList, 214, 1999, EJune, 0, 0, 0);
	AppendDailyEntry(*entryList, 273, 1999, EJune, 0, 0, 0);
	AppendDailyEntry(*entryList, 274, 1999, EJune, 0, 0, 0);
	AppendDailyEntry(*entryList, 305, 1999, EJune, 0, 0, 0);
	AppendDailyEntry(*entryList, 366, 1999, EJuly, 0, 0, 0);

	TInt res = TheScheduler.CreatePersistentSchedule(aRef, *entryList);
	TEST2(res, KErrNone);

	TInt count = entryList->Count();
	delete entryList;
	return count;
	}

LOCAL_D TInt testCreateDailyTimeSpanSchedule2(TSchedulerItemRef& aRef)
	{
	CArrayFixFlat<TScheduleEntryInfo2>* entryList;
	entryList = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(3);
	TName name(_L("and another off-peak"));
	aRef.iName = name;

	AppendDailyEntry(*entryList, 365, 2000, EAugust, 0, 0, 0);

	TInt res = TheScheduler.CreatePersistentSchedule(aRef, *entryList);
	TEST2(res, KErrNone);

	TInt count = entryList->Count();
	delete entryList;
	return count;
	}

LOCAL_D TInt testCreateMonthlyTimeSpanSchedule1(TSchedulerItemRef& aRef)
	{
	CArrayFixFlat<TScheduleEntryInfo2>* entryList;
	entryList = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(3);
	TName name(_L("and another off-peak"));
	aRef.iName = name;

	AppendMonthlyEntry(*entryList, 7, 1999, EMay, 30, 0, 0);
	AppendMonthlyEntry(*entryList, 7, 1999, EJune, 0, 0, 0);
	AppendMonthlyEntry(*entryList, 8, 1999, EJune, 29, 0, 0);
	AppendMonthlyEntry(*entryList, 8, 1999, EJuly, 0, 0, 0);
	AppendMonthlyEntry(*entryList, 9, 1999, EJuly, 0, 0, 0);
	AppendMonthlyEntry(*entryList, 12, 1999, EJuly, 0, 0, 0);

	TInt res = TheScheduler.CreatePersistentSchedule(aRef, *entryList);
	TEST2(res, KErrNone);

	TInt count = entryList->Count();
	delete entryList;
	return count;
	}


LOCAL_D TInt testCreateMonthlyTimeSpanSchedule2(TSchedulerItemRef& aRef)
	{
	CArrayFixFlat<TScheduleEntryInfo2>* entryList;
	entryList = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(3);
	TName name(_L("and another off-peak"));
	aRef.iName = name;

	AppendMonthlyEntry(*entryList, 12, 2000, EAugust, 0, 0, 0);

	TInt res = TheScheduler.CreatePersistentSchedule(aRef, *entryList);
	TEST2(res, KErrNone);

	TInt count = entryList->Count();
	delete entryList;
	return count;
	}

LOCAL_D TInt testCreateYearlyTimeSpanSchedule1(TSchedulerItemRef& aRef)
	{
	CArrayFixFlat<TScheduleEntryInfo2>* entryList;
	entryList = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(3);
	TName name(_L("and another off-peak"));
	aRef.iName = name;

	AppendYearlyEntry(*entryList, 1, 1998, EDecember, 30, 0, 0);
	AppendYearlyEntry(*entryList, 1, 1999, EJanuary, 0, 0, 0);
	AppendYearlyEntry(*entryList, 1, 1999, EFebruary, 27, 0, 0);
	AppendYearlyEntry(*entryList, 1, 1999, EMarch, 0, 0, 0);
	AppendYearlyEntry(*entryList, 1, 1999, EApril, 0, 0, 0);
	AppendYearlyEntry(*entryList, 1, 1999, EJuly, 0, 0, 0);

	TInt res = TheScheduler.CreatePersistentSchedule(aRef, *entryList);
	TEST2(res, KErrNone);

	TInt count = entryList->Count();
	delete entryList;
	return count;
	}


LOCAL_D TInt testCreateYearlyTimeSpanSchedule2(TSchedulerItemRef& aRef)
	{
	CArrayFixFlat<TScheduleEntryInfo2>* entryList;
	entryList = new (ELeave) CArrayFixFlat<TScheduleEntryInfo2>(3);
	TName name(_L("and another off-peak"));
	aRef.iName = name;

	AppendYearlyEntry(*entryList, 1, 2000, EAugust, 0, 0, 0);

	TInt res = TheScheduler.CreatePersistentSchedule(aRef, *entryList);
	TEST2(res, KErrNone);

	TInt count = entryList->Count();
	delete entryList;
	return count;
	}


LOCAL_D TInt testScheduleTask(TInt aScheduleId)
	{
	HBufC* data = HBufC::NewL(20);
	*data = _L("the data");
	TTaskInfo taskInfo;
	taskInfo.iTaskId = 0;
	taskInfo.iName = (_L("Y2K testing"));
	taskInfo.iPriority = 2;
	taskInfo.iRepeat = -1;
	TInt res = TheScheduler.ScheduleTask(taskInfo, *data, aScheduleId);
	TEST2(res, KErrNone);
	delete data;
	return taskInfo.iTaskId;
	}

LOCAL_D void doTestLoopL(TInt aId, TInt aCount)
	{
	TTsTime time = ShowDueTime(aId);
	for (TInt i=0;i<aCount;i++)
		{
		SetTimeTo1SecBefore(time);
		//wait for exe to launch
		TEST2(STaskSemaphore::WaitL(KDefaultTimeout), KErrNone); 
		CleanupHelpers::KillProcess(KMinimalTaskHandler);
		//	
		if (i<(aCount-1))//i.e. if it's going to execute
			time = ShowDueTime(aId);
		}
	}

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0273
@SYMTestCaseDesc 			Year2000 replicated test, boundary test - Hometime
@SYMTestPriority 			High
@SYMTestActions  			Create a schedules, with one entry for each boundary pair & with a task to execute immediately after boundary set time to 5 seconds before each boundary, task should be executed in 5 seconds - also ensures all (pre-and-)post boundary times are valid, also reports the day on each of these dates
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
LOCAL_D void doBoundaryTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0273 "));
	TDateTime dateTime(1998, EAugust, 1, 17, 0, 0, 0);
	SetTime(dateTime);
	TSchedulerItemRef ref;
	TInt count = testCreateBoundarySchedule1(ref);
	TInt taskId;
	taskId = testScheduleTask(ref.iHandle);
	doTestLoopL(ref.iHandle, count);
	TInt res = TheScheduler.DeleteTask(taskId);
	TEST2(res, KErrNone);
	res = TheScheduler.DeleteSchedule(ref.iHandle);
	TEST2(res, KErrNone);

	SetTime(dateTime);
	count = testCreateBoundarySchedule2(ref);
	taskId = testScheduleTask(ref.iHandle);
	doTestLoopL(ref.iHandle, count);
	TheScheduler.DeleteTask(taskId);
	TheScheduler.DeleteSchedule(ref.iHandle);
	}

LOCAL_D void doHourlyTimeSpanTestL()
	{
	TDateTime dateTime(1999, EAugust, 1, 17, 0, 0, 0);
	SetTime(dateTime);
	TSchedulerItemRef ref;
	TInt count = testCreateHourlyTimeSpanSchedule1(ref);
	TInt taskId;
	taskId = testScheduleTask(ref.iHandle);
	doTestLoopL(ref.iHandle, count);
	TInt res = TheScheduler.DeleteTask(taskId);
	TEST2(res, KErrNone);
	res = TheScheduler.DeleteSchedule(ref.iHandle);
	TEST2(res, KErrNone);

	TDateTime dt2(2000, EAugust,1,17,0,0,0);
	SetTime(dt2);
	count = testCreateHourlyTimeSpanSchedule2(ref);
	taskId = testScheduleTask(ref.iHandle);
	doTestLoopL(ref.iHandle, count);
	TheScheduler.DeleteTask(taskId);
	TheScheduler.DeleteSchedule(ref.iHandle);
	}

LOCAL_D void doDailyTimeSpanTestL()
	{
	TDateTime dateTime(1999, EAugust, 1, 17, 0, 0, 0);
	SetTime(dateTime);
	TSchedulerItemRef ref;
	TInt count = testCreateDailyTimeSpanSchedule1(ref);
	TInt taskId;
	taskId = testScheduleTask(ref.iHandle);
	doTestLoopL(ref.iHandle, count);
	TInt res = TheScheduler.DeleteTask(taskId);
	TEST2(res, KErrNone);
	res = TheScheduler.DeleteSchedule(ref.iHandle);
	TEST2(res, KErrNone);

	TDateTime dt2(2000, EAugust,1,17,0,0,0);
	SetTime(dt2);
	count = testCreateDailyTimeSpanSchedule2(ref);
	taskId = testScheduleTask(ref.iHandle);
	doTestLoopL(ref.iHandle, count);
	TheScheduler.DeleteTask(taskId);
	TheScheduler.DeleteSchedule(ref.iHandle);
	}

LOCAL_D void doMonthlyTimeSpanTestL()
	{

	TDateTime dateTime(1999, EAugust, 1, 17, 0, 0, 0);
	SetTime(dateTime);
	TSchedulerItemRef ref;
	TInt count = testCreateMonthlyTimeSpanSchedule1(ref);
	TInt taskId;
	taskId = testScheduleTask(ref.iHandle);
	doTestLoopL(ref.iHandle, count);
	TInt res = TheScheduler.DeleteTask(taskId);
	TEST2(res, KErrNone);
	res = TheScheduler.DeleteSchedule(ref.iHandle);
	TEST2(res, KErrNone);

	TDateTime dt2(2000, EAugust,1,17,0,0,0);
	SetTime(dt2);
	count = testCreateMonthlyTimeSpanSchedule2(ref);
	taskId = testScheduleTask(ref.iHandle);
	doTestLoopL(ref.iHandle, count);
	TheScheduler.DeleteTask(taskId);
	TheScheduler.DeleteSchedule(ref.iHandle);
	}

LOCAL_D void doYearlyTimeSpanTestL()
	{

	TDateTime dateTime(1999, EAugust, 1, 17, 0, 0, 0);
	SetTime(dateTime);
	TSchedulerItemRef ref;
	TInt count = testCreateYearlyTimeSpanSchedule1(ref);
	TInt taskId;
	taskId = testScheduleTask(ref.iHandle);
	doTestLoopL(ref.iHandle, count);
	TInt res = TheScheduler.DeleteTask(taskId);
	TEST2(res, KErrNone);
	res = TheScheduler.DeleteSchedule(ref.iHandle);
	TEST2(res, KErrNone);

	TDateTime dt2(2000, EAugust,1,17,0,0,0);
	SetTime(dt2);
	count = testCreateYearlyTimeSpanSchedule2(ref);
	taskId = testScheduleTask(ref.iHandle);
	doTestLoopL(ref.iHandle, count);
	TheScheduler.DeleteTask(taskId);
	TheScheduler.DeleteSchedule(ref.iHandle);
	}

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0274
@SYMTestCaseDesc 			Year2000 replicated test, time span test - Hometime
@SYMTestPriority 			High
@SYMTestActions  			For each time-span, a set of schedules with the interval defined in terms of hours, days, weeks, months, whose interval=the time span: then set time to some time in between start of span & end: schedules should be next due at end of each respective time-span. Test time-spans measured in days & months. So, 1 schedule for each of these, including 1 entry for each span. Each entry starts at start of span, repeats at end(defined in terms of x days/weeks/months.). For each entry, set time to just before end of span, check it goes off
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
LOCAL_D void doTimeSpanTestL()

	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0274 "));
	TheTest.Printf(_L("\nTesting time-spans...\n"));
	TheTest.Printf(_L("Hourly time-spans...\n"));
	doHourlyTimeSpanTestL();
	TheTest.Printf(_L("Daily time-spans...\n"));
	doDailyTimeSpanTestL();
	TheTest.Printf(_L("Monthly time-spans...\n"));
	doMonthlyTimeSpanTestL();
	TheTest.Printf(_L("Yearly time-spans...\n"));
	doYearlyTimeSpanTestL();
	}

static void DoTestsL()
	{
	TheTest.Next(_L("Delete old files"));
	SchSvrHelpers::DeleteScheduleFilesL();

	TheTest.Next(_L("Connect to Scheduler"));
	TInt res=TheScheduler.Connect();
	TEST2(res, KErrNone);

	TheTest.Next(_L("Registering Client"));
	res = SchSvrHelpers::RegisterClientL(TheScheduler);
	TEST2(res, KErrNone);
	
	// Create exe Semaphore
	STaskSemaphore sem;
	sem.CreateL();

	TheTest.Next(_L("Boundary/valid dates testing"));
	doBoundaryTestL();
	TheTest.Next(_L("Time span testing"));
	doTimeSpanTestL();
	
	// close exe Semaphore
	sem.Close();
	
	//Tidying up so next test will be clear.
	TheTest.Next(_L("Delete all schedules"));
	SchSvrHelpers::DeleteAllSchedulesL(TheScheduler);
 	TheScheduler.Close();	
	}

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;

	CTrapCleanup* cleanup = CTrapCleanup::New();
	TEST(cleanup != NULL);
	
	TEST2(TheFsSession.Connect(), KErrNone);

	TheTest.Title();
	TheTest.Start(_L("Year 2000 - hometime"));

	//If the previous test fails, SCHSVR.exe may stay in memory.
	TRAPD(err,CleanupHelpers::TestCleanupL());
	TEST2(err, KErrNone);

	TRAP(err, DoTestsL());
	TEST2(err,KErrNone);
	TRAP(err,CleanupHelpers::TestCleanupL());
	TEST2(err, KErrNone);
	
	TheTest.End();
	TheFsSession.Close();
	TheTest.Close();
	
	delete cleanup;	

	__UHEAP_MARKEND;
	return KErrNone;
	}
