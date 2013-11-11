// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "centrepnotifyhandlertest.h"
#include "centrepnotifyhandlertestkeys.h"

///////////////////////////////////////////////////////////////////////////////////////
// Local Definitions
///////////////////////////////////////////////////////////////////////////////////////

#define TESTLEAVE(err,msg) if (err!=KErrNone) { RDebug::Print(msg, err); User::Leave(err); }


///////////////////////////////////////////////////////////////////////////////////////
// Member Functions
///////////////////////////////////////////////////////////////////////////////////////

/**
	C++ default constructor can NOT contain any code, that might leave.
*/
CCenRepNotifyHandlerTest::CCenRepNotifyHandlerTest()
	{
	iUseSecondHandler = EFalse;
	}

/**
	Two-phase constructor.
*/
CCenRepNotifyHandlerTest* CCenRepNotifyHandlerTest::NewLC()
	{
    CCenRepNotifyHandlerTest* self = new (ELeave) CCenRepNotifyHandlerTest;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CCenRepNotifyHandlerTest* CCenRepNotifyHandlerTest::NewL()
	{
	CCenRepNotifyHandlerTest* self = CCenRepNotifyHandlerTest::NewLC();
	CleanupStack::Pop(self);
	return self;
	}
	
void CCenRepNotifyHandlerTest::ConstructL()
	{
	iMyScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iMyScheduler);
	}

/**
	Destructor.
*/
CCenRepNotifyHandlerTest::~CCenRepNotifyHandlerTest()
    { 
    if(iMyScheduler) 
    	{
		CActiveScheduler::Install(NULL);
		delete iMyScheduler;	
    	}
    iMyScheduler=NULL;
    }

/**
	Initialization
*/
void CCenRepNotifyHandlerTest::InitializeNotifyData()
	{
	iNotifyInt = 0;
	iNotifyReal = 0;
	iNotifyString = _L("");
	iNotifyBinary = _L8("");
	iNotifyError = KErrNone;
	iNotificationId = 0;

	iNotifyInt2 = 0;
	iNotifyReal2 = 0;
	iNotifyString2 = _L("");
	iNotifyBinary2 = _L8("");
	}

