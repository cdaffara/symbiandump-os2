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
#include <logview.h>

RTest TheTest(_L("t_logviewfail"));

const TLogContactItemId KTestContact = 0x123;
_LIT(KTestStatus, "Test Status Text");


/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0961-0001
@SYMTestCaseDesc	    Tests for CLogViewEvent::NewL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Add an event to the client,Create a new CLogViewEvent on heap,if no error check if the view works
                        Check for memory errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventViewConstructWithHeapFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0961-0001 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	// Add an event
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewEvent* view = NULL;

#ifdef _DEBUG
	TInt failCount = 0;
#endif
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, view = CLogViewEvent::NewL(aClient));

		__UHEAP_RESET;
		
		if (!error)
			{
			finished = ETrue;
			CleanupStack::PushL(view);
		
			// Check the view works
			CLogFilter* filter = CLogFilter::NewL();
			CleanupStack::PushL(filter);
			
			TEST(view->SetFilterL(*filter, active->iStatus));
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			TEST(view->CountL() >= 1);
			TEST(TestUtils::EventsEqual(*event, view->Event()));

			CleanupStack::PopAndDestroy(2); // filter, view
			}
		else
			TEST2(error, KErrNoMemory);
		}

	CleanupStack::PopAndDestroy(2); // event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0962-0001
@SYMTestCaseDesc	    Tests for CLogViewRecent::NewL(),CLogViewRecent::SetRecentListL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Create a new view object,if no errors and test of initialising of the view for the specified recent event list. 
                        Check for no memory errors.                         
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentViewConstructWithHeapFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0962-0001 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	// Add an event
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewRecent* view = NULL;

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, view = CLogViewRecent::NewL(aClient));

		__UHEAP_RESET;
		
		if (!error)
			{
			finished = ETrue;
			CleanupStack::PushL(view);
					
			TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			TEST(view->CountL() >= 1);
			TEST(TestUtils::EventsEqual(*event, view->Event()));

			CleanupStack::PopAndDestroy(); // view
			}
		else
			TEST2(error, KErrNoMemory);
		}

	CleanupStack::PopAndDestroy(2); // event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0963-0001
@SYMTestCaseDesc	    Tests for CLogViewDuplicate::NewL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Create a new logviewrecent object,set the recent log view,
                        List out the duplicates of the current event in the recent event list view,
                        Check for no memory errors.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicateViewConstructWithHeapFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0963-0001 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetContact(KTestContact);

	// Add events
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewDuplicate* view = NULL;

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, view = CLogViewDuplicate::NewL(aClient));

		__UHEAP_RESET;
		
		if (!error)
			{
			finished = ETrue;
			CleanupStack::PushL(view);
			
			CLogViewRecent* recent = CLogViewRecent::NewL(aClient);
			CleanupStack::PushL(recent);
			TEST(recent->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			TEST(recent->CountL() >= 1);

			TEST(recent->DuplicatesL(*view, active->iStatus));
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			TEST(view->CountL() >= 1);

			CleanupStack::PopAndDestroy(2); // recent, view
			}
		else
			TEST2(error, KErrNoMemory);
		}

	CleanupStack::PopAndDestroy(2); // event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0964-0001
