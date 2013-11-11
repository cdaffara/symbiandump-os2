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
// t_logcompat.cpp - verification that old databases without SimId column in Event table can be opened
//                   and events added, viewed, etc.  
// 
//
#include <s32file.h>
#include <s32mem.h>
#include <e32math.h>
#include <bautils.h>
#include <logview.h>
#include "t_logutil2.h"

RTest TheTest(_L("t_logcompat"));

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

// These values are stored in "oldLogdb.dat"
_LIT(KEventNum1, "00000");
_LIT(KEventNum2, "11111");
_LIT(KEventNum3, "22222");
_LIT(KEventNum4, "012345678901234567");
const TLogId KEventId1 = 0;
const TLogId KEventId2 = 1;
const TLogId KEventId3 = 2;
const TLogId KEventId4 = 3;

//New events, added by this test
_LIT(KEventNum5, "123411119");
_LIT(KEventNum6, "999911221");
TLogId TheEventId5 = KLogNullId;
TLogId TheEventId6 = KLogNullId;

//Stored events properties
_LIT(KTestEventDesc1, "Event Type Description");
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

//Sim Ids used by the test
const TSimId KSimId1 = 1;
const TSimId KSimId3 = 3900100200U;

void CheckEventProperties(CLogEvent& aEvent, TLogId aEventId, const TDesC& aEventNumber, TSimId aSimId)
	{
	TEST(aEvent.Id() == aEventId);
	TEST(aEvent.Time() > TTime(0));
	TEST(aEvent.Description() == KTestEventDesc1);
	TEST(aEvent.EventType() == KTestEventUid);
	TEST(aEvent.RemoteParty() == KTestRemoteParty1);
	TEST(aEvent.Direction() == KTestDirection1);
	TEST(aEvent.DurationType() == KTestDurationType1);
	TEST(aEvent.Duration() == KTestDuration1);
	TEST(aEvent.Status() == KTestStatus1);
	TEST(aEvent.Subject() == KTestSubject1);
	TEST(aEvent.Contact() == KTestContact1);
	TEST(aEvent.Link() == KTestLink1);
	TEST(aEvent.Data() == KTestData1);
	TEST(aEvent.Flags() == KTestFlags1);
	TEST(aEvent.Number() == aEventNumber);
	TEST(aEvent.SimId() == aSimId);
	TPtrC eventStatus = aEvent.Subject();
	TPtrC eventNumber = aEvent.Number();
	TheTest.Printf(_L("Id:%d No:%S Sub:%S Contact Id:0x%x SimId:%u\n"), aEvent.Id(), &eventNumber, &eventStatus, aEvent.Contact(), aEvent.SimId());
	}

void DoGetEventTestL(TLogId aEventId, const TDesC& aEventNumber, TSimId aSimId, CTestActive& aActive, CLogClient& aLogClient)
	{
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	// set the id of the event to be fetched
	event->SetId(aEventId);

	aActive.StartL();
	aLogClient.GetEvent(*event, aActive.iStatus);
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);
	
	CheckEventProperties(*event, aEventId, aEventNumber, aSimId);
	
	CleanupStack::PopAndDestroy(event);
	}

void DoChangeEventTestL(TLogId aEventId, const TDesC& aEventNumber, TSimId aSimId, CTestActive& aActive, CLogClient& aLogClient)
	{
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	//Retrieve the event.
	event->SetId(aEventId);
	aActive.StartL();
	aLogClient.GetEvent(*event, aActive.iStatus);
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);
	CheckEventProperties(*event, aEventId, aEventNumber, KLogNullSimId);
	//Change event's SimId.
	event->SetSimId(aSimId);
	aActive.StartL();
	aLogClient.ChangeEvent(*event, aActive.iStatus);
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);
	CheckEventProperties(*event, aEventId, aEventNumber, aSimId);
	
	CleanupStack::PopAndDestroy(event);
	}

