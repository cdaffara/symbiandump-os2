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
// t_logsimid.cpp - Log event tests using the SimId event property
// 
//
#include <s32file.h>
#include <s32mem.h>
#include <e32math.h>
#include <bautils.h>
#include <logview.h>
#include <logcli.h>
#include "t_logutil2.h"
#include "LogServSqlStrings.h"

RTest TheTest(_L("t_logsimid"));

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

const TLogId KEvId1 = 101;
_LIT(KEvDesc1, "Event Type Description 1");
_LIT(KEvRemote1, "Remote Party 1");

//Event type related
//KLogCallEventType is used in order to use the "recent list" related functionality on the server side
//(KEvDirection1 value also plays important role)
const TUid KEvTypeUid = {KLogCallEventType};
//KEvDirection1 together with the KEvTypeUid guarantee that the properties of events being added match the conditions of one
//of the LogEng server recent lists and thus the "recent list" related functionality on the server side will be used.
_LIT(KEvDirection1, "Missed call");

_LIT(KEvStatus1, "Status 1");
_LIT(KEvSubject1, "Subject 1");
_LIT(KEvNumber1, "11119999");
const TLogDurationType KEvDurationType1 = 1;
const TLogDuration KEvDuration1 = 1000;
const TLogContactItemId KEvContact1 = 1010;
const TLogLink KEvLink1 = 2000;
_LIT8(KEvData1, "ABCDEFGH 1");
const TLogFlags KEvFlags1 = 0x5;
const TSimId KEvSimId1 = 3000000910U;//Bigger than KMaxTInt, to check that SQL statement formatting works as expected

const TInt KDiffEvCount = 10;
const TLogDurationType KEvDurationType2 = 2;
_LIT(KEvNumber2, "22226666");

const TUid KEvTypeUidLoggingDisabled = {0x447422};

//The default max log size is 1000. But the test timeouts on hardware with max log size of 1000.
const TInt KMaxLogSize = 200;
const TInt KMaxPlusEvCount = 100; //Added events count on top of the max log size

TInt TheAddedEventCount = -1;
TInt TheMaxLogSize = -1;

enum TLogViewSimId {ELogWithoutSimId, ELogWithSimId, ELogIgnoreSimId, ELogDiffEvents};

TLogId TheEvId = -1;

void DoTestEvent1(const CLogEvent& aEvent)
	{
	TEST(aEvent.Id() == KEvId1);
	TEST(aEvent.EventType() == KEvTypeUid);
	TEST(aEvent.RemoteParty() == KEvRemote1);
	TEST(aEvent.Direction() == KEvDirection1);
	TEST(aEvent.DurationType() == KEvDurationType1);
	TEST(aEvent.Duration() == KEvDuration1);
	TEST(aEvent.Status() == KEvStatus1);
	TEST(aEvent.Subject() == KEvSubject1);
	TEST(aEvent.Number() == KEvNumber1);
	TEST(aEvent.Contact() == KEvContact1);	
	TEST(aEvent.Link() == KEvLink1);
	TEST(aEvent.Description() == KEvDesc1);
	TEST(aEvent.Flags() == KEvFlags1);
	TEST(aEvent.Data() == KEvData1);
	TEST(aEvent.SimId() == KEvSimId1);
	}

