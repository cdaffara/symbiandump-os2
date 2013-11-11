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
// The CResolver test transition class implementations
// 
//

#include "ResolverStateAccessor.h"

// ______________________________________________________________________________
//
_LIT(KDefaultResolverNewLTransition,"CDefaultResolver_NewL_Transition");

inline CDefaultResolver_NewL_Transition::CDefaultResolver_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDefaultResolverNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDefaultResolver_NewL_Transition::TransitMethodL()
	{
	_LIT(KDefaultResolverNewLTransitMethod, "CDefaultResolver::NewL transition");
	Context().DataLogger().LogInformation(KDefaultResolverNewLTransitMethod);

	Context().iDefaultResolver = CDefaultResolver::NewL(*Context().iRegistry);
	}

inline CDefaultResolver_UnitTestContext& CDefaultResolver_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDefaultResolverDtorTransition,"CDefaultResolver_Dtor_Transition");

inline CDefaultResolver_Dtor_Transition::CDefaultResolver_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDefaultResolverDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDefaultResolver_Dtor_Transition::TransitMethodL()
	{
	_LIT(KDefaultResolverDtorTransitMethod, "CDefaultResolver::Dtor transition");
	Context().DataLogger().LogInformation(KDefaultResolverDtorTransitMethod);

	delete Context().iDefaultResolver;
	Context().iDefaultResolver = 0;
	}

inline CDefaultResolver_UnitTestContext& CDefaultResolver_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDefaultResolverIdentifyImplementationLTransition,"CDefaultResolver_IdentifyImplementationL_Transition");

inline CDefaultResolver_IdentifyImplementationL_Transition::CDefaultResolver_IdentifyImplementationL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDefaultResolverIdentifyImplementationLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDefaultResolver_IdentifyImplementationL_Transition::TransitMethodL()
	{
	_LIT(KDefaultResolverIdentifyImplementationLTransitMethod, "CDefaultResolver::IdentifyImplementationL transition");
	Context().DataLogger().LogInformation(KDefaultResolverIdentifyImplementationLTransitMethod);

	Context().iResolvedImpUid = 
		Context().iDefaultResolver->IdentifyImplementationL(Context().iInterfaceUid,
															Context().iAdditionalParameters);
	}

inline CDefaultResolver_UnitTestContext& CDefaultResolver_IdentifyImplementationL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDefaultResolverListAllLTransition,"CDefaultResolver_ListAllL_Transition");

inline CDefaultResolver_ListAllL_Transition::CDefaultResolver_ListAllL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KDefaultResolverListAllLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDefaultResolver_ListAllL_Transition::TransitMethodL()
	{
	_LIT(KDefaultResolverListAllLTransitMethod, "CDefaultResolver::ListAllL transition");
	Context().DataLogger().LogInformation(KDefaultResolverListAllLTransitMethod);

	Context().iDefaultResolver->ListAllL(Context().iInterfaceUid,
										 Context().iAdditionalParameters);
	}

inline CDefaultResolver_UnitTestContext& CDefaultResolver_ListAllL_Transition::Context() const
	{
	return REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDefaultResolverMatchTransition,"CDefaultResolver_Match_Transition");

inline CDefaultResolver_Match_Transition::CDefaultResolver_Match_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDefaultResolverMatchTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDefaultResolver_Match_Transition::TransitMethodL()
	{
	_LIT(KDefaultResolverMatchTransitMethod, "CDefaultResolver::Match transition");
	Context().DataLogger().LogInformation(KDefaultResolverMatchTransitMethod);

	TDefaultResolver_StateAccessor& accessor = 
		REINTERPRET_CAST(TDefaultResolver_StateAccessor&, Context().StateAccessor());
	Context().iMatchResult = accessor.Match(*Context().iDefaultResolver, 
											*Context().iImplementationType,
											*Context().iMatchType,
											 Context().iUseWildcards);
	}

inline CDefaultResolver_UnitTestContext& CDefaultResolver_Match_Transition::Context() const
	{
	return REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KDefaultResolverResolveTransition,"CDefaultResolver_Resolve_Transition");

inline CDefaultResolver_Resolve_Transition::CDefaultResolver_Resolve_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KDefaultResolverResolveTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CDefaultResolver_Resolve_Transition::TransitMethodL()
	{
	_LIT(KDefaultResolverResolveTransitMethod, "CDefaultResolver::Resolve transition");
	Context().DataLogger().LogInformation(KDefaultResolverResolveTransitMethod);

	TDefaultResolver_StateAccessor& accessor = 
		REINTERPRET_CAST(TDefaultResolver_StateAccessor&, Context().StateAccessor());
	accessor.Resolve(*Context().iDefaultResolver, 
					  Context().iImplementationData, 
					  Context().iAdditionalParameters);
	}

inline CDefaultResolver_UnitTestContext& CDefaultResolver_Resolve_Transition::Context() const
	{
	return REINTERPRET_CAST(CDefaultResolver_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
inline CDefaultResolver_UnitTestContext::CDefaultResolver_UnitTestContext(CDataLogger& aDataLogger,
												MStateAccessor& aStateAccessor,
												MTransitionObserver& aObserver)
: CUnitTestContext(aDataLogger, aStateAccessor, aObserver)
	{
	//Do nothing
	}

inline CDefaultResolver_UnitTestContext::~CDefaultResolver_UnitTestContext()
	{
	iFs.Close();

	iImplementationData.Reset();

	delete iDefaultResolver;
	delete iRegistry;
	delete iImplementationType;
	delete iMatchType;
	}

