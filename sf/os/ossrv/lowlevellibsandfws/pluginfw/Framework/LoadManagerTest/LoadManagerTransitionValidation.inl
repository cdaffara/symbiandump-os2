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
// The implementation of the validation classes for CLoadManager transitions.
// 
//

// ______________________________________________________________________________
//
inline TLoadManager_Ctor_TransitionValidator::TLoadManager_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TLoadManager_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TLoadManager_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext).iLoadManager))
		return EFalse;
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TLoadManager_Dtor_TransitionValidator::TLoadManager_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TLoadManager_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext).iLoadManager))
		return EFalse;
	return ETrue;
	}

inline TBool TLoadManager_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TLoadManager_InstantiationMethod_TransitionValidator::TLoadManager_InstantiationMethod_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TLoadManager_InstantiationMethod_TransitionValidator::ValidatePreConditions()
	{
	CLoadManager_UnitTestContext& context = REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iLoadManager))
		return EFalse;
	return context.iLoadMethod == NULL;
	}

inline TBool TLoadManager_InstantiationMethod_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CLoadManager_UnitTestContext& context = REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iLoadManager))
		return EFalse;
	return (context.iLoadMethod != NULL);
	}

// ______________________________________________________________________________
//
inline TLoadManager_InstantiationFailure_TransitionValidator::TLoadManager_InstantiationFailure_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TLoadManager_InstantiationFailure_TransitionValidator::ValidatePreConditions()
	{
	CLoadManager_UnitTestContext& context = REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iLoadManager))
		return EFalse;
	return context.iLoadMethod == NULL;
	}

inline TBool TLoadManager_InstantiationFailure_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CLoadManager_UnitTestContext& context = REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iLoadManager))
		return EFalse;
	return (context.iLoadMethod == NULL);
	}

// ______________________________________________________________________________
//
inline TLoadManager_DestroyThis_TransitionValidator::TLoadManager_DestroyThis_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TLoadManager_DestroyThis_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext).iLoadManager))
		return EFalse;
	return ETrue;
	}

inline TBool TLoadManager_DestroyThis_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CLoadManager_UnitTestContext&,iUTContext).iLoadManager))
		return EFalse;
	return ETrue;
	}

