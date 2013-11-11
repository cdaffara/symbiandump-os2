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
#include <e32math.h>
#include <logview.h>
#include <s32mem.h> 
#include "t_logutil2.h"
#include "t_logutil3.h"

#define UNUSED_VAR(a) a = a

const TLogContactItemId KTestContact = 0x1234;
const TInt KTestEventNum = 10;

RTest TheTest(_L("t_logapi"));

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
_LIT(KTestNumber1, "TheNumber");
_LIT(KTestNumber2, "Changed Number");
const TLogContactItemId KTestContact1 = 0x1234;
const TLogContactItemId KTestContact2 = 0x1234567;
const TLogLink KTestLink1 = 0x1234;
const TLogLink KTestLink2 = 0x1234567;
_LIT8(KTestData1, "ABCDEFGH");
_LIT8(KTestData2, "IJKLMNOPQRSTUVWXYZ");
const TLogFlags KTestFlags1 = 0x5;
const TLogFlags KTestFlags2 = 0xA;
const TLogSize KTestMaxLogSize = 0xFFF;
const TLogRecentSize KTestMaxRecentLogSize = 0xF;
const TLogAge KTestMaxEventAge = 0xFFFFFFF;

TInt gTheId;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TClientObserverTestReceiver : public MLogClientChangeObserver
	{
public:
	TClientObserverTestReceiver(TBool& aFlag) : iCallCount(0), iFlag(aFlag) { }

public:
	void HandleLogClientChangeEventL(TUid aChangeType, TInt /*aChangeParam1*/, TInt /*aChangeParam2*/, TInt /*aChangeParam3*/)
		{
		++iCallCount;
		iFlag = (aChangeType == KLogClientChangeEventLogCleared);
		}
	inline TInt CallCount() const { return iCallCount; }
	inline void Reset() { iCallCount = 0; iFlag = EFalse; }

private:
	TInt iCallCount;
	TBool& iFlag;
	};

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0833
@SYMTestCaseDesc	    Client observer mechanism test
@SYMTestPriority 	    High
@SYMTestActions  	    Set the change observer,add an event type,clear the log and test for the status.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestClientObserverMechanismL(CLogClient& aClient)
	{	
	//
	TBool logClearedFlag = EFalse;
	TClientObserverTestReceiver testReceiver(logClearedFlag);
	//
	aClient.SetGlobalChangeObserverL(&testReceiver);

	TTime now;
	now.UniversalTime();
	TTime clearLogThreshold(now);
	clearLogThreshold -= TTimeIntervalDays(1);
	TTime past(now);
	past -= TTimeIntervalDays(2);

	// Make some events
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	for(TInt i=0; i<10; i++)
		{
		event->SetEventType(KLogCallEventTypeUid);
		event->SetRemoteParty(KTestRemoteParty1);
		event->SetDirection(KTestDirection1);
		event->SetDurationType(KTestDurationType1);
		event->SetDuration(KTestDuration1);
		event->SetStatus(KTestStatus1);
		event->SetSubject(KTestSubject1);
		event->SetNumber(KTestNumber1);
		event->SetContact((TLogContactItemId) i+1);
		event->SetLink(KTestLink1);
		event->SetDataL(KTestData1);
		event->SetFlags(KTestFlags1);

		// Add event
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Make it in the past
		event->SetTime(past);
		active->StartL();
		aClient.ChangeEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TEST2(testReceiver.CallCount(), 0);

	// Clear the log
	active->StartL();
	aClient.ClearLog(clearLogThreshold, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Have to wait a while, since the change observer active object
	// won't run until shortly after completing the 'clear log' operation
	// request status.
	CTestTimer* timer = CTestTimer::NewL();
	CleanupStack::PushL(timer);
	timer->After(5 * 1000000);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(timer);
	//
	TEST2(testReceiver.CallCount(), 1);
	TEST(logClearedFlag);

	// Start again
	testReceiver.Reset();

	// Test removing observer
	aClient.SetGlobalChangeObserverL(NULL);

	// Add an event again
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(testReceiver.CallCount(), 0);

	// Make it in the past
	event->SetTime(past);
	active->StartL(); 
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Clear the log again
	active->StartL();
	aClient.ClearLog(clearLogThreshold, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(testReceiver.CallCount(), 0);
	TEST(logClearedFlag == EFalse);

	aClient.SetGlobalChangeObserverL(&testReceiver);

	// Tidy up
	CleanupStack::PopAndDestroy(2, event); // active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0834
@SYMTestCaseDesc	    Adding an event to the log engine test.
@SYMTestPriority 	    High
@SYMTestActions  	    Create a new event type and add that to the log engine.
                        Start an active scheduler and check for the  error status.
                        Add the event once again and check for the already exits condition.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestAddEventTypeL(CLogClient& aClient)
//
//
//
	{
	LOGTEXT("TestAddEventTypeL()");	
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc1);
	type->SetLoggingEnabled(ETrue);

	CTestActive* active = new(ELeave) CTestActive();
	CleanupStack::PushL(active);

	active->StartL();
	aClient.AddEventType(*type, active->iStatus);
	aClient.Cancel();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);

	active->StartL();
	aClient.AddEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	aClient.Cancel();
	TEST((active->iStatus == KErrNone)||(active->iStatus == KErrAlreadyExists));

	active->StartL();
	aClient.AddEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrAlreadyExists);

	CleanupStack::PopAndDestroy(2); // active, type
	LOGTEXT("TestAddEventTypeL() - end");
	}
/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0835
@SYMTestCaseDesc	    Retrieving the event type information test.
                        Tests for CLogClient::GetEventType() function.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the retrieved information of event type. 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetEventTypeL(CLogClient& aClient)
//
//
//
	{
	LOGTEXT("TestGetEventTypeL()");	
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	active->StartL();
	aClient.GetEventType(*type, active->iStatus);
	aClient.Cancel();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);

	active->StartL();
	aClient.GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	aClient.Cancel();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(type->Uid() == KTestEventUid);
	TEST(type->Description() == KTestEventDesc1);
	TEST(type->LoggingEnabled());

	CleanupStack::PopAndDestroy(2); // active, type
	LOGTEXT("TestGetEventTypeL() - end");
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0836
@SYMTestCaseDesc	    Changing the event type test.
                        Tests for CLogClient::ChangeEventType test
@SYMTestPriority 	    High
@SYMTestActions  	    Create a new event type,change the event type and check for any errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEventTypeL(CLogClient& aClient)
//
//
//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0836 "));
	LOGTEXT("TestChangeEventTypeL()");	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc2);
	type->SetLoggingEnabled(EFalse);

	active->StartL();
	aClient.ChangeEventType(*type, active->iStatus);
	aClient.Cancel();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);

	active->StartL();
	aClient.ChangeEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	aClient.Cancel();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(); // type

	type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	active->StartL();
	aClient.GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(type->Uid() == KTestEventUid);
	TEST(type->Description() == KTestEventDesc2);
	TEST(type->LoggingEnabled() == EFalse);

	CleanupStack::PopAndDestroy(2); // type, active
	LOGTEXT("TestChangeEventTypeL() - end");
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0837
@SYMTestCaseDesc	    Deleting an event type test.
                        Tests for CLogClient::DeleteEventType() test
