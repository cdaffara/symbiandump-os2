
// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <logcntdef.h>
#include "t_logutil2.h"
#include <logview.h>

//LOWCAP defined in t_loglowcapability.mmp file.
#ifdef LOWCAP
    TBool TheHiCapability = EFalse;
    RTest TheTest(_L("t_loglowcapability"));
    _LIT(KTestTitle, "t_loglowcapability");
#else
    TBool TheHiCapability = ETrue;
    RTest TheTest(_L("t_loghicapability"));
    _LIT(KTestTitle, "t_loghicapability");
#endif

const TUid KTestEventUid = {0x10005393};
_LIT(KTestEventDesc1, "Event Type Description");
_LIT(KTestEventDesc2, "Changed Event Description");
_LIT(KTestRemoteParty1, "Remote Party");
_LIT(KTestDirection1, "Direction");
const TLogDurationType KTestDurationType1 = 1;
const TLogDuration KTestDuration1 = 0x1234;
_LIT(KTestStatus1, "Status");
_LIT(KTestSubject1, "Subject");
_LIT(KTestNumber1, "Number");
const TLogContactItemId KTestContact1 = 0x1234;
const TLogContactItemId KTestContact2 = 0x1234567;
const TLogLink KTestLink1 = 0x1234;
_LIT8(KTestData1, "ABCDEFGH");
const TLogSize KTestMaxLogSize = 0xFFF;
const TLogRecentSize KTestMaxRecentLogSize = 0xF;
const TLogAge KTestMaxEventAge = 0xFFFFFFF;


/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0118
@SYMTestCaseDesc The test aims to ensure that the log server starts in reponse 
to a client request.
@SYMTestPriority High
@SYMTestActions The test asks the server for details of a built in event type.
A successful server start-up is essential for the completion of subsequent tests.
@SYMTestExpectedResults Success. (active->iStatus == 0) Regardless of the capability 
of the process running this test, it should always succeed because the GetEventType 
is not policed by platform security.
@SYMREQ REQ3431
*/
LOCAL_C void TestStartupL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0118 "));
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEventType *type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KLogCallEventTypeUid);
	active->StartL();
	client->GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);
		
	CleanupStack::PopAndDestroy(3); // client, active, type
	}




/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0119
@SYMTestCaseDesc Tests AddEvent, GetEvent, ChangeEvent and DeleteEvent.
@SYMTestPriority High
@SYMTestActions See the description and expected results.
@SYMTestExpectedResults 

1. a client with sufficient capability succeed in all cases would expect the following...
AddEvent - KErrNone
GetEvent - KErrNone
ChangeEvent - KErrNone
DeleteEvent - KErrNone

2. a client with insufficient capability would expect the following results...
AddEvent - KErrPermissionDenied
GetEvent - KErrNone
ChangeEvent - KErrPermissionDenied
DeleteEvent - KErrPermissionDenied

