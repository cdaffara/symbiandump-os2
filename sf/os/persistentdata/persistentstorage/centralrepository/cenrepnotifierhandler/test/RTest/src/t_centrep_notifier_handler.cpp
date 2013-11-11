// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32test.h>
#include "t_centrep_notifier_handler.h"
#include "centrepnotifyhandlertest.h"
#include "centrepnotifyhandlertestkeys.h"

typedef void TFunction();

/**
						Listening to a single key

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3522
@SYMTestCaseDesc		Test how single key change generates notification.
@SYMTestPriority		Medium
@SYMTestActions			Open CRepository session
						Create CCenRepNotifyHandler instance for a key of given type
						Start listening for notifications
						Change value of a key of given type
						Check the result
						[Note: Repeat the test for the keys of all different types: Int, Real, String, Binary]
@SYMTestExpectedResults	Notification handler method for a given type is called with correct key ID and value passed.
@SYMREQ					REQ7698
*/
LOCAL_C void ListeningToASingleKeyL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3522 "));
	ListeningTestL(ECaseListeningToSingleKey);
	}


/**
						Listening to all keys in keyspace

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3523
@SYMTestCaseDesc		Test how notification is generated and handled for the entire keyspace.
@SYMTestPriority		Medium
@SYMTestActions			Open CRepository session
						Create CCenRepNotifyHandler instance for the entire repository
						Start listening for notifications
						Change value of a key of given type
						Check the result
						[Note: Repeat the test for the keys of different types]
@SYMTestExpectedResults	Generic notification handler method is called with correct key ID passed.
@SYMREQ					REQ7698
*/
LOCAL_C void ListeningToAllKeysInKeyspaceL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3523 "));
	ListeningTestL(ECaseListeningToAllKeys);
	}


/**
						No multiple notifications on the same value change

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3524
@SYMTestCaseDesc		Test that there is only one notification about two identical changes of the same existing key.
@SYMTestPriority		Medium
@SYMTestActions			Open CRepository session
						Create CCenRepNotifyHandler instance for a given type (a key or the whole repository)
						Start listening for notifications
						Change key value
						Check the result
						Set the same value for the same key
						Check the result
						[Note: Repeat the test for the keys of different types and whole keyspace]
@SYMTestExpectedResults	Only the first change generates notification.
@SYMREQ					REQ7698
*/
LOCAL_C void NoMultipleNotificationsOnTheSameValueChangeL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3524 "));
	ListeningTestAll2L(ECaseNoMultipleNotifications);
	}


/**
						No notification after stop listening

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3525
@SYMTestCaseDesc		Test that there are no notifications after StopListening() call.
@SYMTestPriority		Medium
@SYMTestActions			Open CRepository session
						Create CCenRepNotifyHandler instance for a given type
						Start listening for notifications
						Change key value
						Check the result
						Stop listening
						Change key value again (use another value)
						Check the result
						[Note: Repeat the test for the keys of different types and whole keyspace]
@SYMTestExpectedResults	Only the first change generates notification.
@SYMREQ					REQ7698
*/
LOCAL_C void NoNotificationAfterStopListeningL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3525 "));
	ListeningTestAll2L(ECaseNoNotificationAfterStopListening);
	}


/**
						Notification after whole repository reset

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3526
@SYMTestCaseDesc		Test how the component handles whole repository reset.
@SYMTestPriority		Medium
@SYMTestActions			Open CRepository session
						Create a new key
						Create CCenRepNotifyHandler instance for the whole repository
						Start listening for notifications
						Reset the repository
						Check the result
@SYMTestExpectedResults	Notification with KInvalidNotificationId is received.
@SYMREQ					REQ7698
*/
LOCAL_C void NotificationAfterWholeRepositoryResetL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3526 "));
	ListeningWholeRepositoryL(ECaseWholeRepositoryReset);
	}


/**
						Notification after a single key reset

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3527
@SYMTestCaseDesc		Test that a single key reset generates notification.
@SYMTestPriority		Medium
@SYMTestActions			Open CRepository session
						Change the value of an existing key
						Create CCenRepNotifyHandler instance for a given type
						Start listening for notifications
						Reset the key
						Check the result
						[Note: Repeat the test for the keys of different types and whole keyspace]
@SYMTestExpectedResults	Notification handler method for a given type is called with correct key ID passed.
@SYMREQ					REQ7698
*/
LOCAL_C void NotificationAfterASingleKeyResetL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3527 "));
	ListeningTestAllL(ECaseSingleKeyReset);
	}


