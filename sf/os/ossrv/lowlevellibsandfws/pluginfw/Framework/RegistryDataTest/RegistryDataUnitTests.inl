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
// The implementaition of the unit tests for the CRegistryData class.
// 
//

const TUid KCExampleInterfaceUid = {0x10009DC0};
const TUid KCExampleInterfaceImp = {0x10009DC3};

// ______________________________________________________________________________
//
_LIT(KRegistryDataCreateAndDestroyUnitTest,"CRegistryData_CreateAndDestroy_UnitTest");

CRegistryData_CreateAndDestroy_UnitTest* CRegistryData_CreateAndDestroy_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CRegistryData_CreateAndDestroy_UnitTest* self = 
					new(ELeave) CRegistryData_CreateAndDestroy_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CRegistryData_CreateAndDestroy_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iDtorValidator;
	return error;
	}

inline CRegistryData_CreateAndDestroy_UnitTest::~CRegistryData_CreateAndDestroy_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	delete iCtorValidator;
	delete iDtorValidator;
	}

inline CRegistryData_CreateAndDestroy_UnitTest::CRegistryData_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KRegistryDataCreateAndDestroyUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CRegistryData_CreateAndDestroy_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TRegistryData_StateAccessor;
	// Construct the Unit test context.
	iUTContext = new(ELeave) CRegistryData_UnitTestContext(iDataLogger, *iStateAccessor, *this, iFs);
	// and a default validator
	iCtorValidator = new(ELeave) TRegistryData_Ctor_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TRegistryData_Dtor_TransitionValidator(*iUTContext);
	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	//	Examples of C'tor and D'tor transitions on CRegistryData class.
	AddTransitionL(new(ELeave)CRegistryData_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CRegistryData_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

void CRegistryData_CreateAndDestroy_UnitTest::PrepareUnitTestL()
	{
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataAddDataUnitTest,"CRegistryData_AddData_UnitTest");

CRegistryData_AddData_UnitTest* CRegistryData_AddData_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CRegistryData_AddData_UnitTest* self = 
					new(ELeave) CRegistryData_AddData_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CRegistryData_AddData_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;

	delete iCtorValidator;
	delete iSetObserverValidator;
	delete iUndoTemporaryUninstallValidator;
	delete iAddDllDataValidator;
	delete iIsRegisteredValidator;
	delete iUpdateDllDataValidator;
	delete iTemporaryUninstallValidator;
	delete iPersistNowValidator;
	delete iDtorValidator;

	return error;
	}

inline CRegistryData_AddData_UnitTest::~CRegistryData_AddData_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;
	
	// delete any validators used
	delete iCtorValidator;
	delete iSetObserverValidator;
	delete iUndoTemporaryUninstallValidator;
	delete iAddDllDataValidator;
	delete iIsRegisteredValidator;
	delete iUpdateDllDataValidator;
	delete iTemporaryUninstallValidator;
	delete iPersistNowValidator;

	delete iDtorValidator;
	}