@SYMREQ REQ3431
*/
LOCAL_C void TestBasicL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0119 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	
	TTime now;
	now.UniversalTime();

	event->SetEventType(KLogCallEventTypeUid);

	active->StartL();
	aClient.AddEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNone : KErrPermissionDenied);

	if(!TheHiCapability)
		{
		TInt eventId = TestUtils::AddEventL();
		TEST(eventId >= 0);
		event->SetId(eventId);
		}


	TEST(event->EventType() == KLogCallEventTypeUid);

	now = event->Time();

	TLogId id = event->Id();

	event->SetRemoteParty(KTestRemoteParty1);
	event->SetDirection(KTestDirection1);
	event->SetDurationType(KTestDurationType1);
	event->SetDuration(KTestDuration1);
	event->SetStatus(KTestStatus1);
	event->SetSubject(KTestSubject1);
	event->SetNumber(KTestNumber1);
	event->SetContact(KTestContact1);
	event->SetLink(KTestLink1);
	event->SetDataL(KTestData1);

	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
    TheTest.Printf(_L("TestBasicL(), TheHiCapability=%d, event id=%d\r\n"), TheHiCapability, id);
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNone : KErrPermissionDenied);

	TEST(event->Id() == id );
	TEST(event->EventType() == KLogCallEventTypeUid);

	if(TheHiCapability)
		{
		TEST(event->Description().Length() > 0);
		TEST(event->Time() == now);
		TEST(event->RemoteParty() == KTestRemoteParty1);
		TEST(event->Direction() == KTestDirection1);
		TEST(event->DurationType() == KTestDurationType1);
		TEST(event->Duration() == KTestDuration1);
		TEST(event->Status() == KTestStatus1);
		TEST(event->Subject() == KTestSubject1);
		TEST(event->Number() == KTestNumber1);
		TEST(event->Contact() == KTestContact1);
		TEST(event->Link() == KTestLink1);
		TEST(event->Data() == KTestData1);
		}
		
	CleanupStack::PopAndDestroy(); // event;

	event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(id);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNone : KErrPermissionDenied);

	if(TheHiCapability)
		{
		TEST(event->Id() == id);
		TEST(event->EventType() == KLogCallEventTypeUid);
		TEST(event->Description().Length() > 0);
		TEST(event->Time() == now);
		TEST(event->RemoteParty() == KTestRemoteParty1);
		TEST(event->Direction() == KTestDirection1);
		TEST(event->DurationType() == KTestDurationType1);
		TEST(event->Duration() == KTestDuration1);
		TEST(event->Status() == KTestStatus1);
		TEST(event->Subject() == KTestSubject1);
		TEST(event->Number() == KTestNumber1);
		TEST(event->Contact() == KTestContact1);
		TEST(event->Link() == KTestLink1);
		TEST(event->Data() == KTestData1);
		}
			
	active->StartL();
	aClient.DeleteEvent(id, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNone : KErrPermissionDenied);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNotFound : KErrPermissionDenied);
	
	// try to delete a non-existent event...
	active->StartL();
	aClient.DeleteEvent(123, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNotFound);

	CleanupStack::PopAndDestroy(2); // event, active
	}



/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0120
@SYMTestCaseDesc Test AddEventType for high and low capability clients
@SYMTestPriority High
@SYMTestActions 
1.  For a clients of all capabilities...
	a. Add a new event type
	b. Fill a CLogEventType object with details of the new type
	c. Try to add the new event type for a second time 

@SYMTestExpectedResults 
1.  For a client with sufficient capability...
	Test a. - KErrNone - the new event type was added OK
	Test b. - KErrNone - the new event type is fully visible
	Test c. - KErrAlreadyExists - it was already added by test a. !

2.  For a client with insufficient capability...
	Test a. - KErrPermissionDenied - not allowed !
	Test b. - KErrNotFound - allowed, but it isn't there because test a. failed.
	Test c. - KErrPermissionDenied - exactly the same as test a.
	
@SYMREQ REQ3431
*/
LOCAL_C void TestAddEventTypeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0120 "));
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);
	type->SetDescription(KTestEventDesc1);
	type->SetLoggingEnabled(ETrue);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	aClient.AddEventType(*type, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNone : KErrPermissionDenied);
		
	active->StartL();
	aClient.GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNone : KErrNotFound);
			
	active->StartL();
	aClient.AddEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrAlreadyExists : KErrPermissionDenied);

	CleanupStack::PopAndDestroy(2); // active, type
	}



/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0121
@SYMTestCaseDesc Try to populate an CLogEventType object with details of a 
non built-in event type.
@SYMTestPriority High
@SYMTestActions Calls GetEventType(...)
@SYMTestExpectedResults Always succeeds - all clients should be able to do this as
the GetEventType operation is not policed by platform security.
@SYMREQ REQ3431
*/
LOCAL_C void TestGetEventTypeL(CLogClient& aClient)
	//
	// This test should succeed, regardless of capability
	// GetEventType is not policed.
	//
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0121 "));
	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	aClient.GetEventType(*type, active->iStatus);
	active->StartL();
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TEST(type->Uid() == KTestEventUid);
	TEST(type->Description() == KTestEventDesc1);
	TEST(type->LoggingEnabled());

	CleanupStack::PopAndDestroy(2); // active, type
	}




