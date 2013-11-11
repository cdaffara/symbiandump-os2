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
// The unit test class implementations for the CTestManager class.
// 
//

#include "TestManagerUnitTest.h"
#include "TestController.h"

extern CComponentTester* NewComponentTestLC_STUB(
	CDataLogger& aDataLogger,
	MComponentTestObserver&	aComponentTestObserver);

// ______________________________________________________________________________
//
_LIT(KTestManagerCreateAndDestroyUnitTest,"CTestManager_CreateAndDestroy_UnitTest");

CTestManager_CreateAndDestroy_UnitTest* CTestManager_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CTestManager_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CTestManager_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CTestManager_CreateAndDestroy_UnitTest::RunError(TInt aError)
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

inline CTestManager_CreateAndDestroy_UnitTest::~CTestManager_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CTestManager_CreateAndDestroy_UnitTest::CTestManager_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KTestManagerCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CTestManager_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TTestManager_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CTestManager_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iUTContext->iTestListArray = new(ELeave) RPointerArray<CComponentInfo>;
	// Construct, and store the observer stub
	iUTContext->iTestManagerObserverStub = CTestManagerObserverStub::NewL();

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TTestManager_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TTestManager_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CTestManager_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CTestManager_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KTestManagerManageTestsUnitTest,"CTestManager_ManageTests_UnitTest");

CTestManager_ManageTests_UnitTest* CTestManager_ManageTests_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CTestManager_ManageTests_UnitTest* self = 
					new(ELeave) CTestManager_ManageTests_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CTestManager_ManageTests_UnitTest::RunError(TInt aError)
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
	delete iRunTestsValidator;
	iRunTestsValidator = NULL;
	delete iTestComponentValidator;
	iTestComponentValidator =NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;

	return error;
	}

inline CTestManager_ManageTests_UnitTest::~CTestManager_ManageTests_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iRunTestsValidator;
	delete iTestComponentValidator;
	delete iDtorValidator;
	}

inline CTestManager_ManageTests_UnitTest::CTestManager_ManageTests_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KTestManagerManageTestsUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CTestManager_ManageTests_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TTestManager_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CTestManager_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iUTContext->iTestListArray = new(ELeave) RPointerArray<CComponentInfo>;
	// Construct, and store the observer stub
	iUTContext->iTestManagerObserverStub = CTestManagerObserverStub::NewL();

	CComponentInfo* compInfo = CComponentInfo::NewL(NewComponentTestLC_STUB, NULL);
	CleanupStack::PushL(compInfo);
	
	User::LeaveIfError(iUTContext->iTestListArray->Append(compInfo));
	CleanupStack::Pop(compInfo);


	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TTestManager_Ctor_TransitionValidator(*iUTContext);
	iRunTestsValidator = new(ELeave) TTestManager_RunTests_TransitionValidator(*iUTContext);
	iTestComponentValidator = new(ELeave) TTestManager_TestComponent_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TTestManager_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CTestManager_NewL_Transition(*iUTContext,*iCtorValidator));
//	AddTransitionL(new(ELeave)CTestManager_RunTests_Transition(*iUTContext,*iRunTestsValidator));
	AddTransitionL(new(ELeave)CTestManager_TestComponentL_Transition(*iUTContext,*iTestComponentValidator));
	AddTransitionL(new(ELeave)CTestManager_Dtor_Transition(*iUTContext,*iDtorValidator));

	}

// ______________________________________________________________________________
//
_LIT(KTestManagerCreateAndDestroyUnitTest_STUB,"CTestManager_CreateAndDestroy_UnitTest_STUB");

CTestManager_CreateAndDestroy_UnitTest_STUB* CTestManager_CreateAndDestroy_UnitTest_STUB::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CTestManager_CreateAndDestroy_UnitTest_STUB* self = 
					new(ELeave) CTestManager_CreateAndDestroy_UnitTest_STUB(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CTestManager_CreateAndDestroy_UnitTest_STUB::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base

	//Do nothing else since stub

	return error;
	}

inline CTestManager_CreateAndDestroy_UnitTest_STUB::~CTestManager_CreateAndDestroy_UnitTest_STUB()
	{
	//Do nothing
	}

inline CTestManager_CreateAndDestroy_UnitTest_STUB::CTestManager_CreateAndDestroy_UnitTest_STUB(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KTestManagerCreateAndDestroyUnitTest_STUB, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CTestManager_CreateAndDestroy_UnitTest_STUB::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	//Do nothing else since stub
	}



