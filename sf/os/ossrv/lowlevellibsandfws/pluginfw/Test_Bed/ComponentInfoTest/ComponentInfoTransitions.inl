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
// The implementation of the transition classes upon the CComponentInfo class methods.
// 
//

// ______________________________________________________________________________
//
_LIT(KComponentInfoNewLTransition,"CComponentInfo_NewL_Transition");

inline CComponentInfo_NewL_Transition::CComponentInfo_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KComponentInfoNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentInfo_NewL_Transition::TransitMethodL()
	{
	_LIT(KComponentInfoNewLTransitMethod, "CComponentInfo::NewL transition");
	Context().DataLogger().LogInformation(KComponentInfoNewLTransitMethod);
	Context().iComponentInfo = CComponentInfo::NewL(Context().iEntryFunc,Context().iTestInfoArray);
	Context().iOwnParams = EFalse;
	}

inline CComponentInfo_UnitTestContext& CComponentInfo_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KComponentInfoDtorTransition,"CComponentInfo_Dtor_Transition");

inline CComponentInfo_Dtor_Transition::CComponentInfo_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KComponentInfoDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentInfo_Dtor_Transition::TransitMethodL()
	{
	_LIT(KComponentInfoDtorTransitMethod, "CComponentInfo::Dtor transition");
	Context().DataLogger().LogInformation(KComponentInfoDtorTransitMethod);
	delete Context().iComponentInfo;
	Context().iComponentInfo = NULL;
	__UHEAP_MARKEND; /*removed from leaktransitiontest.cpp and added here to resolve alloc issue due to removal of reference to RHeap's member variable*/

	}

inline CComponentInfo_UnitTestContext& CComponentInfo_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KComponentInfoEntryTransition,"CComponentInfo_Entry_Transition");

inline CComponentInfo_Entry_Transition::CComponentInfo_Entry_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KComponentInfoEntryTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentInfo_Entry_Transition::TransitMethodL()
	{
	_LIT(KComponentInfoEntryTransitMethod, "CComponentInfo::Entry transition");
	Context().DataLogger().LogInformation(KComponentInfoEntryTransitMethod);
	Context().iEntryFuncPostCheck = Context().iComponentInfo->GlobalEntryFunc();
	}

inline CComponentInfo_UnitTestContext& CComponentInfo_Entry_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KComponentInfoUnitTestsInfoTransition,"CComponentInfo_UnitTestsInfo_Transition");

inline CComponentInfo_UnitTestsInfo_Transition::CComponentInfo_UnitTestsInfo_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KComponentInfoUnitTestsInfoTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentInfo_UnitTestsInfo_Transition::TransitMethodL()
	{
	_LIT(KComponentInfoUnitTestsInfoTransitMethod, "CComponentInfo::UnitTestsInfo transition");
	Context().DataLogger().LogInformation(KComponentInfoUnitTestsInfoTransitMethod);
	Context().iTestInfoArrayPostCheck = &Context().iComponentInfo->UnitTestsInfo();
	}

inline CComponentInfo_UnitTestContext& CComponentInfo_UnitTestsInfo_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentInfo_UnitTestContext&,iUTContext);
	}