/**
	Check result for the test case and data type provided
*/
TBool CCenRepNotifyHandlerTest::CheckResult(TTestCaseInfo& aTestInfo)
	{
	TBool result = EFalse;

	RDebug::Print(_L("CheckResult( TestCase = %s, KeyType = %d )\n"), aTestInfo.TestCaseTitle(), aTestInfo.KeyType());

	if (iNotifyError != KErrNone)
		{
		RDebug::Print(_L("iNotifyError = %d\n"), iNotifyError );
		}
	else
		{
		switch (aTestInfo.TestCase())
			{
			case ECaseListeningToSingleKey:
			case ECaseListeningToAllKeys:
			case ECaseNoMultipleNotifications:
			case ECaseNoNotificationAfterStopListening:
			case ECaseRepeatingCalls:
				{
				switch (aTestInfo.KeyType())
					{
					case EKeyTypeInt:
						result = (iNotifyInt == KIntValue);
						RDebug::Print(_L("CheckResult:  Value = %d\n"), iNotifyInt );
						break;

					case EKeyTypeReal:
						result = (iNotifyReal == KRealValue);
						RDebug::Print(_L("CheckResult:  Value = %f\n"), iNotifyReal );
						break;

					case EKeyTypeString:
						result = (iNotifyString == KStringValue);
						RDebug::Print(_L("CheckResult:  Value = %S\n"), &iNotifyString );
						break;

					case EKeyTypeBinary:
						result = (iNotifyBinary == KBinaryValue);
						RDebug::Print(_L("CheckResult:  Value = %s\n"), &iNotifyBinary );
						break;

					default:
						// Currently it's the same as for Int
						result = (iNotifyInt == KIntValue3);
						RDebug::Print(_L("CheckResult:  Value = %d\n"), iNotifyInt );
						break;

					}
				break;
				}

			case ECaseWholeRepositoryReset:
				result = (iNotificationId == KNewTestKey1);
				RDebug::Print(_L("CheckResult:  ID = %d\n"), iNotificationId );
				break;

			case ECaseSingleKeyReset:
				{
				RDebug::Print(_L("CheckResult:  ID = %d\n"), iNotificationId );
				switch (aTestInfo.KeyType())
					{
					case EKeyTypeInt:
						result = (iNotificationId == KKey2);
						break;

					case EKeyTypeReal:
						result = (iNotificationId == KKey3);
						break;

					case EKeyTypeString:
						result = (iNotificationId == KKey1);
						break;

					case EKeyTypeBinary:
						result = (iNotificationId == KKey4);
						break;

					default:
						// Currently it's the same as for Int
						result = (iNotificationId == KKey2);
						break;

					}
				break;
				}

			case ECaseTwoNotificationHandlers:
				{
				switch (aTestInfo.KeyType())
					{
					case EKeyTypeInt:
						result = (iNotifyInt == KIntValue) && (iNotifyInt2 == KIntValue);
						RDebug::Print(_L("CheckResult:  Value = %d\n"), iNotifyInt );
						break;

					case EKeyTypeReal:
						result = (iNotifyReal == KRealValue) && (iNotifyReal2 == KRealValue);
						RDebug::Print(_L("CheckResult:  Value = %f\n"), iNotifyReal );
						break;

					case EKeyTypeString:
						result = (iNotifyString == KStringValue) && (iNotifyString2 == KStringValue);
						RDebug::Print(_L("CheckResult:  Value = %S\n"), &iNotifyString );
						break;

					case EKeyTypeBinary:
						result = (iNotifyBinary == KBinaryValue) && (iNotifyBinary2 == KBinaryValue);
						RDebug::Print(_L("CheckResult:  Value = %s\n"), &iNotifyBinary );
						break;
					}
				break;
				}

			default:
				RDebug::Print(_L("*** CheckResult Error: Test case %d is not implemented! ***\n"), aTestInfo.TestCase());
				result = EFalse;
				break;
			}
		if (!result)
			{
			iNotifyError = KErrAbort;
			}
		}

	if (iNotifyError == KErrNone)
		{
		RDebug::Print(_L("%s PASSED"), aTestInfo.TestCaseTitle());
		}
	else
		{
		RDebug::Print(_L("%s FAILED\niNotifyError:%d"), aTestInfo.TestCaseTitle(), iNotifyError );
		}

	return result;
	}

/**
	First step of the test stage
*/
void CCenRepNotifyHandlerTest::TestStartLC(TTestCaseInfo& aTestInfo)
	{
	iSession = CRepository::NewL(KTestUid);
	CleanupStack::PushL(iSession);

	RDebug::Print(_L("STARTING CCenRepNotifyHandler %s"), aTestInfo.TestCaseTitle());
	InitializeNotifyData();

	// Create notification handler
	switch (aTestInfo.KeyType())
		{
		case EKeyTypeInt:
			iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession, CCenRepNotifyHandler::EIntKey, KKey2);
			break;

		case EKeyTypeReal:
			iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession, CCenRepNotifyHandler::ERealKey, KKey3);
			break;

		case EKeyTypeString:
			iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession, CCenRepNotifyHandler::EStringKey, KKey1);
			break;

		case EKeyTypeBinary:
			iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession, CCenRepNotifyHandler::EBinaryKey, KKey4);
			break;

		case EKeyTypeAll:
			iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession);
			break;
		}

	CleanupStack::PushL(iNotifyHandler);
	}

/**
	Last step of the test stage
*/
void CCenRepNotifyHandlerTest::TestFinish(TTestCaseInfo& aTestInfo)
	{
	RDebug::Print(_L("Stop listening"));
	iNotifyHandler->StopListening();

	CleanupStack::PopAndDestroy(iNotifyHandler);
	CleanupStack::PopAndDestroy(iSession);

	RDebug::Print(_L("COMPLETED : CCenRepNotifyHandler %s\n\n"), aTestInfo.TestCaseTitle());     
	}

// Start the scheduler, and wait for a notification or timeout
void CCenRepNotifyHandlerTest::StartSchedulerL()
	{
	iNotifyError = KErrTimedOut; // if it will stop without reaching handlers, that will be a timeout
	//Timeout was increased from 2000 to 4000 due to longer time needed in DP builds.
	//More info in Demand Paging Engineering Guide or DEF126513
	CTestScheduler::StartAndWaitL(4000);
	}


