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
#include <babackup.h>
#include "t_logutil2.h"
#include <logview.h>

RTest TheTest(_L("t_logbackup"));

CBaBackupSessionWrapper* theBackup = NULL;

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
#ifdef __WINS__

_LIT(KTestEventDesc, "Event Type Description");
_LIT(KTestRemoteParty, "Remote Party");
_LIT(KTestDirection, "Direction");
const TLogDurationType KTestDurationType = 1;
const TLogDuration KTestDuration = 0x1234;
_LIT(KTestStatus, "Status");
_LIT(KTestSubject, "Subject");
_LIT(KTestNumber, "Number");
const TLogContactItemId KTestContact = 0x1234;
const TLogLink KTestLink = 0x1234;
_LIT8(KTestData, "ABCDEFGH");
_LIT(KTestNumber1, "TheNumber");
_LIT(KTestNumber2, "Changed Number");

class TClientObserverTestReceiver : public MLogClientChangeObserver
	{
public:
	TClientObserverTestReceiver(TBool& aFlag) : iCallCount(0), iFlag(aFlag) { }

public:
	void HandleLogClientChangeEventL(TUid aChangeType, TInt /*aChangeParam1*/, TInt /*aChangeParam2*/, TInt /*aChangeParam3*/)
		{
		++iCallCount;
		iFlag = (aChangeType == KLogClientChangeEventRefreshView);
		}
	inline TInt CallCount() const { return iCallCount; }
	inline void Reset() { iCallCount = 0; iFlag = EFalse; }

private:
	TInt iCallCount;
	TBool& iFlag;
	};
	
LOCAL_C TBool IsLogOpenL()
	{
	return TestUtils::IsDatabaseOpenL();
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0993
@SYMTestCaseDesc	    Tests for a file open
@SYMTestPriority 	    High
@SYMTestActions  	    Check for errors while testing for file open 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestLogOpenL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0993 "));
	TEST(IsLogOpenL());
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0994
@SYMTestCaseDesc	    Tests for file close
@SYMTestPriority 	    High
@SYMTestActions  	    Check for errors while testing for file close
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestLogClosedL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0994 "));
	TEST(!IsLogOpenL());
	}

LOCAL_C void StartBackupL()
	{
	User::InfoPrint(_L("Backup"));

	TDriveUnit driveUnit(EDriveC);
	TDriveName name = driveUnit.Name();

	TParse parse;
	LEAVE_IF_ERROR(parse.Set(KLogDatabaseName, &name, NULL));

	delete theBackup;
	theBackup = NULL;
	theBackup = CBaBackupSessionWrapper::NewL();

	theBackup->CloseFileL(parse.FullName(), MBackupObserver::EReleaseLockNoAccess);
	User::After(1000000);
	}

LOCAL_C void DeleteLogDatabaseL()
	{
	User::InfoPrint(_L("delete"));
	TestUtils::DeleteDatabaseL(EFalse);
	}

LOCAL_C void EndBackupL()
	{
	delete theBackup;
	theBackup = NULL;
	User::After(1000000);
	}

LOCAL_C void DelayL(TInt aDelay)
	{
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(aDelay);
	CActiveScheduler::Start();
	delete timer;
	}


const TUid KTestEventUid = {0x10005393};
_LIT(KTestEventDesc1, "Event Type Description");
_LIT(KTestEventDesc2, "Changed Event Description");

