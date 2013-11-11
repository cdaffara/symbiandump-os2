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
// The implementation of the transition validation classes upon the CUnitTest class methods.
// 
//

// ______________________________________________________________________________
//
inline TUnitTest_Ctor_TransitionValidator::TUnitTest_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TUnitTest_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TUnitTest_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TUnitTest_Dtor_TransitionValidator::TUnitTest_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TUnitTest_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

inline TBool TUnitTest_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TUnitTest_TransitionSet_TransitionValidator::TUnitTest_TransitionSet_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TUnitTest_TransitionSet_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TUnitTest_TransitionSet_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition. 
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TUnitTest_GetCurrentTransition_TransitionValidator::TUnitTest_GetCurrentTransition_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TUnitTest_GetCurrentTransition_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TUnitTest_GetCurrentTransition_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition. 
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TUnitTest_SetCurrentTransition_TransitionValidator::TUnitTest_SetCurrentTransition_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TUnitTest_SetCurrentTransition_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

inline TBool TUnitTest_SetCurrentTransition_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TUnitTest_SetParameters_TransitionValidator::TUnitTest_SetParameters_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TUnitTest_SetParameters_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

inline TBool TUnitTest_SetParameters_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TUnitTest_PrepareUnitTest_TransitionValidator::TUnitTest_PrepareUnitTest_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KPrepareUnitTestFailedInvariant,
	"CUnitTest_PrepareUnitTest_Transition invariant failure");

inline TBool TUnitTest_PrepareUnitTest_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		{
		iUTContext.DataLogger().LogInformation(KPrepareUnitTestFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TUnitTest_PrepareUnitTest_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		{
		iUTContext.DataLogger().LogInformation(KPrepareUnitTestFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TUnitTest_RunTest_TransitionValidator::TUnitTest_RunTest_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TUnitTest_RunTest_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TUnitTest_RunTest_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TUnitTest_AddTransition_TransitionValidator::TUnitTest_AddTransition_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TUnitTest_AddTransition_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

inline TBool TUnitTest_AddTransition_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TUnitTest_AddBlockingTransition_TransitionValidator::TUnitTest_AddBlockingTransition_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TUnitTest_AddBlockingTransition_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

inline TBool TUnitTest_AddBlockingTransition_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TUnitTest_AddLeaveErrorCode_TransitionValidator::TUnitTest_AddLeaveErrorCode_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KAddLeaveErrorCodeFailedInvariant,
	"CUnitTest_AddLeaveErrorCode_Transition invariant failure");

inline TBool TUnitTest_AddLeaveErrorCode_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		{
		iUTContext.DataLogger().LogInformation(KAddLeaveErrorCodeFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TUnitTest_AddLeaveErrorCode_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		{
		iUTContext.DataLogger().LogInformation(KAddLeaveErrorCodeFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TUnitTest_UnitTestName_TransitionValidator::TUnitTest_UnitTestName_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TUnitTest_UnitTestName_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TUnitTest_UnitTestName_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CUnitTest_UnitTestContext&,iUTContext).iUnitTest))
		return EFalse;
	return ETrue;
	}
