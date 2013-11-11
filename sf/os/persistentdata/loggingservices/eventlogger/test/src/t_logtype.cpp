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
#include <logview.h>
#include "t_logutil2.h"

RTest TheTest(_L("t_logtype"));

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0830
@SYMTestCaseDesc	    Tests for CLogEventType::Uid(),CLogEventType::Description(),CLogEventType::Copying() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Set a UID and test for retrieving it which identifies the event type.
                        Set a description and test after retrieving it,which gives configuration information related to the event.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestTypeL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0830 "));
	CLogEventType* type1 = CLogEventType::NewL();
	CleanupStack::PushL(type1);

	// Uid
	TEST(type1->Uid() == TUid::Null());

	type1->SetUid(TUid::Uid(0x12345678));
	TEST(type1->Uid() == TUid::Uid(0x12345678));

	type1->SetUid(TUid::Uid(KMaxTInt32));
	TEST(type1->Uid() == TUid::Uid(KMaxTInt32));

	type1->SetUid(TUid::Null());
	TEST(type1->Uid() == TUid::Null());

	// Description
	TEST(type1->Description() == KNullDesC);

	HBufC* buf = TestUtils::CreateBufLC(KLogMaxDescriptionLength / 2);
	type1->SetDescription(buf->Des());
	TEST(type1->Description() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	buf = TestUtils::CreateBufLC(KLogMaxDescriptionLength);
	type1->SetDescription(buf->Des());
	TEST(type1->Description() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	type1->SetDescription(KNullDesC);
	TEST(type1->Description() == KNullDesC);

	TEST(type1->LoggingEnabled());

	type1->SetLoggingEnabled(EFalse);
	TEST(!type1->LoggingEnabled());

	type1->SetLoggingEnabled(ETrue);
	TEST(type1->LoggingEnabled());

	// Copying

	type1->SetUid(TUid::Uid(0x12345678));
	TEST(type1->Uid() == TUid::Uid(0x12345678));

	buf = TestUtils::CreateBufLC(KLogMaxDescriptionLength / 2);
	type1->SetDescription(buf->Des());
	TEST(type1->Description() == buf->Des());
	CleanupStack::PopAndDestroy(); // buf

	type1->SetLoggingEnabled(EFalse);
	TEST(!type1->LoggingEnabled());

	CLogEventType* type2 = CLogEventType::NewL();
	CleanupStack::PushL(type2);
	TEST(!TestUtils::TypesEqual(*type1, *type2));

	type2->Copy(*type1);
	TEST(TestUtils::TypesEqual(*type1, *type2));
	
	CleanupStack::PopAndDestroy(); // type2;

	type2 = CLogEventType::NewL();
	CleanupStack::PushL(type2);
	TEST(!TestUtils::TypesEqual(*type1, *type2));

	type1->Copy(*type2);
	TEST(TestUtils::TypesEqual(*type1, *type2));

	CleanupStack::PopAndDestroy(2); // type1, type2;
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0831
@SYMTestCaseDesc	    Tests for CLogEventType object creation on the heap
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory errors while creating a new object on the heap 
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestTypeWithHeapFailL()
//
//
//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0831 "));
#ifdef _DEBUG
	TInt failCount = 0;
#endif
	TInt error;
	TBool finished = EFalse;

	CLogEventType* type = NULL;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, type = CLogEventType::NewL());
		
		__UHEAP_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;
			delete type;
			}
		else
			TEST2(error, KErrNoMemory);
		}
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-0832
@SYMTestCaseDesc	    Log engine implementation test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for call event,add event,data,fax,SMS,task Scheduler event type to event log.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestInitialEventsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0832 "));
	// Make sure the database is deleted
	TestUtils::DeleteDatabaseL();

	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CLogViewEvent* view = CLogViewEvent::NewL(*client);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// Call event
	filter->SetEventType(KLogCallEventTypeUid);
	event->SetEventType(KLogCallEventTypeUid);

	// Add event
	client->AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check view
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST(view->CountL() > 0);

	// Data event
	filter->SetEventType(KLogDataEventTypeUid);
	event->SetEventType(KLogDataEventTypeUid);

	// Add event
	client->AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check view
	res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST(view->CountL() > 0);

	// Fax event
	filter->SetEventType(KLogFaxEventTypeUid);
	event->SetEventType(KLogFaxEventTypeUid);

	// Add event
	client->AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check view
	res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST(view->CountL() > 0);

	// Sms event
	filter->SetEventType(KLogShortMessageEventTypeUid);
	event->SetEventType(KLogShortMessageEventTypeUid);

	// Add event
	client->AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check view
	res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST(view->CountL() > 0);

	// Task Scheduler event
	filter->SetEventType(KLogTaskSchedulerEventTypeUid);
	event->SetEventType(KLogTaskSchedulerEventTypeUid);

	// Add event
	client->AddEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// Check view
	res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST(view->CountL() > 0);

	CleanupStack::PopAndDestroy(5); // event, filter, view, client, active
	}

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logtype"));
	TheTest.Start(_L("Event Type"));
	TestTypeL();
	theLog.Write(_L8("Test 1 OK\n"));

	TheTest.Next(_L("Event Type with heap failure"));
	TestTypeWithHeapFailL();
	theLog.Write(_L8("Test 2 OK\n"));

	TheTest.Next(_L("Initial events"));
	TestInitialEventsL();
	theLog.Write(_L8("Test 3 OK\n"));
	 
	// tidy up ready for the next test
	TestUtils::DeleteDatabaseL();
	}
