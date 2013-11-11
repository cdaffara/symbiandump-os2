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
// The validation classes for the CDiscoverer transitions.
// 
//

#ifndef __DISCOVERERTRANSITIONVALIDATION_H__
#define __DISCOVERERTRANSITIONVALIDATION_H__

#include "DiscovererTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CreateAndDestroy Unit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.

 */
class TDiscoverer_Ctor_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_Ctor_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	: Standard constructor.
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_Ctor_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_Default_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_Default_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_Ctor_TransitionValidator


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CreateAndDestroy Unit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.

 */
class TDiscoverer_Dtor_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_Dtor_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_Dtor_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_Default_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_Default_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_Dtor_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the MountDriveLUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.

 */
class TDiscoverer_MountDriveL_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_MountDriveL_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_MountDriveL_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_MountDriveL_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_MountDriveL_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_MountDriveL_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the UnmountDriveUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.
 */
class TDiscoverer_UnmountDrive_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_UnmountDrive_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_UnmountDrive_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_UnmountDrive_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_UnmountDrive_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_UnmountDrive_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the IsValidEntryUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.
 */
class TDiscoverer_IsValidEntry_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_IsValidEntry_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_IsValidEntry_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_IsValidEntry_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_IsValidEntry_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_IsValidEntry_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the ProcessEntryLUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.
 */
class TDiscoverer_ProcessEntryL_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_ProcessEntryL_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_ProcessEntryL_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_ProcessEntryL_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_ProcessEntryL_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_ProcessEntryL_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CompleteNotificationProcessingUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.
 */
class TDiscoverer_CompleteNotificationProcessing_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_CompleteNotificationProcessing_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_CompleteNotificationProcessing_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_CompleteNotificationProcessing_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_CompleteNotificationProcessing_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_CompleteNotificationProcessing_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the NotificationUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.
 */
class TDiscoverer_Notification_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_Notification_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_Notification_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_Notification_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_Notification_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_Notification_TransitionValidator
// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the SuspendUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.
 */
class TDiscoverer_Suspend_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_Suspend_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_Suspend_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_Suspend_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_Suspend_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_Suspend_TransitionValidator


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the ResumeUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.
 */
class TDiscoverer_Resume_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_Resume_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_Resume_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_Resume_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_Resume_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_Resume_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the ScanDirectoryUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.
 */
class TDiscoverer_ScanDirectory_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_ScanDirectory_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_ScanDirectory_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_ScanDirectory_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_ScanDirectory_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_ScanDirectory_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the ScanDirectoryCancelUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.
 */
class TDiscoverer_ScanDirectoryCancel_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_ScanDirectoryCancel_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_ScanDirectoryCancel_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_ScanDirectoryCancel_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_ScanDirectoryCancel_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_ScanDirectoryCancel_TransitionValidator

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Provide all the ScanDirectoryIncrementUnit Test's specific
	validatation for the state of a transition before and after its execution.
	on the CDiscoverer test class for a transition.
 */
class TDiscoverer_ScanDirectoryIncrement_TransitionValidator : public TTransitionValidator
	{
public:
	/**
		@fn				TDiscoverer_ScanDirectoryIncrement_TransitionValidator(CUnitTestContext& aUTContext)
		Intended Usage	:	
		@leave			KErrNoMemory
		@since			7.0
		@param			aUTContext The context within which this transition is executing
	 */
	inline TDiscoverer_ScanDirectoryIncrement_TransitionValidator(CUnitTestContext& aUTContext);

	/**
		@fn				ValidatePreConditions()
		Intended Usage	: Implemented by the developer to check the
						end state of the transition behaviour.
		Error Condition	: Invalid pre-conditions
		@since			7.0
		@return			TBool ETrue if the pre-conditions were valid, EFalse otherwise.
		@pre 			TDiscoverer_ScanDirectoryIncrement_TransitionValidator is fully constructed.
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
		@pre 			TDiscoverer_ScanDirectoryIncrement_TransitionValidator is fully constructed.
		@post			No change to the iUTContext class.
	*/
	virtual inline TBool ValidatePostConditions(TTestBedAsyncState aAsyncState);
	};	// TDiscoverer_ScanDirectoryIncrement_TransitionValidator

#include "DiscovererTransitionValidation.inl"

#endif		// __DISCOVERERTRANSITIONVALIDATION_H__

