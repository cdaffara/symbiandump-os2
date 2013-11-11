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
// The implementation of the transition classes upon the CTestController class methods.
// 
//

IMPORT_C CComponentTester* NewComponentTestLC_STUB(
	CDataLogger& aDataLogger,
	MComponentTestObserver&	aComponentTestObserver);

// ______________________________________________________________________________
//
_LIT(KTestControllerNewLTransition,"CTestController_NewL_Transition");

inline CTestController_NewL_Transition::CTestController_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KTestControllerNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTestController_NewL_Transition::TransitMethodL()
	{
	_LIT(KTestControllerNewLTransitMethod, "CTestController::NewL transition");
	Context().DataLogger().LogInformation(KTestControllerNewLTransitMethod);
	Context().iTestController = CTestController::NewL(Context().iScheduler, NewComponentTestLC_STUB, NULL, Context().iLogInfo);
	}

inline CTestController_UnitTestContext& CTestController_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTestControllerDtorTransition,"CTestController_Dtor_Transition");

inline CTestController_Dtor_Transition::CTestController_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTestControllerDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTestController_Dtor_Transition::TransitMethodL()
	{
	_LIT(KTestControllerDtorTransitMethod, "CTestController::Dtor transition");
	Context().DataLogger().LogInformation(KTestControllerDtorTransitMethod);
	delete Context().iTestController;
	Context().iTestController = NULL;
	}

inline CTestController_UnitTestContext& CTestController_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTestControllerFindComponentsTransition,"CTestController_FindComponents_Transition");

inline CTestController_FindComponents_Transition::CTestController_FindComponents_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTestControllerFindComponentsTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

// This is the name of the test that we wish to find
_LIT(KComponentInfoCreateAndDestroyUnitTest,"CTestController_CreateAndDestroy_UnitTest_STUB");

inline void CTestController_FindComponents_Transition::TransitMethodL()
	{
	_LIT(KTestControllerFindComponentsTransitMethod, "CTestController::FindComponents transition");
	Context().DataLogger().LogInformation(KTestControllerFindComponentsTransitMethod);
	
	Context().iTests.ResetAndDestroy();
	// Find the available components and place them in an array
	const RPointerArray<CComponentInfo>& components = Context().iTestController->FindComponents();
	const TInt ccount = components.Count();
	TBool found = EFalse;
	for(TInt i = 0; i < ccount && !found; ++i)
		{
		// Find the unit tests available for each the previously found components
		const RPointerArray<CUnitTestInfo>& unitTests = components[i]->UnitTestsInfo();
		const TInt ucount = unitTests.Count();
		for(TInt j = 0; j < ucount && !found; ++j)
			{
			// Look for the unit test that matches the one that we wish to find
			found = unitTests[j]->UnitTestId() == KComponentInfoCreateAndDestroyUnitTest;
			if(found)
				{
				TTestInfo* testInfo = new(ELeave) TTestInfo;
				testInfo->iComponentId = i;
				testInfo->iUnitTestId = j;
				testInfo->iRunTime = 0;		// Run immediately
				testInfo->iParameters = NULL;
				User::LeaveIfError(Context().iTests.Append(testInfo));
				}
			}
		}
	if(!found)
		User::Leave(KErrNotFound);
	}

inline CTestController_UnitTestContext& CTestController_FindComponents_Transition::Context() const
	{
	return REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTestControllerStartTransition,"CTestController_Start_Transition");

inline CTestController_Start_Transition::CTestController_Start_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTestControllerStartTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTestController_Start_Transition::TransitMethodL()
	{
	_LIT(KTestControllerStartTransitMethod, "CTestController::Start transition");
	Context().DataLogger().LogInformation(KTestControllerStartTransitMethod);
	Context().iTestController->Start(&Context().iTests);
	}

inline CTestController_UnitTestContext& CTestController_Start_Transition::Context() const
	{
	return REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext);
	}


// Add additional Transition class Implementations here...

