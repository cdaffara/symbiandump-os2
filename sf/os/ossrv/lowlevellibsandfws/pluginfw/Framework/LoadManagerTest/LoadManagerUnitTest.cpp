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
// The unit test class implementations for the CLoadManager tests
// 
//

#include "LoadManagerUnitTest.h"
#include "RegistryData.h"

CLoadManager_CreateAndDestroy_UnitTest* CLoadManager_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CLoadManager_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CLoadManager_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CLoadManager_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TLoadManager_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CLoadManager_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TLoadManager_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CLoadManager_NewL_Transition(*iUTContext,*iCtorValidator));
	iDtorValidator = new(ELeave) TLoadManager_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CLoadManager_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

void CLoadManager_CreateAndDestroy_UnitTest::PrepareUnitTestL()
	{
	// Give the context a constructed set of data
	iUTContext->iRegistryData = CRegistryData::NewL(iFs);
	}

// ______________________________________________________________________________
//
CLoadManager_FindInstantiationAndDestroy_UnitTest* CLoadManager_FindInstantiationAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CLoadManager_FindInstantiationAndDestroy_UnitTest* self = 
					new(ELeave) CLoadManager_FindInstantiationAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CLoadManager_FindInstantiationAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TLoadManager_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CLoadManager_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	
	iUTContext->iUniqueImplementationUid = KUniqueImplementationUid_Works;
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TLoadManager_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CLoadManager_NewL_Transition(*iUTContext,*iCtorValidator));
	
	iInstantiationMethodValidator = new(ELeave) TLoadManager_InstantiationMethod_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CLoadManager_InstantiationMethodL_Transition(*iUTContext,*iInstantiationMethodValidator));
	
	iDestroyThisValidator = new(ELeave) TLoadManager_DestroyThis_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CLoadManager_DestroyedThis_Transition(*iUTContext,*iDestroyThisValidator));
	
	iDtorValidator = new(ELeave) TLoadManager_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CLoadManager_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

void CLoadManager_FindInstantiationAndDestroy_UnitTest::PrepareUnitTestL()
	{
	iUTContext->iRegistryData = CRegistryData::NewL(iFs);
	}

// ______________________________________________________________________________
//
CLoadManager_FindInstantiationFailure_UnitTest* CLoadManager_FindInstantiationFailure_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CLoadManager_FindInstantiationFailure_UnitTest* self = 
					new(ELeave) CLoadManager_FindInstantiationFailure_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CLoadManager_FindInstantiationFailure_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TLoadManager_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CLoadManager_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	
	iUTContext->iUniqueImplementationUid = KUniqueImplementationUid_Fails;
	iUTContext->iRegistryData = CRegistryData::NewL(iFs);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TLoadManager_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CLoadManager_NewL_Transition(*iUTContext,*iCtorValidator));
	
	iInstantiationMethodValidator = new(ELeave) TLoadManager_InstantiationFailure_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CLoadManager_InstantiationMethodL_Transition(*iUTContext,*iInstantiationMethodValidator));
	
	iDtorValidator = new(ELeave) TLoadManager_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CLoadManager_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
CLoadManager_DefectFOT56ULPM_UnitTest* CLoadManager_DefectFOT56ULPM_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CLoadManager_DefectFOT56ULPM_UnitTest* self = 
					new(ELeave) CLoadManager_DefectFOT56ULPM_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

// Now the Individual transitions need to be added.
inline void CLoadManager_DefectFOT56ULPM_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TLoadManager_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CLoadManager_UnitTestContext(iDataLogger, *iStateAccessor, *this);
	
	iUTContext->iUniqueImplementationUid = KUniqueImplementationUid_Works;
	iUTContext->iRegistryData = CRegistryData::NewL(iFs);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TLoadManager_Ctor_TransitionValidator(*iUTContext);
	iInstantiationMethodValidator = new(ELeave) TLoadManager_InstantiationMethod_TransitionValidator(*iUTContext);
	iDestroyThisValidator = new(ELeave) TLoadManager_DestroyThis_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TLoadManager_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CLoadManager_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CLoadManager_InstantiationMethodL_Transition(*iUTContext,*iInstantiationMethodValidator));
	AddTransitionL(new(ELeave)CLoadManager_DestroyCreate_Transition(*iUTContext,*iDestroyThisValidator));
	AddTransitionL(new(ELeave)CLoadManager_DestroyedThis_Transition(*iUTContext,*iDestroyThisValidator));
	AddTransitionL(new(ELeave)CLoadManager_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

