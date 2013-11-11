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
// The unit test class implementations for the CDefaultLogOutput class.
// 
//

#include "DefaultLogOutputUnitTest.h"

// ______________________________________________________________________________
//
_LIT(KDefaultLogOutputCreateAndDestroyUnitTest,"CDefaultLogOutput_CreateAndDestroy_UnitTest");

CDefaultLogOutput_CreateAndDestroy_UnitTest* CDefaultLogOutput_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDefaultLogOutput_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CDefaultLogOutput_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CDefaultLogOutput_CreateAndDestroy_UnitTest::RunError(TInt aError)
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

inline CDefaultLogOutput_CreateAndDestroy_UnitTest::~CDefaultLogOutput_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CDefaultLogOutput_CreateAndDestroy_UnitTest::CDefaultLogOutput_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDefaultLogOutputCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CDefaultLogOutput_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDefaultLogOutput_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDefaultLogOutput_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDefaultLogOutput_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TDefaultLogOutput_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CDefaultLogOutput_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CDefaultLogOutput_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KDefaultLogOutputOpenAndCloseUnitTest,"CDefaultLogOutput_OpenAndClose_UnitTest");

CDefaultLogOutput_OpenAndClose_UnitTest* CDefaultLogOutput_OpenAndClose_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDefaultLogOutput_OpenAndClose_UnitTest* self = 
					new(ELeave) CDefaultLogOutput_OpenAndClose_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CDefaultLogOutput_OpenAndClose_UnitTest::RunError(TInt aError)
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
	delete iOpenValidator;
	iOpenValidator = NULL;
	delete iCloseValidator;
	iCloseValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;
		
	return error;
	}

inline CDefaultLogOutput_OpenAndClose_UnitTest::~CDefaultLogOutput_OpenAndClose_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iOpenValidator;
	delete iCloseValidator;
	delete iDtorValidator;
	}

inline CDefaultLogOutput_OpenAndClose_UnitTest::CDefaultLogOutput_OpenAndClose_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDefaultLogOutputOpenAndCloseUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CDefaultLogOutput_OpenAndClose_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDefaultLogOutput_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDefaultLogOutput_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDefaultLogOutput_Ctor_TransitionValidator(*iUTContext);
	iOpenValidator = new(ELeave) TDefaultLogOutput_Open_TransitionValidator(*iUTContext);
	iCloseValidator = new(ELeave) TDefaultLogOutput_Close_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TDefaultLogOutput_Dtor_TransitionValidator(*iUTContext);
		
	AddTransitionL(new(ELeave)CDefaultLogOutput_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CDefaultLogOutput_OpenL_Transition(*iUTContext,*iOpenValidator));
	AddTransitionL(new(ELeave)CDefaultLogOutput_Close_Transition(*iUTContext,*iCloseValidator));
	AddTransitionL(new(ELeave)CDefaultLogOutput_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KDefaultLogOutputOpenWriteCloseUnitTest,"CDefaultLogOutput_OpenWriteClose_UnitTest");

CDefaultLogOutput_OpenWriteClose_UnitTest* CDefaultLogOutput_OpenWriteClose_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CDefaultLogOutput_OpenWriteClose_UnitTest* self = 
					new(ELeave) CDefaultLogOutput_OpenWriteClose_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CDefaultLogOutput_OpenWriteClose_UnitTest::RunError(TInt aError)
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
	delete iOpenValidator;
	iOpenValidator = NULL;
	delete iWriteValidator;
	iWriteValidator = NULL;
	delete iCloseValidator;
	iCloseValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;

	return error;
	}

inline CDefaultLogOutput_OpenWriteClose_UnitTest::~CDefaultLogOutput_OpenWriteClose_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iOpenValidator;
	delete iWriteValidator;
	delete iCloseValidator;
	delete iDtorValidator;
	}

inline CDefaultLogOutput_OpenWriteClose_UnitTest::CDefaultLogOutput_OpenWriteClose_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KDefaultLogOutputOpenWriteCloseUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CDefaultLogOutput_OpenWriteClose_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TDefaultLogOutput_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CDefaultLogOutput_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	
	// The text to be inserted into the test log file by the Write transition
	_LIT(KText, "This is a test log file entry");
	iUTContext->iOutput = &KText();

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TDefaultLogOutput_Ctor_TransitionValidator(*iUTContext);
	iOpenValidator = new(ELeave) TDefaultLogOutput_Open_TransitionValidator(*iUTContext);
	iWriteValidator = new(ELeave) TDefaultLogOutput_Write_TransitionValidator(*iUTContext);
	iCloseValidator = new(ELeave) TDefaultLogOutput_Close_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TDefaultLogOutput_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CDefaultLogOutput_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CDefaultLogOutput_OpenL_Transition(*iUTContext,*iOpenValidator));
	AddTransitionL(new(ELeave)CDefaultLogOutput_Write_Transition(*iUTContext,*iWriteValidator));
	AddTransitionL(new(ELeave)CDefaultLogOutput_Close_Transition(*iUTContext,*iCloseValidator));
	AddTransitionL(new(ELeave)CDefaultLogOutput_Dtor_Transition(*iUTContext,*iDtorValidator));
	}
