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

#include "t_logutil2.h"
#include <logview.h>

RTest TheTest(_L("t_lognotify"));

_LIT(KTestRemoteParty1, "Remote Party");
_LIT(KTestDirection1, "Direction");
const TLogDurationType KTestDurationType1 = 1;
const TLogDuration KTestDuration1 = 0x1234;
_LIT(KTestStatus1, "Status");
_LIT(KTestSubject1, "Subject");
_LIT(KTestNumber1, "TheNumber");
const TLogContactItemId KTestContact1 = 0x1234;
const TLogLink KTestLink1 = 0x1234;
_LIT8(KTestData1, "ABCDEFGH");
const TLogFlags KTestFlags1 = 0x5;

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0926
@SYMTestCaseDesc	    Tests for CLogClient::NotifyChange() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for notification of changes to the logengine database.
                        Add events within the time intervals ,check for the notification of an event
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestNotificationL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0926 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CTestActive* active1 = new(ELeave)CTestActive;
	CleanupStack::PushL(active1);

	CTestActive* active2 = new(ELeave)CTestActive;
	CleanupStack::PushL(active2);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetEventType(KLogCallEventTypeUid);
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

	// *** Notification when change made - no delay
	active1->StartL();
	client->NotifyChange(0, active1->iStatus);

	// Add an event
	active2->StartL();
	client->AddEvent(*event, active2->iStatus);
	
	// Not sure which will finish first
	CActiveScheduler::Start();
	if (!active1->IsActive())
		{
		TEST(active2->IsActive());
		TEST(active1->iStatus.Int() >= 0);
		CActiveScheduler::Start();
		TEST2(active2->iStatus.Int(), KErrNone);
		}
	else
		{
		TEST2(active2->iStatus.Int(), KErrNone);
		CActiveScheduler::Start();
		TEST(active1->iStatus.Int() >= 0);
		}

	// Remember the time
	TTime before;
	before.UniversalTime();

	const TTimeIntervalMicroSeconds32 delay = 3000000;

	// *** Notification when change made - with long delay
	active1->StartL();
	client->NotifyChange(delay, active1->iStatus);

	// Add an event
	active2->StartL();
	client->AddEvent(*event, active2->iStatus);
	
	// We can be fairly certain that the function will complete first
	CActiveScheduler::Start();
	TEST2(active2->iStatus.Int(), KErrNone);
	TEST(active1->IsActive());
	CActiveScheduler::Start();
	TEST(active1->iStatus.Int() >= 0);

	User::After(1000000);

	// Check delay
	TTime after;
	after.UniversalTime();
	TEST(before + delay <= after);

	before.UniversalTime();

	// *** Notification when change made - with long delay - multiple changes
	active1->StartL();
	client->NotifyChange(delay, active1->iStatus);

	// Add an event
	active2->StartL();
	client->AddEvent(*event, active2->iStatus);
	
	// The function should complete
	CActiveScheduler::Start();
	TEST(active1->IsActive());
	TEST2(active2->iStatus.Int(), KErrNone);

	// Change the event
	active2->StartL();
	client->ChangeEvent(*event, active2->iStatus);
	
	// The function should complete
	CActiveScheduler::Start();
	TEST(active1->IsActive());
	TEST2(active2->iStatus.Int(), KErrNone);

	// The notification should complete
	CActiveScheduler::Start();
	TEST(active1->iStatus.Int() >= 0);

#ifdef __WINS__
	User::After(1000000);
