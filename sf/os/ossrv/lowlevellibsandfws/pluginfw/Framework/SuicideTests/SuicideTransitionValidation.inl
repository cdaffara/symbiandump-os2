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
// Implementations for the transition validation classes for ECOM
// 
//

// ______________________________________________________________________________
//
inline TSuicideInterface_Ctor_TransitionValidator::TSuicideInterface_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TSuicideInterface_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TSuicideInterface_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CSuicideInterface_UnitTestContext&,iUTContext).iExampleInterface))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TSuicideInterface_Dtor_TransitionValidator::TSuicideInterface_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TSuicideInterface_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CSuicideInterface_UnitTestContext&,iUTContext).iExampleInterface))
		return EFalse;
	return ETrue;
	}

inline TBool TSuicideInterface_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TSuicideInterface_FireAndForget_TransitionValidator::TSuicideInterface_FireAndForget_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TSuicideInterface_FireAndForget_TransitionValidator::ValidatePreConditions()
	{
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CSuicideInterface_UnitTestContext&,iUTContext).iExampleInterface))
		return EFalse;
	return ETrue;
	}

inline TBool TSuicideInterface_FireAndForget_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CSuicideInterface_UnitTestContext&,iUTContext).iExampleInterface))
		return EFalse;
	return ETrue;
	}