inline CRegistryData_AddData_UnitTest::CRegistryData_AddData_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KRegistryDataAddDataUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CRegistryData_AddData_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TRegistryData_StateAccessor;

	// Construct the Unit test context.
	iUTContext = new(ELeave) CRegistryData_UnitTestContext(iDataLogger, 
														   *iStateAccessor, 
														   *this, 
														   iFs);
	_LIT(KDDriveLetter, "D");
	iUTContext->iDriveUnit = KDDriveLetter;
	iUTContext->iFoundDriveIndex = -1;
	TUid thirdUid = {0x10009DB1};
	iUTContext->iDllEntry.iType = TUidType(KNullUid, KNullUid, thirdUid);
	iUTContext->iDllData = CRegistryData::CDllData::NewLC(iUTContext->iDllEntry);
	CleanupStack::Pop(iUTContext->iDllData);
	iUTContext->iNewDllData = CRegistryData::CDllData::NewLC(iUTContext->iDllEntry);
	CleanupStack::Pop(iUTContext->iNewDllData);

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator	  = new(ELeave) TRegistryData_Ctor_TransitionValidator(*iUTContext);
	iSetObserverValidator = new(ELeave) TRegistryData_SetPolicyObserver_TransitionValidator(*iUTContext);
	iUndoTemporaryUninstallValidator = new(ELeave) TRegistryData_UndoTemporaryUninstall_TransitionValidator(*iUTContext);
	iAddDllDataValidator = new(ELeave) TRegistryData_AddDllDataL_TransitionValidator(*iUTContext);
	iIsRegisteredValidator = new(ELeave) TRegistryData_IsRegisteredWithDate_TransitionValidator(*iUTContext);
	iUpdateDllDataValidator = new(ELeave) TRegistryData_UpdateDllData_TransitionValidator(*iUTContext);
	iTemporaryUninstallValidator = new(ELeave) TRegistryData_TemporaryUninstall_TransitionValidator(*iUTContext);
	iPersistNowValidator = new(ELeave) TRegistryData_PersistNow_TransitionValidator(*iUTContext);
	iDtorValidator	  = new(ELeave) TRegistryData_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CRegistryData_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CRegistryData_UndoTemporaryUninstallL_Transition(*iUTContext,*iUndoTemporaryUninstallValidator));
	AddTransitionL(new(ELeave)CRegistryData_AddDllDataL_Transition(*iUTContext,*iAddDllDataValidator));
	AddTransitionL(new(ELeave)CRegistryData_IsRegisteredWithDateL_Transition(*iUTContext,*iIsRegisteredValidator));
	AddTransitionL(new(ELeave)CRegistryData_UpdateDllDataL_Transition(*iUTContext,*iUpdateDllDataValidator));
	AddTransitionL(new(ELeave)CRegistryData_TemporaryUninstallL_Transition(*iUTContext,*iTemporaryUninstallValidator));
	AddTransitionL(new(ELeave)CRegistryData_PersistNowL_Transition(*iUTContext,*iPersistNowValidator));
	AddTransitionL(new(ELeave)CRegistryData_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataRetrieveInfoUnitTest,"CRegistryData_RetrieveInfo_UnitTest");

CRegistryData_RetrieveInfo_UnitTest* CRegistryData_RetrieveInfo_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CRegistryData_RetrieveInfo_UnitTest* self = 
					new(ELeave) CRegistryData_RetrieveInfo_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self; 
	}

inline TInt CRegistryData_RetrieveInfo_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	/* delete any validators used */
	delete iCtorValidator;
	delete iListImplementationsValidator;
	delete iGetImplementationInformationValidator;
	delete iDtorValidator;
	return error;
	}

inline CRegistryData_RetrieveInfo_UnitTest::~CRegistryData_RetrieveInfo_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	delete iCtorValidator;
	delete iListImplementationsValidator;
	delete iGetImplementationInformationValidator;
	delete iDtorValidator;
	}

inline CRegistryData_RetrieveInfo_UnitTest::CRegistryData_RetrieveInfo_UnitTest(CDataLogger& aDataLogger,
																	MUnitTestObserver& aObserver)
: CUnitTest(KRegistryDataRetrieveInfoUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CRegistryData_RetrieveInfo_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TRegistryData_StateAccessor;

	// Construct the Unit test context.
	iUTContext = new(ELeave) CRegistryData_UnitTestContext(iDataLogger, 
														   *iStateAccessor, 
														   *this, 
														   iFs);
	iUTContext->iInterfaceUid.iUid		= KCExampleInterfaceUid.iUid;
	iUTContext->iImplementationUid.iUid	= KCExampleInterfaceImp.iUid;

	// Add the Transitions in the order they are to run
	iCtorValidator = new(ELeave) TRegistryData_Ctor_TransitionValidator(*iUTContext);
	iListImplementationsValidator = new(ELeave) TRegistryData_ListImplementations_TransitionValidator(*iUTContext);
	iGetImplementationInformationValidator = new(ELeave) TRegistryData_GetImplementationInformation_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TRegistryData_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CRegistryData_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CRegistryData_ListImplementationsL_Transition(*iUTContext,
																			*iListImplementationsValidator));
	AddTransitionL(new(ELeave)CRegistryData_GetImplementationInformationL_Transition(*iUTContext,
																			  *iGetImplementationInformationValidator));
	AddTransitionL(new(ELeave)CRegistryData_Dtor_Transition(*iUTContext,*iDtorValidator));

	// Add the permissible error codes for failures
	AddLeaveErrorCodeL(KEComErrNoInterfaceIdentified);
	}

