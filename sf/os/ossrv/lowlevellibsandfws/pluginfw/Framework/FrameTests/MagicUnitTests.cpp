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
// This file contains the definition of the 
// class CAllTransitionsUnitTest	
// Note	: All the REComSession references from this file
// These are simple fudges for early development tests
// before the true server migration of the code.
// 
//

#include <e32uid.h>

#include "TestUtilities.h"	// For __FILE__LINE__
#include "Interface.h"
#include <ecom/test_bed/datalogger.h>

#include "MagicUnitTests.h"
#include "MagicTransitions.h"
#include "MagicTransitionValidation.h"

#include "TlsData.h"		// For GlobalData

#define UNUSED_VAR(a) a = a

// ______________________________________________________________________________
//
_LIT(KInterfaceCreateAndDestroyUnitTest,"CExampleInterfaceCreateAndDestroyUnitTest");

CExampleInterfaceCreateAndDestroyUnitTest* CExampleInterfaceCreateAndDestroyUnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CExampleInterfaceCreateAndDestroyUnitTest* self = 
					new(ELeave) CExampleInterfaceCreateAndDestroyUnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	// Chain to the base which calls the ConstructL
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

TInt CExampleInterfaceCreateAndDestroyUnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	delete iCtorValidator;
	delete iDtorValidator;
	iCtorValidator = 0;
	iDtorValidator = 0;

	return error;
	}

CExampleInterfaceCreateAndDestroyUnitTest::~CExampleInterfaceCreateAndDestroyUnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iDtorValidator;

	}

CExampleInterfaceCreateAndDestroyUnitTest::CExampleInterfaceCreateAndDestroyUnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KInterfaceCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
void CExampleInterfaceCreateAndDestroyUnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TExampleInterface_StateAccessor();
	
	// context
	iUTContext = new(ELeave) CExampleInterface_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TExampleInterface_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CExampleInterfaceNewLTransition(*iUTContext,*iCtorValidator));
	
	iDtorValidator = new(ELeave) TExampleInterface_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CExampleInterfaceDtorTransition(*iUTContext,*iDtorValidator));
	}
// ______________________________________________________________________________
//
_LIT(KExampleInterfaceAltCreateAndDestroyUnitTest,"CExampleInterfaceAltCreateAndDestroyUnitTest");

CExampleInterfaceAltCreateAndDestroyUnitTest* CExampleInterfaceAltCreateAndDestroyUnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CExampleInterfaceAltCreateAndDestroyUnitTest* self = 
					new(ELeave) CExampleInterfaceAltCreateAndDestroyUnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	// Chain to the base which calls the ConstructL
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

TInt CExampleInterfaceAltCreateAndDestroyUnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;

	return error;
	}

CExampleInterfaceAltCreateAndDestroyUnitTest::~CExampleInterfaceAltCreateAndDestroyUnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iDtorValidator;

	}

CExampleInterfaceAltCreateAndDestroyUnitTest::CExampleInterfaceAltCreateAndDestroyUnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KExampleInterfaceAltCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
void CExampleInterfaceAltCreateAndDestroyUnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TExampleInterface_StateAccessor();
	// context
	iUTContext = new(ELeave) CExampleInterface_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Create the required validators
	iCtorValidator = new(ELeave) TExampleInterface_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TExampleInterface_Dtor_TransitionValidator(*iUTContext);

	// Add the transitions in the order they are to run
	AddTransitionL(new(ELeave)CExampleInterfaceNewWPLTransition(*iUTContext, *iCtorValidator));
	AddTransitionL(new(ELeave)CExampleInterfaceDtorTransition(*iUTContext, *iDtorValidator));

	AddTransitionL(new(ELeave)CExampleInterfaceNewResolveLTransition(*iUTContext, *iCtorValidator));
	AddTransitionL(new(ELeave)CExampleInterfaceDtorTransition(*iUTContext, *iDtorValidator));

	}

// ______________________________________________________________________________
//
_LIT(KExampleInterfaceListAllImplementationsUnitTest,"CExampleInterface_ListImplementations_UnitTest");

