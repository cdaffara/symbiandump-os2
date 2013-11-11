// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SCHTIME.H"

#include <e32base.h>
#include <e32test.h>
#include <f32file.h>
#include <s32file.h>

#include <e32std.h>
#include <tz.h>

#include "Thelpers.h"


_LIT(KTestName,	"TTsTime Tests");
RTest	TheTest(KTestName);
RTz		TheTzServer;
 
static RFs			TheFsSession;
_LIT(KFileName,"_:\\ttstime.dat");
static TBuf<32> fileName;


//
//
//Test macros and functions

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
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0204
@SYMTestCaseDesc 			Check that the default constructor works properly
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of TTsTime and check its data
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void TestsWithDefaultContructorL()
	{
	//	Tests with default constructor:
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0204 TTsTime() default constructor, TTsTime is UTC time by default "));
	TTsTime tsTime; //defalut constructor, TTsTime is UTC	
	
	TheTest.Printf(_L("Timezone set to Europe, London, DST on"));	

	//set the current utc time to 8.55am, 15 May 2005 -Daylight savings apply on this date
	TDateTime date(2005, EMay, 15, 8, 55, 0, 0);
	TTime time(date);	
	TInt err = SchSvrHelpers::SetUTCTimeL(time); 	
	TEST(err == 0);
	
	
	// check that member data are set properly using GetUtcTime() and GetLocalTime()
	TTime utcTime = TTime(tsTime.GetUtcTime());
	TTime localTime = TTime(tsTime.GetLocalTime());
	TDateTime testDate(0, EJanuary, 0, 1, 0, 0, 0);
	TTime testTime(testDate);
	TEST (localTime == testTime);	
	
	// check the offset
	// because we set the current time zone to Europe/London and 
	// ProcessOffsetEvent has been called in GetUtcTime, and since TTsTime is UTC by defalu then 
	// TTsTime::iOffset remains the same
	TTimeIntervalSeconds tsTimeOffset = tsTime.GetOffset(); 
	TEST(tsTimeOffset == TTimeIntervalSeconds(0));
	}
	
/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0205
@SYMTestCaseDesc 			Checks that the second constructor works properly
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of TTsTime and check its data
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/	
static void TestWithSecondConstructorAndUtcL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0205 TTsTime - test for 2nd constructor "));
	TheTest.Printf(_L("Timezone set to Europe, London, DST on"));	

	//set the current utc time to 8.55am, 15 May 2005 -Daylight savings apply on this date
	TDateTime date(2005, EMay, 15, 8, 55, 0, 0);
	TTime time(date);
	TInt err = SchSvrHelpers::SetUTCTimeL(time); 
	TEST(err == 0);
	
	//	Tests with overloaded constructor 
	TheTest.Next(_L("Test TTsTime with overloaded operator and TTsTime is UTC"));
	TTsTime tsTime(time, ETrue); //Sets time to UTC time as boolean is ETrue
	TTime utcTime = tsTime.GetUtcTime(); 
	TTimeIntervalSeconds tsTimeOffset = tsTime.GetOffset();
	TBool isUtc = tsTime.IsUtc();
	TEST(utcTime == time);
	TEST(tsTimeOffset == TTimeIntervalSeconds(0)); //because tsTime is UTC so its offset is zero
	TEST(isUtc); 
	}

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0206
@SYMTestCaseDesc 			Check TTsTime::SetHomeTime works properly
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of TTsTime, sets its values to home time and verifies them 
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void TestSetHomeTimeL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0206 TTsTime::SetHomeTime test "));	
	TheTest.Printf(_L("Timezone set to Europe, London, DST on"));	

	// set the date and time of the device
	TheTest.Next(_L("Test TTsTime is Local Time"));
	TDateTime date(2005, EMay, 15, 8, 55, 0, 0);
	TTime time(date);
	TInt err = SchSvrHelpers::SetUTCTimeL(time);
	TEST(err == 0);
	
	//set date and time of TTsTime to home time
	date =  TDateTime(2005, EMay, 15, 9, 55, 0, 0);
	time = date;
	TTsTime tsTime;
	tsTime.SetLocalTime(time);
	
	TTime localTime = tsTime.GetLocalTime(); 
	TTimeIntervalSeconds tsTimeOffset = tsTime.GetOffset();
	TBool isUtc = tsTime.IsUtc();

	// check that the returned local time is the same as the one we set earlier
	TEST(localTime == time );

	// check now that the stored utc time in TTsTime is correct
	TTime utcTime = tsTime.GetUtcTime();
	date =  TDateTime(2005, EMay, 15, 8, 55, 0, 0); 	
	time = TTime(date);
	TEST(utcTime == time );

	// check offset - 3600 seconds (1 hour) because time of device is in BST
	TEST(tsTimeOffset == TTimeIntervalSeconds(3600));
	TEST(!isUtc); 	//This TTsTime is not UTC.
	}
	