LOCAL_C void ForceClientImplInstL(CLogClient& aClient)
	{
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);
	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc1);
	type->SetLoggingEnabled(ETrue);
	
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);
	aClient.AddEventType(*type, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(2);//type, active
	
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0995
@SYMTestCaseDesc	    Tests for starting and ending the backup session 
@SYMTestPriority 	    High
@SYMTestActions  	    Test for file open and close,start and end of the backup session
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestLockingL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0995 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	ForceClientImplInstL(*client);


	DelayL(1000000);
	TestLogOpenL();
	StartBackupL();
	DelayL(1000000);

	TestLogClosedL();

	EndBackupL();
	TestLogOpenL();

	CleanupStack::PopAndDestroy(); // client
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0996
@SYMTestCaseDesc	    Tests for deleting the file when its locked
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt for deleting the file when its locked
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDeleteWhileLockedL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0996 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	ForceClientImplInstL(*client);

	TestLogOpenL();
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();

	CleanupStack::PopAndDestroy(); // client
	TestLogClosedL();

	client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	ForceClientImplInstL(*client);
	EndBackupL();

	TestLogOpenL();
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	CleanupStack::PopAndDestroy(); // client
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0997
@SYMTestCaseDesc	    Tests for deleting the view while its locked
@SYMTestPriority 	    High
@SYMTestActions  	    Delete the view when its locked
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDeleteViewWhileLockedL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0997 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	ForceClientImplInstL(*client);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogViewEvent* view = CLogViewEvent::NewL(*client);
	CleanupStack::PushL(view);

	TestLogOpenL();
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();

	CleanupStack::PopAndDestroy(3); // view, filter, client
	TestLogClosedL();

	client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	ForceClientImplInstL(*client);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	view = CLogViewEvent::NewL(*client);
	CleanupStack::PushL(view);
	EndBackupL();

	TestLogOpenL();
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	CleanupStack::PopAndDestroy(3); // view, filter, client
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0998
@SYMTestCaseDesc	    Multiple client logging test
@SYMTestPriority 	    High
@SYMTestActions  	    Open multiple clients and test for starting and ending the backup 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMultipleClientLockingL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0998 "));
	CLogClient* client1 = CLogClient::NewL(theFs);
	CleanupStack::PushL(client1);
	ForceClientImplInstL(*client1);

	TestLogOpenL();

	CLogClient* client2 = CLogClient::NewL(theFs);
	CleanupStack::PushL(client2);
	ForceClientImplInstL(*client2);

	TestLogOpenL();

	StartBackupL();
	EndBackupL();
	TestLogOpenL();

	StartBackupL();
	TestLogClosedL();

	EndBackupL();
	TestLogOpenL();

	CleanupStack::PopAndDestroy(2); // client1, client2
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0999
@SYMTestCaseDesc	    Multiple view locking test
@SYMTestPriority 	    High
@SYMTestActions  	    Open multiple views and test for starting and ending of backup
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMultipleViewLockingL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0999 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	ForceClientImplInstL(*client);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogViewEvent* view1 = CLogViewEvent::NewL(*client);
	CleanupStack::PushL(view1);

	CLogViewEvent* view2 = CLogViewEvent::NewL(*client);
	CleanupStack::PushL(view2);

	TestLogOpenL();
	StartBackupL();
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	CleanupStack::PopAndDestroy(4); // view2, view1, filter, client
	}

LOCAL_C void DoTestLogL(CLogBase& aClient, TInt aError)
	{
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
	TEST2(active->iStatus.Int(), aError);

	if (aError == KErrNone)
		{
		TEST(event->EventType() == KLogCallEventTypeUid);
		TEST(event->Description().Length() > 0);
		TEST(event->Time() >= now);
		now = event->Time();
		}
	else
		{
		// User a dummy id to prevent assertion
		event->SetId(123);
		}

	TLogId id = event->Id();

	event->SetRemoteParty(KTestRemoteParty);
	event->SetDirection(KTestDirection);
	event->SetDurationType(KTestDurationType);
	event->SetDuration(KTestDuration);
	event->SetStatus(KTestStatus);
	event->SetSubject(KTestSubject);
	event->SetNumber(KTestNumber);
	event->SetContact(KTestContact);
	event->SetLink(KTestLink);
	event->SetDataL(KTestData);

	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), aError);

	if (aError == KErrNone)
		{
		TEST(event->Id() == id);
		TEST(event->EventType() == KLogCallEventTypeUid);
		TEST(event->Description().Length() > 0);
		TEST(event->Time() == now);
		TEST(event->RemoteParty() == KTestRemoteParty);
		TEST(event->Direction() == KTestDirection);
		TEST(event->DurationType() == KTestDurationType);
		TEST(event->Duration() == KTestDuration);
		TEST(event->Status() == KTestStatus);
		TEST(event->Subject() == KTestSubject);
		TEST(event->Number() == KTestNumber);
		TEST(event->Contact() == KTestContact);
		TEST(event->Link() == KTestLink);
		TEST(event->Data() == KTestData);
		}

	CleanupStack::PopAndDestroy(); // event;

	event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(id);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), aError);

	if (aError == KErrNone)
		{
		TEST(event->Id() == id);
		TEST(event->EventType() == KLogCallEventTypeUid);
		TEST(event->Description().Length() > 0);
		TEST(event->Time() == now);
		TEST(event->RemoteParty() == KTestRemoteParty);
		TEST(event->Direction() == KTestDirection);
		TEST(event->DurationType() == KTestDurationType);
		TEST(event->Duration() == KTestDuration);
		TEST(event->Status() == KTestStatus);
		TEST(event->Subject() == KTestSubject);
		TEST(event->Number() == KTestNumber);
		TEST(event->Contact() == KTestContact);
		TEST(event->Link() == KTestLink);
		TEST(event->Data() == KTestData);
		}

	active->StartL();
	aClient.DeleteEvent(id, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), aError);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TheTest.Printf(_L("*** active->iStatus.Int()=%d\n"), active->iStatus.Int());
	TEST(active->iStatus.Int() == aError || active->iStatus.Int() == KErrNotFound);

	CleanupStack::PopAndDestroy(2); // event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1000
