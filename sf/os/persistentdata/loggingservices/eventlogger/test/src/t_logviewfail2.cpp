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

RTest TheTest(_L("t_logviewfail2"));

const TLogContactItemId KTestContact = 0x123;
_LIT(KTestStatus, "Test Status Text");

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0971
@SYMTestCaseDesc	    Tests for CLogViewEvent::SetFilterL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Create a log filter and set the filter on logview.Check for equality of events on the view and events
						added.Check for file failure error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestEventViewSetupWithFileFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0971 "));
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

	TInt failCount = 0;

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->SetFilterL(*filter, active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}
    TheTest.Printf(_L("\r\nTest-1 has succeeded at iteration %d\n"), failCount);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter
	
	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetEventType(KLogCallEventTypeUid);
	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	failCount = 0;
	finished = EFalse;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->SetFilterL(*list, active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}

	list->ResetAndDestroy();
	CleanupStack::PopAndDestroy(4); // list, view, event, active

    TheTest.Printf(_L("\r\nTest-2 has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0972
@SYMTestCaseDesc	    Tests CLogViewRecent::SetRecentListL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Set recent view list first without a filter and later with log filter,check for file failure error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentViewSetupWithFileFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0972 "));
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

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}
    TheTest.Printf(_L("\r\nTest-1 has succeeded at iteration %d\n"), failCount);

	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetStatus(KTestStatus);

	failCount = 0;
	finished = EFalse;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->SetRecentListL(KLogRecentIncomingCalls, *filter, active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}
    TheTest.Printf(_L("\r\nTest-2 has succeeded at iteration %d\n"), failCount);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetEventType(KLogCallEventTypeUid);
	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	failCount = 0;
	finished = EFalse;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->SetRecentListL(KLogRecentIncomingCalls, *list, active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}

	list->ResetAndDestroy();
	CleanupStack::PopAndDestroy(4); // list, view, event, active

    TheTest.Printf(_L("\r\nTest-3 has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0973
@SYMTestCaseDesc	    Tests for CLogViewRecent::DuplicatesL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Set the specified duplicate event view with logview duplicates.
                        Check for file failure errors 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicateViewSetupWithFileFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0973 "));
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

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->DuplicatesL(*duplicate, active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}
    TheTest.Printf(_L("\r\nTest-1 has succeeded at iteration %d\n"), failCount);

	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetStatus(KTestStatus);

	failCount = 0;
	finished = EFalse;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->DuplicatesL(*duplicate, *filter, active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}
    TheTest.Printf(_L("\r\nTest-2 has succeeded at iteration %d\n"), failCount);

	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetEventType(KLogCallEventTypeUid);
	list->AppendL(filter);
	CleanupStack::Pop(); // filter

	failCount = 0;
	finished = EFalse;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->DuplicatesL(*duplicate, *list, active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}

	list->ResetAndDestroy();
	CleanupStack::PopAndDestroy(5); // list, duplicate, view, event, active

    TheTest.Printf(_L("\r\nTest-3 has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0974
@SYMTestCaseDesc	    Tests for CLogViewEvent::NextL(),CLogViewEvent::LastL(),CLogViewEvent::PreviousL(),
                        CLogViewEvent::FirstL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for all the navigation functions in the view.Check for file failure error 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestNavigationWithFileFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0974 "));
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

	TInt failCount = 0;
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

		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->NextL(active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}
    TheTest.Printf(_L("\r\nTest-1 (\"View Next\") has succeeded at iteration %d\n"), failCount);

	failCount = 0;
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

		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->PreviousL(active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}
    TheTest.Printf(_L("\r\nTest-2 (\"View Previous\") has succeeded at iteration %d\n"), failCount);

	failCount = 0;
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

		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->FirstL(active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}
    TheTest.Printf(_L("\r\nTest-3 (\"View First\") has succeeded at iteration %d\n"), failCount);

	failCount = 0;
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

		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TBool result = EFalse;
		TRAP(error, result = view->LastL(active->iStatus));
		
		if (error == KErrNone)
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
				{
				if(active->iStatus.Int() != KErrNoMemory)
					{
					TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
					}
				TEST2(active->iStatus.Int(), KErrNoMemory);
				}
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}

	CleanupStack::PopAndDestroy(4); // filter, view, event, active

    TheTest.Printf(_L("\r\nTest-4 (\"View Last\") has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0975
@SYMTestCaseDesc	    Tests for CLogViewEvent::CountL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Test for getting the number of events in the view.Check for file failure error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestViewCountWithFileFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0975 "));
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

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TInt result = 0;
		TRAP(error, result = view->CountL());
		
		if (error == KErrNone)
			{
			finished = ETrue;
			TEST(result > 0);
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}

		__FILE_RESET;
		}

	CleanupStack::PopAndDestroy(4); // filter, view, event, active

    TheTest.Printf(_L("\r\nThe test (\"View Count\") has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0976
@SYMTestCaseDesc	    Tests for CLogViewRecent::RemoveL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Test for removing the event with the specified unique event ID from the view.Check for file failure error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestRecentViewRemoveWithFileFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0976 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Incoming
	TLogString buf;
	aClient.GetString(buf, R_LOG_DIR_IN);

	event->SetEventType(KLogCallEventTypeUid);
	event->SetDirection(buf);

	// Add events
	event->SetContact(1);
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

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);
		
		TRAP(error, view->RemoveL(view->Event().Id()));

		__FILE_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;

			TEST(view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus));
			active->StartL();
			CActiveScheduler::Start();
			if(active->iStatus.Int() != KErrNone)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
				}
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(count == view->CountL() + 1);
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}
		// This failCount value causes DBMS to return KErrCorrupt. Apparently this
		// can't actually occur in real life, so we skip this value.
		if (failCount > 9)
			{
			finished = ETrue;
			TRAP(error, view->RemoveL(view->Event().Id()));
			TEST2(error, KErrNone);
			}
		}
	CleanupStack::PopAndDestroy(3); // view, event, active
	
    TheTest.Printf(_L("\r\nThe test (\"View Remove\") has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0977
@SYMTestCaseDesc	    Tests for CLogViewDuplicate::RemoveL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Test for removing the event with the specified unique event ID from the view.Check for file failure error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestDuplicateViewRemoveWithFileFailL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0977 "));
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

	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);
		
		if(failCount == 17)
			{
			__FILE_RESET;
			break;
			}
		
		TRAP(error, duplicate->RemoveL(duplicate->Event().Id()));

		__FILE_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;

			TEST(view->DuplicatesL(*duplicate, active->iStatus));
			active->StartL();
			CActiveScheduler::Start();
			if(active->iStatus.Int() != KErrNone)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, active->iStatus.Int());
				}
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(count == duplicate->CountL() + 1);
			}
		else
			{
			if(error != KErrNoMemory)
				{
				TheTest.Printf(_L("\r\nIter.step: %d, error %d\n"), failCount, error);
				}
			TEST2(error, KErrNoMemory);
			}
		}
	CleanupStack::PopAndDestroy(4); // duplicate, view, event, active
    
    TheTest.Printf(_L("\r\nThe test (\"View Remove Duplicates\") has succeeded at iteration %d\n"), failCount);
	}

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logviewfail2"));
	TestUtils::DeleteDatabaseL();

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	TheTest.Start(_L("Event View Setup"));
	TestEventViewSetupWithFileFailL(*client);
    TheTest.Next(_L("Recent View Setup"));
	TestRecentViewSetupWithFileFailL(*client);
    TheTest.Next(_L("Duplicated View Setup"));
	TestDuplicateViewSetupWithFileFailL(*client);
	theLog.Write(_L8("Test 1 OK\n"));

	TheTest.Next(_L("View Navigation"));
	TestNavigationWithFileFailL(*client);
	theLog.Write(_L8("Test 2 OK\n"));

	TheTest.Next(_L("View Count"));
	TestViewCountWithFileFailL(*client);
    TheTest.Next(_L("View Remove"));
	TestRecentViewRemoveWithFileFailL(*client);
    TheTest.Next(_L("View Remove Duplicates"));
	TestDuplicateViewRemoveWithFileFailL(*client);
	theLog.Write(_L8("Test 3 OK\n"));

	CleanupStack::PopAndDestroy(); // client;
	}
