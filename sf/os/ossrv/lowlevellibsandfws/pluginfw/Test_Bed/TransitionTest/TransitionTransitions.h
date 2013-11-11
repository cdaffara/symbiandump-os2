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

#ifndef __TRANSITIONTRANSITIONS_H__
#define __TRANSITIONTRANSITIONS_H__

#include "TestUtilities.h"
#include "StateAccessor.h"
#include "LeakTestTransition.h"
#include <ecom/test_bed/datalogger.h>
#include <ecom/test_bed/transition.h>
#include "TransitionStateAccessor.h"
#include <ecom/test_bed/unittest.h>

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide a NULL context for the CTargetTransition
 */
class CTargetTransition_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CTargetTransition_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CTargetTransition class under test.
		@pre 			None.
		@post			CTargetTransition_UnitTestContext is fully constructed, and initialised.
	 */
	inline CTargetTransition_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CTargetTransition_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CTargetTransition_UnitTestContext is fully constructed.
		@post			CTargetTransition_UnitTestContext is fully destroyed
	 */
	virtual inline ~CTargetTransition_UnitTestContext();

	/** A leave code value for testing*/
	TInt iLeaveCode;
	}; // CTargetTransition_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTarget::NullMethod method.
 */
class CTarget_NullMethod_Transition : public CTransition
	{
public:
	/**
		@fn				CTarget_NullMethod_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTarget_NullMethod_Transition is fully constructed.
	*/
	CTarget_NullMethod_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTarget::NullMethod method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTarget::NullMethod)
		@since			7.0
		@return			None
		@pre 			CTarget_NullMethod_Transition is fully constructed.
		@post			No change in the CTarget_NullMethod_Transition apart
						from iTarget, which may have changed state.
						(@see CTarget::NullMethod post-condition) for iTarget's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTargetTransition_UnitTestContext
		@pre 			iUTContext is a valid CTargetTransition_UnitTestContext.
		@post			No change in the CTarget_NullMethod_Transition
	*/
	inline CTargetTransition_UnitTestContext& Context() const;
	};	// CTarget_NullMethod_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CTransition specific
	parameters and behaviour on the CTransition
	test class for a transition.
 */
class CTransition_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CTransition_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CTransition class under test.
		@pre 			None.
		@post			CTransition_UnitTestContext is fully constructed, and initialised.
	*/
	inline CTransition_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CTransition_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CTransition_UnitTestContext is fully constructed.
		@post			CTransition_UnitTestContext is fully destroyed
		*/
		virtual inline ~CTransition_UnitTestContext();

	/** The instance of the class under test */
	CTarget_NullMethod_Transition* iTargetTransition;
	/** The target transitions context */
	CUnitTestContext* iTargetContext;
	/** The target transitions validator */
	TTransitionValidator* iTargetValidator;
	/** The blocking flag for testing */
	TBool iBlockingTransition;
	/** The error code returned by the target for itsa last error */
	TInt iTargetErrorCode;
	/** A reference to the id of the transition under test */
	TDesC* iTransitionId;
	/** A reference to the info returned by the tarnsition about itself */
	TTransitionInfo* iTransitionInfo;
	}; // CTransition_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTransition::NewL method.
 */
class CTransition_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CTransition_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTransition_NewL_Transition is fully constructed.
	*/
	CTransition_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTransition::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTransition::NewL)
		@since			7.0
		@return			None
		@pre 			CTransition_NewL_Transition is fully constructed.
		@post			No change in the CTransition_NewL_Transition apart
						from iTransition, which may have changed state.
						(@see CTransition::NewL post-condition) for iTransition's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTransition_UnitTestContext
		@pre 			iUTContext is a valid CTransition_UnitTestContext.
		@post			No change in the CTransition_NewL_Transition
	*/
	inline CTransition_UnitTestContext& Context() const;
	};	// CTransition_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTransition::Dtor method.
 */
class CTransition_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CTransition_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTransition_Dtor_Transition is fully constructed.
	*/
	CTransition_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTransition::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTransition::Dtor)
		@since			7.0
		@return			None
		@pre 			CTransition_Dtor_Transition is fully constructed.
		@post			No change in the CTransition_Dtor_Transition apart
						from iTransition, which may have changed state.
						(@see CTransition::Dtor post-condition) for iTransition's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTransition_UnitTestContext
		@pre 			iUTContext is a valid CTransition_UnitTestContext.
		@post			No change in the CTransition_Dtor_Transition
	*/
	inline CTransition_UnitTestContext& Context() const;
	};	// CTransition_Dtor_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTransition::TransitMethodL method.
 */
