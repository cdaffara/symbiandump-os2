// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#define __PROFILING__
#include <s32file.h>
#include "t_logutil2.h"
#include <logview.h>

RTest TheTest(_L("t_logbench"));

TLogConfig theConfig;

_LIT(KTestString, "%dabcdefghijklmnopqrstuvwxyz");
_LIT(KResultFile, "C:\\LOGENG_TEST.TXT");

const TInt KLogSize = 50;
const TInt KTestFactor = 2;  

// Class used to record test details
class TTestDetails
	{
public:
	TInt iEventNum;
	TInt iTimeToAdd;
	//
	TInt iViewSize;
	TInt iTimeToNavigate;
	//
	TInt iDatabaseSize;
	TInt iHeapSize;
	};

LOCAL_C TInt GetHeapSizeL()
	{	
	TInt heap = 0;
	heap = User::Heap().Size();

	return heap;
	}

LOCAL_C TInt GetServerHeapSizeL()
	{
	return 1024 * 1024;//By default - the process heap is 1M.
	}

LOCAL_C TInt DatabaseSizeL()
	{
		return TestUtils::DatabaseSizeL();
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0988
@SYMTestCaseDesc	    Sets the configuration setup for the tests
@SYMTestPriority 	    High
@SYMTestActions  	    Setup for the environment for the tests
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestSetupL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0988 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	// Get configuration
	aClient.GetConfig(theConfig, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TheTest.Printf(_L("  Log size: %d\n"), theConfig.iMaxLogSize);
	TheTest.Printf(_L("  Recent list size: %d\n"), theConfig.iMaxRecentLogSize);
	TheTest.Printf(_L("  Max Event Age: %d\n"), theConfig.iMaxEventAge);

	TestUtils::DeleteDatabaseL();

	// Get configuration
	aClient.GetConfig(theConfig, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Wait for user interation
	//TheTest.Printf(_L("  Quick tests performed if no key pressed in next 10 seconds\n"));
    //TKeyCode key;
	//if (!TestUtils::WaitForKeyL(10000000, key))
		{
		theConfig.iMaxLogSize = KLogSize;

		// Set configuration in database
		aClient.ChangeConfig(theConfig, active->iStatus);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	CleanupStack::PopAndDestroy(); // active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0989
@SYMTestCaseDesc	    Benchmark test
						Tests for writing the log details to a file
@SYMTestPriority 	    High
@SYMTestActions  	    Add events to the log and write the configuration details to a file
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void BenchmarkTestL(CLogClient& aClient, RFile& aFile)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0989 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	aFile.Write(_L8("Adding Events\n"));

	TBuf8<256> buf;
	TInt count = 0;

	TLogString string;
	string.Format(KTestString, count);

	while(count++ < theConfig.iMaxLogSize * KTestFactor)
		{
		event->SetRemoteParty(string);
		event->SetSubject(string);
		event->SetNumber(string);
		event->SetStatus(string);

		if (count % 10 == 0)
			string.Format(KTestString, count);

		TTime before;
		before.UniversalTime();

		// Add a new event
		aClient.AddEvent(*event, active->iStatus);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TTime after;
		after.UniversalTime();
		TTimeIntervalMicroSeconds interval = after.MicroSecondsFrom(before);

		TInt dbSize = DatabaseSizeL();
		TInt heapSize = GetHeapSizeL();
		TInt serverHeapSize = GetServerHeapSizeL();

		// Store details in file
		TheTest.Printf(_L("  Num: %d, Time: %d, Db Size: %d, Hs: %d, Server Hs: %d\n"), count, I64INT(interval.Int64()), dbSize, heapSize, serverHeapSize);
		buf.Format(_L8("%d,%d,%d,%d,%d\n"), count, I64INT(interval.Int64()), dbSize, heapSize, serverHeapSize);
		aFile.Write(buf);
		}

	aFile.Write(_L8("Navigating View\n"));
	count = 1;

	// Navigate the view
	TEST(view->SetFilterL(*filter, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	while(view->NextL(active->iStatus))
		{
		TTime before;
		before.UniversalTime();

		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TTime after;
		after.UniversalTime();
		TTimeIntervalMicroSeconds interval = after.MicroSecondsFrom(before);

		TInt heapSize = GetHeapSizeL();
		TInt serverHeapSize = GetServerHeapSizeL();

		// Store details in file
		TheTest.Printf(_L("  Count: %d, Time: %d, Hs: %d, Server Hs: %d\n"), count, I64INT(interval.Int64()), heapSize, serverHeapSize);
		buf.Format(_L8("%d,%d,%d,%d\n"), count++, I64INT(interval.Int64()), heapSize, serverHeapSize);
		aFile.Write(buf);
		}

	CleanupStack::PopAndDestroy(4); // event, filter, view, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0990
@SYMTestCaseDesc	    Tests for CLogViewRecent::SetRecentListL(),CLogViewRecent::DuplicatesL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Set the recent log view list and refresh for the duplicates view.
                        Check for ErrNone flag
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void DoTestRecentViewsL(CLogClient& aClient, TLogRecentList aList, TInt aRecentCount, TInt aDuplicateCount)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0990 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	TEST(recent->SetRecentListL(aList, active->iStatus) || aRecentCount == 0);
	do
		{
		if (aRecentCount == 0)
			break;

		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST(recent->DuplicatesL(*duplicate, active->iStatus) || aDuplicateCount == 0);
		do
			{
			if (aDuplicateCount == 0)
				break;

			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}
		while(duplicate->NextL(active->iStatus));
		TEST(duplicate->CountL() == aDuplicateCount);
		}
	while(recent->NextL(active->iStatus));
	TEST(recent->CountL() == aRecentCount);

	CleanupStack::PopAndDestroy(3); // active, recent, duplicate
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0991
@SYMTestCaseDesc	    Tests to clear the duplicates in a view
                        Tests CLogViewRecent::ClearDuplicatesL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Set the recent log view list.Check for ErrNone flag.
                        Write the details to a file.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void DoTestClearDuplicateL(CLogClient& aClient, TLogRecentList aList, RFile& aFile)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0991 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent);

	TTime before;
	before.UniversalTime();

	recent->SetRecentListL(aList, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	recent->ClearDuplicatesL();

	TTime after;
	after.UniversalTime();
	TTimeIntervalMicroSeconds interval = after.MicroSecondsFrom(before);

	TBuf8<256> buf;
	TheTest.Printf(_L("Clearing Duplicates for List %d, %d\n"), recent->RecentList(), I64INT(interval.Int64()));
	buf.Format(_L8("Clearing Duplicates for List %d, %d\n"), recent->RecentList(), I64INT(interval.Int64()));
	aFile.Write(buf);

	CleanupStack::PopAndDestroy(2); // recent, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0992
@SYMTestCaseDesc	    Recent lists view test
@SYMTestPriority 	    High
@SYMTestActions  	    Add the events to the log and execute the test functions. 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentListsL(CLogClient& aClient, RFile& aFile)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0992 "));
	aFile.Write(_L8("Recent Lists\n"));

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig config;
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	for(TInt duplicates = 0; duplicates < 10; duplicates++)
		{
		TTime before;
		before.UniversalTime();

		// Incoming
		TLogString incoming;
		aClient.GetString(incoming, R_LOG_DIR_IN);
		event->SetDirection(incoming);

		TInt count;
		for(count = 0; count < config.iMaxRecentLogSize; count++)
			{
			TLogString number;
			number.Num(count);
			event->SetNumber(number);

			active->StartL();
			aClient.AddEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}

		// Outgoing
		TLogString outgoing;
		aClient.GetString(outgoing, R_LOG_DIR_OUT);
		event->SetDirection(outgoing);

		for(count = 0; count < config.iMaxRecentLogSize; count++)
			{
			TLogString number;
			number.Num(count);
			event->SetNumber(number);

			active->StartL();
			aClient.AddEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}

		// Missed
		TLogString missed;
		aClient.GetString(missed, R_LOG_DIR_MISSED);
		event->SetDirection(missed);

		for(count = 0; count < config.iMaxRecentLogSize; count++)
			{
			TLogString number;
			number.Num(count);
			event->SetNumber(number);

			active->StartL();
			aClient.AddEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}

		TTime afterAdd;
		afterAdd.UniversalTime();

		DoTestRecentViewsL(aClient, KLogRecentIncomingCalls, config.iMaxRecentLogSize, duplicates);
		DoTestRecentViewsL(aClient, KLogRecentOutgoingCalls, config.iMaxRecentLogSize, duplicates);
		DoTestRecentViewsL(aClient, KLogRecentMissedCalls, config.iMaxRecentLogSize, duplicates);

		TTime afterNav;
		afterNav.UniversalTime();

		TTimeIntervalMicroSeconds interval1 = afterAdd.MicroSecondsFrom(before);
		TTimeIntervalMicroSeconds interval2 = afterNav.MicroSecondsFrom(afterAdd);

		// Store details in file
		TBuf8<256> buf;
		TheTest.Printf(_L("  Count: %d, Add: %d, Nav: %d\n"), duplicates, I64INT(interval1.Int64()), I64INT(interval2.Int64()));
		buf.Format(_L8("%d,%d,%d\n"), duplicates, I64INT(interval1.Int64()), I64INT(interval2.Int64()));
		aFile.Write(buf);
		}

	DoTestClearDuplicateL(aClient, KLogRecentIncomingCalls, aFile);
	DoTestClearDuplicateL(aClient, KLogRecentOutgoingCalls, aFile);
	DoTestClearDuplicateL(aClient, KLogRecentMissedCalls, aFile);

	CleanupStack::PopAndDestroy(2); // active, event
	}


void doTestsL()
	{
	TestUtils::Initialize(_L("t_logbench"));

	CLogChangeNotifier* notifier = CLogChangeNotifier::NewL();
	CleanupStack::PushL(notifier);

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig config;
	client->GetConfig(config, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TheTest.Start(_L("Setup"));
	TestSetupL(*client);
	theLog.Write(_L8("Test 1 OK\n"));
	
	RFile results;
	LEAVE_IF_ERROR(results.Replace(theFs, KResultFile, EFileWrite|EFileShareExclusive));

	TheTest.Next(_L("Benchmark tests"));
	BenchmarkTestL(*client, results);
	theLog.Write(_L8("Test 2 OK\n"));

	TestUtils::DeleteDatabaseL();

	TheTest.Next(_L("Recent List tests"));
	TestRecentListsL(*client, results);
	theLog.Write(_L8("Test 3 OK\n"));

	// Restore Config
	client->ChangeConfig(config, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	results.Close();
	CleanupStack::PopAndDestroy(3); // active, notifier, client;
	}

