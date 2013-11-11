// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// T_LOGSERVCRASH.CPP
// This tests the fix implemented for DEF047320
// 
#include <s32file.h>
#include <e32math.h>
#include "t_logutil2.h"
#include <logview.h>

RTest TheTest(_L("t_logservercrash"));

const TInt KTestEventNum = 10;
const TLogDurationType KTestDurationType1 = 1;
const TLogDuration KTestDuration1 = 0x1234;
const TLogFlags KTestFlags1 = 0x5;
const TLogLink KTestLink1 = 0x1234;

_LIT(KTestRemoteParty1, "Remote Party");
_LIT(KTestDirection1, "Direction");
_LIT(KTestStatus1, "Status");
_LIT(KTestSubject1, "Subject");
_LIT(KTestNumber1, "TheNumber");
_LIT8(KTestData1, "ABCDEFGH");

TBool TheMatchingIsEnabled = EFalse;


#define SERVER_NAME _L("LogServ*")

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1021
@SYMTestCaseDesc	    Tests for killing the server
                        Tests for RProcess::Kill(),RThread::Kill() function
@SYMTestPriority 	    High
@SYMTestActions  	    If EKA2 find the process and kill it, for EKA1 find thread and kill it
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/	
LOCAL_C void TestKillServerL()
	{	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1021 Before Killing Server "));
	User::After(2000000);
	
	// for EKA2 find process and Kill
	// Note: this needs CAPABILITY PowerMgmt
	TFindProcess findProcess(SERVER_NAME);
	TFullName result;
	LEAVE_IF_ERROR( findProcess.Next(result) );
	
	RProcess server;
	LEAVE_IF_ERROR( server.Open(findProcess, EOwnerProcess) );
	server.Kill(0);		
 
	TheTest.Next(_L("Server is Killed"));
	User::After(2000000);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1022
@SYMTestCaseDesc	    Tests for operations on log database
@SYMTestPriority 	    High
@SYMTestActions  	    Check by add new event,change the event,and get event and check for integrity of data 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/	
LOCAL_C void TestEventViewL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1022 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewChangeObserver* changeObs = CLogViewChangeObserver::NewLC();
	changeObs->SetActive();

  	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs);
	CleanupStack::PushL(view);

	CLogEvent* event;
	TLogId id;
	TTime now;
	now.UniversalTime();
	TInt count;

	for(count = 0; count < KTestEventNum; count++)
		{
		event = CLogEvent::NewL();
		CleanupStack::PushL(event);
		event->SetEventType(KLogCallEventTypeUid);

		//User::After(Math::Random() % 100000);
		User::After(1000000);
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();

		TEST2(active->iStatus.Int(), KErrNone);
		TEST(event->EventType() == KLogCallEventTypeUid);
		TEST(event->Description().Length() > 0);
		TEST(event->Time() >= now);

		now = event->Time();
		id = event->Id();
		
		event->SetRemoteParty(KTestRemoteParty1);
		event->SetDirection(KTestDirection1);
		event->SetDurationType(KTestDurationType1);
		event->SetDuration(KTestDuration1);
		event->SetStatus(KTestStatus1);
		event->SetSubject(KTestSubject1);
		event->SetNumber(KTestNumber1);
		event->SetLink(KTestLink1);
		event->SetDataL(KTestData1);
		event->SetFlags(KTestFlags1);
		event->SetContact((TLogContactItemId) count+1);

		User::After(Math::Random() % 100000);
		active->StartL();
		aClient.ChangeEvent(*event, active->iStatus);
		CActiveScheduler::Start();

		//If the test fails on the next line with -1 or -12 error, it is possible that the contacts database is missing:
		//c:\private\10003a73\SQLite__Contacts.cdb. Copy it to the specified location and rerun the test.
		TEST2(active->iStatus.Int(), KErrNone);
		TEST(event->EventType() == KLogCallEventTypeUid);
		TEST(event->RemoteParty() == KTestRemoteParty1);
		TEST(event->Direction() == KTestDirection1);
		TEST(event->DurationType() == KTestDurationType1);
		TEST(event->Duration() == KTestDuration1);
		TEST(event->Status() == KTestStatus1);
		TEST(event->Subject() == KTestSubject1);
		TEST(event->Number() == KTestNumber1);
		TEST(event->Link() == KTestLink1);
		TEST(event->Data() == KTestData1);
		TLogFlags eventFlags;
		if(TheMatchingIsEnabled)
			{
			eventFlags=KTestFlags1|KLogEventContactSearched;
			}
		else
			{
			eventFlags=KTestFlags1;
			}
		//If the test fails on the next line, it is quite possible that there is a
		//z:\private\10202be9a\101f401d.txt file that sets the contacts match count to 0.
		//Delete the file and run the test again.
		TEST(event->Flags() == eventFlags);
		TEST(event->Contact()==(TLogContactItemId) count+1);

		CleanupStack::PopAndDestroy(); // event;

		event = CLogEvent::NewL();
		CleanupStack::PushL(event);

		event->SetId(id);
		active->StartL();
		aClient.GetEvent(*event, active->iStatus);
		CActiveScheduler::Start();

		TEST2(active->iStatus.Int(), KErrNone);
		TEST(event->EventType() == KLogCallEventTypeUid);
		TEST(event->RemoteParty() == KTestRemoteParty1);
		TEST(event->Direction() == KTestDirection1);
		TEST(event->DurationType() == KTestDurationType1);
		TEST(event->Duration() == KTestDuration1);
		TEST(event->Status() == KTestStatus1);
		TEST(event->Subject() == KTestSubject1);
		TEST(event->Number() == KTestNumber1);
		TEST(event->Link() == KTestLink1);
		TEST(event->Data() == KTestData1);
		TEST(event->Flags() == eventFlags);
		TEST(event->Contact()==(TLogContactItemId) count+1);
		CleanupStack::PopAndDestroy(); // event;
		}

	CleanupStack::PopAndDestroy(3, active); // Delete view, changeObs, active
	}

/**
 This code tests that the log engine will restart automatically
 by killing the Log engine server and then checking that
 a test event can be added.
 Details can be found in DEF047320
 
 */
void doTestsL() 
	{
	TheMatchingIsEnabled = TestUtils::MatchingEnabledL();
	
	// test 0 adds an event
	TestUtils::Initialize(_L("t_logservercrash"));
	TestUtils::DeleteDatabaseL();

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CLogChangeNotifier* notifier = CLogChangeNotifier::NewL();
	CleanupStack::PushL(notifier);

	TheTest.Start(_L("Event View With No Filter: Before Killing Sever"));
	TheTest.Next(_L("Test Add Event"));
	TestEventViewL(*client);
	theLog.Write(_L8("Test 0 OK\n"));

	// Test 1 stops the server
	TheTest.Next(_L("Test Kill Server"));
	TestKillServerL();
	theLog.Write(_L8("Test 1 OK\n"));

	// Test 2 checks the server is going again
	TheTest.Next(_L("Event View With No Filter : After Killing Sever"));
	TheTest.Next(_L("Test Add Event"));
	TestEventViewL(*client);
	theLog.Write(_L8("Test 2 OK\n"));

	CleanupStack::PopAndDestroy(2);
	}
