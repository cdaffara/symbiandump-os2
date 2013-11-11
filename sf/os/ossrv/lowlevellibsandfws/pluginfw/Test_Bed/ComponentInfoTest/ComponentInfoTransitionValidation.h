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
// The definitions of the transition classes upon the CComponentInfo class methods.
// 
//

#ifndef __COMPONENTINFOTRANSITIONVALIDATION_H__
#define __COMPONENTINFOTRANSITIONVALIDATION_H__

#include "ComponentInfoTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CtorUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CComponentInfo test class for a transition.
 */
class TComponentInfo_Ctor_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TComponentInfo_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	: Default constructor	
		@leave        	KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TComponentInfo_Ctor_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Checks the start state of the ComponentInfo transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TComponentInfo_Ctor_TransitionValidator is fully constructed.
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
		@return			ETrue if the post-conditions were valid, EFalse otherwise.
		@pre 			TComponentInfo_Ctor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TComponentInfo_Ctor_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the DtorUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CComponentInfo test class for a transition.
 */
class TComponentInfo_Dtor_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TComponentInfo_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TComponentInfo_Dtor_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TComponentInfo_Dtor_TransitionValidator is fully constructed.
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
		@pre 			TComponentInfo_Dtor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TComponentInfo_Dtor_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the EntryUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CComponentInfo test class for a transition.
 */
class TComponentInfo_Entry_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TComponentInfo_Entry_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TComponentInfo_Entry_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TComponentInfo_Entry_TransitionValidator is fully constructed.
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
		@pre 			TComponentInfo_Entry_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TComponentInfo_Entry_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the UnitTestsInfoUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CComponentInfo test class for a transition.
 */
class TComponentInfo_UnitTestsInfo_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TComponentInfo_UnitTestsInfo_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TComponentInfo_UnitTestsInfo_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TComponentInfo_UnitTestsInfo_TransitionValidator is fully constructed.
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
		@pre 			TComponentInfo_UnitTestsInfo_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TComponentInfo_UnitTestsInfo_TransitionValidator



#include "ComponentInfoTransitionValidation.inl"

#endif // __COMPONENTINFOTRANSITIONVALIDATION_H__

