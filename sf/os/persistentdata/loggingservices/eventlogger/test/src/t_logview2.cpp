// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <logeng.h>
#include <tz.h>
#include "t_logutil2.h"
#include <logview.h>

RTest TheTest(_L("t_logview2"));

const TInt KTestEventNum = 20;
const TInt KTestErrorEventNum = 3;
const TLogContactItemId KTestContact1 = 0x123;
const TLogContactItemId KTestContact2 = 0x456;
const TLogContactItemId KTestContact3 = 0x789;
_LIT(KTestDirection1, "Direction Test 1");
_LIT(KTestDirection2, "Direction Test 2");
_LIT(KTestDirection3, "Direction Test 3");
_LIT(KTestNumber1, "Number Test 1");
_LIT(KTestNumber2, "Number Test 2");
_LIT(KTestNumber3, "Number Test 3");
_LIT(KTestRemote1, "Remote Test 1");
_LIT(KTestRemote2, "Remote Test 2");
_LIT(KTestRemote3, "Remote Test 3");
_LIT(KTestStatus1, "Status Test 1");
_LIT(KTestStatus2, "Status Test 2");
_LIT(KTestStatus3, "Status Test 3");
_LIT(KTestSubject, "Test Subject");
const TLogLink KTestLink = 0x123456;
_LIT8(KTestData, "Test data");

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0930
@SYMTestCaseDesc	    Tests for the functionality of CLogViewEvent,CLogFilter classes
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for event view after setting filter on the views.
                        Change the filter list configuration and test for the view
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventViewWithFilterListL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0930 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetContact(KTestContact1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KLogDurationNone);
	event->SetNumber(KTestNumber1);
	event->SetRemoteParty(KTestRemote1);
	event->SetStatus(KTestStatus1);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetEventType(KLogDataEventTypeUid);
	event->SetContact(KTestContact2);
	event->SetDirection(KTestDirection2);
	event->SetDurationType(KLogDurationValid);
	event->SetNumber(KTestNumber2);
	event->SetRemoteParty(KTestRemote2);
	event->SetStatus(KTestStatus2);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetEventType(KLogFaxEventTypeUid);
	event->SetContact(KTestContact3);
	event->SetDirection(KTestDirection3);
	event->SetDurationType(KLogDurationData);
	event->SetNumber(KTestNumber3);
	event->SetRemoteParty(KTestRemote3);
	event->SetStatus(KTestStatus3);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(),1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetNumber(KTestNumber1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);
	filter->SetNumber(KTestNumber2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);
	filter->SetNumber(KTestNumber3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetNumber(KTestNumber1);
	filter->SetRemoteParty(KTestRemote1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);
	filter->SetNumber(KTestNumber2);
	filter->SetRemoteParty(KTestRemote2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);
	filter->SetNumber(KTestNumber3);
	filter->SetRemoteParty(KTestRemote3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetNumber(KTestNumber1);
	filter->SetRemoteParty(KTestRemote1);
	filter->SetStatus(KTestStatus1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);
	filter->SetNumber(KTestNumber2);
	filter->SetRemoteParty(KTestRemote2);
	filter->SetStatus(KTestStatus2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);
	filter->SetNumber(KTestNumber3);
	filter->SetRemoteParty(KTestRemote3);
	filter->SetStatus(KTestStatus3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 3);

	list->ResetAndDestroy();

	CleanupStack::PopAndDestroy(4); // list, view, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-3432
@SYMTestCaseDesc	    Test the functionality of CLogViewEvent::SetFilterL with the CLogFilterList parameter containing multiple filters with start time and end time.
@SYMTestPriority 	    High
@SYMTestActions  	    1. Add 2 events.
						2. Setup a CLogFilterList with two filters targeting the first event. Both filters have start and end time. Call SetFilterL.
						3. Verify view has 1 event and no LogServ panic.
						4. Clear the filter list.
						5. Setup filter for first event with start and end time. Call SetFilterL.
						6. Verify view has one event.
                        7. Setup second filter for second event with start and end time. Append this filter to filter list and call SetFilterL.
						8. Verify view has 2 events and no LogServ panic.
@SYMTestExpectedResults Log view has expected number of events and no LogServ panic.
@SYMDEF                 INC102412
*/
LOCAL_C void TestEventViewWithTimestampsInFilterListL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-3432 "));
	TTime testStartTime;
	testStartTime.UniversalTime();
	TTime tomorrow = testStartTime + (TTimeIntervalDays)1;
	TTime twoDaysFromNow = tomorrow + (TTimeIntervalDays)1;

	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(tomorrow, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	// Wait a second before adding events to log.
	User::After(1100000);

	// Add two events to log
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Add event 1
	event->SetEventType(KLogCallEventTypeUid);
	event->SetContact(KTestContact1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KLogDurationNone);
	event->SetNumber(KTestNumber1);
	event->SetRemoteParty(KTestRemote1);
	event->SetStatus(KTestStatus1);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Add event 2
	event->SetEventType(KLogDataEventTypeUid);
	event->SetContact(KTestContact2);
	event->SetDirection(KTestDirection2);
	event->SetDurationType(KLogDurationValid);
	event->SetNumber(KTestNumber2);
	event->SetRemoteParty(KTestRemote2);
	event->SetStatus(KTestStatus2);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Setup filter
	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set half a filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetStartTime(testStartTime);
	TTime t1 = filter->StartTime();
	TEST(t1 == testStartTime);
	filter->SetEndTime(tomorrow);
	TTime t2 = filter->EndTime();
	TEST(t2 == tomorrow);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// set the other half of filter for event 1
	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetNumber(KTestNumber1);
	filter->SetRemoteParty(KTestRemote1);
	filter->SetStatus(KTestStatus1);

	// start time is duplicate of previous filter, 
	// end time is not duplicate
	filter->SetStartTime(testStartTime);
	filter->SetEndTime(twoDaysFromNow);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(),1);

	list->ResetAndDestroy();

	// check contents of the event.
	TBool res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	TEST2(view->Event().Contact(), KTestContact1);
	TEST( view->Event().Number() == KTestNumber1);
	TEST( view->Event().Status() == KTestStatus1);
	TEST( view->Event().RemoteParty() == KTestRemote1);

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetNumber(KTestNumber1);
	filter->SetRemoteParty(KTestRemote1);
	filter->SetStatus(KTestStatus1);
	filter->SetStartTime(testStartTime);
	filter->SetEndTime(tomorrow);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);
	filter->SetNumber(KTestNumber2);
	filter->SetRemoteParty(KTestRemote2);
	filter->SetStatus(KTestStatus2);

	// start time is duplicate,
	// end time is not duplicate
	filter->SetStartTime(testStartTime);
	filter->SetEndTime(twoDaysFromNow);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 2);

	list->ResetAndDestroy();

	// check contents of event 2 (most recent)
	res = view->FirstL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	TEST2(view->Event().Contact(), KTestContact2);
	TEST( view->Event().Number() == KTestNumber2);
	TEST( view->Event().Status() == KTestStatus2);
	TEST( view->Event().RemoteParty() == KTestRemote2);

	// check contents of event 1 (older)
	res = view->NextL(active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	TEST2(view->Event().Contact(), KTestContact1);
	TEST( view->Event().Number() == KTestNumber1);
	TEST( view->Event().Status() == KTestStatus1);
	TEST( view->Event().RemoteParty() == KTestRemote1);

	CleanupStack::PopAndDestroy(4); // list, view, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0931
@SYMTestCaseDesc	    Tests for recent view list with filters 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for recent view list,with different settings of the filter. 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentViewWithFilterL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0931 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetNumber(_L("1"));

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDurationType(KLogDurationData);
	event->SetNumber(_L("2"));

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetStatus(KTestStatus1);
	event->SetNumber(_L("3"));

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetContact(KTestContact1);
	event->SetNumber(_L("4"));

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 4);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	// This should make no difference
	filter->SetEventType(KLogCallEventTypeUid);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 4);

	// This should make no difference
	filter->SetDirection(buf);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 4);

	filter->SetDurationType(KLogDurationData);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 3);

	filter->SetStatus(KTestStatus1);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 2);

	filter->SetContact(KTestContact1);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	filter->SetStatus(_L("Dummy"));
	TEST(!view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));

	CleanupStack::PopAndDestroy(4); // filter, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0932
@SYMTestCaseDesc	    Tests for recent view list with filter set for the view
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for recent view list set by the specified set of filters.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentViewWithFilterListL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0932 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetDurationType(KLogDurationNone);
	event->SetContact(KTestContact1);
	event->SetStatus(KTestStatus1);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDurationType(KLogDurationValid);
	event->SetContact(KTestContact2);
	event->SetStatus(KTestStatus2);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDurationType(KLogDurationData);
	event->SetContact(KTestContact3);
	event->SetStatus(KTestStatus3);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 3);

	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	filter->SetDurationType(KLogDurationNone);
	filter->SetStatus(KTestStatus1);
	filter->SetContact(KTestContact1);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	filter->SetDurationType(KLogDurationValid);
	filter->SetStatus(KTestStatus2);
	filter->SetContact(KTestContact2);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	filter->SetDurationType(KLogDurationData);
	filter->SetStatus(KTestStatus3);
	filter->SetContact(KTestContact3);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 3);

	list->ResetAndDestroy();
	CleanupStack::PopAndDestroy(4); // list, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0933
