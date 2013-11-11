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

#include <s32file.h>
#include <logview.h>
#include "t_logutil2.h"

RTest TheTest(_L("t_logheap"));

const TUid KTestEventUid = {0x10005393};
_LIT(KTestEventDesc1, "Event Type Description");
_LIT(KTestEventDesc2, "Changed Event Description");
_LIT(KTestRemoteParty1, "Remote Party");
_LIT(KTestRemoteParty2, "Changed Remote Party");
_LIT(KTestDirection1, "Direction");
_LIT(KTestDirection2, "Changed Direction");
const TLogDurationType KTestDurationType1 = 1;
const TLogDurationType KTestDurationType2 = 2;
const TLogDuration KTestDuration1 = 0x1234;
const TLogDuration KTestDuration2 = 0x1234567;
_LIT(KTestStatus1, "Status");
_LIT(KTestStatus2, "Changed Status");
_LIT(KTestSubject1, "Subject");
_LIT(KTestSubject2, "Changed Subject");
_LIT(KTestNumber1, "Number");
_LIT(KTestNumber2, "Changed Number");
const TLogContactItemId KTestContact1 = 0x1234;
const TLogContactItemId KTestContact2 = 0x1234567;
const TLogLink KTestLink1 = 0x1234;
const TLogLink KTestLink2 = 0x1234567;
_LIT8(KTestData1, "ABCDEFGH");
_LIT8(KTestData2, "IJKLMNOPQRSTUVWXYZ");
const TLogSize KTestMaxLogSize = 0xFFF;
const TLogRecentSize KTestMaxRecentLogSize = 0xF;
const TLogAge KTestMaxEventAge = 0xFFFFFFF;

