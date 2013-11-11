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
// The implementation of the transition classes upon the CBackupNotifier class methods.
// 
//

// ______________________________________________________________________________
//
_LIT(KBackupNotifierNewLTransition,"CBackupNotifier_NewL_Transition");

inline CBackupNotifier_NewL_Transition::CBackupNotifier_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KBackupNotifierNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CBackupNotifier_NewL_Transition::TransitMethodL()
	{
	_LIT(KBackupNotifierNewLTransitMethod, "CBackupNotifier::NewL transition");
	Context().DataLogger().LogInformation(KBackupNotifierNewLTransitMethod);
	Context().iBackupNotifier = CBackupNotifier::NewL(*(Context().iStub));
	}

inline CBackupNotifier_UnitTestContext& CBackupNotifier_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CBackupNotifier_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KBackupNotifierDtorTransition,"CBackupNotifier_Dtor_Transition");

// WARNING : It is not recommended that CLeakTestTransitions
// are used on Dtor methods, so if CTransitionType is defined as CLeakTestTransition
// it is suggested that this Dtor transition's base is changed explicitly to CTransition
inline CBackupNotifier_Dtor_Transition::CBackupNotifier_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KBackupNotifierDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CBackupNotifier_Dtor_Transition::TransitMethodL()
	{
	_LIT(KBackupNotifierDtorTransitMethod, "CBackupNotifier::Dtor transition");
	Context().DataLogger().LogInformation(KBackupNotifierDtorTransitMethod);
	delete Context().iBackupNotifier;
	Context().iBackupNotifier = NULL;
	}

inline CBackupNotifier_UnitTestContext& CBackupNotifier_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CBackupNotifier_UnitTestContext&,iUTContext);
	}

// Add additional Transition class Implementations here...

