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
// The unit test class implementations for the CComponentInfo class.
// 
//

#include "ComponentInfoUnitTest.h"

// ______________________________________________________________________________
//
_LIT(KComponentInfoCreateAndDestroyUnitTest,"CComponentInfo_CreateAndDestroy_UnitTest");

CComponentInfo_CreateAndDestroy_UnitTest* CComponentInfo_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CComponentInfo_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CComponentInfo_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CComponentInfo_CreateAndDestroy_UnitTest::RunError(TInt aError)
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

inline CComponentInfo_CreateAndDestroy_UnitTest::~CComponentInfo_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CComponentInfo_CreateAndDestroy_UnitTest::CComponentInfo_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KComponentInfoCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CComponentInfo_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TComponentInfo_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CComponentInfo_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	iUTContext->iOwnParams = ETrue;
	//Leave function pointer at 0, assume CBase does this
	//iUTContext->iEntryFunc = iUTContext->iEntryFuncPostCheck = 0;
	iUTContext->iTestInfoArray = new(ELeave) RPointerArray<CUnitTestInfo>;

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TComponentInfo_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TComponentInfo_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CComponentInfo_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CComponentInfo_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KComponentInfoRetrieveTestNamesUnitTest,"CComponentInfo_RetrieveTestNames_UnitTest");

CComponentInfo_RetrieveTestNames_UnitTest* CComponentInfo_RetrieveTestNames_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CComponentInfo_RetrieveTestNames_UnitTest* self = 
					new(ELeave) CComponentInfo_RetrieveTestNames_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CComponentInfo_RetrieveTestNames_UnitTest::RunError(TInt aError)
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
	delete iEntryValidator;
	iEntryValidator = NULL;
	delete iUnitTestsInfoValidator;
	iUnitTestsInfoValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
	return error;
	}

inline CComponentInfo_RetrieveTestNames_UnitTest::~CComponentInfo_RetrieveTestNames_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iEntryValidator;
	delete iUnitTestsInfoValidator;
	delete iDtorValidator;
	}

inline CComponentInfo_RetrieveTestNames_UnitTest::CComponentInfo_RetrieveTestNames_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KComponentInfoRetrieveTestNamesUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CComponentInfo_RetrieveTestNames_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TComponentInfo_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CComponentInfo_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TComponentInfo_Ctor_TransitionValidator(*iUTContext);
	iEntryValidator = new(ELeave) TComponentInfo_Entry_TransitionValidator(*iUTContext);
	iUnitTestsInfoValidator = new(ELeave) TComponentInfo_UnitTestsInfo_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TComponentInfo_Dtor_TransitionValidator(*iUTContext);
	
	AddTransitionL(new(ELeave)CComponentInfo_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CComponentInfo_Entry_Transition(*iUTContext,*iEntryValidator));
	AddTransitionL(new(ELeave)CComponentInfo_UnitTestsInfo_Transition(*iUTContext,*iUnitTestsInfoValidator));
	AddTransitionL(new(ELeave)CComponentInfo_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