@SYMTestCaseDesc	    Client requests test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for client requests when the file is opened and check for no errors
						Tests for client requests when the file is closed and check for access denied errors
						Re-test for client requests when file is opened and check for no errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestClientRequestsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1000 "));
	CLogClient* client1 = CLogClient::NewL(theFs);
	CleanupStack::PushL(client1);
	ForceClientImplInstL(*client1);

	TestLogOpenL();

	CLogClient* client2 = CLogClient::NewL(theFs);
	CleanupStack::PushL(client2);
	ForceClientImplInstL(*client2);

	TestLogOpenL();

	DoTestLogL(*client1, KErrNone);
	DoTestLogL(*client2, KErrNone);

	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();

	DoTestLogL(*client1, KErrAccessDenied);
	DoTestLogL(*client2, KErrAccessDenied);

	EndBackupL();
	TestLogOpenL();

	DoTestLogL(*client1, KErrNone);
	DoTestLogL(*client2, KErrNone);

	CleanupStack::PopAndDestroy(2); // client1, client2
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1001
@SYMTestCaseDesc	    Tests for notification changes
@SYMTestPriority 	    High
@SYMTestActions  	    Set up for client log notification changes.
                        Change the log and unlock the log.Try and change the log again and check for access denied error.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestNotificationsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1001 "));
	CTestActive* notify = new(ELeave)CTestActive;
	CleanupStack::PushL(notify);

	CTestTimer* timer = CTestTimer::NewL();
	CleanupStack::PushL(timer);

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	ForceClientImplInstL(*client);
	EndBackupL();

	// Setup change notification
	notify->StartL();
	client->NotifyChange(10000000, notify->iStatus);

	// Wait a second
	timer->After(1000000);
	CActiveScheduler::Start();

	// Change the log
	DoTestLogL(*client, KErrNone);

	// Unlock the log
	TestLogOpenL();
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();

	// Try and change the log again
	DoTestLogL(*client, KErrAccessDenied);

	// Make sure the notifier doesn't complete
	timer->After(15000000);
	CActiveScheduler::Start();
	TEST(notify->IsActive());

	// Lock the log
	EndBackupL();
	TestLogOpenL();

	// Wait for notification
	CActiveScheduler::Start();
	TEST(!notify->IsActive());

	// Setup change notification
	notify->StartL();
	client->NotifyChange(10000000, notify->iStatus);

	// Wait a second
	timer->After(1000000);
	CActiveScheduler::Start();

	// Unlock the log
	TestLogOpenL();
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();

	// Check notification is completed when client destroyed
	TEST(notify->IsActive());
	CleanupStack::PopAndDestroy(client);

	// Wait for notification
	CActiveScheduler::Start();	
	TEST(!notify->IsActive());
	TEST2(notify->iStatus.Int(), KErrCancel);

	// Recreate client
	client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	// Setup change notification
	notify->StartL();
	client->NotifyChange(10000000, notify->iStatus);

	// Wait a second
	timer->After(1000000);
	CActiveScheduler::Start();

	// Check notification is completed when cancelled
	TEST(notify->IsActive());
	client->NotifyChangeCancel();

	// Wait for notification
	CActiveScheduler::Start();	
	TEST(!notify->IsActive());
	TEST2(notify->iStatus.Int(), KErrCancel);

	// Setup change notification
	notify->StartL();
	client->NotifyChange(10000000, notify->iStatus);

	// Wait a second
	timer->After(1000000);
	CActiveScheduler::Start();

	// Lock the log
	EndBackupL();
	TestLogOpenL();

	// Wait for notification
	TEST(notify->IsActive());
	CActiveScheduler::Start();	
	TEST(!notify->IsActive());
	TEST(notify->iStatus >= 0);

	notify->StartL();
	client->NotifyChange(10000000, notify->iStatus);

	// Wait a second
	timer->After(1000000);
	CActiveScheduler::Start();

	// Change the log
	DoTestLogL(*client, KErrNone);

	// Wait for notification
	TEST(notify->IsActive());
	CActiveScheduler::Start();	
	TEST(!notify->IsActive());
	TEST(notify->iStatus >= 0);

	CleanupStack::PopAndDestroy(3); // client, timer, notify
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1002
@SYMTestCaseDesc	    Tests for request in progress
@SYMTestPriority 	    High
@SYMTestActions  	    Add an event when backup is started and check for access denied error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRequestInProgressL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1002 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	ForceClientImplInstL(*client);
	EndBackupL();

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetEventType(KLogCallEventTypeUid);

	active->StartL();
	client->AddEvent(*event, active->iStatus);
	StartBackupL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrAccessDenied);
	DelayL(1000000);

	// User a dummy id to prevent assertion
	event->SetId(123);

	EndBackupL();
	active->StartL();
	client->ChangeEvent(*event, active->iStatus);
	StartBackupL();

	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrAccessDenied);
	DelayL(1000000);

	EndBackupL();
	active->StartL();
	client->GetEvent(*event, active->iStatus);
	StartBackupL();

	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrAccessDenied);
	DelayL(1000000);

	EndBackupL();
	active->StartL();
	client->DeleteEvent(event->Id(), active->iStatus);
	StartBackupL();

	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrAccessDenied);
	DelayL(1000000);

	EndBackupL();
	active->StartL();
	client->GetEvent(*event, active->iStatus);
	StartBackupL();

	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrAccessDenied);
	DelayL(1000000);

	// Check the log is useable
	EndBackupL();
	DoTestLogL(*client, KErrNone);

	CleanupStack::PopAndDestroy(3); // client, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1003
