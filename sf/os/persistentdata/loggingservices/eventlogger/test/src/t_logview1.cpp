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
#include <s32mem.h>
#include <e32math.h>
#include "t_logutil2.h"
#include <logview.h>

#define UNUSED_VAR(a) a = a

RTest TheTest(_L("t_logview1"));

const TInt KTestLogNumberCharsToMatch = 9; // should be the same as KLogNumberCharsToMatch defined in LogServ\src\LOGFILTQ.CPP
const TInt KTestEventNum = 10;
const TUid KTestEventUid = {0x10005393};
const TLogDurationType KTestDurationType = 0x12;
const TLogContactItemId KTestContact = 0x1234;
_LIT(KTestEventDesc, "Test Event");
_LIT(KTestRemoteParty, "Test Remote Party");
_LIT(KTestDirection, "Test Direction");
_LIT(KTestStatus, "Test Status");
_LIT(KTestNumber, "Test Number");
_LIT(KThreadSemaphoreName, "T_VIEW1_TEST_CODE");


TBool TheMatchingIsEnabled = EFalse;

#define TEST_LOG_UID KTestEventUid

//View's DoCancel() test
void DoViewCancelTestL(CTestActive& aActive, CLogViewEvent& aView)
	{
	//Call FirstL() and cancel the operation
	aActive.StartL();
	(void)aView.FirstL(aActive.iStatus);
	aView.Cancel();
	TEST(!aView.IsActive());
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrCancel);
	
	//Call FirstL(). View cursor positioned on the first record.
	aActive.StartL();
	(void)aView.FirstL(aActive.iStatus);
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);

	//Call NextL() and cancel the operation
	aActive.StartL();
	(void)aView.NextL(aActive.iStatus);
	aView.Cancel();
	TEST(!aView.IsActive());
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrCancel);

	//Call LastL() and cancel the operation
	aActive.StartL();
	(void)aView.LastL(aActive.iStatus);
	aView.Cancel();
	TEST(!aView.IsActive());
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrCancel);

	//Call LastL(). View cursor positioned on the last record.
	aActive.StartL();
	(void)aView.LastL(aActive.iStatus);
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);

	//Call PreviousL() and cancel the operation
	aActive.StartL();
	(void)aView.PreviousL(aActive.iStatus);
	aView.Cancel();
	TEST(!aView.IsActive());
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrCancel);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0851
@SYMTestCaseDesc	    Tests for adding event types to the log 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for adding events,check event count,clearing all the events
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventViewL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0851 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TInt count;
	for(count = 0; count < KTestEventNum; count++)
		{
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	CLogViewChangeObserver* changeObs = CLogViewChangeObserver::NewLC();
	changeObs->SetActive();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	count = view->CountL();
	TEST2(count, 0);
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);

	// Move forward
	count = KTestEventNum;
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		const TInt viewCount = view->CountL();
		TEST2(viewCount, KTestEventNum);
		const TLogId eventId = view->Event().Id();
		TLogId id = --count;
		TEST2(eventId, id);
		}
	while(view->NextL(active->iStatus));
	TEST2(count, 0);

	// Move back
	while(view->PreviousL(active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST2(view->CountL(), KTestEventNum);
		TLogId id = ++count;
		TEST2(view->Event().Id(), id);
		}
	TEST2(count, KTestEventNum - 1);

	active->StartL();
	res = view->FirstL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	res = view->LastL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Nothing has changed in the view yet
	TEST(!changeObs->HaveChanges());

	DoViewCancelTestL(*active, *view);
	
	TTime now;
	now.UniversalTime();
	now+=(TTimeIntervalDays) 1;

	// Clear all the events
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	res = view->SetFilterL(*filter, active->iStatus);
	TEST(!res);
	TEST2(view->CountL(), 0);
	res = view->FirstL(active->iStatus);
	TEST(!res);
	res = view->LastL(active->iStatus);
	TEST(!res);
	res = view->NextL(active->iStatus);
	TEST(!res);
	res = view->PreviousL(active->iStatus);
	TEST(!res);
	
	CleanupStack::PopAndDestroy(5, event); // filter, view, changeObs, active, event

	CLogSchedulerTimer* timer = CLogSchedulerTimer::NewLC();
	timer->Wait(10 * 1000000);
	CleanupStack::PopAndDestroy(timer);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0852
@SYMTestCaseDesc	    Tests for adding event types to the filter list
@SYMTestPriority 	    High
@SYMTestActions  	    Set the configuration data of event type.Add the event type to the filter and 
                        compare the configuration data of event type and from filter view.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewFilterL(CLogClient& aClient, CLogFilter& aFilter)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0852 "));
	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetEventType(KTestEventUid);
	event->SetRemoteParty(aFilter.RemoteParty());
	event->SetDirection(aFilter.Direction());
	event->SetDurationType(aFilter.DurationType());
	event->SetStatus(aFilter.Status());
	event->SetContact(aFilter.Contact());
	event->SetNumber(aFilter.Number());

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	if (view->SetFilterL(aFilter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	TInt total = view->CountL();

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TBool res = view->SetFilterL(aFilter, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST2(view->CountL(), total + 1);

	TEST2(event->EventType().iUid, KTestEventUid.iUid);
	TEST(event->RemoteParty() == aFilter.RemoteParty());
	TEST(event->Direction() == aFilter.Direction());
	TEST2(event->DurationType(), aFilter.DurationType());
	TEST(event->Status() == aFilter.Status());
	TEST2(event->Contact(), aFilter.Contact());
	TEST(event->Number() == aFilter.Number());

	TTime now;
	now.UniversalTime();

	TInt count = 1;

	while(view->NextL(active->iStatus))
		{
		count++;
		TEST(count <= total + 1);

		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST(now >= event->Time());

		if (aFilter.EventType() != KNullUid)
			TEST2(event->EventType().iUid, aFilter.EventType().iUid);

		if (aFilter.RemoteParty().Length() > 0)
			TEST(event->RemoteParty() == aFilter.RemoteParty());

		if (aFilter.Direction().Length() > 0)
			TEST(event->Direction() == aFilter.Direction());

		if (aFilter.DurationType() != KLogNullDurationType)
			TEST2(event->DurationType(), aFilter.DurationType());

		if (aFilter.Status().Length() > 0)
			TEST(event->Status() == aFilter.Status());

		if (aFilter.Contact() > KLogNullContactId)
			TEST2(event->Contact(), aFilter.Contact());

		if (aFilter.Number().Length() > 0)
			TEST(event->Number() == aFilter.Number());
		}

	TEST2(count, total + 1);

	CleanupStack::PopAndDestroy(3); // active, event, view;
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0853
@SYMTestCaseDesc	    Tests for filtering the log view
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the log view filter,which is set with different configuration data
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventViewFilter1L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0853 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	// Create a test event type
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc);
	type->SetLoggingEnabled(ETrue);

	// Register the event type
	active->StartL();
	aClient.AddEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(); // type

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	filter->SetEventType(KTestEventUid);
	TestViewFilterL(aClient, *filter);

	filter->SetEventType(KTestEventUid);
	filter->SetRemoteParty(KTestRemoteParty);
	TestViewFilterL(aClient, *filter);

	filter->SetEventType(KTestEventUid);
	filter->SetRemoteParty(KTestRemoteParty);
	filter->SetDirection(KTestDirection);
	TestViewFilterL(aClient, *filter);

	filter->SetEventType(KTestEventUid);
	filter->SetRemoteParty(KTestRemoteParty);
	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	TestViewFilterL(aClient, *filter);

	filter->SetEventType(KTestEventUid);
	filter->SetRemoteParty(KTestRemoteParty);
	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	TestViewFilterL(aClient, *filter);

	filter->SetEventType(KTestEventUid);
	filter->SetRemoteParty(KTestRemoteParty);
	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	filter->SetContact(KTestContact);
	TestViewFilterL(aClient, *filter);

	filter->SetEventType(KTestEventUid);
	filter->SetRemoteParty(KTestRemoteParty);
	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	filter->SetContact(KTestContact);
	filter->SetNumber(KTestNumber);
	TestViewFilterL(aClient, *filter);

	CleanupStack::PopAndDestroy(); // filter

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	filter->SetRemoteParty(KTestRemoteParty);
	TestViewFilterL(aClient, *filter);

	filter->SetRemoteParty(KTestRemoteParty);
	filter->SetDirection(KTestDirection);
	TestViewFilterL(aClient, *filter);

	filter->SetRemoteParty(KTestRemoteParty);
	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	TestViewFilterL(aClient, *filter);

	filter->SetRemoteParty(KTestRemoteParty);
	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	TestViewFilterL(aClient, *filter);

	filter->SetRemoteParty(KTestRemoteParty);
	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	filter->SetContact(KTestContact);
	TestViewFilterL(aClient, *filter);

	filter->SetRemoteParty(KTestRemoteParty);
	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	filter->SetContact(KTestContact);
	filter->SetNumber(KTestNumber);
	TestViewFilterL(aClient, *filter);

	CleanupStack::PopAndDestroy(); // filter

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	filter->SetDirection(KTestDirection);
	TestViewFilterL(aClient, *filter);

	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	TestViewFilterL(aClient, *filter);

	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	TestViewFilterL(aClient, *filter);

	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	filter->SetContact(KTestContact);
	TestViewFilterL(aClient, *filter);

	filter->SetDirection(KTestDirection);
	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	filter->SetContact(KTestContact);
	filter->SetNumber(KTestNumber);
	TestViewFilterL(aClient, *filter);

	CleanupStack::PopAndDestroy(); // filter

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	filter->SetDurationType(KTestDurationType);
	TestViewFilterL(aClient, *filter);

	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	TestViewFilterL(aClient, *filter);

	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	filter->SetContact(KTestContact);
	TestViewFilterL(aClient, *filter);

	filter->SetDurationType(KTestDurationType);
	filter->SetStatus(KTestStatus);
	filter->SetContact(KTestContact);
	filter->SetNumber(KTestNumber);
	TestViewFilterL(aClient, *filter);

	CleanupStack::PopAndDestroy(); // filter

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	filter->SetStatus(KTestStatus);
	TestViewFilterL(aClient, *filter);

	filter->SetStatus(KTestStatus);
	filter->SetContact(KTestContact);
	TestViewFilterL(aClient, *filter);

	filter->SetStatus(KTestStatus);
	filter->SetContact(KTestContact);
	filter->SetNumber(KTestNumber);
	TestViewFilterL(aClient, *filter);

	CleanupStack::PopAndDestroy(); // filter

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	filter->SetContact(KTestContact);
	TestViewFilterL(aClient, *filter);

	filter->SetContact(KTestContact);
	filter->SetNumber(KTestNumber);
	TestViewFilterL(aClient, *filter);

	CleanupStack::PopAndDestroy(); // filter

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	filter->SetNumber(KTestNumber);
	TestViewFilterL(aClient, *filter);

	CleanupStack::PopAndDestroy(2); // filter, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0854
@SYMTestCaseDesc	    Tests for filtering list event types
@SYMTestPriority 	    High
@SYMTestActions  	    Get the count of all event types,call,data,fax events.
                        Add some event types to the filter,test for the new count
						Check for non existing event type.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventViewFilter2L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0854 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	// Get total count of all events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TInt total = view->CountL();

	filter->SetEventType(KLogCallEventTypeUid);

	// Get number of call events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TInt calls = view->CountL();

	filter->SetEventType(KLogDataEventTypeUid);

	// Get number of data events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TInt data = view->CountL();

	filter->SetEventType(KLogFaxEventTypeUid);

	// Get number of fax events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TInt fax = view->CountL();

	filter->SetEventType(KLogLbsSelfLocateEventTypeUid);

	// Get number of Lbs self locate events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TInt lbsSelf = view->CountL();

	filter->SetEventType(KLogLbsExternalLocateEventTypeUid);

	// Get number of Lbs self locate events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TInt lbsExternal = view->CountL();

	filter->SetEventType(KLogLbsTransmitLocationEventTypeUid);

	// Get number of Lbs transmit location events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TInt lbsXmitLoc = view->CountL();

	filter->SetEventType(KLogLbsNetworkLocateEventTypeUid);

	// Get number of Lbs network locate events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TInt lbsNetwork = view->CountL();

	filter->SetEventType(KLogLbsAssistanceDataEventTypeUid);

	// Get number of Lbs assistance data events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TInt lbsAssistance = view->CountL();

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetEventType(KLogCallEventTypeUid);

	// Add two call events
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetEventType(KLogDataEventTypeUid);

	// Add two data events
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Filter all event types
	filter->SetEventType(KNullUid);

	// Get view of all events
	active->StartL();
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check the count is correct
	TEST2(view->CountL(), total + 4);

	filter->SetEventType(KLogCallEventTypeUid);

	// Get view of call events
	active->StartL();
	res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST2(view->CountL(), calls + 2);

	filter->SetEventType(KLogDataEventTypeUid);

	// Get view of data events
	active->StartL();
	res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST2(view->CountL(), data + 2);

	filter->SetEventType(KLogFaxEventTypeUid);

	// Get view of fax events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TEST2(view->CountL(), fax);

	filter->SetEventType(KLogLbsSelfLocateEventTypeUid);

	// Get number of Lbs self locate events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TEST2(view->CountL(), lbsSelf);

	filter->SetEventType(KLogLbsExternalLocateEventTypeUid);

	// Get number of Lbs self locate events	
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TEST2(view->CountL(), lbsExternal);

	filter->SetEventType(KLogLbsTransmitLocationEventTypeUid);

	// Get number of Lbs transmit location events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TEST2(view->CountL(), lbsXmitLoc);

	filter->SetEventType(KLogLbsNetworkLocateEventTypeUid);

	// Get number of Lbs network locate events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TEST2(view->CountL(), lbsNetwork);

	filter->SetEventType(KLogLbsAssistanceDataEventTypeUid);

	// Get number of Lbs assistance data events
	if (view->SetFilterL(*filter, active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TEST2(view->CountL(), lbsAssistance);

	// Test a non existing event type
	filter->SetEventType(TUid::Uid(0x12345678));
	res = view->SetFilterL(*filter, active->iStatus);
	TEST(!res);
	TEST2(view->CountL(), 0);

	CleanupStack::PopAndDestroy(4); // view, filter, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0855
@SYMTestCaseDesc	    Tests for CLogViewRecent::SetRecentListL() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the count of events,outgoing calls.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentView1L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0855 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	// Incoming
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	TInt count;
	for(count = 0; count < KTestEventNum; count++)
		{
		event->SetContact(count);

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	TEST2(view->CountL(), 0);
	TBool res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);

	// Move forward
	count = KTestEventNum;
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST2(view->CountL(), KTestEventNum);
		TEST2(view->RecentList(), KLogRecentIncomingCalls);
		count--;
		}
	while(view->NextL(active->iStatus));
	TEST2(count, 0);
	TEST2(view->CountL(), KTestEventNum);

	// Move back
	while(view->PreviousL(active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST2(view->CountL(), KTestEventNum);
		TEST2(view->RecentList(), KLogRecentIncomingCalls);
		count++;
		}
	TEST2(count, KTestEventNum - 1);

	active->StartL();
	res = view->FirstL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	res = view->LastL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST2(view->CountL(), KTestEventNum);

	// Outgoing
	res = view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus);
	TEST(!res);
	count = view->CountL();
	TEST2(count, 0);
	CleanupStack::PopAndDestroy(view);
	//
	view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);
	res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), KTestEventNum);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	aClient.GetString(buf, R_LOG_DIR_OUT);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	for(count = 0; count < KTestEventNum; count++)
		{
		event->SetContact(count);

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	count = view->CountL();
	TEST2(count, KTestEventNum);
	res = view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus);
	TEST(res);

	// Move forward
	count = KTestEventNum;
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST2(view->CountL(), KTestEventNum);
		TEST2(view->RecentList(), KLogRecentOutgoingCalls);
		count--;
		}
	while(view->NextL(active->iStatus));
	TEST2(count, 0);
	TEST2(view->CountL(), KTestEventNum);

	// Move back
	while(view->PreviousL(active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST2(view->CountL(), KTestEventNum);
		TEST2(view->RecentList(), KLogRecentOutgoingCalls);
		count++;
		}
	TEST2(count, KTestEventNum - 1);

	active->StartL();
	res = view->FirstL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	res = view->LastL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Missed
	aClient.GetString(buf, R_LOG_DIR_MISSED);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	TEST2(view->CountL(), KTestEventNum);
	res = view->SetRecentListL(KLogRecentMissedCalls, active->iStatus);
	TEST(!res);
	TEST2(view->CountL(), 0);

	for(count = 0; count < KTestEventNum; count++)
		{
		event->SetContact(count);

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Test INC123066 - LogView not updated if log count is zero at
		// time of SetFilterL.
		TEST2(view->CountL(), count+1);
		}


	// Move forward
	count = KTestEventNum;
	while(view->NextL(active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST2(view->CountL(), KTestEventNum);
		TEST2(view->RecentList(), KLogRecentMissedCalls);
		count--;
		}
	TEST2(count, 0);

	// Move back
	while(view->PreviousL(active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST2(view->CountL(), KTestEventNum);
		TEST2(view->RecentList(), KLogRecentMissedCalls);
		count++;
		}
	TEST2(count, KTestEventNum - 1);

	active->StartL();
	res = view->FirstL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	res = view->LastL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// All
	TEST2(view->CountL(), KTestEventNum);
	res = view->SetRecentListL(KLogNullRecentList, active->iStatus);
	TEST(res);

	// Move forward
	count = KTestEventNum * 3;
	TEST2(view->CountL(), count);
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST2(view->CountL(), KTestEventNum * 3);
		count--;
		}
	while(view->NextL(active->iStatus));
	TEST2(count, 0);

	// Move back
	while(view->PreviousL(active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST2(view->CountL(), KTestEventNum * 3);
		count++;
		}
	TEST2(count, KTestEventNum * 3 - 1);

	active->StartL();
	res = view->FirstL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	res = view->LastL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(3); // view, active, event
	theLog.Write(_L8("Test 3.1 OK\n"));
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0856
@SYMTestCaseDesc	    Tests for CLogViewRecent::SetRecentListL(),DuplicatesL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the count of events in the log
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentView2L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0856 "));
	TInt count;

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	TEST2(view->CountL(), 0);
	TBool res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum);

	aClient.ClearLog(KLogRecentIncomingCalls, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	count = view->CountL();
	TEST2(count, 0);

	CleanupStack::PopAndDestroy(view);
	view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);


	// Incoming
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	// All fields are null, they should all be duplicates
	for(count = 0; count < KTestEventNum; count++)
		{
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	TEST2(view->CountL(), 0);

	// Move forward
	res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	CLogViewDuplicate* dView = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(dView);

	res = view->DuplicatesL(*dView, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(dView->CountL(), KTestEventNum - 1);

	CleanupStack::PopAndDestroy(4); // view, active, event, dView
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0857
@SYMTestCaseDesc	    Tests for CLogViewRecent::RemoveL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Add the recently view events to the view.Remove the items and check for the count.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentRemove1L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0857 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	TBool res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TInt count = view->CountL();

	while(view->RemoveL(active->iStatus))
		{
		TEST2(view->CountL(), count - 1);
		count = view->CountL();

		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	TEST2(view->CountL(), 0);

	res = view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), KTestEventNum);

	aClient.ClearLog(KLogRecentOutgoingCalls, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 0);

	res = view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus);
	TEST(!res);

	res = view->SetRecentListL(KLogNullRecentList, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), KTestEventNum);

	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	res = view->SetRecentListL(KLogNullRecentList, active->iStatus);
	TEST(!res);
	TEST2(view->CountL(), 0);

	CleanupStack::PopAndDestroy(2); // view, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0858
@SYMTestCaseDesc	    Tests for CLogViewRecent::RemoveL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Add the recently view events to the two logviewrecents.Remove the items and check for the count of the first logview.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentRemove2L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0858 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view1 = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view1);

	// Incoming
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	TInt count;
	for(count = 0; count < KTestEventNum; count++)
		{
		event->SetContact(count);

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// Create another invalid view and remove an event using it
	CLogViewRecent* view2 = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view2);

	TEST2(view1->CountL(), 0);
	TBool res = view1->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	TEST2(view1->CountL(), KTestEventNum);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		view2->RemoveL(view1->Event().Id());
		}
	while(view1->NextL(active->iStatus));

	aClient.ClearLog(KLogRecentIncomingCalls, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view1->CountL(), 0);

	CleanupStack::PopAndDestroy(4); // view2, view1, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0859