///////////////////////////////////////////////////////////////////////////////
// Test functions
///////////////////////////////////////////////////////////////////////////////

/**
	1. Test how single Int key change generates notification.
	2. Test how notification is generated and handled for the entire keyspace.
	@param aTestInfo Test case info
	@return Symbian OS error code (test case execution error 
	  that is not returned as test case result in aResult)
*/
TInt CCenRepNotifyHandlerTest::ListenToKeyChangesL(TTestCaseInfo& aTestInfo)
	{
	TestStartLC(aTestInfo);

	// Start listening for notifications
	TInt ret = KErrNone;
	TRAP(ret, iNotifyHandler->StartListeningL());
	TESTLEAVE(ret,_L("ListenToKeyChangesL - StartListeningL: %d"));

	// Start the scheduler, and wait for the notification or for the timeout
	StartSchedulerL();

	// Check the result
	CheckResult(aTestInfo);
	
	TestFinish(aTestInfo);

	return iNotifyError;
	}

/**
	Test how notification is generated and handled for the entire keyspace.
	@param aTestInfo Test case info
	@return Symbian OS error code (test case execution error 
	  that is not returned as test case result in aResult)
*/
TInt CCenRepNotifyHandlerTest::ListenToAllKeysL(TTestCaseInfo& aTestInfo)
	{
	iSession = CRepository::NewL(KTestUid);
	CleanupStack::PushL(iSession);

	RDebug::Print(_L("STARTING CCenRepNotifyHandler %s"), aTestInfo.TestCaseTitle());
	InitializeNotifyData();

	// Create notification handler
	iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession);
	CleanupStack::PushL(iNotifyHandler);

	// Start listening for notifications
	TInt ret = KErrNone;
	TRAP(ret, iNotifyHandler->StartListeningL());
	TESTLEAVE(ret,_L("ListenToAllKeysL - StartListeningL: %d"));

	// Start the scheduler, and wait for the notification or for the timeout
	StartSchedulerL();

	// Check the result
	CheckResult(aTestInfo);

	TestFinish(aTestInfo);

	return iNotifyError;
	}

/**
	1. Test how the same value change generates notification.
	2. Test that there are no notifications after StopListening.
	@param aTestInfo Test case info
	@return Symbian OS error code (test case execution error 
	  that is not returned as test case result in aResult)
*/
TInt CCenRepNotifyHandlerTest::NoMultipleNotificationsL(TTestCaseInfo& aTestInfo)
	{
	TestStartLC(aTestInfo);

	// Start listening for notifications
	TInt ret = KErrNone;
	TRAP(ret, iNotifyHandler->StartListeningL());
	TESTLEAVE(ret,_L("NoMultipleNotificationsL - StartListeningL: %d"));

	// Start the scheduler, and wait for a notification or timeout
	StartSchedulerL();

	// Check the result
	if (CheckResult(aTestInfo))
		{
		// Start the scheduler, and wait for a notification or timeout
		StartSchedulerL();
		
		if (iNotifyError == KErrTimedOut) // should not get a notification
			{
			RDebug::Print(_L("%s (Step 2) PASSED"), aTestInfo.TestCaseTitle());
			iNotifyError = KErrNone;
			}
		else
			{
			RDebug::Print(_L("%s (Step 2) FAILED\niNotifyError:%d"),aTestInfo.TestCaseTitle(), iNotifyError );
			}
		}

	TestFinish(aTestInfo);
	
	return iNotifyError;
	}