TInt DoAddEventTestL(const TDesC& aEventNumber, TSimId aSimId, CTestActive& aActive, CLogClient& aLogClient)
	{
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	TTime now;
	now.UniversalTime();
	
	event->SetEventType(KTestEventUid);
	event->SetRemoteParty(KTestRemoteParty1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KTestDurationType1);
	event->SetDuration(KTestDuration1);
	event->SetStatus(KTestStatus1);
	event->SetSubject(KTestSubject1);
	event->SetNumber(aEventNumber);
	event->SetContact(KTestContact1);
	event->SetLink(KTestLink1);
	event->SetDataL(KTestData1);
	event->SetFlags(KTestFlags1);
	if(aSimId != KLogNullSimId)
		{
		event->SetSimId(aSimId);
		}

	aActive.StartL();
	aLogClient.AddEvent(*event, aActive.iStatus);
	CActiveScheduler::Start();
	TEST(!aActive.IsActive());
	TEST2(aActive.iStatus.Int(), KErrNone);
	
	TLogId eventId = event->Id();
	TEST(eventId != KLogNullId);
	TEST(event->Time() >= now);
	
	CleanupStack::PopAndDestroy(event);
	return eventId;
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4018
@SYMTestCaseDesc		Retrieving events from a database without "SimId" column in the "Event" table.
						The test uses the OldLogdbu.dat database, created before introducing the new 
						"SimId" column. The test checks that the events in the database can be accessed and 
						retrieved. The event's SimID property value should be 0.
@SYMTestActions			Retrieving events from a database without "SimId" column in the "Event" table.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12746
*/
void GetEventTestL()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	DoGetEventTestL(KEventId1, KEventNum1, KLogNullSimId, *active, *client);
	DoGetEventTestL(KEventId2, KEventNum2, KLogNullSimId, *active, *client);
	DoGetEventTestL(KEventId3, KEventNum3, KLogNullSimId, *active, *client);
	DoGetEventTestL(KEventId4, KEventNum4, KLogNullSimId, *active, *client);

	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4019
@SYMTestCaseDesc		Changing events from a database without "SimId" column in the "Event" table.
						The test uses the OldLogdbu.dat database, created before introducing the new 
						"SimId" column. The test checks that the events in the database can be accessed and 
						retrieved. After that the test changes some of the events, updating events properties
						with a non-zero SimId. The test checks that the updated event really got its SimId 
						property value changed.
@SYMTestActions			Changing events from a database without "SimId" column in the "Event" table.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12746
*/
void ChangeEventTestL()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	
	//Change two of the events
	DoChangeEventTestL(KEventId1, KEventNum1, KSimId1, *active, *client);
	DoChangeEventTestL(KEventId3, KEventNum3, KSimId3, *active, *client);
	//Check that the other two events are unchanged
	DoGetEventTestL(KEventId2, KEventNum2, KLogNullSimId, *active, *client);
	DoGetEventTestL(KEventId4, KEventNum4, KLogNullSimId, *active, *client);
	
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4020
@SYMTestCaseDesc		Add events to a database without "SimId" column in the "Event" table.
						The test uses the OldLogdbu.dat database, created before introducing the new 
						"SimId" column. The test adds two events to the database, then checks that the
						events can be accessed and retrieved.
@SYMTestActions			Add events to a database without "SimId" column in the "Event" table.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12746
*/
void AddEventTestL()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	
	//Add an event without SimId
	TheEventId5 = DoAddEventTestL(KEventNum5, KLogNullSimId, *active, *client);
	//Add an event with SimId
	TheEventId6 = DoAddEventTestL(KEventNum6, KSimId1, *active, *client);
	//Check that the other 4 events are unchanged
	DoGetEventTestL(KEventId1, KEventNum1, KSimId1, *active, *client);//KSimId1 - set by ChangeEventTestL() 
	DoGetEventTestL(KEventId2, KEventNum2, KLogNullSimId, *active, *client);
	DoGetEventTestL(KEventId3, KEventNum3, KSimId3, *active, *client);//KSimId3 - set by ChangeEventTestL()
	DoGetEventTestL(KEventId4, KEventNum4, KLogNullSimId, *active, *client);
	//Check the first added event
	DoGetEventTestL(TheEventId5, KEventNum5, KLogNullSimId, *active, *client);
	//Check the second added event
	DoGetEventTestL(TheEventId6, KEventNum6, KSimId1, *active, *client);
	
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4021
@SYMTestCaseDesc		Delete events from a database without "SimId" column in the "Event" table.
						The test uses the OldLogdbu.dat database, created before introducing the new 
						"SimId" column. The test deletes an event from the database and checks then that
						the other events are intact, the deleted event is really deleted.
@SYMTestActions			Delete events from a database without "SimId" column in the "Event" table.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12746
*/
void DeleteEventTestL()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	//Delete an event
	active->StartL();
	client->DeleteEvent(KEventId2, active->iStatus);
	CActiveScheduler::Start();
	TEST(!active->IsActive());
	TEST2(active->iStatus.Int(), KErrNone);
	
	//Check that the other 5 events are unchanged
	DoGetEventTestL(KEventId1, KEventNum1, KSimId1, *active, *client);//KSimId1 - set by ChangeEventTestL()
	DoGetEventTestL(KEventId3, KEventNum3, KSimId3, *active, *client);//KSimId3 - set by ChangeEventTestL()
	DoGetEventTestL(KEventId4, KEventNum4, KLogNullSimId, *active, *client);
	DoGetEventTestL(TheEventId5, KEventNum5, KLogNullSimId, *active, *client);
	DoGetEventTestL(TheEventId6, KEventNum6, KSimId1, *active, *client);
	
	//Check that event 2 was really deleted
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetId(KEventId2);
	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);
	
	CleanupStack::PopAndDestroy(event);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4022