@SYMTestCaseDesc	    Tests for CLogViewDuplicate::SetRecentListL(),DuplicatesL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the count of events and the event ID
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicateViewL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0859 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogViewDuplicate* dView = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(dView);

	TEST2(dView->CountL(), 0);

	TBool res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(!res);
	TEST2(view->CountL(), 0);

	res = view->DuplicatesL(*dView, active->iStatus);
	TEST(!res);
	TEST2(dView->CountL(), 0);

	// Incoming
	TBuf<KLogMaxDirectionLength> incoming;
	aClient.GetString(incoming, R_LOG_DIR_IN);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(incoming);
	event->SetRemoteParty(KTestRemoteParty);
	event->SetContact(KTestContact);
	event->SetNumber(KTestNumber);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST2(view->CountL(), 1);
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), event->Id());

	TEST2(dView->CountL(), 0);

	event->SetRemoteParty(KTestRemoteParty);
	event->SetContact(KTestContact);
	event->SetNumber(KNullDesC);

	//Add event with Number field set to NULL
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST2(view->CountL(), 2);
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), event->Id());

	res = view->DuplicatesL(*dView, active->iStatus);
	TEST(!res);

	TEST2(dView->CountL(), 0);

	event->SetRemoteParty(KTestRemoteParty);
	event->SetContact(KLogNullContactId);
	event->SetNumber(KNullDesC);

	//Add event with Number field set to NULL & Contact set to -1
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST2(view->CountL(), 3);
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), event->Id());

	res = view->DuplicatesL(*dView, active->iStatus);
	TEST(!res);

	TEST2(dView->CountL(), 0);

	event->SetRemoteParty(KNullDesC);
	event->SetContact(KLogNullContactId);
	event->SetNumber(KNullDesC);

	//Add event with Number, Remote Party field set to NULL & Contact set to -1
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST2(view->CountL(), 4);

	res = view->DuplicatesL(*dView, active->iStatus);
	TEST(!res);

	TEST2(dView->CountL(), 0);

	TLogId lastId = event->Id();
	
	//Add event with Number, Remote Party field set to NULL & Contact set to -1
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST2(view->CountL(), 4);

	res = view->DuplicatesL(*dView, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST2(dView->CountL(), 1);
	TEST2(dView->Event().Id(), lastId);
	
	CleanupStack::PopAndDestroy(4); // event, dView, view, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0860
@SYMTestCaseDesc	    Tests for CLogViewDuplicate::RemoveL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for removing the events and test for the count
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicateRemoveL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0860 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogViewDuplicate* dView = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(dView);

	TBool res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//The incoming call list should have count as 4. These calls were added in 
	//...the previous function named TestDuplicateViewL() 
	TEST2(view->CountL(), 4);

	res = view->DuplicatesL(*dView, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TLogId id = dView->Event().Id();
	TEST2(dView->CountL(), 1);
	
	res = dView->RemoveL(active->iStatus);
	TEST(!res);

	TEST2(dView->CountL(), 0);

	res = dView->RemoveL(active->iStatus);
	TEST(!res);
	TEST2(dView->CountL(), 0);

	TEST2(view->CountL(), 4);

	CleanupStack::PopAndDestroy(3); // dView, view, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0861
@SYMTestCaseDesc	    Tests for purging on log engine
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for changing the log engine configuration,add event and test for retrieving them back.
						Set up a purge which clears the log and reset the log configuration 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestPurgeOnSetup1L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0861 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	User::After(0x100000);

	TTime now;
	now.UniversalTime();

	// Clear all the events
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	TLogConfig config;

	// Get log configuration
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Set the maximum log age
	TInt oldAge = config.iMaxEventAge;
	config.iMaxEventAge = 10; // 10 seconds!

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that the event can be retrieved
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Wait for 15 seconds (just to be safe)
	User::After(15000000);

	// Check that the event can be retrieved
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that the event can still be retrieved
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that the event can still be retrieved
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	// Setup a view which should purge the event - hence no events in view!
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(!res);

	// Reset the config
	config.iMaxEventAge = oldAge;

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(4); // filter, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0862
@SYMTestCaseDesc	    Tests for purging on log engine
@SYMTestPriority 	    High
@SYMTestActions  	    Clear the events from the log,set the log engine age to maximum.Change the configuration,and test adding and retrieving the events.
                        Set up a purge which clears the log and reset the log configuration 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestPurgeOnSetup2L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0862 "));
	CTestActive* active1 = new(ELeave)CTestActive();
	CleanupStack::PushL(active1);

	CTestActive* active2 = new(ELeave)CTestActive();
	CleanupStack::PushL(active2);

	User::After(0x100000);

	TTime now;
	now.UniversalTime();

	// Clear all the events
	active1->StartL();
	aClient.ClearLog(now, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	TLogConfig config;

	// Get log configuration
	active1->StartL();
	aClient.GetConfig(config, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	// Set the maximum log age
	TInt oldAge = config.iMaxEventAge;
	config.iMaxEventAge = 10; // 10 seconds!

	// Change the log engine config
	active1->StartL();
	aClient.ChangeConfig(config, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	active1->StartL();
	aClient.AddEvent(*event, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	// Check that the event can be retrieved
	active1->StartL();
	aClient.GetEvent(*event, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	// Wait for 15 seconds (just to be safe)
	User::After(15000000);

	// Check that the event can be retrieved
	active1->StartL();
	aClient.GetEvent(*event, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	// Check that the event can still be retrieved
	active1->StartL();
	aClient.GetEvent(*event, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	// Check that the event can still be retrieved
	active1->StartL();
	aClient.GetEvent(*event, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	CLogViewEvent* view1 = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view1);

	CLogViewEvent* view2 = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view2);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	// Setup a view which should purge the event - hence no events in view!
	TBool res = view1->SetFilterL(*filter, active1->iStatus);
	TEST(!res);
	res = view2->SetFilterL(*filter, active2->iStatus);
	TEST(!res);

	// Reset the config
	config.iMaxEventAge = oldAge;

	// Change the log engine config
	active1->StartL();
	aClient.ChangeConfig(config, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(6); // filter, view1, view2, active1, active2, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0863
@SYMTestCaseDesc	    Tests for purge on CLogViewRecent
@SYMTestPriority 	    High
@SYMTestActions  	    Set the log engine maximum log size and recent log size.Change the configuration and add events to the log
                        Tests the count to be less than or equal to recent logsize.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewPurgeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0863 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig config;

	aClient.GetConfig(config, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(config.iMaxLogSize, 1000);

	config.iMaxLogSize = 2000;
	config.iMaxRecentLogSize = 20;

	aClient.ChangeConfig(config, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	// Incoming
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	for(TInt count = 0; count < config.iMaxRecentLogSize * 2; count++)
		{
		event->SetContact(count);

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TBool res = view->SetRecentListL(KLogNullRecentList, active->iStatus);
		TEST(res);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST(view->CountL() <= config.iMaxRecentLogSize);
		}

	CleanupStack::PopAndDestroy(3); // active, event, view
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0864
@SYMTestCaseDesc	    Tests for the functionality of CLogViewDuplicate class
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for clearing the duplicate events
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestClearDuplicatesL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0864 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogViewDuplicate* dView = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(dView);

	TBool res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(!res);

	TBuf<KLogMaxDirectionLength> incoming;
	aClient.GetString(incoming, R_LOG_DIR_IN);

	TBuf<KLogMaxDirectionLength> outgoing;
	aClient.GetString(outgoing, R_LOG_DIR_OUT);

	TBuf<KLogMaxDirectionLength> missed;
	aClient.GetString(missed, R_LOG_DIR_MISSED);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetEventType(KLogCallEventTypeUid);

	// Add 5 recent events to each list - with 5 duplicates each

	// Incoming
	event->SetDirection(incoming);
	TInt recent;
	for(recent = 1; recent <= 5; recent++)
		{
		event->SetContact(recent);
		for(TInt duplicate = 1; duplicate <= 6; duplicate++)
			{
			active->StartL();
			aClient.AddEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}
		}

	// Outgoing
	event->SetDirection(outgoing);
	for(recent = 1; recent <= 5; recent++)
		{
		event->SetContact(recent);
		for(TInt duplicate = 1; duplicate <= 6; duplicate++)
			{
			active->StartL();
			aClient.AddEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}
		}

	// Missed
	event->SetDirection(missed);
	for(recent = 1; recent <= 5; recent++)
		{
		event->SetContact(recent);
		for(TInt duplicate = 1; duplicate <= 6; duplicate++)
			{
			active->StartL();
			aClient.AddEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}
		}

	// Check outgoing
	res = view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(res);
		TEST2(dView->CountL(), 5);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	while(view->NextL(active->iStatus));

	// Check missed
	res = view->SetRecentListL(KLogRecentMissedCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(res);
		TEST2(dView->CountL(), 5);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	while(view->NextL(active->iStatus));

	// Check incoming
	res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(res);
		TEST2(dView->CountL(), 5);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	while(view->NextL(active->iStatus));

	// Clear duplicates for incoming
	view->ClearDuplicatesL();
	TEST2(view->CountL(), 5);
	
	// Recent list should be unchanged
	res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// No duplicates should exist for this view now
		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(!res);
		TEST2(dView->CountL(), 0);
		}
	while(view->NextL(active->iStatus));

	// Check outgoing is unchanged
	res = view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(res);
		TEST2(dView->CountL(), 5);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	while(view->NextL(active->iStatus));

	// Check missed is unchanged
	res = view->SetRecentListL(KLogRecentMissedCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(res);
		TEST2(dView->CountL(), 5);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	while(view->NextL(active->iStatus));

	// Clear duplicates for missed
	view->ClearDuplicatesL();
	TEST2(view->CountL(), 5);
	
	// Recent list should be unchanged
	res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// No duplicates should exist for this view now
		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(!res);
		TEST2(dView->CountL(), 0);
		}
	while(view->NextL(active->iStatus));

	// Missed recent list should be unchanged
	res = view->SetRecentListL(KLogRecentMissedCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// No duplicates should exist for this view now
		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(!res);
		TEST2(dView->CountL(), 0);
		}
	while(view->NextL(active->iStatus));

	// Check outgoing is unchanged
	res = view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(res);
		TEST2(dView->CountL(), 5);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}
	while(view->NextL(active->iStatus));

	// Clear duplicates for outgoing
	view->ClearDuplicatesL();
	TEST2(view->CountL(), 5);
	
	// Recent list should be unchanged
	res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// No duplicates should exist for this view now
		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(!res);
		TEST2(dView->CountL(), 0);
		}
	while(view->NextL(active->iStatus));

	// Missed recent list should be unchanged
	res = view->SetRecentListL(KLogRecentMissedCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// No duplicates should exist for this view now
		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(!res);
		TEST2(dView->CountL(), 0);
		}
	while(view->NextL(active->iStatus));

	// Check outgoing is unchanged
	res = view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus);
	TEST(res);
	TEST2(view->CountL(), 5);

	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// No duplicates should exist for this view now
		res = view->DuplicatesL(*dView, active->iStatus);
		TEST(!res);
		TEST2(dView->CountL(), 0);
		}
	while(view->NextL(active->iStatus));

	CleanupStack::PopAndDestroy(4); // event, dView, view, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0865
@SYMTestCaseDesc	    Test for the functionality of CLogViewEvent class
@SYMTestPriority 	    High
@SYMTestActions  	    Set an event with a number,add the event to the client.Tests for matching a number from the view
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestPhoneNumberMatchingL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0865 "));
	TestUtils::DeleteDatabaseL();

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> missed;
	aClient.GetString(missed, R_LOG_DIR_MISSED);
	event->SetDirection(missed);

	TLogString number;
	TInt i;
	for(i = 0; i < 10; i++)
		{
		TBuf<1> digit;
		digit.Num(i);
		number.Insert(0, digit);
		event->SetNumber(number);

		aClient.AddEvent(*event, active->iStatus);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 10);

	number.Zero();
	for(i = 0; i < 10; i++)
		{
		TBuf<1> digit;
		digit.Num(i);
		number.Insert(0, digit);
		filter->SetNumber(number);

		// This is testing phone number matching
		res = view->SetFilterL(*filter, active->iStatus);
		TEST(res);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Phone numbers with KTestLogNumberCharsToMatch characters or above can match with each other
		TEST2(view->CountL(), (number.Length() < KTestLogNumberCharsToMatch) ? 1 : 11-KTestLogNumberCharsToMatch);		
		}

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	res = recent->SetRecentListL(KLogRecentMissedCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), KTestLogNumberCharsToMatch);

	res = recent->DuplicatesL(*duplicate, active->iStatus);
	TEST(res);
	TEST2(duplicate->CountL(), 10-KTestLogNumberCharsToMatch);

	// Check the first recent event has duplicates
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TPtrC number1(recent->Event().Number().Right(KTestLogNumberCharsToMatch));
		TPtrC number2(duplicate->Event().Number().Right(KTestLogNumberCharsToMatch));
		TEST(number1 == number2);
		}
	while(duplicate->NextL(active->iStatus));

	// Check none of the others have duplicates
	while(recent->NextL(active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		res = recent->DuplicatesL(*duplicate, active->iStatus);
		TEST(!res);
		}

	CleanupStack::PopAndDestroy(6); // duplicate, recent, view, filter, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-3472
@SYMTestCaseDesc	    Tests duplicate number matching correctly compares and matches the last 9 digits
@SYMTestPriority 	    High
@SYMTestActions  	    Create and add several numbers to client, check that duplicates are correctly removed
@SYMTestExpectedResults Number with the same last 9 digits should get matched as duplicates
@SYMDEF                 INC105010
*/	
LOCAL_C void INC105010L(CLogClient& aClient)
{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-3472 "));
	TestUtils::DeleteDatabaseL();

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> missed;
	aClient.GetString(missed, R_LOG_DIR_MISSED);
	event->SetDirection(missed);

	_LIT(KTestNumber1, "0401234567"); // unique
	_LIT(KTestNumber2, "0421234567"); // unique
	_LIT(KTestNumber3, "0521234567"); // unique
	_LIT(KTestNumber4, "9521234567"); // duplicate
	_LIT(KTestNumber5, "9521234567"); // duplicate
	
	TLogString number;

 	// add events for the above numbers
	event->SetNumber(KTestNumber1);
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	event->SetNumber(KTestNumber2);
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	event->SetNumber(KTestNumber3);
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	event->SetNumber(KTestNumber4);
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	event->SetNumber(KTestNumber5);
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	// check 5 entries in log
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);	
	TEST2(view->CountL(), 5);

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// check only 3 entries in recent calls view
	res = recent->SetRecentListL(KLogRecentMissedCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TInt rrr = recent->CountL();
	TEST2(recent->CountL(), 3);

	// and that there are 2 duplicates
	res = recent->DuplicatesL(*duplicate, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);	
	
	TInt ddd = duplicate->CountL();
	TEST2(duplicate->CountL(), 2);

	CleanupStack::PopAndDestroy(6); 
}
	
	
	
/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0866
@SYMTestCaseDesc	    Tests for CLogViewEvent::SetFlagsL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Add events to the log and set the flags.Check if all the flags are set.
                        Clear all the flags and set half view of the flags.Check if only all view of the flags are set.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentFlagsL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0866 "));
	TestUtils::DeleteDatabaseL();

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	// Incoming
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	
	TInt count;
	for(count = 0; count < KTestEventNum; count++)
		{
		event->SetContact(count);

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	TEST2(view->CountL(), 0);
	TBool res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), KTestEventNum);

	// Set all flags in view
	const TLogFlags KAllFlagsSet = 15; // (BIN) 1,1,1,1 
	view->SetFlagsL(KAllFlagsSet);

	// Check we can move to the first record
	res = view->FirstL(active->iStatus);
	TEST(res);

	// Check all flags are now set
	count = KTestEventNum;
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		//
		TEST2(view->CountL(), KTestEventNum);
		TEST2(view->RecentList(), KLogRecentIncomingCalls);
		//
		const TLogFlags flags = view->Event().Flags();
		TEST2(flags, KAllFlagsSet);
		count--;
		}
	while(view->NextL(active->iStatus));

	TEST2(count, 0);
	TEST2(view->CountL(), KTestEventNum);

	// Clear all flags in view
	const TLogFlags KAllFlagsCleared = 0; // (BIN) 0,0,0,0 
	view->SetFlagsL(KAllFlagsCleared);
	TEST2(view->CountL(), KTestEventNum);

	// Check we can move to the first record
	res = view->FirstL(active->iStatus);
	TEST(res);

	// Check all flags are now set
	count = KTestEventNum;
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		//
		TEST2(view->CountL(), KTestEventNum);
		TEST2(view->RecentList(), KLogRecentIncomingCalls);
		//
		const TLogFlags flags = view->Event().Flags();
		TEST2(flags, KAllFlagsCleared);
		count--;
		}
	while(view->NextL(active->iStatus));
	
	TEST2(count, 0);
	TEST2(view->CountL(), KTestEventNum);

	// Set lower half flags in view
	const TLogFlags KLowerHalfFlagsSet = 3; // (BIN) 1,1,0,0 
	view->SetFlagsL(KLowerHalfFlagsSet);

	// Check we can move to the first record
	res = view->FirstL(active->iStatus);
	TEST(res);

	// Check all flags are now set
	count = KTestEventNum;
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		//
		TEST2(view->CountL(), KTestEventNum);
		TEST2(view->RecentList(), KLogRecentIncomingCalls);
		//
		const TLogFlags flags = view->Event().Flags();
		TEST2(flags, KLowerHalfFlagsSet);
		count--;
		}
	while(view->NextL(active->iStatus));

	TEST2(count, 0);
	TEST2(view->CountL(), KTestEventNum);

	CleanupStack::PopAndDestroy(3, event); // view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0867
@SYMTestCaseDesc	    Tests for CLogViewEvent::SetFlagsL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Add events to the log and set the flags.Check if flags for only the events in view are set.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewFlagsL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0867 "));
	TestUtils::DeleteDatabaseL();

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	// Incoming
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	
	event->SetNumber(_L("123"));
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId testId = event->Id();
	event->SetNumber(KNullDesC);
	TheTest.Printf(_L("  Event flags: 0x%X\r\n"), event->Flags());

	TInt count;
	for(count = 0; count < KTestEventNum; count++)
		{
		event->SetContact(count + 1);

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	TEST2(view->CountL(), 0);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetNullFields(ELogNumberField);

	TEST2(view->CountL(), 0);
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), KTestEventNum);

	// Set all flags in view
	const TLogFlags KAllFlagsSet = 15; // (BIN) 1,1,1,1 
	view->SetFlagsL(KAllFlagsSet);

	// Check flags only set for events in the view
	active->StartL();
	event->SetId(testId);
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	if( TheMatchingIsEnabled)
		{//This check will fail if the first AddEvent() call in this function didn't perform contact matching
		TEST2(event->Flags(), KLogEventContactSearched);
		}
	else
		{
		TEST2(event->Flags(), 0);
		}

	// Check we can move to the first record
	res = view->FirstL(active->iStatus);
	TEST(res);

	// Check all flags are now set
	count = KTestEventNum;
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		//
		TEST2(view->CountL(), KTestEventNum);
		//
		const TLogFlags flags = view->Event().Flags();
		TEST2(flags, KAllFlagsSet);
		count--;
		}
	while(view->NextL(active->iStatus));

	TEST2(count, 0);
	TEST2(view->CountL(), KTestEventNum);

	// Clear all flags in view
	const TLogFlags KAllFlagsCleared = 0; // (BIN) 0,0,0,0 
	view->SetFlagsL(KAllFlagsCleared);
	TEST2(view->CountL(), KTestEventNum);

	// Check flags only set for events in the view
	active->StartL();
	event->SetId(testId);
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	if( TheMatchingIsEnabled)
		{
		TEST2(event->Flags(), KLogEventContactSearched);
		}
	else
		{
		TEST2(event->Flags(), 0);
		}

	// Check we can move to the first record
	res = view->FirstL(active->iStatus);
	TEST(res);

	// Check all flags are now set
	count = KTestEventNum;
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		//
		TEST2(view->CountL(), KTestEventNum);
		//
		const TLogFlags flags = view->Event().Flags();
		TEST2(flags, KAllFlagsCleared);
		count--;
		}
	while(view->NextL(active->iStatus));
	
	TEST2(count, 0);
	TEST2(view->CountL(), KTestEventNum);

	// Set lower half flags in view
	const TLogFlags KLowerHalfFlagsSet = 3; // (BIN) 1,1,0,0 
	view->SetFlagsL(KLowerHalfFlagsSet);

	// Check flags only set for events in the view
	active->StartL();
	event->SetId(testId);
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	if( TheMatchingIsEnabled)
		{
		TEST2(event->Flags(), KLogEventContactSearched);
		}
	else
		{
		TEST2(event->Flags(), 0);
		}

	// Check we can move to the first record
	res = view->FirstL(active->iStatus);
	TEST(res);

	// Check all flags are now set
	count = KTestEventNum;
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		//
		TEST2(view->CountL(), KTestEventNum);
		//
		const TLogFlags flags = view->Event().Flags();
		TEST2(flags, KLowerHalfFlagsSet);
		count--;
		}
	while(view->NextL(active->iStatus));

	TEST2(count, 0);
	TEST2(view->CountL(), KTestEventNum);

	CleanupStack::PopAndDestroy(4, event); // filter, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0868