/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0122
@SYMTestCaseDesc Tries to change existing event types.
@SYMTestActions See the description and expected results.
@SYMTestPriority High
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestChangeEventTypeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0122 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

	type->SetDescription(KTestEventDesc1);
	type->SetDescription(KTestEventDesc2);

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		type->SetDescription(KTestEventDesc2);
		type->SetLoggingEnabled(EFalse);

		__UHEAP_FAILNEXT(failCount++);
		
		aClient.ChangeEventType(*type, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			aClient.GetEventType(*type, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(type->Description() == KTestEventDesc1);
			TEST(type->LoggingEnabled());
			}
		else
			{
			TEST2(error, TheHiCapability ? KErrNone : KErrPermissionDenied);
			
			if(!TheHiCapability)
				finished = TRUE;
			}
		}

	type->SetUid(KTestEventUid);

	active->StartL();
	aClient.GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	if(TheHiCapability)
		{
		TEST(type->Uid() == KTestEventUid);
		TEST(type->Description() == KTestEventDesc2);
		TEST(type->LoggingEnabled() == EFalse);
		}
	else
		{
		TEST(type->Uid() == KTestEventUid);
		TEST(type->Description() == KTestEventDesc1);
		TEST(type->LoggingEnabled());
		}

	CleanupStack::PopAndDestroy(2); // type, active
	}




/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0123
@SYMTestCaseDesc Delete event types
@SYMTestActions See the description and expected results.
@SYMTestPriority High
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestDeleteEventTypeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0123 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);

	type->SetUid(KTestEventUid);

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		__UHEAP_FAILNEXT(failCount++);
		aClient.DeleteEventType(KTestEventUid, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();

		__UHEAP_RESET;

		if (error == KErrNoMemory)
			{
			active->StartL();
			aClient.GetEventType(*type, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);
			}
		else
			{
			TEST2(error, TheHiCapability ? KErrNone : KErrPermissionDenied);
			
			if(!TheHiCapability)
				finished = TRUE;
			}
		}

	active->StartL();
	aClient.GetEventType(*type, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNotFound : KErrNone);

	CleanupStack::PopAndDestroy(2); // type, active
	}




/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0124
@SYMTestCaseDesc Try to delete built in event types
@SYMTestPriority High
@SYMTestActions No one is allowed to delete built in event types !
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestDeleteBuiltInEventTypeL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0124 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogEventType* type = CLogEventType::NewL();
	CleanupStack::PushL(type);
	
	TUid someBuiltInTypes [] = 
		{
		KLogCallEventTypeUid, 
		KLogDataEventTypeUid, 
		KLogFaxEventTypeUid
		};
	
	for (TInt i=0; i < 3; i++)
		{
		aClient.DeleteEventType(someBuiltInTypes [i], active->iStatus);
		active->StartL();
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrPermissionDenied);

		type->SetUid(someBuiltInTypes [i]);
		active->StartL();
		aClient.GetEventType(*type, active->iStatus);
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST(type->Uid() == someBuiltInTypes [i]);
		}

	CleanupStack::PopAndDestroy(2); // type, active
	}





/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0125
@SYMTestCaseDesc Get database configuration
@SYMTestPriority High
@SYMTestActions All clients are allowed to do this
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestGetConfigL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0125 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig config;

	TEST(config.iMaxEventAge == 0);
	TEST(config.iMaxLogSize == 0);
	TEST(config.iMaxRecentLogSize == 0);

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;

	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);
		aClient.GetConfig(config, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			TEST2(active->iStatus.Int(), KErrNoMemory);

		__UHEAP_RESET;
		}

	TEST(config.iMaxEventAge > 0);
	TEST(config.iMaxLogSize > 0);
	TEST(config.iMaxRecentLogSize > 0);

	CleanupStack::PopAndDestroy(); // active
	}




/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0126
@SYMTestCaseDesc Change the database configuration
@SYMTestPriority High
@SYMTestActions Low capability clients can't do this
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestChangeConfigL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0126 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TLogConfig configOld;

	active->StartL();
	aClient.GetConfig(configOld, active->iStatus);
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), KErrNone);

	TLogConfig config;