@SYMTestPriority 	    High
@SYMTestActions  	    Delete an event type from log database and test for no errors found
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDeleteEventTypeL(CLogClient& aClient)
//
//
//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0837 "));
	LOGTEXT("TestChangeEventTypeL()");	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	active->StartL();
	aClient.DeleteEventType(KTestEventUid, active->iStatus);
	aClient.Cancel();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);

	active->StartL();
	aClient.DeleteEventType(KTestEventUid, active->iStatus);
	CActiveScheduler::Start();
	aClient.Cancel();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	active->StartL();
	aClient.GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(2); // type, active
	LOGTEXT("TestChangeEventTypeL() - end");
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1329
@SYMTestCaseDesc	    Adding an event type test.
                        Tests for CLogClient::AddEventType() function
@SYMTestPriority 	    High
@SYMTestActions  	    Add an event type to the log database and test for no errors found
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestAddEventL(CLogClient& aClient)
//
//
//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1329 "));	
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

	// Reset
	gTheId = KLogNullId;
	//
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
	event->SetFlags(KTestFlags1);
	//
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	aClient.Cancel();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);
	//
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST(!active->IsActive());
	aClient.Cancel();
	TEST2(active->iStatus.Int(), KErrNone);
	//
	gTheId=event->Id();
	//
	TEST(gTheId != KLogNullId);
	TEST(event->Time() >= now);
	TEST(event->Description() == KTestEventDesc1);
	//
	CleanupStack::PopAndDestroy(3); // event, active, type
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0838
@SYMTestCaseDesc	    Client failure test
@SYMTestPriority 	    High
@SYMTestActions  	    Checks for status after client is deleted,checks for Cancel error flag
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestClientFailL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0838 "));	
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	event->SetId(gTheId);

	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CleanupStack::Pop(); // client
	delete client;	
	
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);

	CleanupStack::PopAndDestroy(2); // event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0839
@SYMTestCaseDesc	    Tests for CLogClient::GetEvent() function
@SYMTestPriority 	    High
@SYMTestActions  	    Retrieve the event type and test for the integrity of the event type information
                        Check for no errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/

LOCAL_C void TestGetEventL(CLogClient& aClient)
//
//
//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0839 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(gTheId);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	aClient.Cancel();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	aClient.Cancel();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(event->Id() == gTheId);
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
	TEST(event->Flags() == KTestFlags1);

	TBuf<500> subject;
	TInt i=450;
	while(i--)
		subject.Append(_L("a"));
	event->SetSubject(subject);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	if	(aClient.IsActive() && active->iStatus == KRequestPending)
		{
		aClient.Cancel();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrCancel);
		}
	else
		{
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// See TestAddEventL on why this might still allow the active object
	// to be cancelled.
//	TEST(active->iStatus == KErrNone || active->iStatus == KErrCancel);

	i=450;
	while(i--)
		subject[i]='b';
	event->SetSubject(subject);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	aClient.Cancel();
 	TEST2(active->iStatus.Int(), KErrNone);


	CleanupStack::PopAndDestroy(2); // event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0840
@SYMTestCaseDesc	    Tests for CLogClient::ChangeEvent() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Change the event,get the event information and test for the integrity.
                        Check for any errors.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEventL(CLogClient& aClient)
//
//
//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0840 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	TTime now;
	now.UniversalTime();

	event->SetId(gTheId);
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
	event->SetFlags(KTestFlags2);

	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	aClient.Cancel();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);

	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	aClient.Cancel();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(); // event

	event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(gTheId);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(event->Id() == gTheId);
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
	TEST(event->Flags() == KTestFlags2);

	CleanupStack::PopAndDestroy(2); // event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0841
@SYMTestCaseDesc	    Tests for CLogClient::DeleteEvent() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Delete the event and test for no errors found.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDeleteEventL(CLogClient& aClient)
//
//
//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0841 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	active->StartL();
	aClient.DeleteEvent(0, active->iStatus);
	aClient.Cancel();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);

	active->StartL();
	aClient.DeleteEvent(0, active->iStatus);
	CActiveScheduler::Start();
	aClient.Cancel();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(0);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(2); // event, active
	}


#ifdef SYSLIBS_TEST

