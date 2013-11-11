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

RTest 	TheTest (_L ("Central Repository Notification Handler Tests"));

const TInt KThreadStackSize		= 0x2000; // 8k
const TInt KThreadMinHeapSize	= 0x4000; // 16k
const TInt KThreadMaxHeapSize	= 0xa000; // 60k

///////////////////////////////////////////////////////////////////////////////////////
// Test macroses and functions
///////////////////////////////////////////////////////////////////////////////////////

LOCAL_C void CheckCondition(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Error in line %d: expected: %d, got: %d\r\n"), aLine, aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}

#define TEST2(aValue, aExpected) ::CheckCondition(aValue, aExpected, __LINE__)
#define TESTMSG(arg,msg) ::CheckCondition((arg), msg, __LINE__)


///////////////////////////////////////////////////////////////////////////////////////
// class TTestCaseInfo
///////////////////////////////////////////////////////////////////////////////////////

const TText* TTestCaseInfo::TestCaseTitle()
	{
	// This array of strings must correspond to enum TTestCase:
	static const TText* KCaseTitles[] =
		{
			_S("ListeningToSingleKey"),
			_S("ListeningToAllKeys"),
			_S("NoMultipleNotifications"),
			_S("NoNotificationAfterStopListening"),
			_S("WholeRepositoryReset"),
			_S("SingleKeyReset"),
			_S("TwoNotificationHandlers"),
			_S("NonExistentKey"),
			_S("IncorrectKeyType"),
			_S("RepeatingCalls"),
			_S("CancellingRequest"),
			_S("ExplicitMethodCalls"),
			_S("ExplicitRunErrorCall"),
			_S("OOMTest"),
		};
	ASSERT(iTestCase < sizeof(KCaseTitles)/sizeof(TText*));
	
	return KCaseTitles[iTestCase];
	}

void TTestCaseInfo::SetTestData(TTestKeyType aKeyType)
	{
	iKeyType = aKeyType;
	}

TTestKeyType TTestCaseInfo::KeyType()
	{
	return iKeyType;
	}

TInt TTestCaseInfo::KeyID()
	{
	TInt keyID = 0;
	switch (iKeyType)
		{
		case EKeyTypeInt:
			keyID = KKey2;
			break;

		case EKeyTypeReal:
			keyID = KKey3;
			break;

		case EKeyTypeString:
			keyID = KKey1;
			break;

		case EKeyTypeBinary:
			keyID = KKey4;
			break;

		default:
			// Will return the Int key ID in case of the whole repository
			keyID = KKey2;
			break;

		}
	return keyID;
	}


///////////////////////////////////////////////////////////////////////////////////////
// Utility functions
///////////////////////////////////////////////////////////////////////////////////////

/**
	Revert the repository to its initial state
*/
LOCAL_C void InitRepositoryL()
	{
	CRepository* session = CRepository::NewL(KTestUid);
	CleanupStack::PushL(session);
	
	User::LeaveIfError( session->Set(KKey1, KStringValueInit) );
	User::LeaveIfError( session->Set(KKey2, KIntValueInit) );
	User::LeaveIfError( session->Set(KKey3, KRealValueInit) );
	User::LeaveIfError( session->Set(KKey4, KBinaryValueInit) );
	
	CleanupStack::PopAndDestroy(session);
	}

/**
	Prepare data before the notification handler has started.
	Used for repository reset tests.
*/
LOCAL_C TInt PrepareTestDataL(TTestCaseInfo &aTestCaseInfo)
	{
	TInt result = KErrNone;
	TheTest.Printf(_L("PrepareTestDataL(TestCase = %s, KeyType = %d)\n"), aTestCaseInfo.TestCaseTitle(), aTestCaseInfo.KeyType());
	
	CRepository* session = CRepository::NewL(KTestUid);
	CleanupStack::PushL(session);

	switch (aTestCaseInfo.TestCase())
		{
		case ECaseWholeRepositoryReset:
			{
			// Create a new key
			result = session->Create(KNewTestKey1, KResetTestString);
			break;
			}

		case ECaseSingleKeyReset:
			{
			// Change an existing key
			switch (aTestCaseInfo.KeyType())
				{
				case EKeyTypeInt:
					TheTest.Printf(_L("Set KKey2 to %d\n"), KIntValue);
					result = session->Set(KKey2, KIntValue);
					break;

				case EKeyTypeReal:
					TheTest.Printf(_L("Set KKey3 to %f\n"), KRealValue);
					result = session->Set(KKey3, KRealValue);
					break;

				case EKeyTypeString:
					TheTest.Printf(_L("Set KKey1 to \"%S\"\n"), &KStringValue);
					result = session->Set(KKey1, KStringValue);
					break;

				case EKeyTypeBinary:
					TheTest.Printf(_L("Set KKey4 to \"%s\"\n"), &KBinaryValue);
					result = session->Set(KKey4, KBinaryValue);
					break;

				default:
					// For the whole repository test we change the Int key
					TheTest.Printf(_L("Set KKey2 to %d\n"), KIntValue3);
					result = session->Set(KKey2, KIntValue);
					break;
				}
			break;
			}
		}

	CleanupStack::PopAndDestroy(session);
	
	return result;
	} 


/**
	This function is called to change data when the notification handler is set up and listening
*/
LOCAL_C TInt ChangeTestDataL(TTestCaseInfo &aTestCaseInfo, TInt aStep)
	{
	TInt result = KErrNone;
	TheTest.Printf(_L("ChangeTestDataL(TestCase = %s, KeyType = %d)\n"), aTestCaseInfo.TestCaseTitle(), aTestCaseInfo.KeyType());

	CRepository* session = NULL;

	TRAP(result, session = CRepository::NewL(KTestUid));
	if (result != KErrNone)
		{
		RDebug::Print(_L("CRepository::NewL(%d) failed! result = %d"), KTestUid, result);
		return result;
		}

	CleanupStack::PushL(session);

	switch (aTestCaseInfo.TestCase())
		{
		case ECaseListeningToSingleKey:
		case ECaseListeningToAllKeys:
		case ECaseNoMultipleNotifications:
		case ECaseTwoNotificationHandlers:
		case ECaseRepeatingCalls:
			{
			switch (aTestCaseInfo.KeyType())
				{
				case EKeyTypeInt:
					TheTest.Printf(_L("Set KKey2 to %d\n"), KIntValue);
					result = session->Set(KKey2, KIntValue);
					break;

				case EKeyTypeReal:
					TheTest.Printf(_L("Set KKey3 to %f\n"), KRealValue);
					result = session->Set(KKey3, KRealValue);
					break;

				case EKeyTypeString:
					TheTest.Printf(_L("Set KKey1 to \"%S\"\n"), &KStringValue);
					result = session->Set(KKey1, KStringValue);
					break;

				case EKeyTypeBinary:
					TheTest.Printf(_L("Set KKey4 to \"%s\"\n"), &KBinaryValue);
					result = session->Set(KKey4, KBinaryValue);
					break;

				default:
					// For the whole repository test we change the Int key
					TheTest.Printf(_L("Set KKey2 to %d\n"), KIntValue3);
					result = session->Set(KKey2, KIntValue3);
					break;

				}
			break;
			}

		case ECaseWholeRepositoryReset:
			{
			// Reset whole repository	
			result = session->Reset();
			break;
			}

		case ECaseSingleKeyReset:
			{
			// Reset the key
			TheTest.Printf(_L("Reset the key with ID=%d\n"), aTestCaseInfo.KeyID());
			result = session->Reset(aTestCaseInfo.KeyID());
			break;
			}

		case ECaseNoNotificationAfterStopListening:
			switch (aStep)
				{
				case 0:
					switch (aTestCaseInfo.KeyType())
						{
						case EKeyTypeInt:
							TheTest.Printf(_L("Set KKey2 to %d\n"), KIntValue);
							result = session->Set(KKey2, KIntValue);
							break;

						case EKeyTypeReal:
							TheTest.Printf(_L("Set KKey3 to %f\n"), KRealValue);
							result = session->Set(KKey3, KRealValue);
							break;

						case EKeyTypeString:
							TheTest.Printf(_L("Set KKey1 to \"%S\"\n"), &KStringValue);
							result = session->Set(KKey1, KStringValue);
							break;

						case EKeyTypeBinary:
							TheTest.Printf(_L("Set KKey4 to \"%s\"\n"), &KBinaryValue);
							result = session->Set(KKey4, KBinaryValue);
							break;

						default:
							// For the whole repository test we change the Int key
							TheTest.Printf(_L("Set KKey2 to %d\n"), KIntValue3);
							result = session->Set(KKey2, KIntValue3);
							break;

						}
					break;

				case 1:
					// Use other values for the second step!
					switch (aTestCaseInfo.KeyType())
						{
						case EKeyTypeInt:
							TheTest.Printf(_L("Set KKey2 to %d\n"), KIntValue2);
							result = session->Set(KKey2, KIntValue2);
							break;

						case EKeyTypeReal:
							TheTest.Printf(_L("Set KKey3 to %f\n"), KRealValue2);
							result = session->Set(KKey3, KRealValue2);
							break;

						case EKeyTypeString:
							TheTest.Printf(_L("Set KKey1 to \"%S\"\n"), &KStringValue2);
							result = session->Set(KKey1, KStringValue2);
							break;

						case EKeyTypeBinary:
							TheTest.Printf(_L("Set KKey4 to \"%s\"\n"), &KBinaryValue2);
							result = session->Set(KKey4, KBinaryValue2);
							break;

						default:
							// For the whole repository test we change the Int key
							TheTest.Printf(_L("Set KKey2 to %d\n"), KIntValue2);
							result = session->Set(KKey2, KIntValue2);
							break;
						}
					break;
				}
			break;

		default:
			RDebug::Print(_L("*** ChangeTestDataL Error: Test case %d is not implemented! ***\n"), aTestCaseInfo.TestCase());
			result = KErrNotSupported;
			break;
		}

	CleanupStack::PopAndDestroy(session);
	
	return result;
	} 


/**
	The listening thread main function.
	Calls a CCenRepNotifyHandlerTest method corresponding to the current test
*/
LOCAL_C TInt ThreadMainL(TTestCaseInfo& aTestCaseInfo)
	{
	TInt ret = KErrNone;

	CCenRepNotifyHandlerTest* ptr = CCenRepNotifyHandlerTest::NewL();
	CleanupStack::PushL(ptr);

	switch (aTestCaseInfo.TestCase())
		{
		case ECaseListeningToSingleKey:
		case ECaseWholeRepositoryReset:
		case ECaseSingleKeyReset:
			ret = ptr->ListenToKeyChangesL(aTestCaseInfo);
			break;

		case ECaseListeningToAllKeys:
			ret = ptr->ListenToAllKeysL(aTestCaseInfo);
			break;

		case ECaseNoMultipleNotifications:
			ret = ptr->NoMultipleNotificationsL(aTestCaseInfo);
			break;

		case ECaseNoNotificationAfterStopListening:
			ret = ptr->NoNotificationAfterStopListeningL(aTestCaseInfo);
			break;

		case ECaseRepeatingCalls:
			ret = ptr->RepeatingCallsL(aTestCaseInfo);
			break;

		case ECaseTwoNotificationHandlers:
			ret = ptr->TwoNotificationHandlersL(aTestCaseInfo);
			break;

		default:
			RDebug::Print(_L("*** ThreadMainL Error: Test case %d is not implemented! ***\n"), aTestCaseInfo.TestCase());
			ret = KErrNotSupported;
			break;
		}

	TEST2(ret, KErrNone);

	CleanupStack::PopAndDestroy(ptr);

	return ret;
	}

/**
	Start the listening thread
*/
LOCAL_C TInt StartTestThread(TAny* aTestCaseInfo)
	{
	CTrapCleanup* TrapCleanup = CTrapCleanup::New();
	ASSERT(aTestCaseInfo);

	TRAPD( err, ThreadMainL( * static_cast<TTestCaseInfo*>(aTestCaseInfo) ) );

	ASSERT(err==KErrNone);
	delete TrapCleanup;
	return err;
	}

/**
	Main test function.
	- Set up and start the listening thread;
	- Wait for confirmation that the notification handler is ready and listening;
	- Change data to trigger notifications
*/
LOCAL_C void TestMainL(TTestCaseInfo& aTestCaseInfo, TInt aMaxStep = 0)
	{
	TInt err = KErrNone;

	// Create a separate thread, where the notification handler will be running
	_LIT(KName, "NotificationHandlerThread");
	RThread thread;
	err = thread.Create(KName, StartTestThread, KThreadStackSize, KThreadMinHeapSize, KThreadMaxHeapSize, &aTestCaseInfo);

	//If the previous NotificationHandlerThread thread is still not cleaned up
	//we need to use a different name
	if(err == KErrAlreadyExists)
		{		
		_LIT(KName1, "NotificationHandlerThread1");
		err = thread.Create(KName1, StartTestThread, KThreadStackSize, KThreadMinHeapSize, KThreadMaxHeapSize, &aTestCaseInfo);
		}
	
	CleanupClosePushL(thread);

	TEST2(err, KErrNone);

	TRequestStatus threadStatus;

	// Create rendezvous to get a signal from the thread
	thread.Rendezvous(threadStatus);

	// Start the thread
	thread.Resume();

	// Wait for the signal
	User::WaitForRequest(threadStatus);

	for (TInt step = 0; step <= aMaxStep; step++)
		{
		// At this point the thread is ready, notification handler is set up
		// and Active Scheduler is running
		
		if (threadStatus == KErrNone)
			{
			RDebug::Print(_L("Thread rendezvous reached\n"));
			}
		else
			{
			TheTest.Printf(_L("Error #%d"), threadStatus.Int());
			err = threadStatus.Int();
			TEST2(err, KErrNone);
			}	

		// Modify data to trigger notification
		err = ChangeTestDataL(aTestCaseInfo, step);
		if (err != KErrNone)
			{
			// Some API error happened
			thread.Terminate(err);
			User::LeaveIfError(err);
			}

		// Create rendezvous
		thread.Rendezvous(threadStatus);

		// Wait for the signal from the thread.
		// The rendezvous could complete or the thread could just exit - depends on the test case
		User::WaitForRequest(threadStatus);
		}

	TInt exitType = thread.ExitType();
	if (exitType != EExitKill)
		{
		TheTest.Printf(_L("Thread exit type = %d, reason = %d\n"), exitType, thread.ExitReason());
		}
	TEST2(thread.ExitType(), EExitKill); // exited normally
	
	CleanupStack::PopAndDestroy(); // it will call thread.Close()

	TheTest(err==KErrNone);

	RDebug::Print(_L("%s - succeeded\n"), aTestCaseInfo.TestCaseTitle());
	}


/**
	First step of the test
*/
LOCAL_C void TestStartL(TTestCaseInfo& aTestInfo)
	{
	InitRepositoryL();
	TheTest.Next(TPtrC(aTestInfo.TestCaseTitle()));
	}

/**
	Last step of the test
*/
LOCAL_C void TestFinish(TTestCaseInfo& aTestInfo)
	{
	TheTest.Printf(_L("\n%s has finished successfully!\n"), aTestInfo.TestCaseTitle());
	}


///////////////////////////////////////////////////////////////////////////////
//
// Actual tests
//
///////////////////////////////////////////////////////////////////////////////

/**
	Generic listening test looping through all key types but not the whole repository.
	Used for the following test cases:
		ListeningToSingleKey
		ListeningToAllKeys
		TwoNotificationHandlers
*/
void ListeningTestL(TTestCase aTestCase)
	{
	TTestCaseInfo testInfo(aTestCase);
	TestStartL(testInfo);

	TTestKeyType keyType;
	for (keyType = EKeyTypeInt; keyType < EKeyTypeAll; keyType = TTestKeyType(keyType+1) )
		{
		testInfo.SetTestData(keyType);
		PrepareTestDataL(testInfo);
		TestMainL(testInfo);
		}

	TestFinish(testInfo);
	}


/**
	Generic listening test looping through all key types and the whole repository.
	Used for the following test cases:
		SingleKeyReset
*/
void ListeningTestAllL(TTestCase aTestCase)
	{
	TTestCaseInfo testInfo(aTestCase);
	TestStartL(testInfo);

	TTestKeyType keyType;
	for (keyType = EKeyTypeInt; keyType <= EKeyTypeAll; keyType = TTestKeyType(keyType+1) )
		{
		testInfo.SetTestData(keyType);
		PrepareTestDataL(testInfo);
		TestMainL(testInfo);
		}

	TestFinish(testInfo);
	}


/**
	Generic two steps listening test looping through all key types and the whole repository.
	Used for the following test cases:
		NoMultipleNotifications
		NoNotificationAfterStopListening 
*/
void ListeningTestAll2L(TTestCase aTestCase)
	{
	TTestCaseInfo testInfo(aTestCase);
	TestStartL(testInfo);

	TTestKeyType keyType;
	for (keyType = EKeyTypeInt; keyType <= EKeyTypeAll; keyType = TTestKeyType(keyType+1) )
		{
		testInfo.SetTestData(keyType);
		TestMainL(testInfo, 1); // two steps
		}

	TestFinish(testInfo);
	}


/**
	Generic listening test for the whole repository.
	Used for the following test cases:
		WholeRepositoryReset
		RepeatingCalls
*/
void ListeningWholeRepositoryL(TTestCase aTestCase)
	{
	TTestCaseInfo testInfo(aTestCase);
	TestStartL(testInfo);

	testInfo.SetTestData(EKeyTypeAll);
	PrepareTestDataL(testInfo);
	TestMainL(testInfo);
	
	TestFinish(testInfo);
	}


/**
	Non-existent key test
*/
void NonExistentKeyL()
	{
	TTestCaseInfo testInfo(ECaseNonExistentKey);
	TestStartL(testInfo);

	TInt ret = KErrNone;

	CCenRepNotifyHandlerTest* ptr = CCenRepNotifyHandlerTest::NewL();
	CleanupStack::PushL(ptr);

	ret = ptr->NonExistentKeyL(testInfo);

	TEST2(ret, KErrNone);

	CleanupStack::PopAndDestroy(ptr);

	TestFinish(testInfo);
	}


/**
	Incorrect type test
*/
void IncorrectKeyTypeL()
	{
	TTestCaseInfo testInfo(ECaseIncorrectKeyType);
	TestStartL(testInfo);

	TInt ret = KErrNone;

	CCenRepNotifyHandlerTest* ptr = CCenRepNotifyHandlerTest::NewL();
	CleanupStack::PushL(ptr);

	ret = ptr->IncorrectKeyTypeL(testInfo);

	TEST2(ret, KErrNone);

	CleanupStack::PopAndDestroy(ptr);

	TestFinish(testInfo);
	}

/**
	Explicit exercise of the base class methods.
	Used for the following test cases:
		ExplicitMethodCalls
		ExplicitRunErrorCall
*/
void ExplicitExerciseL(TTestCase aTestCase)
	{
	TTestCaseInfo testInfo(aTestCase);
	TestStartL(testInfo);

	TInt ret = KErrNone;

	CCenRepNotifyHandlerTest* ptr = CCenRepNotifyHandlerTest::NewL();
	CleanupStack::PushL(ptr);

	switch (aTestCase)
		{
		case ECaseExplicitMethodCalls:
			ret = ptr->ExplicitExercise(testInfo);
			break;

		case ECaseExplicitRunErrorCall:
			ret = ptr->ExplicitExerciseRunErrorL(testInfo);
			break;
		}

	TEST2(ret, KErrNone);

	CleanupStack::PopAndDestroy(ptr);

	TestFinish(testInfo);
	}

/**
	Cancelling request
*/
void CancelRequestL()
	{
	TTestCaseInfo testInfo(ECaseCancellingRequest);
	TestStartL(testInfo);

	TInt ret = KErrNone;

	CCenRepNotifyHandlerTest* ptr = CCenRepNotifyHandlerTest::NewL();
	CleanupStack::PushL(ptr);

	testInfo.SetTestData(EKeyTypeAll);
	ret = ptr->CancelRequestL(testInfo);

	TEST2(ret, KErrNone);

	CleanupStack::PopAndDestroy(ptr);

	TestFinish(testInfo);
	}

