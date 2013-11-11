// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __THELPERS_H__
#define __THELPERS_H__

// User includes
#include <e32test.h>
#include <schinfo.h>
#include <schinfointernal.h>
#include <csch_cli.h>

class RFs;

// Numerical constants
const TInt KPausePeriod						= 1; // 1 second
const TInt KOneSecond = 1000000;
const TInt KDefaultTimeout = KOneSecond * 120; // 2 Minutes

class SchSvrHelpers
	{

public:							// STATIC COMPARISON FUNCTIONS
	IMPORT_C static TBool	IsTimeTheSame(const TTime& aTime1, const TTime& aTime2);
	IMPORT_C static TBool	IsDateTheSame(const TTime& aTime1, const TTime& aTime2);
	IMPORT_C static TBool	IsTaskInfoTheSame(const TTaskInfo& aTaskInfo1, const TTaskInfo& aTaskInfo2);
	IMPORT_C static TBool	IsItemRefTheSame(const TSchedulerItemRef& aItemRef1, const TSchedulerItemRef& aItemRef2);
	IMPORT_C static TBool	IsScheduleStateTheSame(const TScheduleState& aScheduleState1, const TScheduleState& aScheduleState2);


public:							// STATIC OBJECT CREATION FUNCTIONS
	IMPORT_C static TScheduleEntryInfo		RandomScheduleEntryInfo(TInt64& aSeed);
	IMPORT_C static TScheduleEntryInfo		ScheduleEntryInfo(TIntervalType aType, const TTime& aStartTime, TInt aInterval, TTimeIntervalMinutes aValidPeriod = 0);
	IMPORT_C static TTaskInfo				TaskInfo(const TDesC& aName, TInt aPriority, TInt aRepeat = 1);


public:							// STATIC COMPARISON FUNCTIONS - PREQ234 SUPPORT
	IMPORT_C static TBool	IsTimeTheSame(const TTsTime& aTime1, const TTsTime& aTime2);
	IMPORT_C static TBool	IsDateTheSame(const TTsTime& aTime1, const TTsTime& aTime2);
	IMPORT_C static TBool	IsTimeTheSameNoSeconds(const TTsTime& aTime1, const TTsTime& aTime2);
	IMPORT_C static TBool	IsScheduleStateTheSame(const TScheduleState2& aScheduleState1, const TScheduleState2& aScheduleState2);
	
public:							// STATIC OBJECT CREATION FUNCTIONS - PREQ234 SUPPORT
	IMPORT_C static TScheduleEntryInfo2		RandomScheduleEntryInfoHometime(TInt64& aSeed);
	IMPORT_C static TScheduleEntryInfo2		RandomScheduleEntryInfoUtc(TInt64& aSeed);
	IMPORT_C static TScheduleEntryInfo2		ScheduleEntryInfo(TIntervalType aType, const TTsTime& aStartTime, TInt aInterval, TTimeIntervalMinutes aValidPeriod = 0);

public:							// STATIC PRINTING FUNCTIONS - PREQ234 SUPPORT
	IMPORT_C static void	Pause(TInt aPauseAmount = KPausePeriod);

public:							// STATIC NEW TIME HANDLING FUNCTIONS - DEF061625 
	IMPORT_C static TInt SetHomeTimeL(const TTime& aLocalTime);
	IMPORT_C static TInt SetUTCTimeL(const TTime& aUTCTime);
	
public:							// STATIC PRINTING FUNCTIONS
	IMPORT_C static TInt	Rand(const TInt aLow, const TInt aHigh, TInt64& aSeed);
	IMPORT_C static void	Pause(RTest& aTest, TInt aPauseAmount = KPausePeriod);


public:							// SCHEDULE DELETION FUNCTIONS
	IMPORT_C static void DeleteScheduleFilesL();
	IMPORT_C static void DeleteAllSchedulesL(RScheduler& aScheduler);


public:							// client registration
	IMPORT_C static TInt RegisterClientL(RScheduler& aScheduler);	
	IMPORT_C static TInt RegisterPanicingClient(RScheduler& aScheduler);
	IMPORT_C static TInt RegisterNonExistentClient(RScheduler& aScheduler);
	

public:							// MISC FUNCTIONS
	IMPORT_C static TDateTime	TimeBasedOnOffset(TInt aSeconds, 
												TInt aMinutes = 0, 
												TInt aHours = 0, 
												TInt aDays = 0, 
												TInt aMonths = 0, 
												TInt aYears = 0);	
	IMPORT_C static TDateTime	UtcTimeBasedOnOffset(TInt aSeconds, 
												TInt aMinutes = 0, 
												TInt aHours = 0, 
												TInt aDays = 0, 
												TInt aMonths = 0, 
												TInt aYears = 0);	
	IMPORT_C static TInt CheckTaskFilesL();
	IMPORT_C static TInt CreateTaskFilesL();
	
	IMPORT_C static  TInt LaunchTaskSchedulerL();
	};
	

class CleanupHelpers
	{
	public:
	//This function is used in the test code to kill SCHSVR or MinimalTaskHandler
	//processes (or some other) when they leftover and may cause OOM condinitons.
	IMPORT_C static TInt KillProcess(const TDesC& aProcessName);
	
	//Call this method before "TheTest(error == KErrNone);" statement.
	//Otherwise if the error code is not KErrNone and the check fails, your 
	//cleanup code never gets called - probably you will have low memory condition on the 
	//test hardware and the rest of SCHSVR unit tests will fail sometimes and it will be very 
	//hard to find out what is going on there.
	IMPORT_C static void TestCleanupL();
	};	
												

// Used for signaling between launched exe and test to check that the exe is launched.
class STaskSemaphore
	{
public:
	IMPORT_C void CreateL();
	IMPORT_C static void WaitL();
	IMPORT_C static TInt WaitL(TInt aTimeout);
	IMPORT_C void Close();
private:
	RSemaphore iSemaphore;	
	};

#endif

