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
// The implementation of the transition classes upon the CComponentTester class methods.
// 
//

// ______________________________________________________________________________
//
_LIT(KComponentTesterNewLTransition,"CComponentTester_NewL_Transition");

inline CComponentTester_NewL_Transition::CComponentTester_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KComponentTesterNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentTester_NewL_Transition::TransitMethodL()
	{
	_LIT(KComponentTesterNewLTransitMethod, "CComponentTester::NewL transition");
	Context().DataLogger().LogInformation(KComponentTesterNewLTransitMethod);
	TComponentTester_StateAccessor& accessor = REINTERPRET_CAST(TComponentTester_StateAccessor&, Context().StateAccessor());
	Context().iComponentUnderTest = accessor.CreateComponentTester(*(Context().iComponentTester), *Context().iDataLogger, *Context().iObserver);
	}

inline CComponentTester_UnitTestContext& CComponentTester_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KComponentTesterDtorTransition,"CComponentTester_Dtor_Transition");

inline CComponentTester_Dtor_Transition::CComponentTester_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KComponentTesterDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentTester_Dtor_Transition::TransitMethodL()
	{
	_LIT(KComponentTesterDtorTransitMethod, "CComponentTester::Dtor transition");
	Context().DataLogger().LogInformation(KComponentTesterDtorTransitMethod);
	delete Context().iComponentTester;
	Context().iComponentTester = NULL;
	}

inline CComponentTester_UnitTestContext& CComponentTester_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KComponentTesterTransitionSetsLTransition,"CComponentTester_TransitionSetsL_Transition");

inline CComponentTester_TransitionSetsL_Transition::CComponentTester_TransitionSetsL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KComponentTesterTransitionSetsLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentTester_TransitionSetsL_Transition::TransitMethodL()
	{
	_LIT(KComponentTesterTransitionSetsLTransitMethod, "CComponentTester::TransitionSetsL transition");
	Context().DataLogger().LogInformation(KComponentTesterTransitionSetsLTransitMethod);
	Context().iComponentTester->TransitionSetsL();
	}

inline CComponentTester_UnitTestContext& CComponentTester_TransitionSetsL_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KComponentTesterAddUnitTestLTransition,"CComponentTester_AddUnitTestL_Transition");

inline CComponentTester_AddUnitTestL_Transition::CComponentTester_AddUnitTestL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KComponentTesterAddUnitTestLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentTester_AddUnitTestL_Transition::TransitMethodL()
	{
	_LIT(KComponentTesterAddUnitTestLTransitMethod, "CComponentTester::AddUnitTestL transition");
	Context().DataLogger().LogInformation(KComponentTesterAddUnitTestLTransitMethod);
	TComponentTester_StateAccessor& accessor = REINTERPRET_CAST(TComponentTester_StateAccessor&, Context().StateAccessor());
	accessor.AddUnitTestL(*(Context().iComponentTester), (Context().iUnitTest));
	}

inline CComponentTester_UnitTestContext& CComponentTester_AddUnitTestL_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KComponentTesterAddParamUnitTestLTransition,"CComponentTester_AddParamUnitTestL_Transition");

inline CComponentTester_AddParamUnitTestL_Transition::CComponentTester_AddParamUnitTestL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KComponentTesterAddParamUnitTestLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentTester_AddParamUnitTestL_Transition::TransitMethodL()
	{
	_LIT(KComponentTesterAddParamUnitTestLTransitMethod, "CComponentTester::AddParamUnitTestL transition");
	Context().DataLogger().LogInformation(KComponentTesterAddParamUnitTestLTransitMethod);
	TComponentTester_StateAccessor& accessor = REINTERPRET_CAST(TComponentTester_StateAccessor&, Context().StateAccessor());
	accessor.AddParamUnitTestL(*(Context().iComponentTester), (Context().iUnitTest));
	}

inline CComponentTester_UnitTestContext& CComponentTester_AddParamUnitTestL_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KComponentTesterTestComponentTransition,"CComponentTester_TestComponent_Transition");

inline CComponentTester_TestComponent_Transition::CComponentTester_TestComponent_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KComponentTesterTestComponentTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentTester_TestComponent_Transition::TransitMethodL()
	{
	_LIT(KComponentTesterTestComponentTransitMethod, "CComponentTester::TestComponent transition");
	Context().DataLogger().LogInformation(KComponentTesterTestComponentTransitMethod);
	Context().iComponentTester->TestComponent(&Context().iTests);
	}

inline CComponentTester_UnitTestContext& CComponentTester_TestComponent_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KComponentTesterRunLTransition,"CComponentTester_RunL_Transition");

inline CComponentTester_RunL_Transition::CComponentTester_RunL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KComponentTesterRunLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentTester_RunL_Transition::TransitMethodL()
	{
	_LIT(KComponentTesterRunLTransitMethod, "CComponentTester::RunL transition");
	Context().DataLogger().LogInformation(KComponentTesterRunLTransitMethod);
	TComponentTester_StateAccessor& accessor = REINTERPRET_CAST(TComponentTester_StateAccessor&, Context().StateAccessor());
	accessor.RunL(*(Context().iComponentTester));
	}

inline CComponentTester_UnitTestContext& CComponentTester_RunL_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KComponentTesterCompleteTransition,"CComponentTester_Complete_Transition");

inline CComponentTester_Complete_Transition::CComponentTester_Complete_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KComponentTesterCompleteTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CComponentTester_Complete_Transition::TransitMethodL()
	{
	_LIT(KComponentTesterCompleteTransitMethod, "CComponentTester::Complete transition");
	Context().DataLogger().LogInformation(KComponentTesterCompleteTransitMethod);
	Context().iComponentTester->Complete(Context().iUnitTest);
	}

inline CComponentTester_UnitTestContext& CComponentTester_Complete_Transition::Context() const
	{
	return REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext);
	}


// Add additional Transition class Implementations here...

