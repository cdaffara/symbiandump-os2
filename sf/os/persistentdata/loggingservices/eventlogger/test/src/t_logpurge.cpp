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
#include "t_logutil2.h"
#include <logview.h>

RTest TheTest(_L("t_logpurge"));

const TInt KTestEventNum = 10;
const TInt KTestEventAge = 5;
const TInt KTestRecentNum = 10;
const TInt KTestDuplicateNum = 10;

_LIT(KTestRemoteParty, "Test Remote Party %d");

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0875
@SYMTestCaseDesc	    Tests for maximum logging of configuration data 
@SYMTestPriority 	    High
@SYMTestActions  	    Get the event type configuration data.Set the log size to maximum.
                        Change the log engine configuration data with the new one.Add events to the log
                        Set the filter on the view and check for KErrNone flag.
						Add a new event and disable logging by setting the maximum logging size to zero
						Clear all the events and test for KErrNone and the total count of events.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMaxLogSizeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0875 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	TLogConfig config;

	// Get log configuration
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Set the maximum log size
	config.iMaxLogSize = KTestEventNum;

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Initialise the view - There should be no events
	TEST(!view->SetFilterL(*filter, active->iStatus));

	// Add the number of allowed events
	TInt count;
	for(count = 0; count < KTestEventNum; count++)
		{
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	TEST(view->SetFilterL(*filter, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// The view should now have the correct number of events
	TEST(view->CountL() == KTestEventNum);

	// Add the same number of events again - the old ones should be deleted
	for(count = 0; count < KTestEventNum; count++)
		{
		TEST(view->SetFilterL(*filter, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Get the last (oldest) event
		active->StartL();
		view->LastL(active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Remember the Id
		TLogId id = view->Event().Id();

		// Add another event - the oldest should be removed
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// There should be the same number of events in view
		TEST(view->CountL() == KTestEventNum);
		
		event->SetId(id);

		// Try and get the old event
		active->StartL();
		aClient.GetEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNotFound);
		}

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check it's there
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Set the maximum log size to zero, i.e. disable logging
	config.iMaxLogSize = 0;

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check the event has gone
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);;

	// Add an event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotSupported);

	// Check that event doesn't exist
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);;

	User::After(1000000);

	TTime now;
	now.UniversalTime();
	now+=(TTimeIntervalDays) 1;
	// Clear all the events
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(!view->SetFilterL(*filter, active->iStatus));
	TEST(view->CountL() == 0);

	CleanupStack::PopAndDestroy(4); // filter, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0876
@SYMTestCaseDesc	    Tests for maximum logging of configuration data 
@SYMTestPriority 	    High
@SYMTestActions  	    Get the event type configuration data.Set the log size to maximum.
                        Change the log engine configuration data and add the number of allowed events
                        Reduce the number of allowed events.The old ones should be purged
						Change the log engine config and check for the event count.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMaxLogSizeConfigL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0876 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	TLogConfig config;

	// Get log configuration
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Set the maximum log size
	config.iMaxLogSize = KTestEventNum;

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Initialise the view - There should be no events
	TEST(!view->SetFilterL(*filter, active->iStatus));

	// Add the number of allowed events
	TInt count;
	for(count = 0; count < KTestEventNum; count++)
		{
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	TEST(view->SetFilterL(*filter, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// The view should now have the correct number of events
	TEST(view->CountL() == KTestEventNum);

	// Reduce the number of allowed events
	// The old ones should be purged
	config.iMaxLogSize = KTestEventNum / 2;

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check the event count
	TEST(view->SetFilterL(*filter, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST(view->CountL() == KTestEventNum / 2);
		
	User::After(0x1000000);

	TTime now;
	now.UniversalTime();
	now+=(TTimeIntervalDays )1;
	// Clear all the events
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(!view->SetFilterL(*filter, active->iStatus));
	TEST(view->CountL() == 0);

	CleanupStack::PopAndDestroy(4); // filter, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0877
@SYMTestCaseDesc	    Tests for maximum time for which events can be retained in the log
@SYMTestPriority 	    High
@SYMTestActions  	    Get the event type configuration data.Set maximum log age.
                        Change the log engine configuration data with the new one.Add events to the log
                        Set the date and time of events clear the log and check for errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMaxLogAgeL(CLogClient& aClient, TLogAge aMaxLogAge)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0877 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	TLogConfig config;

	// Get log configuration
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Set the maximum log age
	config.iMaxLogSize = KTestEventAge * 2;
	config.iMaxEventAge = aMaxLogAge;

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Initialise the view - There should be no events
	TEST(!view->SetFilterL(*filter, active->iStatus));

	TTime date;
	date.UniversalTime();

	// Wait a second
	User::After(0x1000000);

	// Add events
	TInt count;
	for(count = 0; count < KTestEventAge * 2; count++)
		{
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Set the time and date of the event
		event->SetTime(date);
		date -= TTimeIntervalDays(1);

		active->StartL();
		aClient.ChangeEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		User::After(1000000);

		TEST(view->SetFilterL(*filter, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Check the old events have been removed
		if ((count < KTestEventAge) || !aMaxLogAge)
			TEST(view->CountL() == count + 1);
		else
			TEST(view->CountL() == KTestEventAge);
		}

	User::After(0x1000000);
	date.UniversalTime();

	date+=(TTimeIntervalYears )1;

	// Clear all the events
	active->StartL();
	aClient.ClearLog(date, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(!view->SetFilterL(*filter, active->iStatus));
	TEST(view->CountL() == 0);

	CleanupStack::PopAndDestroy(4); // filter, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0878
@SYMTestCaseDesc	    Tests for maximum number of events that a recent event list holds
@SYMTestPriority 	    High
@SYMTestActions  	    Set the event configuration data of recent log size(10) to maximum and 
						Add events to the log.Set the filter on the view and check for NO error.
						Add a new event and disable logging by setting the maximum logging size to zero
						Clear all the events and test for no errors and the total count of events and the logviewrecent should be zero.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMaxRecentSize1L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0878 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Incoming
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	TLogConfig config;

	// Get log configuration
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Set the maximum log size
	config.iMaxLogSize = KTestRecentNum * 2;
	config.iMaxRecentLogSize = KTestRecentNum;

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Initialise the views - There should be no events
	TEST(!view->SetFilterL(*filter, active->iStatus));
	TEST(!recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));

	// Add a number of events
	TInt count;
	for(count = 0; count < KTestRecentNum; count++)
		{
		event->SetContact(count);

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(recent->CountL() == KTestRecentNum);

	// Add the same number of events again - the old ones should be removed
	for(count = 0; count < KTestRecentNum; count++)
		{
		// Add another event - the oldest should be removed from recent list
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST(view->SetFilterL(*filter, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Check an event has been removed from recent view
		TEST(recent->CountL() == KTestRecentNum);
		TEST(view->CountL() == count + KTestRecentNum + 1);
		}

	User::After(0x1000000);

	TTime now;
	now.UniversalTime();
	now+=(TTimeIntervalDays )1;

	// Clear all the events
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(!view->SetFilterL(*filter, active->iStatus));
	TEST(!recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	TEST(view->CountL() == 0 && recent->CountL() == 0);

	CleanupStack::PopAndDestroy(5); // filter, recent, view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0879
@SYMTestCaseDesc	    Tests for maximum number of events that a recent event list holds
@SYMTestPriority 	    High
@SYMTestActions  	    Set the event configuration data of recent log size(10) to maximum and 
						Add events to the log.Set the filter on the view and check for NO error.
	                    Reduce the recent log size(5)  and set the new configuration.
						Clear all the events and test for no errors and the total count of events and the logviewrecent should be zero.
						Tests for CLogViewRecent::SetRecentListL
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMaxRecentSizeConfigL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0879 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Incoming
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent);

	TLogConfig config;

	// Get log configuration
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Set the maximum log size
	config.iMaxLogSize = KTestRecentNum * 2;
	config.iMaxRecentLogSize = KTestRecentNum;

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Initialise the views - There should be no events
	TEST(!recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));

	// Add a number of events
	TInt count;
	for(count = 0; count < KTestRecentNum; count++)
		{
		event->SetContact(count);

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		}

	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(recent->CountL() == KTestRecentNum);

	// Reduce the maximum allowed recent list size
	// The oldest ones should be removed
	config.iMaxRecentLogSize = KTestRecentNum / 2;

	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(recent->CountL() == KTestRecentNum / 2);
	User::After(0x1000000);

	TTime now;
	now.UniversalTime();
	now+=(TTimeIntervalDays )1;

	// Clear all the events
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(!recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	TEST(recent->CountL() == 0);

	CleanupStack::PopAndDestroy(3); // recent, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0880
@SYMTestCaseDesc	    Tests for CLogViewRecent::SetRecentListL(),CLogViewRecent::DuplicatesL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Set the event configuration data of recent log size(10) to maximum and 
						Add events to the log.Set the filter on the view and check for NO error.
	                    Reduce the recent log size(5)  and set the new configuration.
						Clear all the events and test for no errors and the total count of events and the logviewrecent should be zero.
						Tests for CLogViewRecent::SetRecentListL()
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestMaxRecentSize2L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0880 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Incoming
	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* recent = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(recent);

	CLogViewDuplicate* dup = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(dup);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	TLogConfig config;

	// Get log configuration
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Set the maximum log size
	config.iMaxLogSize = (KTestRecentNum * KTestDuplicateNum) * 2;
	config.iMaxRecentLogSize = KTestRecentNum;

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Initialise the view - There should be no events
	TEST(!recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));

	// Add a number of events
	TInt count;
	for(count = 0; count < KTestRecentNum; count++)
		{
		TBuf<KLogMaxRemotePartyLength> buf;
		buf.Format(KTestRemoteParty, count);
		event->SetRemoteParty(buf);

		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Add some duplicates
		TInt duplicate;
		for(duplicate = 0; duplicate < KTestDuplicateNum; duplicate++)
			{
			active->StartL();
			aClient.AddEvent(*event, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}

		// The views should now have the correct number of events
		TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST(recent->CountL() == count + 1);

		TEST(recent->DuplicatesL(*dup, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST(dup->CountL() == KTestDuplicateNum);
		}

	event->SetRemoteParty(KNullDesC);

	// Add the more events - the old ones should be removed
	for(count = 0; count < KTestRecentNum; count++)
		{
		// Add another event - the oldest should be removed from recent list
		active->StartL();
		aClient.AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		// Check an event has been removed from recent view
		TEST(recent->CountL() == KTestRecentNum);
		}

	User::After(0x1000000);

	TTime now;
	now.UniversalTime();
	now+=(TTimeIntervalDays )1;

	// Clear all the events
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(!recent->DuplicatesL(*dup, active->iStatus));

	CleanupStack::PopAndDestroy(5); // filter, dup, recent, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0881
@SYMTestCaseDesc	    Tests for purge
@SYMTestPriority 	    High
@SYMTestActions  	    Get the event type data 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestNoPurgeWithGetL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0881 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig config;

	// Get log configuration
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Set the maximum log age
	TInt oldAge = config.iMaxEventAge;
	config.iMaxEventAge = 24 * 60 * 60;

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

	// Check that the event can still be retrieved
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Perform a dummy change
	event->SetTime(event->Time() - TTimeIntervalDays(2));
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check the event has been removed
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);;

	// Reset the config
	config.iMaxEventAge = oldAge;

	// Change the log engine config
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(2); // active, event	
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0882
@SYMTestCaseDesc	    Tests for CLogClient::ClearLog() function
@SYMTestPriority 	    High
@SYMTestActions  	    Change locale settings and log duration to 1 day and test for number of event types in log 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestClearLog1L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0882 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	// change Locale
	TLocale locale;
	locale.SetCountryCode(47);//Norway
	locale.SetDateFormat(EDateEuropean);
	locale.SetTimeFormat(ETime12);
	for (int i=0; i<4; i++)
         {
         locale.SetTimeSeparator(TChar('.'),i);
         locale.SetDateSeparator(TChar(':'),i);
         }
	locale.Set();

	// change the log duration settings to 1 day  
	TLogConfig config;
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	config.iMaxLogSize = KTestEventAge * 2;
	config.iMaxEventAge = 86400;
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// add a call event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	User::After(1000000);
	TTime now;
	now.HomeTime();	
	event->SetTime(now);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// forward two days
	now+=(TTimeIntervalDays )2;
	User::SetHomeTime(now);
		
	// dummy call ensures ClearLog() is called
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// try to retrieve event
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);;

	TEST(view->CountL() == 0);

	CleanupStack::PopAndDestroy(3); // view, active, event
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0883
@SYMTestCaseDesc	    Tests for CLogClient::ClearLog() function
@SYMTestPriority 	    High
@SYMTestActions  	    Change locale settings,call up ClearLog and try to retrieve event,test for count of number of events in the view.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestClearLog2L(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0883 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	// change Locale
	TLocale locale;
	locale.SetCountryCode(47);//Norway
	locale.SetDateFormat(EDateEuropean);
	locale.SetTimeFormat(ETime12);
	for (int i=0; i<4; i++)
         {
         locale.SetTimeSeparator(TChar('.'),i);
         locale.SetDateSeparator(TChar(':'),i);
         }
	locale.Set();

	// change the log duration settings to 1 day  
	TLogConfig config;
	active->StartL();
	aClient.GetConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	config.iMaxLogSize = KTestEventAge * 2;
	config.iMaxEventAge = 86400;
	active->StartL();
	aClient.ChangeConfig(config, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// add a call event
	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	User::After(1000000);
	TTime now;
	now.HomeTime();	
	event->SetTime(now);
	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// forward two days
	now+=(TTimeIntervalDays )2;
	User::SetHomeTime(now);
		
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// try to retrieve event
	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);;

	TEST(view->CountL() == 0);

	CleanupStack::PopAndDestroy(3); // view, active, event
	}

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

/**
@SYMTestCaseID			PDS-LOGENG-UT-4036
@SYMTestCaseDesc		Clear log events with specific SimId test.
						The test adds 3 events with different SimIds and then checks that
						CLogEvent::ClearLog() deletes only the event with the specified id.
@SYMTestActions			Clear log events with specific SimId test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12748
*/
void ClearLogSimIdL(CLogClient& aClient)
	{//This test case is compiled only when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM macro is defined.
	const TSimId KSimId1 = 4100000000U;
	const TSimId KSimId2 = 100;
	const TSimId KSimId3 = 1678;
	
	TTime now;
	now.UniversalTime();
	
	TDateTime dt(now.DateTime());
	dt.SetHour(dt.Hour() - 1);
	TTime date(dt);
	
	TTime threshold(date);
	threshold += TTimeIntervalSeconds(10);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	//////// Event1 ///////////////////////////
	CLogEvent* event1 = CLogEvent::NewL();
	CleanupStack::PushL(event1);
	event1->SetEventType(KLogCallEventTypeUid);
	event1->SetSimId(KSimId1);

	active->StartL();
	aClient.AddEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event1->SetTime(date);

	active->StartL();
	aClient.ChangeEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//////// Event2 ///////////////////////////
	CLogEvent* event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);
	event2->SetEventType(KLogCallEventTypeUid);
	event2->SetSimId(KSimId2);

	active->StartL();
	aClient.AddEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event2->SetTime(date);

	active->StartL();
	aClient.ChangeEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//////// Event3 ///////////////////////////
	CLogEvent* event3 = CLogEvent::NewL();
	CleanupStack::PushL(event3);
	event3->SetEventType(KLogCallEventTypeUid);
	event3->SetSimId(KSimId3);

	active->StartL();
	aClient.AddEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event3->SetTime(date);

	active->StartL();
	aClient.ChangeEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Delete event3 /////////////////////////
	aClient.ClearLog(threshold, KSimId3, active->iStatus);
	active->StartL();	
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//Event1 and event2 should be there
	active->StartL();
	aClient.GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	active->StartL();
	aClient.GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	active->StartL();
	aClient.GetEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);
	
	//Delete event2 /////////////////////////
	aClient.ClearLog(threshold, KSimId2, active->iStatus);
	active->StartL();	
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//Event1 should be there
	active->StartL();
	aClient.GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	active->StartL();
	aClient.GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);
	active->StartL();
	aClient.GetEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);
	
	//Delete event1 /////////////////////////
	aClient.ClearLog(threshold, KSimId1, active->iStatus);
	active->StartL();	
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//All events deleted
	active->StartL();
	aClient.GetEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);
	active->StartL();
	aClient.GetEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);
	active->StartL();
	aClient.GetEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(4); //event3, event2, event1, active
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4037
@SYMTestCaseDesc		Clear log events from a recent list with specific SimId test.
						The test adds 3 events to a recent list with different SimIds and then checks that
						CLogEvent::ClearLog() deletes only the event with the specified id.
@SYMTestActions			Clear log events from a recent list with specific SimId test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority 	    High
@SYMREQ					REQ12748
*/
void ClearLogRecentSimIdL(CLogClient& aClient)
	{//This test case is compiled only when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM macro is defined.
	const TSimId KSimId1 = 4200110000U;
	const TSimId KSimId2 = 38223;
	const TSimId KSimId3 = 239816;
	
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
	event1->SetSimId(KSimId1);
	active->StartL();
	aClient.AddEvent(*event1, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//////// Event2 ///////////////////////////
	CLogEvent* event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);
	event2->SetEventType(KEvTypeUid);
	event2->SetDirection(KEvDirection);
	event2->SetNumber(_L("87654321"));
	event2->SetSimId(KSimId2);
	active->StartL();
	aClient.AddEvent(*event2, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//////// Event3 ///////////////////////////
	CLogEvent* event3 = CLogEvent::NewL();
	CleanupStack::PushL(event3);
	event3->SetEventType(KEvTypeUid);
	event3->SetDirection(KEvDirection);
	event3->SetNumber(_L("99229922"));
	event3->SetSimId(KSimId3);
	active->StartL();
	aClient.AddEvent(*event3, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	//Delete event3 /////////////////////////
	aClient.ClearLog(KLogRecentMissedCalls, KSimId3, active->iStatus);
	active->StartL();	
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//Event1 and event2 should be there
	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);
	TBool rc = view->SetRecentListL(KLogRecentMissedCalls, active->iStatus);
	TEST(rc);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TInt count = view->CountL();
	TEST2(count, 2);
	rc = view->FirstL(active->iStatus);
	TEST(rc);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	const CLogEvent& e1 = view->Event();
	TEST(e1.SimId() == KSimId2 || e1.SimId() == KSimId1);
	rc = view->NextL(active->iStatus);
	TEST(rc);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	const CLogEvent& e2 = view->Event();
	TEST(e2.SimId() == KSimId2 || e2.SimId() == KSimId1);
	TEST(e1.Id() != e2.Id());

	CleanupStack::PopAndDestroy(view);
	
	//Delete event1 /////////////////////////
	aClient.ClearLog(KLogRecentMissedCalls, KSimId1, active->iStatus);
	active->StartL();	
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	//Only event2 should be there
	view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);
	rc = view->SetRecentListL(KLogRecentMissedCalls, active->iStatus);
	TEST(rc);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	count = view->CountL();
	TEST2(count, 1);
	rc = view->FirstL(active->iStatus);
	TEST(rc);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	const CLogEvent& e3 = view->Event();
	TEST(e3.SimId() == KSimId2);

	CleanupStack::PopAndDestroy(5); //view, event3, event2, event1, active
	}

#endif//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logpurge"));
	TestUtils::DeleteDatabaseL();

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	TheTest.Start(_L("Maximum Log Size"));
	TestMaxLogSizeL(*client);
	theLog.Write(_L8("Test 1 OK\n"));

	TheTest.Next(_L("Purge Log When Config Changed"));
	TestMaxLogSizeConfigL(*client);
	theLog.Write(_L8("Test 2 OK\n"));

	TheTest.Next(_L("Test purge by Maximum Log Age enabled/disabled"));
	TestMaxLogAgeL(*client, 0);	// disable purging by age
	TestMaxLogAgeL(*client, KTestEventAge * 60 * 60 * 24);  
	theLog.Write(_L8("Test 3 OK\n"));

	TheTest.Next(_L("Maximum Recent List Size"));
	TestMaxRecentSize1L(*client);
	theLog.Write(_L8("Test 4 OK\n"));

	TheTest.Next(_L("Purge Recent Lists When Config Changed"));
	TestMaxRecentSizeConfigL(*client);
	theLog.Write(_L8("Test 5 OK\n"));

	TheTest.Next(_L("Maximum Recent List Size With Duplicates"));
	TestMaxRecentSize2L(*client);
	theLog.Write(_L8("Test 6 OK\n"));

	TheTest.Next(_L("Check no purge when retrieving event"));
	TestNoPurgeWithGetL(*client);
	theLog.Write(_L8("Test 7 OK\n"));

	TheTest.Next(_L("Check ClearLog works for different locales"));
	TestClearLog1L(*client);
	TestClearLog2L(*client);
 	theLog.Write(_L8("Test 8 OK\n"));

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4036 CLogClient::ClearLog() + SimId test"));
 	ClearLogSimIdL(*client);
 	theLog.Write(_L8("Test 9 OK\n"));

	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4037 CLogClient::ClearLog()/recent + SimId test"));
 	ClearLogRecentSimIdL(*client);
 	theLog.Write(_L8("Test 10 OK\n"));
#endif 	
 	
	CleanupStack::PopAndDestroy(); // client;
	}