#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;

	while(!finished)
		{
		error = KErrNone;

		config.iMaxLogSize = KTestMaxLogSize;
		config.iMaxRecentLogSize = KTestMaxRecentLogSize;
		config.iMaxEventAge = KTestMaxEventAge;

		__UHEAP_FAILNEXT(failCount++);
		aClient.ChangeConfig(config, active->iStatus);

		active->StartL();
		CActiveScheduler::Start();

		if (active->iStatus == KErrNone)
			finished = ETrue;
		else
			error = active->iStatus.Int();
		
		__UHEAP_RESET;

		if ((error == KErrNoMemory) || (error == KErrPermissionDenied))
			{
			active->StartL();
			aClient.GetConfig(config, active->iStatus);
			CActiveScheduler::Start();
			TEST2(active->iStatus.Int(), KErrNone);

			TEST(config.iMaxLogSize == configOld.iMaxLogSize);
			TEST(config.iMaxRecentLogSize == configOld.iMaxRecentLogSize);
			TEST(config.iMaxEventAge == configOld.iMaxEventAge);
			}
		else
			{
			TEST2(error, KErrNone);		
			}
			
		if(! TheHiCapability)
			finished = TRUE;
						
		}

	if(TheHiCapability)
		{
		TEST(config.iMaxLogSize == KTestMaxLogSize);
		TEST(config.iMaxRecentLogSize == KTestMaxRecentLogSize);
		TEST(config.iMaxEventAge == KTestMaxEventAge);
		}
	else
		{
		TEST(config.iMaxLogSize == configOld.iMaxLogSize);
		TEST(config.iMaxRecentLogSize == configOld.iMaxRecentLogSize);
		TEST(config.iMaxEventAge == configOld.iMaxEventAge);
		}
		
	CleanupStack::PopAndDestroy(); // active
	}




/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0127
@SYMTestCaseDesc Clear the log
@SYMTestPriority High
@SYMTestActions Low capability clients can't do this
@SYMTestExpectedResults Should always pass
@SYMREQ REQ3431
*/
void ClearLogL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0127 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	
	active->StartL();
	aClient.ClearLog(now, active->iStatus);
		
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNone : KErrPermissionDenied);
	
	CleanupStack::PopAndDestroy(); // active
	}	

/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0128
@SYMTestCaseDesc Try to clear the recent event list
@SYMTestPriority High
@SYMTestActions Low capability clients will fail
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
void ClearRecentListL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0128 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	
	active->StartL();
	aClient.ClearLog(KLogRecentIncomingCalls, active->iStatus);
	
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNone : KErrPermissionDenied);
	
	CleanupStack::PopAndDestroy(); // active
	}	

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

/**
@SYMTestCaseID			PDS-LOGENG-UT-4040
@SYMTestCaseDesc		Clear log events with specific SimId - platform security test.
						This test is called from two test applications: t_logHicapability 
						and t_loglowcapabilty. As the names state, t_loghicapability has platsec capabilities to 
						call CLogClient::CLearLog(), t_loglowcapabilty - hasn't.
						When t_loghicapability calls ClearLogL(), "TheHiCapability" flag is true and it is expected
						the CLogClient::ClearLog() call to complete successfully. 
						When t_loglowcapability calls ClearLogL(), "TheHiCapability" flag is false and it is expected
						the CLogClient::ClearLog() call to fail with KErrPermissionDenied. 
@SYMTestActions			Clear log events with specific SimId - platform security test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12748
*/
void ClearLogL(CLogClient& aClient, TSimId aSimId)
	{//This test case is compiled only when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM macro is defined.
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	
	active->StartL();
	aClient.ClearLog(now, aSimId, active->iStatus);
		
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNone : KErrPermissionDenied);
	
	CleanupStack::PopAndDestroy(); // active
	}	