@SYMTestCaseDesc	    Tests for duplicate event view with filter 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for duplicate event view count. 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicateViewWithFilterL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0933 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetContact(KTestContact1);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDurationType(KLogDurationValid);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetStatus(KTestStatus1);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDurationType(KLogDurationNone);
	event->SetStatus(KNullDesC);

	// Add event - This will be the entry in recent list
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId logId1 = event->Id();

	// Should only be one event in recent list - the rest are duplicates
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);
	
	TLogRecentList recentList = view->RecentList();
	TEST(recentList == KLogRecentIncomingCalls);

	// Check duplicate count
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 3);
	
	TLogId logId2 = duplicate->Source();
	TEST(logId1 == logId2);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	active->StartL();
	TEST(view->DuplicatesL(*duplicate, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 3);

	filter->SetDurationType(KLogDurationValid);

	active->StartL();
	TEST(view->DuplicatesL(*duplicate, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 2);

	filter->SetStatus(KTestStatus1);

	active->StartL();
	TEST(view->DuplicatesL(*duplicate, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 1);

	filter->SetStatus(KTestStatus2);
	TEST(!view->DuplicatesL(*duplicate, *filter, active->iStatus));

	CleanupStack::PopAndDestroy(5); // filter, duplicate, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0934
@SYMTestCaseDesc	    Tests for duplicate event view with filter list
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for duplicate event view count,set by the specified set of filters.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicateViewWithFilterListL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0934 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetContact(KTestContact1);

	event->SetDurationType(KLogDurationNone);
	event->SetStatus(KNullDesC);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDurationType(KLogDurationValid);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDurationType(KLogDurationData);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Final event will be most recent
	event->SetDurationType(KLogDurationNone);
	event->SetStatus(KNullDesC);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 3);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	filter->SetDurationType(KLogDurationValid);

	active->StartL();
	TEST(view->DuplicatesL(*duplicate, *list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	filter->SetDurationType(KLogDurationData);

	active->StartL();
	TEST(view->DuplicatesL(*duplicate, *list, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 2);

	list->ResetAndDestroy();
	CleanupStack::PopAndDestroy(5); // list, duplicate, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0935
@SYMTestCaseDesc	    Tests for null field set on filter in the event view 
                        Tests for CLogFilter::SetNullFields() function
@SYMTestPriority 	    High
@SYMTestActions  	    Clear the log events and add new events with fields set in.Define null fields in the filter and check the view
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventViewForNullFieldsL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0935 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	// Wait a second
	User::After(1000000);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;
	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	// Make sure there are no events
	TEST(!view->SetFilterL(*filter, active->iStatus));

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetContact(KTestContact1);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDirection(KTestDirection1);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetNumber(KTestNumber1);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetRemoteParty(KTestRemote1);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetStatus(KTestStatus1);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetSubject(KTestSubject);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetLink(KTestLink);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDataL(KTestData);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Test for NULL fields

	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 9);

	filter->SetNullFields(ELogContactField);

	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	filter->SetNullFields(ELogContactField | ELogDirectionField);

	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	filter->SetNullFields(ELogContactField | ELogDirectionField);

	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	filter->SetNullFields(ELogContactField | ELogDirectionField | ELogNumberField);

	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	filter->SetNullFields(ELogContactField | ELogDirectionField | ELogNumberField | ELogRemotePartyField);

	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	filter->SetNullFields(ELogContactField | ELogDirectionField | ELogNumberField | ELogRemotePartyField | ELogStatusField);

	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	filter->SetNullFields(ELogContactField | ELogDirectionField | ELogNumberField | ELogRemotePartyField | ELogStatusField | ELogSubjectField);

	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	filter->SetNullFields(ELogContactField | ELogDirectionField | ELogNumberField | ELogRemotePartyField | ELogStatusField | ELogSubjectField | ELogLinkField);

	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	filter->SetNullFields(ELogContactField | ELogDirectionField | ELogNumberField | ELogRemotePartyField | ELogStatusField | ELogSubjectField | ELogLinkField | ELogDataField);

	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	CleanupStack::PopAndDestroy(4); // filter, view, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0936
@SYMTestCaseDesc	    Tests for null field set on filter for the recent event view list 
@SYMTestPriority 	    High
@SYMTestActions  	    Clear the log events and add new events with fields set in.Define null fields in the filter and check the view
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentViewForNullFieldsL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0936 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetNumber(_L("1"));

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetStatus(KTestStatus1);
	event->SetNumber(_L("2"));

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetSubject(KTestSubject);
	event->SetNumber(_L("3"));

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetLink(KTestLink);
	event->SetNumber(_L("4"));

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDataL(KTestData);
	event->SetNumber(_L("5"));

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 5);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Test for NULL fields

	filter->SetNullFields(ELogStatusField);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	filter->SetNullFields(ELogStatusField | ELogSubjectField);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	filter->SetNullFields(ELogStatusField | ELogSubjectField | ELogLinkField);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	filter->SetNullFields(ELogStatusField | ELogSubjectField | ELogLinkField | ELogDataField);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(view->CountL(), 1);

	CleanupStack::PopAndDestroy(4); // filter, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0937
@SYMTestCaseDesc	    Tests for null field set on filter for the duplicate event view list 
@SYMTestPriority 	    High
@SYMTestActions  	    Clear the log events and add new events with fields set in.Define null fields in the filter and check the view
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicateViewForNullFieldsL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0937 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetContact(KTestContact1);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetStatus(KTestStatus1);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetSubject(KTestSubject);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetLink(KTestLink);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDataL(KTestData);

	// Add event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetDurationType(KLogDurationNone);
	event->SetStatus(KNullDesC);

	// Add event - This will be the entry in recent list
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Should only be one event in recent list - the rest are duplicates
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	// Check duplicate count
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 5);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	filter->SetNullFields(ELogStatusField);

	active->StartL();
	TEST(view->DuplicatesL(*duplicate, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 1);

	filter->SetNullFields(ELogStatusField | ELogSubjectField);

	active->StartL();
	TEST(view->DuplicatesL(*duplicate, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 1);

	filter->SetNullFields(ELogStatusField | ELogSubjectField | ELogLinkField);

	active->StartL();
	TEST(view->DuplicatesL(*duplicate, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 1);

	filter->SetNullFields(ELogStatusField | ELogSubjectField | ELogLinkField | ELogDataField);

	active->StartL();
	TEST(view->DuplicatesL(*duplicate, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 1);

	CleanupStack::PopAndDestroy(5); // filter, duplicate, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0938
@SYMTestCaseDesc	    Querying event view for flags test
@SYMTestPriority 	    High
@SYMTestActions  	    Clear the log events and add new events with fields set in.Define null fields and set flags in the filter and check the view
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventViewForFlagsL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0938 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	// Wait a second
	User::After(1000000);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;
	
	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	// Make sure there are now events
	TEST(!view->SetFilterL(*filter, active->iStatus));

	// Add some events
	event->SetEventType(KLogCallEventTypeUid);
	TLogFlags flags = KLogFlagsMask + 1;
	TLogFlags flags2;

	while(flags--)
		{
		event->ClearFlags(0xF);
		event->SetFlags(flags);

		// Add event
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	flags = KLogFlagsMask + 1;
	while(flags--)
		{
		filter->ClearFlags(0xF);
		// we use this because, since the events were all created without contact ids
		// the log engine looked each one up in the contacts DB and set the flag so we
		// need to set it in the flags being looked for
		flags2 = static_cast<TLogFlags>(flags | KLogEventContactSearched);
		filter->SetFlags(flags2);
		active->StartL();
		TEST(view->SetFilterL(*filter, active->iStatus));
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST((view->Event().Flags() & flags2) == flags2);

		while(view->NextL(active->iStatus))
			{
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			TEST((view->Event().Flags() & flags2) == flags2);
			}
		}
	
	// Now TEST we can query for the inverse
	filter->SetNullFields(ELogFlagsField);

	flags = KLogFlagsMask + 1;
	while(flags--)
		{
		filter->ClearFlags(0xF);
		// we use this because, since the events were all created without contact ids
		// the log engine looked each one up in the contacts DB and set the flag so we
		// need to unset it in the flags being excluded
		flags2 = static_cast<TLogFlags>(flags & ~KLogEventContactSearched);
		filter->SetFlags(flags2);

		active->StartL();
		TEST(view->SetFilterL(*filter, active->iStatus));
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST((~view->Event().Flags() & flags2) == flags2);

		while(view->NextL(active->iStatus))
			{
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			TEST((~view->Event().Flags() & flags2) == flags2);
			}
		}

	CleanupStack::PopAndDestroy(4); // filter, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0939
@SYMTestCaseDesc	    Querying recent event view for flags test
@SYMTestPriority 	    High
@SYMTestActions  	    Change the log configuration.Clear the log for any recent list and add some events.Query for flags in the recent view list
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentViewForFlagsL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0939 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig config;

	// Change the log engine config
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	config.iMaxRecentLogSize = KLogFlagsMask + 1;

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Make sure there are no events
	TEST(!view->SetRecentListL(KLogNullRecentList, *filter, active->iStatus));

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	// Add some events
	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	TLogFlags flags = KLogFlagsMask + 1;
	while(flags--)
		{
		event->ClearFlags(0xF);
		event->SetFlags(flags);
		event->SetContact(flags);

		// Add event
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	flags = KLogFlagsMask + 1;
	while(flags--)
		{
		filter->ClearFlags(0xF);
		filter->SetFlags(flags);

		active->StartL();
		TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST((view->Event().Flags() & flags) == flags);

		while(view->NextL(active->iStatus))
			{
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			TEST((view->Event().Flags() & flags) == flags);
			}
		}
	
	// Now TEST we can query for the inverse
	filter->SetNullFields(ELogFlagsField);

	flags = KLogFlagsMask + 1;
	while(flags--)
		{
		filter->ClearFlags(0xF);
		filter->SetFlags(flags);

		active->StartL();
		TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST((~view->Event().Flags() & flags) == flags);

		while(view->NextL(active->iStatus))
			{
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			TEST((~view->Event().Flags() & flags) == flags);
			}
		}

	CleanupStack::PopAndDestroy(4); // filter, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0940
@SYMTestCaseDesc	    Querying duplicate event view for flags test
@SYMTestPriority 	    High
@SYMTestActions  	    Change the log configuration.Clear the log for any recent list and add some events.Query for flags in the duplicate view list
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicateViewForFlagsL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0940 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Make sure there are now events
	TEST(!view->SetRecentListL(KLogNullRecentList, *filter, active->iStatus));

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	// Add some events
	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetDurationType(KLogDurationNone);
	event->SetContact(KTestContact1);
	event->SetStatus(KTestStatus1);

	TLogFlags flags = KLogFlagsMask + 1;
	while(flags--)
		{
		event->ClearFlags(0xF);
		event->SetFlags(flags);

		// Add event
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// Add an extra event which will be the most recent
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);

	flags = KLogFlagsMask + 1;
	while(flags--)
		{
		filter->ClearFlags(0xF);
		filter->SetFlags(flags);

		active->StartL();
		TEST(view->DuplicatesL(*duplicate, *filter, active->iStatus));
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST((duplicate->Event().Flags() & flags) == flags);

		while(duplicate->NextL(active->iStatus))
			{
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			TEST((duplicate->Event().Flags() & flags) == flags);
			}
		}
	
	// Now TEST we can query for the inverse
	filter->SetNullFields(ELogFlagsField);

	flags = KLogFlagsMask + 1;
	while(flags--)
		{
		filter->ClearFlags(0xF);
		filter->SetFlags(flags);

		active->StartL();
		TEST(view->DuplicatesL(*duplicate, *filter, active->iStatus));
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST((~duplicate->Event().Flags() & flags) == flags);

		while(duplicate->NextL(active->iStatus))
			{
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			TEST((~duplicate->Event().Flags() & flags) == flags);
			}
		}

	CleanupStack::PopAndDestroy(5); // filter, duplicate, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0941
@SYMTestCaseDesc	    Tests for removing recent events.Tests for CLogClient::ClearLog() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Clear log for any recent entries.Add events and create the view.Test for removing the recent events from the view
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRemovingRecentEventsL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0941 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Make sure there are no events
	TEST(!view->SetRecentListL(KLogNullRecentList, active->iStatus));

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	// Add some events
	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	TInt count = 10;
	while(count--)
		{
		// Add event
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// Create the view
	active->StartL();
	TEST(view->SetRecentListL(KLogNullRecentList, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TLogId id = view->Event().Id();
	while(view->NextL(active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Try removing the previous event
		view->RemoveL(id);
		id = view->Event().Id();
		}

	// Try removing the current event
	view->RemoveL(id);
	TEST(!view->SetRecentListL(KLogNullRecentList, active->iStatus));

	CleanupStack::PopAndDestroy(3); // view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0942
@SYMTestCaseDesc	    Removing duplicate events from the duplicate view
                        Tests for CLogViewRecent::DuplicatesL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Clear log for any recent entries.Add events and create the view and duplicate view.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRemovingDuplicateEventsL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0942 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Make sure there are no events
	TEST(!view->SetRecentListL(KLogNullRecentList, active->iStatus));

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	// Add some events
	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetDurationType(KLogDurationNone);
	event->SetContact(KTestContact1);
	event->SetStatus(KTestStatus1);

	TInt count = 10;
	while(count--)
		{
		// Add event
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheTest.Printf(_L("EVENT ID: %d\n"), event->Id());
		}

	// Create the view
	active->StartL();
	TEST(view->SetRecentListL(KLogNullRecentList, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Create the duplicate view
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);
	count = duplicate->CountL();
	TEST2(count, 9);

	TLogId id = duplicate->Event().Id();
	while(duplicate->NextL(active->iStatus))
		{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Try removing the previous event
		duplicate->RemoveL(id);
		id = duplicate->Event().Id();
		}

	// Try removing the current event
	duplicate->RemoveL(id);
	count = duplicate->CountL();
	TEST2(count, 0);
	//
	TEST(!view->DuplicatesL(*duplicate, active->iStatus));

	CleanupStack::PopAndDestroy(4); // duplicate, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0943
s@SYMTestCaseDesc	    Multiple log event views test
@SYMTestPriority 	    High
@SYMTestActions  	    Add events and set the filter with different fields.Check for log event views
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMultipleViews1L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0943 "));
	CTestActive* active1 = new(ELeave)CTestActive;
	CleanupStack::PushL(active1);

	// Wait a second
	User::After(1000000);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active1->StartL();
	aClient.ClearLog(now, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	CTestActive* active2 = new(ELeave)CTestActive;
	CleanupStack::PushL(active2);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetContact(KTestContact1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KLogDurationNone);
	event->SetNumber(KTestNumber1);
	event->SetRemoteParty(KTestRemote1);
	event->SetStatus(KTestStatus1);

	// Add event
	active1->StartL();
	aClient.AddEvent(*event, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	event->SetEventType(KLogDataEventTypeUid);
	event->SetContact(KTestContact2);
	event->SetDirection(KTestDirection2);
	event->SetDurationType(KLogDurationValid);
	event->SetNumber(KTestNumber2);
	event->SetRemoteParty(KTestRemote2);
	event->SetStatus(KTestStatus2);

	// Add event
	active1->StartL();
	aClient.AddEvent(*event, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	event->SetEventType(KLogFaxEventTypeUid);
	event->SetContact(KTestContact3);
	event->SetDirection(KTestDirection3);
	event->SetDurationType(KLogDurationData);
	event->SetNumber(KTestNumber3);
	event->SetRemoteParty(KTestRemote3);
	event->SetStatus(KTestStatus3);

	// Add event
	active1->StartL();
	aClient.AddEvent(*event, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	CLogViewEvent* view1 = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view1);

	CLogViewEvent* view2 = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view2);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test Views
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetNumber(KTestNumber1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);
	filter->SetNumber(KTestNumber2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);
	filter->SetNumber(KTestNumber3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetNumber(KTestNumber1);
	filter->SetRemoteParty(KTestRemote1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);
	filter->SetNumber(KTestNumber2);
	filter->SetRemoteParty(KTestRemote2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);
	filter->SetNumber(KTestNumber3);
	filter->SetRemoteParty(KTestRemote3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetNumber(KTestNumber1);
	filter->SetRemoteParty(KTestRemote1);
	filter->SetStatus(KTestStatus1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);
	filter->SetNumber(KTestNumber2);
	filter->SetRemoteParty(KTestRemote2);
	filter->SetStatus(KTestStatus2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);
	filter->SetNumber(KTestNumber3);
	filter->SetRemoteParty(KTestRemote3);
	filter->SetStatus(KTestStatus3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	CleanupStack::PopAndDestroy(6); // list, view1, view2, event, active1, active2
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0944
@SYMTestCaseDesc	    Multiple log event views test
@SYMTestPriority 	    High
@SYMTestActions  	    Add events and set the filter with different fields.
						Check for log event views and the test for the count of events
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMultipleViews2L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0944 "));
	CTestActive* active1 = new(ELeave)CTestActive;
	CleanupStack::PushL(active1);

	// Wait a second
	User::After(1000000);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active1->StartL();
	aClient.ClearLog(now, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	CTestActive* active2 = new(ELeave)CTestActive;
	CleanupStack::PushL(active2);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetContact(KTestContact1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KLogDurationNone);
	event->SetNumber(KTestNumber1);
	event->SetRemoteParty(KTestRemote1);
	event->SetStatus(KTestStatus1);

	// Add event
	active1->StartL();
	aClient.AddEvent(*event, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	event->SetEventType(KLogDataEventTypeUid);
	event->SetContact(KTestContact2);
	event->SetDirection(KTestDirection2);
	event->SetDurationType(KLogDurationValid);
	event->SetNumber(KTestNumber2);
	event->SetRemoteParty(KTestRemote2);
	event->SetStatus(KTestStatus2);

	// Add event
	active1->StartL();
	aClient.AddEvent(*event, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	event->SetEventType(KLogFaxEventTypeUid);
	event->SetContact(KTestContact3);
	event->SetDirection(KTestDirection3);
	event->SetDurationType(KLogDurationData);
	event->SetNumber(KTestNumber3);
	event->SetRemoteParty(KTestRemote3);
	event->SetStatus(KTestStatus3);

	// Add event
	active1->StartL();
	aClient.AddEvent(*event, active1->iStatus);
	CActiveScheduler::Start();
	TEST2(active1->iStatus.Int(), KErrNone);

	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	CLogViewEvent* view1 = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view1);

	CLogClient* client2 = CLogClient::NewL(theFs);
	CleanupStack::PushL(client2);

	CLogViewEvent* view2 = CLogViewEvent::NewL(*client2);
	CleanupStack::PushL(view2);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test Views
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetNumber(KTestNumber1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);
	filter->SetNumber(KTestNumber2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);
	filter->SetNumber(KTestNumber3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetNumber(KTestNumber1);
	filter->SetRemoteParty(KTestRemote1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);
	filter->SetNumber(KTestNumber2);
	filter->SetRemoteParty(KTestRemote2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);
	filter->SetNumber(KTestNumber3);
	filter->SetRemoteParty(KTestRemote3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	/////////////////////////////////////////

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact1);
	filter->SetDirection(KTestDirection1);
	filter->SetDurationType(KLogDurationNone);
	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetNumber(KTestNumber1);
	filter->SetRemoteParty(KTestRemote1);
	filter->SetStatus(KTestStatus1);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 1);
	TEST2(view2->CountL(), 1);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact2);
	filter->SetDirection(KTestDirection2);
	filter->SetDurationType(KLogDurationValid);
	filter->SetEventType(KLogDataEventTypeUid);
	filter->SetNumber(KTestNumber2);
	filter->SetRemoteParty(KTestRemote2);
	filter->SetStatus(KTestStatus2);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 2);
	TEST2(view2->CountL(), 2);

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// Set filter
	filter->SetContact(KTestContact3);
	filter->SetDirection(KTestDirection3);
	filter->SetDurationType(KLogDurationData);
	filter->SetEventType(KLogFaxEventTypeUid);
	filter->SetNumber(KTestNumber3);
	filter->SetRemoteParty(KTestRemote3);
	filter->SetStatus(KTestStatus3);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	// Test View
	active1->StartL();
	active2->StartL();
	TEST(view1->SetFilterL(*list, active1->iStatus));
	TEST(view2->SetFilterL(*list, active2->iStatus));
	CActiveScheduler::Start();
	CActiveScheduler::Start();
	TEST2(view1->CountL(), 3);
	TEST2(view2->CountL(), 3);

	list->ResetAndDestroy();

	CleanupStack::PopAndDestroy(7); // list, view1, view2, client2, event, active1, active2
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0945
@SYMTestCaseDesc	    Tests for notification on changes done to the view
                        Tests for CLogViewChangeObserver::iStatus 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for setting a filter on the view.Check for receiving notifications on adding events to the view
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewChangeEventsL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0945 "));
	TestUtils::DeleteDatabaseL();

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	CTestActive* active = new(ELeave) CTestActive();
	CleanupStack::PushL(active);

	CLogViewChangeObserver* changeObs = CLogViewChangeObserver::NewLC();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	TEST(!view->SetFilterL(*filter, active->iStatus));

	// Add some events and check we receive change notifications
	TInt count;
	TLogId logId;
	TInt viewIndex;
	//
	for(count = 0; count < KTestEventNum; count++)
		{
		aClient.AddEvent(*event, changeObs->iStatus);
		CLogChangeDefinition* changes = changeObs->WaitForChangesLC(CLogViewChangeObserver::EStopOnBoth);
		TEST2(changeObs->iStatus.Int(), KErrNone);
		TEST(event->Id() == (TLogId) count);
		//
		User::After(1);
		//
		TEST2(changes->Count(), 1);
		TEST2(changes->At(0, logId, viewIndex), ELogChangeTypeEventAdded);
		TEST(logId == (TLogId) count);
		TEST2(viewIndex, 0);
		CleanupStack::PopAndDestroy(changes);
		}

	CleanupStack::PopAndDestroy(5, event); // filter, view, changeObs, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0946
@SYMTestCaseDesc	    Tests for events in the recent view list 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for events in the recent view list after setting the filters for incoming,outgoing and missed calls
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestAltEventsInRecentListL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0946 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	// Clear recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TLogString buf;
	event->SetEventType(KLogCallEventTypeUid);
	
	// Add Incoming events
	aClient.GetString(buf, R_LOG_DIR_IN);	
	event->SetDirection(buf);
	event->SetContact(1);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);	
	aClient.GetString(buf, R_LOG_DIR_IN_ALT);
	event->SetDirection(buf);
	event->SetContact(2);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	// Verify both events in recent list
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);  
	TEST2(view->CountL(), 2);

	// Add Outgoing events
	aClient.GetString(buf, R_LOG_DIR_OUT);
	event->SetDirection(buf);
	event->SetContact(3);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	aClient.GetString(buf, R_LOG_DIR_OUT_ALT);
	event->SetDirection(buf);
	event->SetContact(4);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	// Verify both events in recent list
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);

	// Add Missed events
	aClient.GetString(buf, R_LOG_DIR_MISSED);
	event->SetDirection(buf);
	event->SetContact(5);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	aClient.GetString(buf, R_LOG_DIR_MISSED_ALT);
	event->SetDirection(buf);
	event->SetContact(6);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	// Verify both events in recent list
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);

	CleanupStack::PopAndDestroy(3); // view, active, event, 
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0947
@SYMTestCaseDesc	    Tests for view with filter by time 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for view with filters set by different time
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestFilterByTimeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0947 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	// Clear recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetEventType(KLogCallEventTypeUid);
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);	
	event->SetDirection(buf);
	
	// add first event (events following are added on successive days)
	event->SetContact(1);  // ensures events are not duplicates
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);		

	TTime systemTime;
	systemTime.UniversalTime();

	// set filter startDate
	TTime startDate = systemTime + (TTimeIntervalHours)+1;

	// add event 2	
	systemTime += (TTimeIntervalDays)+1;
	event->SetContact(2);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);	

	event->SetTime(systemTime);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	// add event 3
	systemTime += (TTimeIntervalDays)+1;
	event->SetContact(3);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetTime(systemTime);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// set filter endDate 
	TTime endDate = systemTime + (TTimeIntervalHours)+1;

	// add event 4
	systemTime += (TTimeIntervalDays)+1;
	event->SetContact(4);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetTime(systemTime);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	// check using both start and end
	filter->SetStartTime(startDate);	
	filter->SetEndTime(endDate);		
	active->StartL();
	TEST(view->SetRecentListL(KLogNullRecentList, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);  
	TEST2(view->CountL(), 2); 

	//  only end filter
	filter->SetStartTime(TTime(0));	// reset	
	filter->SetEndTime(endDate);		
	active->StartL();
	TEST(view->SetRecentListL(KLogNullRecentList, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);  
	TEST2(view->CountL(), 3); 

	//  only start filter
	filter->SetStartTime(startDate); 
	filter->SetEndTime(TTime(0));		
	active->StartL();
	TEST(view->SetRecentListL(KLogNullRecentList, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);  
	TEST2(view->CountL(), 3); 	

	//  no time filter
	filter->SetStartTime(TTime(0)); 
	filter->SetEndTime(TTime(0));		
	active->StartL();
	TEST(view->SetRecentListL(KLogNullRecentList, *filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);  
	TEST2(view->CountL(), 4); 	

	CleanupStack::PopAndDestroy(4); // view, active, event, filter 
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0948
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events for duplicates
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents1L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0948 "));
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;

	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();


	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	// Add a new event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check change events
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(100000);
	CActiveScheduler::Start();
	delete timer;
	
	TEST2(changeObs2->Changes().Count(), 2);
	type = changeObs2->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	type = changeObs2->Changes().At(1, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Check duplicate count
	active->StartL();
	TEST(recent->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 1);

	// Check change events
	User::After(100000);
	TEST2(changeObs1->Changes().Count(), 1);
	type = changeObs1->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);

	CleanupStack::PopAndDestroy(8); // changeObs1, changeObs2, view, filter, event, active, recent, duplicate
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0949
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events when recent list purged
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents2L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0949 "));
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;

	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Get config
	TLogConfig config;
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add max allowed events
	TInt count = config.iMaxRecentLogSize;
	while(count--)
		{
		event->SetContact(count+1);
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), config.iMaxRecentLogSize);

	// Add a new event, will cause one to be purged from recent view
	event->SetContact(1234);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check change events
	CTestTimer *timer = CTestTimer::NewL();
 	timer->After(100000);
 	CActiveScheduler::Start();
 	delete timer;
		
	TEST2(changeObs2->Changes().Count(), 2);
	type = changeObs2->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	type = changeObs2->Changes().At(1, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), config.iMaxRecentLogSize);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Check duplicate count
	TEST(!recent->DuplicatesL(*duplicate, active->iStatus));

	// Check change events
	User::After(100000);
	TEST2(changeObs1->Changes().Count(), 1);
	type = changeObs1->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);

	CleanupStack::PopAndDestroy(8); // changeObs1, changeObs2, view, filter, event, active, recent, duplicate
	}
/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0950
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events when recent list purged after changing config
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents3L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0950 "));
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;

	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Get and set config
	TLogConfig config;
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	config.iMaxRecentLogSize = 2;
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add max allowed events
	TInt count = config.iMaxRecentLogSize;
	while(count--)
		{
		event->SetContact(count+1);
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), config.iMaxRecentLogSize);

	// Change config, will cause one event to be purged from recent view
	config.iMaxRecentLogSize = 1;
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check change events
	User::After(100000);
	TEST2(changeObs2->Changes().Count(), 1);
	type = changeObs2->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), config.iMaxRecentLogSize);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Check duplicate count
	TEST(!recent->DuplicatesL(*duplicate, active->iStatus));

	// Check change events
	User::After(100000);
	TEST(!changeObs1->HaveChanges());

	CleanupStack::PopAndDestroy(8); // changeObs1, changeObs2, view, filter, event, active, recent, duplicate
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0951
@SYMTestCaseDesc	    Test for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that when deleting an event causing recent lists to be updated,views don't get unnecessary change events
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents4L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0951 "));
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;

	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetFlags(1); // Need to hide an event from the view

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	event->SetFlags(1);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->ClearFlags(0xF);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	changeObs1->StartCollectingChanges();
	changeObs2->StartCollectingChanges();

	// Delete the event in recent list
	active->StartL();
	aClient.DeleteEvent(event->Id(), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check change events
	User::After(1000000);
	TEST2(changeObs2->Changes().Count(), 2);
	type = changeObs2->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	type = changeObs2->Changes().At(1, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);

	TEST(!changeObs1->HaveChanges());

	CleanupStack::PopAndDestroy(7); // changeObs1, changeObs2, view, filter, event, active, recent
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0952
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events when recent list cleared
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents5L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0952 "));
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;

	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Get config
	TLogConfig config;
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add max allowed events
	TInt count = config.iMaxRecentLogSize;
	while(count--)
		{
		event->SetContact(count+1);
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), config.iMaxRecentLogSize);

	// Clear recent view
	active->StartL();
	aClient.ClearLog(KLogRecentIncomingCalls, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check change events
	User::After(100000);

	count = config.iMaxRecentLogSize;
	TEST2(changeObs2->Changes().Count(), count);
	while(count--)
		{
		type = changeObs2->Changes().At(count, logId, viewIndex);
		TEST2(type, ELogChangeTypeEventDeleted);
		}

	// Check recent view
	TEST(!recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));

	// Check change events
	User::After(100000);
	TEST(!changeObs1->HaveChanges());

	CleanupStack::PopAndDestroy(7); // changeObs1, changeObs2, view, filter, event, active, recent
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0953
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events when removing events from a recent or duplicate view
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents6L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0953 "));
	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Add a duplicate
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Check duplicate count
	active->StartL();
	TEST(recent->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	// Remove duplicate
	duplicate->RemoveL(duplicate->Event().Id());

	CTestTimer* timer = CTestTimer::NewL();
	CleanupStack::PushL(timer);
	timer->After(100000);
	CActiveScheduler::Start();

	// Check change events
	TEST(!changeObs1->HaveChanges());
	TEST(!changeObs2->HaveChanges());

	// Remove recent
	recent->RemoveL(recent->Event().Id());

	// Check change events
	timer->After(100000);
	CActiveScheduler::Start();

	TEST(!changeObs1->HaveChanges());
	TEST(!changeObs2->HaveChanges()); // No event because we should know we called Remove on ourselves

	CleanupStack::PopAndDestroy(9); // changeObs1, changeObs2, view, filter, event, active, recent, duplicate, timer
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0954
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events when clearing duplicates
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents7L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0954 "));
	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Add a duplicate
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Check duplicate count
	active->StartL();
	TEST(recent->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	// Remove duplicate
	recent->ClearDuplicatesL();

	CTestTimer* timer = CTestTimer::NewL();
	CleanupStack::PushL(timer);
	timer->After(100000);
	CActiveScheduler::Start();

	// Check change events
	TEST(!changeObs1->HaveChanges());
	TEST(!changeObs2->HaveChanges());

	CleanupStack::PopAndDestroy(9); // changeObs1, changeObs2, view, filter, event, active, recent, duplicate, timer
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1363
@SYMTestCaseDesc	    Tests for log view change observer 
						Making a call from recent call list causes total freeze if log 
						has been cleared
@SYMTestPriority 	    High
@SYMTestActions  	    Test that editing a recent event does not cause a panic if 
						part of the log has been cleared
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF066296
*/
LOCAL_C void DEF066296L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1363 "));
	TBuf<KLogMaxDirectionLength> buf;

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	// To reproduce this defect, we need a view to be updated
	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	// Set up the event and log a call from number 444
	event->SetEventType(KLogCallEventTypeUid);
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);
	event->SetNumber(_L("444"));
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	// Set up the view to display the recent call list
	active->StartL();
	TEST(view->SetRecentListL(-1, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Get and print current UTC time
	TTime now;
	now.UniversalTime();
	TBuf<0x80> testString;
	now.FormatL(testString,(_L("%H:%T %1\\%2\\%3 %E")));
	TheTest.Printf(_L("Current  UTC  time    - %S\n"), &testString);

	// Move the UTC time forward by one day
	now += TTimeIntervalDays(1);
	User::SetUTCTime(now);

	now.FormatL(testString,(_L("%H:%T %1\\%2\\%3 %E")));
	TheTest.Printf(_L("Changed  UTC  time to - %S\n"), &testString);
	
	// Set up the event and log a call from number 555
	event->SetNumber(_L("555"));
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	// Set up the event and log a call from number 611
	event->SetNumber(_L("611"));
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	//remember this Id - we will try to change this log event later
	TInt logId = event->Id();

	// Set up the event and log a call from number 777
	event->SetNumber(_L("777"));
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Delete all events occurring before today
	// this should remove only the first call to number 444
	TDateTime midnight = now.DateTime();
	midnight.SetHour(0);
	midnight.SetMinute(0);
	midnight.SetSecond(0);
	midnight.SetMicroSecond(0);

	active->StartL();
	aClient.ClearLog(midnight, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Attempt to change an entry from the recent list - that for the call to number 611 
	// without this defect fix, the call to ChangeEvent() will panic with EBadArrayPosition
	event->SetNumber(_L("555"));
	event->SetId(logId);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Restore and print UTC time
	now.UniversalTime();
	now -= TTimeIntervalDays(1);
	User::SetUTCTime(now);

	now.FormatL(testString,(_L("%H:%T %1\\%2\\%3 %E")));
	TheTest.Printf(_L("Restored UTC  time to - %S\n"), &testString);

	CleanupStack::PopAndDestroy(3, event); //event, active, view
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1807
@SYMTestCaseDesc	    Tests that DEF087459 works between 00:00 and 01:00am hometime
@SYMTestPriority 	    Medium
@SYMTestActions  	    Change the home time to 00:30 and run DEF066296 test
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF087459
*/
LOCAL_C void DEF087459L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1807 "));
	RTz tz;				
	TInt err = tz.Connect();
	TEST2(err, KErrNone);
	CleanupClosePushL(tz);
	TTime now, saveNow;
	
	// Get and print current home time and save it
	now.HomeTime();
	saveNow=now;
	
	TBuf<0x80> testString;
	now.FormatL(testString,(_L("%H:%T %1\\%2\\%3 %E")));
	TheTest.Printf(_L("Current  home time    - %S\n"), &testString);

	// Problem occurred between midnight and 01:00am hometime, so change home time to
	// be 00:30am
	TDateTime midnight = now.DateTime();
	midnight.SetHour(0);
	midnight.SetMinute(30);
	midnight.SetSecond(0);
	midnight.SetMicroSecond(0);
	
	TTime newTime=midnight;

	err = tz.SetHomeTime(midnight);
	TEST2(err, KErrNone);

	// Get and print new home time
	now.HomeTime();
	now.FormatL(testString,(_L("%H:%T %1\\%2\\%3 %E")));
	TheTest.Printf(_L("Changed  home time to - %S\n"), &testString);
	TheTest.Printf(_L("Re-running DEF066296 test\n"));
	
	DEF066296L(aClient);
	
	// Restore and print hometime
	err = tz.SetHomeTime(saveNow);
	TEST2(err, KErrNone);
	now.HomeTime();
	now.FormatL(testString,(_L("%H:%T %1\\%2\\%3 %E")));
	TheTest.Printf(_L("Restored home time to - %S\n"), &testString);
	
	CleanupStack::PopAndDestroy(); //tz
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0955
@SYMTestCaseDesc	    Tests that events can be changed to move them between recent lists
@SYMTestPriority 	    High
@SYMTestActions  	    Add the events to the view.Move most recent event to another recent list
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEventRecentView1L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0955 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CLogViewRecent* recent1 = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent1);

	CLogViewDuplicate* duplicate1 = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate1);

	CLogViewRecent* recent2 = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent2);

	CLogViewDuplicate* duplicate2 = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate2);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	TBuf<KLogMaxDirectionLength> incoming;
	aClient.GetString(incoming, R_LOG_DIR_IN);

	// Missed call event
	TBuf<KLogMaxDirectionLength> missed;
	aClient.GetString(missed, R_LOG_DIR_MISSED);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(missed);
	event->SetContact(1);

	// Add some events
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id1 = event->Id();

	User::After(1000000);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id2 = event->Id();

	User::After(1000000);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id3 = event->Id();

	User::After(1000000);

	// Most recent
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id4 = event->Id();

	// Check there's a recent event in the list
	active->StartL();
	TEST(recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent1->CountL(), 1);
	TEST2(recent1->Event().Id(), id4);

	// Check it has duplicates
	active->StartL();
	TEST(recent1->DuplicatesL(*duplicate1, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->CountL(), 3);
	TEST2(duplicate1->Event().Id(), id3);

	// Check id
	active->StartL();
	TEST(duplicate1->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->Event().Id(), id2);

	// Check id
	active->StartL();
	TEST(duplicate1->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->Event().Id(), id1);

	// Other recent list should be empty
	TEST(!recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));

	// Now move most recent event to another recent list
	event->SetDirection(incoming);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Most recent duplicate should now be the latest
	active->StartL();
	TEST(recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent1->CountL(), 1);
	TEST2(recent1->Event().Id(), id3);
	event->CopyL(recent1->Event()); // for later

	// There should still be some duplicates
	active->StartL();
	TEST(recent1->DuplicatesL(*duplicate1, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->CountL(), 2);
	TEST2(duplicate1->Event().Id(), id2);

	// Check id
	active->StartL();
	TEST(duplicate1->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->Event().Id(), id1);

	// Now the event should have moved to another recent list
	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 1);
	TEST2(recent2->Event().Id(), id4);

	// No duplicates yet
	TEST(!recent2->DuplicatesL(*duplicate2, active->iStatus));

	// Now move most recent event to another recent list
	event->SetDirection(incoming);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Most recent duplicate should now be the latest
	active->StartL();
	TEST(recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent1->CountL(), 1);
	TEST2(recent1->Event().Id(), id2);
	event->CopyL(recent1->Event()); // for later

	// There should still be some duplicates
	active->StartL();
	TEST(recent1->DuplicatesL(*duplicate1, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->CountL(), 1);
	TEST2(duplicate1->Event().Id(), id1);

	// Now the event should have moved to another recent list
	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 1);
	TEST2(recent2->Event().Id(), id4);

	// There should still be some duplicates
	active->StartL();
	TEST(recent2->DuplicatesL(*duplicate2, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->CountL(), 1);
	TEST2(duplicate2->Event().Id(), id3);

	// Now move most recent event to another recent list
	event->SetDirection(incoming);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Most recent duplicate should now be the latest
	active->StartL();
	TEST(recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent1->CountL(), 1);
	TEST2(recent1->Event().Id(), id1);
	event->CopyL(recent1->Event()); // for later

	// Should be no more duplicates
	TEST(!recent1->DuplicatesL(*duplicate1, active->iStatus));

	// Now the event should have moved to another recent list
	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 1);
	TEST2(recent2->Event().Id(), id4);

	// There should still be some duplicates
	active->StartL();
	TEST(recent2->DuplicatesL(*duplicate2, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->CountL(), 2);
	TEST2(duplicate2->Event().Id(), id3);

	// Check id
	active->StartL();
	TEST(duplicate2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->Event().Id(), id2);

	// Now move most recent event to another recent list
	event->SetDirection(incoming);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// No more recent events
	TEST(!recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));

	// Now the event should have moved to another recent list
	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 1);
	TEST2(recent2->Event().Id(), id4);

	// There should still be some duplicates
	active->StartL();
	TEST(recent2->DuplicatesL(*duplicate2, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->CountL(), 3);
	TEST2(duplicate2->Event().Id(), id3);

	// Check id
	active->StartL();
	TEST(duplicate2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->Event().Id(), id2);

	// Check id
	active->StartL();
	TEST(duplicate2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->Event().Id(), id1);

	CleanupStack::PopAndDestroy(6); // active, event, recent1, duplicate1, recent2, duplicate2
	}

/**
Test that events can be changed to move them between recent lists
The same as TestChangeEventRecentView1L, but the events are transferred in reverse order

@SYMTestCaseID          SYSLIB-LOGENG-CT-0956
@SYMTestCaseDesc	    Tests that events can be changed to move them between recent lists
@SYMTestPriority 	    High
@SYMTestActions  	    Add the events to the view.Move most recent event to another recent list in reverse order
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEventRecentView2L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0956 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CLogViewRecent* recent1 = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent1);

	CLogViewDuplicate* duplicate1 = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate1);

	CLogViewRecent* recent2 = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent2);

	CLogViewDuplicate* duplicate2 = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate2);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	TBuf<KLogMaxDirectionLength> incoming;
	aClient.GetString(incoming, R_LOG_DIR_IN);

	// Missed call event
	TBuf<KLogMaxDirectionLength> missed;
	aClient.GetString(missed, R_LOG_DIR_MISSED);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(missed);
	event->SetContact(1);

	// Add some events
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id1 = event->Id();

	User::After(1000000);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id2 = event->Id();

	User::After(1000000);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id3 = event->Id();

	User::After(1000000);

	// Most recent
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id4 = event->Id();

	// Check there's a recent event in the list
	active->StartL();
	TEST(recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent1->CountL(), 1);
	TEST2(recent1->Event().Id(), id4);

	// Check it has duplicates
	active->StartL();
	TEST(recent1->DuplicatesL(*duplicate1, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->CountL(), 3);
	TEST2(duplicate1->Event().Id(), id3);

	// Check id
	active->StartL();
	TEST(duplicate1->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->Event().Id(), id2);

	// Check id
	active->StartL();
	TEST(duplicate1->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->Event().Id(), id1);
	event->CopyL(duplicate1->Event()); // for later

	// Other recent list should be empty
	TEST(!recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));

	// Now move latest recent event to another recent list
	event->SetDirection(incoming);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Most recent duplicate should still be the latest
	active->StartL();
	TEST(recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent1->CountL(), 1);
	TEST2(recent1->Event().Id(), id4);

	// There should still be some duplicates
	active->StartL();
	TEST(recent1->DuplicatesL(*duplicate1, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->CountL(), 2);
	TEST2(duplicate1->Event().Id(), id3);

	// Check id
	active->StartL();
	TEST(duplicate1->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->Event().Id(), id2);
	event->CopyL(duplicate1->Event()); // for later

	// Now the event should have moved to another recent list
	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 1);
	TEST2(recent2->Event().Id(), id1);

	// No duplicates yet
	TEST(!recent2->DuplicatesL(*duplicate2, active->iStatus));

	// Now move most recent event to another recent list
	event->SetDirection(incoming);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Most recent duplicate should still be the latest
	active->StartL();
	TEST(recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent1->CountL(), 1);
	TEST2(recent1->Event().Id(), id4);

	// There should still be some duplicates
	active->StartL();
	TEST(recent1->DuplicatesL(*duplicate1, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate1->CountL(), 1);
	TEST2(duplicate1->Event().Id(), id3);
	event->CopyL(duplicate1->Event()); // for later

	// Now the event should have moved to another recent list
	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 1);
	TEST2(recent2->Event().Id(), id2);

	// There should still be some duplicates
	active->StartL();
	TEST(recent2->DuplicatesL(*duplicate2, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->CountL(), 1);
	TEST2(duplicate2->Event().Id(), id1);

	// Now move most recent event to another recent list
	event->SetDirection(incoming);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Most recent duplicate should still be the latest
	active->StartL();
	TEST(recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent1->CountL(), 1);
	TEST2(recent1->Event().Id(), id4);
	event->CopyL(recent1->Event()); // for later

	// Should be no more duplicates
	TEST(!recent1->DuplicatesL(*duplicate1, active->iStatus));

	// Now the event should have moved to another recent list
	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 1);
	TEST2(recent2->Event().Id(), id3);

	// There should still be some duplicates
	active->StartL();
	TEST(recent2->DuplicatesL(*duplicate2, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->CountL(), 2);
	TEST2(duplicate2->Event().Id(), id2);

	// Check id
	active->StartL();
	TEST(duplicate2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->Event().Id(), id1);

	// Now move most recent event to another recent list
	event->SetDirection(incoming);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// No more recent events
	TEST(!recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));

	// Now the event should have moved to another recent list
	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 1);
	TEST2(recent2->Event().Id(), id4);

	// There should still be some duplicates
	active->StartL();
	TEST(recent2->DuplicatesL(*duplicate2, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->CountL(), 3);
	TEST2(duplicate2->Event().Id(), id3);

	// Check id
	active->StartL();
	TEST(duplicate2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->Event().Id(), id2);

	// Check id
	active->StartL();
	TEST(duplicate2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate2->Event().Id(), id1);

	CleanupStack::PopAndDestroy(6); // active, event, recent1, duplicate1, recent2, duplicate2
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0957
@SYMTestCaseDesc	    Tests that change in log view after moving an event
@SYMTestPriority 	    High
@SYMTestActions  	    Check recent views are purged if necessary after moving an event
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEventRecentViewAndPurgeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0957 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CLogViewRecent* recent1 = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent1);

	CLogViewRecent* recent2 = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent2);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogNullRecentList, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Get log config
	TLogConfig config;
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Set the max recent list size to something small
	config.iMaxRecentLogSize = 3;
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	TBuf<KLogMaxDirectionLength> incoming;
	aClient.GetString(incoming, R_LOG_DIR_IN);

	// Missed call event
	TBuf<KLogMaxDirectionLength> missed;
	aClient.GetString(missed, R_LOG_DIR_MISSED);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(missed);

	// Add an event
	event->SetContact(1);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id1 = event->Id();

	User::After(1000000);
	event->SetDirection(incoming);

	// Add an event
	event->SetContact(2);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id2 = event->Id();

	User::After(1000000);

	// Add an event
	event->SetContact(3);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id3 = event->Id();

	User::After(1000000);

	// Add an event
	event->SetContact(4);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id4 = event->Id();

	// Check the views
	active->StartL();
	TEST(recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent1->CountL(), 1);

	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 3);

	// Retrieve event
	event->SetId(id1);
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Now move the oldest event to another recent list - it should be purged
	event->SetDirection(incoming);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Missed call list now empty
	TEST(!recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));

	// Incoming view should be unchanged
	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 3);
	TEST2(recent2->Event().Id(), id4);

	// Check id
	active->StartL();
	TEST(recent2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->Event().Id(), id3);

	// Check id
	active->StartL();
	TEST(recent2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->Event().Id(), id2);

	TTime now;
	now.UniversalTime();

	// This should have no effect
	event->SetTime(now);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming view should be unchanged
	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 3);
	TEST2(recent2->Event().Id(), id4);

	// Check id
	active->StartL();
	TEST(recent2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->Event().Id(), id3);

	// Check id
	active->StartL();
	TEST(recent2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->Event().Id(), id2);

	// Now add a new event to missed view (the latest)
	User::After(1000000);
	event->SetDirection(missed);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	User::After(1000000);
	now.UniversalTime();

	// This should have no effect
	event->SetTime(now);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check the views
	active->StartL();
	TEST(recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent1->CountL(), 1);

	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 3);

	// Now move the latest event to another recent list - oldest should be purged
	event->SetDirection(incoming);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(!recent1->SetRecentListL(KLogRecentMissedCalls, active->iStatus));

	active->StartL();
	TEST(recent2->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->CountL(), 3);
	TEST2(recent2->Event().Id(), event->Id());

	// Check id
	active->StartL();
	TEST(recent2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->Event().Id(), id4);

	// Check id
	active->StartL();
	TEST(recent2->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent2->Event().Id(), id3);

	CleanupStack::PopAndDestroy(4); // active, event, recent1, recent2
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0958
@SYMTestCaseDesc	    Tests for the log view,on changing an event
@SYMTestPriority 	    High
@SYMTestActions  	    Check an event can be removed from recent lists by changing it
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestChangeEventRecentViewRemoveL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0958 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Missed call event
	TBuf<KLogMaxDirectionLength> missed;
	aClient.GetString(missed, R_LOG_DIR_MISSED);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(missed);

	// Add an event
	event->SetContact(1);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id1 = event->Id();

	User::After(1000000);

	// Add some duplicates
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id2 = event->Id();

	User::After(1000000);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id3 = event->Id();

	User::After(1000000);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TLogId id4 = event->Id();

	// Check views
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);
	TEST2(recent->Event().Id(), id4);

	active->StartL();
	TEST(recent->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 3);
	TEST2(duplicate->Event().Id(), id3);

	// Check id
	active->StartL();
	TEST(duplicate->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->Event().Id(), id2);

	// Check id
	active->StartL();
	TEST(duplicate->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->Event().Id(), id1);

	// Fetched call event
	TBuf<KLogMaxDirectionLength> fetched;
	aClient.GetString(missed, R_LOG_DIR_FETCHED);

	// Now change the event to remove it from recent lists
	event->SetDirection(fetched);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check views
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);
	TEST2(recent->Event().Id(), id3);

	active->StartL();
	TEST(recent->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 2);
	TEST2(duplicate->Event().Id(), id2);
	event->CopyL(duplicate->Event()); // For later

	// Check id
	active->StartL();
	TEST(duplicate->NextL(active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->Event().Id(), id1);

	// Now change the event to remove it from recent lists
	event->SetDirection(fetched);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check views
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);
	TEST2(recent->Event().Id(), id3);

	active->StartL();
	TEST(recent->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 1);
	TEST2(duplicate->Event().Id(), id1);
	event->CopyL(duplicate->Event()); // For later

	// Now change the event to remove it from recent lists
	event->SetDirection(fetched);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check views
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentMissedCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);
	TEST2(recent->Event().Id(), id3);
	event->CopyL(recent->Event()); // For later

	// No more duplicates
	TEST(!recent->DuplicatesL(*duplicate, active->iStatus));

	// Now change the event to remove it from recent lists
	event->SetDirection(fetched);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(!recent->SetRecentListL(KLogRecentMissedCalls, active->iStatus));

	CleanupStack::PopAndDestroy(4); // active, event, recent, duplicate
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
@SYMTestCaseID          SYSLIB-LOGENG-CT-0959
@SYMTestCaseDesc	    Tests for log view change observer
						Test change stuff with backup
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events when clearing duplicates even after backup
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents1aL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0959 "));
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;

	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();


	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	// Add a new event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	DelayL(1000000);
	TestLogOpenL();
	
	TEST2(changeObs2->Changes().Count(), 2);
	type = changeObs2->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	type = changeObs2->Changes().At(1, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Check duplicate count
	active->StartL();
	TEST(recent->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 1);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Check change events
	User::After(100000);
	TEST2(changeObs1->Changes().Count(), 1);
	type = changeObs1->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);

	CleanupStack::PopAndDestroy(8, changeObs1); // changeObs1, changeObs2, view, filter, event, active, recent, duplicate
	}

/**
Check that a view doesn't get change events when recent list purged

@SYMTestCaseID          SYSLIB-LOGENG-CT-0960
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events when recent list purged,even after backup
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents2aL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0960 "));
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;

	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Get config
	TLogConfig config;
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add max allowed events
	TInt count = config.iMaxRecentLogSize;
	while(count--)
		{
		event->SetContact(count+1);
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), config.iMaxRecentLogSize);

	// Add a new event, will cause one to be purged from recent view
	event->SetContact(1234);
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();
	DelayL(1000000);
		
	TEST2(changeObs2->Changes().Count(), 2);
	type = changeObs2->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	type = changeObs2->Changes().At(1, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), config.iMaxRecentLogSize);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Check duplicate count
	TEST(!recent->DuplicatesL(*duplicate, active->iStatus));

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Check change events
	User::After(100000);
	TEST2(changeObs1->Changes().Count(), 1);
	type = changeObs1->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);

	CleanupStack::PopAndDestroy(8, changeObs1); // changeObs1, changeObs2, view, filter, event, active, recent, duplicate
	}

/**
Check that a view doesn't get change events when recent list purged after changing config

@SYMTestCaseID          SYSLIB-LOGENG-CT-0961
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events when recent list purged after changing config,even after backup
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents3aL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0961 "));
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;

	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Get and set config
	TLogConfig config;
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	config.iMaxRecentLogSize = 2;
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add max allowed events
	TInt count = config.iMaxRecentLogSize;
	while(count--)
		{
		event->SetContact(count+1);
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), config.iMaxRecentLogSize);

	// Change config, will cause one event to be purged from recent view
	config.iMaxRecentLogSize = 1;
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();
	DelayL(1000000);

	// Check change events
	User::After(100000);
	TEST2(changeObs2->Changes().Count(), 1);
	type = changeObs2->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), config.iMaxRecentLogSize);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Check duplicate count
	TEST(!recent->DuplicatesL(*duplicate, active->iStatus));

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Check change events
	User::After(100000);
	TEST(!changeObs1->HaveChanges());

	CleanupStack::PopAndDestroy(8, changeObs1); // changeObs1, changeObs2, view, filter, event, active, recent, duplicate
	}

/**
Check that when deleting an event causing recent lists to be updated, views don't get unnecessary change events

@SYMTestCaseID          SYSLIB-LOGENG-CT-0962
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that when deleting an event causing recent lists to be updated,views don't get unnecessary change events.
                        Check the test work even after backup operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents4aL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0962 "));
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;

	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetFlags(1); // Need to hide an event from the view

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	event->SetFlags(1);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->ClearFlags(0xF);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	changeObs1->StartCollectingChanges();
	changeObs2->StartCollectingChanges();

	// Delete the event in recent list
	active->StartL();
	aClient.DeleteEvent(event->Id(), active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Check change events
	TEST2(changeObs2->Changes().Count(), 2);
	type = changeObs2->Changes().At(0, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventAdded);
	type = changeObs2->Changes().At(1, logId, viewIndex);
	TEST2(type, ELogChangeTypeEventDeleted);

	TEST(!changeObs1->HaveChanges());

	CleanupStack::PopAndDestroy(7, changeObs1); // changeObs1, changeObs2, view, filter, event, active, recent
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0963
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events when recent list cleared
                        Check the test work even after backup operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents5aL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0963 "));
	TLogId logId;
	TInt viewIndex;
	TLogDatabaseChangeType type;

	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Get config
	TLogConfig config;
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add max allowed events
	TInt count = config.iMaxRecentLogSize;
	while(count--)
		{
		event->SetContact(count+1);
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), config.iMaxRecentLogSize);

	// Clear recent view
	active->StartL();
	aClient.ClearLog(KLogRecentIncomingCalls, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	count = config.iMaxRecentLogSize;
	TEST2(changeObs2->Changes().Count(), count);
	while(count--)
		{
		type = changeObs2->Changes().At(count, logId, viewIndex);
		TEST2(type, ELogChangeTypeEventDeleted);
		}

	// Check recent view
	TEST(!recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Check change events
	User::After(100000);
	TEST(!changeObs1->HaveChanges());

	CleanupStack::PopAndDestroy(7, changeObs1); // changeObs1, changeObs2, view, filter, event, active, recent
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0964
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events when removing events from a recent or duplicate view
                        Check the test work even after backup operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents6aL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0964 "));
	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Add a duplicate
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST(view->CountL() == 2);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Check duplicate count
	active->StartL();
	TEST(recent->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	// Remove duplicate
	duplicate->RemoveL(duplicate->Event().Id());

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Check change events
	TEST(!changeObs1->HaveChanges());
	TEST(!changeObs2->HaveChanges());

	// Remove recent
	recent->RemoveL(recent->Event().Id());

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	TEST(!changeObs1->HaveChanges());
	TEST(!changeObs2->HaveChanges()); // No event because we should know we called Remove on ourselves

	CleanupStack::PopAndDestroy(8, changeObs1); // changeObs1, changeObs2, view, filter, event, active, recent, duplicate
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0965
@SYMTestCaseDesc	    Tests for log view change observer
@SYMTestPriority 	    High
@SYMTestActions  	    Check that a view doesn't get change events when clearing duplicates
                        Check the test work even after backup operations
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHiddenChangeEvents7aL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0965 "));
	CLogViewChangeObserver* changeObs1 = CLogViewChangeObserver::NewLC();
	changeObs1->StartCollectingChanges();

	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs1);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	now += (TTimeIntervalDays)+1;

	// Get rid of all the events in the log
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Incoming call event
	event->SetEventType(KLogCallEventTypeUid);
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);
	event->SetDirection(buf);

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Add a duplicate
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Setup normal event view
	active->StartL();
	TEST(view->SetFilterL(*filter, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);

	CLogViewChangeObserver* changeObs2 = CLogViewChangeObserver::NewLC();
	changeObs2->StartCollectingChanges();

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient, *changeObs2);
	CleanupStack::PushL(recent);

	// Check recent view
	active->StartL();
	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Check duplicate count
	active->StartL();
	TEST(recent->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(recent->CountL(), 1);

	// Remove duplicate
	recent->ClearDuplicatesL();

	// Check that changes work after a backup
	StartBackupL();
	DelayL(1000000);
	TestLogClosedL();
	EndBackupL();
	TestLogOpenL();

	// Check change events
	TEST(!changeObs1->HaveChanges());
	TEST(!changeObs2->HaveChanges());

	CleanupStack::PopAndDestroy(8, changeObs1); // changeObs1, changeObs2, view, filter, event, active, recent, duplicate
	}
	
/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-3511
@SYMTestCaseDesc	    Tests an error condition for adding an event.                       
@SYMTestPriority 	    High
@SYMTestActions  	    A leave will be caused in an observer when a specific event is added. This test 
                        ensures by successful test completion that there are no panics in this 
                        situation.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 DEF108741
*/
LOCAL_C void DEF108741L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-3511 "));
	// Perform clean up
 	TestUtils::DeleteDatabaseL();

	// Create a new event 
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	CTestActive* active = new(ELeave) CTestActive();
	CleanupStack::PushL(active);

	// Create a change observer. This observer will cause errors in order to verify that 
	// the system handles them properly.
	CLogViewChangeObserverErrorTest* changeObs = CLogViewChangeObserverErrorTest::NewLC();

	// Create a view for the event log  
	CLogViewEvent* view = CLogViewEvent::NewL(aClient, *changeObs);
	CleanupStack::PushL(view);
	
	// Create a filter to specify what events appear in the view
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	TEST(!view->SetFilterL(*filter, active->iStatus));
	 
	TInt count;
	TLogId logId;
	TInt viewIndex;
 
 	// This code adds 3 events with ids 0,1 and 2.
 	// The observer add event handler :
 	// CLogViewChangeObserverErrorTest::HandleLogViewChangeEventAddedL
 	// has been written so that it will leave if an event with log id 1 is added.
 	// The test completion proves that the leave is dealt with gracefully without 
 	// any panics.
	for(count = 0; count < KTestErrorEventNum; count++)
		{
		// Add an event to the log database
		aClient.AddEvent(*event, changeObs->iStatus);
		CLogChangeDefinition* changes = changeObs->WaitForChangesLC(CLogViewChangeObserver::EStopOnBoth);
		
		// Check the results/notifications are as expected
		TEST2(changeObs->iStatus.Int(), KErrNone);
		TEST(event->Id() == (TLogId) count);
		User::After(1);
		TEST2(changes->Count(), 1);
		TEST2(changes->At(0, logId, viewIndex), ELogChangeTypeEventAdded);
		TEST(logId == (TLogId) count);
		TEST2(viewIndex, 0);
		CleanupStack::PopAndDestroy(changes);
		}
 
	CleanupStack::PopAndDestroy(5, event); // filter, view, changeObs, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-3716
@SYMTestCaseDesc	    Tests for the defect INC108225 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the count of out call list & the duplicates for CS & PS calls
						The fields to be considered while finding duplicates records are set in the resource file named LOGWRAP.RSS
						i.e. duplicate = ELogRemotePartyField|ELogContactField|ELogNumberField;
						Considering this duplicate value, the where clause in the query is made to find duplicate records.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 INC108225
*/
LOCAL_C void TestNumberFieldINC108225L(CLogClient& aClient)
{
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-3716 ")); 
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CLogEvent* event1 = CLogEvent::NewL();
	CleanupStack::PushL(event1);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogRecentOutgoingCalls, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	TLogString buf1;
	aClient.GetString(buf1, R_LOG_DIR_OUT);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf1);
	event->SetDurationType(KLogDurationNone);
	event->SetContact(KTestContact1);
	event->SetRemoteParty(KTestRemote1);
	event->SetNumber(KTestNumber1);

	// Add event 1st CS Call with contact as KTestContact & number as KTestNumber1
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	TBool res = view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);
		
	// Set new event with the same contact details (i.e. KTestContact) and 
	//...event type as KLogCallEventTypeUid & no number field
	event1->SetEventType(KLogCallEventTypeUid);
	event1->SetDirection(buf1);
	event1->SetDurationType(KLogDurationNone);
	event1->SetContact(KTestContact1);
	event1->SetRemoteParty(KTestRemote1);
	
	// Add event 1st PS Call with contact as KTestContact1 & Number as Null
	active->StartL();
	aClient.AddEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);
	
	// Add event 2nd CS Call
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);
	
	// Check duplicate count for the last call made i.e. CS call
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(duplicate->CountL(), 1);
		
	// Add event 2nd PS Call
	active->StartL();
	aClient.AddEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);

	// Check duplicate count for the last call made i.e. PS call
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(duplicate->CountL(), 1);
	
	// Add event 3rd CS Call
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);
	
	// Check duplicate count for the last call made i.e. CS call
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(duplicate->CountL(), 2);

	// Add event 3rd PS Call
	active->StartL();
	aClient.AddEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 2);

	// Check duplicate count for the last call made i.e. PS call
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(duplicate->CountL(), 2);
		
	CleanupStack::PopAndDestroy(5);	
}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-3755
@SYMTestCaseDesc	    Tests for the changes made in defect INC108225 for other fields 
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the count of out call list & the duplicates for CS calls referring other fields
						The fields to be considered while finding duplicates records are set in the resource file named LOGWRAP.RSS
						i.e. duplicate = ELogRemotePartyField|ELogContactField|ELogNumberField;
						Considering this duplicate value, the where clause in the query is made to find duplicate records.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 INC108225
