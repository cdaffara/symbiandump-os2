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
#include "t_logutil2.h"

/*
This test may fail occasionally, especially after changing the LogEng server code. The reported error is KErrCorrupt.
The typical scenario when the test fails is:
1) Some of the test functions calls "__FILE_FAILNEXT(failCount++)"
2) The test calls some CLogClient method, typically "aClient.GetEventType()".
3) The statement "test(active->iStatus == KErrNone)" fails.
This behaviour might be related to INC044553 "DBMS does not handle OOD error 
scenarios correctly when committing a transaction".
"__FILE_FAILNEXT" is a macro, which calls theFs.SetErrorCondition(KErrGeneral, X).
If you perform a search with "SetErrorCondition" keyword in "Programming" database, you will find
some explanations that the programmer has to be careful when using SetErrorCondition() method,
because:

"You have to be careful if you use this in the same way as you do for heap testing, because you 
can get the file system in a state where it think the file you're working on is corrupted."

Anyway, LOGENG server internally uses DBMS component. Calling SetErrorCondition() we test DBMS 
component actually not LOGENG component!
There is one addidtional thing in the test, which makes me believing that the problem is not 
in LOGENG but in SetErrorCondition()/DBMS interaction - in many if the test functions a statement
like:

  		if (failCount == 5)
			failCount++;

may be found. I think that it is there to prevent exactly the problem with 
SetErrorCondition()/DBMS interaction.
*/

RTest TheTest(_L("t_logfile"));

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

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1334
@SYMTestCaseDesc	    Basic test to add events
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for CLogClient::AddEvent() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestBasicL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1334 "));
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
	TEST2(active->iStatus.Int(),KErrNone);

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
	TEST2(active->iStatus.Int(),KErrNone);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(2); // event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0911
@SYMTestCaseDesc	    Tests for new CLogClient object creation
@SYMTestPriority 	    High
@SYMTestActions  	    Create a new CLogClient check for memory errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestConstructionL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0911 "));
	CLogClient* client = NULL;

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		TheTest.Printf(_L("%d\r\n"), failCount);

        if(failCount > 8)
            {
            TRAP(error, client = CLogClient::NewL(theFs));
            TEST2(error, KErrNone);
            TestBasicL(*client);
            finished = ETrue;
            continue;
            }
		
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TRAP(error, client = CLogClient::NewL(theFs));

		__FILE_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;
			TestBasicL(*client);
			}
		else
			{
			TEST2(error, KErrNoMemory);
			TestUtils::DeleteDatabaseL();
			}
		}

	delete client;
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0912
@SYMTestCaseDesc	    Adding an event type test,
                        Tests for CLogClient::AddEventType() function
@SYMTestPriority 	    High
@SYMTestActions  	    Check for file failure errors while adding event types
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestAddEventTypeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0912 "));
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
        if(failCount == 8)
            {
            aClient.AddEventType(*type, active->iStatus);
            active->StartL();
            CActiveScheduler::Start();
            TEST2(active->iStatus.Int(), KErrNone);
            finished = ETrue;
            continue;
            }

		TheTest.Printf(_L("%d  \r"), failCount);
        
		__FILE_FAILNEXT(KErrNoMemory, failCount++);
		
		aClient.AddEventType(*type, active->iStatus);
	
		active->StartL();
		CActiveScheduler::Start();
		
		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__FILE_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			aClient.GetEventType(*type, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNotFound);
			}
		else
			TEST2(error, KErrNone);

		}

	active->StartL();
	aClient.AddEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrAlreadyExists);

	CleanupStack::PopAndDestroy(2); // active, type
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0913
@SYMTestCaseDesc	    Getting an event type test.
                        Tests for CLogClient::GetEventType() function
@SYMTestPriority 	    High
@SYMTestActions  	    Check for file failure errors while getting event types
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetEventTypeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0913 "));
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		aClient.GetEventType(*type, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();
		
		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			{
			TEST2(active->iStatus.Int(), KErrNoMemory);
			TEST(type->Description() == KNullDesC);
			}

		__FILE_RESET;
		}

	TEST(type->Uid() == KTestEventUid);
	TEST(type->Description() == KTestEventDesc1);
	TEST(type->LoggingEnabled());

	CleanupStack::PopAndDestroy(2); // active, type
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0914
@SYMTestCaseDesc	    Changing an event type test,
                        Tests for CLogClient::ChangeEventType() function
