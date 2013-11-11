/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/



/**
 @file TEFUnit.inl
*/


#if !(defined TEF_LITE)
#include <basched.h>
#endif

template <class T>
inline TBool AssertEquals(const T& aExpected, const T& aActual)
/**
 * AssertEquals
 *
 * @publishedPartner
 * @test
 *
 * @param aExpected - Expected result
 * @param aActual - Actual result
 * @return - True if equal
 */
	{
	if( aExpected==aActual )
		{
		return ETrue;
		}
	return EFalse;
	}

template <class T>
inline TBool AssertNull(const T* aPtr)
/**
 * AssertNull
 *
 * @publishedPartner
 * @test
 *
 * @param aPtr - Pointer
 * @return - True if NULL
 */
	{
	if( aPtr==NULL )
		{
		return ETrue;
		}
	return EFalse;
	}

template <class T>
inline TBool AssertSame(const T* aExpectedPtr, const T* aActualPtr)
/**
 * AssertSame
 *
 * @publishedPartner
 * @test
 *
 * @param aExpectedPtr - Expected pointer
 * @param aActualPtr - Actual pointer
 * @return - True if equal
 */
	{
	if( aExpectedPtr==aActualPtr )
		{
		return ETrue;
		}
	return EFalse;
	}

inline TBool AssertTrue(const TBool& aCondition)
/**
 * AssertTrue
 *
 * @publishedPartner
 * @test
 *
 * @param aCondition - Condition
 * @return - True if aCondition is true
 */
	{
	if( !aCondition )
		{
		return EFalse;
		}
	return ETrue;
	}
	
	
template<class Fixture>
inline void AddTestCase(CTestSuite* aTestSuite, const TDesC& aName, void (Fixture::*aTestMethod)())
/**
 * AddTestCase
 *
 * @param aTestSuite - CTestSuite
 * @param aName - Test name
 * @param aTestMethod - Test method within the fixture
 */
	{
	CTestCaseTemplate<Fixture> *testCaseTemplate = new (ELeave) CTestCaseTemplate<Fixture>(aName, aTestMethod);
	CleanupStack::PushL(testCaseTemplate);
	aTestSuite->AddL( testCaseTemplate );
	CleanupStack::Pop(testCaseTemplate);
	}

template<class Fixture>
inline void AddAsyncTestCase(CTestSuite* aTestSuite, const TDesC& aName, void (Fixture::*aTestMethod)())
/**
 * AddAsyncTestCase
 *
 * @param aTestSuite - CTestSuite
 * @param aName - Test name
 * @param aTestMethod - Test method within the active fixture
 */
	{
	CAsyncTestCase<Fixture> *asyncTestCase = new (ELeave) CAsyncTestCase<Fixture>(aName, aTestMethod);
	CleanupStack::PushL(asyncTestCase);
	aTestSuite->AddL( asyncTestCase );
	CleanupStack::Pop(asyncTestCase);
	}

template <class Fixture>
inline CTestCaseTemplate<Fixture>::CTestCaseTemplate(const TDesC &aName, TestMethod aTestMethod )
/**
 * Constructor
 *
 * @param aName - Test name
 * @param aTestMethod - Test method
 */
	: CTestCase(aName), iTest(aTestMethod)
	{
	}

template <class Fixture>
inline CTestCaseTemplate<Fixture>::~CTestCaseTemplate()
/**
 * Destructor
 */
	{
	}

template <class Fixture>
inline void CTestCaseTemplate<Fixture>::RunL(CTestConfig& aConfig, CTestExecuteLogger& aLogger)
/**
 * RunL
 *
 * @param aConfig - Test configuration
 * @param aLogger - Test logger
 */
	{
	Fixture* iFixture = new (ELeave) Fixture();
	CleanupStack::PushL(iFixture);
	
	// Must set the Logger and Config now
	iFixture->SetLoggerL( aLogger );
	iFixture->SetConfigL( aConfig );
	
	// Setup code
	iFixture->SetupL();
	
	// Run the test
	TRAPD( err, (iFixture->*iTest)() );

	// Log the result
	CTEFLogger::LogResult( iName, err, aLogger );

	// Now the test case has been logged we need to leave
	// again if that is what the test did
	if( err != KErrTEFUnitPass )
		{
		User::Leave(err);
		}

	// Teardown code
	iFixture->TearDownL();

	CleanupStack::PopAndDestroy();
	}

template <class Fixture>
inline CActiveTester<Fixture>::CActiveTester(Fixture* aFixture, TestMethod aTestMethod, TTimeIntervalMicroSeconds32 /*aDelays[KNumberOfDelays]*/ )
/**
 * Constructor
 *
 * @param aFixture - Test fixture
 * @param aTestMethod - Test method within the fixture
 */
	:	CTimer(EPriorityStandard), iState(EConstruct), iFixture(aFixture),
		iTest(aTestMethod), iError(KErrTEFUnitPass)
	{
	for( TInt i=0 ; i<KNumberOfDelays ; i++ )
		{
		iDelays[i] = 1;
		}
	}