#endif

	// Check delay
	after.UniversalTime();
	TEST(before + TTimeIntervalMicroSeconds32(delay) <= after);

	// *** Notification when change made - after long delay
	active1->StartL();
	client->NotifyChange(delay, active1->iStatus);

	// Wait around for a time equal to the delay
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(delay);
	CActiveScheduler::Start();
	delete timer;

	// Add an event
	active2->StartL();
	client->AddEvent(*event, active2->iStatus);
	
	// Not sure which will finish first
	CActiveScheduler::Start();
	if (!active1->IsActive())
		{
		TEST(active2->IsActive());
		TEST(active1->iStatus.Int() >= 0);
		CActiveScheduler::Start();
		TEST2(active2->iStatus.Int(), KErrNone);
		}
	else
		{
		TEST2(active2->iStatus.Int(), KErrNone);
		CActiveScheduler::Start();
		TEST(active1->iStatus.Int() >= 0);
		}

	CleanupStack::PopAndDestroy(4); // event, active2, active1, client
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0927
@SYMTestCaseDesc	    Tests for CLogClient::NotifyChangeCancel() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for cancelling of any outstanding notification requests for changes to log engine database
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestCancelNotificationL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0927 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CTestActive* active1 = new(ELeave)CTestActive;
	CleanupStack::PushL(active1);

	// *** Cancel without delay
	active1->StartL();
	client->NotifyChange(0, active1->iStatus);
	client->NotifyChangeCancel();
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrCancel);

	const TInt delay = 3000000;

	// *** Cancel with delay	
	active1->StartL();
	client->NotifyChange(delay, active1->iStatus);

	// Wait around for a bit
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(delay / 2);
	CActiveScheduler::Start();
	delete timer;

	client->NotifyChangeCancel();
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrCancel);

	// *** Cancel with long delay	
	active1->StartL();
	client->NotifyChange(delay, active1->iStatus);

	// Wait around for a bit
	timer = CTestTimer::NewL();
	timer->After(delay * 2);
	CActiveScheduler::Start();
	delete timer;

	client->NotifyChangeCancel();
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrCancel);

	// *** Cancel after change
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	active1->StartL();
	client->NotifyChange(delay, active1->iStatus);

	CTestActive* active2 = new(ELeave)CTestActive;
	CleanupStack::PushL(active2);

	active2->StartL();
	client->AddEvent(*event, active2->iStatus);
	CActiveScheduler::Start();
	TEST(active1->IsActive());
	TEST2(active2->iStatus.Int(), KErrNone);

	client->NotifyChangeCancel();
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrCancel);

	// *** Cancel after change and delay
	active1->StartL();
	client->NotifyChange(delay, active1->iStatus);

	// Wait around for a bit
	timer = CTestTimer::NewL();
	timer->After(delay / 2);
	CActiveScheduler::Start();
	delete timer;

	active2->StartL();
	client->AddEvent(*event, active2->iStatus);
	CActiveScheduler::Start();
	TEST(active1->IsActive());
	TEST2(active2->iStatus.Int(), KErrNone);

	client->NotifyChangeCancel();
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrCancel);

	// *** Cancel after change and delay
	active1->StartL();
	client->NotifyChange(delay, active1->iStatus);

	active2->StartL();
	client->AddEvent(*event, active2->iStatus);
	CActiveScheduler::Start();
	TEST(active1->IsActive());
	TEST2(active2->iStatus.Int(), KErrNone);

	// Wait around for a bit
	timer = CTestTimer::NewL();
	timer->After(delay / 2);
	CActiveScheduler::Start();
	delete timer;

	client->NotifyChangeCancel();
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrCancel);

	// *** Cancel when not active
	client->NotifyChangeCancel();

	CleanupStack::PopAndDestroy(4); // active2, event, active, client
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0928
@SYMTestCaseDesc	    Tests for notification requests on view purge
@SYMTestPriority 	    High
@SYMTestActions  	    Notify changes on log view event
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewPurgeNotifyL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0928 "));
	CTestTimer* timer = CTestTimer::NewL();
	CleanupStack::PushL(timer);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CLogViewEvent* view = CLogViewEvent::NewL(*client);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CTestActive* notify = new(ELeave)CTestActive;
	CleanupStack::PushL(notify);

	// Start notifier
	notify->StartL();
	client->NotifyChange(1000000, notify->iStatus);

	// Wait for 10 seconds
	TEST(notify->IsActive());
	timer = CTestTimer::NewL();
	timer->After(7000000);
	CActiveScheduler::Start();
	delete timer;

	// Setup a view
	TEST(notify->IsActive());
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST(view->CountL() > 0);
		}

	// Wait for 10 seconds
	TEST(notify->IsActive());
	timer = CTestTimer::NewL();
	timer->After(10000000);
	CActiveScheduler::Start();
	delete timer;

	// We shouldn't be notified because nothing has changed
	TEST(notify->IsActive());
	client->NotifyChangeCancel();
	CActiveScheduler::Start();
	TEST2(notify->iStatus.Int(), KErrCancel);

	CleanupStack::PopAndDestroy(6); // notify, client, filter, view, active, timer
	}

