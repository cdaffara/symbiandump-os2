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
// The unit test class implementations for the CTestController class.
// 
//

#include "TestControllerUnitTest.h"

// ______________________________________________________________________________
//
_LIT(KTestControllerCreateAndDestroyUnitTest,"CTestController_CreateAndDestroy_UnitTest");

CTestController_CreateAndDestroy_UnitTest* CTestController_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CTestController_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CTestController_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CTestController_CreateAndDestroy_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	// delete any validators used
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;

	return error;
	}

inline CTestController_CreateAndDestroy_UnitTest::~CTestController_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	iFs.Close();
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CTestController_CreateAndDestroy_UnitTest::CTestController_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KTestControllerCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

_LIT(KLogTitle,"Dummy Log title");

// Now the Individual transitions need to be added.
inline void CTestController_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TTestController_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CTestController_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	iUTContext->iScheduler = new(ELeave) CActiveScheduler;
	iUTContext->iLogInfo = new(ELeave) TLoggingInfo;
	iUTContext->iLogInfo->iTitle = &(KLogTitle);
	iUTContext->iLogInfo->iUseRDebug = ETrue;
	iUTContext->iLogInfo->iLogOutput = 0;
	iUTContext->iLogInfo->iReportOutput = 0;
	iUTContext->iLogInfo->iStyle = EHtml;

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TTestController_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TTestController_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CTestController_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CTestController_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KTestControllerFindTestsUnitTest,"CTestController_FindTests_UnitTest");

CTestController_FindTests_UnitTest* CTestController_FindTests_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CTestController_FindTests_UnitTest* self = 
					new(ELeave) CTestController_FindTests_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CTestController_FindTests_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	// delete any validators used
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iFindComponentsValidator;
	iFindComponentsValidator = NULL;
	delete iStartValidator;
	iStartValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;

	return error;
	}

inline CTestController_FindTests_UnitTest::~CTestController_FindTests_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	iFs.Close();
	delete iCtorValidator;
	delete iFindComponentsValidator;
	delete iStartValidator;
	delete iDtorValidator;
	}

inline CTestController_FindTests_UnitTest::CTestController_FindTests_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KTestControllerFindTestsUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CTestController_FindTests_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TTestController_StateAccessor;
	// Construct the Unit test context.

	// Open the required file server connection
	User::LeaveIfError(iFs.Connect());

	// Construct the Unit test context.
	iUTContext = new(ELeave) CTestController_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	iUTContext->iScheduler = new(ELeave) CActiveScheduler;
	iUTContext->iLogInfo = new(ELeave) TLoggingInfo;
	iUTContext->iLogInfo->iTitle = &(KLogTitle);
	iUTContext->iLogInfo->iUseRDebug = ETrue;
	iUTContext->iLogInfo->iLogOutput = 0;
	iUTContext->iLogInfo->iReportOutput = 0;
	iUTContext->iLogInfo->iStyle = EHtml;

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TTestController_Ctor_TransitionValidator(*iUTContext);
	iFindComponentsValidator = new(ELeave) TTestController_FindComponents_TransitionValidator(*iUTContext);
	iStartValidator = new(ELeave) TTestController_Start_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TTestController_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CTestController_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CTestController_FindComponents_Transition(*iUTContext, *iFindComponentsValidator));
	AddTransitionL(new(ELeave)CTestController_Start_Transition(*iUTContext, *iStartValidator));
	AddTransitionL(new(ELeave)CTestController_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KTestControllerCreateAndDestroyUnitTest_STUB,"CTestController_CreateAndDestroy_UnitTest_STUB");

CTestController_CreateAndDestroy_UnitTest_STUB* CTestController_CreateAndDestroy_UnitTest_STUB::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CTestController_CreateAndDestroy_UnitTest_STUB* self = 
					new(ELeave) CTestController_CreateAndDestroy_UnitTest_STUB(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CTestController_CreateAndDestroy_UnitTest_STUB::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base

	//Do nothing else since stub

	return error;
	}

inline CTestController_CreateAndDestroy_UnitTest_STUB::~CTestController_CreateAndDestroy_UnitTest_STUB()
	{
	//Do nothing
	}

inline CTestController_CreateAndDestroy_UnitTest_STUB::CTestController_CreateAndDestroy_UnitTest_STUB(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KTestControllerCreateAndDestroyUnitTest_STUB, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CTestController_CreateAndDestroy_UnitTest_STUB::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	//Do nothing else since stub
	}