/**
						Listening for two notification handlers

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3528
@SYMTestCaseDesc		Test that two notification handlers set and listening on the same repository at the same time receive correct notifications when an existing key is modified.
@SYMTestPriority		Medium
@SYMTestActions			Open CRepository session
						Create two CCenRepNotifyHandler instances for a given type
						For both handlers start listening for notifications
						Change a key value
						Check the result
						[Note: Repeat the test for the keys of different types and whole keyspace]
@SYMTestExpectedResults	For each CCenRepNotifyHandler instance a notification handler method for a given type is called with correct key ID and value passed as parameters.
@SYMREQ					REQ7698
*/
LOCAL_C void ListeningForTwoNotificationHandlersL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3528 "));
	ListeningTestL(ECaseTwoNotificationHandlers);
	}


/**
						Handling notification for a non-existent key

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3529
@SYMTestCaseDesc		Test how component handles a non-existent key.
@SYMTestPriority		Medium
@SYMTestActions			Open CRepository session
						Create CCenRepNotifyHandler instance for a non-existent key
						Start listening for notifications
						Check the result
@SYMTestExpectedResults	StartListeningL leaves with error code KErrNotFound.
@SYMREQ					REQ7698
*/
LOCAL_C void HandlingNotificationForANonExistentKeyL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3529 "));
	NonExistentKeyL();
	}


/**
						Test incorrect key type

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-3530
@SYMTestCaseDesc		Test that incorrect aKeyType parameter passed to the CCenRepNotifyHandler methods is handled properly
@SYMTestPriority		Medium
@SYMTestActions			Pass bad enum to CCenRepNotifyHandler::NewL method
@SYMTestExpectedResults	NewL leaves with error code KErrArgument.
@SYMREQ					REQ7698
*/
LOCAL_C void TestIncorrectKeyTypeL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-3530 "));
	IncorrectKeyTypeL();
	}


/**
						Repeating calls to StartListeningL/StopListening

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3531
@SYMTestCaseDesc		Test that repeating calls to the methods do not cause a problem
@SYMTestPriority		Medium
@SYMTestActions			Open CRepository session
						Create CCenRepNotifyHandler instance for the whole repository
						Make consecutive calls to StartListeningL
						Change the value of an existing key
						Make consecutive calls to StopListening
						Check the result
@SYMTestExpectedResults	Notification handler method for a given type is called with correct key ID and value passed. No errors happened.
@SYMREQ					REQ7698
*/
LOCAL_C void RepeatingCallsToStartListeningAndStopListeningL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3531 "));
	ListeningWholeRepositoryL(ECaseRepeatingCalls);
	}


/**
						Cancelling request

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3532
@SYMTestCaseDesc		Test how cancelling request is handled
@SYMTestPriority		Medium
@SYMTestActions			Open CRepository session
						Create CCenRepNotifyHandler instance for a given type
						Start listening for notifications
						Cancel the request by calling CCenRepNotifyHandler::DoCancel
						Check the result
						[Note: Repeat the test for the keys of different types and whole keyspace]
@SYMTestExpectedResults	Request completes with KUnspecifiedKey.
@SYMREQ					REQ7698
*/
LOCAL_C void CancellingRequestL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3532 "));
	CancelRequestL();
	}


/**
						Explicit exercise of the base class callback methods

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-3533
@SYMTestCaseDesc		Test that base class notification handler callback methods do not cause any problems if used.
@SYMTestPriority		Medium
@SYMTestActions			Call base class methods MCenRepNotifyHandlerCallback::HandleNotifyXXX
@SYMTestExpectedResults	No errors or exceptions.
						A manual check can be made for debug print statements in the epocwind.out for runs of the test on debug builds of the dll.
@SYMREQ					REQ7698
*/
LOCAL_C void ExplicitExerciseOfTheBaseClassCallbackMethodsL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-3533 "));
	ExplicitExerciseL(ECaseExplicitMethodCalls);
	}


/**
						Explicit exercise of the base class RunError method

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-UT-3534
@SYMTestCaseDesc		Test that CCenRepNotifyHandler::RunError does not cause any problems if called.
@SYMTestPriority		Medium
@SYMTestActions			Call CCenRepNotifyHandler::RunError method
@SYMTestExpectedResults	No errors or exceptions.
						A manual check can be made for debug print statements in the epocwind.out for runs of the test on debug builds of the dll.
@SYMREQ					REQ7698
*/
LOCAL_C void ExplicitExerciseOfTheBaseClassRunErrorMethodL()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-UT-3534 "));
	ExplicitExerciseL(ECaseExplicitRunErrorCall);
	}


