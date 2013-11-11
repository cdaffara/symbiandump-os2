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

#include "Thelpers.h"

// System includes
#include <e32math.h>
#include <f32file.h>

_LIT(KPauseMessage,							"=== Pausing for %d seconds ===\n");

//
// Helper class for Scheduler Server test code
//

EXPORT_C TBool SchSvrHelpers::IsTimeTheSame(const TTime& aTime1, const TTime& aTime2)
	{
	// Just compares the time, not the date
	TDateTime time1 = aTime1.DateTime();
	TDateTime time2 = aTime2.DateTime();
	return ((time1.Hour() == time2.Hour()) && (time1.Minute() == time2.Minute()) && (time1.Second() == time2.Second()));
	}

EXPORT_C TBool SchSvrHelpers::IsDateTheSame(const TTime& aTime1, const TTime& aTime2)
	{
	// Just compares the date not the time
	TDateTime time1 = aTime1.DateTime();
	TDateTime time2 = aTime2.DateTime();
	return ((time1.Day() == time2.Day()) && (time1.Month() == time2.Month()) && (time1.Year() == time2.Year()));
	}

EXPORT_C TBool SchSvrHelpers::IsTaskInfoTheSame(const TTaskInfo& aTaskInfo1, const TTaskInfo& aTaskInfo2)
	{
	if	(	aTaskInfo1.iRepeat == aTaskInfo2.iRepeat	&& aTaskInfo1.iTaskId == aTaskInfo2.iTaskId
		 &&	aTaskInfo1.iName == aTaskInfo2.iName		&& aTaskInfo1.iPriority == aTaskInfo2.iPriority)
		 return ETrue;
	return EFalse;
	}

EXPORT_C TBool SchSvrHelpers::IsItemRefTheSame(const TSchedulerItemRef& aItemRef1, const TSchedulerItemRef& aItemRef2)
	{
	if	(aItemRef1.iHandle == aItemRef2.iHandle && aItemRef1.iName == aItemRef2.iName)
		return ETrue;
	return EFalse;
	}

EXPORT_C TBool SchSvrHelpers::IsScheduleStateTheSame(const TScheduleState& aScheduleState1, const TScheduleState& aScheduleState2)
	{
	if	(	aScheduleState1.iDueTime == aScheduleState2.iDueTime	&& aScheduleState1.iPersists == aScheduleState2.iPersists
		 &&	aScheduleState1.iName == aScheduleState2.iName			&& aScheduleState1.iEnabled == aScheduleState2.iEnabled)
		 return ETrue;
	return EFalse;
	}

EXPORT_C TScheduleEntryInfo SchSvrHelpers::RandomScheduleEntryInfo(TInt64& aSeed)
	{
	TScheduleEntryInfo entryInfo;
	entryInfo.iIntervalType		= STATIC_CAST(TIntervalType, Rand(EHourly, EYearly, aSeed));
	entryInfo.iStartTime		= TimeBasedOnOffset(Rand(0, 59, aSeed), Rand(0, 10, aSeed), Rand(0, 1, aSeed));
	entryInfo.iInterval			= Rand(1, 100, aSeed);
	entryInfo.iValidityPeriod	= Rand(1, 100, aSeed);
	return entryInfo;
	}

EXPORT_C TScheduleEntryInfo SchSvrHelpers::ScheduleEntryInfo(TIntervalType aType, const TTime& aStartTime, TInt aInterval, TTimeIntervalMinutes aValidPeriod)
	{
	TScheduleEntryInfo info;
	info.iIntervalType		= aType;
	info.iStartTime			= aStartTime;
	info.iInterval			= aInterval;
	info.iValidityPeriod	= aValidPeriod;
	return info;
	}

EXPORT_C TTaskInfo SchSvrHelpers::TaskInfo(const TDesC& aName, TInt aPriority, TInt aRepeat)
	{
	TName name(aName);
	return TTaskInfo(0, name, aPriority, aRepeat); // zero the id for now (returned by server)
	}

// Generate a random number based upon a seed and a range
EXPORT_C TInt SchSvrHelpers::Rand(const TInt aLow, const TInt aHigh, TInt64& aSeed)
	{
	TReal initialRand = (Math::FRand(aSeed) * (aHigh - aLow));
	TInt32 rand;

	// Round to 0 decimal places, ie. the nearest whole numer
	Math::Round(initialRand, initialRand, 0);
	Math::Int(rand, initialRand);

	//aSeed = seed;
	return (aLow + rand);
	}

