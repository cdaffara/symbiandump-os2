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
// 
//

#include <e32uid.h>

#include "TestUtilities.h"	// For __FILE__LINE__
#include "Interface.h"
#include <test_bed/datalogger.h>

#include "SuicideUnitTests.h"
#include "SuicideTransitions.h"
#include "SuicideTransitionValidation.h"

#include "TlsData.h"		// For GlobalData

// ______________________________________________________________________________
//
_LIT(KInterfaceCreateAndDestroyUnitTest,"CSuicideInterfaceCreateAndDestroyUnitTest");

CSuicideInterfaceCreateAndDestroyUnitTest* CSuicideInterfaceCreateAndDestroyUnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CSuicideInterfaceCreateAndDestroyUnitTest* self = 
					new(ELeave) CSuicideInterfaceCreateAndDestroyUnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	// Chain to the base which calls the ConstructL
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

TInt CSuicideInterfaceCreateAndDestroyUnitTest::RunError(TInt aError)
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

CSuicideInterfaceCreateAndDestroyUnitTest::~CSuicideInterfaceCreateAndDestroyUnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iDtorValidator;

	}

CSuicideInterfaceCreateAndDestroyUnitTest::CSuicideInterfaceCreateAndDestroyUnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KInterfaceCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
void CSuicideInterfaceCreateAndDestroyUnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TSuicideInterface_StateAccessor();
	
	// context
	iUTContext = new(ELeave) CSuicideInterface_UnitTestContext(iDataLogger, *iStateAccessor, *this);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TSuicideInterface_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CSuicideInterfaceNewLTransition(*iUTContext,*iCtorValidator));
	
	iDtorValidator = new(ELeave) TSuicideInterface_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CSuicideInterfaceDtorTransition(*iUTContext,*iDtorValidator));
	}
// ______________________________________________________________________________
//
_LIT(KInterfaceFireAndForgetUnitTest,"CSuicideInterfaceFireAndForgetUnitTest");

CSuicideInterfaceFireAndForgetUnitTest* CSuicideInterfaceFireAndForgetUnitTest::NewL(CDataLogger& aDataLogger, 
																					 MUnitTestObserver& aObserver)
	{
	CSuicideInterfaceFireAndForgetUnitTest* self = new(ELeave) CSuicideInterfaceFireAndForgetUnitTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	// Chain to the base which calls the ConstructL
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

TInt CSuicideInterfaceFireAndForgetUnitTest::RunError(TInt aError)
	{
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	iUTContext = NULL;
	delete iStateAccessor;
	iStateAccessor = NULL;
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iFireAndForgetValidator;
	iFireAndForgetValidator = NULL;

	return error;
	}

CSuicideInterfaceFireAndForgetUnitTest::~CSuicideInterfaceFireAndForgetUnitTest()
	{
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iFireAndForgetValidator;

	}

CSuicideInterfaceFireAndForgetUnitTest::CSuicideInterfaceFireAndForgetUnitTest(CDataLogger& aDataLogger, MUnitTestObserver& aObserver)
: CUnitTest(KInterfaceFireAndForgetUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
void CSuicideInterfaceFireAndForgetUnitTest::ConstructL()
	{
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TSuicideInterface_StateAccessor();
	// context
	iUTContext = new(ELeave) CSuicideInterface_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TSuicideInterface_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CSuicideInterfaceNewLTransition(*iUTContext,*iCtorValidator));

	iFireAndForgetValidator = new(ELeave) TSuicideInterface_FireAndForget_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CSuicideInterfaceFireAndForgetTransition(*iUTContext, *iFireAndForgetValidator));
	}