void DoTestFilter1(const CLogFilter& aFilter)
	{
	TEST(aFilter.EventType() == KEvTypeUid);
	TEST(aFilter.RemoteParty() == KEvRemote1);
	TEST(aFilter.Direction() == KEvDirection1);
	TEST(aFilter.DurationType() == KEvDurationType1);
	TEST(aFilter.Status() == KEvStatus1);
	TEST(aFilter.Contact() == KEvContact1);
	TEST(aFilter.Number() == KEvNumber1);
	TEST(aFilter.Flags() == KEvFlags1);
	TEST(aFilter.SimId() == KEvSimId1);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4025
@SYMTestCaseDesc		CLogEvent API  and SimId test.
						The test creates an event and checks that event Copy(), Internalize() and Externalize()
						operations work properly.
@SYMTestActions			CLogEvent API  and SimId test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority 	    Medium
@SYMREQ					REQ12746
*/
void LogEventApiTestL()
	{
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	
	event->SetId(KEvId1);
	event->SetEventType(KEvTypeUid);
	event->SetRemoteParty(KEvRemote1);
	event->SetDirection(KEvDirection1);
	event->SetDurationType(KEvDurationType1);
	event->SetDuration(KEvDuration1);
	event->SetStatus(KEvStatus1);
	event->SetSubject(KEvSubject1);
	event->SetNumber(KEvNumber1);
	event->SetContact(KEvContact1);
	event->SetLink(KEvLink1);
	event->SetDescription(KEvDesc1);
	event->SetFlags(KEvFlags1);
	event->SetDataL(KEvData1);
	event->SetSimId( KEvSimId1);
	
	//Copy event
	CLogEvent* event2 = CLogEvent::NewL();
	CleanupStack::PushL(event2);
	event2->CopyL(*event);
	DoTestEvent1(*event2);
	CleanupStack::PopAndDestroy(event2);

	//Externalize event
	TBuf8<512> buf;
	RDesWriteStream strmOut;
	strmOut.Open(buf);
	event->ExternalizeL(strmOut);
	strmOut.CommitL();
	strmOut.Close();
	
	//Internalize event
	CLogEvent* event3 = CLogEvent::NewL();
	CleanupStack::PushL(event3);
	RDesReadStream strmIn;
	strmIn.Open(buf);
	event3->InternalizeL(strmIn);
	strmIn.Close();
	DoTestEvent1(*event3);
	CleanupStack::PopAndDestroy(event3);

	CleanupStack::PopAndDestroy(event);
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4026
@SYMTestCaseDesc		CLogEvent API  and SimId test.
						The test creates a filter and checks that filter Copy() operation work properly.
@SYMTestActions			CLogEvent API  and SimId test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority 	    Medium
@SYMREQ					REQ12747
*/
void LogFilterApiTestL()
	{
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	filter->SetEventType(KEvTypeUid);
	filter->SetRemoteParty(KEvRemote1);
	filter->SetDirection(KEvDirection1);
	filter->SetDurationType(KEvDurationType1);
	filter->SetStatus(KEvStatus1);
	filter->SetContact(KEvContact1);
	filter->SetNumber(KEvNumber1);
	filter->SetFlags(KEvFlags1);
	filter->SetSimId( KEvSimId1);

	//Copy filter
	CLogFilter* filter2 = CLogFilter::NewL();
	CleanupStack::PushL(filter2);
	filter2->Copy(*filter);
	DoTestFilter1(*filter2);
	CleanupStack::PopAndDestroy(filter2);

	CleanupStack::PopAndDestroy(filter);
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

/**
@SYMTestCaseID			PDS-LOGENG-UT-4027
@SYMTestCaseDesc		Add events test.
						The test adds events to the LogEng database. The events count is bigger than the
						max log size. Half of the events will be with non-zero SimId property. 
						10 of the events will have different DurationType and Number values than the other events.
						Apart from checking that the "add event" opertaion works with the new SimId property, this 
						test case also prepares data for the other "view event" test cases.
@SYMTestActions			Add events test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority 	    Medium
@SYMREQ					REQ12746
*/
void LogAddEventsTestL()
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
	event->SetRemoteParty(KEvRemote1);
	event->SetDirection(KEvDirection1);
	event->SetDurationType(KEvDurationType1);
	event->SetDuration(KEvDuration1);
	event->SetStatus(KEvStatus1);
	event->SetSubject(KEvSubject1);
	event->SetNumber(KEvNumber1);
	event->SetContact(KEvContact1);
	event->SetLink(KEvLink1);
	event->SetDescription(KEvDesc1);
	event->SetFlags(KEvFlags1);
	event->SetDataL(KEvData1);
	
	TTime st_time;
	st_time.UniversalTime();
	
	TheTest.Printf(_L("Added events:\n"));
	TInt diffEvCount = KDiffEvCount;
	for(TInt i=0;i<TheAddedEventCount;++i)
		{
		event->SetSimId(KLogNullSimId);
		event->SetDurationType(KEvDurationType1);
		event->SetNumber(KEvNumber1);
		if((i %2) == 0)
			{
			event->SetSimId(KEvSimId1);
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
		if((i % 50) == 0)
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

void DoSetFilterL(TLogViewSimId aViewSimId, TInt aExpectedEventCount, 
				 CLogViewEvent& aLogView, CLogFilter& aLogFilter, CTestActive& aActive)
	{
	switch(aViewSimId)
		{
		case ELogWithoutSimId:
			aLogFilter.SetNullFields(ELogSimIdField);
			break;
		case ELogWithSimId:
			aLogFilter.SetSimId(KEvSimId1);
			break;
		case ELogDiffEvents:
			aLogFilter.SetSimId(KEvSimId1);
			aLogFilter.SetDurationType(KEvDurationType2);
			aLogFilter.SetNumber(KEvNumber2);
			break;
		case ELogIgnoreSimId:
		default:
			break;
		}
	TBool res = aLogView.SetFilterL(aLogFilter, aActive.iStatus);
	TEST(res);
	aActive.StartL();
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);
	TInt count = aLogView.CountL();
	TheTest.Printf(_L("===Events count: %d\n"), count);
	TEST2(count, aExpectedEventCount);
	}

void DoTestViewEvent(TLogViewSimId aViewSimId, CLogViewEvent& aLogView)
	{
	const CLogEvent& e = aLogView.Event();
	TSimId simid = e.SimId();
	TLogDurationType durationType = e.DurationType();
	const TDesC& number = e.Number();
	switch(aViewSimId)
		{
		case ELogWithoutSimId:
			TEST2(simid, KLogNullSimId);
			break;
		case ELogWithSimId:
			TEST2U(simid, KEvSimId1);
			break;
		case ELogDiffEvents:
			TEST2U(simid, KEvSimId1);
			TEST2(durationType, KEvDurationType2);
			TEST(number == KEvNumber2);
			break;
		case ELogIgnoreSimId:
		default:
			break;
		}
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4028
@SYMTestCaseDesc		View events test.
						The test case expects the LogEng database to be already filled with events.
						Depending what is the value of the aViewSimId, the test will prepare a view and
						set the view filter to filter in events without SimId, events with SimId, all events,
						or the events with different DurationType and Number property values. The purpose
						of the test is to check that the event filtering works as expected with the new SimId
						property. 
@SYMTestActions			View events test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority 	    Medium
@SYMREQ					REQ12746
*/
void LogViewEventsTestL(TLogViewSimId aViewSimId, TInt aExpectedEventCount)
	{
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);
	CLogViewEvent* view = CLogViewEvent::NewL(*client);
	CleanupStack::PushL(view);
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	TTime st_time;
	st_time.UniversalTime();
	DoSetFilterL(aViewSimId, aExpectedEventCount, *view, *filter, *active); 
	TTime end_time;
	end_time.UniversalTime();
	TTimeIntervalMicroSeconds us = end_time.MicroSecondsFrom(st_time);
	TheTest.Printf(_L("SetFilter(). Time: %ld milliseconds\n"), us.Int64() / 1000);
	
	TInt count = 0; 
	st_time.UniversalTime();
	if(view->FirstL(active->iStatus))
		{
		do
			{
			active->StartL();
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			DoTestViewEvent(aViewSimId, *view);
			++count;
			}
		while(view->NextL(active->iStatus));
		}
	end_time.UniversalTime();
	us = end_time.MicroSecondsFrom(st_time);
	TheTest.Printf(_L("Event view walk completed. Events count: %d. Time: %ld milliseconds\n"), count, us.Int64() / 1000);
		
	CleanupStack::PopAndDestroy(filter);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(active);
	CleanupStack::PopAndDestroy(client);
	}

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

void doTestsL()
	{
    TestUtils::Initialize(_L("t_logsimid"));
	TestUtils::DeleteDatabaseL();
	//
	TheTest.Start(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4025: CLogEvent API test"));
	LogEventApiTestL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4026: CLogFilter API test"));
	LogFilterApiTestL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4027: Add events test"));
	LogAddEventsTestL();
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4028: View events without SimId test"));
	LogViewEventsTestL(ELogWithoutSimId, TheMaxLogSize / 2);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4028: View events with SimId test"));
	LogViewEventsTestL(ELogWithSimId, TheMaxLogSize / 2);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4028: View events with or without SimId test"));
	LogViewEventsTestL(ELogIgnoreSimId, TheMaxLogSize);
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4028: More complex event view test"));
	LogViewEventsTestL(ELogDiffEvents, KDiffEvCount);
	//
	TestUtils::DeleteDatabaseL();
	}

#else//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM

void doTestsL()
	{
    TestUtils::Initialize(_L("t_logsimid"));
	TheTest.Start(_L("The t_simid test cases are compiled only when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM macro is defined!"));
	}

#endif//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM
