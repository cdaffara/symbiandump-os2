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
// The definition of the transition classes on the CLoadManager class
// 
//

// ______________________________________________________________________________
//
inline CLoadManager_UnitTestContext::CLoadManager_UnitTestContext(CDataLogger& aDataLogger,
												MStateAccessor& aStateAccessor,
												MTransitionObserver& aObserver)
: CUnitTestContext(aDataLogger, aStateAccessor, aObserver)
	{
	//Do nothing
	}

inline CLoadManager_UnitTestContext::~CLoadManager_UnitTestContext()
	{
	delete iLoadManager;
	delete iRegistryData;
	}

// ______________________________________________________________________________
//
_LIT(KLoadManagerNewLTransition,"CLoadManager_NewL_Transition");

inline CLoadManager_NewL_Transition::CLoadManager_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KLoadManagerNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CLoadManager_NewL_Transition::TransitMethodL()
	{
	Context().iLoadManager = CLoadManager::NewL();
	}

inline CLoadManager_UnitTestContext& CLoadManager_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KLoadManagerDtorTransition,"CLoadManager_Dtor_Transition");

inline CLoadManager_Dtor_Transition::CLoadManager_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KLoadManagerDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CLoadManager_Dtor_Transition::TransitMethodL()
	{
	delete Context().iLoadManager;
	Context().iLoadManager = NULL;
	}

inline CLoadManager_UnitTestContext& CLoadManager_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KLoadManagerInstantiationMethodLTransition,"CLoadManager_InstantiationMethodL_Transition");

inline CLoadManager_InstantiationMethodL_Transition::CLoadManager_InstantiationMethodL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KLoadManagerInstantiationMethodLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CLoadManager_InstantiationMethodL_Transition::TransitMethodL()
	{
	_LIT(KLoadManagerInstantiationMethodLTransitMethod, "CLoadManager::InstantiationMethodL transition");
	Context().DataLogger().LogInformation(KLoadManagerInstantiationMethodLTransitMethod);
	Context().iLoadMethod = Context().iLoadManager->InstantiationMethodL(Context().iUniqueImplementationUid,
 																		 Context().iDllEntry);
	}

inline CLoadManager_UnitTestContext& CLoadManager_InstantiationMethodL_Transition::Context() const
	{
	return REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KLoadManagerDestroyedThisTransition,"CLoadManager_DestroyedThis_Transition");

inline CLoadManager_DestroyedThis_Transition::CLoadManager_DestroyedThis_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KLoadManagerDestroyedThisTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CLoadManager_DestroyedThis_Transition::TransitMethodL()
	{
	_LIT(KLoadManagerDestroyedThisTransitMethod, "CLoadManager::DestroyedThis transition");
	Context().DataLogger().LogInformation(KLoadManagerDestroyedThisTransitMethod);
	Context().iLoadManager->DestroyedThis(Context().iUniqueImplementationUid);
	}

inline CLoadManager_UnitTestContext& CLoadManager_DestroyedThis_Transition::Context() const
	{
	return REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KLoadManagerDestroyCreateTransition,"CLoadManager_DestroyCreate_Transition");

inline CLoadManager_DestroyCreate_Transition::CLoadManager_DestroyCreate_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KLoadManagerDestroyCreateTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CLoadManager_DestroyCreate_Transition::TransitMethodL()
	{
	_LIT(KLoadManagerDestroyCreateTransitMethod, "CLoadManager_DestroyCreate_Transition");
	Context().DataLogger().LogInformation(KLoadManagerDestroyCreateTransitMethod);
	Context().iLoadManager->DestroyedThis(Context().iUniqueImplementationUid);
	Context().iLoadMethod = Context().iLoadManager->InstantiationMethodL(Context().iUniqueImplementationUid,
 																		 Context().iDllEntry);

	}

inline CLoadManager_UnitTestContext& CLoadManager_DestroyCreate_Transition::Context() const
	{
	return REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext);
	}