@SYMTestCaseDesc	    Tests for log view when request is in progress
@SYMTestPriority 	    High
@SYMTestActions  	    Add events to the log,set a filter on log view and start the backup session 
                        and check for the request status for access denied.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewRequestInProgressL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1003 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	ForceClientImplInstL(*client);
	EndBackupL();

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogViewEvent* view = CLogViewEvent::NewL(*client);
	CleanupStack::PushL(view);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	active->StartL();
	client->AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	client->AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TestLogOpenL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	active->StartL();
	StartBackupL();

	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrAccessDenied);
	DelayL(1000000);
	TestLogClosedL();

	EndBackupL();
	TestLogOpenL();

	TEST(view->SetFilterL(*filter, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	TEST(view->NextL(active->iStatus));
	active->StartL();
	StartBackupL();

	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrAccessDenied);
	DelayL(1000000);
	TestLogClosedL();

	EndBackupL();
	TestLogOpenL();

	TEST(view->SetFilterL(*filter, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Now check a view is no longer valid after a backup
	TEST(view->SetFilterL(*filter, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST(view->CountL() > 1);

	StartBackupL();
	EndBackupL();

	DelayL(1000000);

	// Check the view can be setup again
	TEST(view->SetFilterL(*filter, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST(view->CountL() > 1);

	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(5); // event, view, filter, active, client
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1004
@SYMTestCaseDesc	    Tests for view event log
@SYMTestPriority 	    High
@SYMTestActions  	    Create an event type and add the event to the log.
                        Close the log and try setting the filter on view.Check for access denied error.
						Repeat the operation after opening the log and check for no error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewSetup1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1004 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	ForceClientImplInstL(*client);
	EndBackupL();

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogViewEvent* view = CLogViewEvent::NewL(*client);
	CleanupStack::PushL(view);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	active->StartL();
	client->AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TestLogOpenL();
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();

	TRAPD(error, view->SetFilterL(*filter, active->iStatus));
	TEST2(error, KErrAccessDenied);

	EndBackupL();
	TestLogOpenL();

	DelayL(1000000);
	TEST(view->SetFilterL(*filter, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	//
	CleanupStack::PopAndDestroy(5); // event, view, filter, active, client
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1005
@SYMTestCaseDesc	    Tests for view event log
@SYMTestPriority 	    High
@SYMTestActions  	    Create an event type and add the event to the log.
                        Close the log and try setting the filter on view twice with an interval of one second.Check for access denied errors.
						Repeat the operation after opening the log and check for no error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewSetup2L()
 	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1005 "));
 	CLogClient* client = CLogClient::NewL(theFs);
 	CleanupStack::PushL(client);
 	ForceClientImplInstL(*client);
 	EndBackupL();
 
 	CTestActive* active = new(ELeave)CTestActive();
 	CleanupStack::PushL(active);
 
 	CLogFilter* filter = CLogFilter::NewL();
 	CleanupStack::PushL(filter);
 
 	CLogViewEvent* view = CLogViewEvent::NewL(*client);
 	CleanupStack::PushL(view);
 
 	CLogEvent* event = CLogEvent::NewL();
 	CleanupStack::PushL(event);
 	event->SetEventType(KLogCallEventTypeUid);
 
 	active->StartL();
 	client->AddEvent(*event, active->iStatus);
 	CActiveScheduler::Start();
 	TEST2(active->iStatus.Int(), KErrNone);
 
 	DelayL(1000000);
 	TEST(view->SetFilterL(*filter, active->iStatus));
 	active->StartL();
 	CActiveScheduler::Start();
 	TEST2(active->iStatus.Int(), KErrNone);
 
 	TestLogOpenL();
 	StartBackupL();
 	TestLogClosedL();
 
 	TRAPD(error, view->SetFilterL(*filter, active->iStatus));
 	TEST2(error, KErrAccessDenied);
 
 	DelayL(1000000);
 
 	TRAP(error, view->SetFilterL(*filter, active->iStatus));
 	TEST2(error, KErrAccessDenied);
 
 	EndBackupL();
 	TestLogOpenL();
 
 //	DelayL(1000000);
 	TEST(view->SetFilterL(*filter, active->iStatus));
 	active->StartL();
 	CActiveScheduler::Start();
 	TEST2(active->iStatus.Int(), KErrNone);
 
 	StartBackupL();
 	DelayL(1000000);
 	TestLogClosedL();
 	//
 	CleanupStack::PopAndDestroy(5); // event, view, filter, active, client
 	}

/**
Check that a KLogClientChangeEventRefreshView message is sent if the database is changed
during a backup.  This could occur if the database is restored.
 
This was added to test the fix for DEF051602 - Problems with MLogViewChangeObserver when a backup occurs

@SYMTestCaseID          SYSLIB-LOGENG-CT-1006
@SYMTestCaseDesc	    Check for any problems with MLogViewChangeObserver when a backup occurs
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a KLogClientChangeEventRefreshView message is sent if the database is changed
						during a backup.  This could occur if the database is restored.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRefreshViewL()
{
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1006 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	// Create a test observer MLogClientChangeObserver
	TBool logRefreshViewFlag = EFalse;
	TClientObserverTestReceiver testReceiver(logRefreshViewFlag);
	client->SetGlobalChangeObserverL(&testReceiver);

	// start a backup
	TestLogOpenL();
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	
	// delete the database so a new empty db will be created
	// This will cause a KLogClientChangeEventRefreshViewRefresh 
	// message to be sent
	DeleteLogDatabaseL();

	// end the backup
	EndBackupL();
	DelayL(1000000);
	

	// check the obsever hs recieved a message
	TEST(logRefreshViewFlag);
	// 
	
	CleanupStack::PopAndDestroy(client);
}


#endif//__WINS__
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logbackup"));
	TestUtils::DeleteDatabaseL();

    TheTest.Start(_L("t_logbackup"));
	
#ifndef __WINS__
	TheTest.Printf(_L("This test harness will only work on WINS\n"));
	User::After(5000000);
#else//__WINS__
	CLogChangeNotifier* notifier = CLogChangeNotifier::NewL();
	CleanupStack::PushL(notifier);

	TheTest.Next(_L("Locking"));
	TestLockingL();
	theLog.Write(_L8("Test 1 OK\n"));

	TheTest.Next(_L("Delete client while locked"));
	TestDeleteWhileLockedL();
	theLog.Write(_L8("Test 2 OK\n"));

	TheTest.Next(_L("Delete view while locked"));
	TestDeleteViewWhileLockedL();
	theLog.Write(_L8("Test 3 OK\n"));

	TheTest.Next(_L("Multiple clients"));
	TestMultipleClientLockingL();
	theLog.Write(_L8("Test 4 OK\n"));

	TheTest.Next(_L("Multiple views"));
	TestMultipleViewLockingL();
	theLog.Write(_L8("Test 5 OK\n"));

	TheTest.Next(_L("Client Requests"));
	TestClientRequestsL();
	theLog.Write(_L8("Test 6 OK\n"));

	TheTest.Next(_L("View setup"));
	TestViewSetup1L();
	TestViewSetup2L();
	theLog.Write(_L8("Test 7 OK\n"));

	TheTest.Next(_L("Notifications"));
	TestNotificationsL();
	theLog.Write(_L8("Test 8 OK\n"));

	TheTest.Next(_L("Request in progress"));
	TestRequestInProgressL();
	theLog.Write(_L8("Test 9 OK\n"));

	TheTest.Next(_L("View request in progress"));
	TestViewRequestInProgressL();
	theLog.Write(_L8("Test 10 OK\n"));

	TheTest.Next(_L("Refresh view"));
	TestRefreshViewL();
	theLog.Write(_L8("Test 11 OK\n"));

	CleanupStack::PopAndDestroy(notifier);
#endif//__WINS__
	}
