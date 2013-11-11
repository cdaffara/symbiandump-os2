// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// test code for INC045441 - Log Engine does not return events in sequence order
// 
//

#include <s32file.h>
#include <e32math.h>
#include <logview.h>
#include "t_logutil2.h"

RTest TheTest(_L("t_logorderbyid"));

_LIT( KTestNumber1, "11111" );
_LIT( KTestNumber2, "22222" );
_LIT( KTestNumber3, "33333" );

/**
Add an event to the log engine database.
The event ID assigned by logengine is store in gTheId 

@param  aClient 
@param  aNumber The number that the event should contain
@return The index for the event added.
  
*/
LOCAL_C TInt AddEventL( CLogClient& aClient, CLogEvent& aEvent, CTestActive& aActive, TInt aIndex )
	{
	TInt returnId = KLogNullId;

	// Reset
	TTime now;
	now.UniversalTime();

	aEvent.SetContact( aIndex );

	// load the event with test values
	switch (aIndex)
		{
	case 1:
		aEvent.SetNumber( KTestNumber1 );
		break;
	case 2:
		aEvent.SetNumber( KTestNumber2 );
		break;
	case 3:
		aEvent.SetNumber( KTestNumber3 );
		break;
		}

	// add the event to the logeng database
	aActive.StartL();
	aClient.AddEvent( aEvent, aActive.iStatus );
	CActiveScheduler::Start();
	TEST2(aActive.iStatus.Int(), KErrNone);

	// check that an ID has been assigned
	returnId = aEvent.Id();
	TEST( returnId != KLogNullId );
	TEST( aEvent.Time() >= now );
	
	// return the event id which has been assigned by the 
	// log engine
	return returnId;
	}


/**
Get the event from the log engine database.
 
@param aClient 
@param aTheId Unique id for the event to be fetch
@param aNumber The number that the event should contain
*/
LOCAL_C void TestGetEventL( CLogClient& aClient, TInt aTheId, TInt aIndex )

	{
	CTestActive* active = new( ELeave )CTestActive();
	CleanupStack::PushL( active );

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL( event );

	event->SetId( aTheId );

	active->StartL();
	aClient.GetEvent( *event, active->iStatus );
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	// check we got the right one back
	TEST( event->Contact() == aIndex );

	TPtrC eventNumber = event->Number();

	TBuf<30> dateString;
	_LIT( KDateString5, "%-B%:0%J%:1%T%:2%S%:3%+B" );
	event->Time().FormatL( dateString, KDateString5 );
	TPtrC eventDate = dateString.Ptr();
	TheTest.Printf( _L( "Id:%d No:%S Time:%S \n" ), event->Id(), &eventNumber, &eventDate );
	
	CleanupStack::PopAndDestroy( 2 ); // event, active
	}

/**
Test code for INC045441 - Log Engine does not return events in sequence order

@SYMTestCaseID          SYSLIB-LOGENG-CT-1020
@SYMTestCaseDesc	    Tests for checking the sequence order on events returned by log engine
@SYMTestPriority 	    High
@SYMTestActions  	    Test for getting the event in order as they were added to the log
                        Check for memory and no error
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/	
LOCAL_C void TestRecentViewOrderingL( CLogClient& aClient )
//
//
//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1020 "));
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	CTestActive* active = new( ELeave )CTestActive();
	CleanupStack::PushL( active );

	CLogViewRecent* view = CLogViewRecent::NewL( aClient );
	CleanupStack::PushL( view );

	TBuf<KLogMaxDirectionLength> buf;
	aClient.GetString(buf, R_LOG_DIR_MISSED);

	event->SetEventType( KLogCallEventTypeUid );
	event->SetDirection( buf );

	TEST( !view->SetRecentListL( KLogRecentMissedCalls, active->iStatus ) );
	TEST( view->CountL() == 0 );

	TTime time;
	TheTest.Next( _L( "add new event 1" ) );
	TInt eventId1 = AddEventL( aClient, *event, *active, 1 );
	TestGetEventL( aClient, eventId1, 1 );

	TheTest.Next( _L( "time plus 10 mins - add new event 2" ) );
	time.HomeTime();
	TTimeIntervalMinutes timeTravelForward( 10 );
	time += timeTravelForward;
	User::SetHomeTime( time );
	TInt eventId2 = AddEventL( aClient, *event, *active, 2 );
	TestGetEventL( aClient, eventId2, 2 );

	TheTest.Next( _L( "time minus 5 mins - add new event 3" ) );
	time.HomeTime();
	TTimeIntervalMinutes timeTravelBackward( 5 );
	time -= timeTravelBackward;
	User::SetHomeTime( time );
	TInt eventId3 = AddEventL( aClient, *event, *active, 3 );
	TestGetEventL( aClient, eventId3, 3 );

	TEST( view->CountL() == 3 );

	active->StartL();
	// Get most recent
	TEST( view->FirstL( active->iStatus ) );
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TInt id3 = view->Event().Id();
	// Get the one before that
	active->StartL();
	TEST( view->NextL( active->iStatus ) );
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TInt id2 = view->Event().Id();

	// Get the one before that
	active->StartL();
	TEST( view->NextL( active->iStatus ) );
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
	TInt id1 = view->Event().Id();

	TEST( id1 == eventId1 );
	TEST( id2 == eventId2 );
	TEST( id3 == eventId3 );

	CleanupStack::PopAndDestroy( 3 ); // view, active, event
	theLog.Write( _L8( "Test 1.1 OK\n" ) );
	}

	
	
void doTestsL()
	{
	TestUtils::Initialize(_L("t_logorderbyid"));
	TestUtils::DeleteDatabaseL();

	CLogClient* client = CLogClient::NewL( theFs );
	CleanupStack::PushL( client );

	CLogChangeNotifier* notifier = CLogChangeNotifier::NewL();
	CleanupStack::PushL( notifier );

	TheTest.Start( _L( "Recent view sorts by Id not ETime" ) );
	TestRecentViewOrderingL( *client );
	theLog.Write( _L8( "Test 1 OK\n" ) );

	CleanupStack::PopAndDestroy( 2 ); // notifier, client;
	}