/**
@SYMTestCaseID          SYSLIB-LOGENG-UT-4015
@SYMTestCaseDesc	    Test the behaviour implemented by PREQ2103
@SYMTestPriority 	    Medium
@SYMTestActions  	    Get the settings from logeng repository file / resource file.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ11125
                        REQ11126
                        REQ11127
                        REQ11128
*/
LOCAL_C void TestGetConfigSettingsFromRepositoryFileL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-UT-4015 "));

	TInt contactMatchCount1;
	TLogContactNameFormat contactNameFormat1;
	LogGetContactmatchCountAndNameFormatL(contactMatchCount1, contactNameFormat1);
	TheTest.Printf(_L("Contact match count = %d, contact name format = %d\r\n"), contactMatchCount1, (TInt)contactNameFormat1);
	//contactMatchCount1 and contactNameFormat1 are loaded directly from the repository, if exists.
	//Otherwise they are initialzied with their default values.
	//The LogEng server should load these resource values in a similar way.
	//They will be stored in contactMatchCount2 and contactNameFormat2.  
	TInt16 contactMatchCount2;
	TInt16 contactNameFormat2;
	RFs fs;
	
	LEAVE_IF_ERROR(fs.Connect());
	CleanupClosePushL(fs);
	
	//Creating a new CLogClient Object make the server getting the contact match settings. 
	//As the database is deleted it gets them from the LogEng repository.
	CLogClient* client2 = CLogClient::NewL(fs);
	CleanupStack::PushL(client2);
	CleanupStack::PopAndDestroy(client2);
		
	//Read these settings via "reader" object.
	RFileReadStream resFileCount_reader;
	CleanupClosePushL(resFileCount_reader);
	RFileReadStream resFileFormat_reader;
	CleanupClosePushL(resFileFormat_reader);
	_LIT(KLogengTestFileNameCount, "c:\\test\\test_logengconfig_count.ini");
	_LIT(KLogengTestFileNameFormat, "c:\\test\\test_logengconfig_format.ini");
	LEAVE_IF_ERROR(resFileCount_reader.Open(fs, KLogengTestFileNameCount, EFileRead));
	LEAVE_IF_ERROR(resFileFormat_reader.Open(fs, KLogengTestFileNameFormat, EFileRead));		
	contactMatchCount2 = resFileCount_reader.ReadInt32L();
	contactNameFormat2 = resFileFormat_reader.ReadInt32L();
		
	//The settings should match the ones from resource file.
	TEST2(contactMatchCount1, contactMatchCount2);
	TEST2(contactNameFormat1, contactNameFormat2);
		
	CleanupStack::PopAndDestroy(&resFileFormat_reader);
	CleanupStack::PopAndDestroy(&resFileCount_reader);
	
	//Get the config settings (Three config settings). 
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
		
	TLogConfig config;
	TEST(config.iMaxEventAge == 0);
	TEST(config.iMaxLogSize == 0);
	TEST(config.iMaxRecentLogSize == 0);
	active->StartL();
	aClient.GetConfig(config, active->iStatus);	
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	//The config settings should match the ones from the repository.
	TEST(config.iMaxEventAge == 2592000);
	TEST(config.iMaxLogSize == 1000);
	TEST(config.iMaxRecentLogSize == 20);
	
	
	//Now let's provide the repository file, so the server will find it and will get the settings
	//from it.
	
	//Create the directory c:\private\10202be9\ if it does not exist.
	_LIT(KExecutableFileName,		"t_logapi_helper.exe");
	_LIT(KCommandParameters,		"c:\\private\\10202be9\\;c:\\private\\10202be9\\101f401d.txt;3");
	RProcess process;
	TRequestStatus processWaitStatus;
	TInt r = process.Create(KExecutableFileName, KCommandParameters); 
	TEST(r == KErrNone);
	process.Logon(processWaitStatus);
	process.Resume();
	User::WaitForRequest(processWaitStatus);
	TEST(processWaitStatus.Int() == KErrNone);
	process.Close();
	
	 //copy the repository file to the folder 10202be9
	_LIT(KCommandParameters1,		"z:\\test\\101f401d_TEST.txt;c:\\private\\10202be9\\101f401d.txt;0");
	r = process.Create(KExecutableFileName, KCommandParameters1); 
	TEST(r == KErrNone);
	process.Logon(processWaitStatus);
	process.Resume();
	User::WaitForRequest(processWaitStatus);
	TEST(processWaitStatus.Int() == KErrNone);
	process.Close();
	
	TestUtils::DeleteDatabaseL();
	
	//Get the config settings.
	config.iMaxEventAge = 0;
	config.iMaxLogSize = 0;
	config.iMaxRecentLogSize = 0;	
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//They should match the ones from the repository file.
	TEST(config.iMaxEventAge == 2592001);
	TEST(config.iMaxLogSize == 1001);
	TEST(config.iMaxRecentLogSize == 21);
	
	CleanupStack::PopAndDestroy(active);
	
	//Get the contact match count and the contact name format.
	RFileReadStream repFileCount_reader;
	CleanupClosePushL(repFileCount_reader);
	RFileReadStream repFileFormat_reader;
	CleanupClosePushL(repFileFormat_reader);
	LEAVE_IF_ERROR(repFileCount_reader.Open(fs, KLogengTestFileNameCount, EFileRead));
	LEAVE_IF_ERROR(repFileFormat_reader.Open(fs, KLogengTestFileNameFormat, EFileRead));		
	contactMatchCount2 = repFileCount_reader.ReadInt32L();
	contactNameFormat2 = repFileFormat_reader.ReadInt32L();
	CleanupStack::PopAndDestroy(&repFileFormat_reader);
	CleanupStack::PopAndDestroy(&repFileCount_reader);
	CleanupStack::PopAndDestroy(&fs);
	
	//The values should match the ones from the repository file.
	TEST(contactMatchCount2 == 6);
	TEST(contactNameFormat2 == 1);
	
	//delete the repository file c:\\private\\10202be9\\101f401d.txt.
	_LIT(KCommandParameters2,		"c:\\private\\10202be9\\101f401d.txt;private\\10202be9\101f401d.txt;2");
	r = process.Create(KExecutableFileName, KCommandParameters2); 
	TEST(r == KErrNone);
	process.Logon(processWaitStatus);
	process.Resume();
	User::WaitForRequest(processWaitStatus);
	TEST(processWaitStatus.Int() == KErrNone);
	process.Close();

	theLog.Write(_L8("Deleting the Log engine database... \n"));	
	TestUtils::DeleteDatabaseL();
	TheTest.Next(_L("Delay of 2 min, the necessary time to central repository to unload its cache... "));	
	User::After(125000000); // Delay to time to cenrep to unload its cache.
	}
#endif


/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0842
@SYMTestCaseDesc	    Tests for retrieving the log engine configuration data
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for CLogClient::GetConfig() function,check for the integrity of the data
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetConfigL(CLogClient& aClient)
//
//
//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0842 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig config;

	TEST(config.iMaxEventAge == 0);
	TEST(config.iMaxLogSize == 0);
	TEST(config.iMaxRecentLogSize == 0);

	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	aClient.Cancel();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);

	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	aClient.Cancel();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(config.iMaxEventAge > 0);
	TEST(config.iMaxLogSize > 0);
	TEST(config.iMaxRecentLogSize > 0);

	CleanupStack::PopAndDestroy(); // active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0843
@SYMTestCaseDesc	    Tests for CLogClient::ChangeConfig() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Change the configuration data and test for the integrity.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeConfigL(CLogClient& aClient)
//
//
//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0843 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig config;

	config.iMaxLogSize = KTestMaxLogSize;
	config.iMaxRecentLogSize = KTestMaxRecentLogSize;
	config.iMaxEventAge = KTestMaxEventAge;

	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	aClient.Cancel();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);

	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	aClient.Cancel();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(config.iMaxLogSize == KTestMaxLogSize);
	TEST(config.iMaxRecentLogSize == KTestMaxRecentLogSize);
	TEST(config.iMaxEventAge == KTestMaxEventAge);

	CleanupStack::PopAndDestroy(); // active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0844