@SYMTestPriority 	    High
@SYMTestActions  	    Check for file failure errors while changing event types
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEventTypeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0914 "));
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

		type->SetDescription(KTestEventDesc2);
		type->SetLoggingEnabled(EFalse);

        if(failCount == 8)
            {
            aClient.ChangeEventType(*type, active->iStatus);
            active->StartL();
            CActiveScheduler::Start();
            TEST2(active->iStatus.Int(), KErrNone);
            finished = ETrue;
            continue;
            }
		
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		aClient.ChangeEventType(*type, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__FILE_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			aClient.GetEventType(*type, active->iStatus);
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
	aClient.GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(type->Uid() == KTestEventUid);
	TEST(type->Description() == KTestEventDesc2);
	TEST(type->LoggingEnabled() == EFalse);

	CleanupStack::PopAndDestroy(2); // type, active
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0915
@SYMTestCaseDesc	    Deleting an event type test
                        Tests for CLogClient::DeleteEventType() function
@SYMTestPriority 	    High
@SYMTestActions  	    Check for file failure errors while deleting event types
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDeleteEventTypeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0915 "));
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

        if(failCount == 8)
            {
            aClient.DeleteEventType(KTestEventUid, active->iStatus);
            active->StartL();
            CActiveScheduler::Start();
            TEST2(active->iStatus.Int(), KErrNone);
            finished = ETrue;
            continue;
            }
		
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		aClient.DeleteEventType(KTestEventUid, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__FILE_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			aClient.GetEventType(*type, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}
		else
			TEST2(error, KErrNone);
		}

	active->StartL();
	aClient.GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(2); // type, active
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0916
@SYMTestCaseDesc	    Adding an event test,
                        Tests for CLogClient::AddEvent() function
@SYMTestPriority 	    High
@SYMTestActions  	    Check for file failure errors while adding event
                        Tests for no error conditons,and the event information 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestAddEventL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0916 "));
	// Ensure we always start from the same state
    TestUtils::DeleteDatabaseL();

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc1);
	type->SetLoggingEnabled(ETrue);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	active->StartL();
	aClient.AddEventType(*type, active->iStatus);
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

        if(failCount == 8)
            {
            aClient.AddEvent(*event, active->iStatus);
            active->StartL();
            CActiveScheduler::Start();
            TEST2(active->iStatus.Int(), KErrNone);
            finished = ETrue;
            continue;
            }

		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);
		aClient.AddEvent(*event, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__FILE_RESET;

		if (error == KErrNoMemory)
			{
			event->SetId(0);

			active->StartL();
			aClient.GetEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNotFound);

			event->SetId(KLogNullId);
			}
		else
			TEST2(error, KErrNone);
		}

	TEST(event->Id() == 0);
	TEST(event->Time() >= now);
	TEST(event->Description() == KTestEventDesc1);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(3); // event, active, type
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0917
@SYMTestCaseDesc	    Get an events' information test
                        Tests for CLogClient::GetEvent() function
@SYMTestPriority 	    High
@SYMTestActions  	    Check for file failure errors while getting an event
                        Tests for no error conditons,and the event information. 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetEventL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0917 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(0);

	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);
		aClient.GetEvent(*event, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);

		__FILE_RESET;
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

	CleanupStack::PopAndDestroy(2); // event, active
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0918
@SYMTestCaseDesc	    Changing an event type test
                        Tests for CLogClient::ChangeEvent() function
