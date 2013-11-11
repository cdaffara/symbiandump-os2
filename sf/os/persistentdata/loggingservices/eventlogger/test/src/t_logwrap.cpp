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
#include <bautils.h>
#include "t_logutil2.h"

RTest TheTest(_L("t_logwrap"));

_LIT(KTestRemoteParty, "Remote Party");
_LIT(KTestDirection, "Direction");
const TLogDurationType KTestDurationType = 1;
const TLogDuration KTestDuration = 0x1234;
_LIT(KTestStatus, "Status");
_LIT(KTestSubject, "Subject");
_LIT(KTestNumber, "Number");
const TLogContactItemId KTestContact = 0x1234;
const TLogLink KTestLink = 0x1234;
_LIT8(KTestData, "ABCDEFGH");

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1011
@SYMTestCaseDesc	    Tests for the functionality of CLogEvent class
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for adding,changing,deleting and getting event type functions on the logevent
                        Check for memory,KErrNone and KErrNotSupported error flags
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestBasicL(CLogBase& aClient, TBool aClientAvailable)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1011 "));
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
	if(aClientAvailable)
		{
		TEST2(active->iStatus.Int(), KErrNone);
		}
	else
		{
		TEST2(active->iStatus.Int(), KErrNotSupported);
		}

	if (aClientAvailable)
		{
		TEST(event->EventType() == KLogCallEventTypeUid);
		TEST(event->Description().Length() > 0);
		TEST(event->Time() >= now);
		now = event->Time();
		}

	TLogId id = event->Id();

	event->SetRemoteParty(KTestRemoteParty);
	event->SetDirection(KTestDirection);
	event->SetDurationType(KTestDurationType);
	event->SetDuration(KTestDuration);
	event->SetStatus(KTestStatus);
	event->SetSubject(KTestSubject);
	event->SetNumber(KTestNumber);
	event->SetContact(KTestContact);
	event->SetLink(KTestLink);
	event->SetDataL(KTestData);

	active->StartL();
	aClient.ChangeEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	if(aClientAvailable)
		{
		TEST2(active->iStatus.Int(), KErrNone);
		}
	else
		{
		TEST2(active->iStatus.Int(), KErrNotSupported);
		}

	if (aClientAvailable)
		{
		TEST(event->Id() == id);
		TEST(event->EventType() == KLogCallEventTypeUid);
		TEST(event->Description().Length() > 0);
		TEST(event->Time() == now);
		TEST(event->RemoteParty() == KTestRemoteParty);
		TEST(event->Direction() == KTestDirection);
		TEST(event->DurationType() == KTestDurationType);
		TEST(event->Duration() == KTestDuration);
		TEST(event->Status() == KTestStatus);
		TEST(event->Subject() == KTestSubject);
		TEST(event->Number() == KTestNumber);
		TEST(event->Contact() == KTestContact);
		TEST(event->Link() == KTestLink);
		TEST(event->Data() == KTestData);
		}

	CleanupStack::PopAndDestroy(); // event;

	event = CLogEvent::NewL();
	CleanupStack::PushL(event);

	event->SetId(id);

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	if(aClientAvailable)
		{
		TEST2(active->iStatus.Int(), KErrNone);
		}
	else
		{
		TEST2(active->iStatus.Int(), KErrNotSupported);
		}

	if (aClientAvailable)
		{
		TEST(event->Id() == id);
		TEST(event->EventType() == KLogCallEventTypeUid);
		TEST(event->Description().Length() > 0);
		TEST(event->Time() == now);
		TEST(event->RemoteParty() == KTestRemoteParty);
		TEST(event->Direction() == KTestDirection);
		TEST(event->DurationType() == KTestDurationType);
		TEST(event->Duration() == KTestDuration);
		TEST(event->Status() == KTestStatus);
		TEST(event->Subject() == KTestSubject);
		TEST(event->Number() == KTestNumber);
		TEST(event->Contact() == KTestContact);
		TEST(event->Link() == KTestLink);
		TEST(event->Data() == KTestData);
		}

	active->StartL();
	aClient.DeleteEvent(id, active->iStatus);
	CActiveScheduler::Start();
	if(aClientAvailable)
		{
		TEST2(active->iStatus.Int(), KErrNone);
		}
	else
		{
		TEST2(active->iStatus.Int(), KErrNotSupported);
		}

	active->StartL();
	aClient.GetEvent(*event, active->iStatus);
	CActiveScheduler::Start();
	if(aClientAvailable)
		{
		TEST2(active->iStatus.Int(), KErrNotFound);
		}
	else
		{
		TEST2(active->iStatus.Int(), KErrNotSupported);
		}

	CleanupStack::PopAndDestroy(2); // event, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1012
@SYMTestCaseDesc	    Test for CLogWrapper::Log(),CLogWrapper::ClientAvailable() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Execute basics event test on a log wrapper
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestWrapperL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1012 "));
	CTestActive* active = new(ELeave)CTestActive();
	CleanupStack::PushL(active);

	CLogWrapper* wrapper = CLogWrapper::NewL(theFs);
	CleanupStack::PushL(wrapper);

	CLogBase& logBase = wrapper->Log();
	TestBasicL(logBase, wrapper->ClientAvailable());

	TBuf<KLogMaxSharedStringLength> buf;
	if (wrapper->ClientAvailable())
		{
		CLogBase& logBase2 = wrapper->Log();
		TEST2(logBase2.GetString(buf, R_LOG_DIR_IN), KErrNone);
		TEST(buf.Length() > 0);
		}
	else
		{
		CLogBase& logBase3 = wrapper->Log();
		TEST2(logBase3.GetString(buf, R_LOG_DIR_IN), KErrNotSupported);
		TEST(buf.Length() == 0);
		}

	CleanupStack::PopAndDestroy(2); // wrapper, active
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1013
@SYMTestCaseDesc	    Tests for out of memory errors on CLogWrapper::NewL() function call
@SYMTestPriority 	    High
@SYMTestActions  	    Check for no memory error condition
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestHeapFailL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1013 "));
	CLogWrapper* wrapper = NULL;
	