@SYMTestCaseDesc	    Tests for CLogClient::GetString() function
@SYMTestPriority 	    High
@SYMTestActions  	    Get the string from the resource file and check for no errors and the zero string length
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestGetStringL(CLogClient& aClient)
//
//
//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0844 "));
	TBuf<KLogMaxSharedStringLength> str;
	
	TInt err = aClient.GetString(str, R_LOG_DIR_IN);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DIR_OUT);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DIR_IN_ALT);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DIR_OUT_ALT);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DIR_FETCHED);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DIR_MISSED);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DEL_PENDING);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DEL_SENT);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DEL_FAILED);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DEL_NONE);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DEL_DONE);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DEL_NOT_SENT);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DEL_NOTIFIED);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_DEL_EXPIRED);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_REMOTE_UNKNOWN);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_REMOTE_MULTIPLE);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();

	err = aClient.GetString(str, R_LOG_SUBJECT_NONE);
	TEST2(err, KErrNone);
	TEST(str.Length() > 0);
	str.Zero();
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0845
@SYMTestCaseDesc	    Tests for clearing the event types from the log
@SYMTestPriority 	    High
@SYMTestActions  	    Clear the event types from the log and check for event not found error.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestClearEventLogL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0845 "));
	TTime now;
	now.UniversalTime();
	TDateTime d = now.DateTime();
	TheTest.Printf(_L("TimeNow: Y=%d, M=%d, D=%d, H=%d, M=%d, S=%d\n"), d.Year(), d.Month() + 1, d.Day() + 1, d.Hour(), d.Minute(), d.Second());

	TTime date1(now);
	date1 -= TTimeIntervalDays(1);

	TTime date2(date1);
	date2 -= TTimeIntervalDays(1);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	//////////////////////////////////////////////////////////////////////////////	
	//Clear all events before (current date). Actually - all events!
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//////////////////////////////////////////////////////////////////////////////	
	//The next part of the test will create 4 events:
	//-event1 and event2 will have time = current date - 1 day + 10 seconds;
	//-event3 and event4 will have time = current date - 2 days;
	//The test is:
	//ClearLog - Delete all events, which time is less or equal to current date - 1 day.
	//Check - event3 and event4 should disappear, event1 and event2 should stay.
	//ClearLog - Delete all events, which time is less or equal to current date.
	//Check - event1 and event2 should disappear too.
	//
	//The "10 seconds" addition is very important. The creation of the 4 events is 
	//an operation, which is performed very quickly. It is possible that the 
	//first ClearLog operation will be executed at the same second at which the events
	//were created. Which means, that all events will be deleted and the next check 
	//for the presence of event1 and event2 will fail.

	//////////////////////////////////////////////////////////////////////////////	
	//Create and add new event - event1. 
	//Set event1 date to be (current date - 1 day + 10 seconds).
	CLogEvent* event1 = CLogEvent::NewL();
	CleanupStack::PushL(event1);
	
	event1->SetEventType(KTestEventUid);
	active->StartL();
	aClient.AddEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TTimeIntervalSeconds tenSeconds(10);
	event1->SetTime(date1 + tenSeconds);
	active->StartL();
	aClient.ChangeEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//////////////////////////////////////////////////////////////////////////////	
	//Create and add new event - event2. 
	//Set event2 date to be (current date - 1 day + 10 seconds).
	CLogEvent* event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);
	
	event2->SetEventType(KTestEventUid);
	active->StartL();
	aClient.AddEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event2->SetTime(date1 + tenSeconds);
	active->StartL();
	aClient.ChangeEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//////////////////////////////////////////////////////////////////////////////	
	//Create and add new event - event3. 
	//Set event3 date to be (current date - 2 days).
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

	//////////////////////////////////////////////////////////////////////////////	
	//Create and add new event - event4. 
	//Set event4 date to be (current date - 2 days).
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

	//////////////////////////////////////////////////////////////////////////////	
	//Clear all events before (current date - 1 day).
	//Then cancel the operation.
	TheTest.Printf(_L("=.= ClearLog 1\n"));
	active->StartL();
	aClient.ClearLog(date1, active->iStatus);
	aClient.Cancel();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrCancel);

	//////////////////////////////////////////////////////////////////////////////	
	//Clear all events before (current date - 1 day).
	//event3 and event4 should be removed.
	TheTest.Printf(_L("=.= ClearLog 2\n"));
	active->StartL();
	aClient.ClearLog(date1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//////////////////////////////////////////////////////////////////////////////	
	//Get event1. It should be there - its time is (current date - 1 day + 10 seconds).
	TheTest.Printf(_L("=.= GetEvent 1\n"));
	active->StartL();
	aClient.GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	if(active->iStatus != KErrNone) 
		{
		TheTest.Printf(_L("=1= error code:%d\n"),active->iStatus.Int());
		}
	TEST2(active->iStatus.Int(), KErrNone);

	//////////////////////////////////////////////////////////////////////////////	
	//Get event2. It should be there - its time is (current date - 1 day + 10 seconds).
	TheTest.Printf(_L("=.= GetEvent 2\n"));
	active->StartL();
	aClient.GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	if(active->iStatus != KErrNone) 
		{
		TheTest.Printf(_L("=2= error code:%d\n"),active->iStatus.Int());
		}
	TEST2(active->iStatus.Int(), KErrNone);

	//////////////////////////////////////////////////////////////////////////////	
	//Get event3. It should not be there - its time is (current date - 2 days).
	TheTest.Printf(_L("=.= GetEvent 3\n"));
	active->StartL();
	aClient.GetEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	//////////////////////////////////////////////////////////////////////////////	
	//Get event4. It should not be there - its time is (current date - 2 days).
	TheTest.Printf(_L("=.= GetEvent 4\n"));
	active->StartL();
	aClient.GetEvent(*event4, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	//////////////////////////////////////////////////////////////////////////////	
	//Clear all events happened before (current date).
	//event1 and event2 should be removed.
	TheTest.Printf(_L("=#= ClearLog 1\n"));
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//////////////////////////////////////////////////////////////////////////////	
	//Get event1. It should not be there - its time is (current date - 1 day + 10 seconds).
	TheTest.Printf(_L("=#= GetEvent 1\n"));
	active->StartL();
	aClient.GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	//////////////////////////////////////////////////////////////////////////////	
	//Get event2. It should not be there - its time is (current date - 1 day + 10 seconds).
	TheTest.Printf(_L("=#= GetEvent 2\n"));
	active->StartL();
	aClient.GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(5); // event4, event3, event2, event1, active
	}

LOCAL_C void DoTestLogL(CLogBase& aClient)
	{
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	TTime now;
	now.UniversalTime();

	event->SetEventType(KLogCallEventTypeUid);

	User::After((Math::Random() % 4) * 100000);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TTEST2(active->iStatus.Int(), KErrNone);

	TTEST(event->EventType() == KLogCallEventTypeUid);
	TTEST(event->Description().Length() > 0);
	TTEST(event->Time() >= now);
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

	User::After((Math::Random() % 4) * 100000);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TTEST2(active->iStatus.Int(), KErrNone);

	TTEST(event->Id() == id);
	TTEST(event->EventType() == KLogCallEventTypeUid);
	TTEST(event->Description().Length() > 0);
	TTEST(event->Time() == now);
	TTEST(event->RemoteParty() == KTestRemoteParty1);
	TTEST(event->Direction() == KTestDirection1);
	TTEST(event->DurationType() == KTestDurationType1);
	TTEST(event->Duration() == KTestDuration1);
	TTEST(event->Status() == KTestStatus1);
	TTEST(event->Subject() == KTestSubject1);
	TTEST(event->Number() == KTestNumber1);
	TTEST(event->Contact() == KTestContact1);
	TTEST(event->Link() == KTestLink1);
	TTEST(event->Data() == KTestData1);

	CleanupStack::PopAndDestroy(); // event;

	event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(id);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TTEST2(active->iStatus.Int(), KErrNone);

	TTEST(event->Id() == id);
	TTEST(event->EventType() == KLogCallEventTypeUid);
	TTEST(event->Description().Length() > 0);
	TTEST(event->Time() == now);
	TTEST(event->RemoteParty() == KTestRemoteParty1);
	TTEST(event->Direction() == KTestDirection1);
	TTEST(event->DurationType() == KTestDurationType1);
	TTEST(event->Duration() == KTestDuration1);
	TTEST(event->Status() == KTestStatus1);
	TTEST(event->Subject() == KTestSubject1);
	TTEST(event->Number() == KTestNumber1);
	TTEST(event->Contact() == KTestContact1);
	TTEST(event->Link() == KTestLink1);
	TTEST(event->Data() == KTestData1);

	User::After((Math::Random() % 4) * 100000);
	active->StartL();
	aClient.DeleteEvent(id, active->iStatus);
	CActiveScheduler::Start();
	TTEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TTEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(2); // event, active
	}

void DoTestMultipleClientAccessL()
	{
	CActiveScheduler::Install(new(ELeave)CActiveScheduler);
	CleanupStack::PushL(CActiveScheduler::Current());

	RFs fs;
	TTEST2(fs.Connect(), KErrNone);
	CleanupClosePushL(fs);
	
	CLogClient* client = CLogClient::NewL(fs);
	CleanupStack::PushL(client);

	TBuf8<500> buf;

	TInt count = 10;
	while(count--)
		{
		DoTestLogL(*client);
		}

	CleanupStack::PopAndDestroy(3); // fs, client, CActiveScheduler
	}

void DoThreadDieL()
	{
	CActiveScheduler::Install(new(ELeave)CActiveScheduler);
	CleanupStack::PushL(CActiveScheduler::Current());

	RFs fs;
	TTEST2(fs.Connect(), KErrNone);
	CleanupClosePushL(fs);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	const TInt KLogClientCnt = 10;
	for(TInt i=0;i<KLogClientCnt;i++)
		{
		CLogClient* client = CLogClient::NewL(fs);
		event->SetId(i);
		TRequestStatus status;
		client->GetEvent(*event, status);
		}
	
	User::After(100000);

	RThread thread;
	thread.Kill(KErrGeneral);
	}

static TInt DoDyingThreadStart(TAny*)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TTEST(cleanup != NULL);
	TRAPD(err, DoThreadDieL());		
	delete cleanup;
	if(err != KErrNone)
		{
		RDebug::Print(_L("*** DoThreadDieL() failed with err %d\r\n"), err);
		User::Panic(_L("ThrChk4"), 4);
		}
	return err;
	}