@SYMTestCaseDesc		Event View test with an old database (without SimId).
						The test uses the OldLogdbu.dat database, created before introducing the new 
						"SimId" column. The test creates an event view and iterates over the events in the view.
						The test checks that events without SimId property set can be retrieved.
@SYMTestActions			Event View test with an old database (without SimId).
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12747
*/
void EventViewTestL()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewChangeObserver* observer = CLogViewChangeObserver::NewLC();
	observer->SetActive();
	
	CLogViewEvent* view = CLogViewEvent::NewL(*client, *observer);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	
	//SimId:                      KSimId1    KSimId3    KLogNullSimId  KLogNullSimId   KSimId1 
	const TLogId KLogEventIds[] ={KEventId1, KEventId3, KEventId4,  TheEventId5, TheEventId6};
	TBool logEventVisited[] =    {EFalse,    EFalse,    EFalse,     EFalse,      EFalse};
	const TInt KLogEventCnt = sizeof(KLogEventIds)/sizeof(KLogEventIds[0]); 

	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		const TInt viewCount = view->CountL();
		TEST2(viewCount, KLogEventCnt);
		
		TInt eventId = view->Event().Id();
		TInt i = 0;
		for(;i<KLogEventCnt;++i)
			{
			if(KLogEventIds[i] == eventId)
				{
				break;
				}
			}
		TEST(i < KLogEventCnt);
		logEventVisited[i] = ETrue;
		}
	while(view->NextL(active->iStatus));

	//Check that all events were visited. 
	for(TInt i=0;i<KLogEventCnt;++i)
		{
		TEST(logEventVisited[i]);
		}
	
	CleanupStack::PopAndDestroy(filter);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(observer);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4023
@SYMTestCaseDesc		Event View test with an old database (without SimId).
						The test uses the OldLogdbu.dat database, created before introducing the new 
						"SimId" column. The test creates an event view and iterates over the events in the view.
						The test checks that events without SimId property set can be retrieved. The test is very
						similar to the EventViewTestL() test, except the fact that the filter filters out all events
						with non-zero SimId.