@SYMTestPriority 	    High
@SYMTestActions  	    Check for file failure errors while changing an event
                        Tests for no error conditons,and the event information for the changed event
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEventL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0918 "));
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

        if(failCount == 14)
            {
            aClient.ChangeEvent(*event, active->iStatus);
            active->StartL();
            CActiveScheduler::Start();
            TEST2(active->iStatus.Int(), KErrNone);
            finished = ETrue;
            continue;
            }
		
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		aClient.ChangeEvent(*event, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();
		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__FILE_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			aClient.GetEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			if (active->iStatus != KErrNone) 
			    TheTest.Printf(_L("\nerror code:%d failcount:%d\n"),active->iStatus.Int(),failCount);
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
	aClient.GetEvent(*event, active->iStatus);
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

	CleanupStack::PopAndDestroy(2); // event, active
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0919
@SYMTestCaseDesc	    Deleting an event test,
                        Tests for CLogClient::DeleteEvent() function
@SYMTestPriority 	    High
@SYMTestActions  	    Check for file failure errors while deleting an event
                        Tests for ErrNone flag.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDeleteEventL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0919 "));
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

        if(failCount == 10)
            {
            aClient.DeleteEvent(0, active->iStatus);
            active->StartL();
            CActiveScheduler::Start();
            TEST2(active->iStatus.Int(), KErrNone);
            finished = ETrue;
            continue;
            }
		
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		aClient.DeleteEvent(0, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__FILE_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			aClient.GetEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}
		else
			TEST2(error, KErrNone);
		}

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(2); // event, active
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0920
@SYMTestCaseDesc	    Getting the Log Engine configuration information test
                        Tests for CLogClient::GetConfig() function
@SYMTestPriority 	    High
@SYMTestActions  	    Check for file failure errors while getting the configuration data
                        Tests for ErrNone flag.Tests for the retrieved information
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetConfigL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0920 "));
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
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);
		aClient.GetConfig(config, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);

		__FILE_RESET;
		}

	TEST(config.iMaxEventAge > 0);
	TEST(config.iMaxLogSize > 0);
	TEST(config.iMaxRecentLogSize > 0);

	CleanupStack::PopAndDestroy(); // active
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0921
@SYMTestCaseDesc	    Changing the configuration of Log Engine test,
                        Tests for CLogClient::ChangeConfig() function