static TInt DoThreadStart(TAny* aThreadNumber)
	{
	TInt thrNum = (TInt)aThreadNumber;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TTEST(cleanup != NULL);
	TRAPD(err, DoTestMultipleClientAccessL());		
	delete cleanup;
	if(err != KErrNone)
		{
		RDebug::Print(_L("*** Thread %d failed with err %d\r\n"), thrNum, err);
		User::Panic(_L("ThrChk3"), 3);
		}
	return err;
	}

const TInt KTestThreadCount = 20;
const TInt KMinTestHeapSize = 0x10000;
const TInt KMaxTestHeapSize = 0x100000;

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0846
@SYMTestCaseDesc	    Thread invoking test
@SYMTestPriority 	    High
@SYMTestActions  	    Create a new thread,check for no errors,register for notification of status when the thread dies
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestThreadDieL()
	{
	RThread thread;
	TRequestStatus status;

	TName name;
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0846 "));
	_LIT(KThreadName, "Test thread");
	name.Format(KThreadName);

	TInt err = thread.Create(name, DoDyingThreadStart, KDefaultStackSize, KMinTestHeapSize, KMaxTestHeapSize, NULL, EOwnerThread);
	// Create the thread
	TEST2(err, KErrNone);
	thread.Logon(status);

	// Let the thread run
	thread.Resume();

	User::WaitForRequest(status);
	TEST2(thread.ExitType(), EExitKill);
	thread.Close();
	TEST2(status.Int(), KErrGeneral);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0847
@SYMTestCaseDesc	    Multiple client access test
@SYMTestPriority 	    High
@SYMTestActions  	    Create threads,request for notification of thread death.
                        Resume and stop all the threads executions.Check for no errors.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMultipleClientAccessL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0847 "));
	RThread threadArray[KTestThreadCount];
	TRequestStatus statusArray[KTestThreadCount];

	// Create the threads
	TInt count = KTestThreadCount;
	while(count--)
		{
		TName name;
		_LIT(KThreadName, "TmcaTh%d");
		name.Format(KThreadName, count);

		// Create the thread
		TInt err = threadArray[count].Create(name, DoThreadStart, KDefaultStackSize, KMinTestHeapSize, KMaxTestHeapSize, (TAny*) count, EOwnerThread);
		TEST2(err, KErrNone);
		threadArray[count].Logon(statusArray[count]);
		}

	// Let the thread run
	count = KTestThreadCount;
	while(count--)
		{
		TheTest.Printf(_L("   ** Resume thread %d\r\n"), count);
		threadArray[count].Resume();
		}

	TheTest.Printf(_L("   ** Waiting threads to complete....\r\n"));
	
	// Wait for all the threads to complete
	count = KTestThreadCount;
	while(count--)
		{
		User::WaitForRequest(statusArray[count]);
		TheTest.Printf(_L("   ** Thread %d completed\r\n"), count);
		TEST(threadArray[count].ExitType() != EExitPanic);
		threadArray[count].Close();
		}
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0848
@SYMTestCaseDesc	    Test for checking notification of status
@SYMTestPriority 	    High
@SYMTestActions  	    Call up add - get - change - delete event type.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestNoNotifierL()
	{
	TestUtils::DeleteDatabaseL();
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0848 "));
	TheTest.Printf(_L("TestNoNotifierL - begin\n"));

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);
	
	TheTest.Printf(_L("TestAddEventTypeL\n"));
	TestAddEventTypeL(*client);

	TheTest.Printf(_L("TestGetEventTypeL\n"));
	TestGetEventTypeL(*client);

	TheTest.Printf(_L("TestChangeEventTypeL\n"));
	TestChangeEventTypeL(*client);

	TheTest.Printf(_L("TestDeleteEventTypeL\n"));
	TestDeleteEventTypeL(*client);

	TheTest.Printf(_L("TestAddEventL\n"));
	TestAddEventL(*client);

	TheTest.Printf(_L("TestGetEventL\n"));
	TestGetEventL(*client);

	TheTest.Printf(_L("TestChangeEventL\n"));
	TestChangeEventL(*client);

	TheTest.Printf(_L("TestDeleteEventL\n"));
	TestDeleteEventL(*client);

	TheTest.Printf(_L("TestGetConfigL\n"));
	TestGetConfigL(*client);

	TheTest.Printf(_L("TestChangeConfigL\n"));
	TestChangeConfigL(*client);

	TheTest.Printf(_L("TestGetStringL\n"));
	TestGetStringL(*client);

	TheTest.Printf(_L("TestClearEventLogL\n"));
	TestClearEventLogL(*client);

	CleanupStack::PopAndDestroy(2); // active, client
	TheTest.Printf(_L("TestNoNotifierL - end\n"));
	}

