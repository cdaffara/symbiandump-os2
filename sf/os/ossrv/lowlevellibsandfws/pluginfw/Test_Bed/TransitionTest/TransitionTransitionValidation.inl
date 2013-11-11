// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The implementation of the transition validation classes upon the CTransition class methods.
// 
//

// ______________________________________________________________________________
//
inline TTransition_Ctor_TransitionValidator::TTransition_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KCtorFailedInvariant,
	"CTransition_Ctor_Transition invariant failure");

inline TBool TTransition_Ctor_TransitionValidator::ValidatePreConditions()
	{
	return ETrue;
	}

inline TBool TTransition_Ctor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext).iTargetTransition))
		{
		iUTContext.DataLogger().LogInformation(KCtorFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TTransition_Dtor_TransitionValidator::TTransition_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KDtorFailedInvariant,
	"CTransition_Dtor_Transition invariant failure");

inline TBool TTransition_Dtor_TransitionValidator::ValidatePreConditions()
	{
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext).iTargetTransition))
		{
		iUTContext.DataLogger().LogInformation(KDtorFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TTransition_Dtor_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	return ETrue;
	}

// ______________________________________________________________________________
//
inline TTransition_TransitMethodL_TransitionValidator::TTransition_TransitMethodL_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KTransitMethodLFailedInvariant,
	"CTransition_TransitMethodL_Transition invariant failure");

inline TBool TTransition_TransitMethodL_TransitionValidator::ValidatePreConditions()
	{
	// For a c'tor this should be empty.
	if(iUTContext.StateAccessor().InvariantTest(REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext).iTargetTransition))
		{
		iUTContext.DataLogger().LogInformation(KTransitMethodLFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TTransition_TransitMethodL_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	TTransition_StateAccessor& stateAccessor = REINTERPRET_CAST(TTransition_StateAccessor&, context.StateAccessor());
	if(stateAccessor.InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KTransitMethodLFailedInvariant);
		return EFalse;
		}
	return stateAccessor.ErrorCode(context.iTargetTransition) == KErrNone;
	}

// ______________________________________________________________________________
//
inline TTransition_RepeatOnce_TransitionValidator::TTransition_RepeatOnce_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KRepeatOnceFailedInvariant,
	"CTransition_RepeatOnce_Transition invariant failure");

inline TBool TTransition_RepeatOnce_TransitionValidator::ValidatePreConditions()
	{
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KRepeatOnceFailedInvariant);
		return EFalse;
		}
	return !REINTERPRET_CAST(TTransition_StateAccessor&,context.StateAccessor()).IsRepeated(context.iTargetTransition);
	}

inline TBool TTransition_RepeatOnce_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// For a c'tor this should be empty.
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KRepeatOnceFailedInvariant);
		return EFalse;
		}
	return REINTERPRET_CAST(TTransition_StateAccessor&,context.StateAccessor()).IsRepeated(context.iTargetTransition);
	}

// ______________________________________________________________________________
//
inline TTransition_TransitionId_TransitionValidator::TTransition_TransitionId_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KTransitionIdFailedInvariant,
	"CTransition_TransitionId_Transition invariant failure");

inline TBool TTransition_TransitionId_TransitionValidator::ValidatePreConditions()
	{
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KTransitionIdFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TTransition_TransitionId_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// For a c'tor this should be empty.
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	TTransition_StateAccessor& stateAccessor = REINTERPRET_CAST(TTransition_StateAccessor&, context.StateAccessor());
	if(stateAccessor.InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KTransitionIdFailedInvariant);
		return EFalse;
		}
	// Compare the contents of the ids for a match
	return *(context.iTransitionId) == *(stateAccessor.TransitionId(context.iTargetTransition));
	}

// ______________________________________________________________________________
//
inline TTransition_TransitionInfo_TransitionValidator::TTransition_TransitionInfo_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KTransitionInfoFailedInvariant,
	"CTransition_TransitionInfo_Transition invariant failure");

inline TBool TTransition_TransitionInfo_TransitionValidator::ValidatePreConditions()
	{
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KTransitionInfoFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TTransition_TransitionInfo_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// For a c'tor this should be empty.
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	TTransition_StateAccessor& stateAccessor = REINTERPRET_CAST(TTransition_StateAccessor&, context.StateAccessor());
	if(stateAccessor.InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KTransitionInfoFailedInvariant);
		return EFalse;
		}
	return context.iTransitionInfo == stateAccessor.TransitionInfo(context.iTargetTransition);
	}

// ______________________________________________________________________________
//
inline TTransition_SetBlocking_TransitionValidator::TTransition_SetBlocking_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KSetBlockingFailedInvariant,
	"CTransition_SetBlocking_Transition invariant failure");