@SYMTestCaseDesc	    Tests for CLogViewChangeObserver::HaveChanges(),Changes() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Check for observing changes while adding and deleting events
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewChangeEvents1L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0868 "));
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

	TInt i;
	TInt count;
	for(count = 0; count < KTestEventNum; count++)
		{
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		User::After(1 * 1000000);
		}
	//
	TEST2(view->CountL(), 0);
	active->StartL();
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), KTestEventNum);
	//
	count = KTestEventNum;
	res = view->FirstL(active->iStatus);
	TEST(res);
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		//
		TEST2(view->CountL(), KTestEventNum);
		const TLogId eventId = view->Event().Id();
		TLogId id = --count;
		TEST2(eventId, id);
		}
	while(view->NextL(active->iStatus));

	// Transients
	TInt changeCount;
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;
	CLogChangeDefinition* changes;
	
	// Add an event and check for changes
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	changes = changeObs->WaitForChangesLC();
	if	(active->IsActive())
		CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum+1);

	changeCount = changes->Count();
	for(i=0; i<changeCount; i++)
		{
		type = changes->At(i, logId, viewIndex);
		TheTest.Printf(_L("Change Type: %d, logId: %d, viewIndex: %d\n"), type, logId, viewIndex);
		}
	
	// Check the change was as expected
	TEST2(changes->Count(), 1);
	type = changes->At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(viewIndex, 0); // first (newest) event in the view
	{
	const TLogId expectedLogId = ((TLogId) KTestEventNum);
	TEST2(logId, expectedLogId);
	}
	CleanupStack::PopAndDestroy(changes);

	// Check view navigation
	i=0;
	res = view->FirstL(active->iStatus);
	TEST(res);
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Delete some events
	TEST(KTestEventNum >= 10);
	changeObs->StartCollectingChanges();
	for(i=1; i<KTestEventNum; i+=2)
		{
		active->StartL();
		aClient.DeleteEvent(TLogId(i), active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 5); // 1,3,5,7,9
	for(i=0; i<changeCount; i++)
		{
		TEST2(changeObs->Changes().At(i, logId, viewIndex), ELogChangeTypeEventDeleted);
		TEST2(logId, TLogId(2*i + 1));
		TEST2(viewIndex, 10 - ((2*i) + 1));
		}
	
	// Add a new event that shouldn't appear in the view
	changeObs->StartCollectingChanges();
	event->SetContact(TLogContactItemId(0));
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 1 - 5);
	TEST2(event->Id(), KTestEventNum + 1);

	// Check changes
	TEST(!changeObs->HaveChanges());

	// Check view navigation
	i=0;
	res = view->FirstL(active->iStatus);
	TEST(res);
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Delete event which isn't in view
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.DeleteEvent(event->Id(), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 1 - 5);

	// Check changes
	TEST(!changeObs->HaveChanges());

	// Check view navigation
	i=0;
	res = view->FirstL(active->iStatus);
	TEST(res);
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Add a new event again that shouldn't appear in the view
	changeObs->StartCollectingChanges();
	event->SetContact(TLogContactItemId(0));
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 1 - 5);
	TEST2(event->Id(),  KTestEventNum + 2);

	// Check changes
	TEST(!changeObs->HaveChanges());

	// Check view navigation
	i=0;
	res = view->FirstL(active->iStatus);
	TEST(res);
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Change event so that it appears in the view
	changeObs->StartCollectingChanges();
	event->SetContact(KTestContact);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 2 - 5);
	TEST2(event->Id(),  KTestEventNum + 2); // Shouldn't change

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(logId, KTestEventNum + 2);
	TEST2(viewIndex, 0);

	// Check view navigation
	i=0;
	res = view->FirstL(active->iStatus);
	TEST(res);
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Simply change an event in the view
	changeObs->StartCollectingChanges();
	event->SetDescription(_L("Test"));
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 2 - 5); // Shouldn't change
	TEST2(event->Id(),  KTestEventNum + 2); // Shouldn't change

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventChanged);
	TEST2(logId, KTestEventNum + 2);
	TEST2(viewIndex, 0);

	// Change an event in the view so that it is deleted
	changeObs->StartCollectingChanges();
	event->SetContact(0);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 2 - 5 - 1); // one less now

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, KTestEventNum + 2);
	TEST2(viewIndex, 0);

	// Navigate part way through the view so we have
	// a cursor position part way through...
	i=0;
	count = view->CountL();
	res = view->FirstL(active->iStatus);
	TEST(res);
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	TEST2(view->Event().Id(), TLogId(0));

	// Save id
	TLogId savedId = view->Event().Id();

	// Go one back
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Insert something
	changeObs->StartCollectingChanges();
	event->SetContact(KTestContact);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 2 - 5); // one more now
	TEST2(event->Id(),  KTestEventNum + 3);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(logId, KTestEventNum + 3);
	TEST2(viewIndex, 0);

	// Check we can still go forward to the last record
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), savedId);

	// Go one back
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Delete current record
	savedId = view->Event().Id();
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.DeleteEvent(savedId, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 2 - 5 - 1); // one less

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, savedId);
	TEST2(viewIndex, KTestEventNum + 2 - 5 - 2); // last but one event

	// Check we're now at the end of the view
	res = view->NextL(active->iStatus);
	TEST(!res);

	// Go to the first record
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Delete the first record
	savedId = view->Event().Id();
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.DeleteEvent(savedId, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum - 5);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, savedId);
	TEST2(viewIndex, 0); // first item

	// Check 'next' navigation can be performed correctly
	count = 0;
	view->NextL(active->iStatus);
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		++count;
		}
	while(view->NextL(active->iStatus));
	TEST2(count, KTestEventNum - 5 - 1);

	// Check last record
	savedId = view->Event().Id();
	res = view->LastL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(savedId, view->Event().Id());
	
	// Delete the last record
	savedId = view->Event().Id();
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.DeleteEvent(savedId, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum - 6);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, savedId);
	TEST2(viewIndex, count); // There's now one less item, and we deleted the last item of the previous view

	// Check we're still at the end of the view
	res = view->NextL(active->iStatus);
	TEST(!res);

	CleanupStack::PopAndDestroy(5, event); // view, changeObs, active, filter, event
	}