/**
@SYMTestCaseID          PDS-LOGENG-CT-4016
@SYMTestCaseDesc	    Tests for CLogChangeDefinition public APIs
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for CLogChangeDefinition::Find() functions, and different NewL() functions.
@SYMTestExpectedResults CLogChangeDefinition object need to be created propertly. Find() functions
						need to return proper items from list.
@SYMDEF                 DEF135499
*/
LOCAL_C void TestLogChangeDefinitionL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-CT-4016"));
	TestUtils::DeleteDatabaseL();

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetContact(KTestContact);
	filter->SetEventType(KLogPacketDataEventTypeUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewChangeObserver* changeObs = CLogViewChangeObserver::NewLC();
	changeObs->SetActive();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs);
	CleanupStack::PushL(view);

	// Incoming
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogPacketDataEventTypeUid);
	event->SetDirection(buf);
	event->SetContact(KTestContact);
	
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	User::After(1 * 1000000);
	
	TEST2(view->CountL(), 0);
	active->StartL();
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);
	
	// Transients
	TInt changeCount;
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;
	
	for(TInt count = 0; count < KTestEventNum; count++)
		{
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		User::After(1 * 1000000);
		}

	const CLogChangeDefinition& changes = changeObs->Changes();

	changeCount = changes.Count();
	TheTest.Printf(_L("Change Count: %d\n"), changeCount);
	TEST2(changeCount, KTestEventNum);
	for(TInt i=0; i<changeCount; i++)
		{
		type = changes.At(i, logId, viewIndex);
		TheTest.Printf(_L("Change Type: %d, logId: %d, viewIndex: %d\n"), type, logId, viewIndex);
		TEST(changes.Find(logId)==i);
		TEST(changes.Find(logId, ELogChangeTypeEventAdded)>=0);
		TEST(changes.Find(TLogId(100000000), ELogChangeTypeEventAdded)==KErrNotFound);
		TEST(changes.Find(logId, ELogChangeTypeLogCleared)==KErrNotFound);
		TEST(changes.FindByViewIndex(viewIndex)>=0);
		TEST(changes.Find(TLogId(100000000))==KErrNotFound);
		}
	
	CBufFlat* buffer = CBufFlat::NewL(10*1024);
	CleanupStack::PushL(buffer);
	RBufWriteStream wstr(*buffer,0);
	wstr << changes;
	wstr.CommitL();
	wstr.Close();
	RBufReadStream rstr(*buffer,0);
	CLogChangeDefinition* changes2 = CLogChangeDefinition::NewL(rstr);
	CleanupStack::PushL(changes2);
	rstr.Close();
	
	TEST(changes.Count()==changes2->Count());

	CleanupStack::PopAndDestroy(3); // changes2, buffer, view
	
	// Check the change was as expected
	TEST2(changes.Count(), 10);
	type = changes.At(0, logId, viewIndex);

	CleanupStack::PopAndDestroy(4, event); // changeObs, active, filter, event
	
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(viewIndex, 0); // first (newest) events in the view

	const TLogId expectedLogId = ((TLogId) 1);
	TEST2(logId, expectedLogId);
	}

void DoStartL()
	{
	CActiveScheduler::Install(new(ELeave)CActiveScheduler);
	RFs fs;
	LEAVE_IF_ERROR(fs.Connect());
	CleanupClosePushL(fs);
	CLogClient* client = CLogClient::NewL(fs);
	delete client;
	CleanupStack::PopAndDestroy(&fs);
	delete CActiveScheduler::Current();
	}

static TInt LaunchThread(TAny* /*aAny*/)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(err, DoStartL());		
	delete cleanup;	
	__UHEAP_MARKEND;
	if(err != KErrNone)
		{
		RDebug::Print(_L("*** DoStartL() failed with err %d\r\n"), err);
		User::Panic(_L("ThrChk5"), 5);
		}
	return KErrNone;
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0849
@SYMTestCaseDesc	    Tests for creation of two simultaneous threads
@SYMTestPriority 	    High
@SYMTestActions  	    Create two threads,start and close the threads 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestStartupL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0849 "));
	RThread thread1;
	RThread thread2;

	// Create the threads
	LEAVE_IF_ERROR(thread1.Create(_L("Thread1"), LaunchThread, KDefaultStackSize, KMinHeapSize , KMinHeapSize , NULL));
	LEAVE_IF_ERROR(thread2.Create(_L("Thread2"), LaunchThread, KDefaultStackSize, KMinHeapSize , KMinHeapSize, NULL));

	// Let them run
	TRequestStatus s1;
	thread1.Logon(s1);
	thread1.Resume();
	
	TRequestStatus s2;
	thread2.Logon(s2);
	thread2.Resume();

	User::WaitForRequest(s1);
	TEST(thread1.ExitType() != EExitPanic);
	thread1.Close();
	
	User::WaitForRequest(s2);
	TEST(thread2.ExitType() != EExitPanic);
	thread2.Close();
	
	TEST2(s1.Int(), KErrNone);
	TEST2(s2.Int(), KErrNone);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0850
