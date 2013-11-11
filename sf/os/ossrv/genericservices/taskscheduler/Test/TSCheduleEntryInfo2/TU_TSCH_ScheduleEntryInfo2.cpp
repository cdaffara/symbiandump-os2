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
// T_ScheduleEntryInfo2.cpp
// This file contains the implementation of test classe for TScheduleEntryInfo2. 
// 
//

#include <e32test.h>
#include <f32file.h>
#include <s32file.h>
#include <schinfointernal.h>
#include "Thelpers.h"

#include "SCHINFO.H"

_LIT(KTestName,	"TScheduleEntryInfo2 Tests");
RTest	TheTest(KTestName);

static RFs			TheFsSession;
_LIT(KFileName,"_:\\tscheduleinfo2.dat"); 
static TBuf<32> fileName;
 
//
//
//Test macroses and functions

static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		(void)TheFsSession.Delete(fileName);
		TheTest(EFalse, aLine);
		}
	}
static  void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		(void)TheFsSession.Delete(fileName);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__) 


/**
State accessor for the TScheduleEntryInfo2 object under test.
*/
class TScheduleEntryInfo2_StateAccessor
	{

public:
	void TestDefaultConstructor();
	void TestCopyConstructorOverloadedConstructor();	
		
	//utility Get and Set methods
	void TestIntervalTypeSetIntervalType();
	void TestStartTimeSetStartTime();
	void TestIntervalSetInterval();
	void TestValidityPeriodSetValidityPeriod();
	void TestNonExportedConstructor();
	void TestExternalizeInternalizeL();
	};



/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0231
@SYMTestCaseDesc 			Check the default constructor
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of 	TScheduleEntryInfo2 using the default constructor
							anc checks that the data has been properly initialised
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/	
void TScheduleEntryInfo2_StateAccessor::TestDefaultConstructor()
	{
	//Default constructor called
	//Sets Start Time to 0 UTC time
	//the interval type to 0,
	//the interval to 0,
	//the validityPeriod to 0
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0231 Test default constructor "));
	TScheduleEntryInfo2 entry;
	
	TEST(entry.iIntervalType==0);
	TEST(entry.iStartTime.GetUtcTime() == TTime(0));
	TEST(entry.iInterval == 0);
	TEST(entry.iValidityPeriod == TTimeIntervalMinutes(0));	
	}


/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0232
@SYMTestCaseDesc 			Check the copy constructor and the overloaded constructor
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of 	TScheduleEntryInfo2 using the overloaded
							constructor and another instance of TScheduleEntryInfo2 using the copy constructor.
							Then checks that data of both instances are equal. 
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/		
void TScheduleEntryInfo2_StateAccessor::TestCopyConstructorOverloadedConstructor()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0232 Text contructors "));	

	TDateTime date(2005, EMay, 15, 8, 55, 0, 0);
	TTime time(date);
	TTsTime startTime(time, EFalse);
	TIntervalType intervalType(EHourly);
	TInt interval(1);
	TTimeIntervalMinutes validityPeriod(60);
	
	TScheduleEntryInfo2 entry1(startTime, intervalType, interval, validityPeriod);
	
	TScheduleEntryInfo2 entry2(entry1);	
	
	//Test entry1 == entry2
	TEST(entry1.StartTime().GetLocalTime() == entry2.StartTime().GetLocalTime());
	TEST(entry1.ValidityPeriod() == entry2.ValidityPeriod());
	TEST(entry1.Interval() == entry2.Interval());
	
	}
	
	 
/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0233
@SYMTestCaseDesc 			Check SetIntervalType and IntervalType.
@SYMTestActions  			Creates an instance of 	TScheduleEntryInfo2 using the default constructor.
							Sets its interval type and checks that the returned value of IntervalType() 
							is equal to the one set initially.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/	
void TScheduleEntryInfo2_StateAccessor::TestIntervalTypeSetIntervalType()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0233 Test set and get for Interval Type "));

	TScheduleEntryInfo2 scheduleEntryInfo;
	
	scheduleEntryInfo.SetIntervalType(TIntervalType(EHourly));
	TIntervalType intervalType = scheduleEntryInfo.IntervalType();
	TEST(intervalType == EHourly);	
	}

	
/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0234
@SYMTestCaseDesc 			Check SetStartTime() and StartTime()
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of 	TScheduleEntryInfo2 using the default constructor
							Sets its start time using SetStartTime() and compares StartTime() returned value
							to the one set initially.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/		