CExampleInterface_ListImplementations_UnitTest* CExampleInterface_ListImplementations_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CExampleInterface_ListImplementations_UnitTest* self = 
					new(ELeave) CExampleInterface_ListImplementations_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CExampleInterface_ListImplementations_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iListImplementationsValidator;
	iListImplementationsValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;

	return error;
	}

inline CExampleInterface_ListImplementations_UnitTest::~CExampleInterface_ListImplementations_UnitTest()
	{
	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	delete iListImplementationsValidator;
	delete iDtorValidator;
	}

inline CExampleInterface_ListImplementations_UnitTest::CExampleInterface_ListImplementations_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KExampleInterfaceListAllImplementationsUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CExampleInterface_ListImplementations_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TExampleInterface_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CExampleInterface_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	//	Examples of C'tor and D'tor transitions on CExampleInterface class.
	//	using ctor and dtor validators
	iCtorValidator = new(ELeave) TExampleInterface_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CExampleInterfaceNewLTransition(*iUTContext,*iCtorValidator));
	
	iListImplementationsValidator = new(ELeave) TExampleInterface_ListImplementations_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CExampleInterfaceListAllImplementationsLTransition(*iUTContext,*iListImplementationsValidator));
	AddTransitionL(new(ELeave)CExampleInterfaceListImplementationsLTransition(*iUTContext,*iListImplementationsValidator));
	
	iDtorValidator = new(ELeave) TExampleInterface_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CExampleInterfaceDtorTransition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KExampleInterfaceDefectHAN4WZHSYUnitTest,"CExampleInterface_DefectHAN4WZHSY_UnitTest");

CExampleInterface_DefectHAN4WZHSY_UnitTest* CExampleInterface_DefectHAN4WZHSY_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CExampleInterface_DefectHAN4WZHSY_UnitTest* self = 
					new(ELeave) CExampleInterface_DefectHAN4WZHSY_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CExampleInterface_DefectHAN4WZHSY_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	
	iUTContext = 0;
	iStateAccessor = 0;

	delete iCtorValidator;
	delete iDtorValidator;

	iCtorValidator = 0;
	iDtorValidator = 0;
	
	return error;
	}

inline CExampleInterface_DefectHAN4WZHSY_UnitTest::~CExampleInterface_DefectHAN4WZHSY_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CExampleInterface_DefectHAN4WZHSY_UnitTest::CExampleInterface_DefectHAN4WZHSY_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KExampleInterfaceDefectHAN4WZHSYUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CExampleInterface_DefectHAN4WZHSY_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TExampleInterface_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CExampleInterface_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	iCtorValidator = new(ELeave) TExampleInterface_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TExampleInterface_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CExampleInterfaceNewLTransition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CExampleInterfaceDtorTransition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KExampleInterfaceDefectCUO4YCEUEUnitTest,"CExampleInterface_DefectCUO4YCEUE_UnitTest");

CExampleInterface_DefectCUO4YCEUE_UnitTest* CExampleInterface_DefectCUO4YCEUE_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CExampleInterface_DefectCUO4YCEUE_UnitTest* self = 
					new(ELeave) CExampleInterface_DefectCUO4YCEUE_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CExampleInterface_DefectCUO4YCEUE_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
    delete iUTContext;
    delete iStateAccessor;

    delete iCtorValidator;
    delete iDtorValidator;

    iUTContext = 0;
    iStateAccessor = 0;

    iCtorValidator = 0;
    iDtorValidator = 0;

	return error;
	}

inline CExampleInterface_DefectCUO4YCEUE_UnitTest::~CExampleInterface_DefectCUO4YCEUE_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CExampleInterface_DefectCUO4YCEUE_UnitTest::CExampleInterface_DefectCUO4YCEUE_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KExampleInterfaceDefectCUO4YCEUEUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CExampleInterface_DefectCUO4YCEUE_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TExampleInterface_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CExampleInterface_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	iCtorValidator = new(ELeave) TExampleInterface_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TExampleInterface_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CExampleInterfaceDoubleNewLTransition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CExampleInterfaceDoubleDtorTransition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KExampleInterfaceDefectEVS4Z9BPGUnitTest,"CExampleInterface_DefectEVS4Z9BPG_UnitTest");