/**
@SYMTestCaseID			PDS-LOGENG-UT-4041
@SYMTestCaseDesc		Clear log events from the specified recent list with specific SimId - platform security test.
						This test is called from two test applications: t_logHicapability 
						and t_loglowcapabilty. As the names state, t_loghicapability has platsec capabilities to 
						call CLogClient::CLearLog(), t_loglowcapabilty - hasn't.
						When t_loghicapability calls ClearLogL(), "TheHiCapability" flag is true and it is expected
						the CLogClient::ClearLog() call to complete successfully. 
						When t_loglowcapability calls ClearLogL(), "TheHiCapability" flag is false and it is expected
						the CLogClient::ClearLog() call to fail with KErrPermissionDenied. 
@SYMTestActions			Clear log events from the specified recent list with specific SimId - platform security test.
@SYMTestExpectedResults Test must not fail
@SYMTestPriority		High
@SYMREQ					REQ12748
*/
void ClearRecentListL(CLogClient& aClient, TSimId aSimId)
	{//This test case is compiled only when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM macro is defined.
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	TTime now;
	now.UniversalTime();
	
	active->StartL();
	aClient.ClearLog(KLogRecentIncomingCalls, aSimId, active->iStatus);
	
	CActiveScheduler::Start();
	TEST2(active->iStatus.Int(), TheHiCapability ? KErrNone : KErrPermissionDenied);
	
	CleanupStack::PopAndDestroy(); // active
	}	

#endif//SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0129
@SYMTestCaseDesc Test that a view only contains events that are suitable for a clients cabability
@SYMTestPriority High
@SYMTestActions See comments in the trest code below for further info.
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestEventViewWithFilterL(CLogClient& aClient)
	{
	// TestUtils::AddTestEventsL() --> should be called before this function.
	// It deletes the database, then...
	// adds 8 events - 2 are visible to clients with no capabilities
	// and 6 are of type KLogCallEventTypeUid, which is protected.
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0129 "));
	CTestActive* active = new(ELeave)CTestActive;
	CleanupStack::PushL(active);

	CLogFilterList* list = new(ELeave)CLogFilterList;
	CleanupStack::PushL(list);

	CLogViewEvent* view = CLogViewEvent::NewL(aClient);
	CleanupStack::PushL(view);

	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	
	TLogString direction;
	aClient.GetString(direction, R_LOG_DIR_IN);
	
	// Test 1.
	// Of the 8 new events, 2 have KTestContact1 as the contact field
	// One of them is a secure type and the other isn't.
	// So, if running at hi capability, expect 2 events, else 1
	TInt expectedEventCount = (TheHiCapability) ? 2 : 1;
	filter->SetContact(KTestContact1);
	list->AppendL(filter);
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST(view->CountL() == expectedEventCount);
	
	// Test 2.
	// Of the 8 new events, 6 have KTestContact2 as the contact field
	// One of them is a secure type and the other isn't.
	// The filters will be combined in the query, so expect
	// 8 events if running at hi capability, else 2
	expectedEventCount = (TheHiCapability) ? 8 : 2;
	CleanupStack::Pop(); // filter
	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetContact(KTestContact2);
	list->AppendL(filter);
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST(view->CountL() == expectedEventCount);
		
	// Test 3.
	// Of the 8 new events, 7 have R_LOG_DIR_IN as the direction field.
	// Two of these are only visible for hi capability clients, the other one
	// can be viewed by all clients.
	expectedEventCount = (TheHiCapability) ? 7 : 1;
	CleanupStack::Pop(); // filter
	filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetDirection(direction);
	list->AppendL(filter);
	active->StartL();
	TEST(view->SetFilterL(*list, active->iStatus));
	CActiveScheduler::Start();
	TEST(view->CountL() == expectedEventCount);
	
	// clear up...
	list->ResetAndDestroy();
	CleanupStack::Pop(); // filter
	CleanupStack::PopAndDestroy(3);  // view, list, active
	}




/**
@SYMTestCaseID SYSLIB-LOGENG-CT-0130
@SYMTestCaseDesc Test the clients ability to view recent events
@SYMTestActions See the description and expected results.
@SYMTestPriority High
@SYMTestExpectedResults Should always succeed
@SYMREQ REQ3431
*/
LOCAL_C void TestRecentViewL(CLogClient& aClient)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-0130 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogViewRecent* view = CLogViewRecent::NewL(aClient);
	CleanupStack::PushL(view);

	active->StartL();
	
	// will return FALSE if the view is empty, which should  be the case
	// with low/no capability clients.
	if (view->SetRecentListL(KLogRecentIncomingCalls, active->iStatus))
		{
		__ASSERT_ALWAYS(TheHiCapability, _L("TestRecentViewL"));
		CActiveScheduler::Start();
		TEST2(active->iStatus.Int(), KErrNone);
		TEST(view->CountL() == 2);
		TInt count = view->CountL();
		}
	else	// low capability client won't be able to see recent incoming calls
		{
		__ASSERT_ALWAYS(!TheHiCapability, _L("TestRecentViewL"));
		active->Cancel();
		}

	CleanupStack::PopAndDestroy(2);	// view, active
	}