CBaBackupSessionWrapper* theBackup = NULL;

LOCAL_C TBool IsLogOpenL()
	{
	return TestUtils::IsDatabaseOpenL();
	}

LOCAL_C void TestLogOpenL()
	{
	TEST(IsLogOpenL());
	}

LOCAL_C void TestLogClosedL()
	{
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

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0869
@SYMTestCaseDesc	    Tests for CLogViewChangeObserver::HaveChanges(),CLogViewChangeObserver::Changes() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Check for observing changes while adding,deleting events.Check for observing changes after a backup.
                        Check for navigating the view.Test for KErrNone condition.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewChangeEvents1aL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0869 "));
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

	TInt i;
	TInt count;
	for(count = 0; count < KTestEventNum; count++)
		{
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		User::After(1 * 1000000);
		}
	//
	TEST2(view->CountL(), 0);
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), KTestEventNum);
	//
	count = KTestEventNum;
	TEST(view->FirstL(active->iStatus));
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		//
		TEST2(view->CountL(), KTestEventNum);
		const TLogId eventId = view->Event().Id();
		TEST2(eventId, (TLogId)--count);
		}
	while(view->NextL(active->iStatus));

	// Transients
	TInt changeCount;
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;
	CLogChangeDefinition* changes;
	
	// Add an event and check for changes
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	changes = changeObs->WaitForChangesLC();
	if	(active->IsActive())
		CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum+1);

	changeCount = changes->Count();
	for(i=0; i<changeCount; i++)
		{
		type = changes->At(i, logId, viewIndex);
		TheTest.Printf(_L("Change Type: %d, logId: %d, viewIndex: %d"), type, logId, viewIndex);
		}
	
	// Check the change was as expected
	TEST2(changes->Count(), 1);
	type = changes->At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(viewIndex, 0); // first (newest) event in the view
	{
	const TLogId expectedLogId = ((TLogId) KTestEventNum);
	TEST2(logId, expectedLogId);
	}
	CleanupStack::PopAndDestroy(changes);

	// Check view navigation
	i=0;
	TEST(view->FirstL(active->iStatus));
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	TEST(view->FirstL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	DelayL(1000000);
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Delete some events
	TEST(KTestEventNum >= 10);
	changeObs->StartCollectingChanges();
	for(i=1; i<KTestEventNum; i+=2)
		{
		active->StartL();
		aClient.DeleteEvent(TLogId(i), active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 5); // 1,3,5,7,9
	for(i=0; i<changeCount; i++)
		{
		TEST2(changeObs->Changes().At(i, logId, viewIndex), ELogChangeTypeEventDeleted);
		TEST2(logId, TLogId(2*i + 1));
		TEST2(viewIndex, 10 - ((2*i) + 1));
		}
	
	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Add a new event that shouldn't appear in the view
	changeObs->StartCollectingChanges();
	event->SetContact(TLogContactItemId(0));
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 1 - 5);
	TEST2(event->Id(),  KTestEventNum + 1);

	// Check changes
	TEST(!changeObs->HaveChanges());

	// Check view navigation
	i=0;
	TEST(view->FirstL(active->iStatus));
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	TEST(view->FirstL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Delete event which isn't in view
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.DeleteEvent(event->Id(), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 1 - 5);

	// Check changes
	TEST(!changeObs->HaveChanges());

	// Check view navigation
	i=0;
	TEST(view->FirstL(active->iStatus));
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	TEST(view->FirstL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Add a new event again that shouldn't appear in the view
	changeObs->StartCollectingChanges();
	event->SetContact(TLogContactItemId(0));
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 1 - 5);
	TEST2(event->Id(), KTestEventNum + 2);

	// Check changes
	TEST(!changeObs->HaveChanges());

	// Check view navigation
	i=0;
	TEST(view->FirstL(active->iStatus));
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	TEST(view->FirstL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Change event so that it appears in the view
	changeObs->StartCollectingChanges();
	event->SetContact(KTestContact);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 2 - 5);
	TEST2(event->Id(), KTestEventNum + 2); // Shouldn't change

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(logId, KTestEventNum + 2);
	TEST2(viewIndex, 0);

	// Check view navigation
	i=0;
	TEST(view->FirstL(active->iStatus));
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	TEST(view->FirstL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Simply change an event in the view
	changeObs->StartCollectingChanges();
	event->SetDescription(_L("Test"));
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 2 - 5); // Shouldn't change
	TEST2(event->Id(), KTestEventNum + 2); // Shouldn't change

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventChanged);
	TEST2(logId, KTestEventNum + 2);
	TEST2(viewIndex, 0);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Change an event in the view so that it is deleted
	changeObs->StartCollectingChanges();
	event->SetContact(0);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 2 - 5 - 1); // one less now

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, KTestEventNum + 2);
	TEST2(viewIndex, 0);

	// Navigate part way through the view so we have
	// a cursor position part way through...
	i=0;
	count = view->CountL();
	TEST(view->FirstL(active->iStatus));
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	TheTest.Printf(_L("==\n"));
	TEST2(i, count);
	TEST2(view->Event().Id(), TLogId(0));

	// Save id
	TLogId savedId = view->Event().Id();

	// Go one back
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Insert something
	changeObs->StartCollectingChanges();
	event->SetContact(KTestContact);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 2 - 5); // one more now
	TEST2(event->Id(), KTestEventNum + 3);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(logId, KTestEventNum + 3);
	TEST2(viewIndex, 0);

	// Check we can still go forward to the last record
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), savedId);

	// Go one back
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Delete current record
	savedId = view->Event().Id();
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.DeleteEvent(savedId, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum + 2 - 5 - 1); // one less

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, savedId);
	TEST2(viewIndex, KTestEventNum + 2 - 5 - 2); // last but one event

	// Check we're now at the end of the view
	TEST(!view->NextL(active->iStatus));

	// Go to the first record
	TEST(view->FirstL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Delete the first record
	savedId = view->Event().Id();
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.DeleteEvent(savedId, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum - 5);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, savedId);
	TEST2(viewIndex, 0); // first item

	// Check 'next' navigation can be performed correctly
	count = 0;
	view->NextL(active->iStatus);
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		++count;
		}
	while(view->NextL(active->iStatus));
	TEST2(count, KTestEventNum - 5 - 1);

	// Check last record
	savedId = view->Event().Id();
	TEST(view->LastL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(savedId, view->Event().Id());

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Delete the last record
	savedId = view->Event().Id();
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.DeleteEvent(savedId, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KTestEventNum - 6);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, savedId);
	TEST2(viewIndex, count); // There's now one less item, and we deleted the last item of the previous view

	// Check we're still at the end of the view
	TEST(!view->NextL(active->iStatus));

	CleanupStack::PopAndDestroy(5, event); // view, changeObs, active, filter, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0870