CExampleInterface_DefectEVS4Z9BPG_UnitTest* CExampleInterface_DefectEVS4Z9BPG_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CExampleInterface_DefectEVS4Z9BPG_UnitTest* self = 
					new(ELeave) CExampleInterface_DefectEVS4Z9BPG_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CExampleInterface_DefectEVS4Z9BPG_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
    delete iUTContext;
    delete iStateAccessor;

    delete iCtorValidator;
    delete iDefaultValidator;
    delete iDtorValidator;

    iUTContext = 0;
    iStateAccessor = 0;

    iCtorValidator = 0;
    iDefaultValidator = 0;
    iDtorValidator = 0;

	return error;
	}

inline CExampleInterface_DefectEVS4Z9BPG_UnitTest::~CExampleInterface_DefectEVS4Z9BPG_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	delete iCtorValidator;
	delete iDefaultValidator;
	delete iDtorValidator;
	}

inline CExampleInterface_DefectEVS4Z9BPG_UnitTest::CExampleInterface_DefectEVS4Z9BPG_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KExampleInterfaceDefectEVS4Z9BPGUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CExampleInterface_DefectEVS4Z9BPG_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TExampleInterface_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CExampleInterface_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	iCtorValidator = new(ELeave) TExampleInterface_Ctor_TransitionValidator(*iUTContext);
	iDefaultValidator = new(ELeave) TExampleInterface_Default_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TExampleInterface_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CExampleInterfaceDoubleNewLTransition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CExampleInterfaceDoMethodLTransition(*iUTContext,*iDefaultValidator));
	AddTransitionL(new(ELeave)CExampleInterfaceDoubleDtorTransition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KExampleInterfaceDefectKRN53SL4QUnitTest,"CExampleInterface_DefectKRN53SL4Q_UnitTest");

CExampleInterface_DefectKRN53SL4Q_UnitTest* CExampleInterface_DefectKRN53SL4Q_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CExampleInterface_DefectKRN53SL4Q_UnitTest* self = 
					new(ELeave) CExampleInterface_DefectKRN53SL4Q_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CExampleInterface_DefectKRN53SL4Q_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
    delete iUTContext;
    delete iStateAccessor;

    delete iCtorValidator;
    delete iDefaultValidator;
    delete iDtorValidator;

    iUTContext = 0;
    iStateAccessor = 0;

    iCtorValidator = 0;
    iDefaultValidator = 0;
    iDtorValidator = 0;

	return error;
	}

inline CExampleInterface_DefectKRN53SL4Q_UnitTest::~CExampleInterface_DefectKRN53SL4Q_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	delete iCtorValidator;
	delete iDefaultValidator;
	delete iDtorValidator;
	}

inline CExampleInterface_DefectKRN53SL4Q_UnitTest::CExampleInterface_DefectKRN53SL4Q_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KExampleInterfaceDefectKRN53SL4QUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

TInt ThreadFunc(TAny* aTrapHandler)
	{
	User::SetTrapHandler(REINTERPRET_CAST(TCleanupTrapHandler*, aTrapHandler));

	RImplInfoPtrArray implementations;
	TRAPD(error, CExampleInterface::ListImplementationsL(implementations));
    UNUSED_VAR(error);
	return KErrNone;
	}

// Now the Individual transitions need to be added.
inline void CExampleInterface_DefectKRN53SL4Q_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TExampleInterface_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CExampleInterface_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	_LIT(KNewThreadName, "ListImpl");
	User::Leave (KErrNone != iUTContext->iListImplThread.Create(KNewThreadName(),	// The name of the thread
									   ThreadFunc, //CExampleInterface_UnitTestContext::ListImplementationsThreadFunction,	// The function to be called when this thread resumes
									   2048,				// The stacksize for this thread
									   2048,				// min heap size
									   2048,				// max heap size
									   User::TrapHandler()));	// Parameter to be passed to function

		
	iCtorValidator = new(ELeave) TExampleInterface_Ctor_TransitionValidator(*iUTContext);
	iDefaultValidator = new(ELeave) TExampleInterface_Default_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TExampleInterface_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CExampleInterfaceNewLTransition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CExampleInterfaceListImplementationsLNewThreadTransition(*iUTContext,*iDefaultValidator));
	AddBlockingTransitionL(new(ELeave)CExampleInterfaceDtorTransition(*iUTContext,*iDtorValidator));
	}

