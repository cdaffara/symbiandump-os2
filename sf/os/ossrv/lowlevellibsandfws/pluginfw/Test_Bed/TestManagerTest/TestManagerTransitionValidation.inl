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
// The implementation of the transition validation classes upon the CTestManager class methods.
// 
//

// ______________________________________________________________________________
//
inline TTestManager_Ctor_TransitionValidator::TTestManager_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTestManager_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TTestManager_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestManager_UnitTestContext&,iUTContext).iTestManager))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TTestManager_Dtor_TransitionValidator::TTestManager_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTestManager_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestManager_UnitTestContext&,iUTContext).iTestManager))
		return EFalse;
	return ETrue;
	}

inline TBool TTestManager_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TTestManager_RunTests_TransitionValidator::TTestManager_RunTests_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTestManager_RunTests_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestManager_UnitTestContext&,iUTContext).iTestManager))
		return EFalse;
	return ETrue;
	}

inline TBool TTestManager_RunTests_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestManager_UnitTestContext&,iUTContext).iTestManager))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TTestManager_TestComponent_TransitionValidator::TTestManager_TestComponent_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTestManager_TestComponent_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestManager_UnitTestContext&,iUTContext).iTestManager))
		return EFalse;
	return ETrue;
	}

inline TBool TTestManager_TestComponent_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestManager_UnitTestContext&,iUTContext).iTestManager))
		return EFalse;
	return ETrue;
	}
