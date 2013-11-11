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
// The implementation of the transition validation classes upon the CComponentTester class methods.
// 
//

// ______________________________________________________________________________
//
inline TComponentTester_Ctor_TransitionValidator::TComponentTester_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentTester_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TComponentTester_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TComponentTester_Dtor_TransitionValidator::TComponentTester_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentTester_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

inline TBool TComponentTester_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TComponentTester_TransitionSets_TransitionValidator::TComponentTester_TransitionSets_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentTester_TransitionSets_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

inline TBool TComponentTester_TransitionSets_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TComponentTester_AddUnitTest_TransitionValidator::TComponentTester_AddUnitTest_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentTester_AddUnitTest_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

inline TBool TComponentTester_AddUnitTest_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TComponentTester_AddParamUnitTest_TransitionValidator::TComponentTester_AddParamUnitTest_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentTester_AddParamUnitTest_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

inline TBool TComponentTester_AddParamUnitTest_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TComponentTester_TestComponent_TransitionValidator::TComponentTester_TestComponent_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentTester_TestComponent_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

inline TBool TComponentTester_TestComponent_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TComponentTester_Run_TransitionValidator::TComponentTester_Run_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentTester_Run_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

inline TBool TComponentTester_Run_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TComponentTester_Complete_TransitionValidator::TComponentTester_Complete_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TComponentTester_Complete_TransitionValidator::ValidatePreConditions()
	{
	// Implement the correct pre-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct pre-condition test for this unit test transition.")
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}

inline TBool TComponentTester_Complete_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Implement the correct post-condition test for this unit test transition.
//#pragma message( __FILE__LINE__ "TO DO : Implement the correct post-condition test for this unit test transition.")
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CComponentTester_UnitTestContext&,iUTContext).iComponentTester))
		return EFalse;
	return ETrue;
	}