/**
	Run all tests
*/
LOCAL_C void RunAllTestsL()
	{

	ListeningToASingleKeyL();

	ListeningToAllKeysInKeyspaceL();

	NoMultipleNotificationsOnTheSameValueChangeL();

	NoNotificationAfterStopListeningL();

	NotificationAfterWholeRepositoryResetL();

	NotificationAfterASingleKeyResetL();

	ListeningForTwoNotificationHandlersL();

	HandlingNotificationForANonExistentKeyL();

	TestIncorrectKeyTypeL();

	RepeatingCallsToStartListeningAndStopListeningL();

	CancellingRequestL();

	ExplicitExerciseOfTheBaseClassCallbackMethodsL();

	ExplicitExerciseOfTheBaseClassRunErrorMethodL();
	}


/**
Utility function for RunAllTestsOOML, to ensure that the cleanup stack does not expand
during the test, which would cause a false positive heap memory leak check.
*/
void PushLotsL()
	{
	TInt* dummy = NULL;
	for (TInt i = 0; i < 100; i++)
		{
		CleanupStack::PushL(dummy);
		}
	CleanupStack::Pop(100);
	}

/**
	Calls the function from OOM test loop
*/
LOCAL_C void OOMTest(TFunction aFunction)
	{
	TheTest.Next(_L("OOM Test"));
	TheTest.Start(_L("Running a series of OOM tests..."));

	TInt err = KErrNone;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		RDebug::Print(_L("****** OOM try count: %d\n"), tryCount);
		TRAP(err, aFunction());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

	TheTest.End();

	TheTest(err==KErrNone);
	TheTest.Printf(_L("- succeeded at heap failure rate of %i\n"), tryCount);
	}

/**
						OOM Tests

@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-3535
@SYMTestCaseDesc		Test that OOM situation is handled
@SYMTestPriority		Medium
@SYMTestActions			Repeat all the above tests in an OOM loop.
@SYMTestExpectedResults	OOM tests complete successfully.
						Number of threads and process handles after the test is the same as before.
@SYMREQ					REQ7698
*/
LOCAL_C void RunAllTestsOOML()
	{
	TheTest.Next (_L (" @SYMTestCaseID:SYSLIB-CENTRALREPOSITORY-CT-3535 "));
	// Prepare cleanup stack so it does not expand during the test causing false positives
	PushLotsL();

	// Count handles
	TInt originalThreadHandleCount = 0;
	TInt processHandleCount = 0;
	RThread().HandleCount(processHandleCount, originalThreadHandleCount);

	// Run each test case in a OOM test loop
	OOMTest(ListeningToASingleKeyL);

	OOMTest(ListeningToAllKeysInKeyspaceL);

	OOMTest(NoMultipleNotificationsOnTheSameValueChangeL);

	OOMTest(NoNotificationAfterStopListeningL);

	OOMTest(NotificationAfterWholeRepositoryResetL);

	OOMTest(NotificationAfterASingleKeyResetL);

	OOMTest(ListeningForTwoNotificationHandlersL);

	OOMTest(HandlingNotificationForANonExistentKeyL);

	OOMTest(TestIncorrectKeyTypeL);

	OOMTest(RepeatingCallsToStartListeningAndStopListeningL);

	OOMTest(CancellingRequestL);

	OOMTest(ExplicitExerciseOfTheBaseClassCallbackMethodsL);

	OOMTest(ExplicitExerciseOfTheBaseClassRunErrorMethodL);

	// Check we haven't leaked any handles
	TInt threadHandleCount = 0;
	RThread().HandleCount(processHandleCount, threadHandleCount);
	TheTest(threadHandleCount == originalThreadHandleCount);
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TheTest.Title ();
	TheTest.Start (_L ("Central Repository Notification Handler Tests"));
	CTrapCleanup* cleanup = CTrapCleanup::New();

	// Disable debugging for the test - log the errors instead
	TBool jitEnabled = User::JustInTime();
	User::SetJustInTime(EFalse);

	// Run the tests
	TInt err;

	TRAP(err, RunAllTestsL());
	TheTest(err==KErrNone);

	TRAP(err, RunAllTestsOOML());
	TheTest(err==KErrNone);

	// Restore the debugging flag
	User::SetJustInTime(jitEnabled);

	delete cleanup;

	TheTest.End ();
	TheTest.Close ();

	__UHEAP_MARKEND;
	User::Heap().Check();

	return err;
	}
