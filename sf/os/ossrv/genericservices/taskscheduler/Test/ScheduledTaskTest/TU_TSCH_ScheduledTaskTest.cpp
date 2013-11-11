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

#include <e32test.h>
#include <f32file.h>
#include <tz.h>
#include <schinfo.h>
#include <schinfointernal.h>
#include <schtask.h>
#include "Thelpers.h"

_LIT(KHBufTest, "This is a CScheduledTask test");
_LIT(KTestName,	"Scheduled Task");
RTest	TheTest(KTestName);

// persistent file for externalize and internalize test
_LIT(KFileName,"_:\\CSheduledTask.dat");
static TBuf<32> fileName;

LOCAL_D RFs TheFsSession;

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
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0202
@SYMTestCaseDesc 			Check that externalize executes correctly for tasks with local based times
@SYMTestPriority 			low
@SYMTestActions  			Create a local time based instance of CSheduledTask and externalize
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
LOCAL_D void doLocalTimeExternalizeTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0202 CScheduledTask Externalize Test (local time) "));

	TheTest.Printf(_L("Tests with timezone set to Europe, London"));	
	RTz tz;
	tz.Connect();
	CTzId* tzId = CTzId::NewL(2592); //set the timezone to Europe/London
	CleanupStack::PushL(tzId);
	tz.SetTimeZoneL(*tzId);
	
	TheTest.Printf(_L("Tests with DST on"));	
	//set the current utc time to 8.55am, 15 May 2005 -Daylight savings apply on this date
	TDateTime date(2005, EMay, 15, 8, 55, 0, 0);
	TTime time(date);
	TInt err = SchSvrHelpers::SetUTCTimeL(time); 
	TEST(err == 0);
	User::After(KOneSecond * 3);	
	// set data for CScheduledTask construction
	TTaskInfo taskInfo;
	
	HBufC* taskdata = HBufC::NewLC(KHBufTest().Length());
	TPtr pData(taskdata->Des());
	pData.Append(KHBufTest);

	TSecurityInfo securityInfo;

	CScheduledTask* extTask = new(ELeave) CScheduledTask(taskInfo, 
										taskdata, 
										ETimeSchedule, 
										securityInfo);
	CleanupStack::Pop(taskdata); //taskdata now owned by newTask
	CleanupStack::PushL(extTask);
	
	// set due time, 63284489700000000 microseconds since 1st Jan 0 AD
	// This is local time
	TDateTime extDate(2005, EMay, 15, 8, 55, 0, 0);
	TTime extTime(extDate);
	TTime extDueTime(extTime);
	extTask->OnDue(TTsTime(extDueTime,EFalse));
	
	// externalize
	RFile extFile;
	TFileName extFileName(fileName);
	TEST2(extFile.Replace(TheFsSession,extFileName,EFileWrite), KErrNone);
	RFileBuf extBuf;
	CleanupClosePushL(extBuf);
	extBuf.Attach(extFile);
	RWriteStream extStream(&extBuf);

	TRAP(err, extTask->ExternalizeL(extStream));
	TEST(err == KErrNone);
	
	CleanupStack::PopAndDestroy(3, tzId);
	}
	
/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0203
@SYMTestCaseDesc 			Check that internalize executes correctly for tasks with local based times
@SYMTestPriority 			low
@SYMTestActions  			Create a local time based instance of CSheduledTask which does an internalize
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
LOCAL_D void doLocalTimeInternalizeTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0203 CScheduledTask Internalize Test (local time) "));

	// internalize
	RFile intFile;
	TFileName intFileName(fileName);
	TEST2(intFile.Open(TheFsSession,intFileName,EFileRead), KErrNone);
	RFileBuf intBuf;
	CleanupClosePushL(intBuf);
	intBuf.Attach(intFile);
	RReadStream intStream(&intBuf);
	
	CScheduledTask* intTask = CScheduledTask::NewLC(intStream);
	
	// due date/time is 8.55am, 15 May 2005 -Daylight savings apply on this date
	TTsTime ttime = intTask->ValidUntil();
	TTime time = ttime.GetLocalTime();
	TDateTime dtime = time.DateTime();
	
	TEST(dtime.Year() == 2005);
	TEST(dtime.Month() == EMay);
	TEST(dtime.Day() == 15);
	TEST(dtime.Hour() == 8);
	TEST(dtime.Minute() == 55);
	TEST(dtime.Second() == 0);
	TEST(dtime.MicroSecond() == 0);
	
	// test offset - 3600 seconds (1 hour) because time of device is in BST
	TTimeIntervalSeconds offset(3600);
	TEST(ttime.GetOffset() == offset);
	
	// test difference between returned values
	TTime utcTime = intTask->ValidUntil().GetUtcTime();
	TTime localTime = intTask->ValidUntil().GetLocalTime();
	Int64 t = localTime.Int64() - utcTime.Int64();
	TTimeIntervalSeconds diff((localTime.Int64() - utcTime.Int64())/1000000);
	TEST(diff == offset);
	
	// test that this instance is home time and not UTC
	TEST(ttime.IsUtc() == EFalse);

	// test data
	HBufC* data = const_cast<HBufC*>(&(intTask->Data()));
	TEST(data->Compare(KHBufTest) == 0);

	CleanupStack::PopAndDestroy(2, &intBuf);
	}