/**
	Test that there are no notifications after StopListening.
	@param aTestInfo Test case info
	@return Symbian OS error code (test case execution error 
	  that is not returned as test case result in aResult)
*/
TInt CCenRepNotifyHandlerTest::NoNotificationAfterStopListeningL(TTestCaseInfo& aTestInfo)
	{
	TestStartLC(aTestInfo);

	// Start listening for notifications
	TInt ret = KErrNone;
	TRAP(ret, iNotifyHandler->StartListeningL());
	TESTLEAVE(ret,_L("NoNotificationAfterStopListeningL - StartListeningL: %d"));

	// Start the scheduler, and wait for a notification or timeout
	StartSchedulerL();

	// Check the result
	if (CheckResult(aTestInfo))
		{
		RDebug::Print(_L("Stop listening"));
		iNotifyHandler->StopListening();

		// Start the scheduler, and wait for a notification or timeout
		StartSchedulerL();
		
		if (iNotifyError == KErrTimedOut) // should not get a notification
			{
			RDebug::Print(_L("%s (Step 2) PASSED"), aTestInfo.TestCaseTitle());
			iNotifyError = KErrNone;
			}
		else
			{
			RDebug::Print(_L("%s (Step 2) FAILED\niNotifyError:%d"),aTestInfo.TestCaseTitle(), iNotifyError );
			}
		}

	TestFinish(aTestInfo);
	
	return iNotifyError;
	}


/**
	Test how two notification handlers receive notification.
	@param aTestInfo Test case info
	@return Symbian OS error code (test case execution error 
	  that is not returned as test case result in aResult)
*/
TInt CCenRepNotifyHandlerTest::TwoNotificationHandlersL(TTestCaseInfo& aTestInfo)
	{
	TestStartLC(aTestInfo);
	
	//create second handler for whole repository
	iNotifyHandler2 = CCenRepNotifyHandler::NewL(*this, *iSession);
	CleanupStack::PushL(iNotifyHandler2);

	iUseSecondHandler = ETrue;

	// Start listening for notifications
	TInt ret = KErrNone;
	TRAP(ret, iNotifyHandler->StartListeningL());
	TESTLEAVE(ret,_L("TwoNotificationHandlersL - StartListeningL 1: %d"));

	TRAP(ret, iNotifyHandler2->StartListeningL());
	TESTLEAVE(ret,_L("TwoNotificationHandlersL - StartListeningL 2: %d"));

	// Start the scheduler, and wait for the notification or for the timeout
	StartSchedulerL();

	// Check the result
	CheckResult(aTestInfo);

	CleanupStack::PopAndDestroy(iNotifyHandler2);

	TestFinish(aTestInfo);

	return iNotifyError;
	}


/**
	Handling notification for a non-existent key.
	@param aTestInfo Test case info
	@return Symbian OS error code (test case execution error 
	  that is not returned as test case result in aResult)
*/
TInt CCenRepNotifyHandlerTest::NonExistentKeyL(TTestCaseInfo& aTestInfo)
	{
	TInt ret = KErrNone;

	iSession = CRepository::NewL(KTestUid);
	CleanupStack::PushL(iSession);

	RDebug::Print(_L("STARTING CCenRepNotifyHandler %s"), aTestInfo.TestCaseTitle());
	InitializeNotifyData();

	iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession, CCenRepNotifyHandler::EIntKey, KKeyNotExists);

	CleanupStack::PushL(iNotifyHandler);

	// Start listening for notifications
	TRAP(ret, iNotifyHandler->StartListeningL());
	RDebug::Print(_L("Start listening: %d"), ret);

	if (ret == KErrNotFound)
		{
		RDebug::Print(_L("%s PASSED"), aTestInfo.TestCaseTitle());
		ret = KErrNone;
		}
	else
		{
		ret = KErrAbort;
		RDebug::Print(_L("%s FAILED\niNotifyError:%d"), aTestInfo.TestCaseTitle(), iNotifyError );
		}

	TestFinish(aTestInfo);

	return iNotifyError;
	}