// Pause execution for the specified number of seconds
EXPORT_C void SchSvrHelpers::Pause(RTest& aTest, TInt aPauseAmount)
	{
	aTest.Printf(KPauseMessage, aPauseAmount);
	User::After(KOneSecond * aPauseAmount); // default is 2 seconds
	}

// To be used by Hometime based schedules
EXPORT_C TDateTime SchSvrHelpers::TimeBasedOnOffset(TInt aSeconds, TInt aMinutes, TInt aHours, TInt aDays, TInt aMonths, TInt aYears)
	{
	TTime now;
	now.HomeTime();
	now += TTimeIntervalSeconds(aSeconds);
	now += TTimeIntervalMinutes(aMinutes);
	now += TTimeIntervalHours(aHours);
	now += TTimeIntervalDays(aDays);
	now += TTimeIntervalMonths(aMonths);
	now += TTimeIntervalYears(aYears);
	return now.DateTime();
	}

// To be used by UTC time based schedules
EXPORT_C TDateTime SchSvrHelpers::UtcTimeBasedOnOffset(TInt aSeconds, TInt aMinutes, TInt aHours, TInt aDays, TInt aMonths, TInt aYears)
	{
	TTime now;
	now.UniversalTime();
	now += TTimeIntervalSeconds(aSeconds);
	now += TTimeIntervalMinutes(aMinutes);
	now += TTimeIntervalHours(aHours);
	now += TTimeIntervalDays(aDays);
	now += TTimeIntervalMonths(aMonths);
	now += TTimeIntervalYears(aYears);
	return now.DateTime();
	}
	
// Check task file left
// It creates a separate process to do this as file to access in in private
// data cage which requires all files capability 
EXPORT_C TInt SchSvrHelpers::CheckTaskFilesL()
	{
	_LIT(KTaskFileChecker, "TTaskFileChecker");
	TRequestStatus stat;
	RProcess p;
	User::LeaveIfError(p.Create(KTaskFileChecker, KNullDesC));
	// Asynchronous logon: completes when process terminates with process exit code
	p.Logon(stat);
	p.Resume();

	User::WaitForRequest(stat);
	TInt exitReason = p.ExitReason();
	p.Close();
	
	// return exit reasons as checking result
	return exitReason;
	}	
	
	
// Create task files for testing
// It creates a separate process to do this as file to access in in private
// data cage which requires all files capability 
EXPORT_C TInt SchSvrHelpers::CreateTaskFilesL()
	{
	_LIT(KTaskFileCreator, "TTaskFileCreator");
	TRequestStatus stat;
	RProcess p;
	User::LeaveIfError(p.Create(KTaskFileCreator, KNullDesC));
	// Asynchronous logon: completes when process terminates with process exit code
	p.Logon(stat);
	p.Resume();

	User::WaitForRequest(stat);
	TInt exitReason = p.ExitReason();
	p.Close();
	
	// return exit reasons as checking result
	return exitReason;
	}	
	
// Start Task Scheduler
// It creates a separate process to do this as need to set appropriate SID
// to enable task scheduler to be started 
EXPORT_C TInt SchSvrHelpers::LaunchTaskSchedulerL()
	{
	_LIT(KTaskSchedulerLauncher, "TTaskSchedulerLauncher");
	TRequestStatus stat;
	RProcess p;
	User::LeaveIfError(p.Create(KTaskSchedulerLauncher, KNullDesC));
	// Asynchronous logon: completes when process terminates with process exit code
	p.Logon(stat);
	p.Resume();

	User::WaitForRequest(stat);
	TInt exitReason = p.ExitReason();
	p.Close();
	
	// return exit reasons as checking result
	return exitReason;
	}	


//
// PREQ234 support //

EXPORT_C TBool SchSvrHelpers::IsTimeTheSame(const TTsTime& aTime1, const TTsTime& aTime2)
	{
	// Just compares the time, not the date
	TBool ret = EFalse;
	
	if (aTime1.IsUtc() && aTime2.IsUtc())
		{
		TDateTime time1 = aTime1.GetUtcTime().DateTime();
		TDateTime time2 = aTime2.GetUtcTime().DateTime();
		ret = ((time1.Hour() == time2.Hour()) && (time1.Minute() == time2.Minute()) && (time1.Second() == time2.Second()));
		}
	if (!aTime1.IsUtc() && !aTime2.IsUtc())
		{
		TDateTime time1 = aTime1.GetLocalTime().DateTime();
		TDateTime time2 = aTime2.GetLocalTime().DateTime();
		ret = ((time1.Hour() == time2.Hour()) && (time1.Minute() == time2.Minute()) && (time1.Second() == time2.Second()));
		}

	return ret;
	}