@SYMTestCaseDesc	    Tests for CLogViewChangeObserver::HaveChanges(),Changes() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Check for observing changes while adding,deleting events.Check for observing changes after a backup.
                        Check for observing changes while traversing through the events in the view.
						Delete everything in the view and make sure we can't navigate anymore.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewChangeEvents2L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0870 "));
	// Transients
	TInt i=0;
	TInt count;
	TBuf<1> description; description.SetLength(1);
	TInt changeCount;
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;
	TLogId runningNewId = 0;
	//
	const TInt KChangeEventTest2NumberOfEventsInView = 7;
	TestUtils::DeleteDatabaseL();

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetContact(KTestContact);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewChangeObserver* changeObs = CLogViewChangeObserver::NewLC();
	changeObs->SetActive();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs);
	CleanupStack::PushL(view);

	// Add seed events
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogPacketDataEventTypeUid);
	event->SetDirection(buf);
	//
	for(count = 0; count < KChangeEventTest2NumberOfEventsInView; count++)
		{
		// Have to create one event which isn't in the view by default, but
		// when it 'appears' will be part way through
		if	(count == 3 && description[0] != TUint16('Z'))
			{
			description[0] = TUint16('Z');
			event->SetContact(0);

			// Go back and do this one again but with a real view entry this time, i.e. 'D'
			--count;
			}
		else
			{
			description[0] = TUint16('A' + KChangeEventTest2NumberOfEventsInView - count);
			event->SetContact(KTestContact);
			}
		event->SetDescription(description);
		//
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST2(event->Id(), runningNewId++);
	User::After(1 * 1000000);
		}
	//
	TEST2(view->CountL(), 0);
	active->StartL();
	TBool res =view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView);

	res = view->FirstL(active->iStatus);
	TEST(res);
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	
	// Check addition change events
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // [7], 6, 5, 4, 2, 1, 0
	TEST2(view->Event().Id(), 7);

	// Add a new entry - should appear as the first item in the view
	description[0] = TUint16('X');
	event->SetDescription(description);
	//
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(event->Id(), runningNewId++);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0); // 8, [7], 6, 5, 4, 2, 1, 0

	// Check we can go forward
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(6)); // 8, 7, [6], 5, 4, 2, 1, 0

	// Check we can now go back (new first entry)
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // 8, [7], 6, 5, 4, 2, 1, 0
	TEST2(view->Event().Id(), 7);
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // [8], 7, 6, 5, 4, 2, 1, 0
	TEST2(view->Event().Id(), TLogId(KChangeEventTest2NumberOfEventsInView+1));

	// Delete added event
	changeObs->ResetChanges();
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 0);
	active->StartL();
	aClient.DeleteEvent(view->Event().Id(), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // [7], 6, 5, 4, 2, 1, 0

	// Check deletion changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0);

	// Go to end of view
	res = view->LastL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(0)); // 7, 6, 5, 4, 2, 1, [0]

	// Add new event
	description[0] = TUint16('X');
	event->SetDescription(description);
	//
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(event->Id(), runningNewId++);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0); // 9, 7, 6, 5, 4, 2, 1, [0]

	// Check we can't go forward
	res = view->NextL(active->iStatus);
	TEST(!res);

	// Go back to the first record
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(runningNewId-1)); // 9, 7, 6, 5, 4, 2, 1, [0]

	// Delete added event
	changeObs->ResetChanges();
	active->StartL();
	aClient.DeleteEvent(view->Event().Id(), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // [7], 6, 5, 4, 2, 1, 0

	// Check deletion changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0);

	// Go part way through view
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(6)); // 7, [6], 5, 4, 2, 1, 0
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(5)); // 7, 6, [5], 4, 2, 1, 0

	// Add new event
	description[0] = TUint16('X');
	event->SetDescription(description);
	//
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(event->Id(), runningNewId++);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0); // 10, 7, 6, [5], 4, 2, 1, 0
	changeObs->ResetChanges();

	// Work back to beginning
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(6)); // 10, 7, [6], 5, 4, 2, 1, 0
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(7)); // 10, [7], 6, 5, 4, 2, 1, 0
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), runningNewId-1); // [10], 7, 6, 5, 4, 2, 1, 0

	// Delete added event
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 0);
	active->StartL();
	aClient.DeleteEvent(view->Event().Id(), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // [7], 6, 5, 4, 2, 1, 0

	// Check deletion changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0);
	changeObs->ResetChanges();

	// Go back to the first record
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(7)); // [7], 6, 5, 4, 2, 1, 0

	// Move one record forward
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(6)); // 7, [6], 5, 4, 2, 1, 0

	// Change 'Z' event so that it now appears in the view
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 0);
	active->StartL();
	event->SetId(TLogId(3));
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//
	event->SetContact(KTestContact);
	active->StartL();
	changeObs->StartCollectingChanges();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView+1);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded); // Change resulted in an addition to the view
	TEST2(logId, TLogId(3));
	TEST2(viewIndex, 4); // 7, [6], 5, 4, 3, 2, 1, 0
	changeObs->ResetChanges();

	// Move forwards and check
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(5)); // 7, 6, [5], 4, 3, 2, 1, 0
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(4)); // 7, 6, 5, [4], 3, 2, 1, 0
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(3)); // 7, 6, 5, 4, [3], 2, 1, 0

	// Change 'Z' event so that its now missing from the view again
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 0);
	active->StartL();
	event->SetId(TLogId(3));
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//
	event->SetContact(0);
	active->StartL();
	changeObs->StartCollectingChanges();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(3));
	TEST2(viewIndex, 4); // 7, 6, 5, 4, [2], 1, 0
	changeObs->ResetChanges();

	// Move forwards and check
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // 7, 6, 5, 4, 2, [1], 0
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(0)); // 7, 6, 5, 4, 2, 1, [0]

	// Move back one
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // 7, 6, 5, 4, 2, [1], 0
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 0);

	// Change 'Z' event so that it now appears in the view
	active->StartL();
	event->SetId(TLogId(3));
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//
	event->SetContact(KTestContact);
	active->StartL();
	changeObs->StartCollectingChanges();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView+1);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded); // Change resulted in an addition to the view
	TEST2(logId, TLogId(3));
	TEST2(viewIndex, 4); // 7, 6, 5, 4, 3, 2, [1], 0
	changeObs->ResetChanges();

	// Check can only move forward one more record
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(0)); // 7, 6, 5, 4, 3, 2, 1, [0]

	// Move back until we are before the inserted record
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // 7, 6, 5, 4, 3, 2, [1], 0
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(2)); // 7, 6, 5, 4, 3, [2], 1, 0
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(3)); // 7, 6, 5, 4, [3], 2, 1, 0
	res = view->PreviousL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(4)); // 7, 6, 5, [4], 3, 2, 1, 0
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 0);

	// Change 'Z' event so that its now missing from the view again
	active->StartL();
	event->SetId(TLogId(3));
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//
	event->SetContact(0);
	active->StartL();
	changeObs->StartCollectingChanges();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(3));
	TEST2(viewIndex, 4); // 7, 6, 5, [4], 2, 1, 0
	changeObs->ResetChanges();

	// Check navigating to the end of the view
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(2)); // 7, 6, 5, 4, [2], 1, 0
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // 7, 6, 5, 4, 2, [1], 0
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(0)); // 7, 6, 5, 4, 2, 1, [0]
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 0);
	res = view->NextL(active->iStatus);
	TEST(!res);

	// Delete everything in the view and make sure we can't navigate anymore.
	active->StartL();
	aClient.DeleteEvent(TLogId(4), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 1);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(4));
	TEST2(viewIndex, 3); // 7, 6, 5, 2, 1, [0]
	changeObs->ResetChanges();
	//
	active->StartL();
	aClient.DeleteEvent(TLogId(7), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 2);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(7));
	TEST2(viewIndex, 0); // 6, 5, 2, 1, [0]
	changeObs->ResetChanges();
	//
	active->StartL();
	aClient.DeleteEvent(TLogId(0), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 3);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(0));
	TEST2(viewIndex, 4); // 6, 5, 2, [1]
	changeObs->ResetChanges();
	//
	active->StartL();
	aClient.DeleteEvent(TLogId(5), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 4);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(5));
	TEST2(viewIndex, 1); // 6, 2, [1]
	changeObs->ResetChanges();
	//
	active->StartL();
	aClient.DeleteEvent(TLogId(2), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 5);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(2));
	TEST2(viewIndex, 1); // 6, [1]
	changeObs->ResetChanges();
	//
	active->StartL();
	aClient.DeleteEvent(TLogId(6), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 6);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(6));
	TEST2(viewIndex, 0); // [1]
	changeObs->ResetChanges();

	// Check we can't go backwards or forwards
	res = view->NextL(active->iStatus);
	TEST(!res);
	res = view->PreviousL(active->iStatus);
	TEST(!res);
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // [1]
	res = view->LastL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // [1]

	// Delete last event in view
	active->StartL();
	aClient.DeleteEvent(TLogId(1), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 7);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(1));
	TEST2(viewIndex, 0); // *Empty*
	changeObs->ResetChanges();

	// Check we can't navigate
	res = view->NextL(active->iStatus);
	TEST(!res);
	res = view->PreviousL(active->iStatus);
	TEST(!res);
	res = view->FirstL(active->iStatus);
	TEST(!res);
	res = view->LastL(active->iStatus);
	TEST(!res);

	CleanupStack::PopAndDestroy(5, event); // view, changeObs, active, filter, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0871