@SYMTestPriority 	    High
@SYMTestActions  	    Check for file failure errors while changing the log engine configuration data
                        Tests for ErrNone error conditons.Tests for the changed configuration data
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeConfigL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0921 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig configOld;

	active->StartL();
	aClient.GetConfig(configOld, active->iStatus);
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

        if(failCount == 11)
            {
            aClient.ChangeConfig(config, active->iStatus);
            active->StartL();
            CActiveScheduler::Start();
            TEST2(active->iStatus.Int(), KErrNone);
            finished = ETrue;
            continue;
            }
		
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		aClient.ChangeConfig(config, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();
		
		__FILE_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			aClient.GetConfig(config, active->iStatus);
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

	CleanupStack::PopAndDestroy(); // active
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0922
@SYMTestCaseDesc	    Getting a standard string from the specified resource file test
                        Tests for CLogClient::GetString() function
@SYMTestPriority 	    High
@SYMTestActions  	    Check for file failure errors while changing the log engine configuration data
                        Check for ErrNone flag.Tests for general errors,and retrieved string length. 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetStringL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0922 "));
	TBuf<KLogMaxSharedStringLength> str;
	
	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		error = aClient.GetString(str, R_LOG_DIR_IN);

		__FILE_RESET;

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
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0923
@SYMTestCaseDesc	    Clearing the Log Event test.
                        Tests for CLogClient::ClearLog() function
@SYMTestPriority 	    High
@SYMTestActions  	    Add 4 events to the event log,check for ErrNone flag
                        Clear the log by calling CLogClient::ClearLog() function.
						Try for getting the added 4 events.Check for not found error.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ12748
*/
LOCAL_C void TestClearEventLogL(CLogClient& aClient
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		, TBool aUseSimId = EFalse
#endif		
		)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0923 "));
	TTime now;
	now.UniversalTime();
	
	TDateTime dt(now.DateTime());
	dt.SetYear(dt.Year() - 1);
	TTime date1(dt);

	dt.SetYear(dt.Year() - 1);
	TTime date2(dt);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event1 = CLogEvent::NewL();
	CleanupStack::PushL(event1);
	event1->SetEventType(KTestEventUid);

	active->StartL();
	aClient.AddEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event1->SetTime(date1);

	active->StartL();
	aClient.ChangeEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);
	event2->SetEventType(KTestEventUid);

	active->StartL();
	aClient.AddEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event2->SetTime(date1);

	active->StartL();
	aClient.ChangeEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event3 = CLogEvent::NewL();
	CleanupStack::PushL(event3);
	event3->SetEventType(KTestEventUid);

	active->StartL();
	aClient.AddEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event3->SetTime(date2);

	active->StartL();
	aClient.ChangeEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event4 = CLogEvent::NewL();
	CleanupStack::PushL(event4);
	event4->SetEventType(KTestEventUid);

	active->StartL();
	aClient.AddEvent(*event4, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event4->SetTime(date2);

	active->StartL();
	aClient.ChangeEvent(*event4, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		if(aUseSimId)
			{
			aClient.ClearLog(date1, KLogNullSimId, active->iStatus);
			}
		else
#endif			
			{
			aClient.ClearLog(date1, active->iStatus);
			}

		__FILE_RESET;

		active->StartL();	
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);
		}

	active->StartL();
	aClient.GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	if (active->iStatus != KErrNone) 
	    TheTest.Printf(_L("error code:%d\n"),active->iStatus.Int());
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	aClient.GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	aClient.GetEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	active->StartL();
	aClient.GetEvent(*event4, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	aClient.GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	active->StartL();
	aClient.GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(5); // event4, event3, event2, event1, active
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logfile"));

    TheTest.Start(_L("Construction + create db"));
	TestUtils::DeleteDatabaseL();
	TestConstructionL(); // Creates database
	TheTest.Next(_L("Construction + open db"));
	TestConstructionL(); // Opens existing database
	TestUtils::DeleteDatabaseL();
	theLog.Write(_L8("Test 1 OK\n"));

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	TheTest.Next(_L("Add Event Type"));
	TestAddEventTypeL(*client);
	theLog.Write(_L8("Test 2 OK\n"));

	TheTest.Next(_L("Get Event Type"));
	TestGetEventTypeL(*client);
	theLog.Write(_L8("Test 3 OK\n"));

	TheTest.Next(_L("Change Event Type"));
	TestChangeEventTypeL(*client);
	theLog.Write(_L8("Test 4 OK\n"));

	TheTest.Next(_L("Delete Event Type"));
	TestDeleteEventTypeL(*client);
	theLog.Write(_L8("Test 5 OK\n"));

	TestUtils::DeleteDatabaseL();

	TheTest.Next(_L("Add Event"));
	TestAddEventL(*client);
	theLog.Write(_L8("Test 6 OK\n"));

	TheTest.Next(_L("Get Event"));
	TestGetEventL(*client);
	theLog.Write(_L8("Test 7 OK\n"));

	TheTest.Next(_L("Change Event"));
	TestChangeEventL(*client);
	theLog.Write(_L8("Test 8 OK\n"));

	TheTest.Next(_L("Delete Event"));
	TestDeleteEventL(*client);
	theLog.Write(_L8("Test 9 OK\n"));

	TheTest.Next(_L("Get Config"));
	TestGetConfigL(*client);
	theLog.Write(_L8("Test 10 OK\n"));

	TheTest.Next(_L("Change Config"));
	TestChangeConfigL(*client);
	theLog.Write(_L8("Test 11 OK\n"));

	TheTest.Next(_L("Get String"));
	TestGetStringL(*client);
	theLog.Write(_L8("Test 12 OK\n"));

	TheTest.Next(_L("Clear Event Log"));
	TestClearEventLogL(*client);
	theLog.Write(_L8("Test 13 OK\n"));

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	TheTest.Next(_L("Clear Event Log + SimId"));
	TestClearEventLogL(*client, ETrue);
	theLog.Write(_L8("Test 14 OK\n"));
#endif	

	CleanupStack::PopAndDestroy(); // client
	}