EXPORT_C TBool SchSvrHelpers::IsTimeTheSameNoSeconds(const TTsTime& aTime1, const TTsTime& aTime2)
	{
	// Just compares the time up to hours and minutes
	TBool ret = EFalse;
	
	if (aTime1.IsUtc() && aTime2.IsUtc())
		{
		TDateTime time1 = aTime1.GetUtcTime().DateTime();
		TDateTime time2 = aTime2.GetUtcTime().DateTime();
		ret = ((time1.Hour() == time2.Hour()) && (time1.Minute() == time2.Minute()));
		}
	if (!aTime1.IsUtc() && !aTime2.IsUtc())
		{
		TDateTime time1 = aTime1.GetLocalTime().DateTime();
		TDateTime time2 = aTime2.GetLocalTime().DateTime();
		ret = ((time1.Hour() == time2.Hour()) && (time1.Minute() == time2.Minute()));
		}
	return ret;
	}

EXPORT_C TBool SchSvrHelpers::IsDateTheSame(const TTsTime& aTime1, const TTsTime& aTime2)
	{
	// Just compares the date not the time
	TBool ret = EFalse;
	
	if (aTime1.IsUtc() && aTime2.IsUtc())
		{
		TDateTime time1 = aTime1.GetUtcTime().DateTime();
		TDateTime time2 = aTime2.GetUtcTime().DateTime();
		ret = ((time1.Day() == time2.Day()) && (time1.Month() == time2.Month()) && (time1.Year() == time2.Year()));
		}
	if (!aTime1.IsUtc() && !aTime2.IsUtc())
		{
		TDateTime time1 = aTime1.GetLocalTime().DateTime();
		TDateTime time2 = aTime2.GetLocalTime().DateTime();
		ret = ((time1.Day() == time2.Day()) && (time1.Month() == time2.Month()) && (time1.Year() == time2.Year()));
		}

	return ret;
	}

EXPORT_C TBool SchSvrHelpers::IsScheduleStateTheSame(const TScheduleState2& aScheduleState1, const TScheduleState2& aScheduleState2)
	{
	if (aScheduleState1.DueTime().IsUtc() && aScheduleState2.DueTime().IsUtc())
		{
		TTime dueTime1 = aScheduleState1.DueTime().GetUtcTime();
		TTime dueTime2 = aScheduleState2.DueTime().GetUtcTime();
		
		if (aScheduleState1.Persists() == aScheduleState2.Persists()	&&	aScheduleState1.Name() == aScheduleState2.Name()
		&& 	aScheduleState1.Enabled() == aScheduleState2.Enabled()		&&	dueTime1 == dueTime2)
			return ETrue;
		}

	if (!aScheduleState1.DueTime().IsUtc() && !aScheduleState2.DueTime().IsUtc())
		{
		TTime dueTime1 = aScheduleState1.DueTime().GetLocalTime();
		TTime dueTime2 = aScheduleState2.DueTime().GetLocalTime();
		
		if (aScheduleState1.Persists() == aScheduleState2.Persists()	&&	aScheduleState1.Name() == aScheduleState2.Name()
		&& 	aScheduleState1.Enabled() == aScheduleState2.Enabled()		&&	dueTime1 == dueTime2)
			return ETrue;
		}	
	
	return EFalse;
	}

EXPORT_C TScheduleEntryInfo2 SchSvrHelpers::RandomScheduleEntryInfoHometime(TInt64& aSeed)
	{
	TScheduleEntryInfo2 entryInfo;
	entryInfo.SetIntervalType(STATIC_CAST(TIntervalType, Rand(EHourly, EYearly, aSeed)));
	entryInfo.SetStartTime(TTsTime (TTime(TimeBasedOnOffset(Rand(0, 59, aSeed), Rand(0, 10, aSeed), Rand(0, 1, aSeed))), ETrue));
	entryInfo.SetInterval(Rand(1, 100, aSeed));
	entryInfo.SetValidityPeriod(Rand(1, 100, aSeed));
	return entryInfo;
	}

