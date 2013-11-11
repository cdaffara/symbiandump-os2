// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// t_logapi2.cpp - LogEng API tests 
// 
//
#include <s32file.h>
#include <s32mem.h>
#include <e32math.h>
#include <bautils.h>
#include <logview.h>
#include "t_logutil2.h"
#include "LogServSqlStrings.h"

RTest TheTest(_L("t_logapi2"));

_LIT(KTestRemoteParty1, "Remote Party");
_LIT(KTestDirection1, "Direction");
_LIT(KTestStatus1, "Status");
_LIT(KTestSubject1, "Subject");
const TUid KTestEventUid = {0x10005393};
const TLogDurationType KTestDurationType1 = 1;
const TLogDuration KTestDuration1 = 0x1234;
const TLogContactItemId KTestContact1 = 0x1234;
const TLogLink KTestLink1 = 0x1234;
_LIT8(KTestData1, "ABCDEFGH");
const TLogFlags KTestFlags1 = 0x5;
_LIT(KEvNumber1, "11119999");
_LIT(KEvNumber2, "22226666");
const TLogDurationType KEvDurationType1 = 1;
const TLogDurationType KEvDurationType2 = 2;
const TLogContactItemId KEvContact1 = 1010;
const TLogLink KEvLink1 = 2000;
_LIT(KEvDesc1, "Event Type Description 1");

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
const TSimId KEvSimId1 = 3000000910U;//Bigger than KMaxTInt, to check that SQL statement formatting works as expected
const TSimId KEvSimId2 = 2911;
#endif

const TInt KDiffEvCount = 10;

//KEvDirection2 used together with KEvTypeUid guarantees that the properties of the event being changed
//match the conditions of one the LogEng server recent lists and thus the "recent list" related functionality 
//on the server side will be used.
_LIT(KEvDirection2, "Incoming");

const TUid KEvTypeUid = {KLogCallEventType};
const TUid KEvTypeUidLoggingDisabled = {0x447422};

enum TLogAddEventOps {ELogOpAddEvent, ELogOpAddEventInvalidType, ELogOpAddEventLoggingDisabled};
enum TLogGetEventOps {ELogOpGetEvent, ELogOpGetEventZeroId, ELogOpGetEventNotExists};

TLogId TheEvId = -1;
TInt TheAddedEventCount = -1;
TInt TheMaxLogSize = -1;

//The default max log size is 1000. But the test timeouts on hardware with max log size of 1000.
const TInt KMaxLogSize = 200;
const TInt KMaxRecentLogSize = 10; // Default recent log size is 20
const TInt KMaxPlusEvCount = 100; //Added events count on top of the max log size

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Add a new event type with "logging disabled"
void DoAddEventTypeL(CLogClient& aClient, CTestActive& aActive)
	{
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KEvTypeUidLoggingDisabled);
	type->SetDescription(_L("110111011011"));
	type->SetLoggingEnabled(EFalse);

	aClient.AddEventType(*type, aActive.iStatus);
	aActive.StartL();
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);

	CleanupStack::PopAndDestroy(type);
	}

void DoSetNewMaxLogSizeL(CLogClient& aClient, CTestActive& aActive)
	{
	//Check what is the max log size.
	TLogConfig config;
	aActive.StartL();
	aClient.GetConfig(config, aActive.iStatus);
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);
	//Set new max log size
	config.iMaxLogSize = KMaxLogSize;
	aActive.StartL();
	aClient.ChangeConfig(config, aActive.iStatus);
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);
	//Test the new max log size
	aActive.StartL();
	aClient.GetConfig(config, aActive.iStatus);
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);
	TEST2(config.iMaxLogSize, KMaxLogSize);
	//
	TheMaxLogSize = config.iMaxLogSize;
	TheAddedEventCount = config.iMaxLogSize + KMaxPlusEvCount;
	}