@SYMTestActions			Event View test with an old database (without SimId).
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12747
*/
void EventViewTest2L()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewChangeObserver* observer = CLogViewChangeObserver::NewLC();
	observer->SetActive();
	
	CLogViewEvent* view = CLogViewEvent::NewL(*client, *observer);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetNullFields(ELogSimIdField);
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	
	//Only events KEventId4 and TheEventId5 are with zero SimId.
	const TLogId KLogEventIds[] ={KEventId4,  TheEventId5};
	TBool logEventVisited[] =    {EFalse,    EFalse};
	const TInt KLogEventCnt = sizeof(KLogEventIds)/sizeof(KLogEventIds[0]); 

	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		const TInt viewCount = view->CountL();
		TEST2(viewCount, KLogEventCnt);
		
		TInt eventId = view->Event().Id();
		TInt i = 0;
		for(;i<KLogEventCnt;++i)
			{
			if(KLogEventIds[i] == eventId)
				{
				break;
				}
			}
		TEST(i < KLogEventCnt);
		logEventVisited[i] = ETrue;
		}
	while(view->NextL(active->iStatus));

	//Check that all events were visited. 
	for(TInt i=0;i<KLogEventCnt;++i)
		{
		TEST(logEventVisited[i]);
		}
	
	CleanupStack::PopAndDestroy(filter);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(observer);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4024
@SYMTestCaseDesc		Event View test with an old database (without SimId).
						The test uses the OldLogdbu.dat database, created before introducing the new 
						"SimId" column. The test creates an event view, sets a filter with a non-zero SimId 
						and iterates over the events in the view. It should be possible to view events with SimId
						set, when the database was created using the old format.
@SYMTestActions			Event View test with an old database (without SimId).
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12747
*/
void EventViewTest3L()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewChangeObserver* observer = CLogViewChangeObserver::NewLC();
	observer->SetActive();
	
	CLogViewEvent* view = CLogViewEvent::NewL(*client, *observer);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetSimId(KSimId1);
	TSimId simId = filter->SimId();
	TEST2(simId, KSimId1);
	TBool res = view->SetFilterL(*filter, active->iStatus);
	TEST(res);
	
	//Only events KEventId1 and TheEventId6 are with KSimId1.
	const TLogId KLogEventIds[] ={KEventId1,  TheEventId6};
	TBool logEventVisited[] =    {EFalse,    EFalse};
	const TInt KLogEventCnt = sizeof(KLogEventIds)/sizeof(KLogEventIds[0]); 

	do	{
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);

		const TInt viewCount = view->CountL();
		TEST2(viewCount, KLogEventCnt);
		
		TInt eventId = view->Event().Id();
		TInt i = 0;
		for(;i<KLogEventCnt;++i)
			{
			if(KLogEventIds[i] == eventId)
				{
				break;
				}
			}
		TEST(i < KLogEventCnt);
		logEventVisited[i] = ETrue;
		}
	while(view->NextL(active->iStatus));

	//Check that all events were visited. 
	for(TInt i=0;i<KLogEventCnt;++i)
		{
		TEST(logEventVisited[i]);
		}
	
	CleanupStack::PopAndDestroy(filter);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(observer);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4043
@SYMTestCaseDesc		CLogEvent - data compatibility test.
						The test uses data externalized from an event prior the "dual SIM support"
						changes were introduced. The test internalizes the event data and checks 
						that the data is interpreted correctly.
