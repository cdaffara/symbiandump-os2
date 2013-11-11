// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "LOGREPDEFS.H"
#include "t_logutil2.h"
#include "t_logutil3.h"
#include "t_logcntmatchplugin.h"

RTest TheTest(_L("t_logcntmatch"));

#ifdef SYSLIBS_TEST

TBool TheMatchingIsEnabled = EFalse;
//TheContactNameFmt variable must be initialized before tests. 
//It gives an information what is the contact name format in the logs.
TLogContactNameFormat TheContactNameFmt = KLogContactNameFormat;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//This function checks the logged name is the same as the event name.
//Contact name logging format is taken into account.
static void CheckContactName(CLogEvent& aEvent, const TDesC& aGivenName, const TDesC& aFamilyName)
	{
	TBuf<128> fullname;
	if(TheContactNameFmt == ELogWesternFormat)
		{
		fullname.Format(_L("%S %S"), &aGivenName, &aFamilyName);
		}
	else //ELogChineseFormat
		{
		fullname.Format(_L("%S %S"), &aFamilyName, &aGivenName);
		}
	TEST(aEvent.RemoteParty() == fullname);
	}

/**
Check normal operation

@SYMTestCaseID          SYSLIB-LOGENG-CT-1016
@SYMTestCaseDesc	    Tests for normal operations of CContactItem class
@SYMTestPriority 	    High
@SYMTestActions  	    The test adds 3 events and checks that the retrieved contact details are correct.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void TestContactMatch1L(CLogClient& aClient)
	{
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);
	event->SetNumber(KNumber1);

	// Add event. Expected contact to be found: {KFirstName1, KLastName1}.
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check result
	TLogContactItemId id1 = event->Contact(); 
	TEST(id1 != KLogNullContactId);
	::CheckContactName(*event, KFirstName1, KLastName1);
	TEST(event->Flags() & KLogEventContactSearched);

	// Create new event
	CleanupStack::PopAndDestroy(event);
	event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);
	event->SetNumber(KNumber2);

	// Add event. Expected contact to be found: {KFirstName2, KLastName2}.
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check result
    TLogContactItemId id2 = event->Contact(); 
	TEST(id2 != KLogNullContactId && id2 != id1);
	::CheckContactName(*event, KFirstName2, KLastName2);
	TEST(event->Flags() & KLogEventContactSearched);

	// Create new event
	CleanupStack::PopAndDestroy(event);
	event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);
	event->SetNumber(KNumber3);

	// Add event. Expected contact to be found: {KFirstName3, KLastName3}.
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check result
    TLogContactItemId id3 = event->Contact(); 
    TEST(id3 != KLogNullContactId && id3 != id2);
	::CheckContactName(*event, KFirstName3, KLastName3);
	TEST(event->Flags() & KLogEventContactSearched);
	
	CleanupStack::PopAndDestroy(2); // event, active
	}

/**
Check special cases

@SYMTestCaseID          SYSLIB-LOGENG-CT-1017
@SYMTestCaseDesc	    Tests for special cases on CContactItem class
@SYMTestPriority 	    High
@SYMTestActions  	    The test adds couple events and checks that the retrieved contact details are correct.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
static void TestContactMatch2L(CLogClient& aClient)
	{
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	const TLogContactItemId KIdInvalid = 77711;
	event->SetNumber(KNumber3);
	event->SetContact(KIdInvalid);

	// Add event. There is a contacts record with phone number KNumber3, but the contact id is not KIdInvalid. 
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check result - details should be unchanged
    TLogContactItemId id1 = event->Contact(); 
	TEST(id1 == KIdInvalid);
	TEST(event->RemoteParty().Length() == 0);
	TEST(!(event->Flags() & KLogEventContactSearched));

	// Set remote party. The contact id is still KIdInvalid.
	// The remote part of the contacts record with phone number KNumber3 is KFirstName3.
	event->SetRemoteParty(KFirstName5);

	// Add event 
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check result - details should be unchanged
    id1 = event->Contact(); 
	TEST(id1 == KIdInvalid);
	TEST(event->RemoteParty() == KFirstName5);
	TEST(!(event->Flags() & KLogEventContactSearched));

	// Create new event. The contact id is not set.
    // The phone number is set to be the same as the phone number of the contacts record.
	// The remote party is set but is different in the contacts record.
	CleanupStack::PopAndDestroy(event);
	event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);
	event->SetNumber(KNumber3);
	event->SetRemoteParty(KFirstName5);
	
	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check result - Remote party should be unchanged. But the contact id should be set and valid.
    TLogContactItemId id2 = event->Contact(); 
	TEST(id2 != KLogNullContactId && id2 != id1);
	TEST(event->RemoteParty() == KFirstName5);
	TEST(event->Flags() & KLogEventContactSearched);

	// Create new event
	CleanupStack::PopAndDestroy(event);
	event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);
	event->SetNumber(KNumber4);//There are 2 contacts records with phone number = KNumber4.

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check result - Details should not be set, because more than one contact found
	TEST(event->Contact() == KLogNullContactId);
	TEST(event->RemoteParty().Length() == 0);
	TEST(event->Flags() & KLogEventContactSearched);

	// Create new event
	CleanupStack::PopAndDestroy(event);
	event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	TInt count = KNumber6().Length();
	while(count--)
		{
		TPtrC num = KNumber6().Right(KNumber6().Length() - count);
		event->SetNumber(num);

		// Add event
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

	    TLogContactItemId id3 = event->Contact();
		
		// Shouldn't perform a contacts match if number isn't big enough
		if (num.Length() < 7)
			{
			// Check result - contact should not be set
			TEST(id3 == KLogNullContactId);
			TEST(event->RemoteParty().Length() == 0);
			}
		else
			{
			// Check result - Details should be set now
            TEST(id3 != KLogNullContactId);
			::CheckContactName(*event, KFirstName6, KLastName6);
			}
		TEST(event->Flags() & KLogEventContactSearched);
		}
		
	CleanupStack::PopAndDestroy(2); // event, active
	}
	
/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1392
@SYMTestCaseDesc	    Test for DEF068087 fix - "Chinese names don't display in Chinese name format."
@SYMTestPriority 	    Medium
@SYMTestActions  	    Checks that added {given_name,family_name} strings pair
                        is stored in the logs using the correct order (degtermined 
                        by r_log_contact_name_format resource value).
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DEF068087L(CLogClient& aClient)
	{
	CTestActive* active = new (ELeave) CTestActive();
	CleanupStack::PushL(active);
	
	//Add "phone call" event using one of the existing contacts
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);
	event->SetNumber(KNumber7);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TInt eventId = event->Id();
	TEST(eventId != KLogNullId);

	//Check result
	TEST(event->Contact() != KLogNullContactId);
	::CheckContactName(*event, KFirstName7, KLastName7);
	TEST(event->Flags() & KLogEventContactSearched);
	
	//Cleanup
	CleanupStack::PopAndDestroy(event);
	CleanupStack::PopAndDestroy(active);
	}

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logcntmatch"));

	TheTest.Start(_L("Prepare the test environment"));

	TheMatchingIsEnabled = TestUtils::MatchingEnabledL();

	if (!TheMatchingIsEnabled)
		{
		TheTest.Printf(_L("Contacts matching not enabled. Contacts matching tests NOT run\n"));
		return;
		}

	TInt contactMatchCount = 0;
	LogGetContactmatchCountAndNameFormatL(contactMatchCount, TheContactNameFmt);
	TheTest.Printf(_L("Contact match count = %d, TheContactNameFmt = %d\r\n"), contactMatchCount, (TInt)TheContactNameFmt);

	TestUtils::DeleteDatabaseL();

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	//All tests bellow are likely to fail if:
	// 101f401d.txt file exists in CentralRepository private data cage and the contact mach count is set to 0 in that file.
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1392: DEF068087: Chinese names don't display in Chinese name format"));
	::DEF068087L(*client);
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1016: Contacts matching - test1"));
	TestContactMatch1L(*client);
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1017: Contacts matching - test2"));
	TestContactMatch2L(*client);

	TestUtils::DeleteDatabaseL();

	CleanupStack::PopAndDestroy(client);
	}

#else //SYSLIBS_TEST

void doTestsL()
	{
	TheTest.Start(_L("This test can be run only when built with SYSLIBS_TEST macro defined!"));
	}

#endif //SYSLIBS_TEST