//.............................................................................

void doTestsL()
	{
    TestUtils::Initialize(KTestTitle);
					
	TheTest.Start(_L("Starting tests..."));
		
	CLogClient* client = CLogClient::NewL(theFs);
	CleanupStack::PushL(client);
	
	TestUtils::DeleteDatabaseL();
	
	TheTest.Next(_L("Server startup"));
	TestStartupL();
	User::After(100000);

	TheTest.Next(_L("Test Add, Get, Change & Delete event"));
	TestUtils::DeleteDatabaseL();
	TestBasicL(*client);
	theLog.Write(_L8("Basic tests OK\n"));

	TheTest.Next(_L("Add Event Type"));
	TestAddEventTypeL(*client);
	theLog.Write(_L8("Add Event Type test OK\n"));
	
	if(!TheHiCapability)
		{	
		// TestAddEventTypeL() will have failed to add an event type, so
		// this will ensure one is present for the next tests.
		TestUtils::AddEventTypeL();
		}
	
	TheTest.Next(_L("Get Event Type"));
	TestGetEventTypeL(*client);
	theLog.Write(_L8("Get Event Type test OK\n"));
	
	TheTest.Next(_L("Change Event Type"));
	TestChangeEventTypeL(*client);
	theLog.Write(_L8("Change Event Type test OK\n"));
	
	TheTest.Next(_L("Delete Event Type"));
	TestDeleteEventTypeL(*client);
	theLog.Write(_L8("Delete Event Type test OK\n"));
	
	TheTest.Next(_L("Delete Built-in Event Type"));
	TestDeleteBuiltInEventTypeL(*client);
	theLog.Write(_L8("Delete Built-in Event Type test OK\n"));
	
	TheTest.Next(_L("Get Server Configuration"));
	TestGetConfigL(*client);
	theLog.Write(_L8("Get Server Configuration test OK\n"));
	
	TheTest.Next(_L("Change Server Configuration"));
	TestChangeConfigL(*client);
	theLog.Write(_L8("Change Server Configuration test OK\n"));
	
	TheTest.Next(_L("Clear the Log test"));
	ClearLogL(*client);
	theLog.Write(_L8("Clear the Log test OK\n"));
		
	TheTest.Next(_L("Clear Recent List test"));
	ClearRecentListL(*client);
	theLog.Write(_L8("Clear Recent List test OK\n"));
	
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	const TSimId KSimId = 4000000000U;

	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4040 Clear the \"Log + SimId\" test"));
	ClearLogL(*client, KSimId);
	theLog.Write(_L8("Clear the \"Log + SimId\" test OK\n"));

	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4041 Clear \"Recent List + SimId\" test"));
	ClearRecentListL(*client, KSimId);
	theLog.Write(_L8("Clear \"Recent List + SimId\" test OK\n"));
#endif
	
	//
	// View API tests below this
	//
	TestUtils::DeleteDatabaseL();
	
	// TestUtils::AddTestEventsL() -->  adds 8 events.
	// 2 are visible to clients with no capabilities
	// and 6 are of type KLogCallEventTypeUid, which is protected.
	TestUtils::AddViewTestEventsL();
	
	TheTest.Next(_L("Event View with Filter list"));
	TestEventViewWithFilterL(*client);	
	theLog.Write(_L8("Event View with Filter list test OK\n"));
	
	TheTest.Next(_L("Recent View"));
	TestRecentViewL(*client);
	theLog.Write(_L8("Recent View test OK\n"));

	TestUtils::DeleteDatabaseL();	// ready for next test

	CleanupStack::PopAndDestroy(); // client
	}
