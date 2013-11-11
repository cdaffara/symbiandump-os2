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
// The definitions of the transition classes upon the CTransition class methods.
// 
//

#ifndef __TRANSITIONTRANSITIONVALIDATION_H__
#define __TRANSITIONTRANSITIONVALIDATION_H__

#include "TransitionTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CtorUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_Ctor_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_Ctor_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_Ctor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_Ctor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_Ctor_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the DtorUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_Dtor_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_Dtor_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_Dtor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_Dtor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_Dtor_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the TransitMethodL Unit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_TransitMethodL_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_TransitMethodL_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_TransitMethodL_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_TransitMethodL_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_TransitMethodL_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_TransitMethodL_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the RepeatOnce Unit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_RepeatOnce_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_RepeatOnce_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_RepeatOnce_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_RepeatOnce_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_RepeatOnce_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_RepeatOnce_TransitionValidator


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the TransitionId Unit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_TransitionId_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_TransitionId_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_TransitionId_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_TransitionId_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_TransitionId_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_TransitionId_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the TransitionInfo Unit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_TransitionInfo_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_TransitionInfo_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_TransitionInfo_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_TransitionInfo_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_TransitionInfo_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_TransitionInfo_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the IsBlockingTransition Unit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_IsBlocking_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_IsBlocking_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_IsBlocking_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_IsBlocking_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_IsBlocking_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_IsBlocking_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the SetBlockingTransition Unit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_SetBlocking_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_SetBlocking_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_SetBlocking_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_SetBlocking_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_SetBlocking_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_SetBlocking_TransitionValidator


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the GetErrorCodeTransition Unit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_GetErrorCode_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_GetErrorCode_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_GetErrorCode_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_GetErrorCode_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_GetErrorCode_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_GetErrorCode_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the RunTransitionTransition Unit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_RunTransition_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_RunTransition_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_RunTransition_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_RunTransition_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_RunTransition_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_RunTransition_TransitionValidator
// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CancelTransition Unit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_Cancel_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_Cancel_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_Cancel_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_Cancel_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_Cancel_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_Cancel_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the DtorUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTransition test class for a transition.
 */
class TTransition_NULL_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTransition_NULL_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTransition_NULL_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTransition_Dtor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePreConditions();

	/**
		@fn				ValidatePostConditions(TTestBedAsyncState aAsyncState)
		Intended Usage	:	Implemented by the developer to check the
						end state of the transition behaviour.
						When overriding, if the transition calls an asynchronous function
						ValidatePostConditions will be called twice. Firstly, after the
						asynchronous function has been called and, secondly, after the 
						asynchronous request has completed. The parameter aAsyncState can
						be used to distinguish between these two cases.
		Error Condition	: Invalid post-conditions.
		@since			7.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TTransition_Dtor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTransition_Dtor_TransitionValidator


#include "TransitionTransitionValidation.inl"

#endif // __TRANSITIONTRANSITIONVALIDATION_H__

