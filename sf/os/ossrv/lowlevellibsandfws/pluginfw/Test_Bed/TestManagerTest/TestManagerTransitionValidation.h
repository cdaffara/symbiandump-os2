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
// The definitions of the transition classes upon the CTestManager class methods.
// 
//

#ifndef __TESTMANAGERTRANSITIONVALIDATION_H__
#define __TESTMANAGERTRANSITIONVALIDATION_H__

#include "TestManagerTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CtorUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTestManager test class for a transition.
 */
class TTestManager_Ctor_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTestManager_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			6.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTestManager_Ctor_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			6.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTestManager_Ctor_TransitionValidator is fully constructed.
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
		@pre 			TTestManager_Ctor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTestManager_Ctor_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the DtorUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTestManager test class for a transition.
 */
class TTestManager_Dtor_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTestManager_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			6.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTestManager_Dtor_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			6.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTestManager_Dtor_TransitionValidator is fully constructed.
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
		@pre 			TTestManager_Dtor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTestManager_Dtor_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the RunTestsUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTestManager test class for a transition.
 */
class TTestManager_RunTests_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTestManager_RunTests_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			6.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTestManager_RunTests_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			6.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTestManager_RunTests_TransitionValidator is fully constructed.
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
		@pre 			TTestManager_RunTests_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTestManager_RunTests_TransitionValidator


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the TestComponentUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CTestManager test class for a transition.
 */
class TTestManager_TestComponent_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TTestManager_TestComponent_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			6.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TTestManager_TestComponent_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			6.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TTestManager_TestComponent_TransitionValidator is fully constructed.
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
		@pre 			TTestManager_TestComponent_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TTestManager_TestComponent_TransitionValidator


#include "TestManagerTransitionValidation.inl"

#endif // __TESTMANAGERTRANSITIONVALIDATION_H__