void DoAddEventsL()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	
	//Set new max log size
	DoSetNewMaxLogSizeL(*client, *active);

	//Add TheAddedEventCount events. Some of them with SimId, some - not.
	//This certainly will involve some log purging operations when the log size reaches the max.
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(KLogCallEventTypeUid);
	event->SetRemoteParty(KTestRemoteParty1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KEvDurationType1);
	event->SetDuration(KTestDuration1);
	event->SetStatus(KTestStatus1);
	event->SetSubject(KTestSubject1);
	event->SetNumber(KEvNumber1);
	event->SetContact(KEvContact1);
	event->SetLink(KEvLink1);
	event->SetDescription(KEvDesc1);
	event->SetFlags(KTestFlags1);
	event->SetDataL(KTestData1);
	
	TTime st_time;
	st_time.UniversalTime();
	
	TheTest.Printf(_L("Added events:\n"));
	TInt diffEvCount = KDiffEvCount;
	for(TInt i=0;i<TheAddedEventCount;++i)
		{
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		event->SetSimId(KLogNullSimId);
#endif		
		event->SetDurationType(KEvDurationType1);
		event->SetNumber(KEvNumber1);
		if((i %2) == 0)
			{
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
			event->SetSimId(KEvSimId1);
#endif			
			if((i > TheMaxLogSize / 2 + 1) && (i % 10) == 0 && --diffEvCount >= 0)
				{//Add after the second half because when the log grows above the max log size, the first events will be deleted
				event->SetDurationType(KEvDurationType2);
				event->SetNumber(KEvNumber2);
				}
			}
		active->StartL();
		client->AddEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST(!active->IsActive());
		TEST2(active->iStatus.Int(), KErrNone);
		if((i % 50) == 0 && i > 0)
			{
			TheTest.Printf(_L("%d\r"), i);
			}
		}
	TheTest.Printf(_L("%d\n"), TheAddedEventCount);
	
	TTime end_time;
	end_time.UniversalTime();
	
	CleanupStack::PopAndDestroy(event);
	
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	
	TTimeIntervalMicroSeconds us = end_time.MicroSecondsFrom(st_time);
	TheTest.Printf(_L("%d events added. Time: %ld milliseconds\n"), TheAddedEventCount, us.Int64() / 1000);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
@SYMTestCaseID			PDS-LOGENG-UT-4042
@SYMTestCaseDesc		"Cancel operation" test.
						The test uses RLogTestSession to establish a connection with the EventViewer server.
						The the test initializes an "Add Event" opertaion.
						But instead of completing the operation the test cancels it. 
@SYMTestActions			"Cancel operation" test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12747
*/
void CancelOperationTest()
	{
	RLogTestSession sess;
	TInt err = sess.Connect();
	TEST2(err, KErrNone);
	
	CLogEvent* event = NULL;
	TRAP(err, event = CLogEvent::NewL());
	TEST2(err, KErrNone);
	event->SetEventType(KTestEventUid);
	event->SetRemoteParty(KTestRemoteParty1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KTestDurationType1);
	event->SetDuration(KTestDuration1);
	event->SetStatus(KTestStatus1);
	event->SetSubject(KTestSubject1);
	event->SetContact(KTestContact1);
	event->SetLink(KTestLink1);
	TRAP(err, event->SetDataL(KTestData1));
	TEST2(err, KErrNone);
	event->SetFlags(KTestFlags1);
	
	TBuf8<1024> buf;
	RDesWriteStream strm(buf);
	TRAP(err, event->ExternalizeL(strm));
	TEST2(err, KErrNone);
	TRAP(err, strm.CommitL());
	TEST2(err, KErrNone);
	
	TPckgBuf<TLogClientServerData> data;
	data().iOperationType = ELogOperationEventAdd;
	data().iOperationId = KLogNullOperationId;
	
	TRequestStatus stat;
	TIpcArgs args1(&data, &buf);
	sess.Send(ELogOperationInitiate, args1, stat);
	TEST2(stat.Int(), KRequestPending);
	TIpcArgs args2(&data);
	err = sess.Send(ELogOperationCancel, args2);
	TEST2(err, KErrCancel);
	User::WaitForRequest(stat);
	err = stat.Int();
	TEST2(err, KErrCancel);
	
	strm.Close();
	delete event;
	sess.Close();
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4029
@SYMTestCaseDesc		AddEvent() server side OOM test.
						This test uses the results from the previous test cases that
						the LogEng database contains TheMaxLogSize events.
						This ensures that the "purge main" functionality on the server side 
						will be used.
						The properties of the event being added are such (KEvDirection1 and KEvTypeUid) that 
						one of the LogServ recent lists will be used. This ensures that the "purge recent"
						LogEng server functionality will be used.
						The test, as any other of the LogEng server OOM tests, does not fully cover the used on the
						server side functions, because:
						1) CLogAddEvent will set the iEventAdded flag right after adding the event to the database.
						   After that, any OOM failure that occurs for example in "purge recent" related calls, will be
						   ignored.
						2) CLogEvent will use the CLogDuplicate active object for the "purge recent" action.
						   When CLogDuplicate fails with KErrNoMemory in DoRunL(), the error is ignored and not reported
						   back.