@SYMTestCaseDesc	    Tests for CLogViewEvent::SetFilterL() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Create a log filter and set the filter on logview.Check for equality of events on the view and events
						added.Check for memory errors.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventViewSetupWithHeapFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0964-0001 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);
	event->SetStatus(KTestStatus);

	// Add an event
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetStatus(KTestStatus);

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->SetFilterL(*filter, active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;		
				
				TEST(view->CountL() >= 1);
				TEST(TestUtils::EventsEqual(*event, view->Event()));
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

	list->AppendL(filter);
	CleanupStack::Pop(); // filter
	
	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetEventType(KLogCallEventTypeUid);
	list->AppendL(filter);
	CleanupStack::Pop(); // filter

#ifdef _DEBUG
	failCount = 0;
#endif

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->SetFilterL(*list, active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;		
				
				TEST(view->CountL() >= 1);
				TEST(TestUtils::EventsEqual(*event, view->Event()));
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

	list->ResetAndDestroy();
	CleanupStack::PopAndDestroy(4); // list, view, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0965-0001
@SYMTestCaseDesc	    Tests for CLogViewRecent::SetRecentListL() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Set recent view list first without a filter and later with log filter,check for memory errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentViewSetupWithHeapFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0965-0001 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetStatus(KTestStatus);

	// Add an event
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;		
				TEST(view->CountL() >= 1);
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetStatus(KTestStatus);

#ifdef _DEBUG
	failCount = 0;
#endif
	finished = EFalse;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;		
				TEST(view->CountL() >= 1);
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetEventType(KLogCallEventTypeUid);
	list->AppendL(filter);
	CleanupStack::Pop(); // filter

#ifdef _DEBUG
	failCount = 0;
#endif
	finished = EFalse;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->SetRecentListL(KLogRecentIncomingCalls, *list, active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;		
				TEST(view->CountL() >= 1);
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

	list->ResetAndDestroy();
	CleanupStack::PopAndDestroy(4); // list, view, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0966
@SYMTestCaseDesc	    Tests for CLogViewRecent::DuplicatesL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Set the specified duplicate event view with logview duplicates.
                        Check for memory errors 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicateViewSetupWithHeapFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0966 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetContact(KTestContact);
	event->SetStatus(KTestStatus);

	// Add events
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->DuplicatesL(*duplicate, active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;		
				TEST(duplicate->CountL() >= 1);
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetStatus(KTestStatus);

#ifdef _DEBUG
	failCount = 0;
#endif
	finished = EFalse;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->DuplicatesL(*duplicate, *filter, active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;		
				TEST(duplicate->CountL() >= 1);
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetEventType(KLogCallEventTypeUid);
	list->AppendL(filter);
	CleanupStack::Pop(); // filter

#ifdef _DEBUG
	failCount = 0;
#endif
	finished = EFalse;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->DuplicatesL(*duplicate, *list, active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;		
				TEST(duplicate->CountL() >= 1);
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

	list->ResetAndDestroy();
	CleanupStack::PopAndDestroy(5); // list, duplicate, view, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0967
@SYMTestCaseDesc	    Tests for CLogViewEvent::NextL(),CLogViewEvent::LastL(),CLogViewEvent::PreviousL(),
                        CLogViewEvent::FirstL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for all the navigation functions in the view. 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestNavigationWithHeapFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0967 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	// Add an event
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		TEST(view->SetFilterL(*filter, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST(view->CountL() > 1);
		event->CopyL(view->Event());

		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->NextL(active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;
				TEST(!TestUtils::EventsEqual(*event, view->Event()));
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

#ifdef _DEBUG
	failCount = 0;
#endif
	finished = EFalse;

	while(!finished)
		{
		TEST(view->SetFilterL(*filter, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST(view->CountL() > 1);

		TEST(view->LastL(active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		event->CopyL(view->Event());

		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->PreviousL(active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;
				TEST(!TestUtils::EventsEqual(*event, view->Event()));
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

#ifdef _DEBUG
	failCount = 0;
#endif
	finished = EFalse;

	while(!finished)
		{
		TEST(view->SetFilterL(*filter, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST(view->CountL() > 1);

		TEST(view->LastL(active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		event->CopyL(view->Event());

		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->FirstL(active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;
				TEST(!TestUtils::EventsEqual(*event, view->Event()));
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

#ifdef _DEBUG
	failCount = 0;
#endif
	finished = EFalse;

	while(!finished)
		{
		TEST(view->SetFilterL(*filter, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST(view->CountL() > 1);

		TEST(view->FirstL(active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		event->CopyL(view->Event());

		__UHEAP_FAILNEXT(failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->LastL(active->iStatus));
		
		if (!error)
			{
			TEST(result);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus.Int() == KErrNone)
				{
				finished = ETrue;
				TEST(!TestUtils::EventsEqual(*event, view->Event()));
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

	CleanupStack::PopAndDestroy(4); // filter, view, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0968
@SYMTestCaseDesc	    Tests for CLogViewEvent::CountL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Test for getting the number of events in the view.Check for memory errors
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewCountWithHeapFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0968 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);

	// Add an event
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	TEST(view->SetFilterL(*filter, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TInt result = 0;
		TRAP(error, result = view->CountL());
		
		if (!error)
			{
			finished = ETrue;
			TEST(result > 0);
			}
		else
			TEST2(error, KErrNoMemory);

		__UHEAP_RESET;
		}

	CleanupStack::PopAndDestroy(4); // filter, view, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0969
@SYMTestCaseDesc	    Tests for CLogViewRecent::RemoveL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Test for removing the event with the specified unique event ID from the view.Check for memory error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentViewRemoveWithHeapFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0969 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetContact(1);

	// Add events
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetContact(2);

	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	event->SetContact(3);

	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TInt count = view->CountL();
	TEST(count > 1);

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, view->RemoveL(view->Event().Id()));

		__UHEAP_RESET;

		if (!error)
			{
			finished = ETrue;

			TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(count == view->CountL() + 1);
			}
		else
			TEST2(error, KErrNoMemory);
		}

#ifdef _DEBUG
	failCount = 0;
#endif
	finished = EFalse;

	while(!finished)
		{
		TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		count = view->CountL();
		TEST(count > 1);

		__UHEAP_FAILNEXT(failCount++);

		TBool ret = EFalse;
		TRAP(error, ret = view->RemoveL(active->iStatus));

		__UHEAP_RESET;

		if (!error)
			{
			TEST(ret);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus == KErrNone)
				{
				TEST(count == view->CountL() + 1);
				finished = ETrue;
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);
		}

	CleanupStack::PopAndDestroy(3); // view, event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0970
@SYMTestCaseDesc	    Tests for CLogViewDuplicate::RemoveL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Test for removing the event with the specified unique event ID from the view.Check for memory error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicateViewRemoveWithHeapFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0970 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);
	event->SetContact(KTestContact);

	// Add events
	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	aClient.AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(aClient);
	CleanupStack::PushL(duplicate);

	TEST(view->DuplicatesL(*duplicate, active->iStatus));
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	TInt count = duplicate->CountL();
	TEST(count > 0);

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, duplicate->RemoveL(duplicate->Event().Id()));

		__UHEAP_RESET;

		if (!error)
			{
			finished = ETrue;

			TEST(view->DuplicatesL(*duplicate, active->iStatus));
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(count == duplicate->CountL() + 1);
			}
		else
			TEST2(error, KErrNoMemory);
		}

#ifdef _DEBUG
	failCount = 0;
#endif
	finished = EFalse;

	while(!finished)
		{
		TEST(view->DuplicatesL(*duplicate, active->iStatus));
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		count = duplicate->CountL();
		TEST(count > 1);

		__UHEAP_FAILNEXT(failCount++);

		TBool ret = EFalse;
		TRAP(error, ret = duplicate->RemoveL(active->iStatus));

		__UHEAP_RESET;

		if (!error)
			{
			TEST(ret);

			active->StartL();
			CActiveScheduler::Start();

			if (active->iStatus == KErrNone)
				{
				finished = ETrue;
				TEST(count == duplicate->CountL() + 1);
				}
			else
				TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		else
			TEST2(error, KErrNoMemory);
		}

	CleanupStack::PopAndDestroy(4); // duplicate, view, event, active
	}

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logviewfail"));
	TestUtils::DeleteDatabaseL();

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	TheTest.Start(_L("Heap Failure in Client"));

	TheTest.Next(_L("View Construction"));
	TestEventViewConstructWithHeapFailL(*client);
	TestRecentViewConstructWithHeapFailL(*client);
	TestDuplicateViewConstructWithHeapFailL(*client);
	theLog.Write(_L8("Test 1 OK\n"));

	TheTest.Next(_L("View Setup"));
	TestEventViewSetupWithHeapFailL(*client);
	TestRecentViewSetupWithHeapFailL(*client);
	TestDuplicateViewSetupWithHeapFailL(*client);
	theLog.Write(_L8("Test 2 OK\n"));

	TheTest.Next(_L("View Navigation"));
	TestNavigationWithHeapFailL(*client);
	theLog.Write(_L8("Test 3 OK\n"));

	TheTest.Next(_L("Other"));
	TestViewCountWithHeapFailL(*client);
	TestRecentViewRemoveWithHeapFailL(*client);
	TestDuplicateViewRemoveWithHeapFailL(*client);
	theLog.Write(_L8("Test 4 OK\n"));

	CleanupStack::PopAndDestroy(); // client;
	}