@SYMTestCaseDesc	    Tests for invalid database scheme 
@SYMTestPriority 	    High
@SYMTestActions  	    Create a log event implementation,should leave if there is a problem
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestInvalidSchemaL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0850 "));
	TestUtils::TestInvalidSchemaL();
	}

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logapi"));
	
	// This test should be first to ensure no clients kicking around
	TheTest.Next(_L("Invalid database scheme"));
	TestInvalidSchemaL();
	theLog.Write(_L8("Test 0 OK\n"));

	TheTest.Start(_L("Simultaneous Startup"));
	TestStartupL();
	theLog.Write(_L8("Test 1 OK\n"));

	CLogChangeNotifier* notifier = CLogChangeNotifier::NewL();
	CleanupStack::PushL(notifier);

	TheTest.Next(_L("Dying thread test"));
	TestThreadDieL();
	theLog.Write(_L8("Test 2 OK\n"));
	
	TestUtils::DeleteDatabaseL();

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CTestActive* active = new(ELeave) CTestActive(CActive::EPriorityIdle - 500);
	CleanupStack::PushL(active);
	
	TheTest.Next(_L("Additional tests on CLogChangeDefinition."));
	TestLogChangeDefinitionL(*client);
	theLog.Write(_L8("Test 2.1 OK\n"));
	TestUtils::DeleteDatabaseL();
	
	TheTest.Next(_L("Client death"));
	TestClientFailL();
	theLog.Write(_L8("Test 3 OK\n"));

	TheTest.Next(_L("Testing client API"));
	TestNoNotifierL();
	theLog.Write(_L8("Test 4 OK\n"));

	// Delay for testing change notification
	TInt delay = 5000000;	
	User::After(delay);	


	active->StartL();
	client->NotifyChange(delay, active->iStatus);

	TheTest.Next(_L("Delete Event Type"));
	TestDeleteEventTypeL(*client);
	theLog.Write(_L8("Test 5 OK\n"));

	CActiveScheduler::Start();
	TEST(active->iStatus.Int() >= 0);

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0834 Add Event Type "));
	TestAddEventTypeL(*client);
	theLog.Write(_L8("Test 6 OK\n"));

	// Must delete the database before testing
	// the notification API, since we just created the
	// entry in the previous test (6) and therefore
	// attempting to add the entry again just results in
	// KErrAlreadyExists and does not cause a change to the
	// database.
	TestUtils::DeleteDatabaseL();

	active->StartL();
	client->NotifyChange(delay, active->iStatus);

	TestAddEventTypeL(*client);

	CActiveScheduler::Start();
	TEST(active->iStatus.Int() >= 0);
	active->StartL();
	client->NotifyChange(delay, active->iStatus);

	// The following doesn't make any changes
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0835 Get Event Type "));
	TestGetEventTypeL(*client);
	theLog.Write(_L8("Test 7 OK\n"));

	TheTest.Next(_L("Change Event Type"));
	TestChangeEventTypeL(*client);
	theLog.Write(_L8("Test 8 OK\n"));

	CActiveScheduler::Start();
	TEST(active->iStatus.Int() >= 0);
	active->StartL();
	client->NotifyChange(delay, active->iStatus);

	TheTest.Next(_L("Delete Event Type"));
	TestDeleteEventTypeL(*client);
	theLog.Write(_L8("Test 9 OK\n"));

	CActiveScheduler::Start();
	TEST(active->iStatus.Int() >= 0);
	active->StartL();
	client->NotifyChange(delay, active->iStatus);

	TheTest.Next(_L("Add Event"));
	TestAddEventL(*client);
	theLog.Write(_L8("Test 10 OK\n"));

	CActiveScheduler::Start();
	TEST(active->iStatus.Int() >= 0);
	active->StartL();
	client->NotifyChange(delay, active->iStatus);

	// The following doesn't make any changes
	TheTest.Next(_L("Get Event"));
	TestGetEventL(*client);
	theLog.Write(_L8("Test 11 OK\n"));

	TheTest.Next(_L("Change Event"));
	TestChangeEventL(*client);
	theLog.Write(_L8("Test 12 OK\n"));

	CActiveScheduler::Start();
	TEST(active->iStatus.Int() >= 0);
	active->StartL();
	client->NotifyChange(delay, active->iStatus);

	TheTest.Next(_L("Delete Event"));
	TestDeleteEventL(*client);
	theLog.Write(_L8("Test 13 OK\n"));

	CActiveScheduler::Start();
	TEST(active->iStatus.Int() >= 0);
	active->StartL();
	client->NotifyChange(delay, active->iStatus);

	// The following doesn't make any changes
	TheTest.Next(_L("Get Config"));
	TestGetConfigL(*client);
	theLog.Write(_L8("Test 14 OK\n"));

	TheTest.Next(_L("Change Config"));
	TestChangeConfigL(*client);
	theLog.Write(_L8("Test 15 OK\n"));

	CActiveScheduler::Start();
	TEST(active->iStatus.Int() >= 0);
	active->StartL();
	client->NotifyChange(delay*3, active->iStatus);

	// The following doesn't make any changes
	TheTest.Next(_L("Get String"));
	TestGetStringL(*client);
	theLog.Write(_L8("Test 16 OK\n"));

	TheTest.Next(_L("Clear Event Log"));
	TestClearEventLogL(*client);
	theLog.Write(_L8("Test 17 OK\n"));

	CActiveScheduler::Start();
	TEST(active->iStatus.Int() >= 0);
	active->StartL();
	client->NotifyChange(delay, active->iStatus);

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0833 Test global change API "));
	TestClientObserverMechanismL(*client);
	theLog.Write(_L8("Test 18 OK\n"));

	TheTest.Next(_L("Multiple client access"));
	TestMultipleClientAccessL();
	theLog.Write(_L8("Test 19 OK\n"));
	
	theLog.Write(_L8("Destroying: active\n"));
	CleanupStack::PopAndDestroy(active);
	theLog.Write(_L8("Destroyed ok\n"));
	theLog.Write(_L8("Destroying: client\n"));
	CleanupStack::PopAndDestroy(client);
	theLog.Write(_L8("Destroyed ok\n"));
	theLog.Write(_L8("Destroying: notifier\n"));
	CleanupStack::PopAndDestroy(notifier);
	theLog.Write(_L8("Destroyed ok\n"));


#ifdef SYSLIBS_TEST		
	theLog.Write(_L8("Preparing the context for the test : @SYMTestCaseID:SYSLIB-LOGENG-UT-4015... \n"));
	theLog.Write(_L8("Deleting the Log engine database... \n"));	
	TestUtils::DeleteDatabaseL();
	theLog.Write(_L8("Allocating a new CLogClient object... \n"));
	CLogClient* theClient = CLogClient::NewL(theFs);
	CleanupStack::PushL(theClient);	
	TheTest.Next(_L("TestGetConfigSettingsFromRepositoryFileL () "));
	TestGetConfigSettingsFromRepositoryFileL(*theClient);
	theLog.Write(_L8("TestGetConfigSettingsFromRepositoryFileL () OK\n"));
	theLog.Write(_L8("Destroying the CLogClient object... \n"));	
	CleanupStack::PopAndDestroy(theClient);
	theLog.Write(_L8("Destroyed ok\n"));
#else
	theLog.Write(_L8("The Test PREQ2103 works only when SYSLIBS_TEST macro is defined"));
#endif
	}