EXPORT_C TScheduleEntryInfo2 SchSvrHelpers::RandomScheduleEntryInfoUtc(TInt64& aSeed)
	{
	TScheduleEntryInfo2 entryInfo;
	entryInfo.SetIntervalType(STATIC_CAST(TIntervalType, Rand(EHourly, EYearly, aSeed)));
	entryInfo.SetStartTime(TTsTime(TTime(TimeBasedOnOffset(Rand(0, 59, aSeed), Rand(0, 10, aSeed), Rand(0, 1, aSeed))), ETrue));
	entryInfo.SetInterval(Rand(1, 100, aSeed));
	entryInfo.SetValidityPeriod(Rand(1, 100, aSeed));
	return entryInfo;
	}

EXPORT_C TScheduleEntryInfo2 SchSvrHelpers::ScheduleEntryInfo(TIntervalType aType, const TTsTime& aStartTime, TInt aInterval, TTimeIntervalMinutes aValidPeriod)
	{
	TScheduleEntryInfo2 info (aStartTime, aType, aInterval, aValidPeriod);
	return info;
	}

// Pause execution for the specified number of seconds
EXPORT_C void SchSvrHelpers::Pause(TInt aPauseAmount)
	{
	//aTest.Printf(KPauseMessage, aPauseAmount);
	User::After(KOneSecond * aPauseAmount); // default is 2 seconds
	}

// PREQ234 support //
//

EXPORT_C void SchSvrHelpers::DeleteAllSchedulesL(RScheduler& aScheduler)
	{
	// First fetch task references so that we can delete all the tasks.
	CArrayFixFlat<TSchedulerItemRef>* refs = new (ELeave) CArrayFixFlat<TSchedulerItemRef>(3);
	CleanupStack::PushL(refs);

	{
	User::LeaveIfError(aScheduler.GetTaskRefsL(*refs, EAllSchedules, EAllTasks));
	const TInt count = refs->Count();
	// Delete all tasks
	for(TInt i=0; i<count; ++i)
		{
		const TSchedulerItemRef& ref = refs->At(i);
		User::LeaveIfError(aScheduler.DeleteTask(ref.iHandle));
		}
	}
	refs->Reset();
	{
	User::LeaveIfError(aScheduler.GetScheduleRefsL(*refs, EAllSchedules));
	// Delete all schedules
	const TInt count = refs->Count();
	for(TInt i=0; i<count; ++i)
		{
		const TSchedulerItemRef& ref = refs->At(i);
		User::LeaveIfError(aScheduler.DeleteSchedule(ref.iHandle));
		}
	}
	CleanupStack::PopAndDestroy(refs);
	}

// Deletes the task scheduler persisted files.
// It creates a separate process to do this as file to delete in in private
// data cage which requires all files capability which we don't want to 
// grant to all test exes.
EXPORT_C void SchSvrHelpers::DeleteScheduleFilesL()
	{
	_LIT(KScheduleDeleter, "TScheduleDeleter");
	TRequestStatus stat;
	RProcess p;
	User::LeaveIfError(p.Create(KScheduleDeleter, KNullDesC));
	// Asynchronous logon: completes when process terminates with process exit code
	p.Logon(stat);
	p.Resume();

	User::WaitForRequest(stat);
	TInt exitReason = p.ExitReason();
	p.Close();
	User::LeaveIfError(exitReason);
	}
	
// registers the client exe, leaving if it is not found 
EXPORT_C TInt SchSvrHelpers::RegisterClientL(RScheduler& aScheduler)
	{
	TFileName filename;
	filename = _L("MinimalTaskHandler");

	return aScheduler.Register(filename, 27);
	}
	
// registers the panicing client exe
EXPORT_C TInt SchSvrHelpers::RegisterPanicingClient(RScheduler& aScheduler)
	{
	TFileName filename;
	filename = _L("FaultyMinimalTaskHandler");

	return aScheduler.Register(filename, 27);
	}
	
// registers the a non existent exe
EXPORT_C TInt SchSvrHelpers::RegisterNonExistentClient(RScheduler& aScheduler)
	{
	TFileName filename;
	filename = _L("NonExistentClient");

	return aScheduler.Register(filename, 27);
	}
	
