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
// The implementation of the transition validation classes upon the RStringPool class methods.
// 
//

// ______________________________________________________________________________
//
inline TStringPool_True_TransitionValidator::TStringPool_True_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TStringPool_True_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}
	
inline TBool TStringPool_True_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}
	
// ______________________________________________________________________________
//
inline TStringPool_False_TransitionValidator::TStringPool_False_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TStringPool_False_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TStringPool_False_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return (static_cast<CStringPool_UnitTestContext&>(iUTContext).iResult == EFalse);
	}