/**
	Handling incorrect key type.
	@param aTestInfo Test case info
	@return Symbian OS error code (test case execution error 
	  that is not returned as test case result in aResult)
*/
TInt CCenRepNotifyHandlerTest::IncorrectKeyTypeL(TTestCaseInfo& aTestInfo)
	{
	TInt ret = KErrNone;

	iSession = CRepository::NewL(KTestUid);
	CleanupStack::PushL(iSession);

	RDebug::Print(_L("STARTING CCenRepNotifyHandler %S"), aTestInfo.TestCaseTitle());
	InitializeNotifyData();

	TRAP(ret, iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iSession, CCenRepNotifyHandler::TCenRepKeyType(10), KKey1));

	RDebug::Print(_L("ret = %d"), ret);

	if (ret == KErrArgument)
		{
		RDebug::Print(_L("%s PASSED"), aTestInfo.TestCaseTitle());
		ret = KErrNone;
		}
	else
		{
		ret = KErrAbort;
		delete iNotifyHandler; // NewL didn't fail, so we the object should be created
		RDebug::Print(_L("%s FAILED\niNotifyError:%d"), aTestInfo.TestCaseTitle(), iNotifyError );
		}

	CleanupStack::PopAndDestroy(iSession);
	
	RDebug::Print(_L("COMPLETED : CCenRepNotifyHandler %s\n\n"), aTestInfo.TestCaseTitle());     

	return iNotifyError;
	}


/**
	Test repeating calls to StartListening/StopListening.
	@param aTestInfo Test case info
	@return Symbian OS error code (test case execution error 
	  that is not returned as test case result in aResult)
*/
TInt CCenRepNotifyHandlerTest::RepeatingCallsL(TTestCaseInfo& aTestInfo)
	{
	TestStartLC(aTestInfo);

	// Start listening for notifications
	RDebug::Print(_L("Repeating calls to StartListening"));
	TInt ret = KErrNone;
	TRAP(ret, iNotifyHandler->StartListeningL());
	TESTLEAVE(ret,_L("RepeatingCallsL - StartListeningL 1: %d"));
	TRAP(ret, iNotifyHandler->StartListeningL());
	TESTLEAVE(ret,_L("RepeatingCallsL - StartListeningL 2: %d"));
	TRAP(ret, iNotifyHandler->StartListeningL());
	TESTLEAVE(ret,_L("RepeatingCallsL - StartListeningL 3: %d"));


	// Start the scheduler, and wait for the notification or for the timeout
	StartSchedulerL();

	// Check the result
	CheckResult(aTestInfo);

	RDebug::Print(_L("Repeating calls to StopListening"));
	iNotifyHandler->StopListening();
	iNotifyHandler->StopListening();
	TestFinish(aTestInfo);

	return iNotifyError;
	}


/**
	Cancelling request.
	@param aTestInfo Test case info
	@return Symbian OS error code (test case execution error 
	  that is not returned as test case result in aResult)
*/
TInt CCenRepNotifyHandlerTest::CancelRequestL(TTestCaseInfo& aTestInfo)
	{
	TestStartLC(aTestInfo);

	// Start listening for notifications
	TInt ret = KErrNone;
	TRAP(ret, iNotifyHandler->StartListeningL());
	TESTLEAVE(ret,_L("CancelRequestL - StartListeningL: %d"));

	// Finish will call StopListening, which will cancel the request
	TestFinish(aTestInfo);

	return iNotifyError;
	}


/**
	Explicit excersise of MCenRepNotifyHandlerCallback methods.
	@param aTestInfo Test case info
	@return Symbian OS error code (test case execution error 
	  that is not returned as test case result in aResult)
*/
TInt CCenRepNotifyHandlerTest::ExplicitExercise(TTestCaseInfo& aTestInfo)
	{
	RDebug::Print(_L("STARTING CCenRepNotifyHandler %s"), aTestInfo.TestCaseTitle());

	MCenRepNotifyHandlerCallback notifyHandlerCallback;

	notifyHandlerCallback.HandleNotifyInt( KKey2, 0 );
	notifyHandlerCallback.HandleNotifyReal( KKey3, 0 );
	_LIT16(KString, "string");	
	notifyHandlerCallback.HandleNotifyString( KKey1, KString );
	_LIT8(KBinary, "binary");	
	notifyHandlerCallback.HandleNotifyBinary( KKey4, KBinary );
	notifyHandlerCallback.HandleNotifyGeneric( KKey1 );
	notifyHandlerCallback.HandleNotifyGeneric( NCentralRepositoryConstants::KInvalidNotificationId );
	notifyHandlerCallback.HandleNotifyError( KKey1, KErrNone, NULL );

	RDebug::Print(_L("%s PASSED"), aTestInfo.TestCaseTitle());

	RDebug::Print(_L("COMPLETED : CCenRepNotifyHandler %s\n\n"), aTestInfo.TestCaseTitle());     

	return KErrNone;
	}

