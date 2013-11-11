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
// The validation class definitions for the CRegistrar class.
// 
//

#ifndef __REGISTRARTRANSITIONVALIDATION_H__
#define __REGISTRARTRANSITIONVALIDATION_H__

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the CtorUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CRegistrar test class for a transition.
 */
class TRegistrar_Ctor_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TRegistrar_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TRegistrar_Ctor_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TRegistrar_Ctor_TransitionValidator is fully constructed.
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
		@pre 			TRegistrar_Ctor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TRegistrar_Ctor_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the DtorUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CRegistrar test class for a transition.
 */
class TRegistrar_Dtor_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TRegistrar_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TRegistrar_Dtor_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TRegistrar_Dtor_TransitionValidator is fully constructed.
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
		@pre 			TRegistrar_Dtor_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TRegistrar_Dtor_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the DefaultUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CRegistrar test class for a transition.
 */
class TRegistrar_Default_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TRegistrar_Default_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TRegistrar_Default_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TRegistrar_Default_TransitionValidator is fully constructed.
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
		@pre 			TRegistrar_Default_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TRegistrar_Default_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the RegisterDiscoveryLUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CRegistrar test class for a transition.
 */
class TRegistrar_RegisterDiscoveryL_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TRegistrar_RegisterDiscoveryL_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TRegistrar_RegisterDiscoveryL_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TRegistrar_RegisterDiscoveryL_TransitionValidator is fully constructed.
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
		@pre 			TRegistrar_RegisterDiscoveryL_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TRegistrar_RegisterDiscoveryL_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the DriveRemoved Transition's validatation.
 */
class TRegistrar_DriveRemoved_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TRegistrar_DriveRemoved_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TRegistrar_DriveRemoved_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TRegistrar_DriveRemoved_TransitionValidator is fully constructed.
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
		@pre 			TRegistrar_DriveRemoved_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TRegistrar_DriveRemoved_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent 
	Comments : Provide all the DriveRemoved Transition's validatation.
 */
class TRegistrar_CheckRegWhileDiscovering_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TRegistrar_CheckRegWhileDiscovering_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TRegistrar_CheckRegWhileDiscovering_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TRegistrar_CheckRegWhileDiscovering_TransitionValidator is fully constructed.
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
		@pre 			TRegistrar_CheckRegWhileDiscovering_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TRegistrar_CheckRegWhileDiscovering_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent 
	Comments : Provide all the DiscoveriesComplete Transition's validatation.
 */
class TRegistrar_NewLHasContent_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TRegistrar_NewLHasContent_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave  		KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TRegistrar_NewLHasContent_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TRegistrar_NewLHasContent_TransitionValidator is fully constructed.
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
		@pre 			TRegistrar_NewLHasContent_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TRegistrar_NewLHasContent_TransitionValidator

#include "RegistrarTransitionValidation.inl"

#endif		// __REGISTRARTRANSITIONVALIDATION_H__