// Sets the system time to the given local time
// It creates a separate process to do this, 
// as this requires WDD capability which we don't want to grant to all test exes.
EXPORT_C TInt SchSvrHelpers::SetHomeTimeL(const TTime& aLocalTime)
	{
	
	_LIT(KSetHomeTime, "TSetHomeTime");
	_LIT(KTimeFormat, "%F%Y%M%D:%H%T%S.%*C6");
	
	RProcess p;	
	TRequestStatus stat;
	TBuf<128> bufLocalTime;	
	
	aLocalTime.FormatL(bufLocalTime, KTimeFormat);
	
	User::LeaveIfError(p.Create(KSetHomeTime, bufLocalTime));
	
	// Asynchronous logon: completes when process terminates with process exit code
	p.Logon(stat);
	p.Resume();

	User::WaitForRequest(stat);
	TInt exitReason = p.ExitReason();
	p.Close();
	return (exitReason);
	}

	
// Sets the UTC time to a specified time value
// It creates a separate process to do this, 
// as this requires WDD capability which we don't want to grant to all test exes.
EXPORT_C TInt SchSvrHelpers::SetUTCTimeL(const TTime& aUTCTime)
	{
	_LIT(KSetUTCTime, "TSetUTCTime");
	_LIT(KTimeFormat, "%F%Y%M%D:%H%T%S.%*C6");
	
	RProcess p;	
	TRequestStatus stat;
	TBuf<128> bufUTCTime;	
	
	aUTCTime.FormatL(bufUTCTime, KTimeFormat);	

	User::LeaveIfError(p.Create(KSetUTCTime, bufUTCTime));
		
	// Asynchronous logon: completes when process terminates with process exit code
	p.Logon(stat);
	p.Resume();

	User::WaitForRequest(stat);
	TInt exitReason = p.ExitReason();	
	p.Close();	
	return (exitReason);
	}

//This function is used in the test code to kill SCHSVR or MinimalTaskHandler
//processes (or some other) when they leftover and may cause OOM conditions.
// It creates a separate process to do this as killing a process requires
// PwrMgmt capability which we don't want to grant to all test exes.
EXPORT_C TInt CleanupHelpers::KillProcess(const TDesC& aProcessName)
	{
	_LIT(KProcessKiller, "TProcessKiller");
	TRequestStatus stat;
	RProcess p;
	TInt result = p.Create(KProcessKiller, aProcessName);
	
	if(result == KErrNone)
		{
		// Asynchronous logon: completes when process terminates with process exit code
		p.Logon(stat);
		p.Resume();

		User::WaitForRequest(stat);
		result = p.ExitReason();
		p.Close();			
		}

	return result;
	}

//Call this method before "TheTest(error == KErrNone);" statement.
//Otherwise if the error code is not KErrNone and the check fails, your 
//cleanup code never gets called - probably you will have low memory condition on the 
//test hardware and the rest of SCHSVR unit tests will fail sometimes and it will be very 
//hard to find out what is going on there.
EXPORT_C void CleanupHelpers::TestCleanupL()
	{
	// 5 second delay to clean up any launched processes 
	// left over by the test
	RDebug::Print(KPauseMessage, 5);
	User::After(5 * KOneSecond); 

	_LIT(KLogServerName, "SCHEXE");
	
	TInt err = CleanupHelpers::KillProcess(KLogServerName);
	//dont leave if the process was not found, or if it is already dead
	//but do leave if some other error occured
	if((err != KErrNotFound)&&(err != KErrDied))
		{
		User::LeaveIfError(err);	
		}
	}
	

//
//class STaskSemaphore

_LIT(KSchSemaphoreName, "SCHMinimalTaskHandler");

EXPORT_C void STaskSemaphore::CreateL()
	{
	//create semaphore and set it to 0
	User::LeaveIfError(iSemaphore.CreateGlobal(KSchSemaphoreName,0));
	}

EXPORT_C void STaskSemaphore::WaitL()
	{
	RSemaphore sem;
	User::LeaveIfError(sem.OpenGlobal(KSchSemaphoreName));
	sem.Wait();
	sem.Close();		
	}

EXPORT_C TInt STaskSemaphore::WaitL(TInt aTimeout)
	{
	RSemaphore sem;
	User::LeaveIfError(sem.OpenGlobal(KSchSemaphoreName));
	TInt r = sem.Wait(aTimeout);
	sem.Close();		
	return r;
	}

EXPORT_C void STaskSemaphore::Close()
	{
	iSemaphore.Close();
	}
	
	

