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
// The implementation of the transition validation classes upon the CBackupNotifier class methods.
// 
//

// ______________________________________________________________________________
//
inline TBackupNotifier_Ctor_TransitionValidator::TBackupNotifier_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KCtorFailedInvariant,
	"CBackupNotifier_Ctor_Transition invariant failure");

inline TBool TBackupNotifier_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TBackupNotifier_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// For a d'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CBackupNotifier_UnitTestContext&,iUTContext).iBackupNotifier))
		{
		iUTContext.DataLogger().LogInformation(KCtorFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TBackupNotifier_Dtor_TransitionValidator::TBackupNotifier_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KDtorFailedInvariant,
	"CBackupNotifier_Dtor_Transition invariant failure");

inline TBool TBackupNotifier_Dtor_TransitionValidator::ValidatePreConditions()
	{
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CBackupNotifier_UnitTestContext&,iUTContext).iBackupNotifier))
		{
		iUTContext.DataLogger().LogInformation(KDtorFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TBackupNotifier_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