/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0207
@SYMTestCaseDesc 			Checks ProcessOffsetEvent 
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of TTsTime, changes timezone, and checks that the TTsTime object is updated correctly.
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
static void TestProcessOffsetEventL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0207 ProcessOffsetEvent Test "));
	TheTest.Printf(_L("Timezone set to Europe, London, DST on"));	
	
	//set the current home time to 8.55am, 15 May 2005
	//Daylight savings do apply on this date
	//so utc time should be 7.55
	TDateTime date(2005, EMay, 15, 8, 55, 0, 0);
	TTime time(date);	
	TInt err = SchSvrHelpers::SetHomeTimeL(time);
	TEST(err == KErrNone);
		
	//Check the ProcessOffsetEvent () method and print iUtcTime, iOffset
	//create hometime based ttstime object
	//should have 9.55 in iUtcTime
	date = TDateTime(2005, EMay, 15, 10, 55, 0, 0);
	time = date;
	TTsTime tsTime(time, EFalse);

	// remember current offset
	TTimeIntervalSeconds savedOffset = User::UTCOffset();
	
	// Increase offset by 1Hr by moving to Paris
	TheTest.Printf(_L("Move timezone to Paris, Europe. Check TTsTime updated accordingly"));
	CTzId* tzParisId = CTzId::NewL(2656);
	CleanupStack::PushL(tzParisId);
	TheTzServer.SetTimeZoneL(*tzParisId);
	
	//get new UTC offset
	TTimeIntervalSeconds newOffset = User::UTCOffset();
	
	// call ProcessOffsetEvent, should put 8:55 in iUtcTime 
	tsTime.ProcessOffsetEvent();
	
	// check the updated iUtcTime
	TTime utcTime = tsTime.GetUtcTime();
	TDateTime a = utcTime.DateTime();
	date = TDateTime(2005, EMay, 15, 8, 55, 0, 0);
	time = date;
	TEST(utcTime == time);
	
	// check the updated offset
	TTimeIntervalSeconds tsTimeOffset = tsTime.GetOffset();
	TEST(tsTimeOffset == newOffset);
	
	CleanupStack::PopAndDestroy(tzParisId);
	
	//return Timezone to London, Europe for other tests
	CTzId* tzLondonId = CTzId::NewL(2592); 
	CleanupStack::PushL(tzLondonId);
	TheTzServer.SetTimeZoneL(*tzLondonId);
	CleanupStack::PopAndDestroy(tzLondonId);
	}

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0208
@SYMTestCaseDesc 			Checks ExternalizeL and InternalizeL
@SYMTestPriority 			low
@SYMTestActions  			Creates an instance of TTsTime, Externalizes 
							it and internaziles and checks that teh data is the same
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/	
static void TestExternalizeInternalizeL()
	{
	// time is UTC
	TTsTime time(TTime(5), ETrue);

	//	Test externalise 
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0208 Test TTsTime::ExternalizeL "));
	RFile file;

	TFileName testFileName(fileName);
	TEST2(file.Replace(TheFsSession,testFileName,EFileWrite|EFileRead), KErrNone);
	RFileBuf buf;
	CleanupClosePushL(buf);
	buf.Attach(file);
	RWriteStream stream(&buf);
	//Externalize the time
	stream << time;
	buf.SynchL();
	CleanupStack::PopAndDestroy(&buf);
	
	//	Test internalise 
	TheTest.Next(_L("Test TTsTime::InternalizeL"));
	TTsTime newTime;
	TInt err = file.Open(TheFsSession,testFileName,EFileRead);
	TEST2(err, KErrNone);
	RFileBuf buf2;
	CleanupClosePushL(buf2);
	buf2.Attach(file);
	RReadStream stream2(&buf2);

	//Internalize the time
	stream2 >> newTime; //the externelized time is UTC time, same as the externalized time
	TTime utcTime = newTime.GetUtcTime();
	TTimeIntervalSeconds offset = newTime.GetOffset();
	TBool isUtc = newTime.IsUtc();
	TEST(utcTime == TTime(5));
	TEST(offset == TTimeIntervalSeconds(0));
	TEST(isUtc);
	CleanupStack::PopAndDestroy(&buf2);	
	}
	
	
static void RunTestsL()
	{
	//All tests assume timezone is London, Europe.
	TheTzServer.Connect();
	CTzId* tzLondonId = CTzId::NewL(2592); 
	CleanupStack::PushL(tzLondonId);
	TheTzServer.SetTimeZoneL(*tzLondonId);


	TestsWithDefaultContructorL();
	TestWithSecondConstructorAndUtcL();
	TestSetHomeTimeL();
	TestProcessOffsetEventL();
	TestExternalizeInternalizeL();
	
	CleanupStack::PopAndDestroy(tzLondonId);
	TheTzServer.Close();
	}

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
	TheTest.Start(_L("Task Scheduler TTsTime unit tests"));
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