*/
LOCAL_C void TestOtherFieldsINC108225L(CLogClient& aClient)
{
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-3755 ")); 
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	// Make sure there are no entries in any recent lists
	active->StartL();
	aClient.ClearLog(KLogRecentOutgoingCalls, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	TLogString buf1;
	aClient.GetString(buf1, R_LOG_DIR_OUT);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf1);
	event->SetContact(KTestContact1);
	event->SetRemoteParty(KTestRemote1);
	event->SetNumber(KTestNumber1);
	event->SetDurationType(KLogDurationNone);

	// Add event 1st CS Call with contact as KTestContact & number as KTestNumber1 &
	//.. durationtype as KLogDurationNone
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	TBool res = view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);
	
	// Add event 2nd CS Call with duration type as KLogDurationValid
	event->SetDurationType(KLogDurationValid);
	
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	active->StartL();
	TEST(view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);
	
	// Check duplicate count for the last call made 
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(duplicate->CountL(), 1);
	
	// Add event 3rd CS Call with Flag as KLogNullFlags	
	event->SetFlags(KLogNullFlags);
	
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);
	
	// Check duplicate count for the last call made 
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(duplicate->CountL(), 2);
	
	// Add event 4th CS Call with Flag as KLogFlagsMask
	event->SetFlags(KLogFlagsMask);
	
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);
	
	// Check duplicate count for the last call made 
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(duplicate->CountL(), 3);

	// Add event 5th CS Call with Status as KNullDesC
	event->SetStatus(KNullDesC);
	
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);
	
	// Check duplicate count for the last call made 
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(duplicate->CountL(), 4);
	
	// Add event 6th CS Call with Status as KTestStatus1
	event->SetStatus(KTestStatus1);
	
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	active->StartL();
	TEST(view->SetRecentListL(KLogRecentOutgoingCalls, active->iStatus));
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(view->CountL(), 1);
	
	// Check duplicate count for the last call made
	active->StartL();
	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	CActiveScheduler::Start();
	TEST2(duplicate->CountL(), 5);
	
	CleanupStack::PopAndDestroy(4);	
}

