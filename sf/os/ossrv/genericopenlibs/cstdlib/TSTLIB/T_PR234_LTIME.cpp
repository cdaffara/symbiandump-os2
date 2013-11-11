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
// Simple STDLIB tests.
// 
//

#include <e32test.h>
#include <e32svr.h>
#include <sys/time.h>
#include <time.h>
#include <sys/reent.h>
#include <tz.h>


//CPP file is used for C tests, because by default any console opened from a C file
//expects a key to be pressed when it is about to be closed. That makes impossible
//the creation of automated C tests.

//
// Globals

LOCAL_D RTest test(_L("TTime"));
_LIT16(priorUnixTime,"19700000:000000.000000"); //0 AD to the start of Unix Time

//
// Tests

/**
@file
@SYMTestCaseID		SYSLIB-STDLIB-CT-0143
@SYMTestCaseDesc 	Check that gettimeofday() returns universaltime, rather than the local time. 
@SYMTestPriority 	low
@SYMTestActions  	retrieve return value of gettimeofday() and compare with preset universaltime
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ234
*/
void Testgettimeofday()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STDLIB-CT-0143 "));
	test.Printf(_L("\ntesting gettimeofday()...\n"));
	RTz tz;
	TInt error=tz.Connect();
	test(error==KErrNone);
	CTzId* tzId = CTzId::NewL(2592); //set the timezone to Europe/London
	CleanupStack::PushL(tzId);
	tz.SetTimeZoneL(*tzId);
	
	struct timeval tv;
	struct timezone tzone;
	TTime t,unix;
	unix.Set(priorUnixTime);
	
	test.Printf(_L("tests during summertime (dst on)...\t"));	
	//set the utc time to 8.55am, 15 May 2005 -Daylight savings apply on this date
	TInt err=User::SetUTCTime(TTime(TDateTime(2005, EMay, 15, 8, 55, 0, 0))); 
	test(err==0);
	t.UniversalTime();
	err = gettimeofday(&tv, &tzone);
	test(err==0);
	TTimeIntervalSeconds s = (User::UTCOffset().Int())/60;
	test(tzone.tz_minuteswest==s.Int());
	test(tzone.tz_dsttime == 0);
	// Conversion needed as TTime returns micro seconds from 0AD to now, and gettimeofday() returns 
	// seconds from 1970 to now,
	TInt64 sec = tv.tv_sec;
	TUint64 microSec = (sec*1000000) + tv.tv_usec + unix.Int64();
    test.Printf(_L("Expected Time: %ld\tReceived Time: %ld\n"),t.Int64(),microSec);
	test((microSec-t.Int64())<1000000);//allowing a 1 sec delay in time
	test.Printf(_L("-OK\n"));
	
	test.Printf(_L("tests during wintertime (dst off)...\t"));	
	//set the utc time to 8.55am, 15 January 2005 -Daylight savings DON'T apply on this date
	err=User::SetUTCTime(TTime(TDateTime(2005, EJanuary, 15, 8, 55, 0, 0))); 
	test(err==0);
	t.UniversalTime();
	err = gettimeofday(&tv, &tzone);
	test(err==0);
	// Conversion needed as TTime returns micro seconds from 0AD to now, and gettimeofday() returns 
	// seconds from 1970 to now,
	sec = tv.tv_sec;
	microSec = (sec*1000000) + tv.tv_usec + unix.Int64();
	test((microSec-t.Int64())<1000000);//allowing a 1 sec delay in time
	test.Printf(_L("-OK\n"));
	//
	CleanupStack::PopAndDestroy(tzId);
	tz.Close();
	}

/**
@file
@SYMTestCaseID		SYSLIB-STDLIB-CT-0144
@SYMTestCaseDesc 	Check that time() returns universaltime, rather than the local time. 
@SYMTestPriority 	low
@SYMTestActions  	retrieve return value of time() and compare with preset universaltime
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ234
*/
void Testtime()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STDLIB-CT-0144 \ntesting time()...\n "));
	TInt r = KErrNone;
    RTz tz;
    r = tz.Connect();
    if (r != KErrNone)
        {
            User::Leave(r);
        }
	CTzId* tzId = CTzId::NewL(2592); //set the timezone to Europe/London
	CleanupStack::PushL(tzId);
	tz.SetTimeZoneL(*tzId);
	TTime t,unix;
	unix.Set(priorUnixTime);
	
	test.Printf(_L("tests during summertime (dst on)...\t"));
	//set the utc time to 8.55am, 15 May 2005 -Daylight savings apply on this date
	TInt err=User::SetUTCTime(TTime(TDateTime(2005, EMay, 15, 8, 55, 0, 0)));
	test(err==0);
	t.UniversalTime(); 	
	time_t res = time(0) * 1000000; // current time
	// As TTime returns micro seconds from 0AD to now, and time() returns seconds from 1970 to now,
	// the start date of t needs to be changed to 1 Jan 1970 midnight and converted into seconds
	TInt64 sec = t.Int64() - unix.Int64(); 
	test((res-sec)<1000000);//allowing 1 sec delay in time
	test.Printf(_L("- OK!\n"));
	
	test.Printf(_L("tests during wintertime (dst off)...\t"));
	//set the utc time to 8.55am, 15 January 2005 -Daylight savings DON'T apply on this date
	err=User::SetUTCTime(TTime(TDateTime(2005, EJanuary, 15, 8, 55, 0, 0)));
	test(err==0);
	t.UniversalTime(); 	
	res = time(0) * 1000000; // current time
	// As TTime returns micro seconds from 0AD to now, and time() returns seconds from 1970 to now,
	// the start date of t needs to be changed to 1 Jan 1970 midnight and converted into seconds
	sec = t.Int64() - unix.Int64(); 
	test((res-sec)<1000000);//allowing 1 sec delay in time
	test.Printf(_L("- OK!\n"));
	
	//
	CleanupStack::PopAndDestroy(tzId);
	tz.Close();	
	}