/**
INC045485 - AV20 Messaging App crashes when attempting to write New SMS

@SYMTestCaseID          SYSLIB-LOGENG-CT-0929
@SYMTestCaseDesc	    Tests for defect number INC045485L.
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by adding a fax event type
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void INC045485L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0929 "));
	const TInt KEventCnt = 5;//test events count
	const TInt KSizeOfEventData = 400;//should be bigger than KLogSizeOfEventGuess constant
	//Create client, active, event
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	CTestActive* active = new (ELeave) CTestActive;
	CleanupStack::PushL(active);
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	//Add KEventCnt events
	for(TInt i=0;i<KEventCnt;++i)
		{
		event->SetId(0x13579 + i);
		event->SetEventType(KLogFaxEventTypeUid);
		TUint8 buf[KSizeOfEventData];
		Mem::Fill(buf, KSizeOfEventData, TChar('1' + i));
		TPtr8 ptr(buf, KSizeOfEventData, KSizeOfEventData);
		event->SetDataL(ptr);
		//
		active->StartL();
		client->AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST(!active->IsActive());
		client->Cancel();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	//Create LogViewEvent, CLogFilter
	CLogViewEvent* view = CLogViewEvent::NewL(*client);
	CleanupStack::PushL(view);
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetEventType(KLogFaxEventTypeUid);//the same as the type UID of added events
	//Filter the events
	if(view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST(view->CountL() == KEventCnt);
		}
	//Visit the events
	TInt j = 0;
	TEST(view->FirstL(active->iStatus));
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);//If the defect is not fixed this check fails with "KErrOverflow"
		const CLogEvent& e = view->Event();
		TheTest.Printf(_L("View Entry[%d]: Id:%d, Type:%x\n"), j, e.Id(), e.EventType().iUid);
		const TDesC8& data = e.Data();
		TEST(data.Length() == KSizeOfEventData);
		//Touch the data.
		for(TInt k=0;k<KSizeOfEventData;++k)
			{
			TChar c = data[k];
			TEST(c >= TChar('1') && c < TChar('1' + KEventCnt));
			}
		++j;
		}
	while(view->NextL(active->iStatus));
	//Destroy filter, view, event, active, client
	CleanupStack::PopAndDestroy(filter);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(event);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

//DEF060381  Propagated: Receiving Chinese SMSes causes the battery to drain too fast 
static void DEF060381L()
	{
	const TInt KSizeOfEventData = 1025; //should be bigger than KLogSizeOfEventGuess constant

	//Create two clients. The first one will be used to send one message to the server,
	//which length is too big. The second client will be used to receive the message from
	//the server.
	//If the defect is not fixed, the function hangs and its return point is never reached.
	CLogClient* client1 = CLogClient::NewL(theFs);
	CleanupStack::PushL(client1);

	CLogClient* client2 = CLogClient::NewL(theFs);
	CleanupStack::PushL(client2);

	CTestActive* active = new (ELeave) CTestActive;
	CleanupStack::PushL(active);

	//Create one SMS event, which holds a block of data with size bigger than the default 
	//size of the client side buffer (KLogSizeOfEventGuess).
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(0x9991118);
	event->SetEventType(KLogShortMessageEventTypeUid);
	TUint8 buf[KSizeOfEventData];
	for(TInt i=0;i<KSizeOfEventData;++i)
		{
		buf[i] = static_cast <TUint8> (i % 256);
		}
	TPtr8 ptr(buf, KSizeOfEventData, KSizeOfEventData);
	event->SetDataL(ptr);

	//Send the created event to the server unsing the first client.
	active->StartL();
	client1->AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST(!active->IsActive());
	TEST2(active->iStatus.Int(), KErrNone);

	//Create a view using the second client.
	//Create a filter for SMS messages.
	CLogViewEvent* view = CLogViewEvent::NewL(*client2);
	CleanupStack::PushL(view);
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetEventType(KLogShortMessageEventTypeUid);//the same as the type UID of added events
	//Filter the events
	if(view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();//Here the test fucntion hangs and never returns, if the defect is not fixed.
		TEST(view->CountL() == 1);
		}
	//Visit the events
	TInt j = 0;
	TEST(view->FirstL(active->iStatus));
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		const CLogEvent& e = view->Event();
		TheTest.Printf(_L("View Entry[%d]: Id:%d, Type:%x\n"), j, e.Id(), e.EventType().iUid);
		const TDesC8& data = e.Data();
		TEST(data.Length() == KSizeOfEventData);
		//Touch the data.
		for(TInt k=0;k<KSizeOfEventData;++k)
			{
			TUint8 c = data[k];
			TEST(c == static_cast <TUint8> (k % 256));
			}
		++j;
		}
	while(view->NextL(active->iStatus));

	CleanupStack::PopAndDestroy(filter);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(event);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client2);
	CleanupStack::PopAndDestroy(client1);
	}

void doTestsL()
	{
	TestUtils::Initialize(_L("t_lognotify"));
	TestUtils::DeleteDatabaseL();

	TheTest.Start(_L("Notifications"));
	TestNotificationL();
	theLog.Write(_L8("Test 1 OK\n"));

	TheTest.Next(_L("INC045485"));
	::INC045485L();

	TheTest.Next(_L("Cancelling Notifications"));
	TestCancelNotificationL();
	theLog.Write(_L8("Test 2 OK\n"));

	TheTest.Next(_L("Notify with View Purge"));
	TestViewPurgeNotifyL();
	theLog.Write(_L8("Test 3 OK\n"));

	TheTest.Next(_L("DEF060381"));
	::DEF060381L();
	}