/**
@SYMTestCaseID          PDS-LOGENG-CT-4050
@SYMTestCaseDesc        Tests for the changes made in defect DEF144552 
@SYMTestPriority        High
@SYMTestActions         Tests for the sucessful completion of the following operation - 
                        1)Applying a filter where the event's start time matches the end time.
                        2)Clearing the log where the clear log time is 1 microsecs after the 
                        timestamp of the event.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 DEF144552
*/
LOCAL_C void DEF144552L(CLogClient& aClient)
    {
    TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-CT-4050 ")); 
    TestUtils::DeleteDatabaseL(EFalse);
    
    CTestActive* active = new(ELeave)CTestActive();
    CleanupStack::PushL(active);

    //Setup the database by adding a event and changing the timestamp for the event
    //Add 1 event
    CLogEvent* event = CLogEvent::NewL();
    CleanupStack::PushL(event);
    event->SetEventType(KLogCallEventTypeUid);
    event->SetContact(KTestContact1);
    event->SetNumber(KTestNumber2);
    event->SetDuration(0);
    active->StartL();
    aClient.AddEvent(*event, active->iStatus);
    CActiveScheduler::Start();
    TEST2(active->iStatus.Int(), KErrNone);
    
    //Change the event
    TTime now;
    now.UniversalTime();
    event->SetTime(now);
    active->StartL();
    aClient.ChangeEvent(*event, active->iStatus);
    CActiveScheduler::Start();
    TEST2(active->iStatus.Int(), KErrNone);
    
    //Test 1: Setup and apply a filter where the start time matches the end time
    //If the defect is fixed then the view will contain the event previously added,
    //Otherwise the time parameters will be truncated and the event will not be found
    
    //Setup filter so that the start and the end time matches
    CLogFilter* filter = CLogFilter::NewL();
    CleanupStack::PushL(filter);
    filter->SetStartTime(now);
    filter->SetEndTime(now);
    
    //Apply the filter and check that 1 event is in the view
    CLogViewEvent* view = CLogViewEvent::NewL(aClient);
    CleanupStack::PushL(view);
    active->StartL();
    TEST(view->SetFilterL(*filter, active->iStatus));
    CActiveScheduler::Start();
    TEST2(active->iStatus.Int(), KErrNone);
    TEST2(view->CountL(), 1);
    
    //Retrieve and check the event
    active->StartL();
    TEST(view->FirstL(active->iStatus));
    CActiveScheduler::Start();
    TEST2(active->iStatus.Int(), KErrNone);
    const CLogEvent& currentEvent(view->Event());
    TEST2(currentEvent.EventType().iUid, KLogCallEventTypeUid.iUid);
    TEST2(currentEvent.Duration(), 0);
    TEST2(currentEvent.Time().Int64(), now.Int64());
    
    // Test 2: Clearing a log where the time specified in ClearLog() is just
    // after the timestamp of the event by 1 microsecond. If the defect is fixed
    // then the event will be deleted. However if the defect is not fixed then the
    // time specified in ClearLog() will be truncated and the event will not be 
    // deleted
    
    //Clear the log
    active->StartL();
    TTimeIntervalMicroSeconds microsecond(1);
    now += microsecond;
    aClient.ClearLog(now, active->iStatus);
    CActiveScheduler::Start();
    TEST2(active->iStatus.Int(), KErrNone);
    
    //Check that the event has been deleted in the view
    CleanupStack::PopAndDestroy(2);
    view = CLogViewEvent::NewL(aClient);
    CleanupStack::PushL(view);
    filter = CLogFilter::NewL();
    CleanupStack::PushL(filter);
    active->StartL();
    TEST(!view->SetFilterL(*filter, active->iStatus));

    CleanupStack::PopAndDestroy(4, active); 
    }

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logview2"));
	TestUtils::DeleteDatabaseL();

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CLogChangeNotifier* notifier = CLogChangeNotifier::NewL();
	CleanupStack::PushL(notifier);	

	TheTest.Start(_L("Event View With Filter List"));
	TestEventViewWithFilterListL(*client);
	theLog.Write(_L8("Test 1 OK\n"));

	TheTest.Next(_L("View change observer"));
	TestViewChangeEventsL(*client);
	theLog.Write(_L8("Test 2 OK\n"));

	TheTest.Next(_L("Recent View With Filter"));
	TestRecentViewWithFilterL(*client);
	theLog.Write(_L8("Test 3 OK\n"));

	TheTest.Next(_L("Recent View With Filter List"));
	TestRecentViewWithFilterListL(*client);
	theLog.Write(_L8("Test 4 OK\n"));

	TheTest.Next(_L("Duplicate View With Filter"));
	TestDuplicateViewWithFilterL(*client);
	theLog.Write(_L8("Test 5 OK\n"));

	TheTest.Next(_L("Duplicate View With Filter List"));
	TestDuplicateViewWithFilterListL(*client);
	theLog.Write(_L8("Test 6 OK\n"));

	TheTest.Next(_L("Querying Event View For Null Fields"));
	TestEventViewForNullFieldsL(*client);
	theLog.Write(_L8("Test 7 OK\n"));

	TheTest.Next(_L("Querying Recent View For Null Fields"));
	TestRecentViewForNullFieldsL(*client);
	theLog.Write(_L8("Test 8 OK\n"));

	TheTest.Next(_L("Querying Duplicate View For Null Fields"));
	TestDuplicateViewForNullFieldsL(*client);
	theLog.Write(_L8("Test 9 OK\n"));

	TheTest.Next(_L("Querying Event View For Flags"));
	TestEventViewForFlagsL(*client);
	theLog.Write(_L8("Test 10 OK\n"));

	TheTest.Next(_L("Querying Recent View For Flags"));
	TestRecentViewForFlagsL(*client);
	theLog.Write(_L8("Test 11 OK\n"));

	TheTest.Next(_L("Querying Duplicate View For Flags"));
	TestDuplicateViewForFlagsL(*client);
	theLog.Write(_L8("Test 12 OK\n"));

	TheTest.Next(_L("Removing Recent Events"));
	TestRemovingRecentEventsL(*client);
	theLog.Write(_L8("Test 13 OK\n"));

	TheTest.Next(_L("Removing Duplicate Events"));
	TestRemovingDuplicateEventsL(*client);
	theLog.Write(_L8("Test 14 OK\n"));

	TheTest.Next(_L("Multiple Views"));
	TestMultipleViews1L(*client);
	TestMultipleViews2L(*client);
	theLog.Write(_L8("Test 15 OK\n"));
	
	TheTest.Next(_L("Test _ALT Events In Recent List")); 
	TestAltEventsInRecentListL(*client);   
	theLog.Write(_L8("Test 16 OK\n"));

	TheTest.Next(_L("Test filtering Events by time")); 
	// This TEST sets system's 'Home Time'
	TestFilterByTimeL(*client);
	theLog.Write(_L8("Test 17 OK\n"));

	TheTest.Next(_L("Test hidden change events")); 
	TestUtils::DeleteDatabaseL();
	TestHiddenChangeEvents1L(*client);
	TestHiddenChangeEvents1aL(*client);
	TestHiddenChangeEvents2L(*client);
	TestHiddenChangeEvents2aL(*client);
	TestHiddenChangeEvents3L(*client);
	TestHiddenChangeEvents3aL(*client);
	TestHiddenChangeEvents4L(*client);
	TestHiddenChangeEvents4aL(*client);
	TestHiddenChangeEvents5L(*client);
	TestHiddenChangeEvents5aL(*client);
	TestHiddenChangeEvents6L(*client);
	TestHiddenChangeEvents6aL(*client);
	TestHiddenChangeEvents7L(*client);
	TestHiddenChangeEvents7aL(*client);
	theLog.Write(_L8("Test 18 OK\n"));

	TheTest.Next(_L("Changing events and recent views"));
	TestChangeEventRecentView1L(*client);
	TestChangeEventRecentView2L(*client);
	TestChangeEventRecentViewAndPurgeL(*client);
	TestChangeEventRecentViewRemoveL(*client);
	theLog.Write(_L8("Test 19 OK\n"));

	TheTest.Next(_L("Test for DEF066296"));
	DEF066296L(*client);
	TheTest.Next(_L("Test for DEF087459"));
	DEF087459L(*client);
	theLog.Write(_L8("Test 20 OK\n"));

	TheTest.Next(_L("Event View With Filter List containing StartTime and EndTime"));
	TestEventViewWithTimestampsInFilterListL(*client);
	theLog.Write(_L8("Test 21 OK\n"));
		
	TheTest.Next(_L("Test for DEF108741 - View change observer, error condition"));
	DEF108741L(*client);
	theLog.Write(_L8("Test 22 OK\n"));
	
	TheTest.Next(_L("Test Defect INC108225: Call history check for the CS & PS(VOIP) calls "));
	TestNumberFieldINC108225L(*client);
	TestOtherFieldsINC108225L(*client);
	theLog.Write(_L8("Test 23 OK\n"));

	TheTest.Next(_L("Test Defect DEF144552: CLogFilter doesn't work when Starttime is the same as EndTime"));
	DEF144552L(*client);
	theLog.Write(_L8("Test 24 OK\n"));
	
	CleanupStack::PopAndDestroy(2); // client, notifier
	}