@SYMTestCaseDesc	    Tests for CLogViewChangeObserver::HaveChanges(),Changes() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Check for observing changes while adding,deleting events.Check for observing changes after a backup.
                        Check for observing changes while traversing through the events in the view.
						Delete everything in the view and make sure we can't navigate anymore.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewChangeEvents2aL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0871 "));
	// Transients
	TInt i=0;
	TInt count;
	TBuf<1> description; description.SetLength(1);
	TInt changeCount;
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;
	TLogId runningNewId = 0;
	//
	const TInt KChangeEventTest2NumberOfEventsInView = 7;
	TestUtils::DeleteDatabaseL();

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetContact(KTestContact);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewChangeObserver* changeObs = CLogViewChangeObserver::NewLC();
	changeObs->SetActive();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs);
	CleanupStack::PushL(view);

	// Add seed events
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogPacketDataEventTypeUid);
	event->SetDirection(buf);
	//
	for(count = 0; count < KChangeEventTest2NumberOfEventsInView; count++)
		{
		// Have to create one event which isn't in the view by default, but
		// when it 'appears' will be part way through
		if	(count == 3 && description[0] != TUint16('Z'))
			{
			description[0] = TUint16('Z');
			event->SetContact(0);

			// Go back and do this one again but with a real view entry this time, i.e. 'D'
			--count;
			}
		else
			{
			description[0] = TUint16('A' + KChangeEventTest2NumberOfEventsInView - count);
			event->SetContact(KTestContact);
			}
		event->SetDescription(description);
		//
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST2(event->Id(), runningNewId++);
	User::After(1 * 1000000);
		}
	//
	TEST2(view->CountL(), 0);
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView);


	TEST(view->FirstL(active->iStatus));
	do
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("View Entry[%d]: Id: %d\r\n"), i++, view->Event().Id());
		}
	while(view->NextL(active->iStatus));
	
	// Check addition change events
	TEST(view->FirstL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // [7], 6, 5, 4, 2, 1, 0
	TEST2(view->Event().Id(), 7);

	// Add a new entry - should appear as the first item in the view
	description[0] = TUint16('X');
	event->SetDescription(description);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	//
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(event->Id(), runningNewId++);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0); // 8, [7], 6, 5, 4, 2, 1, 0

	// Check we can go forward
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(6)); // 8, 7, [6], 5, 4, 2, 1, 0

	// Check we can now go back (new first entry)
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // 8, [7], 6, 5, 4, 2, 1, 0
	TEST2(view->Event().Id(), 7);
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // [8], 7, 6, 5, 4, 2, 1, 0
	TEST2(view->Event().Id(), TLogId(KChangeEventTest2NumberOfEventsInView+1));

	// Delete added event
	changeObs->ResetChanges();
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 0);
	active->StartL();
	aClient.DeleteEvent(view->Event().Id(), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // [7], 6, 5, 4, 2, 1, 0

	// Check deletion changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0);

	// Go to end of view
	TEST(view->LastL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(0)); // 7, 6, 5, 4, 2, 1, [0]

	// Add new event
	description[0] = TUint16('X');
	event->SetDescription(description);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	//
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(event->Id(), runningNewId++);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0); // 9, 7, 6, 5, 4, 2, 1, [0]

	// Check we can't go forward
	TEST(!view->NextL(active->iStatus));

	// Go back to the first record
	TEST(view->FirstL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(runningNewId-1)); // 9, 7, 6, 5, 4, 2, 1, [0]

	// Delete added event
	changeObs->ResetChanges();
	active->StartL();
	aClient.DeleteEvent(view->Event().Id(), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // [7], 6, 5, 4, 2, 1, 0

	// Check deletion changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0);

	// Go part way through view
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(6)); // 7, [6], 5, 4, 2, 1, 0
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(5)); // 7, 6, [5], 4, 2, 1, 0

	// Add new event
	description[0] = TUint16('X');
	event->SetDescription(description);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	//
	changeObs->StartCollectingChanges();
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(event->Id(), runningNewId++);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0); // 10, 7, 6, [5], 4, 2, 1, 0
	changeObs->ResetChanges();

	// Work back to beginning
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(6)); // 10, 7, [6], 5, 4, 2, 1, 0
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(7)); // 10, [7], 6, 5, 4, 2, 1, 0
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), runningNewId-1); // [10], 7, 6, 5, 4, 2, 1, 0

	// Delete added event
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 0);
	active->StartL();
	aClient.DeleteEvent(view->Event().Id(), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone); // [7], 6, 5, 4, 2, 1, 0

	// Check deletion changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);
	TEST2(logId, runningNewId-1);
	TEST2(viewIndex, 0);
	changeObs->ResetChanges();

	// Go back to the first record
	TEST(view->FirstL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(7)); // [7], 6, 5, 4, 2, 1, 0

	// Move one record forward
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(6)); // 7, [6], 5, 4, 2, 1, 0

	// Change 'Z' event so that it now appears in the view
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount ,0);
	active->StartL();
	event->SetId(TLogId(3));
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//
	event->SetContact(KTestContact);
	active->StartL();

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	changeObs->StartCollectingChanges();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView+1);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded); // Change resulted in an addition to the view
	TEST2(logId, TLogId(3));
	TEST2(viewIndex, 4); // 7, [6], 5, 4, 3, 2, 1, 0
	changeObs->ResetChanges();

	// Move forwards and check
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(5)); // 7, 6, [5], 4, 3, 2, 1, 0
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(4)); // 7, 6, 5, [4], 3, 2, 1, 0
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(3)); // 7, 6, 5, 4, [3], 2, 1, 0

	// Change 'Z' event so that its now missing from the view again
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount ,0);
	active->StartL();
	event->SetId(TLogId(3));
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//
	event->SetContact(0);
	active->StartL();

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	changeObs->StartCollectingChanges();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(3));
	TEST2(viewIndex, 4); // 7, 6, 5, 4, [2], 1, 0
	changeObs->ResetChanges();

	// Move forwards and check
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // 7, 6, 5, 4, 2, [1], 0
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(0)); // 7, 6, 5, 4, 2, 1, [0]

	// Move back one
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // 7, 6, 5, 4, 2, [1], 0
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount ,0);

	// Change 'Z' event so that it now appears in the view
	active->StartL();
	event->SetId(TLogId(3));
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//
	event->SetContact(KTestContact);
	active->StartL();

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	changeObs->StartCollectingChanges();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView+1);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded); // Change resulted in an addition to the view
	TEST2(logId, TLogId(3));
	TEST2(viewIndex, 4); // 7, 6, 5, 4, 3, 2, [1], 0
	changeObs->ResetChanges();

	// Check can only move forward one more record
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(0)); // 7, 6, 5, 4, 3, 2, 1, [0]

	// Move back until we are before the inserted record
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // 7, 6, 5, 4, 3, 2, [1], 0
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(2)); // 7, 6, 5, 4, 3, [2], 1, 0
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(3)); // 7, 6, 5, 4, [3], 2, 1, 0
	TEST(view->PreviousL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(4)); // 7, 6, 5, [4], 3, 2, 1, 0
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount ,0);

	// Change 'Z' event so that its now missing from the view again
	active->StartL();
	event->SetId(TLogId(3));
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//
	event->SetContact(0);
	active->StartL();

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	changeObs->StartCollectingChanges();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView);

	// Check changes
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(3));
	TEST2(viewIndex, 4); // 7, 6, 5, [4], 2, 1, 0
	changeObs->ResetChanges();

	// Check navigating to the end of the view
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(2)); // 7, 6, 5, 4, [2], 1, 0
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // 7, 6, 5, 4, 2, [1], 0
	TEST(view->NextL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(0)); // 7, 6, 5, 4, 2, 1, [0]
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount ,0);
	TEST(!view->NextL(active->iStatus));

	// Delete everything in the view and make sure we can't navigate anymore.
	active->StartL();
	aClient.DeleteEvent(TLogId(4), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 1);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(4));
	TEST2(viewIndex, 3); // 7, 6, 5, 2, 1, [0]
	changeObs->ResetChanges();
	//
	active->StartL();
	aClient.DeleteEvent(TLogId(7), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 2);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(7));
	TEST2(viewIndex, 0); // 6, 5, 2, 1, [0]
	changeObs->ResetChanges();
	//
	active->StartL();
	aClient.DeleteEvent(TLogId(0), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 3);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(0));
	TEST2(viewIndex, 4); // 6, 5, 2, [1]
	changeObs->ResetChanges();
	//
	active->StartL();
	aClient.DeleteEvent(TLogId(5), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 4);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(5));
	TEST2(viewIndex, 1); // 6, 2, [1]
	changeObs->ResetChanges();
	//
	active->StartL();
	aClient.DeleteEvent(TLogId(2), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 5);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(2));
	TEST2(viewIndex, 1); // 6, [1]
	changeObs->ResetChanges();
	//
	active->StartL();
	aClient.DeleteEvent(TLogId(6), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 6);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(6));
	TEST2(viewIndex, 0); // [1]
	changeObs->ResetChanges();

	// Check we can't go backwards or forwards
	TEST(!view->NextL(active->iStatus));
	TEST(!view->PreviousL(active->iStatus));
	TEST(view->FirstL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // [1]
	TEST(view->LastL(active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->Event().Id(), TLogId(1)); // [1]

	// Delete last event in view
	active->StartL();
	aClient.DeleteEvent(TLogId(1), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, KChangeEventTest2NumberOfEventsInView - 7);
	changeCount = changeObs->Changes().Count();
	TEST2(changeCount, 1);
	type = changeObs->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted); // Change resulted in a removal from the view
	TEST2(logId, TLogId(1));
	TEST2(viewIndex, 0); // *Empty*
	changeObs->ResetChanges();

	// Check we can't navigate
	TEST(!view->NextL(active->iStatus));
	TEST(!view->PreviousL(active->iStatus));
	TEST(!view->FirstL(active->iStatus));
	TEST(!view->LastL(active->iStatus));

	CleanupStack::PopAndDestroy(5, event); // view, changeObs, active, filter, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0872
@SYMTestCaseDesc	    Filtering log view test
@SYMTestPriority 	    High
@SYMTestActions  	    Add the Voice call,FAX,SMS event types to the log client.
                        Set all the event types to filter list.Check for the correct event types and no errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewFilteringDefect1L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0872 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave) CTestActive();
	CleanupStack::PushL(active);

	// VOICE CALL
	event->SetEventType(KLogCallEventTypeUid);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	const TLogId event1 = event->Id();

	// FAX
	event->SetEventType(KLogFaxEventTypeUid);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	const TLogId event2 = event->Id();
	(void) event2;

	// SMS, CONTACT SEARCHED
	event->SetEventType(KLogShortMessageEventTypeUid);
	event->SetFlags(KLogEventContactSearched);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	const TLogId event3 = event->Id();

	CLogViewChangeObserver* changeObs = CLogViewChangeObserver::NewLC();
	changeObs->SetActive();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs);
	CleanupStack::PushL(view);

	CLogFilterList* filterList = new(ELeave) CLogFilterList();
	CleanupStack::PushL(filterList);

	CLogFilter* filter1 = CLogFilter::NewL();
	CleanupStack::PushL(filter1);
	filter1->SetEventType(KLogCallEventTypeUid); // ALL VOICE CALL EVENTS
	filterList->AppendL(filter1);
	
	CLogFilter* filter2 = CLogFilter::NewL();
	CleanupStack::PushL(filter2);
	filter2->SetEventType(KLogShortMessageEventTypeUid); // ALL SMS's WHERE CONTACTS SEARCH FLAG IS SET
	filter2->SetFlags(KLogEventContactSearched);
	filterList->AppendL(filter2);

	TInt count = view->CountL();
	TEST2(count, 0);
	TBool res = view->SetFilterParseFilterByFilterL(*filterList, active->iStatus);
	TEST(res);
	CleanupStack::PopAndDestroy(3, filterList);

	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, 2);

	// Check correct events
	TLogId eventId;
	//
	active->StartL();
	res = view->FirstL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	eventId = view->Event().Id();
	TEST2(eventId, event3);
	//
	active->StartL();
	res = view->NextL(active->iStatus);
	TEST(res);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	eventId = view->Event().Id();
	TEST2(eventId, event1);
	
	CleanupStack::PopAndDestroy(4, event);
	}

//
//
//
static TInt TestDeletingViewWithinObserverCallbackL_CallbackFn(TAny* aView)
	{
	CLogViewEvent* view = reinterpret_cast<CLogViewEvent*>(aView);
	delete view;
	return KErrNone;
	}

