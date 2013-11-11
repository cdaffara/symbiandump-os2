// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// test code for INC041118 - Numberfield in logdatabase/engine is too small
// 
//

#include <s32file.h>
#include <e32math.h>
#include <bautils.h>
#include "t_logutil2.h"

RTest TheTest(_L("t_logmaxnumlen"));

_LIT(KTestEventDesc1, "Event Type Description");
_LIT(KTestRemoteParty1, "Remote Party");
_LIT(KTestDirection1, "Direction");
_LIT(KTestStatus1, "Status");
_LIT(KTestSubject1, "Subject");


// These values are stored in "oldLogdb.dat"
_LIT(KTestEvent0, "00000");
_LIT(KTestEvent1, "11111");
_LIT(KTestEvent2, "22222");

const TUid KTestEventUid = {0x10005393};
const TLogDurationType KTestDurationType1 = 1;
const TLogDuration KTestDuration1 = 0x1234;
const TLogContactItemId KTestContact1 = 0x1234;
const TLogLink KTestLink1 = 0x1234;
_LIT8(KTestData1, "ABCDEFGH");
const TLogFlags KTestFlags1 = 0x5;


// with this value the test will ONLY pass with the new database
_LIT(KTestNumberMax, 
	"0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");

// This test needs a number > 32 to check that database has been converted
_LIT(KTestNumber50, "01234567890123456789012345678901234567890123456789");
_LIT(KTestNumberMin, "0");

// this coresponds to an event already stored in oldLogDb.dat 
const TInt KoldId = 3;
_LIT(KTestOldDbNumber, "012345678901234567");


/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1336
@SYMTestCaseDesc	    Tests for adding events to the log engine database
@SYMTestPriority 	    High
@SYMTestActions  	    Check for event ID assigned by log engine
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
TInt TestAddEventL(CLogClient& aClient, const TDesC &aNumber)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1336 "));
	TInt returnId = KLogNullId;

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	// create a new event
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Reset
	TTime now;
	now.UniversalTime();

	// load the event with test values
	event->SetEventType(KTestEventUid);
	event->SetRemoteParty(KTestRemoteParty1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KTestDurationType1);
	event->SetDuration(KTestDuration1);
	event->SetStatus(KTestStatus1);
	event->SetSubject(KTestSubject1);
	event->SetNumber(aNumber);
	event->SetContact(KTestContact1);
	event->SetLink(KTestLink1);
	event->SetDataL(KTestData1);
	event->SetFlags(KTestFlags1);

	// add the event to the logeng database
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);

	CActiveScheduler::Start();
	TEST(!active->IsActive());
	aClient.Cancel();
	TEST2(active->iStatus.Int(), KErrNone);

	// check that an ID has been assigned
	returnId = event->Id();
	TEST(returnId != KLogNullId);
	
	TEST(event->Time() >= now);
	TEST(event->Description() == KTestEventDesc1);
	
	CleanupStack::PopAndDestroy(2); // event, active
	
	// return the event id which has been assigned by the 
	// log engine
	return returnId;
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1337
@SYMTestCaseDesc	    Tests for getting the event from the log engine database
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for CLogClient::GetEvent() function
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TestGetEventL(CLogClient& aClient, TInt aTheId, const TDesC& aNumber)

	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1337 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// set the id of the event to be fetched
	event->SetId(aTheId);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	aClient.Cancel();
	TEST2(active->iStatus.Int(), KErrNone);
		
	TEST(event->Id() == aTheId);
	TEST(event->Time() > TTime(0));
	TEST(event->Description() == KTestEventDesc1);
	TEST(event->EventType() == KTestEventUid);
	TEST(event->RemoteParty() == KTestRemoteParty1);
	TEST(event->Direction() == KTestDirection1);
	TEST(event->DurationType() == KTestDurationType1);
	TEST(event->Duration() == KTestDuration1);
	TEST(event->Status() == KTestStatus1);
	TEST(event->Subject() == KTestSubject1);
	TEST(event->Contact() == KTestContact1);
	TEST(event->Link() == KTestLink1);
	TEST(event->Data() == KTestData1);
	TEST(event->Flags() == KTestFlags1);

	// this is the important test
	// check the number has not been truncated
	TEST(event->Number() == aNumber);

	TPtrC eventStatus = event->Subject();
	TPtrC eventNumber = event->Number();
	TheTest.Printf(_L("Id:%d No:%S Sub:%S Con:0x%x \n"), 
		event->Id(), &eventNumber, &eventStatus, event->Contact());
	
	CleanupStack::PopAndDestroy(2); // event, active
	}


/**
Test code for INC041118 - Numberfield in logdatabase/engine is too small
This is the main part of the test

@SYMTestCaseID          SYSLIB-LOGENG-CT-1019
@SYMTestCaseDesc	    Tests for number field in logdatabase/engine is too small
@SYMTestPriority 	    High
@SYMTestActions  	    Add an event to the log engine database.When the database is opened it should be
                        converted to new format.Check by reading the event and checking the number length
                        Check with maximum and minimum number field
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void TestStartupL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1019 "));
	TestUtils::CopyOldDbL();
	
	// add an event to the log engine database.
	// When the database is opened it should be checked for the old numberfield length
	// and converted to the new format if required. (It will be required, as old db has been copied)
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	User::After(1000000);
	TheTest.Next(_L("check database conversion"));
	TInt eventId = TestAddEventL(*client, KTestNumber50 );

	// check that the database has been converted to the new format
	// by reading back an event the checking that number is the
	// correct length
	TestGetEventL(*client, eventId, KTestNumber50);
	
	// check using Max
	TheTest.Next(_L("check a maxium length number"));
	eventId = TestAddEventL(*client, KTestNumberMax );
	TestGetEventL(*client, eventId, KTestNumberMax);

	// check using Min
	TheTest.Next(_L("check a mimium length number"));
	eventId = TestAddEventL(*client, KTestNumberMin );
	TestGetEventL(*client, eventId, KTestNumberMin);

	// When OldLogdbu.dat was created, using TestBuildTestDbL() below,
	// 4 events were stored.  Check that these can still be accessed.
	// Note: There was a problem here with old events being purged after 30 days
	// This was fixed by setting TLogConfig::iMaxEventAge = 0 in OldLogdb.dat 
	TheTest.Next(_L("check all events in the old database"));
	TestGetEventL(*client, 0, KTestEvent0);
	TestGetEventL(*client, 1, KTestEvent1);
	TestGetEventL(*client, 2, KTestEvent2);
	TestGetEventL(*client, KoldId, KTestOldDbNumber);

	CleanupStack::PopAndDestroy(); // client
	}

// Test code for INC041118 - Numberfield in logdatabase/engine is too small
void doTestsL()
	{
	TestUtils::Initialize(_L("t_logmaxnumlen"));
#ifdef _DEBUG	
	TheTest.Start(_L("T_MaxNumberLength Set/Check Phone Number Maximum Length"));

	TestStartupL();
	theLog.Write(_L8("Test T_MAXNUMBERLENGTH OK\n"));
#else
	TheTest.Start(_L("This test works only in debug mode, otherwise the LogEng server cannot be stopped. See TestUtils::CopyOldDbL()"));
#endif//_DEBUG	
	}