@SYMTestActions			AddEvent() server side OOM test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12746
*/
void AddEventOomTestL(TLogAddEventOps aLogAddEventOp, TInt aExpectedError)
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	if(aLogAddEventOp == ELogOpAddEventLoggingDisabled)
		{
		DoAddEventTypeL(*client, *active);
		}
	
	TTime now;
	now.UniversalTime();

	TUid eventType = KEvTypeUid;
	if(aLogAddEventOp == ELogOpAddEventInvalidType)
		{
		eventType = TUid::Uid(0x445972);
		}
	else if(aLogAddEventOp == ELogOpAddEventLoggingDisabled)
		{
		eventType = KEvTypeUidLoggingDisabled;
		}

	event->SetEventType(eventType);
	event->SetRemoteParty(KTestRemoteParty1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KEvDurationType1);
	event->SetDuration(KTestDuration1);
	event->SetStatus(KTestStatus1);
	event->SetSubject(KTestSubject1);
	event->SetNumber(KEvNumber1);
	event->SetContact(KEvContact1);
	event->SetLink(KEvLink1);
	event->SetDescription(KEvDesc1);
	event->SetFlags(KTestFlags1);
	event->SetDataL(KTestData1);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	event->SetSimId(KEvSimId1);
#endif	
	
	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->AddEvent(*event, active->iStatus);
		active->StartL();
		CActiveScheduler::Start();

		if(active->iStatus.Int() == aExpectedError)
			{
			finished = ETrue;
			}
		
		__LOGSERV_UHEAP_RESET;

		if(active->iStatus.Int() != aExpectedError)
			{
			TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		}

	TEST2(active->iStatus.Int(), aExpectedError);
	if(aLogAddEventOp == ELogOpAddEvent)
		{
		TEST(event->Time() >= now);
		active->StartL();
		client->GetEvent(*event, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TheEvId = event->Id();
		TEST(TheEvId > 0);
		}

	CleanupStack::PopAndDestroy(event);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	
	TheTest.Printf(_L("===CLogClient::AddEvent() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4030
@SYMTestCaseDesc		ChangeEvent() server side OOM test.
						This test uses the results from the previous test cases that
						the LogEng database contains TheMaxLogSize events.
						This ensures that the "purge main" functionality on the server side 
						will be used.
						The properties of the event being changed are such (KEvDirection2 and KEvTypeUid) that 
						one of the LogServ recent lists will be used. This ensures that the "purge recent"
						LogEng server functionality will be used.
@SYMTestActions			ChangeEvent() server side OOM test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12746
*/
void ChangeEventOomTestL(TBool aEventExists)
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	
	event->SetId(TheEvId);
	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(TheEvId, event->Id());
	event->SetDirection(KEvDirection2);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	event->SetSimId(KEvSimId2);
#endif	
	
	TInt failCount = 0;
	TBool finished = EFalse;

	if(!aEventExists)
		{//Search for a non-existing event in the OOM loop
		event->SetId(TheEvId + 5678);
		}
	
	while(!finished)
		{
		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->ChangeEvent(*event, active->iStatus);
		active->StartL();
		CActiveScheduler::Start();

		TInt err = active->iStatus.Int();
		if(err == (aEventExists ? KErrNone : KErrNotFound))
			{
			finished = ETrue;
			}
		
		__LOGSERV_UHEAP_RESET;

		if(err != (aEventExists ? KErrNone : KErrNotFound))
			{
			TEST2(err, KErrNoMemory);
			}
		}

	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	if(aEventExists)
		{
		TEST2(active->iStatus.Int(), KErrNone);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		TEST2(event->SimId(), KEvSimId2);
#endif		
		}
	else
		{
		TEST2(active->iStatus.Int(), KErrNotFound);
		}

	CleanupStack::PopAndDestroy(event);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	
	TheTest.Printf(_L("===CLogClient::ChangeEvent() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4031
@SYMTestCaseDesc		DeleteEvent() server side OOM test.
						This test uses the results from the previous test cases that
						the LogEng database contains TheMaxLogSize events.
						This ensures that the "purge main" functionality on the server side 
						will be used.
@SYMTestActions			DeleteEvent() server side OOM test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12746
*/
void DeleteEventOomTestL(TBool aEventExists)
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	
	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->DeleteEvent(TheEvId, active->iStatus);
		active->StartL();
		CActiveScheduler::Start();

		TInt err = active->iStatus.Int();
		if(err == (aEventExists ? KErrNone : KErrNotFound))
			{
			finished = ETrue;
			}
		
		__LOGSERV_UHEAP_RESET;

		if(err != (aEventExists ? KErrNone : KErrNotFound))
			{
			TEST2(err, KErrNoMemory);
			}
		}

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetId(TheEvId);
	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);
	CleanupStack::PopAndDestroy(event);
	
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	
	TheTest.Printf(_L("===CLogClient::DeleteEvent() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4032
@SYMTestCaseDesc		GetEvent() server side OOM test.
						This test uses the results from the previous test cases that
						the LogEng database contains TheMaxLogSize events.
						GetEvent() is executed in OOM loop.
@SYMTestActions			GetEvent() server side OOM test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12746
*/
void GetEventOomTestL(TLogGetEventOps aGetEventOp, TInt aExpectedError)
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	
	event->SetId(aGetEventOp == ELogOpGetEvent ? TheEvId : (aGetEventOp == ELogOpGetEventNotExists ? 0x932271F : 0x0));
	
	TInt failCount = 0;
	TBool finished = EFalse;

	while(!finished)
		{
		__LOGSERV_UHEAP_FAILNEXT(++failCount);
		client->GetEvent(*event, active->iStatus);
		active->StartL();
		CActiveScheduler::Start();

		if(active->iStatus.Int() == aExpectedError)
			{
			finished = ETrue;
			}
		
		__LOGSERV_UHEAP_RESET;

		if(active->iStatus.Int() != aExpectedError)
			{
			TEST2(active->iStatus.Int(), KErrNoMemory);
			}
		}
	
	TEST2(active->iStatus.Int(), aExpectedError);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	if(aGetEventOp == ELogOpGetEvent)
		{
		TEST2(event->SimId(), KEvSimId2);
		}
#endif
	
	CleanupStack::PopAndDestroy(event);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	
	TheTest.Printf(_L("===CLogClient::GetEvent() server side OOM test succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4033
@SYMTestCaseDesc		Change Event Type test.
						The test attempts to change the type of an existing event.
						The attempt must fails with KErrPermissionDenied error. 
@SYMTestActions			Change Event Type test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12746
*/
void ChangeStandardEventTypeTestL()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	
	CLogEventType* evType = CLogEventType::NewL(); 
	CleanupStack::PushL(evType);
	TUid uid = {KLogCallEventType};
	evType->SetUid(uid);
	evType->SetDescription(_L("aaaaaaaa"));

	active->StartL();
	client->ChangeEventType(*evType, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrPermissionDenied);
	
	CleanupStack::PopAndDestroy(evType);
	
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

CLogFilter* DoCreateFilter1LC()
	{
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	filter->SetSimId(KEvSimId2);
#endif	
	filter->SetDurationType(KEvDurationType2);
	filter->SetNumber(KEvNumber2);
	filter->SetStatus(KTestStatus1);
	filter->SetDirection(KTestDirection1);
	
	return filter;
	}

CLogFilter* DoCreateFilter2LC()
	{
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	filter->SetSimId(KEvSimId1);
#endif	
	filter->SetDurationType(KEvDurationType1);
	filter->SetNumber(KEvNumber1);
	filter->SetStatus(KTestStatus1);
	filter->SetDirection(KTestDirection1);
	filter->SetEventType(KEvTypeUid);
	filter->SetRemoteParty(KTestRemoteParty1);
	filter->SetNullFields(ELogFlagsField);

	return filter;
	}

CLogFilter* DoCreateFilter3LC()
	{
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	filter->SetSimId(KEvSimId1);
#endif	
	filter->SetDurationType(KEvDurationType1);
	filter->SetNumber(KEvNumber1);
	filter->SetStatus(KTestStatus1);
	filter->SetDirection(KTestDirection1);
	filter->SetEventType(KEvTypeUid);
	filter->SetRemoteParty(KTestRemoteParty1);
	filter->SetContact(KEvContact1);
	filter->SetNullFields(ELogFlagsField);

	return filter;
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4034
@SYMTestCaseDesc		Complex view filter set test.
						The test creates a set of filters using all event properties and duplicating some of them
						in the filters. Then sets the set of filters to a view and iterates over the view's events.
						The idea is to test as much as possible the server side filter processing in TLogFilterExprBuilder
						class implementation.
@SYMTestActions			Complex view filter set test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12746
*/
void ComplexFilterSetTestL()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	CLogViewEvent* view = CLogViewEvent::NewL(*client);
	CleanupStack::PushL(view);
	CLogFilter* filter1 = DoCreateFilter1LC();
	CLogFilter* filter2 = DoCreateFilter2LC();
	CLogFilter* filter3 = DoCreateFilter3LC();
	CLogFilterList* filterList = new(ELeave)CLogFilterList;
	CleanupStack::PushL(filterList);
	filterList->AppendL(filter1);
	filterList->AppendL(filter2);
	filterList->AppendL(filter3);
	
	//Check the "copy filter list" operations
	CLogFilterList* filterList2 = filterList->CopyL();
	TEST2(filterList2->Count(), filterList->Count());
	delete filterList2;
	CLogFilterList* filterList3 = filterList->CopyLC();
	TEST2(filterList3->Count(), filterList->Count());
	CleanupStack::PopAndDestroy(filterList3);
	
	TTime st_time;
	st_time.UniversalTime();
	TBool res = view->SetFilterL(*filterList, active->iStatus);
	TEST(res);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TInt count = view->CountL();
	TheTest.Printf(_L("===Events count: %d\n"), count);
	TTime end_time;
	end_time.UniversalTime();
	TTimeIntervalMicroSeconds us = end_time.MicroSecondsFrom(st_time);
	TheTest.Printf(_L("SetFilter(). Time: %ld milliseconds\n"), us.Int64() / 1000);

	st_time.UniversalTime();
	if(view->FirstL(active->iStatus))
		{
		count = 0; 
		do
			{
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			++count;
			}
		while(view->NextL(active->iStatus));
		}
	end_time.UniversalTime();
	us = end_time.MicroSecondsFrom(st_time);
	TheTest.Printf(_L("Event view walk completed. Events count: %d. Time: %ld milliseconds\n"), count, us.Int64() / 1000);
		
	CleanupStack::PopAndDestroy(filterList);
	CleanupStack::PopAndDestroy(filter3);
	CleanupStack::PopAndDestroy(filter2);
	CleanupStack::PopAndDestroy(filter1);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

#ifdef _DEBUG
static void StopLogServerL()
    {
    static RLogTestSession logServSession;
    //this function doesn't have any effect on UREL builds as LogEng server doesn't 
    //support the transient mode in UREL builds 
    //Used for LogEng server side heap failure testing.
    TInt error = KErrNone;
      
    if(!logServSession.Handle())
        {
        error = logServSession.Connect();
        }
      
    // Is the server running?
    if(error == KErrNotFound)
        {
        return;
        }
    LEAVE_IF_ERROR(error);
      
    // Make the server transient
    TInt p0 = 1;
    TIpcArgs  ipcArgs(p0);
    LEAVE_IF_ERROR(logServSession.Send(ELogMakeTransient, ipcArgs));
      
    logServSession.Close();
      
    User::After(6 * 1000000); // Enough time for the server to exit
    }
#else//_DEBUG
static void StopLogServerL()
    {
    TheTest.Printf(_L("StopLogServerL(): the LogEng server cannot be stopped in release mode. ELogMakeTransient is a debug message.\n"));
    }
#endif//_DEBUG

/**
@SYMTestCaseID          PDS-LOGENG-UT-4047
@SYMTestCaseDesc        Configuration value update test.
                        The test changes the configuration values & thne stops the log server. Start the 
                        server again & check if the configuration values are updated. 
                        Again restores the original value back. 
@SYMTestActions         Configuration value update test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority        Medium
@SYMREQ                 DEF142142
*/
void DoCheckUpdateConfigL()
    {
 
    CLogClient* client = CLogClient::NewL(theFs);
    CleanupStack::PushL(client);
    CTestActive* active = new(ELeave)CTestActive();
    CleanupStack::PushL(active);

    //Check for updated configurations.
    TLogConfig config;
    active->StartL();
    client->GetConfig(config, active->iStatus);
    CActiveScheduler::Start();
    TEST2(active->iStatus.Int(), KErrNone);
    
    //Set new max log size
    TLogSize tmpMaxLogSize = config.iMaxLogSize;
    config.iMaxLogSize = KMaxLogSize;
    config.iMaxRecentLogSize = KMaxRecentLogSize;
    active->StartL();
    client->ChangeConfig(config, active->iStatus);
    CActiveScheduler::Start();
    TEST2(active->iStatus.Int(), KErrNone);
    
    //Stop the Server
    StopLogServerL();
    
    //Test the new max log size
    active->StartL();
    client->GetConfig(config, active->iStatus);
    CActiveScheduler::Start();
    TEST2(active->iStatus.Int(), KErrNone);
    TEST2(config.iMaxLogSize, KMaxLogSize);
    
    //Set original max log size
    config.iMaxLogSize = tmpMaxLogSize;
    active->StartL();
    client->ChangeConfig(config, active->iStatus);
    CActiveScheduler::Start();
    TEST2(active->iStatus.Int(), KErrNone);
    
    CleanupStack::PopAndDestroy(active);
    CleanupStack::PopAndDestroy(client);
    }

//////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef SYSLIBS_TEST

TLogId DoAddEvent2TestRecentListL(CLogClient& aClient, CTestActive& aActive)
	{
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	
	event->SetEventType(KLogCallEventTypeUid);
	event->SetRemoteParty(KLogRctTListRemoteParty);
	event->SetDirection(KLogRctTListDirection);
	event->SetDurationType(KLogRctTListDurationType);
	event->SetDuration(KTestDuration1);
	event->SetStatus(KLogRctTListStatus);
	event->SetSubject(KTestSubject1);
	event->SetNumber(KLogRctTListNumber);
	event->SetContact(KEvContact1);
	event->SetLink(KEvLink1);
	event->SetDescription(KEvDesc1);
	event->SetFlags(KLogRctTListFlags);
	event->SetDataL(KTestData1);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	event->SetSimId(KLogRctTListSimId);
#endif	
	
	aActive.StartL();
	aClient.AddEvent(*event, aActive.iStatus);
	CActiveScheduler::Start();
	TEST(!aActive.IsActive());
	TEST2(aActive.iStatus.Int(), KErrNone);
	TLogId logId = event->Id();
	
	//Get the just added event
	aActive.StartL();
	aClient.GetEvent(*event, aActive.iStatus);
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);
	
	CleanupStack::PopAndDestroy(event);
	
	return logId;
	}

TLogId DoAddEvent2TestRecentListL()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogId logId = DoAddEvent2TestRecentListL(*client, *active);

	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	
	return logId;
	}

struct TLogViewTestObserver : public MLogViewChangeObserver
	{
	TLogViewTestObserver() : 
		iIdAdded(-1),
		iIdChanged(-1),
		iIdDeleted(-1)
		{
		}
	virtual void HandleLogViewChangeEventAddedL(TLogId aId, TInt, TInt, TInt)
		{
		iIdAdded = aId;
		}
	virtual void HandleLogViewChangeEventChangedL(TLogId aId, TInt, TInt, TInt)
		{
		iIdChanged = aId;
		}
	virtual void HandleLogViewChangeEventDeletedL(TLogId aId, TInt, TInt, TInt)
		{
		iIdDeleted = aId;
		}
	TLogId iIdAdded;
	TLogId iIdChanged;
	TLogId iIdDeleted;
	};
	
void DoViewEventFromTestRecentListL(TLogId aId1, TLogId aId2)
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	CLogViewRecent* view = CLogViewRecent::NewL(*client);
	CleanupStack::PushL(view);
	TLogViewTestObserver observer;
	CLogViewDuplicate* duplicate = CLogViewDuplicate::NewL(*client, observer);
	CleanupStack::PushL(duplicate);
	
	TBool rc = view->SetRecentListL(KLogRctTListId, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	TInt count = 0; 
	if(view->FirstL(active->iStatus))
		{
		do
			{
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			const CLogEvent& e = view->Event();
			TEST2(aId2, e.Id());
			++count;
			}
		while(view->NextL(active->iStatus));
		}
	TEST2(count, 1);//The second event is a duplicate => not in the list
		
	//Check duplicates 
	active->StartL();
	rc = view->DuplicatesL(*duplicate, active->iStatus);
	TEST(rc);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(duplicate->CountL(), 1);
	
	//Test the duplicated event
	rc = duplicate->FirstL(active->iStatus);
	TEST(rc);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	const CLogEvent& e2 = duplicate->Event();
	TEST2(aId1, e2.Id());

	//Get the event
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetId(aId1);
	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	
	//Change the event. The observer should register the change. 
	event->SetDataL(_L8("123444555"));
	client->ChangeEvent(*event, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TEST2(observer.iIdChanged, event->Id());

	//Add  a new event to the recent list. This operation should be detected as a "delete" one by the observer 
	//and the existing event should be cleared from the recent list.
	TLogId logId = DoAddEvent2TestRecentListL(*client, *active);
	TEST2(observer.iIdDeleted, aId1);
	TEST(logId != observer.iIdDeleted);
	//No added events detected by the observer
	TEST2(observer.iIdAdded, -1);
	
	//Cleanup
	CleanupStack::PopAndDestroy(event);
	CleanupStack::PopAndDestroy(duplicate);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4035
@SYMTestCaseDesc		Complex recent list test.
						The test works only if SYSLIBS_TEST macro is defined, in which case the server side will
						create a recent list with a lot of fields.
						The test case will add 2 events with the same properties, such that both will be added
						to that test recent list. Because the second event is a match of the first one and matches 
						the recent list "duplicates" mask, the second event will be marked as a duplicate of the
						first one. After adding the events a view is used to check for their presence.
@SYMTestActions			Complex recent list test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12747
*/
void ComplexRecentListTestL()
	{
	TLogId logId1 = DoAddEvent2TestRecentListL();
	TLogId logId2 = DoAddEvent2TestRecentListL();
	DoViewEventFromTestRecentListL(logId1, logId2);
	}

#endif//SYSLIBS_TEST
//////////////////////////////////////////////////////////////////////////////////////////////////////

void doTestsL()
	{
    TestUtils::Initialize(_L("t_logapi2"));
	TestUtils::DeleteDatabaseL();
	//
	TheTest.Start(_L("Preparation. Adding 200 events..."));
	DoAddEventsL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4029: AddEvent() server side OOM test when logged events count is max (200)"));
	AddEventOomTestL(ELogOpAddEvent, KErrNone);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4029: AddEvent() server side OOM test when logged events count is max (200). Invalid event type."));
	AddEventOomTestL(ELogOpAddEventInvalidType, KErrNotFound);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4030: AddEvent() server side OOM test when logged events count is max (200). Logging disabled."));
	AddEventOomTestL(ELogOpAddEventLoggingDisabled, KErrNotSupported);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4030: ChangeEvent() server side OOM test when logged events count is max (200). The event does exist."));
	ChangeEventOomTestL(ETrue);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4030: ChangeEvent() server side OOM test when logged events count is max (200). The event does not exist."));
	ChangeEventOomTestL(EFalse);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4032: GetEvent() server side OOM test when logged events count is max (200)."));
	GetEventOomTestL(ELogOpGetEvent, KErrNone);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4032: GetEvent() server side OOM test when logged events count is max (200). The event does not exist."));
	GetEventOomTestL(ELogOpGetEventNotExists, KErrNotFound);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4032: GetEvent() server side OOM test when logged events count is max (200). The event id is 0."));
	GetEventOomTestL(ELogOpGetEventZeroId, KErrNotFound);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4031: DeleteEvent() server side OOM test when logged events count is max (200). The event does exist."));
	DeleteEventOomTestL(ETrue);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4031: DeleteEvent() server side OOM test when logged events count is max (200). The event does not exist."));
	DeleteEventOomTestL(EFalse);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4033: Attempt to change one of the standard event types."));
	ChangeStandardEventTypeTestL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4042: \"Cancel Operation\" test"));
	CancelOperationTest();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4034: Complex filter set test."));
	ComplexFilterSetTestL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4047: Check for updated configuration values"));
	DoCheckUpdateConfigL();
#ifdef SYSLIBS_TEST
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4035: Complex recent list test."));
	ComplexRecentListTestL();
#endif
	//
	TestUtils::DeleteDatabaseL();
	}