/**
@file
@SYMTestCaseID		SYSLIB-STDLIB-CT-0145
@SYMTestCaseDesc 	Check that toLocal() converts into correct localtime
@SYMTestPriority 	low
@SYMTestActions  	With the Timezone set to Europe/London, a universaltime is passed to 
the function localtime (as toLocal cannot be accessed directly) which is expected to return a hometime, with DST on
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ234
*/
void TesttoLocal()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-STDLIB-CT-0145  \ntesting toLocal()...\n "));
	
	//test when dst is on...
	test.Printf(_L("tests during summertime (dst on)...\t"));
	
	TInt rt = KErrNone;
	TInt rz = KErrNone;
	
	//set the utc time to 8.55am, 15 May 2005 -Daylight savings apply on this date
	TInt err=User::SetUTCTime(TTime(TDateTime(2005, EMay, 15, 8, 55, 0, 0)));
	test(err==0);
	RTz tz;
    rt = tz.Connect();
    if (rt != KErrNone)
        {
            User::Leave(rt);
        }
	CTzId* tzId = CTzId::NewL(2592); //set the timezone to Europe/London
	CleanupStack::PushL(tzId);
	tz.SetTimeZoneL(*tzId);
	CleanupStack::PopAndDestroy(tzId);
	tz.Close();
	
	struct tm *ptr, *ptr2;
	struct tm setup;
	time_t seconds;
	
	setup.tm_hour = 8; //8 o'clock utc time
	setup.tm_min = 55; 
	setup.tm_sec = 0; 
	setup.tm_mday = 1;
	setup.tm_mon = 3;
	setup.tm_year = 105;
	seconds = mktime(&setup);
	
	ptr2 = gmtime(&seconds); //for a quick routine test
	test(ptr2->tm_hour == 8);//
	
	ptr = localtime(&seconds);	
	test(ptr->tm_hour == 9); //test against hometime hour: 9;
	test(ptr->tm_min == 55);
	test(ptr->tm_sec == 0);
	test(ptr->tm_mday == 1);
	test(ptr->tm_mon == 3);
	test(ptr->tm_year == 105);
	test.Printf(_L("Time:9:55 -correct!\n"));
	
	//test when DST is off
	test.Printf(_L("tests during wintertime (dst off)...\t"));
	err=User::SetUTCTime(TTime(TDateTime(2005, EJanuary, 15, 8, 55, 0, 0)));
	test(err==0);
	
    rz = tz.Connect();
    if (rz != KErrNone)
        {
            User::Leave(rz);
        }
    
	CTzId* tzId2 = CTzId::NewL(2592); //set the timezone to Europe/London
	CleanupStack::PushL(tzId2);
	tz.SetTimeZoneL(*tzId2);
	CleanupStack::PopAndDestroy(tzId2);
	tz.Close();
	
	ptr2 = gmtime(&seconds); //for a quick routine test
	test(ptr2->tm_hour == 8);//
	
	ptr = localtime(&seconds);	
	test(ptr->tm_hour == 8); //test against hometime hour: 8;
	test(ptr->tm_min == 55);
	test(ptr->tm_sec == 0);
	test(ptr->tm_mday == 1);
	test(ptr->tm_mon == 3);
	test(ptr->tm_year == 105);
	test.Printf(_L("Time:8:55 -correct!\n"));
	//
	CloseSTDLIB();	
	}

//
//

LOCAL_C void DoTestsL()
	{
	TRAPD(err,Testgettimeofday());
	test(err==KErrNone);
	TRAP(err,Testtime());
	test(err==KErrNone);
	TRAP(err,TesttoLocal());
	test(err==KErrNone);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Title();
	test.Start(_L("Time & Date Tests..."));

	CTrapCleanup* trapCleanup=CTrapCleanup::New();
	TRAPD(error, DoTestsL());
	test(error==KErrNone);
	delete trapCleanup;

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return error;
	}

