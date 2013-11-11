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
// The implementation of the transition classes upon the CUnitTest class methods.
// 
//

// ______________________________________________________________________________
//
_LIT(KUnitTestNewLTransition,"CUnitTest_NewL_Transition");

inline CUnitTest_NewL_Transition::CUnitTest_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KUnitTestNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_NewL_Transition::TransitMethodL()
	{
	_LIT(KUnitTestNewLTransitMethod, "CUnitTest::NewL transition");
	Context().DataLogger().LogInformation(KUnitTestNewLTransitMethod);
	TUnitTest_StateAccessor& accessor = REINTERPRET_CAST(TUnitTest_StateAccessor&, Context().StateAccessor());
	Context().iUnitTestUnderTest =  accessor.CreateUnitTest(*(Context().iUnitTest), *Context().iName, *Context().iDataLogger, *Context().iUnitTestObserver);
	}

inline CUnitTest_UnitTestContext& CUnitTest_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestDtorTransition,"CUnitTest_Dtor_Transition");

inline CUnitTest_Dtor_Transition::CUnitTest_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KUnitTestDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_Dtor_Transition::TransitMethodL()
	{
	_LIT(KUnitTestDtorTransitMethod, "CUnitTest::Dtor transition");
	Context().DataLogger().LogInformation(KUnitTestDtorTransitMethod);
	delete Context().iUnitTest;
	Context().iUnitTest = NULL;
	}

inline CUnitTest_UnitTestContext& CUnitTest_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestTransitionSetLTransition,"CUnitTest_TransitionSetL_Transition");

inline CUnitTest_TransitionSetL_Transition::CUnitTest_TransitionSetL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KUnitTestTransitionSetLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_TransitionSetL_Transition::TransitMethodL()
	{
	_LIT(KUnitTestTransitionSetLTransitMethod, "CUnitTest::TransitionSetL transition");
	Context().DataLogger().LogInformation(KUnitTestTransitionSetLTransitMethod);
	Context().iUnitTestUnderTest->TransitionSetL();
	}

inline CUnitTest_UnitTestContext& CUnitTest_TransitionSetL_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestGetCurrentTransitionTransition,"CUnitTest_GetCurrentTransition_Transition");

inline CUnitTest_GetCurrentTransition_Transition::CUnitTest_GetCurrentTransition_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KUnitTestGetCurrentTransitionTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_GetCurrentTransition_Transition::TransitMethodL()
	{
	_LIT(KUnitTestGetCurrentTransitionTransitMethod, "CUnitTest::GetCurrentTransition transition");
	Context().DataLogger().LogInformation(KUnitTestGetCurrentTransitionTransitMethod);
	/*Context().iCurrentTransition =*/ Context().iUnitTestUnderTest->GetCurrentTransition();
	}

inline CUnitTest_UnitTestContext& CUnitTest_GetCurrentTransition_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestSetCurrentTransitionTransition,"CUnitTest_SetCurrentTransition_Transition");

inline CUnitTest_SetCurrentTransition_Transition::CUnitTest_SetCurrentTransition_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KUnitTestSetCurrentTransitionTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_SetCurrentTransition_Transition::TransitMethodL()
	{
	_LIT(KUnitTestSetCurrentTransitionTransitMethod, "CUnitTest::SetCurrentTransition transition");
	Context().DataLogger().LogInformation(KUnitTestSetCurrentTransitionTransitMethod);
	Context().iUnitTest->SetCurrentTransition(*(Context().iTransition));
	}

inline CUnitTest_UnitTestContext& CUnitTest_SetCurrentTransition_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestSetParametersLTransition,"CUnitTest_SetParametersL_Transition");

inline CUnitTest_SetParametersL_Transition::CUnitTest_SetParametersL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KUnitTestSetParametersLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_SetParametersL_Transition::TransitMethodL()
	{
	_LIT(KUnitTestSetParametersLTransitMethod, "CUnitTest::SetParametersL transition");
	Context().DataLogger().LogInformation(KUnitTestSetParametersLTransitMethod);
	Context().iUnitTest->SetParametersL(Context().iParams);
	}

inline CUnitTest_UnitTestContext& CUnitTest_SetParametersL_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestPrepareUnitTestLTransition,"CUnitTest_PrepareUnitTestL_Transition");

