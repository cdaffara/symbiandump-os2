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
// The implementation of the unit test classes for the CRegistrar.
// 
//

#include "RegistrarTransitions.h"
#include "RegistrarStateAccessor.h"

_LIT(KRegistrarNotificationMsg,"Notification %d recieved from CRegistrar!");

// ______________________________________________________________________________
//
_LIT(KRegistrarCreateAndDestroyUnitTest,"CRegistrar_CreateAndDestroy_UnitTest");

CRegistrar_CreateAndDestroy_UnitTest* CRegistrar_CreateAndDestroy_UnitTest::NewL(
															CDataLogger& aDataLogger,
															MUnitTestObserver& aObserver)
	{
	CRegistrar_CreateAndDestroy_UnitTest* self = 
				new(ELeave) CRegistrar_CreateAndDestroy_UnitTest(aDataLogger, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CRegistrar_CreateAndDestroy_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	delete iDtorValidator;

	return error;
	}

inline CRegistrar_CreateAndDestroy_UnitTest::~CRegistrar_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	delete iRegistryData;

	// delete any validators used
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CRegistrar_CreateAndDestroy_UnitTest::CRegistrar_CreateAndDestroy_UnitTest(
															CDataLogger& aDataLogger,
															MUnitTestObserver& aObserver)
: CUnitTest(KRegistrarCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CRegistrar_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TRegistrar_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CRegistrar_UnitTestContext(iDataLogger, 
														*iStateAccessor, 
														*this,
														*iRegistryData,
														*this,
														iFs);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TRegistrar_Ctor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CRegistrar_NewL_Transition(*iUTContext,*iCtorValidator));
	iDtorValidator = new(ELeave) TRegistrar_Dtor_TransitionValidator(*iUTContext);
	AddTransitionL(new(ELeave)CRegistrar_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

void CRegistrar_CreateAndDestroy_UnitTest::PrepareUnitTestL()
	{
	iRegistryData = CRegistryData::NewL(iFs);
	}

void CRegistrar_CreateAndDestroy_UnitTest::Notification(TInt aNotificationCode)
	{
	iDataLogger.LogInformationWithParameters(KRegistrarNotificationMsg, aNotificationCode);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarBeginCompleteUnitTest,"CRegistrar_BeginComplete_UnitTest");

CRegistrar_BeginComplete_UnitTest* CRegistrar_BeginComplete_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CRegistrar_BeginComplete_UnitTest* self = 
					new(ELeave) CRegistrar_BeginComplete_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CRegistrar_BeginComplete_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;

	delete iRegistryData;

	delete iCtorValidator;
	delete iDefaultValidator;
	delete iDtorValidator;

	return error;
	}

inline CRegistrar_BeginComplete_UnitTest::~CRegistrar_BeginComplete_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	delete iRegistryData;

	delete iCtorValidator;
	delete iDefaultValidator;
	delete iDtorValidator;
	}

inline CRegistrar_BeginComplete_UnitTest::CRegistrar_BeginComplete_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KRegistrarBeginCompleteUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CRegistrar_BeginComplete_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	iRegistryData = CRegistryData::NewL(iFs);

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TRegistrar_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CRegistrar_UnitTestContext(iDataLogger, 
														*iStateAccessor, 
														*this,
														*iRegistryData,
														*this,
														iFs);
	iUTContext->iDllData = CRegistryData::CDllData::NewLC();
	CleanupStack::Pop();	//iDllData

	// Create the validators which are used by the transitions
	iCtorValidator	  = new(ELeave) TRegistrar_Ctor_TransitionValidator(*iUTContext);
	iDefaultValidator = new(ELeave) TRegistrar_Default_TransitionValidator(*iUTContext);
	iDtorValidator	  = new(ELeave) TRegistrar_Dtor_TransitionValidator(*iUTContext);

	// Add the Transitions in the order they are to run
	// C'tor, Begin, Complete, D'tor
	AddTransitionL(new(ELeave)CRegistrar_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CRegistrar_DiscoveriesBegin_Transition(*iUTContext,*iDefaultValidator));
//	AddTransitionL(new(ELeave)CRegistrar_ParseL_Transition(*iUTContext,*iDefaultValidator));
	AddTransitionL(new(ELeave)CRegistrar_DiscoveriesComplete_Transition(*iUTContext,*iDefaultValidator));
	AddTransitionL(new(ELeave)CRegistrar_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

void CRegistrar_BeginComplete_UnitTest::Notification(TInt aNotificationCode)
	{
	iDataLogger.LogInformationWithParameters(KRegistrarNotificationMsg, aNotificationCode);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarRegisterDiscoveryUnitTest,"CRegistrar_RegisterDiscovery_UnitTest");

CRegistrar_RegisterDiscovery_UnitTest* CRegistrar_RegisterDiscovery_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CRegistrar_RegisterDiscovery_UnitTest* self = 
					new(ELeave) CRegistrar_RegisterDiscovery_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CRegistrar_RegisterDiscovery_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;

	delete iRegistryData;

	// delete any validators used
	delete iCtorValidator;
	delete iRegisterDiscoveryValidator;
	delete iDtorValidator;

	return error;
	}

