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
// The unit test class implementations for the CUnitTest class.
// 
//

#include "UnitTestUnitTest.h"

// ______________________________________________________________________________
//
_LIT(KUnitTestCreateAndDestroyUnitTest,"CUnitTest_CreateAndDestroy_UnitTest");

CUnitTest_CreateAndDestroy_UnitTest* CUnitTest_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CUnitTest_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CUnitTest_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CUnitTest_CreateAndDestroy_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CUnitTest_CreateAndDestroy_UnitTest::~CUnitTest_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CUnitTest_CreateAndDestroy_UnitTest::CUnitTest_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KUnitTestCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CUnitTest_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TUnitTest_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CUnitTest_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TUnitTest_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TUnitTest_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CUnitTest_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CUnitTest_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestManageTransitionsUnitTest,"CUnitTest_ManageTransitions_UnitTest");

CUnitTest_ManageTransitions_UnitTest* CUnitTest_ManageTransitions_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CUnitTest_ManageTransitions_UnitTest* self = 
					new(ELeave) CUnitTest_ManageTransitions_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CUnitTest_ManageTransitions_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iTransitionSetValidator;
	iTransitionSetValidator = NULL;
	delete iGetCurrentTransitionValidator;
	iGetCurrentTransitionValidator = NULL;
	delete iSetCurrentTransitionValidator;
	iSetCurrentTransitionValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CUnitTest_ManageTransitions_UnitTest::~CUnitTest_ManageTransitions_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iTransitionSetValidator;
	delete iGetCurrentTransitionValidator;
	delete iSetCurrentTransitionValidator;
	delete iDtorValidator;
	}

inline CUnitTest_ManageTransitions_UnitTest::CUnitTest_ManageTransitions_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KUnitTestManageTransitionsUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CUnitTest_ManageTransitions_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TUnitTest_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CUnitTest_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TUnitTest_Ctor_TransitionValidator(*iUTContext);
	iTransitionSetValidator = new(ELeave) TUnitTest_TransitionSet_TransitionValidator(*iUTContext);
	iGetCurrentTransitionValidator = new(ELeave) TUnitTest_GetCurrentTransition_TransitionValidator(*iUTContext);
	iSetCurrentTransitionValidator = new(ELeave) TUnitTest_SetCurrentTransition_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TUnitTest_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CUnitTest_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CUnitTest_TransitionSetL_Transition(*iUTContext,*iTransitionSetValidator));
	AddTransitionL(new(ELeave)CUnitTest_GetCurrentTransition_Transition(*iUTContext,*iGetCurrentTransitionValidator));
	AddTransitionL(new(ELeave)CUnitTest_SetCurrentTransition_Transition(*iUTContext,*iSetCurrentTransitionValidator));
	AddTransitionL(new(ELeave)CUnitTest_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestRunTestsUnitTest,"CUnitTest_RunTests_UnitTest");

CUnitTest_RunTests_UnitTest* CUnitTest_RunTests_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CUnitTest_RunTests_UnitTest* self = 
					new(ELeave) CUnitTest_RunTests_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CUnitTest_RunTests_UnitTest::RunError(TInt aError)
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
	delete iSetParametersValidator;
	iSetParametersValidator = NULL;
	delete iPrepareUnitTestValidator;
	iPrepareUnitTestValidator = NULL;
	delete iRunTestValidator;
	iRunTestValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CUnitTest_RunTests_UnitTest::~CUnitTest_RunTests_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iSetParametersValidator;
	delete iPrepareUnitTestValidator;
	delete iRunTestValidator;
	delete iDtorValidator;
	}

inline CUnitTest_RunTests_UnitTest::CUnitTest_RunTests_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KUnitTestRunTestsUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CUnitTest_RunTests_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TUnitTest_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CUnitTest_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TUnitTest_Ctor_TransitionValidator(*iUTContext);
	iSetParametersValidator = new(ELeave) TUnitTest_SetParameters_TransitionValidator(*iUTContext);
	iPrepareUnitTestValidator = new(ELeave) TUnitTest_PrepareUnitTest_TransitionValidator(*iUTContext);
	iRunTestValidator = new(ELeave) TUnitTest_RunTest_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TUnitTest_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CUnitTest_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CUnitTest_SetParametersL_Transition(*iUTContext,*iSetParametersValidator));
	AddTransitionL(new(ELeave)CUnitTest_PrepareUnitTestL_Transition(*iUTContext,*iPrepareUnitTestValidator));
	AddTransitionL(new(ELeave)CUnitTest_RunTest_Transition(*iUTContext,*iRunTestValidator));
	AddTransitionL(new(ELeave)CUnitTest_Dtor_Transition(*iUTContext,*iDtorValidator));
	}


// ______________________________________________________________________________
//
_LIT(KUnitTestAllOtherUnitTest,"CUnitTest_AllOther_UnitTest");

CUnitTest_AllOther_UnitTest* CUnitTest_AllOther_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CUnitTest_AllOther_UnitTest* self = 
					new(ELeave) CUnitTest_AllOther_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CUnitTest_AllOther_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iAddTransitionValidator;
	iAddTransitionValidator = NULL;
	delete iAddBlockingTransitionValidator;
	iAddBlockingTransitionValidator = NULL;
	delete iAddLeaveErrorCodeValidator;
	iAddLeaveErrorCodeValidator = NULL;
	delete iUnitTestNameValidator;
	iUnitTestNameValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;

	return error;
	}

inline CUnitTest_AllOther_UnitTest::~CUnitTest_AllOther_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iAddTransitionValidator;
	delete iAddBlockingTransitionValidator;
	delete iAddLeaveErrorCodeValidator;
	delete iUnitTestNameValidator;
	delete iDtorValidator;
	}

inline CUnitTest_AllOther_UnitTest::CUnitTest_AllOther_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KUnitTestAllOtherUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CUnitTest_AllOther_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TUnitTest_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CUnitTest_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TUnitTest_Ctor_TransitionValidator(*iUTContext);
	iAddTransitionValidator = new(ELeave) TUnitTest_AddTransition_TransitionValidator(*iUTContext);
	iAddBlockingTransitionValidator = new(ELeave) TUnitTest_AddBlockingTransition_TransitionValidator(*iUTContext);
	iAddLeaveErrorCodeValidator = new(ELeave) TUnitTest_AddLeaveErrorCode_TransitionValidator(*iUTContext);
	iUnitTestNameValidator = new(ELeave) TUnitTest_UnitTestName_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TUnitTest_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CUnitTest_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CUnitTest_AddTransitionL_Transition(*iUTContext,*iAddTransitionValidator));
	AddTransitionL(new(ELeave)CUnitTest_AddBlockingTransitionL_Transition(*iUTContext,*iAddBlockingTransitionValidator));
	AddTransitionL(new(ELeave)CUnitTest_AddLeaveErrorCodeL_Transition(*iUTContext,*iAddLeaveErrorCodeValidator));
	AddTransitionL(new(ELeave)CUnitTest_UnitTestName_Transition(*iUTContext,*iUnitTestNameValidator));
	AddTransitionL(new(ELeave)CUnitTest_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