/**
	Explicit excersise of CCenRepNotifyHandler::RunError method.
	@param aTestInfo Test case info
	@return Symbian OS error code (test case execution error 
	  that is not returned as test case result in aResult)
*/
TInt CCenRepNotifyHandlerTest::ExplicitExerciseRunErrorL(TTestCaseInfo& aTestInfo)
	{
// This has been compiled out for this platform as GCC has a problem with the code.
// GCC wont compile this because it cannot call the parent class constructor
// (which has been marked private as is normal with the standard Symbian construction
// pattern). Ideally this test should be re-written with public method (under a test
// macro) in the parent class itself.
#ifndef __X86GCC__		
	class CCenRepNotifyHandler_TestRunError: public CCenRepNotifyHandler
		{
		public:
			TInt CallRunError(TInt aError)
				{
				return RunError(aError);
				}
		};

	iSession = CRepository::NewL(KTestUid);
	CleanupStack::PushL(iSession);

	RDebug::Print(_L("STARTING CCenRepNotifyHandler %s"), aTestInfo.TestCaseTitle());
	InitializeNotifyData();

	// Create notification handler
	// This kind of casting is not a good practice. But acceptable for this test.
	// Test for the whole repository
	CCenRepNotifyHandler_TestRunError* notifyHandler = reinterpret_cast<CCenRepNotifyHandler_TestRunError*>( CCenRepNotifyHandler::NewL(*this, *iSession) );
	notifyHandler->CallRunError(KErrNone);
	delete notifyHandler;

	// Test for one key
	notifyHandler = reinterpret_cast<CCenRepNotifyHandler_TestRunError*>( CCenRepNotifyHandler::NewL(*this, *iSession, CCenRepNotifyHandler::EIntKey, KKey2) );
	notifyHandler->CallRunError(KErrNone);
	delete notifyHandler;

	CleanupStack::PopAndDestroy(iSession);

	RDebug::Print(_L("%s PASSED"), aTestInfo.TestCaseTitle());

	RDebug::Print(_L("COMPLETED : CCenRepNotifyHandler %s\n\n"), aTestInfo.TestCaseTitle());     
#endif
	return KErrNone;
	}


//HANDLING NOTIFICATIONS    

// -----------------------------------------------------------------------------
// CCenRepNotifyHandlerTest::HandleNotifyInt
// -----------------------------------------------------------------------------
//
void CCenRepNotifyHandlerTest::HandleNotifyInt(TUint32 aId, TInt aNewValue)
	{
	iNotifyError = KErrNone;

	iNotifyInt = aNewValue;
	iNotificationId = aId;

	RDebug::Print(_L("HandleNotifyInt - Keyword: %d, Value: %d\n"), iNotificationId, iNotifyInt);

	// Got notification - can stop running now
	if (!iUseSecondHandler)
		{
		RDebug::Print(_L("HandleNotifyInt - Stop the scheduler\n"));
		CActiveScheduler::Stop();
		}
	}

// -----------------------------------------------------------------------------
// CCenRepNotifyHandlerTest::HandleNotifyReal
// -----------------------------------------------------------------------------
//
void CCenRepNotifyHandlerTest::HandleNotifyReal(TUint32 aId, TReal aNewValue)
	{
	iNotifyError = KErrNone;

	iNotifyReal = (TReal)aNewValue;
	iNotificationId = aId;
	RDebug::Print(_L("HandleNotifyReal - Keyword: %d, Value: %e"), iNotificationId, iNotifyReal);

	// Got notification - can stop running now
	if (!iUseSecondHandler)
		{
		RDebug::Print(_L("HandleNotifyReal - Stop the scheduler\n"));
		CActiveScheduler::Stop();
		}
	}

// -----------------------------------------------------------------------------
// CCenRepNotifyHandlerTest::HandleNotifyString
// -----------------------------------------------------------------------------
//
void CCenRepNotifyHandlerTest::HandleNotifyString(TUint32 aId, const TDesC16& aNewValue)
	{
	iNotifyError = KErrNone;

	iNotifyString.Append(aNewValue);
	iNotificationId = aId;
	RDebug::Print(_L("HandleNotifyString - Keyword: %d, Value: \"%S\""), iNotificationId, &iNotifyString);

	// Got notification - can stop running now
	if (!iUseSecondHandler)
		{
		RDebug::Print(_L("HandleNotifyString - Stop the scheduler\n"));
		CActiveScheduler::Stop();
		}
	}