class CTransition_TransitMethodL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CTransition_TransitMethodL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTransition_TransitMethodL_Transition is fully constructed.
	*/
	CTransition_TransitMethodL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTransition::TransitMethodL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTransition::TransitMethodL)
		@since			7.0
		@return			None
		@pre 			CTransition_TransitMethodL_Transition is fully constructed.
		@post			No change in the CTransition_TransitMethodL_Transition apart
						from iTransition, which may have changed state.
						(@see CTransition::TransitMethodL post-condition) for iTransition's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTransition_UnitTestContext
		@pre 			iUTContext is a valid CTransition_UnitTestContext.
		@post			No change in the CTransition_TransitMethodL_Transition
	*/
	inline CTransition_UnitTestContext& Context() const;
	};	// CTransition_TransitMethodL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTransition::RepeatOnce method.
 */
class CTransition_RepeatOnce_Transition : public CTransition
	{
public:
	/**
		@fn				CTransition_RepeatOnce_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTransition_RepeatOnce_Transition is fully constructed.
	*/
	CTransition_RepeatOnce_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTransition::RepeatOnce method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTransition::RepeatOnce)
		@since			7.0
		@return			None
		@pre 			CTransition_RepeatOnce_Transition is fully constructed.
		@post			No change in the CTransition_RepeatOnce_Transition apart
						from iTransition, which may have changed state.
						(@see CTransition::RepeatOnce post-condition) for iTransition's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTransition_UnitTestContext
		@pre 			iUTContext is a valid CTransition_UnitTestContext.
		@post			No change in the CTransition_RepeatOnce_Transition
	*/
	inline CTransition_UnitTestContext& Context() const;
	};	// CTransition_RepeatOnce_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTransition::TransitionId method.
 */
class CTransition_TransitionId_Transition : public CTransition
	{
public:
	/**
		@fn				CTransition_TransitionId_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTransition_TransitionId_Transition is fully constructed.
	*/
	CTransition_TransitionId_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTransition::TransitionId method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTransition::TransitionId)
		@since			7.0
		@return			None
		@pre 			CTransition_TransitionId_Transition is fully constructed.
		@post			No change in the CTransition_TransitionId_Transition apart
						from iTransition, which may have changed state.
						(@see CTransition::TransitionId post-condition) for iTransition's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTransition_UnitTestContext
		@pre 			iUTContext is a valid CTransition_UnitTestContext.
		@post			No change in the CTransition_TransitionId_Transition
	*/
	inline CTransition_UnitTestContext& Context() const;
	};	// CTransition_TransitionId_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTransition::TransitionInfo method.
 */
class CTransition_TransitionInfo_Transition : public CTransition
	{
public:
	/**
		@fn				CTransition_TransitionInfo_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTransition_TransitionInfo_Transition is fully constructed.
	*/
	CTransition_TransitionInfo_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTransition::TransitionInfo method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTransition::TransitionInfo)
		@since			7.0
		@return			None
		@pre 			CTransition_TransitionInfo_Transition is fully constructed.
		@post			No change in the CTransition_TransitionInfo_Transition apart
						from iTransition, which may have changed state.
						(@see CTransition::TransitionInfo post-condition) for iTransition's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTransition_UnitTestContext
		@pre 			iUTContext is a valid CTransition_UnitTestContext.
		@post			No change in the CTransition_TransitionInfo_Transition
	*/
	inline CTransition_UnitTestContext& Context() const;
	};	// CTransition_TransitionInfo_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTransition::SetBlocking method.
 */
class CTransition_SetBlocking_Transition : public CTransition
	{
public:
	/**
		@fn				CTransition_SetBlocking_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTransition_SetBlocking_Transition is fully constructed.
	*/
	CTransition_SetBlocking_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTransition::SetBlocking method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTransition::SetBlocking)
		@since			7.0
		@return			None
		@pre 			CTransition_SetBlocking_Transition is fully constructed.
		@post			No change in the CTransition_SetBlocking_Transition apart
						from iTransition, which may have changed state.
						(@see CTransition::SetBlocking post-condition) for iTransition's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTransition_UnitTestContext
		@pre 			iUTContext is a valid CTransition_UnitTestContext.
		@post			No change in the CTransition_SetBlocking_Transition
	*/
	inline CTransition_UnitTestContext& Context() const;
	};	// CTransition_SetBlocking_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTransition::IsBlocking method.
 */