_LIT(KCreateDb, "Create Db");
_LIT(KOpenDb,   "Open Db");

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
_LIT(KServerSide, "server side");
_LIT(KClientSide, "client side");
#endif

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0884
@SYMTestCaseDesc	    Tests for functionality of CLogEvent,CTestActive classes
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for no memory errors.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestStartup1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0884 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	TTime now;
	now.UniversalTime();

	event->SetEventType(KLogCallEventTypeUid);

	active->StartL();
	client->AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	TBool finished = EFalse;
	TInt failCount = 0;

	while(!finished)
		{
		__UHEAP_FAILNEXT(++failCount);
		client->ChangeEvent(*event, active->iStatus);
		__UHEAP_RESET;
		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			{
			finished = ETrue;
			}
		else
			{
			TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		}

	CleanupStack::PopAndDestroy(3); // client, active, event
    TheTest.Printf(_L("===CLogClient::ChangeEvent() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0885
@SYMTestCaseDesc	    Tests for CLogEvent,CTestActive classes
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for all basic functions of CLogEvent,CTestActive classes.Check for ErrNone flag.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestBasicL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0885 "));
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	TTime now;
	now.UniversalTime();

	event->SetEventType(KLogCallEventTypeUid);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(event->EventType() == KLogCallEventTypeUid);
	TEST(event->Description().Length() > 0);
	TEST(event->Time() >= now);
	now = event->Time();

	TLogId id = event->Id();

	event->SetRemoteParty(KTestRemoteParty1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KTestDurationType1);
	event->SetDuration(KTestDuration1);
	event->SetStatus(KTestStatus1);
	event->SetSubject(KTestSubject1);
	event->SetNumber(KTestNumber1);
	event->SetContact(KTestContact1);
	event->SetLink(KTestLink1);
	event->SetDataL(KTestData1);

	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(event->Id() == id);
	TEST(event->EventType() == KLogCallEventTypeUid);
	TEST(event->Description().Length() > 0);
	TEST(event->Time() == now);
	TEST(event->RemoteParty() == KTestRemoteParty1);
	TEST(event->Direction() == KTestDirection1);
	TEST(event->DurationType() == KTestDurationType1);
	TEST(event->Duration() == KTestDuration1);
	TEST(event->Status() == KTestStatus1);
	TEST(event->Subject() == KTestSubject1);
	TEST(event->Number() == KTestNumber1);
	TEST(event->Contact() == KTestContact1);
	TEST(event->Link() == KTestLink1);
	TEST(event->Data() == KTestData1);

	CleanupStack::PopAndDestroy(); // event;

	event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(id);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(event->Id() == id);
	TEST(event->EventType() == KLogCallEventTypeUid);
	TEST(event->Description().Length() > 0);
	TEST(event->Time() == now);
	TEST(event->RemoteParty() == KTestRemoteParty1);
	TEST(event->Direction() == KTestDirection1);
	TEST(event->DurationType() == KTestDurationType1);
	TEST(event->Duration() == KTestDuration1);
	TEST(event->Status() == KTestStatus1);
	TEST(event->Subject() == KTestSubject1);
	TEST(event->Number() == KTestNumber1);
	TEST(event->Contact() == KTestContact1);
	TEST(event->Link() == KTestLink1);
	TEST(event->Data() == KTestData1);

	active->StartL();
	aClient.DeleteEvent(id, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(2, active); // event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0886
@SYMTestCaseDesc	    Tests for new CLogClient object creation
@SYMTestPriority 	    High
@SYMTestActions  	    Create a CLogClient object on heap.Check for memory errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestConstruction1L(TBool aCreateDb)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0886 "));

	if(aCreateDb)
	    {
	    TestUtils::DeleteDatabaseL();
	    }
		
	CLogClient* client = NULL;
	TInt failCount = 0;

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__UHEAP_FAILNEXT(++failCount);

		TRAP(error, client = CLogClient::NewL(theFs));

		__UHEAP_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;
			TestBasicL(*client);
			}
		else
			TEST2(error, KErrNoMemory);
		}

	delete client;
    TheTest.Printf(_L("==CLogClient::NewL()+\"%S\" client side OOM test succeeded at iteration %d\n"), aCreateDb ? &KCreateDb : &KOpenDb, failCount);
	}

/**s
@SYMTestCaseID          SYSLIB-LOGENG-CT-0887
@SYMTestCaseDesc	    Tests for creation of new CLogClient object 
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors and error in the environment 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestConstruction3L(TBool aCreateDb)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0887 "));

    if(aCreateDb)
        {
        TestUtils::DeleteDatabaseL();
        }

	CLogClient* client = NULL;

	TInt failCount = 0;

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__LOGSERV_UHEAP_FAILNEXT(++failCount);

		TRAP(error, client = CLogClient::NewL(theFs));

		__LOGSERV_UHEAP_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;
			TestBasicL(*client);
			}
		else
			TEST2(error, KErrNoMemory);
		}

	delete client;
    TheTest.Printf(_L("==CLogClient::NewL()+\"%S\" server side OOM test succeeded at iteration %d\n"), aCreateDb ? &KCreateDb : &KOpenDb, failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0888
@SYMTestCaseDesc	    Tests for CLogClient::AddEventType() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for adding events to the log
                        Check for memory errors and error in the environment 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestAddEventType1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0888 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc1);
	type->SetLoggingEnabled(ETrue);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		__UHEAP_FAILNEXT(++failCount);
		client->AddEventType(*type, active->iStatus);
	
		active->StartL();
		CActiveScheduler::Start();
		
		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetEventType(*type, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNotFound);
			}
		else
			TEST2(error, KErrNone);

		}

	active->StartL();
	client->AddEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrAlreadyExists);

	CleanupStack::PopAndDestroy(3, client); // active, type, client
   TheTest.Printf(_L("===CLogClient::AddEventType() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0889
@SYMTestCaseDesc	    Tests for CLogClient::AddEventType() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for adding events to the log
                        Check for memory errors and error in the environment 
						Test for adding the same event and check for already exits error.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestAddEventType3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0889 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc1);
	type->SetLoggingEnabled(ETrue);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TInt failCount = 0;

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->AddEventType(*type, active->iStatus);
	
		active->StartL();
		CActiveScheduler::Start();
		
		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__LOGSERV_UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetEventType(*type, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNotFound);
			}
		else
			TEST2(error, KErrNone);

		}

	active->StartL();
	client->AddEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrAlreadyExists);

	CleanupStack::PopAndDestroy(3, client); // active, type, client
	TheTest.Printf(_L("===CLogClient::AddEventType() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0890
@SYMTestCaseDesc	    Tests for CLogClient::GetEventType() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for getting event type for the specific UID  from the log
                        Check for memory errors and error in the environment
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetEventType1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0890 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		__UHEAP_FAILNEXT(++failCount);

		client->GetEventType(*type, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();
		
		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			{
			TEST2(active->iStatus.Int(), KErrNoMemory);
			TEST(type->Description() == KNullDesC);
			}

		__UHEAP_RESET;
		}

	TEST(type->Uid() == KTestEventUid);
	TEST(type->Description() == KTestEventDesc1);
	TEST(type->LoggingEnabled());

	CleanupStack::PopAndDestroy(3, client); // active, type, client
    TheTest.Printf(_L("===CLogClient::GetEventType() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0891
@SYMTestCaseDesc	    Tests for CLogClient::GetEventType() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for getting event type for the specific UID  from the log
                        Check for memory errors and error in the environment
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetEventType3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0891 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		__LOGSERV_UHEAP_FAILNEXT(++failCount);

		client->GetEventType(*type, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();
		
		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			{
			TEST2(active->iStatus.Int(), KErrNoMemory);
			TEST(type->Description() == KNullDesC);
			}

		__LOGSERV_UHEAP_RESET;
		}

	TEST(type->Uid() == KTestEventUid);
	TEST(type->Description() == KTestEventDesc1);
	TEST(type->LoggingEnabled());

	CleanupStack::PopAndDestroy(3, client); // active, type, client
    TheTest.Printf(_L("===CLogClient::GetEventType() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0892
@SYMTestCaseDesc	    Tests for CLogClient::ChangeEventType() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for changing the event type 
                        Check for memory errors and error in the environment 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEventType1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0892 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	type->SetDescription(KTestEventDesc1);
	type->SetDescription(KTestEventDesc2);

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		type->SetDescription(KTestEventDesc2);
		type->SetLoggingEnabled(EFalse);

		__UHEAP_FAILNEXT(++failCount);
		
		client->ChangeEventType(*type, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetEventType(*type, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(type->Description() == KTestEventDesc1);
			TEST(type->LoggingEnabled());
			}
		else
			TEST2(error, KErrNone);
		}

	type->SetUid(KTestEventUid);

	active->StartL();
	client->GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(type->Uid() == KTestEventUid);
	TEST(type->Description() == KTestEventDesc2);
	TEST(type->LoggingEnabled() == EFalse);

	CleanupStack::PopAndDestroy(3, client); // type, active, client
    TheTest.Printf(_L("===CLogClient::ChangeEventType() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0893
@SYMTestCaseDesc	    Tests for CLogClient::ChangeEventType() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for changing the event type. 
                        Check for memory errors and error in the environment 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEventType3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0893 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	type->SetDescription(KTestEventDesc1);
	type->SetDescription(KTestEventDesc2);

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		type->SetDescription(KTestEventDesc2);
		type->SetLoggingEnabled(EFalse);

		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		
		client->ChangeEventType(*type, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__LOGSERV_UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetEventType(*type, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(type->Description() == KTestEventDesc1);
			TEST(type->LoggingEnabled());
			}
		else
			TEST2(error, KErrNone);
		}

	type->SetUid(KTestEventUid);

	active->StartL();
	client->GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(type->Uid() == KTestEventUid);
	TEST(type->Description() == KTestEventDesc2);
	TEST(type->LoggingEnabled() == EFalse);

	CleanupStack::PopAndDestroy(3, client); // type, active, client
    TheTest.Printf(_L("===CLogClient::ChangeEventType() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0894
@SYMTestCaseDesc	    Tests for CLogClient::DeleteEventType() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for deleting of event type.
                        After deletion check by trying to retrieve the event.
						Check for ErrNotFound,error flag
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDeleteEventType1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0894 "));

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		__UHEAP_FAILNEXT(++failCount);
		client->DeleteEventType(KTestEventUid, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetEventType(*type, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}
		else
			TEST2(error, KErrNone);
		}

	active->StartL();
	client->GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(3, client); // type, active, client
    TheTest.Printf(_L("===CLogClient::DeleteEventType() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0895
@SYMTestCaseDesc	    Tests for CLogClient::DeleteEventType() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for deleting of event type.
                        After deletion check by trying to retrieve the event.
						Check for ErrNotFound,error flag.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDeleteEventType3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0895 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	TInt failCount = 0;

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->DeleteEventType(KTestEventUid, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__LOGSERV_UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetEventType(*type, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}
		else
			TEST2(error, KErrNone);
		}

	active->StartL();
	client->GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(3, client); // type, active, client
    TheTest.Printf(_L("===CLogClient::DeleteEventType() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0896
@SYMTestCaseDesc	    Tests for adding event CLogClient::AddEvent() function  
@SYMTestPriority 	    High
@SYMTestActions  	    Add logeventtype and logevent to the log
                        Tests for memory errors and environment errors.
@SYMTestExpectedResults Test must not fail 
@SYMREQ                 REQ0000
*/
LOCAL_C void TestAddEvent1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0896 "));

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc1);
	type->SetLoggingEnabled(ETrue);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	active->StartL();
	client->AddEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	TTime now;
	now.UniversalTime();

	event->SetEventType(KTestEventUid);
	event->SetRemoteParty(KTestRemoteParty1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KTestDurationType1);
	event->SetDuration(KTestDuration1);
	event->SetStatus(KTestStatus1);
	event->SetSubject(KTestSubject1);
	event->SetNumber(KTestNumber1);
	event->SetContact(KTestContact1);
	event->SetLink(KTestLink1);
	event->SetDataL(KTestData1);

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		__UHEAP_FAILNEXT(++failCount);
		client->AddEvent(*event, active->iStatus);
		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__UHEAP_RESET;

		if(error != KErrNoMemory)
			{
			TEST2(error, KErrNone);
			}
		}

	TEST(event->Time() >= now);
	TEST(event->Description() == KTestEventDesc1);

	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(4, client); // event, active, type, client
    TheTest.Printf(_L("===CLogClient::AddEvent() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0897
@SYMTestCaseDesc	    Tests for adding event CLogClient::AddEvent() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Add logeventtype and logevent to the log
                        Tests for memory errors and environment errors.
@SYMTestExpectedResults Test must not fail 
@SYMREQ                 REQ0000
*/
LOCAL_C void TestAddEvent3L()
	{
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0897 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc1);
	type->SetLoggingEnabled(ETrue);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	active->StartL();
	client->AddEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	TTime now;
	now.UniversalTime();

	event->SetEventType(KTestEventUid);
	event->SetRemoteParty(KTestRemoteParty1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KTestDurationType1);
	event->SetDuration(KTestDuration1);
	event->SetStatus(KTestStatus1);
	event->SetSubject(KTestSubject1);
	event->SetNumber(KTestNumber1);
	event->SetContact(KTestContact1);
	event->SetLink(KTestLink1);
	event->SetDataL(KTestData1);

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->AddEvent(*event, active->iStatus);
		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__LOGSERV_UHEAP_RESET;

		if(error != KErrNoMemory)
			{
			TEST2(error, KErrNone);
			}
		}

	TEST(event->Time() >= now);
	TEST(event->Description() == KTestEventDesc1);

	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(4, client); // event, active, type, client
    TheTest.Printf(_L("===CLogClient::AddEvent() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0898
@SYMTestCaseDesc	    Tests for retrieving details of an event 
                        Tests for CLogClient::GetEvent() function.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for getting event details.Check for memory errors and error in the environment
                        Tests for the integrity of the information
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetEvent1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0898 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(0);

	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		__UHEAP_FAILNEXT(++failCount);
		client->GetEvent(*event, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);

		__UHEAP_RESET;
		}

	TEST(event->Id() == 0);
	TEST(event->Time() > TTime(0));
	TEST(event->Description() == KTestEventDesc1);
	TEST(event->EventType() == KTestEventUid);
	TEST(event->RemoteParty() == KTestRemoteParty1);
	TEST(event->Direction() == KTestDirection1);
	TEST(event->DurationType() == KTestDurationType1);
	TEST(event->Duration() == KTestDuration1);
	TEST(event->Status() == KTestStatus1);
	TEST(event->Subject() == KTestSubject1);
	TEST(event->Number() == KTestNumber1);
	TEST(event->Contact() == KTestContact1);
	TEST(event->Link() == KTestLink1);
	TEST(event->Data() == KTestData1);

	CleanupStack::PopAndDestroy(3, client); // event, active, client
    TheTest.Printf(_L("===CLogClient::AddEvent() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0899
@SYMTestCaseDesc	    Tests for retrieving details of an event.Tests for CLogClient::GetEventL() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for getting event details of UID 0.Check for memory errors and error in the environment
                        Tests for the integrity of the information
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetEvent3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0899 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(0);

	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->GetEvent(*event, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);

		__LOGSERV_UHEAP_RESET;
		}

	TEST(event->Id() == 0);
	TEST(event->Time() > TTime(0));
	TEST(event->Description() == KTestEventDesc1);
	TEST(event->EventType() == KTestEventUid);
	TEST(event->RemoteParty() == KTestRemoteParty1);
	TEST(event->Direction() == KTestDirection1);
	TEST(event->DurationType() == KTestDurationType1);
	TEST(event->Duration() == KTestDuration1);
	TEST(event->Status() == KTestStatus1);
	TEST(event->Subject() == KTestSubject1);
	TEST(event->Number() == KTestNumber1);
	TEST(event->Contact() == KTestContact1);
	TEST(event->Link() == KTestLink1);
	TEST(event->Data() == KTestData1);

	CleanupStack::PopAndDestroy(3, client); // event, active, client
    TheTest.Printf(_L("===CLogClient::GetEvent() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0900
@SYMTestCaseDesc	    Tests for changing the log event details.
                        Tests for CLogClient::ChangeEvent() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for changing the event details.Check for memory errors and error in the environment
                        If no memory error,get the event details and check for the integrity.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEvent3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0900 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	TTime now;
	now.UniversalTime();

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		event->SetId(0);
		event->SetTime(now);
		event->SetRemoteParty(KTestRemoteParty2);
		event->SetDirection(KTestDirection2);
		event->SetDurationType(KTestDurationType2);
		event->SetDuration(KTestDuration2);
		event->SetStatus(KTestStatus2);
		event->SetSubject(KTestSubject2);
		event->SetNumber(KTestNumber2);
		event->SetContact(KTestContact2);
		event->SetLink(KTestLink2);
		event->SetDataL(KTestData2);

		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->ChangeEvent(*event, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();
		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__LOGSERV_UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(event->Id() == 0);
			TEST(event->Time() > TTime(0));
			TEST(event->Description() == KTestEventDesc1);
			TEST(event->EventType() == KTestEventUid);
			TEST(event->RemoteParty() == KTestRemoteParty1);
			TEST(event->Direction() == KTestDirection1);
			TEST(event->DurationType() == KTestDurationType1);
			TEST(event->Duration() == KTestDuration1);
			TEST(event->Status() == KTestStatus1);
			TEST(event->Subject() == KTestSubject1);
			TEST(event->Number() == KTestNumber1);
			TEST(event->Contact() == KTestContact1);
			TEST(event->Link() == KTestLink1);
			TEST(event->Data() == KTestData1);
			}
		else
			TEST2(error, KErrNone);
		}

	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(event->Id() == 0);
	TEST(event->Time() == now);
	TEST(event->Description() == KTestEventDesc1);
	TEST(event->EventType() == KTestEventUid);
	TEST(event->RemoteParty() == KTestRemoteParty2);
	TEST(event->Direction() == KTestDirection2);
	TEST(event->DurationType() == KTestDurationType2);
	TEST(event->Duration() == KTestDuration2);
	TEST(event->Status() == KTestStatus2);
	TEST(event->Subject() == KTestSubject2);
	TEST(event->Number() == KTestNumber2);
	TEST(event->Contact() == KTestContact2);
	TEST(event->Link() == KTestLink2);
	TEST(event->Data() == KTestData2);

	CleanupStack::PopAndDestroy(3, client); // event, active, client
    TheTest.Printf(_L("===CLogClient::ChangeEvent() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0901
@SYMTestCaseDesc	    Tests for changing the log event details.Tests for CLogClient::ChangeEvent() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for changing the event details.Check for memory errors and error in the environment
                        If no memory error,get the event details and check for the integrity.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEvent1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0901 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	TTime now;
	now.UniversalTime();

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		event->SetId(0);
		event->SetTime(now);
		event->SetRemoteParty(KTestRemoteParty2);
		event->SetDirection(KTestDirection2);
		event->SetDurationType(KTestDurationType2);
		event->SetDuration(KTestDuration2);
		event->SetStatus(KTestStatus2);
		event->SetSubject(KTestSubject2);
		event->SetNumber(KTestNumber2);
		event->SetContact(KTestContact2);
		event->SetLink(KTestLink2);
		event->SetDataL(KTestData2);

		__UHEAP_FAILNEXT(++failCount);
		client->ChangeEvent(*event, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();
		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(event->Id() == 0);
			TEST(event->Time() > TTime(0));
			TEST(event->Description() == KTestEventDesc1);
			TEST(event->EventType() == KTestEventUid);
			TEST(event->RemoteParty() == KTestRemoteParty1);
			TEST(event->Direction() == KTestDirection1);
			TEST(event->DurationType() == KTestDurationType1);
			TEST(event->Duration() == KTestDuration1);
			TEST(event->Status() == KTestStatus1);
			TEST(event->Subject() == KTestSubject1);
			TEST(event->Number() == KTestNumber1);
			TEST(event->Contact() == KTestContact1);
			TEST(event->Link() == KTestLink1);
			TEST(event->Data() == KTestData1);
			}
		else
			TEST2(error, KErrNone);
		}

	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(event->Id() == 0);
	TEST(event->Time() == now);
	TEST(event->Description() == KTestEventDesc1);
	TEST(event->EventType() == KTestEventUid);
	TEST(event->RemoteParty() == KTestRemoteParty2);
	TEST(event->Direction() == KTestDirection2);
	TEST(event->DurationType() == KTestDurationType2);
	TEST(event->Duration() == KTestDuration2);
	TEST(event->Status() == KTestStatus2);
	TEST(event->Subject() == KTestSubject2);
	TEST(event->Number() == KTestNumber2);
	TEST(event->Contact() == KTestContact2);
	TEST(event->Link() == KTestLink2);
	TEST(event->Data() == KTestData2);

	CleanupStack::PopAndDestroy(3, client); // event, active, client
    TheTest.Printf(_L("===CLogClient::ChangeEvent() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0902
@SYMTestCaseDesc	    Tests for deleting an event from the main event log.Tests for CLogClient::DeleteEvent() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for deleting an event with unique ID of 0,check for any memory errors
                        Try to get the deleted event and test for not found error.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDeleteEvent1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0902 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(0);

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		__UHEAP_FAILNEXT(++failCount);
		client->DeleteEvent(0, active->iStatus);

		if (error == KErrNone)
			{
			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus == KErrNone)
				finished = ETrue;
			else
				error = active->iStatus.Int();
			}

		__UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}
		else
			TEST2(error, KErrNone);
		}

	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(3); // event, active, client
    TheTest.Printf(_L("===CLogClient::DeleteEvent() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0903
@SYMTestCaseDesc	    Tests for deleting an event from the main event log
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for deleting an event with unique ID of 0,check for any memory errors
                        Try to get the deleted event and test for not found error.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDeleteEvent3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0903 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(0);

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->DeleteEvent(0, active->iStatus);

		if (error == KErrNone)
			{
			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus == KErrNone)
				finished = ETrue;
			else
				error = active->iStatus.Int();
			}

		__LOGSERV_UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}
		else
			TEST2(error, KErrNone);
		}

	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(3, client); // event, active, client
    TheTest.Printf(_L("===CLogClient::DeleteEvent() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0904
@SYMTestCaseDesc	    Tests for CLogClient::GetConfig() function
@SYMTestPriority 	    High
@SYMTestActions  	    Set the configuration data with zero values.
                        Get the log engine configuration data,check for any memory errors.
						Tests for the integrity of the data
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetConfig1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0904 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig config;

	TEST(config.iMaxEventAge == 0);
	TEST(config.iMaxLogSize == 0);
	TEST(config.iMaxRecentLogSize == 0);

	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		__UHEAP_FAILNEXT(++failCount);
		client->GetConfig(config, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);

		__UHEAP_RESET;
		}

	TEST(config.iMaxEventAge > 0);
	TEST(config.iMaxLogSize > 0);
	TEST(config.iMaxRecentLogSize > 0);

	CleanupStack::PopAndDestroy(2, client); // active, client
    TheTest.Printf(_L("===CLogClient::GetConfig() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0905
@SYMTestCaseDesc	    Tests for CLogClient::GetConfig() function
@SYMTestPriority 	    High
@SYMTestActions  	    Set the configuration data with zero values.
                        Get the log engine configuration data,check for any memory errors.
						Tests for the integrity of the data
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetConfig3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0905 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig config;

	TEST(config.iMaxEventAge == 0);
	TEST(config.iMaxLogSize == 0);
	TEST(config.iMaxRecentLogSize == 0);

	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->GetConfig(config, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);

		__LOGSERV_UHEAP_RESET;
		}

	TEST(config.iMaxEventAge > 0);
	TEST(config.iMaxLogSize > 0);
	TEST(config.iMaxRecentLogSize > 0);

	CleanupStack::PopAndDestroy(2, client); // active, client
    TheTest.Printf(_L("===CLogClient::GetConfig() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0906
@SYMTestCaseDesc	    Tests for CLogClient::ChangeConfig() function
@SYMTestPriority 	    High
@SYMTestActions  	    Get the configuration data and store in temporary object.
                        Set the new configuration data and change the log engine configuration,check for no memory error
                        If no memory error,get the configuration data and test for the new changes of the data with the old data.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeConfig1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0906 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig configOld;

	active->StartL();
	client->GetConfig(configOld, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TLogConfig config;

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		config.iMaxLogSize = KTestMaxLogSize;
		config.iMaxRecentLogSize = KTestMaxRecentLogSize;
		config.iMaxEventAge = KTestMaxEventAge;

		__UHEAP_FAILNEXT(++failCount);
		client->ChangeConfig(config, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();
		
		__UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetConfig(config, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(config.iMaxLogSize == configOld.iMaxLogSize);
			TEST(config.iMaxRecentLogSize == configOld.iMaxRecentLogSize);
			TEST(config.iMaxEventAge == configOld.iMaxEventAge);
			}
		else
			TEST2(error, KErrNone);		
		}

	TEST(config.iMaxLogSize == KTestMaxLogSize);
	TEST(config.iMaxRecentLogSize == KTestMaxRecentLogSize);
	TEST(config.iMaxEventAge == KTestMaxEventAge);

	CleanupStack::PopAndDestroy(2, client); // active, client
    TheTest.Printf(_L("===CLogClient::ChangeConfig() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0907
@SYMTestCaseDesc	    Tests for CLogClient::ChangeConfig() function
@SYMTestPriority 	    High
@SYMTestActions  	    Get the configuration data and store in temporary object.
                        Set the new configuration data and change the log engine configuration,check for no memory error
                        If no memory error,get the configuration data and test for the new changes of the data with the old data.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeConfig3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0907 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig configOld;

	active->StartL();
	client->GetConfig(configOld, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TLogConfig config;

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		config.iMaxLogSize = KTestMaxLogSize;
		config.iMaxRecentLogSize = KTestMaxRecentLogSize;
		config.iMaxEventAge = KTestMaxEventAge;

		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->ChangeConfig(config, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();
		
		__LOGSERV_UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			client->GetConfig(config, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(config.iMaxLogSize == configOld.iMaxLogSize);
			TEST(config.iMaxRecentLogSize == configOld.iMaxRecentLogSize);
			TEST(config.iMaxEventAge == configOld.iMaxEventAge);
			}
		else
			TEST2(error, KErrNone);		
		}

	TEST(config.iMaxLogSize == KTestMaxLogSize);
	TEST(config.iMaxRecentLogSize == KTestMaxRecentLogSize);
	TEST(config.iMaxEventAge == KTestMaxEventAge);

	CleanupStack::PopAndDestroy(2, client); // active, client
    TheTest.Printf(_L("===CLogClient::ChangeConfig() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0908
@SYMTestCaseDesc	    Tests for CLogClient::GetString() function
@SYMTestPriority 	    High
@SYMTestActions  	    Get the standard string name from the resource.
                        Tests for no memory error and null string.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetString1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0908 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	TBuf<KLogMaxSharedStringLength> str;
	
	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__UHEAP_FAILNEXT(++failCount);

		error = client->GetString(str, R_LOG_DIR_IN);

		__UHEAP_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;
			TEST(str.Length() > 0);
			}
		else
			{
			TEST2(error, KErrNoMemory);
			TEST(str.Length() == 0);
			}
		}
	CleanupStack::PopAndDestroy(client);
    TheTest.Printf(_L("===CLogClient::GetString() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0909
@SYMTestCaseDesc	    Tests for CLogClient::ClearLog() function
@SYMTestPriority 	    High
@SYMTestActions  	    Add 4 events to the event log,check for no errors
                        Clear the log by calling ClearLog function.
						Try for getting the before added 4 events and check for not found error.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestClearEventLog1L(TBool aMicroSeconds)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0909 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	TTime now;
	now.UniversalTime();
	
	TDateTime dt(now.DateTime());
	if(!aMicroSeconds)
		dt.SetMicroSecond(0);
	dt.SetYear(dt.Year() - 1);
	TTime date1(dt);

	dt.SetYear(dt.Year() - 1);
	TTime date2(dt);

	TTime ClearThreshold(date1);
	ClearThreshold -= TTimeIntervalSeconds(1);
	/*need to use set the clear threshold (at least 1 sec) before the date of event 1,2.
	Otherwise the test will fail if now happens to occur at 0 microseconds (e.g the  
	last 6 digit of now is 0) */
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event1 = CLogEvent::NewL();
	CleanupStack::PushL(event1);
	event1->SetEventType(KTestEventUid);

	active->StartL();
	client->AddEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event1->SetTime(date1);

	active->StartL();
	client->ChangeEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);
	event2->SetEventType(KTestEventUid);

	active->StartL();
	client->AddEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event2->SetTime(date1);

	active->StartL();
	client->ChangeEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event3 = CLogEvent::NewL();
	CleanupStack::PushL(event3);
	event3->SetEventType(KTestEventUid);

	active->StartL();
	client->AddEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event3->SetTime(date2);

	active->StartL();
	client->ChangeEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event4 = CLogEvent::NewL();
	CleanupStack::PushL(event4);
	event4->SetEventType(KTestEventUid);

	active->StartL();
	client->AddEvent(*event4, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event4->SetTime(date2);

	active->StartL();
	client->ChangeEvent(*event4, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		__UHEAP_FAILNEXT(++failCount);

		client->ClearLog(ClearThreshold, active->iStatus);

		__UHEAP_RESET;

		active->StartL();	
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);
		}

	active->StartL();
	client->GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	if (active->iStatus != KErrNone) 
	    TheTest.Printf(_L("error code:%d failcoount:%d\n"),active->iStatus.Int(),failCount);
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	client->GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	client->GetEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	active->StartL();
	client->GetEvent(*event4, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	active->StartL();
	client->ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	client->GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	active->StartL();
	client->GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(6, client); // event4, event3, event2, event1, active, client
    TheTest.Printf(_L("===CLogClient::ClearLog() client side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0910
@SYMTestCaseDesc	    Tests for CLogClient::ClearLog() function
@SYMTestPriority 	    High
@SYMTestActions  	    Add 4 events to the event log,check for no errors
                        Clear the log by calling ClearLog function.
						Try for getting the before added 4 events and check for not found error.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestClearEventLog3L(TBool aMicroSeconds)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0910 "));
	
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	TTime now;
	now.UniversalTime();
	
	TDateTime dt(now.DateTime());
	if(!aMicroSeconds)
		dt.SetMicroSecond(0);
	dt.SetYear(dt.Year() - 1);
	TTime date1(dt);

	dt.SetYear(dt.Year() - 1);
	TTime date2(dt);
	
	TTime ClearThreshold(date1);
	ClearThreshold -= TTimeIntervalSeconds(1);
	/*need to use set the clear threshold (at least 1 sec) before the date of event 1,2.
	Otherwise the test will fail if now happens to occur at 0 microseconds (e.g the  
	last 6 digit of now is 0) */
	
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event1 = CLogEvent::NewL();
	CleanupStack::PushL(event1);
	event1->SetEventType(KTestEventUid);

	active->StartL();
	client->AddEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event1->SetTime(date1);

	active->StartL();
	client->ChangeEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);
	event2->SetEventType(KTestEventUid);

	active->StartL();
	client->AddEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event2->SetTime(date1);

	active->StartL();
	client->ChangeEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event3 = CLogEvent::NewL();
	CleanupStack::PushL(event3);
	event3->SetEventType(KTestEventUid);

	active->StartL();
	client->AddEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event3->SetTime(date2);

	active->StartL();
	client->ChangeEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event4 = CLogEvent::NewL();
	CleanupStack::PushL(event4);
	event4->SetEventType(KTestEventUid);

	active->StartL();
	client->AddEvent(*event4, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event4->SetTime(date2);

	active->StartL();
	client->ChangeEvent(*event4, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		__LOGSERV_UHEAP_FAILNEXT(++failCount);

		client->ClearLog(ClearThreshold, active->iStatus);

		__LOGSERV_UHEAP_RESET;

		active->StartL();	
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);
		}

	active->StartL();
	client->GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	client->GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	client->GetEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	active->StartL();
	client->GetEvent(*event4, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	active->StartL();
	client->ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	client->GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	active->StartL();
	client->GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(6, client); // event4, event3, event2, event1, active, client
    TheTest.Printf(_L("===CLogClient::ClearLog() server side OOM test succeeded at iteration %d\n"), failCount);
	}
	
	/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1910
@SYMTestCaseDesc	    Tests for CLogClient::ClearLog() under boundary conditions
@SYMTestPriority 	    High
@SYMTestActions  	    Add 4 events to the event log,check for no errors
                        Clear the log by calling ClearLog function. 
                        However using the date for 2 events the same as the clear date threshold.
						Try for getting the before added 4 events and check for not found error.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF093601
*/
LOCAL_C void TestDEF093601L(TBool aClientTest)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1910 "));
	if(aClientTest)
		TestClearEventLog3L(EFalse);
	else
		TestClearEventLog1L(EFalse);
	}

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

/**
@SYMTestCaseID			PDS-LOGENG-UT-4038
@SYMTestCaseDesc		Clear log events with specific SimId - client/server side OOM test.
						The test adds 2 events, one of them with a SimId. Then the test
						executes CLogClient::ClearLog() in an OOM loop.
@SYMTestActions			Clear log events with specific SimId - client/server side OOM test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12748
*/
void ClearLogSimIdOomTestL(TBool aServerSideOomTest)
	{//This test case is compiled only when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM macro is defined
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	const TSimId KSimId = 4100000000U;
	TTime now;
	now.UniversalTime();
	
	TDateTime dt(now.DateTime());
	dt.SetYear(dt.Year() - 1);
	TTime date(dt);
	
	TTime threshold(date);
	threshold += TTimeIntervalSeconds(10);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	
	//////// Event1 ///////////////////////////
	CLogEvent* event1 = CLogEvent::NewL();
	CleanupStack::PushL(event1);
	event1->SetEventType(KTestEventUid);

	active->StartL();
	client->AddEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event1->SetTime(date);

	active->StartL();
	client->ChangeEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//////// Event2 ///////////////////////////
	CLogEvent* event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);
	event2->SetEventType(KTestEventUid);

	active->StartL();
	client->AddEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event2->SetTime(date);
	event2->SetSimId(KSimId);

	active->StartL();
	client->ChangeEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	////////////////////////////////////////////
	
	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		if(aServerSideOomTest)
			{
			__LOGSERV_UHEAP_FAILNEXT(++failCount);
			}
		else
			{
			__UHEAP_FAILNEXT(++failCount);
			}

		client->ClearLog(threshold, KSimId, active->iStatus);

		if(aServerSideOomTest)
			{
			__LOGSERV_UHEAP_RESET;
			}
		else
			{
			__UHEAP_RESET;
			}

		active->StartL();	
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);
		}

	//Check: event1 is without SimId and must not be deleted. event2 is with SimId and should be deleted. 
	active->StartL();
	client->GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	client->GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	//Delete all events
	active->StartL();
	client->ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	active->StartL();
	client->GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	active->StartL();
	client->GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);	
	
	CleanupStack::PopAndDestroy(4, client); //event2, event1, active, client
    TheTest.Printf(_L("===CLogClient::ClearLog() + SimId %S OOM test succeeded at iteration %d\n"), aServerSideOomTest ? &KServerSide : &KClientSide, failCount);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4039
@SYMTestCaseDesc		Clear log events with specific SimId and recent list- client/server side OOM test.
						The test adds 2 events to a recent list, one of them with a SimId. Then the test
						executes CLogClient::ClearLog() in an OOM loop.
@SYMTestActions			Clear log events with specific SimId and recent list- client/server side OOM test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12748
*/
void ClearLogRecentSimIdOomTestL(TBool aServerSideOomTest)
	{//This test case is compiled only when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM macro is defined
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	const TSimId KSimId = 4200000000U;
	const TUid KEvTypeUid = {KLogCallEventType};
	_LIT(KEvDirection, "Missed call");
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	
	//////// Event1 ///////////////////////////
	CLogEvent* event1 = CLogEvent::NewL();
	CleanupStack::PushL(event1);
	event1->SetEventType(KEvTypeUid);
	event1->SetDirection(KEvDirection);
	event1->SetNumber(_L("12345678"));

	active->StartL();
	client->AddEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//////// Event2 ///////////////////////////
	CLogEvent* event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);
	event2->SetEventType(KEvTypeUid);
	event2->SetDirection(KEvDirection);
	event2->SetNumber(_L("87654321"));
	event2->SetSimId(KSimId);

	active->StartL();
	client->AddEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	////////////////////////////////////////////
	
	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		if(aServerSideOomTest)
			{
			__LOGSERV_UHEAP_FAILNEXT(++failCount);
			}
		else
			{
			__UHEAP_FAILNEXT(++failCount);
			}

		client->ClearLog(KLogRecentMissedCalls, KSimId, active->iStatus);

		if(aServerSideOomTest)
			{
			__LOGSERV_UHEAP_RESET;
			}
		else
			{
			__UHEAP_RESET;
			}

		active->StartL();	
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);
		}

	//Check: event1 is without SimId and must be presented in the view. Event2 is with SimId and should not be in the view.
	CLogViewRecent* view = CLogViewRecent::NewL(*client);
	CleanupStack::PushL(view);
	TBool rc = view->SetRecentListL(KLogRecentMissedCalls, active->iStatus);
	TEST(rc);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	TInt count = view->CountL();
	TEST2(count, 1);
	rc = view->FirstL(active->iStatus);
	TEST(rc);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	const CLogEvent& e = view->Event();
	TEST2(e.SimId(), KLogNullSimId);
	
	CleanupStack::PopAndDestroy(view);

	//Delete all events
	active->StartL();
	client->ClearLog(KLogRecentMissedCalls, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	//Check: no events in the recent view list 
	view = CLogViewRecent::NewL(*client);
	CleanupStack::PushL(view);
	rc = view->SetRecentListL(KLogRecentMissedCalls, active->iStatus);
	TEST(!rc);
	
	CleanupStack::PopAndDestroy(5, client); //view, event2, event1, active, client
    TheTest.Printf(_L("===CLogClient::ClearLog()/Recent + SimId %S OOM test succeeded at iteration %d\n"), aServerSideOomTest ? &KServerSide : &KClientSide, failCount);
	}

#endif//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

LOCAL_C void doClientFailureL()
	{
	TheTest.Next(_L("Server startup"));
	TestStartup1L();
	User::After(100000);

	TheTest.Next(_L("Construction + create logeng database"));
	TestConstruction1L(ETrue); // Creates database
    TheTest.Next(_L("Construction + open logeng database"));
	TestConstruction1L(EFalse); // Opens existing database
	TestUtils::DeleteDatabaseL();
	theLog.Write(_L8("Test 1.1 OK\n"));

	TheTest.Next(_L("Add Event Type"));
	TestAddEventType1L();
	theLog.Write(_L8("Test 1.2 OK\n"));

	TheTest.Next(_L("Get Event Type"));
	TestGetEventType1L();
	theLog.Write(_L8("Test 1.3 OK\n"));

	TheTest.Next(_L("Change Event Type"));
	TestChangeEventType1L();
	theLog.Write(_L8("Test 1.4 OK\n"));

	TheTest.Next(_L("Delete Event Type"));
	TestDeleteEventType1L();
	theLog.Write(_L8("Test 1.5 OK\n"));

	TheTest.Next(_L("Add Event"));
	TestAddEvent1L();
	theLog.Write(_L8("Test 1.6 OK\n"));

	TheTest.Next(_L("Get Event"));
	TestGetEvent1L();
	theLog.Write(_L8("Test 1.7 OK\n"));

	TheTest.Next(_L("Change Event"));
	TestChangeEvent1L();
	theLog.Write(_L8("Test 1.8 OK\n"));

	TheTest.Next(_L("Delete Event"));
	TestDeleteEvent1L();
	theLog.Write(_L8("Test 1.9 OK\n"));

	TheTest.Next(_L("Get Config"));
	TestGetConfig1L();
	theLog.Write(_L8("Test 1.10 OK\n"));

	TheTest.Next(_L("Change Config"));
	TestChangeConfig1L();
	theLog.Write(_L8("Test 1.11 OK\n"));

	TheTest.Next(_L("Get String"));
	TestGetString1L();
	theLog.Write(_L8("Test 1.12 OK\n"));
	
	TheTest.Next(_L("Clear Event Log"));
	TestClearEventLog1L(ETrue);
	theLog.Write(_L8("Test 1.13 OK\n"));
	
	TheTest.Next(_L("DEF093601"));
	TestDEF093601L(EFalse);
	theLog.Write(_L8("Test 1.14 OK\n"));

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4038 Clear Event Log + SimId"));
	ClearLogSimIdOomTestL(EFalse);
	theLog.Write(_L8("Test 1.15 OK\n"));
	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4039 Clear Event Log/Recent + SimId"));
	ClearLogRecentSimIdOomTestL(EFalse);
	theLog.Write(_L8("Test 1.16 OK\n"));
#endif	
	}

LOCAL_C void doLogServFailureL()
	{
	TheTest.Next(_L("Construction + create logeng database"));
	TestConstruction3L(ETrue); // Creates database
    TheTest.Next(_L("Construction + open logeng database"));
	TestConstruction3L(EFalse); // Opens existing database
	TestUtils::DeleteDatabaseL();
	theLog.Write(_L8("Test 2.1 OK\n"));

	TheTest.Next(_L("Add Event Type"));
	TestAddEventType3L();
	theLog.Write(_L8("Test 2.2 OK\n"));

	TheTest.Next(_L("Get Event Type"));
	TestGetEventType3L();
	theLog.Write(_L8("Test 2.3 OK\n"));

	TheTest.Next(_L("Change Event Type"));
	TestChangeEventType3L();
	theLog.Write(_L8("Test 2.4 OK\n"));

	TheTest.Next(_L("Delete Event Type"));
	TestDeleteEventType3L();
	theLog.Write(_L8("Test 2.5 OK\n"));

	TheTest.Next(_L("Add Event"));
	TestAddEvent3L();
	theLog.Write(_L8("Test 2.6 OK\n"));

	TheTest.Next(_L("Get Event"));
	TestGetEvent3L(); 
	theLog.Write(_L8("Test 2.7 OK\n"));

	TheTest.Next(_L("Change Event"));
	TestChangeEvent3L();
	theLog.Write(_L8("Test 2.8 OK\n"));

	TheTest.Next(_L("Delete Event"));
	TestDeleteEvent3L();
	theLog.Write(_L8("Test 2.9 OK\n"));

	TheTest.Next(_L("Get Config"));
	TestGetConfig3L();
	theLog.Write(_L8("Test 2.10 OK\n"));

	TheTest.Next(_L("Change Config"));
	TestChangeConfig3L();
	theLog.Write(_L8("Test 2.11 OK\n"));

	TheTest.Next(_L("Clear Event Log"));
	TestClearEventLog3L(ETrue);
	theLog.Write(_L8("Test 2.12 OK\n"));
	
	TheTest.Next(_L("DEF093601"));
	TestDEF093601L(ETrue);
	theLog.Write(_L8("Test 2.13 OK\n"));
	
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4038 Clear Event Log + SimId"));
	ClearLogSimIdOomTestL(ETrue);
	theLog.Write(_L8("Test 2.14 OK\n"));

	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4039 Clear Event Log/Recent + SimId"));
	ClearLogRecentSimIdOomTestL(ETrue);
	theLog.Write(_L8("Test 2.15 OK\n"));
#endif	
	}

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logheap"));

	TheTest.Start(_L("Log Client"));
	doClientFailureL();

	TheTest.Next(_L("Log Server"));
	doLogServFailureL();
	}