//
//
//
static void TestDeletingViewWithinObserverCallbackL_ThreadFunctionL()
	{
	RFs fsSession;
	LEAVE_IF_ERROR(fsSession.Connect());
	CleanupClosePushL(fsSession);
	//
	CLogClient* client = CLogClient::NewL(fsSession);
	CleanupStack::PushL(client);
	//
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);
	//
	CTestActive* active = new(ELeave) CTestActive();
	CleanupStack::PushL(active);

	// Wait for main test code thread
	RSemaphore semaphore;
	const TInt error = semaphore.OpenGlobal(KThreadSemaphoreName);
	LEAVE_IF_ERROR(error);

	// Add 5 recent events to the list
	TBuf<KLogMaxDirectionLength> incoming;
	client->GetString(incoming, R_LOG_DIR_IN);
	event->SetDirection(incoming);

	// Incoming
	for(TInt recent = 1; recent <= 5; recent++)
		{
		event->SetContact(recent);
		for(TInt duplicate = 1; duplicate <= 6; duplicate++)
			{
			active->StartL();
			client->AddEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			__ASSERT_ALWAYS(active->iStatus == KErrNone, User::Panic(_L("ThreadPanic"), 1));
			}
		}

	// Wait now until the main thread says we can delete all events
	semaphore.Signal();
	User::After(5 * 1000000);
	semaphore.Wait();

	// Clear the duplicates which should result in a batch of changes
	// in the other thread
	CLogViewRecent* view = CLogViewRecent::NewL(*client);
	CleanupStack::PushL(view);
	//
	__ASSERT_ALWAYS(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus), User::Panic(_L("ThreadPanic"), 2));
	__ASSERT_ALWAYS(view->CountL() == 5, User::Panic(_L("ThreadPanic"), 3));
	active->StartL();
	CActiveScheduler::Start();
	__ASSERT_ALWAYS(active->iStatus == KErrNone, User::Panic(_L("ThreadPanic"), 4));

	// Now do the clear
	view->ClearDuplicatesL();
	__ASSERT_ALWAYS(view->CountL() == 5, User::Panic(_L("ThreadPanic"), 5));

	// Cleanup
	CleanupStack::PopAndDestroy(view);
	semaphore.Close();
	CleanupStack::PopAndDestroy(4, &fsSession); // active, event, client, fsSession
	}

//
//
//
static TInt TestDeletingViewWithinObserverCallbackL_ThreadFunction(TAny* /*aData*/)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		return KErrNoMemory;
	CActiveScheduler* scheduler = new CActiveScheduler();
	if	(!scheduler)
		return KErrNoMemory;
	CActiveScheduler::Install(scheduler);
	//
	TRAPD(err, TestDeletingViewWithinObserverCallbackL_ThreadFunctionL());
    UNUSED_VAR(err);
	//
	delete scheduler;
	delete cleanup;
	//
	return KErrNone;
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0873
@SYMTestCaseDesc	    Tests for observing changes within a callback
@SYMTestPriority 	    High
@SYMTestActions  	    Check for observing changes within a callback
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDeletingViewWithinObserverCallbackL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0873 "));
	TestUtils::DeleteDatabaseL();

	TInt error = 0;
	//
	CTestActive* active = new(ELeave) CTestActive();
	CleanupStack::PushL(active);

	CLogViewChangeObserver* changeObs = CLogViewChangeObserver::NewLC();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetEventType(KLogCallEventTypeUid); 
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(!res);
	CleanupStack::PopAndDestroy(filter);

	// Create semaphore so we can control other thread
	RSemaphore semaphore;
	error = semaphore.CreateGlobal(KThreadSemaphoreName, 0);
	LEAVE_IF_ERROR(error);
	CleanupClosePushL(semaphore);

	// Create thread which adds two events
	RThread thread;
	error = thread.Create(_L("T_VIEW1_OBS_EVENT_CREATION_THREAD"), TestDeletingViewWithinObserverCallbackL_ThreadFunction, 10 * 1024, 5 * 1024, 20 * 1024, NULL);
	LEAVE_IF_ERROR(error);
	CleanupClosePushL(thread);

	// Start the worker thread going
	thread.Resume();
	User::After(1 * 100000);
	semaphore.Wait();

	// Wait for changes
	CLogChangeDefinition* changes = changeObs->WaitForChangesLC(TCallBack(TestDeletingViewWithinObserverCallbackL_CallbackFn, view), CLogViewChangeObserver::EStopOnChanges);

	// Check the changes
	TEST2(changes->Count(), 1);
	TEST2(changes->At(0), ELogChangeTypeEventAdded);

	TLogId logId = -1;
	TLogDatabaseChangeType dbChangeType = changes->At(0, logId);
	TEST2(dbChangeType, ELogChangeTypeEventAdded);
	TEST2(logId, 0);
	TInt viewIndex = -1;
	dbChangeType = changes->At(0, logId, viewIndex);
	TEST2(logId, 0);
	TEST2(viewIndex, 0);
	//Find a change position by the log id
	TInt pos = changes->Find(logId);
	TEST2(pos, 0);
	pos = changes->Find(8125347);
	TEST2(pos, KErrNotFound);
	//Find a change position by the log id and the change type
	pos = changes->Find(logId, dbChangeType);
	TEST2(pos, 0);
	pos = changes->Find(432837, ELogChangeTypeEventChanged);
	TEST2(pos, KErrNotFound);
	//Find a view position by the view index
	pos = changes->FindByViewIndex(viewIndex);
	TEST2(pos, 0);
	pos = changes->FindByViewIndex(33939);
	TEST2(pos, KErrNotFound);
	//Stream out the changes to a buffer
	TBuf8<100> strmBuf;
	RDesWriteStream strmOut(strmBuf);
	changes->ExternalizeL(strmOut);
	strmOut.CommitL();
	
	CleanupStack::PopAndDestroy(changes);
	
	//Re-create changes from the buffer
	RDesReadStream strmIn(strmBuf);
	changes = CLogChangeDefinition::NewL(strmIn);
	dbChangeType = changes->At(0, logId);
	TEST2(dbChangeType, ELogChangeTypeEventAdded);
	TEST2(logId, 0);
	delete changes;

	// The view should be deleted...
	CleanupStack::PopAndDestroy(&thread);
	CleanupStack::PopAndDestroy(&semaphore);

	CleanupStack::Pop(); // view
	CleanupStack::PopAndDestroy(2, active); // changeObs, active
	}



//
//
//
struct TThreadData
	{
	inline TThreadData() : iArray(NULL), iStopThread(EFalse) { }
	//
	RArray<TLogId>* iArray;
	TBool iStopThread;
	};

// Slave Thread for the TestNavigationWhilstBusyL() test.
// This runs continuously while aThreadData.iStopThread is False.
static void TestNavigationWhilstBusyL_ThreadFunctionL(TThreadData& aThreadData)
	{
	enum TNextOp
		{
		EAdd = 0,
		EDelete,
		EClear,
		};

	const static TNextOp KOpTable[] = 
		{
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EDelete,	EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EDelete,	EDelete,	EDelete,	EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EDelete,	EDelete,	EDelete,	EAdd,		EAdd,		EAdd,		EAdd,
		EDelete,	EDelete,	EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EDelete,	EDelete,	EDelete,	EAdd,		EAdd,		EAdd,		EAdd,
		EDelete,	EDelete,	EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EDelete,	EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,	EAdd,		EDelete,
		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,		EAdd,
		};
	const static TInt KIndexTable[] = 
		{
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			-1,			-1,			0,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			0,			4,			20,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			0,			1,			0,			-1,			-1,			-1,			-1,
		25,			8,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			2,			4,			15,			-1,			-1,			-1,			-1,
		0,			0,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			-1,			-1,			0,			-1,			-1,			-1,			-1,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			0,			-1,			30,			-1,			0,			-1,			31,			-1,			14,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			10,			-1,			2,			-1,			0,			-1,			24,			-1,			11,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			0,			-1,			10,			-1,			0,			-1,			12,			-1,			5,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			15,			-1,			9,			-1,			0,			-1,			19,			-1,			25,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		-1,			0,			-1,			1,			-1,			0,			-1,			7,			-1,			31,
		-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,			-1,
		};
	const static TInt KDelayTable[] = 
		{
		0001,		0001,		0001,		0001,		0001,		0001,		0001,		0001,		0001,		0001,
		0001,		0001,		0001,		0001,		0001,		0001,		0001,		0001,		0001,		0001,
		0001,		0001,		0001,		0001,		0001,		0001,		0001,		0001,		0001,		0001,
		0001,		0001,		0001,		0001,		0001,		0001,		0002,		0002,		0002,		0002,
		0002,		0002,		0002,		0001,		0001,		0001,		0002,		0002,		0002,		0002,
		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,
		0002,		0002,		0002,		0001,		0002,		0001,		0002,		0002,		0002,		0002,
		2500,		8000,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,
		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,
		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,
		0002,		0002,		0002,		2000,		4000000,	1500,		0002,		0002,		0002,		0002,
		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,
		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,
		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,		0002,
		0002,		0002,		0002,		0002,		0002,		0001,		0002,		0002,		0002,		0002,
		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,
		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,
		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,
		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,
		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,
		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,
		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,
		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,
		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,
		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,
		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,		0000,
		};
	const TInt KOpCount = sizeof(KOpTable) / sizeof(TNextOp);

	TInt TotalAdds = 0;
	TInt TotalDeletes = 0;
	
	RFs fsSession;
	LEAVE_IF_ERROR(fsSession.Connect());
	CleanupClosePushL(fsSession);
	//
	CLogClient* client = CLogClient::NewL(fsSession);
	CleanupStack::PushL(client);
	//
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(TEST_LOG_UID);
	//
	CTestActive* active = new(ELeave) CTestActive();
	CleanupStack::PushL(active);

	// Add 5 recent events to the list
	TBuf<KLogMaxDirectionLength> incoming;
	client->GetString(incoming, R_LOG_DIR_IN);
	event->SetDirection(incoming);

	CLogSchedulerTimer* timer = CLogSchedulerTimer::NewLC();
	TInt count = 0;
	TInt i=0;
	TInt Repeat =0;
	//
	while(!(aThreadData.iStopThread))
		{
		const TInt op = KOpTable[i];
		//
		switch(op)
			{
			case EAdd:
				{
				event->SetContact(i);
				active->StartL();
				client->AddEvent(*event, active->iStatus);
				CActiveScheduler::Start();
				__ASSERT_ALWAYS(active->iStatus == KErrNone, User::Panic(_L("ThreadPanic"), 1));
				++count;
				LEAVE_IF_ERROR(aThreadData.iArray->Append(event->Id()));
				// TheTest.Printf(_L("EAdd: i:%d count:%d, id:%d\n"), i, aThreadData.iArray->Count(), event->Id());

				// Have to wait for a minimum of 1 ms here because otherwise DMBS sorts records (by time)
				// in an arbitrary manor
				// If this occurs this test will fail the logId == arrayId test below, as the order
				// will be different between the logeng and arrayOfIds[].
				const TInt timeToWait = KDelayTable[i];
				if	(timeToWait)
					timer->Wait(timeToWait);

				timer->Wait(500000);
				
				TotalAdds++;	
				break;
				}
			case EDelete:
				{
				const TInt index = KIndexTable[i];
				const TLogId logId = (*aThreadData.iArray)[index];
				RDebug::Print(_L("EDelete index[%i]=%d id:%d\n"),i, index, logId);
				active->StartL();
				client->DeleteEvent(logId, active->iStatus);
				CActiveScheduler::Start();
				if (active->iStatus != KErrNone)
					{
					/* This happens if the LogID is not present in the log,
					   so the LogEngine cannot find the event to delete.
					   This can happen if the log is full, as it will start
					   deleting the old events itself */
					RDebug::Print(_L("Delete failed error:%d\n"), active->iStatus.Int());
					__ASSERT_ALWAYS(active->iStatus == KErrNone, User::Panic(_L("ThreadPanic"), 2));
					}
				aThreadData.iArray->Remove(index);
				--count;
				TotalDeletes++;
				break;
				}
			case EClear:
				{
				TTime now;
				now.UniversalTime();
				now += TTimeIntervalDays(1);

				// Clear log
				active->StartL();
				client->ClearLog(now, active->iStatus);
				CActiveScheduler::Start();
				__ASSERT_ALWAYS(active->iStatus == KErrNone, User::Panic(_L("ThreadPanic"), 3));
				aThreadData.iArray->Reset();
				count = 0;
				RDebug::Print(_L("TEST CODE: Log cleared\n"));
				break;
				}
			}
	
		if	(++i >= KOpCount)
			{
			i = 0;
			Repeat++;
			}
		}

	RDebug::Print(_L("Repeats:%d TotalAdds:%d TotalDeletes:%d = %d\n"),Repeat,TotalAdds, TotalDeletes, count);
	
	CleanupStack::PopAndDestroy(timer);

	// Tidy up
	CleanupStack::PopAndDestroy(4, &fsSession);
	}

// Slave Thread for the TestNavigationWhilstBusyL() test.
//
static TInt TestNavigationWhilstBusyL_ThreadFunction(TAny* aData)
	{
	TThreadData* threadData = reinterpret_cast<TThreadData*>(aData);
	//
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		return KErrNoMemory;
	CActiveScheduler* scheduler = new CActiveScheduler();
	if	(!scheduler)
		return KErrNoMemory;
	CActiveScheduler::Install(scheduler);
	//
	TRAPD(err, TestNavigationWhilstBusyL_ThreadFunctionL(*threadData));
    UNUSED_VAR(err);
	//
	delete scheduler;
	delete cleanup;
	//
	return KErrNone;
	}