class CTransition_IsBlocking_Transition : public CTransition
	{
public:
	/**
		@fn				CTransition_IsBlocking_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTransition_IsBlocking_Transition is fully constructed.
	*/
	CTransition_IsBlocking_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTransition::IsBlocking method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTransition::IsBlocking)
		@since			7.0
		@return			None
		@pre 			CTransition_IsBlocking_Transition is fully constructed.
		@post			No change in the CTransition_IsBlocking_Transition apart
						from iTransition, which may have changed state.
						(@see CTransition::IsBlocking post-condition) for iTransition's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTransition_UnitTestContext
		@pre 			iUTContext is a valid CTransition_UnitTestContext.
		@post			No change in the CTransition_IsBlocking_Transition
	*/
	inline CTransition_UnitTestContext& Context() const;
	};	// CTransition_IsBlocking_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTransition::GetErrorCode method.
 */
class CTransition_GetErrorCode_Transition : public CTransition
	{
public:
	/**
		@fn				CTransition_GetErrorCode_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTransition_GetErrorCode_Transition is fully constructed.
	*/
	CTransition_GetErrorCode_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTransition::GetErrorCode method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTransition::GetErrorCode)
		@since			7.0
		@return			None
		@pre 			CTransition_GetErrorCode_Transition is fully constructed.
		@post			No change in the CTransition_GetErrorCode_Transition apart
						from iTransition, which may have changed state.
						(@see CTransition::GetErrorCode post-condition) for iTransition's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTransition_UnitTestContext
		@pre 			iUTContext is a valid CTransition_UnitTestContext.
		@post			No change in the CTransition_GetErrorCode_Transition
	*/
	inline CTransition_UnitTestContext& Context() const;
	};	// CTransition_GetErrorCode_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTransition::RunTransition method.
 */
class CTransition_RunTransition_Transition : public CTransition
	{
public:
	/**
		@fn				CTransition_RunTransition_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTransition_RunTransition_Transition is fully constructed.
	*/
	CTransition_RunTransition_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTransition::RunTransition method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTransition::RunTransition)
		@since			7.0
		@return			None
		@pre 			CTransition_RunTransition_Transition is fully constructed.
		@post			No change in the CTransition_RunTransition_Transition apart
						from iTransition, which may have changed state.
						(@see CTransition::RunTransition post-condition) for iTransition's new state.
	*/
	inline void TransitMethodL();

	/**
	@fn				virtual void DoCancel()
	Intended Usage	:	Standard Active Object method for cancelling the current request
	@since			7.0
	@pre 			None
	@post			Any outstanding requests are cancelled
	*/
	virtual void DoCancel();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTransition_UnitTestContext
		@pre 			iUTContext is a valid CTransition_UnitTestContext.
		@post			No change in the CTransition_RunTransition_Transition
	*/
	inline CTransition_UnitTestContext& Context() const;
	};	// CTransition_RunTransition_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Transition test of the CTransition::Cancel method.
 */
class CTransition_Cancel_Transition : public CTransition
	{
public:
	/**
		@fn				CTransition_Cancel_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTransition_Cancel_Transition is fully constructed.
	*/
	CTransition_Cancel_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTransition::Cancel method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTransition::Cancel)
		@since			7.0
		@return			None
		@pre 			CTransition_Cancel_Transition is fully constructed.
		@post			No change in the CTransition_Cancel_Transition apart
						from iTransition, which may have changed state.
						(@see CTransition::Cancel post-condition) for iTransition's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTransition_UnitTestContext
		@pre 			iUTContext is a valid CTransition_UnitTestContext.
		@post			No change in the CTransition_Cancel_Transition
	*/
	inline CTransition_UnitTestContext& Context() const;
	};	// CTransition_Cancel_Transition

#include "TransitionTransitions.inl"

#include "TransitionUnitTestContext.inl"

#endif // __TRANSITIONTRANSITIONS_H__