template <class Fixture>
inline CActiveTester<Fixture>::~CActiveTester()
/**
 * Destructor
 */
	{
	}

template <class Fixture>
inline void CActiveTester<Fixture>::RunL()
/**
 * RunL
 */
	{
	switch( iState )
		{
		case EConstruct:
			// Run generic startup code - delegate to the fixture
			iFixture->SetupL();
			iState = ERunTest;
			After( iDelays[0] );
			break;
		case ERunTest:
			// Run the test - delegate to the fixture
			(iFixture->*iTest)();
			iState = EDestroy;
			After( iDelays[1] );
			break;
		case EDestroy:
			// Run generic teardown code - delegate to the fixture
			iFixture->TearDownL();
			iState = EExit;
			After( iDelays[2] );
			break;
		case EExit:
			// Stop the Active Scheduler, test complete - delegate to the fixture
			iFixture->StopEnvL();
			return;
		default:
			break;
		}
	}

template <class Fixture>
inline TInt CActiveTester<Fixture>::RunError(TInt aError)
/**
 * RunError
 *
 * @return - Error
 */
	{
	if (aError != KErrNone)
		{
		#if !(defined TEF_LITE)	
		if(aError != KLeaveExit)
			{
		#endif
			iError = KErrTEFUnitFail;
			// Run generic teardown code - delegate to the fixture
			iFixture->TearDownL();
			// Stop the ActiveScheduler - delegate to the fixture
			iFixture->StopEnvL();
			return KErrNone;
		#if !(defined TEF_LITE)
			}
		else
			{
			return aError;
			}
		#endif
		}
	else
		{
		return aError;
		}
	}

template <class Fixture>
inline TInt CActiveTester<Fixture>::Error()
/**
 * Error
 *
 * @return - Error
 */
	{
	return iError;
	}

template <class Fixture>
inline void CActiveTester<Fixture>::ConstructEnvL()
/**
 * ConstructEnvL
 */
	{
	CTimer::ConstructL();
	// Delegate to the fixture
	iFixture->ConstructEnvL();
	}

template <class Fixture>
inline void CActiveTester<Fixture>::DestroyEnvL()
/**
 * DestroyEnvL
 */
	{
	// Delgate to the fixture
	iFixture->DestroyEnvL();
	}

template <class Fixture>
inline void CActiveTester<Fixture>::StartEnvL()
/**
 * StartEnvL
 */
	{
	After( iDelays[0] );
	// Delegate to the fixture
	iFixture->StartEnvL();
	}

template <class Fixture>
inline void CActiveTester<Fixture>::SetLoggerL(CTestExecuteLogger& aLogger)
/**
 * SetLoggerL
 *
 * @param aLogger - Test logger
 */
	{
	// Delegate to the fixture
	iFixture->SetLoggerL(aLogger);
	}

template <class Fixture>
inline void CActiveTester<Fixture>::SetConfigL(CTestConfig& aConfig)
/**
 * SetConfigL
 *
 * @param aConfig - test configuration
 */
	{
	// Delegate to the fixture
	iFixture->SetConfigL(aConfig);
	}


template <class Fixture>
inline CAsyncTestCase<Fixture>::CAsyncTestCase(const TDesC &aName, TestMethod aTestMethod )
/**
 * Constructor
 *
 * @param aName - Test name
 * @param aTestMethod - Test method within the fixture
 */
	: CTestCase(aName), iTest(aTestMethod)
	{
	}

template <class Fixture>
inline CAsyncTestCase<Fixture>::~CAsyncTestCase()
/**
 * Destructor
 */
	{
	}

template <class Fixture>
inline void CAsyncTestCase<Fixture>::RunL(CTestConfig& aConfig, CTestExecuteLogger& aLogger)
/**
 * RunL
 *
 * @param aConfig - Test configuration
 * @param aLogger - Test logger
 */
	{
	// Create the active Object
	Fixture *fixture = new (ELeave) Fixture();
	CleanupStack::PushL(fixture);
	CActiveTester<Fixture>* tester = new (ELeave) CActiveTester<Fixture>(fixture, iTest, 0);
	CleanupStack::PushL(tester);

	// Set the Logger and Config now
	tester->SetLoggerL( aLogger );
	tester->SetConfigL( aConfig );

	// Create the Active Scheduler Environment
	tester->ConstructEnvL();

	// Add the ActiveObject which has the test fixture
	CActiveScheduler::Add( tester );

	// Start the ActiveSheduler
	tester->StartEnvL();

	// When complete Teardown
	tester->DestroyEnvL();

	// Log the result
	CTEFLogger::LogResult( iName, tester->Error(), aLogger );

	// If an error was returned from the ActiveTester Leave here
	if( tester->Error() != KErrTEFUnitPass )
		{
		User::Leave( tester->Error() );
		}

	CleanupStack::PopAndDestroy(2, fixture);
	}