@SYMTestActions			CLogEvent - data compatibility test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12746
*/
void EventDataTestL()
	{
	//Externalized to a buffer event data where the following property values have been set:
	// Id=1, Type=2, "RemoteParty", "Direction", "Data"
	//The used event format is before the "dual SIM support" changes were introduced.
	const TUint8 KEvtData1[] =
		//  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F   
		{ 
		    1,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		  255,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0, 44,
		   82,101,109,111,116,101, 80, 97,114,116,121, 36, 68,105,114,101, 
		   99,116,105,111,110,  0,  0,  0,  4,  0,  0,  0, 18, 68, 97,116,
		   97
		};

	CLogEvent* evt1 = CLogEvent::NewL();
	CleanupStack::PushL(evt1);
	TPtrC8 ptr1(KEvtData1, sizeof(KEvtData1) / sizeof(KEvtData1[0]));
	RDesReadStream strm1(ptr1);
	strm1 >> *evt1;
	strm1.Close();
	TEST2(evt1->Id(), 1);
	TEST(evt1->EventType() == TUid::Uid(2));
	_LIT(KRemoteParty, "RemoteParty");
	TEST(evt1->RemoteParty() == KRemoteParty);
	_LIT(KDirection, "Direction");
	TEST(evt1->Direction() == KDirection);
	_LIT8(KData, "Data");
	TEST(evt1->Data() == KData);
	TEST(evt1->SimId() == KLogNullSimId);
	CleanupStack::PopAndDestroy(evt1);
	
	//Externalized to a buffer event data where the following property values have been set:
	// Id=3, Type=4, "RemoteParty2", "Direction2"
	//The used event format is before the "dual SIM support" changes were introduced.
	const TUint8 KEvtData2[] =
		//  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F   
		{ 
		    3,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		  255,  0,  0,  0,  0,255,255,255,255,  0,  0,  0,  0,  0,  0, 48,
		   82,101,109,111,116,101, 80, 97,114,116,121, 50, 40, 68,105,114, 
		  101, 99,116,105,111,110, 50,  0,  0,  0,  0,  0,  0,  0
		};

	CLogEvent* evt2 = CLogEvent::NewL();
	CleanupStack::PushL(evt2);
	TPtrC8 ptr2(KEvtData2, sizeof(KEvtData2) / sizeof(KEvtData2[0]));
	RDesReadStream strm2(ptr2);
	strm2 >> *evt2;
	strm2.Close();
	TEST2(evt2->Id(), 3);
	TEST(evt2->EventType() == TUid::Uid(4));
	_LIT(KRemoteParty2, "RemoteParty2");
	TEST(evt2->RemoteParty() == KRemoteParty2);
	_LIT(KDirection2, "Direction2");
	TEST(evt2->Direction() == KDirection2);
	TEST(evt2->SimId() == KLogNullSimId);
	CleanupStack::PopAndDestroy(evt2);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4046
@SYMTestCaseDesc		CLogClient::ClearLog() - data compatibility test.
						The test uses the OldLogdbu.dat database, created before introducing the new 
						"SimId" column. The test verifies that CLogClient::ClearLog() can work with 
						database with the old format. 
@SYMTestActions			CLogClient::ClearLog() - data compatibility test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		Medium
@SYMREQ					REQ12748
*/
void ClearLogTestL()
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	//Get one of the events
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetId(KEventId1);
	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TTime evtTime = event->Time();
	CleanupStack::PopAndDestroy(event);
	event = NULL;
	
	//Delete all events with time < (evtTime + 1s). 
	evtTime += TTimeIntervalSeconds(1);
	active->StartL();
	client->ClearLog(evtTime, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	//The event with time "evtTime" should not be in the log anymore.
	event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetId(KEventId1);
	active->StartL();
	client->GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);
	CleanupStack::PopAndDestroy(event);
	
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

void doTestsL()
	{
    TestUtils::Initialize(_L("t_logcompat"));
	
#ifdef _DEBUG	
	TheTest.Start(_L("Copy the old LogEng database"));
	TestUtils::CopyOldDbL();
	//
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4018: \"Get Event\" compatibility test"));
	GetEventTestL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4019: \"Change Event\" compatibility test"));
	ChangeEventTestL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4020: \"Add Event\" compatibility test"));
	AddEventTestL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4021: \"Delete Event\" compatibility test"));
	DeleteEventTestL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4022: \"Event View\" compatibility test"));
	EventViewTestL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4023: \"Event View 2\" compatibility test"));
	EventViewTest2L();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4024: \"Event View 3\" compatibility test"));
	EventViewTest3L();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4043: \"CLogEvent data\" compatibility test"));
	EventDataTestL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4046: \"CLogClient::ClearLog()\" compatibility test"));
	ClearLogTestL();
	//
	TestUtils::DeleteDatabaseL();
#else
	TheTest.Start(_L("This test works only in debug mode, otherwise the LogEng server cannot be stopped. See TestUtils::CopyOldDbL()"));
#endif
	}

#else//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logcompat"));
	TheTest.Start(_L("The LogEng compatibility test cases are compiled only when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM macro is defined!"));
	}

#endif//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM 	