/**
This tests using the Logeng while it is busy.  It starts a new thread which adds and deletes events
using the data in KOpTable[] and KIndexTable[]. It then checks the database while the new thread is 
running.

@SYMTestCaseID          SYSLIB-LOGENG-CT-0874
@SYMTestCaseDesc	    Tests for navigation while Log engine is busy.
@SYMTestPriority 	    High
@SYMTestActions  	    Start a new thread which adds and deletes events
                        using the data in defined tables.It then checks the database while the new thread is running.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestNavigationWhilstBusyL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0874 "));
	TestUtils::DeleteDatabaseL();

	TInt error = 0;
	TInt count = 0;
	//
	CTestActive* active = new(ELeave) CTestActive();
	CleanupStack::PushL(active);

	CLogViewChangeObserver* changeObs = CLogViewChangeObserver::NewLC();

	// Create a test event type - TEST_LOG_UID
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(TEST_LOG_UID);
	type->SetDescription(KTestEventDesc);
	type->SetLoggingEnabled(ETrue);
	TheTest.Printf(_L("\nTest event type added %x\n"),TEST_LOG_UID );

	// Register the event type
	active->StartL();
	aClient.AddEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(); // type

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetEventType(TEST_LOG_UID);  
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(!res);

	// increase the default log size
	TLogConfig Config;
	const TInt kMaxLogSize = 2000;
	Config.iMaxLogSize = kMaxLogSize;
	aClient.ChangeConfig( Config, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	// Shared data between this and the worker thread
	RArray<TLogId> arrayOfIds(100);
	CleanupClosePushL(arrayOfIds);
	TThreadData threadData;
	threadData.iArray = &arrayOfIds;

	// Create thread which adds two events
	RThread thread;
	RHeap& heap = User::Heap();
	error = thread.Create(_L("T_VIEW1_OBS_EVENT_CREATION_THREAD2"), TestNavigationWhilstBusyL_ThreadFunction, 10 * 1024, &heap, &threadData);
	LEAVE_IF_ERROR(error);
	CleanupClosePushL(thread);

	// Start the worker thread going
	thread.Resume();

	// Suspend this thread
	TheTest.Printf(_L("TEST CODE: Suspending thread for 2 seconds\n"));
	CLogSchedulerTimer* timer = CLogSchedulerTimer::NewLC();
	timer->Wait(10 * 1000000);

	// Apply the filter now there are some records
	TheTest.Printf(_L("TEST CODE: Setting filter again - should be some records now\n"));
	res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST(active->iStatus == KErrNone || active->iStatus == KErrCancel); // KErrCancel when the worker thread clears the log at the same time as we are navigating to the first record
	count = view->CountL();
	TEST(count > 0);

	// Navigate around (up and down) whilst events are being created
	count = view->CountL();
	res = view->FirstL(active->iStatus);
	TEST(res);
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST(active->iStatus == KErrNone || active->iStatus == KErrCancel);
		count--;
		if	(active->iStatus == KErrNone)
			TEST(view->Event().Id() != KLogNullId);
		}
	while(view->NextL(active->iStatus));

	timer->Wait(5 * 1000000);
	count = view->CountL();
	res = view->FirstL(active->iStatus);
	TEST(res);
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST(active->iStatus == KErrNone || active->iStatus == KErrCancel);
		count--;
		if	(active->iStatus == KErrNone)
			TEST(view->Event().Id() != KLogNullId);
		}
	while(view->NextL(active->iStatus));

	timer->Wait(5 * 1000000);
	count = view->CountL();
	res = view->LastL(active->iStatus);
	TEST(res);
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST(active->iStatus == KErrNone || active->iStatus == KErrCancel);
		count--;
		if	(active->iStatus == KErrNone)
			TEST(view->Event().Id() != KLogNullId);
		}
	while(view->PreviousL(active->iStatus));

	timer->Wait(5 * 1000000);
	count = view->CountL();
	res = view->FirstL(active->iStatus);
	TEST(res);
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST(active->iStatus == KErrNone || active->iStatus == KErrCancel);
		count--;
		if	(active->iStatus == KErrNone)
			TEST(view->Event().Id() != KLogNullId);
		}
	while(view->NextL(active->iStatus));

	timer->Wait(5 * 1000000);
	count = view->CountL();
	res = view->LastL(active->iStatus);
	TEST(res);
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST(active->iStatus == KErrNone || active->iStatus == KErrCancel);
		count--;
		if	(active->iStatus == KErrNone)
			TEST(view->Event().Id() != KLogNullId);
		}
	while(view->PreviousL(active->iStatus));

	timer->Wait(5 * 1000000);
	count = view->CountL();
	res = view->LastL(active->iStatus);
	TEST(res);
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST(active->iStatus == KErrNone || active->iStatus == KErrCancel);
		count--;
		if	(active->iStatus == KErrNone)
			TEST(view->Event().Id() != KLogNullId);
		}
	while(view->PreviousL(active->iStatus));
	
	// STOP THE WORKER THREAD
	threadData.iStopThread = ETrue;
	TheTest.Printf(_L("Stopping worker thread\n"));
	timer->Wait(30 * 1000000);
	
	// do some checks on the log
	TLogConfig logConfig;
	aClient.GetConfig( logConfig, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TheTest.Printf(_L("MaxLogSize:%ld MaxRecentLogSize:%d\n"),
		logConfig.iMaxLogSize, logConfig.iMaxRecentLogSize );
	TEST(logConfig.iMaxLogSize > logConfig.iMaxRecentLogSize);

	/*  Check the log has not overflowed.
	    If this fails either increase the log size or reduce the time the 
	    worker thread has been running
     */
	TEST(arrayOfIds.Count() < kMaxLogSize);
	
	// Loop though the logevents created by the worker thread and compare against the 
	// data stored in arrayOfIds. 
	res = view->LastL(active->iStatus);
	TEST(res);
	count = 0;
	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		
		const TLogId logId = view->Event().Id();
		const TLogId arrayId = arrayOfIds[count];
		if	(logId != arrayId)
			TheTest.Printf(_L("Events[%d] - array:%5d, id:%5d ****\n"), count, arrayId, logId);
		// Note: If this test fails the fist thing to check is the time delay after EAdd.
		// If several events get the same time stamp, the order is arbitrary and the test fails here.
		TEST2(logId, arrayId);
		count++;
		}
	while(view->PreviousL(active->iStatus));

	CleanupStack::PopAndDestroy(timer);
	CleanupStack::PopAndDestroy(&thread);
	CleanupStack::PopAndDestroy(&arrayOfIds);
	CleanupStack::PopAndDestroy(filter);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(2, active); // changeObs, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-4014
@SYMTestCaseDesc	    Tests CLogView::CountL works correctly after SetFilterL returns no event.
@SYMTestPriority 	    High
@SYMTestActions  	    1. ClearLog. 
						2. SetFilterL.
						3. Add events and check count.
						4. Step through the added events.
@SYMTestExpectedResults 2 - SetFilterL indicates no event in view.
						3. CountL method returns correct number of events.
						4. Can step through the added events.
						
@SYMDEF                 INC123066
*/
LOCAL_C void INC123066L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-4014 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	// deliberately not using CLogViewChangeObserver

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	// Clear all the events
	TTime now;
	now.UniversalTime();
	now += TTimeIntervalDays(1);
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST2(res, 0);

	TInt count;
	for(count = 0; count < KTestEventNum; count++)
		{
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST2(view->CountL(), count+1);
		}

	// save the most recent logId for checking.
	TLogId lastLogId = event->Id();

	// Can iterate over the added events
	for (count = 0; count < KTestEventNum; count++)
		{
		active->StartL();
		TEST( view->NextL(active->iStatus) );
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TLogId id = view->Event().Id();
		// Most recent event is first. Oldest event is last.
		TEST2((lastLogId - count), id);
		}

	CleanupStack::PopAndDestroy(4, event); // filter, view, active, event

	// See same test using CLogViewRecent in TestRecentView1L.
	}

/**
@SYMTestCaseID          PDS-LOGENG-CT-4017
@SYMTestCaseDesc	    Tests Handling of CLogViewRecent::SetRecentListL when
						Remote Party string contains single quote character
@SYMTestPriority 	    High
@SYMTestActions  	    1. Create 10 Incoming call events from 3 different 
							Remote Parties using single quotes in strings
						2. Call CLogViewRecent::SetRecentListL.
						3. Check that duplicate entries have been deleted and only 
							3 events are showm
						
@SYMTestExpectedResults SetRecentListL indicates 3 events in view					
@SYMDEF                 INC136334
*/
LOCAL_C void INC136334L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-CT-4017 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	// Incoming
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	TInt count;
	//Add 10 missed calls from 3 different contacts
	//These should result in only 3 entries int he log as
	//duplicates should be deleted.
	for(count = 0; count < KTestEventNum; count++)
		{
		//Use a string with a single quote
		event->SetRemoteParty(_L("Sam's Wife"));

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		
		//Use a string with multiple single quotes
		event->SetRemoteParty(_L("'Sam's W'i'f'e'''''''"));

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		
		//Use a string with no single quotes
		event->SetRemoteParty(_L("Sams Wife"));

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		
		}	

	TEST2(view->CountL(), 0);
	TBool res = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus);
	TEST(res);
	
	//All of the duplicate entries should be deleted and there should 
	//only be 3 events in the log
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 3);
	
	//Clear the log for he next test
	aClient.ClearLog(KLogRecentIncomingCalls, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	count = view->CountL();
	TEST2(count, 0);
	
	CleanupStack::PopAndDestroy(3); // view, active, event

	}
//
//
//
void doTestsL()
	{
	TheMatchingIsEnabled = TestUtils::MatchingEnabledL();
	
	TestUtils::Initialize(_L("t_logview1"));
	TestUtils::DeleteDatabaseL();

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CLogChangeNotifier* notifier = CLogChangeNotifier::NewL();
	CleanupStack::PushL(notifier);

	TheTest.Start(_L("Event View With No Filter"));

	TestEventViewL(*client);
	theLog.Write(_L8("Test 1 OK\n"));

	TheTest.Next(_L("Event View Filter alternative"));
	TestViewFilteringDefect1L(*client);
	theLog.Write(_L8("Test 2 OK\n"));
	TheTest.Next(_L("Event View With Filter"));
	TestEventViewFilter1L(*client);
	TestEventViewFilter2L(*client);
	TheTest.Next(_L("INC123066 - LogView not updated if SetFilterL found no event"));
	INC123066L(*client);
	theLog.Write(_L8("Test 3 OK\n"));
	
	TheTest.Next(_L("Recent View"));
	TestRecentView1L(*client);
	TestRecentView2L(*client);
	theLog.Write(_L8("Test 4 OK\n"));

	TheTest.Next(_L("Removing recent events"));
	TestRecentRemove1L(*client);
	TestRecentRemove2L(*client);
	theLog.Write(_L8("Test 5 OK\n"));

	TheTest.Next(_L("Duplicate View"));
	TestDuplicateViewL(*client);
	theLog.Write(_L8("Test 6 OK\n"));

	TheTest.Next(_L("Removing duplicate events"));
	TestDuplicateRemoveL(*client);
	theLog.Write(_L8("Test 7 OK\n"));

	TheTest.Next(_L("Check purge performed on view setup"));
	TestPurgeOnSetup1L(*client); 
	TestPurgeOnSetup2L(*client);
	theLog.Write(_L8("Test 8 OK\n"));

	TheTest.Next(_L("View Purging"));
	TestViewPurgeL(*client);
	theLog.Write(_L8("Test 9 OK\n"));

	TheTest.Next(_L("Clearing duplicate lists"));
	TestClearDuplicatesL(*client);
	theLog.Write(_L8("Test 10 OK\n"));

	TheTest.Next(_L("Phone number matching"));
	TestPhoneNumberMatchingL(*client);
	theLog.Write(_L8("Test 11 OK\n"));	

	TheTest.Next(_L("view flags setting/clearing"));
	TestViewFlagsL(*client);
	theLog.Write(_L8("Test 12 OK\n"));

	TheTest.Next(_L("Recent view flags setting/clearing"));
	TestRecentFlagsL(*client);
	theLog.Write(_L8("Test 13 OK\n"));

	TheTest.Next(_L("View change tests 1"));
	TestViewChangeEvents1L(*client);
	TestViewChangeEvents1aL(*client);
	theLog.Write(_L8("Test 14 OK\n"));

	TheTest.Next(_L("View change tests 2"));
	TestViewChangeEvents2L(*client);
	TestViewChangeEvents2aL(*client);
	theLog.Write(_L8("Test 15 OK\n"));

	TheTest.Next(_L("View deletion from within callback"));
	TestDeletingViewWithinObserverCallbackL(*client);
	theLog.Write(_L8("Test 16 OK\n"));

	TheTest.Next(_L("Test navigation whilst events are added"));
	TestNavigationWhilstBusyL(*client);
	theLog.Write(_L8("Test 17 OK\n"));
	
	TheTest.Next(_L("Defect INC105010 - phone number matching"));
	INC105010L(*client);
	theLog.Write(_L8("Test 18 OK\n"));

	TheTest.Next(_L("INC136334 - The miss call log won't show if you have  '  in your contact"));
	INC136334L(*client);
	theLog.Write(_L8("Test 19 OK\n"));

	CleanupStack::PopAndDestroy(2); // notifier, client;
	}