/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0239 
@SYMTestCaseDesc 			Check that externalize executes correctly for tasks with UTC based times
@SYMTestPriority 			low
@SYMTestActions  			Create a UTC time based instance of CSheduledTask and externalize
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
LOCAL_D void doUtcExternalizeTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0239 CScheduledTask Externalize Test (UTC) "));

	TheTest.Printf(_L("Tests with timezone set to Europe, London"));	
	RTz tz;
	tz.Connect();
	CTzId* tzId = CTzId::NewL(2592); //set the timezone to Europe/London
	CleanupStack::PushL(tzId);
	tz.SetTimeZoneL(*tzId);
	
	TheTest.Printf(_L("Tests with DST on"));	
	//set the current utc time to 8.55am, 15 May 2005 -Daylight savings apply on this date
	TDateTime date(2005, EMay, 15, 8, 55, 0, 0);
	TTime time(date);
	TInt err = SchSvrHelpers::SetUTCTimeL(time); 
	TEST(err == 0);
	User::After(KOneSecond * 3);	
	// set data for CScheduledTask construction
	TTaskInfo taskInfo;
	
	HBufC* taskdata = HBufC::NewLC(KHBufTest().Length());
	TPtr pData(taskdata->Des());
	pData.Append(KHBufTest);

	TSecurityInfo securityInfo;

	CScheduledTask* extTask = new(ELeave) CScheduledTask(taskInfo, 
										taskdata, 
										ETimeSchedule, 
										securityInfo);
	CleanupStack::Pop(taskdata); //taskdata now owned by newTask
	CleanupStack::PushL(extTask);
	
	// set due time, 63284489700000000 microseconds since 1st Jan 0 AD
	// This is UTC time
	TDateTime extDate(2005, EMay, 15, 8, 55, 0, 0);
	TTime extTime(extDate);
	TTime extDueTime(extTime);
	extTask->OnDue(TTsTime(extDueTime,ETrue));
	
	// externalize
	RFile extFile;
	TFileName extFileName(fileName);
	TEST2(extFile.Replace(TheFsSession,extFileName,EFileWrite), KErrNone);
	RFileBuf extBuf;
	CleanupClosePushL(extBuf);
	extBuf.Attach(extFile);
	RWriteStream extStream(&extBuf);

	TRAP(err, extTask->ExternalizeL(extStream));
	TEST(err == KErrNone);
	
	CleanupStack::PopAndDestroy(3, tzId);
	}
	
/**
@file
@SYMTestCaseID				SYSLIB-SCHSVR-CT-0240	
@SYMTestCaseDesc 			Check that internalize executes correctly for tasks with UTC times
@SYMTestPriority 			low
@SYMTestActions  			Create a UTC based instance of CSheduledTask which does an internalize
@SYMTestExpectedResults		The test must not fail.
@SYMPREQ					PREQ234
*/
LOCAL_D void doUtcInternalizeTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-0240 CScheduledTask Internalize Test (UTC) "));

	// internalize
	RFile intFile;
	TFileName intFileName(fileName);
	TEST2(intFile.Open(TheFsSession,intFileName,EFileRead), KErrNone);
	RFileBuf intBuf;
	CleanupClosePushL(intBuf);
	intBuf.Attach(intFile);
	RReadStream intStream(&intBuf);
	
	CScheduledTask* intTask = CScheduledTask::NewLC(intStream);
	
	// due date/time is 8.55am, 15 May 2005 -Daylight savings apply on this date
	TTsTime ttime = intTask->ValidUntil();
	TTime time = ttime.GetUtcTime();
	TDateTime dtime = time.DateTime();
	
	TEST(dtime.Year() == 2005);
	TEST(dtime.Month() == EMay);
	TEST(dtime.Day() == 15);
	TEST(dtime.Hour() == 8);
	TEST(dtime.Minute() == 55);
	TEST(dtime.Second() == 0);
	TEST(dtime.MicroSecond() == 0);
	
	// test offset - should be zero because the object is UTC based
	TTimeIntervalSeconds offset(0);
	TEST(ttime.GetOffset() == offset);
	
	// test difference between returned values
	TTime utcTime = intTask->ValidUntil().GetUtcTime();
	TTime localTime = intTask->ValidUntil().GetLocalTime();
	Int64 t = localTime.Int64() - utcTime.Int64();
	TTimeIntervalSeconds diff((localTime.Int64() - utcTime.Int64())/1000000);
	// difference should be the kernel offset from UTC	
	offset = User::UTCOffset();
	TEST(diff == offset);
	
	// test that this instance is UTC and not local time based
	TEST(ttime.IsUtc());

	// test data
	HBufC* data = const_cast<HBufC*>(&(intTask->Data()));
	TEST(data->Compare(KHBufTest) == 0);

	CleanupStack::PopAndDestroy(2, &intBuf);
	}


static void RunTestsL()
	{
	doLocalTimeExternalizeTestL();
	doLocalTimeInternalizeTestL();
	doUtcExternalizeTestL();
	doUtcInternalizeTestL();
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
	TEST(err == KErrNone);

	TheTest.Title();
	TheTest.Start(_L("Unit tests for CScheduledTask"));
	TRAP(err, ::RunTestsL())
	TEST(err == KErrNone);
	
	(void)TheFsSession.Delete(fileName);
	TheFsSession.Close();
	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	
	delete tc;
	
	return(KErrNone);
	}
