// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The implementation of the transition validation classes upon the CTestController class methods.
// 
//

// ______________________________________________________________________________
//
inline TTestController_Ctor_TransitionValidator::TTestController_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTestController_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TTestController_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TTestController_Dtor_TransitionValidator::TTestController_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTestController_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}

inline TBool TTestController_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TTestController_ScanDrive_TransitionValidator::TTestController_ScanDrive_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTestController_ScanDrive_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}

inline TBool TTestController_ScanDrive_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TTestController_DiscoverTests_TransitionValidator::TTestController_DiscoverTests_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTestController_DiscoverTests_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}

inline TBool TTestController_DiscoverTests_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TTestController_FindComponents_TransitionValidator::TTestController_FindComponents_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTestController_FindComponents_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}

inline TBool TTestController_FindComponents_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TTestController_GetUnitTests_TransitionValidator::TTestController_GetUnitTests_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTestController_GetUnitTests_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}

inline TBool TTestController_GetUnitTests_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TTestController_Start_TransitionValidator::TTestController_Start_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTestController_Start_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}

inline TBool TTestController_Start_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTestController_UnitTestContext&,iUTContext).iTestController))
		return EFalse;
	return ETrue;
	}