void TScheduleEntryInfo2_StateAccessor::TestStartTimeSetStartTime()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0234 Test start time set and get methods "));
	
	TScheduleEntryInfo2 scheduleEntryInfo;
	
	TDateTime date(2005, EMay, 15, 8, 55, 0, 0);
	TTime time(date);
	TTsTime startTime(time, EFalse);
	
	scheduleEntryInfo.SetStartTime(startTime);
	
	TEST(scheduleEntryInfo.StartTime().GetLocalTime() == startTime.GetLocalTime());
	}


/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0235
@SYMTestCaseDesc 			Check SetInterval() and Intervale() 
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of 	TScheduleEntryInfo2 using the default constructor
							Sets its Interval using SetInterval() and compares the returned value of 
							Interval() to the one set initially.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/		
void TScheduleEntryInfo2_StateAccessor::TestIntervalSetInterval()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0235 Test set and get for Interval "));
	
	TScheduleEntryInfo2 scheduleEntryInfo;
	scheduleEntryInfo.SetInterval(1);
	TEST(scheduleEntryInfo.Interval() == TInt(1));
	}
	
/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0236
@SYMTestCaseDesc 			Check SetValidityPeriod() and ValidityPeriod()
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of 	TScheduleEntryInfo2 using the default constructor
							Sets its validty period using SetValidityPeriod() and checks that ValidityPeriod() returns 
							the same value as the one set initially.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/	
void TScheduleEntryInfo2_StateAccessor::TestValidityPeriodSetValidityPeriod()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0236 Set and get for validity period  "));
	TScheduleEntryInfo2 scheduleEntryInfo;
	
	scheduleEntryInfo.SetValidityPeriod(60);
	TTimeIntervalMinutes retValidityPeriod = scheduleEntryInfo.ValidityPeriod();
	
	TEST(retValidityPeriod == TTimeIntervalMinutes(60));
	
	}
	
/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0237
@SYMTestCaseDesc 			Check non exported constructor provided for use with the deprecated APIs
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of 	TScheduleEntryInfo using the default constructor, sets its member data, 
							and creates an instance of TScheduleEntryInfo2 using the provied constructor for use with deprecated APIs.
							Then checks that TScheduleEntryInfo2 date is equal to TScheduleEntryInfo data.

@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/	
void TScheduleEntryInfo2_StateAccessor::TestNonExportedConstructor()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0237 Test non exported constructor "));
	
	TScheduleEntryInfo entry1;
	entry1.iIntervalType	= EHourly;
	entry1.iStartTime = SchSvrHelpers::TimeBasedOnOffset(1); 
	
	entry1.iInterval		= 1;
	entry1.iValidityPeriod	= 20;
	
	TScheduleEntryInfo2 entry2(entry1);
	
	//test that scheduleEntryInfo2 startTime is local time
	TEST(!entry2.iStartTime.IsUtc());
	
	//test intervalType
	TEST(entry2.iIntervalType == entry1.iIntervalType);
		
	//test interval
	TEST(entry2.iInterval == entry1.iInterval);
	
	//test validity period
	TEST(entry2.iValidityPeriod == entry1.iValidityPeriod);	
	}


/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0238
@SYMTestCaseDesc 			Check Exetrnalize and Internalize methods
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of 	TScheduleEntryInfo2 using the overloaded operator, externalizes it
							to a file and then internalizes into another instance of TScheduleEntryInfo2.
							Checks the externalized data is the same as the internalized one.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/	
void TScheduleEntryInfo2_StateAccessor::TestExternalizeInternalizeL()
	{
	
	//	Test externalise 
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0238 \n=== Test TScheduleEntryInfo2::ExternalizeL\n"));
	RFile file;

	
	TFileName testFileName(fileName);
	TEST2(file.Replace(TheFsSession,testFileName,EFileWrite|EFileRead), KErrNone);
	RFileBuf buf;
	CleanupClosePushL(buf);
	buf.Attach(file);
	RWriteStream stream(&buf);
	
	TTsTime ttime(SchSvrHelpers::TimeBasedOnOffset(1), EFalse);
	TScheduleEntryInfo2 entry1(ttime, EHourly, 1, 20);
	stream << entry1; //externalize TScheduleEntryInfo2
	buf.SynchL();
	CleanupStack::PopAndDestroy(&buf);

	
	//	Test internalise 
	TheTest.Next(_L("\n=== Test TScheduleEntryInfo2::InternalizeL\n"));
	TScheduleEntryInfo2 entry2;
	TInt err = file.Open(TheFsSession,testFileName,EFileRead);
	TEST2(err, KErrNone);
	RFileBuf buf2;
	CleanupClosePushL(buf2);
	buf2.Attach(file);
	RReadStream stream2(&buf2);
	
	stream2 >> entry2; //internalize TScheduleEntryInfo2
	
	//check iIntervalType
	TEST(entry1.iIntervalType == entry2.iIntervalType);
	
	//check iStartTime
	TEST(entry1.iStartTime.GetUtcTime() == entry2.iStartTime.GetUtcTime());
	
	//check iInterval
	TEST(entry1.iInterval == entry2.iInterval);
	
	//check iValidityPeriod
	TEST(entry1.iValidityPeriod == entry2.iValidityPeriod);
	
	CleanupStack::PopAndDestroy(&buf2);	
		
	}