inline CRegistrar_RegisterDiscovery_UnitTest::~CRegistrar_RegisterDiscovery_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	delete iRegistryData;

	// delete any validators used
	delete iCtorValidator;
	delete iRegisterDiscoveryValidator;
	delete iDtorValidator;
	}

inline CRegistrar_RegisterDiscovery_UnitTest::CRegistrar_RegisterDiscovery_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KRegistrarRegisterDiscoveryUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CRegistrar_RegisterDiscovery_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	iRegistryData = CRegistryData::NewL(iFs);
	TDriveUnit drive(EDriveZ);
	iRegistryData->UndoTemporaryUninstallL(drive);

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TRegistrar_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CRegistrar_UnitTestContext(iDataLogger, 
														*iStateAccessor, 
														*this,
														*iRegistryData,
														*this,
														iFs);
	iUTContext->iDllData = CRegistryData::CDllData::NewLC();
	CleanupStack::Pop();	//iDllData

	// Artificially set up the TEntry info on the dll
	TPtr dllName = iUTContext->iDllEntry.iName.Des();
	dllName = KExampleDllFileName();

	// Only the third entry (unique dll id) matters in this case
	TUid uid1, uid2, uid3;
	uid1.iUid = 0;
	uid2.iUid = 0;
	uid3.iUid = 0x10009DB0;
	iUTContext->iDllEntry.iType = TUidType(uid1, uid2, uid3);

	// Set the modified time to an unrealistic time to ensure it is reloaded
	iUTContext->iDllEntry.iModified = 5;
	iUTContext->iUpdate = EFalse;
	iUTContext->iIndex = KErrNotFound;

	iCtorValidator = new(ELeave) TRegistrar_Ctor_TransitionValidator(*iUTContext);
	iRegisterDiscoveryValidator = new(ELeave) TRegistrar_RegisterDiscoveryL_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TRegistrar_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CRegistrar_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CRegistrar_ParseL_Transition(*iUTContext,*iRegisterDiscoveryValidator));
	AddTransitionL(new(ELeave)CRegistrar_ParseRegistrationDataL_Transition(*iUTContext,*iRegisterDiscoveryValidator));
	AddTransitionL(new(ELeave)CRegistrar_RegisterDiscoveryL_Transition(*iUTContext,*iRegisterDiscoveryValidator));
	AddTransitionL(new(ELeave)CRegistrar_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

void CRegistrar_RegisterDiscovery_UnitTest::Notification(TInt aNotificationCode)
	{
	iDataLogger.LogInformationWithParameters(KRegistrarNotificationMsg, aNotificationCode);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarRegisterTwiceUnitTest,"CRegistrar_RegisterTwice_UnitTest");

CRegistrar_RegisterTwice_UnitTest* CRegistrar_RegisterTwice_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CRegistrar_RegisterTwice_UnitTest* self = 
					new(ELeave) CRegistrar_RegisterTwice_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CRegistrar_RegisterTwice_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;

	delete iRegistryData;

	// delete any validators used
	delete iCtorValidator;
	delete iRegisterDiscoveryValidator;
	delete iDefaultValidator;
	delete iDtorValidator;

	return error;
	}

inline CRegistrar_RegisterTwice_UnitTest::~CRegistrar_RegisterTwice_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	delete iRegistryData;

	// delete any validators used
	delete iCtorValidator;
	delete iRegisterDiscoveryValidator;
	delete iDefaultValidator;
	delete iDtorValidator;
	}

