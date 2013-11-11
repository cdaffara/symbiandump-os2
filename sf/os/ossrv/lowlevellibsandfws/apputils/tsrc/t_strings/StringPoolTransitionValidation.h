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
// The definitions of the transition classes upon the RStringPool class methods.
// 
//

#ifndef __STRINGPOOLTRANSITIONVALIDATION_H__
#define __STRINGPOOLTRANSITIONVALIDATION_H__

#include "StringPoolTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the TrueUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the RStringPool test class for a transition.
 */
class TStringPool_True_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TStringPool_True_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave		KErrNoMemory
		@since			6.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TStringPool_True_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			6.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TStringPool_True_TransitionValidator is fully constructed.
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
		@since			6.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TStringPool_True_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TStringPool_True_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the FalseUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the RStringPool test class for a transition.
 */
class TStringPool_False_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TStringPool_False_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave		KErrNoMemory
		@since			6.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TStringPool_False_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			6.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TStringPool_False_TransitionValidator is fully constructed.
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
		@since			6.0
		@param			aAsyncState EAsyncCalled if the async function has been just been called,
						EAsyncCompleted if the function has completed.
		@return			TBool ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TStringPool_False_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TStringPool_False_TransitionValidator

#include "StringPoolTransitionValidation.inl"

#endif // __STRINGPOOLTRANSITIONVALIDATION_H__