// ______________________________________________________________________________
//
_LIT(KRegistryDataInfoWhileDiscoveringUnitTest,"CRegistryData_GetImplInfoWhileDiscovering_UnitTest");

CRegistryData_GetImplInfoWhileDiscovering_UnitTest* 
	CRegistryData_GetImplInfoWhileDiscovering_UnitTest::NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver)
	{
	CRegistryData_GetImplInfoWhileDiscovering_UnitTest* self = 
			new(ELeave) CRegistryData_GetImplInfoWhileDiscovering_UnitTest(aDataLogger,
																aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}

inline TInt CRegistryData_GetImplInfoWhileDiscovering_UnitTest::RunError(TInt aError)
	{
	// The RunL left so chain to the base first and then cleanup
	TInt error = CUnitTest::RunError(aError);	// Chain to base
	delete iUTContext;
	delete iStateAccessor;
	// delete any validators used
	delete iCtorValidator;
	delete iGetImplInfoValidator;
	delete iDiscoveriesBeginningValidator;
	delete iGetSecondImplInfoValidator;
	delete iDtorValidator;

	return error;
	}

inline CRegistryData_GetImplInfoWhileDiscovering_UnitTest::~CRegistryData_GetImplInfoWhileDiscovering_UnitTest()
	{
	// Simply delete our test class instance
	delete iUTContext;
	delete iStateAccessor;

	// delete any validators used
	delete iCtorValidator;
	delete iGetImplInfoValidator;
	delete iDiscoveriesBeginningValidator;
	delete iGetSecondImplInfoValidator;
	delete iDtorValidator;
	}

inline CRegistryData_GetImplInfoWhileDiscovering_UnitTest::
	CRegistryData_GetImplInfoWhileDiscovering_UnitTest(CDataLogger& aDataLogger,
													   MUnitTestObserver& aObserver)
: CUnitTest(KRegistryDataInfoWhileDiscoveringUnitTest, aDataLogger, aObserver)
	{
	//Do nothing
	}

// Now the Individual transitions need to be added.
inline void CRegistryData_GetImplInfoWhileDiscovering_UnitTest::ConstructL()
	{
	// Perform the base class initialization
	UnitTestConstructL();

	// Create the Unit test state accessor
	iStateAccessor = new(ELeave) TRegistryData_StateAccessor;

	// Construct the Unit test context.
	iUTContext = new(ELeave) CRegistryData_UnitTestContext(iDataLogger, 
														   *iStateAccessor, 
														   *this, 
														   iFs);

	iUTContext->iInterfaceUid.iUid		= KCExampleInterfaceUid.iUid;
	iUTContext->iImplementationUid.iUid	= KCExampleInterfaceImp.iUid;

	// Add the Transitions in the order they are to run
	// C'tor first, D'tor last...
	iCtorValidator = new(ELeave) TRegistryData_Ctor_TransitionValidator(*iUTContext);
	iGetImplInfoValidator = new(ELeave) TRegistryData_GetImplementationInformation_TransitionValidator(*iUTContext);
	iDiscoveriesBeginningValidator = new(ELeave) TRegistryData_DiscoveriesBeginning_TransitionValidator(*iUTContext);
	iGetSecondImplInfoValidator = new(ELeave) TRegistryData_GetSecondImplInfo_TransitionValidator(*iUTContext);
	iDtorValidator = new(ELeave) TRegistryData_Dtor_TransitionValidator(*iUTContext);

	AddTransitionL(new(ELeave)CRegistryData_NewL_Transition(*iUTContext,*iCtorValidator));
	AddTransitionL(new(ELeave)CRegistryData_GetImplementationInformationL_Transition(*iUTContext,*iGetImplInfoValidator));
	AddTransitionL(new(ELeave)CRegistryData_DiscoveriesBeginning_Transition(*iUTContext,*iDiscoveriesBeginningValidator));
	AddTransitionL(new(ELeave)CRegistryData_GetImplementationInformationL_Transition(*iUTContext,*iGetSecondImplInfoValidator));
	AddTransitionL(new(ELeave)CRegistryData_Dtor_Transition(*iUTContext,*iDtorValidator));
	}

