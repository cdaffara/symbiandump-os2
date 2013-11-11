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
// The implementation of the transition validation classes upon the CDefaultLogOutput class methods.
// 
//

// ______________________________________________________________________________
//
inline TDefaultLogOutput_Ctor_TransitionValidator::TDefaultLogOutput_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDefaultLogOutput_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TDefaultLogOutput_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext).iDefaultLogOutput))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDefaultLogOutput_Dtor_TransitionValidator::TDefaultLogOutput_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDefaultLogOutput_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext).iDefaultLogOutput))
		return EFalse;
	return ETrue;
	}

inline TBool TDefaultLogOutput_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDefaultLogOutput_Open_TransitionValidator::TDefaultLogOutput_Open_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDefaultLogOutput_Open_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext).iDefaultLogOutput))
		return EFalse;
	return ETrue;
	}

inline TBool TDefaultLogOutput_Open_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext).iDefaultLogOutput))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDefaultLogOutput_Close_TransitionValidator::TDefaultLogOutput_Close_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDefaultLogOutput_Close_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext).iDefaultLogOutput))
		return EFalse;
	return ETrue;
	}

inline TBool TDefaultLogOutput_Close_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext).iDefaultLogOutput))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TDefaultLogOutput_Write_TransitionValidator::TDefaultLogOutput_Write_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TDefaultLogOutput_Write_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext).iDefaultLogOutput))
		return EFalse;
	return ETrue;
	}

inline TBool TDefaultLogOutput_Write_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CDefaultLogOutput_UnitTestContext&,iUTContext).iDefaultLogOutput))
		return EFalse;
	return ETrue;
	}