// -----------------------------------------------------------------------------
// CCenRepNotifyHandlerTest::HandleNotifyBinary
// -----------------------------------------------------------------------------
//
void CCenRepNotifyHandlerTest::HandleNotifyBinary(TUint32 aId, const TDesC8& aNewValue)
	{
	iNotifyError = KErrNone;

	iNotifyBinary.Append(aNewValue);
	iNotificationId = aId;
	RDebug::Print(_L("HandleNotifyBinary - Keyword: %d, Value: \"%s\"\n"), iNotificationId, &iNotifyBinary);

	// Got notification - can stop running now
	if (!iUseSecondHandler)
		{
		RDebug::Print(_L("HandleNotifyBinary - Stop the scheduler\n"));
		CActiveScheduler::Stop();
		}
	}

// -----------------------------------------------------------------------------
// CCenRepNotifyHandlerTest::HandleNotifyGeneric
// -----------------------------------------------------------------------------
//
void CCenRepNotifyHandlerTest::HandleNotifyGeneric( TUint32 aId )
	{
	iNotifyError = KErrNone;
	iNotificationId = aId;

	switch (aId)
		{	
		case NCentralRepositoryConstants::KInvalidNotificationId:
			RDebug::Print(_L("KInvalidNotification"));
			/*Repository wide reset*/
			break;

		case KNewTestKey1:
			// The key was supposed to be deleted with reset, so we don't get its value here
			RDebug::Print(_L("KNewTestKey1 notification"));
			break;

		case KKey1:
			if (iUseSecondHandler)
				{
				iNotifyError = iSession->Get(KKey1, iNotifyString2);
				RDebug::Print(_L("KKey1 notification: \"%S\""), &iNotifyString2);
				}
			else
				{
				iNotifyError = iSession->Get(KKey1, iNotifyString);
				RDebug::Print(_L("KKey1 notification: \"%S\""), &iNotifyString);
				}
			break;

		case KKey2:
			if (iUseSecondHandler)
				{
				iNotifyError = iSession->Get(KKey2, iNotifyInt2);
				RDebug::Print(_L("KKey2 notification %d"), iNotifyInt2);
				}
			else
				{
				iNotifyError = iSession->Get(KKey2, iNotifyInt);
				RDebug::Print(_L("KKey2 notification %d"), iNotifyInt);
				}
			break;

		case KKey3:
			if (iUseSecondHandler)
				{
				iNotifyError = iSession->Get(KKey3, iNotifyReal2);
				RDebug::Print(_L("KKey3 notification %e"), iNotifyReal2);
				}
			else
				{
				iNotifyError = iSession->Get(KKey3, iNotifyReal);
				RDebug::Print(_L("KKey3 notification %e"), iNotifyReal);
				}
			break;

		case KKey4:
			if (iUseSecondHandler)
				{
				iNotifyError = iSession->Get(KKey4, iNotifyBinary2); 
				RDebug::Print(_L("KKey4 notification %s"), &iNotifyBinary2);
				}
			else
				{
				iNotifyError = iSession->Get(KKey4, iNotifyBinary); 
				RDebug::Print(_L("KKey4 notification %s"), &iNotifyBinary);
				}

		default:
			break;
		}


	// Got notification - can stop running now
	RDebug::Print(_L("HandleNotifyGeneric - Stop the scheduler\n"));
	CActiveScheduler::Stop();
	}

// -----------------------------------------------------------------------------
// CCenRepNotifyHandlerTest::HandleNotifyErrorL
// -----------------------------------------------------------------------------
//
void CCenRepNotifyHandlerTest::HandleNotifyErrorL(TUint32 aId, TInt error, CCenRepNotifyHandler* aHandler)
	{
	iNotifyError = error; 

	RDebug::Print(_L("HandleNotifyErrorL( Id=%d, error=%d, Handler=%x )"), aId, error, aHandler);

	return;
	}

