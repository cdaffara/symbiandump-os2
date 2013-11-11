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
// The implementation of the transition classes upon the CTransition class methods,
// together with the derived transition used for testing.
// 
//

// ______________________________________________________________________________
//
_LIT(KTargetNullMethodTransition,"CTarget_NullMethod_Transition");

inline CTarget_NullMethod_Transition::CTarget_NullMethod_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTargetNullMethodTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTarget_NullMethod_Transition::TransitMethodL()
	{
	// There is no method to execute
	// However exercise both possible results by 
	// provoking a leave with the context's leave code
	// on the second TransitMethodL call.
	_LIT(KTargetNullMethodTransitMethod, "CTarget::NullMethod transition");
	Context().DataLogger().LogInformation(KTargetNullMethodTransitMethod);
	if(Context().iLeaveCode != KErrNone)
		User::Leave(Context().iLeaveCode);
	else
		Context().iLeaveCode = KErrAbort;	// For next entry
	}

inline CTargetTransition_UnitTestContext& CTarget_NullMethod_Transition::Context() const
	{
	return REINTERPRET_CAST(CTargetTransition_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
inline CTargetTransition_UnitTestContext::CTargetTransition_UnitTestContext(CDataLogger& aDataLogger,
												MStateAccessor& aStateAccessor,
												MTransitionObserver& aObserver)
: CUnitTestContext(aDataLogger, aStateAccessor, aObserver)
	{
	//Do nothing
	}

inline CTargetTransition_UnitTestContext::~CTargetTransition_UnitTestContext()
	{
	// Do Nothing
	}

// ______________________________________________________________________________
//
_LIT(KTransitionNewLTransition,"CTransition_NewL_Transition");

inline CTransition_NewL_Transition::CTransition_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KTransitionNewLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTransition_NewL_Transition::TransitMethodL()
	{
	_LIT(KTransitionNewLTransitMethod, "CTransition::NewL transition");
	Context().DataLogger().LogInformation(KTransitionNewLTransitMethod);
	Context().iTargetTransition = new(ELeave)CTarget_NullMethod_Transition(*(Context().iTargetContext), *(Context().iTargetValidator));
	}

inline CTransition_UnitTestContext& CTransition_NewL_Transition::Context() const
	{
	return REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTransitionDtorTransition,"CTransition_Dtor_Transition");

inline CTransition_Dtor_Transition::CTransition_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTransitionDtorTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTransition_Dtor_Transition::TransitMethodL()
	{
	_LIT(KTransitionDtorTransitMethod, "CTransition::Dtor transition");
	Context().DataLogger().LogInformation(KTransitionDtorTransitMethod);
	delete Context().iTargetTransition;
	Context().iTargetTransition = NULL;
	}

inline CTransition_UnitTestContext& CTransition_Dtor_Transition::Context() const
	{
	return REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTransitionTransitMethodLTransition,"CTransition_TransitMethodL_Transition");

inline CTransition_TransitMethodL_Transition::CTransition_TransitMethodL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransitionType(KTransitionTransitMethodLTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTransition_TransitMethodL_Transition::TransitMethodL()
	{
	_LIT(KTransitionTransitMethodLTransitMethod, "CTransition::TransitMethodL transition");
	Context().DataLogger().LogInformation(KTransitionTransitMethodLTransitMethod);
	Context().iTargetTransition->TransitMethodL();
	}

inline CTransition_UnitTestContext& CTransition_TransitMethodL_Transition::Context() const
	{
	return REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTransitionRepeatOnceTransition,"CTransition_RepeatOnce_Transition");

inline CTransition_RepeatOnce_Transition::CTransition_RepeatOnce_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTransitionRepeatOnceTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTransition_RepeatOnce_Transition::TransitMethodL()
	{
	_LIT(KTransitionRepeatOnceTransitMethod, "CTransition::RepeatOnce transition");
	Context().DataLogger().LogInformation(KTransitionRepeatOnceTransitMethod);
	Context().iTargetTransition->RepeatOnce();
	}

inline CTransition_UnitTestContext& CTransition_RepeatOnce_Transition::Context() const
	{
	return REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTransitionTransitionIdTransition,"CTransition_TransitionId_Transition");

inline CTransition_TransitionId_Transition::CTransition_TransitionId_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTransitionTransitionIdTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTransition_TransitionId_Transition::TransitMethodL()
	{
	_LIT(KTransitionTransitionIdTransitMethod, "CTransition::TransitionId transition");
	Context().DataLogger().LogInformation(KTransitionTransitionIdTransitMethod);
	Context().iTransitionId = CONST_CAST(TDesC*, &(Context().iTargetTransition->TransitionId()));
	}

inline CTransition_UnitTestContext& CTransition_TransitionId_Transition::Context() const
	{
	return REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTransitionTransitionInfoTransition,"CTransition_TransitionInfo_Transition");

inline CTransition_TransitionInfo_Transition::CTransition_TransitionInfo_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTransitionTransitionInfoTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTransition_TransitionInfo_Transition::TransitMethodL()
	{
	_LIT(KTransitionTransitionInfoTransitMethod, "CTransition::TransitionInfo transition");
	Context().DataLogger().LogInformation(KTransitionTransitionInfoTransitMethod);
	Context().iTransitionInfo = CONST_CAST(TTransitionInfo*, &(Context().iTargetTransition->TransitionInfo()));
	}

inline CTransition_UnitTestContext& CTransition_TransitionInfo_Transition::Context() const
	{
	return REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTransitionSetBlockingTransition,"CTransition_SetBlocking_Transition");

inline CTransition_SetBlocking_Transition::CTransition_SetBlocking_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTransitionSetBlockingTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTransition_SetBlocking_Transition::TransitMethodL()
	{
	_LIT(KTransitionSetBlockingTransitMethod, "CTransition::SetBlocking transition");
	Context().DataLogger().LogInformation(KTransitionSetBlockingTransitMethod);
	Context().iTargetTransition->SetBlockingTransition(Context().iBlockingTransition);
	}

inline CTransition_UnitTestContext& CTransition_SetBlocking_Transition::Context() const
	{
	return REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTransitionIsBlockingTransition,"CTransition_IsBlocking_Transition");

inline CTransition_IsBlocking_Transition::CTransition_IsBlocking_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTransitionIsBlockingTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTransition_IsBlocking_Transition::TransitMethodL()
	{
	_LIT(KTransitionIsBlockingTransitMethod, "CTransition::IsBlocking transition");
	Context().DataLogger().LogInformation(KTransitionIsBlockingTransitMethod);
	Context().iBlockingTransition = !Context().iTargetTransition->IsBlockingTransition();
	}

inline CTransition_UnitTestContext& CTransition_IsBlocking_Transition::Context() const
	{
	return REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTransitionGetErrorCodeTransition,"CTransition_GetErrorCode_Transition");

inline CTransition_GetErrorCode_Transition::CTransition_GetErrorCode_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTransitionGetErrorCodeTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTransition_GetErrorCode_Transition::TransitMethodL()
	{
	_LIT(KTransitionGetErrorCodeTransitMethod, "CTransition::GetErrorCode transition");
	Context().DataLogger().LogInformation(KTransitionGetErrorCodeTransitMethod);
	Context().iTargetErrorCode = Context().iTargetTransition->GetErrorCode();
	}

inline CTransition_UnitTestContext& CTransition_GetErrorCode_Transition::Context() const
	{
	return REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTransitionRunTransitionTransition,"CTransition_RunTransition_Transition");

inline CTransition_RunTransition_Transition::CTransition_RunTransition_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTransitionRunTransitionTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTransition_RunTransition_Transition::TransitMethodL()
	{
	_LIT(KTransitionRunTransitionTransitMethod, "CTransition::RunTransition transition");
	Context().DataLogger().LogInformation(KTransitionRunTransitionTransitMethod);
	// Kick off this transition as an asynchronous call on the RunTransition
	// So that this transition will complete when our target transition completes and signals its observer
	iStatus = KRequestPending;
	TRequestStatus* status = &iStatus;
	Context().iTargetTransition->RunTransition(status);
	}

inline void CTransition_RunTransition_Transition::DoCancel()
	{
	_LIT(KTransitionRunTransitionTransitMethodCancel, "CTransition::RunTransition transition cancelled");
	Context().DataLogger().LogInformation(KTransitionRunTransitionTransitMethodCancel);
	Context().iTargetTransition->Cancel();
	}

inline CTransition_UnitTestContext& CTransition_RunTransition_Transition::Context() const
	{
	return REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	}

// ______________________________________________________________________________
//
_LIT(KTransitionCancelTransition,"CTransition_Cancel_Transition");

inline CTransition_Cancel_Transition::CTransition_Cancel_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator)
: CTransition(KTransitionCancelTransition, aUTContext, aValidator)
	{
	// Do nothing here.
	}

inline void CTransition_Cancel_Transition::TransitMethodL()
	{
	_LIT(KTransitionCancelTransitMethod, "CTransition::Cancel transition");
	Context().DataLogger().LogInformation(KTransitionCancelTransitMethod);
	Context().iTargetTransition->Cancel();
	}

inline CTransition_UnitTestContext& CTransition_Cancel_Transition::Context() const
	{
	return REINTERPRET_CAST(CTransition_UnitTestContext&,iUTContext);
	}

