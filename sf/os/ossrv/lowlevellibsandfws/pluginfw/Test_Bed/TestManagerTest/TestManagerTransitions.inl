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
// The implementation of the transition classes upon the CTestManager class methods.
// 
//

// ______________________________________________________________________________
//
_LIT(KTestManagerNewLTransition,"CTestManager_NewL_Transition");

inline CTestManager_NewL_Transition::CTestManager_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KTestManagerNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTestManager_NewL_Transition::TransitMethodL()
	{
	_LIT(KTestManagerNewLTransitMethod, "CTestManager::NewL transition");
	Context().DataLogger().LogInformation(KTestManagerNewLTransitMethod);
	Context().iTestManager = CTestManager::NewL(Context().iTestListArray, Context().DataLogger(), Context().ManagerObserver());
	}

inline CTestManager_UnitTestContext& CTestManager_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CTestManager_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTestManagerDtorTransition,"CTestManager_Dtor_Transition");

inline CTestManager_Dtor_Transition::CTestManager_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTestManagerDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTestManager_Dtor_Transition::TransitMethodL()
	{
	_LIT(KTestManagerDtorTransitMethod, "CTestManager::Dtor transition");
	Context().DataLogger().LogInformation(KTestManagerDtorTransitMethod);
	delete Context().iTestManager;
	Context().iTestManager = NULL;
	}

inline CTestManager_UnitTestContext& CTestManager_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CTestManager_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTestManagerRunTestsTransition,"CTestManager_RunTests_Transition");

inline CTestManager_RunTests_Transition::CTestManager_RunTests_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTestManagerRunTestsTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTestManager_RunTests_Transition::TransitMethodL()
	{
	_LIT(KTestManagerRunTestsTransitMethod, "CTestManager::RunTests transition");
	Context().DataLogger().LogInformation(KTestManagerRunTestsTransitMethod);
	
	_LIT(KComponentInfoCreateAndDestroyUnitTest,"CComponentInfo_CreateAndDestroy_UnitTest_STUB");
	Context().iTests.ResetAndDestroy();
	Context().iTestController = CTestController::NewLC(CActiveScheduler::Current(),NULL);
	// Find the available components and place them in an array
	Context().iComponents = &(Context().iTestController->FindComponents());
	const RPointerArray<CComponentInfo>& components = *(Context().iComponents);
	const TInt ccount = Context().iComponents->Count();
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

	CleanupStack::PopAndDestroy(Context().iTestController);
	Context().iTestController = NULL;
	Context().iTestManager->RunTests(&Context().iTests);
	}

inline CTestManager_UnitTestContext& CTestManager_RunTests_Transition::Context() const
	{
	return REINTERPRET_CAST(CTestManager_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTestManagerTestComponentLTransition,"CTestManager_TestComponentL_Transition");

inline CTestManager_TestComponentL_Transition::CTestManager_TestComponentL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KTestManagerTestComponentLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTestManager_TestComponentL_Transition::TransitMethodL()
	{
	_LIT(KTestManagerTestComponentLTransitMethod, "CTestManager::TestComponentL transition");
	Context().DataLogger().LogInformation(KTestManagerTestComponentLTransitMethod);
	TTestManager_StateAccessor& accessor = REINTERPRET_CAST(TTestManager_StateAccessor&, Context().StateAccessor());
	accessor.TestComponentL(*(Context().iTestManager), Context().iComponentIndex);
	accessor.ActivateManager(*(Context().iTestManager));
	Context().iTestManagerObserverStub->SetSchedulerStarted(ETrue);
	CActiveScheduler::Start();
	}

inline CTestManager_UnitTestContext& CTestManager_TestComponentL_Transition::Context() const
	{
	return REINTERPRET_CAST(CTestManager_UnitTestContext&,iUTContext);
	}