inline TBool TTransition_SetBlocking_TransitionValidator::ValidatePreConditions()
	{
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KSetBlockingFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TTransition_SetBlocking_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// For a c'tor this should be empty.
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	TTransition_StateAccessor& stateAccessor = REINTERPRET_CAST(TTransition_StateAccessor&, context.StateAccessor());
	if(stateAccessor.InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KSetBlockingFailedInvariant);
		return EFalse;
		}
	return context.iBlockingTransition == stateAccessor.BlockingFlag(context.iTargetTransition);
	}
// ______________________________________________________________________________
//
inline TTransition_IsBlocking_TransitionValidator::TTransition_IsBlocking_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KIsBlockingFailedInvariant,
	"CTransition_IsBlocking_Transition invariant failure");

inline TBool TTransition_IsBlocking_TransitionValidator::ValidatePreConditions()
	{
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KIsBlockingFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TTransition_IsBlocking_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// For a c'tor this should be empty.
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	TTransition_StateAccessor& stateAccessor = REINTERPRET_CAST(TTransition_StateAccessor&, context.StateAccessor());
	if(stateAccessor.InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KIsBlockingFailedInvariant);
		return EFalse;
		}
	return context.iBlockingTransition != stateAccessor.BlockingFlag(context.iTargetTransition);
	}

// ______________________________________________________________________________
//
inline TTransition_GetErrorCode_TransitionValidator::TTransition_GetErrorCode_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KGetErrorCodeFailedInvariant,
	"CTransition_GetErrorCode_Transition invariant failure");

inline TBool TTransition_GetErrorCode_TransitionValidator::ValidatePreConditions()
	{
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KGetErrorCodeFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TTransition_GetErrorCode_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// For a c'tor this should be empty.
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	TTransition_StateAccessor& stateAccessor = REINTERPRET_CAST(TTransition_StateAccessor&, context.StateAccessor());
	if(stateAccessor.InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KGetErrorCodeFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}
// ______________________________________________________________________________
//
inline TTransition_RunTransition_TransitionValidator::TTransition_RunTransition_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KRunTransitionFailedInvariant,
	"CTransition_RunTransition_Transition invariant failure");

inline TBool TTransition_RunTransition_TransitionValidator::ValidatePreConditions()
	{
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KRunTransitionFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TTransition_RunTransition_TransitionValidator::ValidatePostConditions(TTestBedAsyncState aAsyncState)
	{
	// For a c'tor this should be empty.
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	TTransition_StateAccessor& stateAccessor = REINTERPRET_CAST(TTransition_StateAccessor&, context.StateAccessor());
	if(stateAccessor.InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KRunTransitionFailedInvariant);
		return EFalse;
		}
	if(aAsyncState)
		{
		return (stateAccessor.Status(context.iTargetTransition).Int() == stateAccessor.ErrorCode(context.iTargetTransition) &&
				stateAccessor.ErrorCode(context.iTargetTransition) == KErrNone);
		}
	else
		return stateAccessor.ErrorCode(context.iTargetTransition) == KErrNone;
	}

// ______________________________________________________________________________
//
inline TTransition_Cancel_TransitionValidator::TTransition_Cancel_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

_LIT(KCancelFailedInvariant,
	"CTransition_Cancel_Transition invariant failure");

inline TBool TTransition_Cancel_TransitionValidator::ValidatePreConditions()
	{
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	if(context.StateAccessor().InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KCancelFailedInvariant);
		return EFalse;
		}
	return ETrue;
	}

inline TBool TTransition_Cancel_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// For a c'tor this should be empty.
	CTransition_UnitTestContext& context = REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	TTransition_StateAccessor& stateAccessor = REINTERPRET_CAST(TTransition_StateAccessor&, context.StateAccessor());
	if(stateAccessor.InvariantTest(context.iTargetTransition))
		{
		context.DataLogger().LogInformation(KCancelFailedInvariant);
		return EFalse;
		}
	return stateAccessor.Status(context.iTargetTransition).Int() == KErrCancel;
	}

// ______________________________________________________________________________
//
inline TTransition_NULL_TransitionValidator::TTransition_NULL_TransitionValidator(CUnitTestContext& aUTContext)
:TTransitionValidator(aUTContext)
	{
	// Do nothing
	}

inline TBool TTransition_NULL_TransitionValidator::ValidatePreConditions()
	{
	// Always
	return ETrue;
	}

inline TBool TTransition_NULL_TransitionValidator::ValidatePostConditions(TTestBedAsyncState /* aAsyncState */)
	{
	// Always
	return ETrue;
	}