inline CRegistrar_RegisterTwice_UnitTest::CRegistrar_RegisterTwice_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KRegistrarRegisterTwiceUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CRegistrar_RegisterTwice_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	iRegistryData = CRegistryData::NewL(iFs);
	TDriveUnit drive(EDriveZ);
	iRegistryData->UndoTemporaryUninstallL(drive);

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TRegistrar_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CRegistrar_UnitTestContext(iDataLogger, 
														*iStateAccessor, 
														*this,
														*iRegistryData,
														*this,
														iFs);
	iUTContext->iDllData = CRegistryData::CDllData::NewLC();
	CleanupStack::Pop();	//iDllData

	// Artificially set up the TEntry info on the dll
	TPtr dllName = iUTContext->iDllEntry.iName.Des();
	dllName = KExampleDllFileName();

	// Only the third entry (unique dll id) matters in this case
	TUid uid1, uid2, uid3;
	uid1.iUid = 0;
	uid2.iUid = 0;
	uid3.iUid = 0x10009DB1;
	iUTContext->iDllEntry.iType = TUidType(uid1, uid2, uid3);

	// Set the modified time to an unrealistic time to ensure it is reloaded
	iUTContext->iDllEntry.iModified = 5;
	iUTContext->iUpdate = EFalse;
	iUTContext->iIndex = KErrNotFound;

	iCtorValidator = new(ELeave) TRegistrar_Ctor_TransitionValidator(*iUTContext);
	iRegisterDiscoveryValidator = new(ELeave) TRegistrar_RegisterDiscoveryL_TransitionValidator(*iUTContext);
	iDefaultValidator = new(ELeave) TRegistrar_Default_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TRegistrar_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CRegistrar_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CRegistrar_RegisterDiscoveryL_Transition(*iUTContext,*iRegisterDiscoveryValidator));
	AddTransitionL(new(ELeave)CRegistrar_DiscoveriesComplete_Transition(*iUTContext,*iDefaultValidator));
	AddTransitionL(new(ELeave)CRegistrar_RegisterDiscoveryL_Transition(*iUTContext,*iRegisterDiscoveryValidator));
	AddTransitionL(new(ELeave)CRegistrar_DiscoveriesComplete_Transition(*iUTContext,*iDefaultValidator));
	AddTransitionL(new(ELeave)CRegistrar_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

void CRegistrar_RegisterTwice_UnitTest::Notification(TInt aNotificationCode)
	{
	iDataLogger.LogInformationWithParameters(KRegistrarNotificationMsg, aNotificationCode);
	}

// ______________________________________________________________________________
//
_LIT(KRegistrarUseWhileDiscoveringUnitTest,"CRegistrar_UseWhileDiscovering_UnitTest");

CRegistrar_UseWhileDiscovering_UnitTest* CRegistrar_UseWhileDiscovering_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CRegistrar_UseWhileDiscovering_UnitTest* self = 
					new(ELeave) CRegistrar_UseWhileDiscovering_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CRegistrar_UseWhileDiscovering_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;

	delete iRegistryData;

	// delete any validators used
	delete iCtorValidator;
	iCtorValidator = NULL;
	delete iCheckRegValidator;
	iCheckRegValidator = NULL;
	delete iDtorValidator;
	iDtorValidator = NULL;

	return error;
	}

inline CRegistrar_UseWhileDiscovering_UnitTest::~CRegistrar_UseWhileDiscovering_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	delete iRegistryData;

	// delete any validators used
	delete iCtorValidator;
	delete iCheckRegValidator;
	delete iDtorValidator;
	}

inline CRegistrar_UseWhileDiscovering_UnitTest::CRegistrar_UseWhileDiscovering_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KRegistrarUseWhileDiscoveringUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CRegistrar_UseWhileDiscovering_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	iRegistryData = CRegistryData::NewL(iFs);

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TRegistrar_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CRegistrar_UnitTestContext(iDataLogger, 
														*iStateAccessor, 
														*this,
														*iRegistryData,
														*this,
														iFs);
	iUTContext->iDllData = CRegistryData::CDllData::NewLC();
	CleanupStack::Pop();	//iDllData

	iUTContext->iInterfaceUid.iUid = 0x10009DC0;

	// Artificially set up the TEntry info on the dll
	TPtr dllName = iUTContext->iDllEntry.iName.Des();
	dllName = KExampleDllFileName();

	iUTContext->iIndex = KErrNotFound;
#ifdef __WINS__
	iUTContext->iDriveUnit = EDriveZ;
#else
	iUTContext->iDriveUnit = EDriveC;
#endif

	iCtorValidator = new(ELeave) TRegistrar_Ctor_TransitionValidator(*iUTContext);
	iCheckRegValidator = new(ELeave) TRegistrar_CheckRegWhileDiscovering_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TRegistrar_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CRegistrar_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CRegistrar_DiscoveriesBegin_Transition(*iUTContext,*iCheckRegValidator));
	AddTransitionL(new(ELeave)CRegistrar_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

void CRegistrar_UseWhileDiscovering_UnitTest::Notification(TInt aNotificationCode)
	{
	iDataLogger.LogInformationWithParameters(KRegistrarNotificationMsg, aNotificationCode);
	}