/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-3368
@SYMTestCaseDesc	    Task Scheduler: Persistent Schedules not ReScheduled on Device Bootup.
@SYMTestPriority 	    High
@SYMTestActions  	    This test does not have any capability. It tries to create persistent  
						time/ conduction schedule, it fail to create schedule
						So return code will be KErrPermissionDenied
@SYMTestExpectedResults Test must not fail 
@SYMDEF                 PDEF098080: Task Scheduler: Persistent Schedules not ReScheduled on Device Bootup.
*/
LOCAL_D void CreatePersistentScheduleCapabilityTest()
	{
	__UHEAP_MARK;

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-3368 \nPDEF098080: Task Scheduler: Persistent Schedules not ReScheduled on Device Bootup\n"));
	
	RScheduler	TheScheduler;
	TheTest(TheScheduler.Connect() == KErrNone);;
	
	//Create persistent  time based schedule 
	CArrayFixFlat<TScheduleEntryInfo2>* entryList = new(ELeave) CArrayFixFlat<TScheduleEntryInfo2>(1);
	CleanupStack::PushL(entryList);
	TSchedulerItemRef ref;
	
	TTime ttime(SchSvrHelpers::TimeBasedOnOffset(0));
	TTsTime startTime1(ttime, EFalse); 
 
	TScheduleEntryInfo2 entry1 (startTime1, EDaily, 1, 20);
	entryList->AppendL(entry1);
	
	// Create the time base schedule and check capability policing is working .
	TEST2(TheScheduler.CreatePersistentSchedule(ref, *entryList),KErrPermissionDenied);	
	
	CleanupStack::PopAndDestroy(entryList);	
	
	//Create persistent  Condition based schedule 
	CArrayFixFlat<TTaskSchedulerCondition>* conditionList = new(ELeave) CArrayFixFlat<TTaskSchedulerCondition>(1);
	CleanupStack::PushL(conditionList);
	TSchedulerItemRef ref1;
	
	TTaskSchedulerCondition condition1;
	const TUid KTestUid = TUid::Uid(0x01234566);
	condition1.iCategory = KTestUid;
	condition1.iKey		= 10;
	condition1.iState	= 10;
	condition1.iType	= TTaskSchedulerCondition::EEquals;
	conditionList->AppendL(condition1);
	
	// Create the condition base schedule and check capability policing is working .
	TEST2(TheScheduler.CreatePersistentSchedule(ref1, *conditionList,startTime1),KErrPermissionDenied);	

	CleanupStack::PopAndDestroy(conditionList);	
	TheScheduler.Close();
	
	__UHEAP_MARKEND;
	}
	
	
/**
Runs all the tests.
*/
static void RunTestsL()
	{
	TheTest.Start(_L("=== Start TScheduleEntryInfo2 tests \n"));
	
	CreatePersistentScheduleCapabilityTest();
	
	TScheduleEntryInfo2_StateAccessor* scheduleStateAccessor = new (ELeave) TScheduleEntryInfo2_StateAccessor;
	
	CleanupStack::PushL(scheduleStateAccessor);
	
	scheduleStateAccessor->TestDefaultConstructor();
	scheduleStateAccessor->TestCopyConstructorOverloadedConstructor();		
	
	//utility Get and Set methods
	scheduleStateAccessor->TestIntervalTypeSetIntervalType();
	scheduleStateAccessor->TestStartTimeSetStartTime();
	scheduleStateAccessor->TestIntervalSetInterval();
	scheduleStateAccessor->TestValidityPeriodSetValidityPeriod();
	scheduleStateAccessor->TestNonExportedConstructor();
	
	//utility externalize internalize
	scheduleStateAccessor->TestExternalizeInternalizeL();	
	CleanupStack::PopAndDestroy();
	}



//***********************************************************************************
GLDEF_C TInt E32Main()
 {
	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);
	
	__UHEAP_MARK;
	
	fileName.Copy(KFileName);
	fileName[0] = RFs::GetSystemDriveChar();

	TInt err = TheFsSession.Connect();
	TEST2(err, KErrNone);

	TheTest.Title();

	TRAP(err, ::RunTestsL())
	TEST2(err, KErrNone);
	
	(void)TheFsSession.Delete(fileName);
	TheFsSession.Close();
	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	
	delete tc;
	
	return(KErrNone);
	
	}

	
	
