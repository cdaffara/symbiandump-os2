// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The unit test class implementations for the CComponentTester class.
// 
//

#include "ComponentTesterUnitTest.h"

// ______________________________________________________________________________
//
_LIT(KComponentTesterCreateAndDestroyUnitTest,"CComponentTester_CreateAndDestroy_UnitTest");

CComponentTester_CreateAndDestroy_UnitTest* CComponentTester_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CComponentTester_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CComponentTester_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CComponentTester_CreateAndDestroy_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	/* delete any validators used */
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CComponentTester_CreateAndDestroy_UnitTest::~CComponentTester_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CComponentTester_CreateAndDestroy_UnitTest::CComponentTester_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KComponentTesterCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CComponentTester_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TComponentTester_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CComponentTester_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TComponentTester_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TComponentTester_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CComponentTester_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CComponentTester_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KComponentTesterManageUnitTestsUnitTest,"CComponentTester_ManageUnitTests_UnitTest");

CComponentTester_ManageUnitTests_UnitTest* CComponentTester_ManageUnitTests_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CComponentTester_ManageUnitTests_UnitTest* self = 
					new(ELeave) CComponentTester_ManageUnitTests_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CComponentTester_ManageUnitTests_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	/* delete any validators used */
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iTransitionSetsValidator;
	iTransitionSetsValidator = NULL;
	delete iAddUnitTestValidator;
	iAddUnitTestValidator = NULL;
	delete iAddParamUnitTestValidator;
	iAddParamUnitTestValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CComponentTester_ManageUnitTests_UnitTest::~CComponentTester_ManageUnitTests_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iTransitionSetsValidator;
	delete iAddUnitTestValidator;
	delete iAddParamUnitTestValidator;
	delete iDtorValidator;
	}

inline CComponentTester_ManageUnitTests_UnitTest::CComponentTester_ManageUnitTests_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KComponentTesterManageUnitTestsUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CComponentTester_ManageUnitTests_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TComponentTester_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CComponentTester_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TComponentTester_Ctor_TransitionValidator(*iUTContext);
	iTransitionSetsValidator = new(ELeave) TComponentTester_TransitionSets_TransitionValidator(*iUTContext);
	iAddUnitTestValidator = new(ELeave) TComponentTester_AddUnitTest_TransitionValidator(*iUTContext);
	iAddParamUnitTestValidator = new(ELeave) TComponentTester_AddParamUnitTest_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TComponentTester_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CComponentTester_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CComponentTester_TransitionSetsL_Transition(*iUTContext,*iTransitionSetsValidator));
	AddTransitionL(new(ELeave)CComponentTester_AddUnitTestL_Transition(*iUTContext,*iAddUnitTestValidator));
	AddTransitionL(new(ELeave)CComponentTester_AddParamUnitTestL_Transition(*iUTContext,*iAddParamUnitTestValidator));
	AddTransitionL(new(ELeave)CComponentTester_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KComponentTesterRunTestsUnitTest,"CComponentTester_RunTests_UnitTest");

CComponentTester_RunTests_UnitTest* CComponentTester_RunTests_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CComponentTester_RunTests_UnitTest* self = 
					new(ELeave) CComponentTester_RunTests_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CComponentTester_RunTests_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	/* delete any validators used */
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iTestComponentValidator;
	iTestComponentValidator = NULL;
	delete iRunValidator;
	iRunValidator = NULL;
	delete iCompleteValidator;
	iCompleteValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CComponentTester_RunTests_UnitTest::~CComponentTester_RunTests_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iTestComponentValidator;
	delete iRunValidator;
	delete iCompleteValidator;
	delete iDtorValidator;
	}

inline CComponentTester_RunTests_UnitTest::CComponentTester_RunTests_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KComponentTesterRunTestsUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CComponentTester_RunTests_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TComponentTester_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CComponentTester_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Specify the tests to be run by the TestComponent method. 0 means run all tests
//	iUTContext->iTests = 0;
	TTestInfo* testInfo = new(ELeave) TTestInfo;
	testInfo->iComponentId = 0;		// Run the first component found
	testInfo->iUnitTestId = 0;		// Run the first unit test from the the component found
	testInfo->iRunTime = 0;			// Run the test immediately
	testInfo->iParameters = NULL;
	User::LeaveIfError(iUTContext->iTests.Append(testInfo));

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TComponentTester_Ctor_TransitionValidator(*iUTContext);
	iTestComponentValidator = new(ELeave) TComponentTester_TestComponent_TransitionValidator(*iUTContext);
	iRunValidator = new(ELeave) TComponentTester_Run_TransitionValidator(*iUTContext);
	iCompleteValidator = new(ELeave) TComponentTester_Complete_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TComponentTester_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CComponentTester_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CComponentTester_TestComponent_Transition(*iUTContext,*iTestComponentValidator));
	AddTransitionL(new(ELeave)CComponentTester_RunL_Transition(*iUTContext,*iRunValidator));
	AddTransitionL(new(ELeave)CComponentTester_Complete_Transition(*iUTContext,*iCompleteValidator));
	AddTransitionL(new(ELeave)CComponentTester_Dtor_Transition(*iUTContext,*iDtorValidator));
	}


