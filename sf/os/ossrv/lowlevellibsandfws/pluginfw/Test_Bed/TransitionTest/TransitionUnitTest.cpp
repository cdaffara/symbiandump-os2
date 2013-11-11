// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The unit test class implementations for the CTransition class.
// 
//

#include "TransitionUnitTest.h"

// ______________________________________________________________________________
//
_LIT(KTransitionCreateAndDestroyUnitTest,"CTransition_CreateAndDestroy_UnitTest");

CTransition_CreateAndDestroy_UnitTest* CTransition_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CTransition_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CTransition_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CTransition_CreateAndDestroy_UnitTest::RunError(TInt aError)
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

inline CTransition_CreateAndDestroy_UnitTest::~CTransition_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CTransition_CreateAndDestroy_UnitTest::CTransition_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KTransitionCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CTransition_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TTransition_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CTransition_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Provide a valid context and validator for the target transition class under test.
	iUTContext->iTargetContext = new(ELeave) CTargetTransition_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iUTContext->iTargetValidator  = new(ELeave) TTransition_NULL_TransitionValidator(*(iUTContext->iTargetContext));

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TTransition_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_NewL_Transition(*iUTContext,*iCtorValidator));

	iDtorValidator = new(ELeave) TTransition_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KTransitionTransitMethodLUnitTest,"CTransition_TransitMethodL_UnitTest");

CTransition_TransitMethodL_UnitTest* CTransition_TransitMethodL_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CTransition_TransitMethodL_UnitTest* self = 
					new(ELeave) CTransition_TransitMethodL_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CTransition_TransitMethodL_UnitTest::RunError(TInt aError)
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
	delete iTransitMethodLValidator;
	iTransitMethodLValidator = NULL;
	delete iRepeatOnceValidator;
	iRepeatOnceValidator = NULL;
	delete iTransitionIdValidator;
	iTransitionIdValidator = NULL;
	delete iTransitionInfoValidator;
	iTransitionInfoValidator = NULL;
	delete iIsBlockingValidator;
	iIsBlockingValidator = NULL;
	delete iSetBlockingValidator;
	iSetBlockingValidator = NULL;
	delete iTransitMethodLValidator;
	iTransitMethodLValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CTransition_TransitMethodL_UnitTest::~CTransition_TransitMethodL_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iTransitionIdValidator;
	delete iTransitionInfoValidator;
	delete iIsBlockingValidator;
	delete iSetBlockingValidator;
	delete iTransitMethodLValidator;
	delete iRepeatOnceValidator;
	delete iDtorValidator;
	}

inline CTransition_TransitMethodL_UnitTest::CTransition_TransitMethodL_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KTransitionTransitMethodLUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CTransition_TransitMethodL_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TTransition_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CTransition_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	// Provide a valid context and validator for the target transition class under test.
	iUTContext->iTargetContext = new(ELeave) CTargetTransition_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iUTContext->iTargetValidator  = new(ELeave) TTransition_NULL_TransitionValidator(*(iUTContext->iTargetContext));
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TTransition_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_NewL_Transition(*iUTContext,*iCtorValidator));

	// Test the flag setting
	iTransitionIdValidator = new(ELeave) TTransition_TransitionId_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_TransitionId_Transition(*iUTContext,*iTransitionIdValidator));
	iTransitionInfoValidator = new(ELeave) TTransition_TransitionInfo_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_TransitionInfo_Transition(*iUTContext,*iTransitionInfoValidator));
	iIsBlockingValidator = new(ELeave) TTransition_IsBlocking_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_IsBlocking_Transition(*iUTContext,*iIsBlockingValidator));
	iSetBlockingValidator = new(ELeave) TTransition_SetBlocking_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_SetBlocking_Transition(*iUTContext,*iSetBlockingValidator));
	AddTransitionL(new(ELeave)CTransition_IsBlocking_Transition(*iUTContext,*iIsBlockingValidator));
	AddTransitionL(new(ELeave)CTransition_SetBlocking_Transition(*iUTContext,*iSetBlockingValidator));
	AddTransitionL(new(ELeave)CTransition_IsBlocking_Transition(*iUTContext,*iIsBlockingValidator));

	// The transit
	iTransitMethodLValidator = new(ELeave) TTransition_TransitMethodL_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_TransitMethodL_Transition(*iUTContext,*iTransitMethodLValidator));
	// The repeat flag
	iRepeatOnceValidator = new(ELeave) TTransition_RepeatOnce_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_RepeatOnce_Transition(*iUTContext,*iRepeatOnceValidator));
	AddTransitionL(new(ELeave)CTransition_TransitMethodL_Transition(*iUTContext,*iTransitMethodLValidator));
	// We are expecting the Abort code on the repeat call So no dtor required.
	AddLeaveErrorCodeL(KErrAbort);
	}


// ______________________________________________________________________________
//
_LIT(KTransitionAsyncOperationUnitTest,"CTransition_AsyncOperation_UnitTest");

CTransition_AsyncOperation_UnitTest* CTransition_AsyncOperation_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CTransition_AsyncOperation_UnitTest* self = 
					new(ELeave) CTransition_AsyncOperation_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CTransition_AsyncOperation_UnitTest::RunError(TInt aError)
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
	delete iRunTransitionValidator;
	iRunTransitionValidator = NULL;
	delete iCancelValidator;
	iCancelValidator = NULL;
	delete iGetErrorCodeValidator;
	iGetErrorCodeValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CTransition_AsyncOperation_UnitTest::~CTransition_AsyncOperation_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iRunTransitionValidator;
	delete iCancelValidator;
	delete iGetErrorCodeValidator;
	delete iDtorValidator;
	}

inline CTransition_AsyncOperation_UnitTest::CTransition_AsyncOperation_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KTransitionAsyncOperationUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CTransition_AsyncOperation_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TTransition_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CTransition_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	// Provide a valid context and validator for the target transition class under test.
	iUTContext->iTargetContext = new(ELeave) CTargetTransition_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iUTContext->iTargetValidator  = new(ELeave) TTransition_NULL_TransitionValidator(*(iUTContext->iTargetContext));
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	/*	Examples of C'tor and D'tor transitions on CTransition class.
		using ctor and dtor validators */
	iCtorValidator = new(ELeave) TTransition_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_NewL_Transition(*iUTContext,*iCtorValidator));

	// Run the target transition asynchronously, cancel, check the error code, 
	// and then run asynchronously again to completion
	iRunTransitionValidator = new(ELeave) TTransition_RunTransition_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_RunTransition_Transition(*iUTContext,*iRunTransitionValidator));
	iCancelValidator = new(ELeave) TTransition_Cancel_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_Cancel_Transition(*iUTContext,*iRunTransitionValidator));
	iGetErrorCodeValidator = new(ELeave) TTransition_GetErrorCode_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CTransition_GetErrorCode_Transition(*iUTContext,*iGetErrorCodeValidator));
	AddTransitionL(new(ELeave)CTransition_RunTransition_Transition(*iUTContext,*iRunTransitionValidator));

	iDtorValidator = new(ELeave) TTransition_Dtor_TransitionValidator(*iUTContext);
	AddBlockingTransitionL(new(ELeave)CTransition_Dtor_Transition(*iUTContext,*iDtorValidator));
	}