inline CUnitTest_PrepareUnitTestL_Transition::CUnitTest_PrepareUnitTestL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KUnitTestPrepareUnitTestLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_PrepareUnitTestL_Transition::TransitMethodL()
	{
	_LIT(KUnitTestPrepareUnitTestLTransitMethod, "CUnitTest::PrepareUnitTestL transition");
	Context().DataLogger().LogInformation(KUnitTestPrepareUnitTestLTransitMethod);
	Context().iUnitTest->PrepareUnitTestL();
	}

inline CUnitTest_UnitTestContext& CUnitTest_PrepareUnitTestL_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestRunTestTransition,"CUnitTest_RunTest_Transition");

inline CUnitTest_RunTest_Transition::CUnitTest_RunTest_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KUnitTestRunTestTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_RunTest_Transition::TransitMethodL()
	{
	_LIT(KUnitTestRunTestTransitMethod, "CUnitTest::RunTest transition");
	Context().DataLogger().LogInformation(KUnitTestRunTestTransitMethod);
	Context().iUnitTest->RunTest(Context().iTimeAfter);
	}

inline CUnitTest_UnitTestContext& CUnitTest_RunTest_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestAddTransitionLTransition,"CUnitTest_AddTransitionL_Transition");

inline CUnitTest_AddTransitionL_Transition::CUnitTest_AddTransitionL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KUnitTestAddTransitionLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_AddTransitionL_Transition::TransitMethodL()
	{
	_LIT(KUnitTestAddTransitionLTransitMethod, "CUnitTest::AddTransitionL transition");
	Context().DataLogger().LogInformation(KUnitTestAddTransitionLTransitMethod);	
	TUnitTest_StateAccessor& accessor = REINTERPRET_CAST(TUnitTest_StateAccessor&, Context().StateAccessor());
	accessor.AddTransitionL(*(Context().iUnitTest), (Context().iTransition));
	}

inline CUnitTest_UnitTestContext& CUnitTest_AddTransitionL_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestAddBlockingTransitionLTransition,"CUnitTest_AddBlockingTransitionL_Transition");

inline CUnitTest_AddBlockingTransitionL_Transition::CUnitTest_AddBlockingTransitionL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KUnitTestAddBlockingTransitionLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_AddBlockingTransitionL_Transition::TransitMethodL()
	{
	_LIT(KUnitTestAddBlockingTransitionLTransitMethod, "CUnitTest::AddBlockingTransitionL transition");
	Context().DataLogger().LogInformation(KUnitTestAddBlockingTransitionLTransitMethod);
	TUnitTest_StateAccessor& accessor = REINTERPRET_CAST(TUnitTest_StateAccessor&, Context().StateAccessor());
	accessor.AddBlockingTransitionL(*(Context().iUnitTest), (Context().iTransition));
	}

inline CUnitTest_UnitTestContext& CUnitTest_AddBlockingTransitionL_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestAddLeaveErrorCodeLTransition,"CUnitTest_AddLeaveErrorCodeL_Transition");

inline CUnitTest_AddLeaveErrorCodeL_Transition::CUnitTest_AddLeaveErrorCodeL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KUnitTestAddLeaveErrorCodeLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_AddLeaveErrorCodeL_Transition::TransitMethodL()
	{
	_LIT(KUnitTestAddLeaveErrorCodeLTransitMethod, "CUnitTest::AddLeaveErrorCodeL transition");
	Context().DataLogger().LogInformation(KUnitTestAddLeaveErrorCodeLTransitMethod);
	TUnitTest_StateAccessor& accessor = REINTERPRET_CAST(TUnitTest_StateAccessor&, Context().StateAccessor());
	accessor.AddLeaveErrorCodeL(*(Context().iUnitTest), (Context().iLeaveErrorCode));
	}

inline CUnitTest_UnitTestContext& CUnitTest_AddLeaveErrorCodeL_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KUnitTestUnitTestNameTransition,"CUnitTest_UnitTestName_Transition");

inline CUnitTest_UnitTestName_Transition::CUnitTest_UnitTestName_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KUnitTestUnitTestNameTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CUnitTest_UnitTestName_Transition::TransitMethodL()
	{
	_LIT(KUnitTestUnitTestNameTransitMethod, "CUnitTest::UnitTestName transition");
	Context().DataLogger().LogInformation(KUnitTestUnitTestNameTransitMethod);
	Context().iUnitTest->UnitTestName();
	}

inline CUnitTest_UnitTestContext& CUnitTest_UnitTestName_Transition::Context() const
	{
	return REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext);
	}


// Add additional Transition class Implementations here...