#ifdef _DEBUG
	TInt failCount = 0;
#endif

	TBool finished = EFalse;
	TInt error;
	
	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, wrapper = CLogWrapper::NewL(theFs));

		__UHEAP_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;
			CLogBase& logBase = wrapper->Log();
			TestBasicL(logBase, wrapper->ClientAvailable());
			delete wrapper;
			}
		else
			TEST2(error, KErrNoMemory);
		}
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1014
@SYMTestCaseDesc	    Tests for CLogWrapper::NewL() function 
@SYMTestPriority 	    High
@SYMTestActions  	    Create log wrapper on heap,if no error execute the basic event tests.
                        Check for general errors.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void TestFileFailL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1014 "));
	CLogWrapper* wrapper = NULL;
	
	TInt failCount = 0;
	TBool finished = EFalse;
	TInt error;
	
	while(!finished)
		{
		TheTest.Printf(_L("%d  \r"), failCount);
		__FILE_FAILNEXT(KErrNoMemory, failCount++);

		TRAP(error, wrapper = CLogWrapper::NewL(theFs));

		__FILE_RESET;

		if (error == KErrNone)
			{
			finished = ETrue;
			CLogBase& logBase = wrapper->Log();
			TestBasicL(logBase, wrapper->ClientAvailable());
			delete wrapper;
			}
		else
			{
			TEST2(error, KErrNoMemory);
			}
		}
	TheTest.Printf(_L("\r\nThe test has succeeded at iteration %d\n"), failCount);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-1015
@SYMTestCaseDesc	    Tests for INC047632
@SYMTestPriority 	    High
@SYMTestActions  	    This test uses a stored copy of a corrupt database which is copied to z:\system\data.
						When this database ia accessed open returns an error KErrEof.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_C void Test4INC047632L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-1015 "));
#ifdef _DEBUG	
	TestUtils::CopyCorruptDbL();

	// run basic test
	CLogWrapper* wrapper = NULL;
	
	TInt error = KErrNone;
	TRAP(error, wrapper = CLogWrapper::NewL(theFs));

	if (error == KErrNone)
		{
		CLogBase& logBase = wrapper->Log();
		TestBasicL(logBase, wrapper->ClientAvailable());
		delete wrapper;
		}
#else
	RDebug::Print(_L("Test4INC047632L() can be executed only in debug mode. In release mode the LogEng server cannot be stopped.\n"));
#endif//_DEBUG
	}
/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-4001
@SYMTestCaseDesc	    Tests for INC114909
@SYMTestPriority 	    High
@SYMTestActions  	    This test uses a stored copy of a corrupt damaged database which is 
						copied to z:\system\data.
						When an attempt is made to open the database the fault should be 
						recognised and the database deleted.
@SYMTestExpectedResults Test must not fail
@SYMDEF                 INC114909
*/
LOCAL_C void Test5INC114909L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-4001 "));
#ifdef _DEBUG	
 	// Copy over the damaged database to be used.
	TestUtils::CopyCorruptDamagedDbL();
 
	// run basic test
	CLogWrapper* wrapper = NULL;	
	TInt error = KErrNone;
	TRAP(error, wrapper = CLogWrapper::NewL(theFs));
	delete wrapper;
	
	// Without the code for this defect the error here will be KErrCorrupt.
	// and the log server will continually try to restart and be brought down
	// with KErrCorrupt 
	// With the fix the error with the database is detected on opening the database 
	// and the  database file is deleted and re-created.
 
	TEST(error == KErrNone);
	
	// Perform an extra check that the log server has not been brought down
	// as is the case without the defect fix.
	RLogTestSession theLogServ;
	error = theLogServ.Connect();
 	TEST(error == KErrNone);
#else
	RDebug::Print(_L("Test4INC047632L() can be executed only in debug mode. In release mode the LogEng server cannot be stopped.\n"));
#endif//_DEBUG
	}

void doTestsL()
	{
	TestUtils::Initialize(_L("t_logwrap"));

	TheTest.Start(_L("Wrapper"));
	TestWrapperL();
	theLog.Write(_L8("Test 1 OK\n"));

	TheTest.Next(_L("Heap Failure"));
	TestHeapFailL();
	theLog.Write(_L8("Test 2 OK\n"));

	TheTest.Next(_L("File Failure"));
	TestFileFailL();
	theLog.Write(_L8("Test 3 OK\n"));

	TheTest.Next(_L("Test4 for INC047632 - corrupt Logdbu.dat returns KErrEoF"));
	Test4INC047632L();
	theLog.Write(_L8("Test 4 for INC047632 OK\n"));
	
	TheTest.Next(_L("Test5 for INC114909 - test damaged Logdbu.dat is dealt with correctly "));
	Test5INC114909L();
	theLog.Write(_L8("Test 5 for INC114909 OK\n"));
	}
