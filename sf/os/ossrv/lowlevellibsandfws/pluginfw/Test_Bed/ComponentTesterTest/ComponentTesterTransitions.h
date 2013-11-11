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
// The definitions of the transition classes upon the CComponentTester class methods.
// 
//

#ifndef __COMPONENTTESTERTRANSITIONS_H__
#define __COMPONENTTESTERTRANSITIONS_H__

#include "TestUtilities.h"
#include "StateAccessor.h"
#include "LeakTestTransition.h"
#include <ecom/test_bed/datalogger.h>
#include "ComponentTester.h"
#include "ComponentTesterStateAccessor.h"
#include <ecom/test_bed/unittest.h>

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CComponentTester specific
	parameters and behaviour on the CComponentTester
	test class for a transition.
 */
class CComponentTester_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CComponentTester_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CComponentTester class under test.
		@pre 			None.
		@post			CComponentTester_UnitTestContext is fully constructed, and initialised.
	*/
	inline CComponentTester_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CComponentTester_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CComponentTester_UnitTestContext is fully constructed.
		@post			CComponentTester_UnitTestContext is fully destroyed
		*/
		virtual inline ~CComponentTester_UnitTestContext();

	/** The instance of the class under test */
	CComponentTester* iComponentTester;
	/** The data logger for the ComponentTester to use */
	CDataLogger* iDataLogger;
	/** An observer to inform when the test has been completed */
	MComponentTestObserver* iObserver;
	/** The unit test for the ComponentTester to use */
	CUnitTest* iUnitTest;
	/** An array of the tests to be run by ComponentTester */
	RPointerArray<TTestInfo> iTests;
	/** A class derived from the class under test */
	CComponentTesterTestDerivation* iComponentUnderTest;
	}; // CComponentTester_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CComponentTester::NewL method.
 */
class CComponentTester_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CComponentTester_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentTester_NewL_Transition is fully constructed.
	*/
	CComponentTester_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentTester::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentTester::NewL)
		@since			7.0
		@return			None
		@pre 			CComponentTester_NewL_Transition is fully constructed.
		@post			No change in the CComponentTester_NewL_Transition apart
						from iComponentTester, which may have changed state.
						(@see CComponentTester::NewL post-condition) for iComponentTester's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentTester_UnitTestContext
		@pre 			iUTContext is a valid CComponentTester_UnitTestContext.
		@post			No change in the CComponentTester_NewL_Transition
	*/
	inline CComponentTester_UnitTestContext& Context() const;
	};	// CComponentTester_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CComponentTester::Dtor method.
 */
class CComponentTester_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CComponentTester_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentTester_Dtor_Transition is fully constructed.
	*/
	CComponentTester_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentTester::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentTester::Dtor)
		@since			7.0
		@return			None
		@pre 			CComponentTester_Dtor_Transition is fully constructed.
		@post			No change in the CComponentTester_Dtor_Transition apart
						from iComponentTester, which may have changed state.
						(@see CComponentTester::Dtor post-condition) for iComponentTester's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentTester_UnitTestContext
		@pre 			iUTContext is a valid CComponentTester_UnitTestContext.
		@post			No change in the CComponentTester_Dtor_Transition
	*/
	inline CComponentTester_UnitTestContext& Context() const;
	};	// CComponentTester_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CComponentTester::TransitionSetsL method.
 */
class CComponentTester_TransitionSetsL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CComponentTester_TransitionSetsL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentTester_TransitionSetsL_Transition is fully constructed.
	*/
	CComponentTester_TransitionSetsL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentTester::TransitionSetsL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentTester::TransitionSetsL)
		@since			7.0
		@return			None
		@pre 			CComponentTester_TransitionSetsL_Transition is fully constructed.
		@post			No change in the CComponentTester_TransitionSetsL_Transition apart
						from iComponentTester, which may have changed state.
						(@see CComponentTester::TransitionSetsL post-condition) for iComponentTester's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentTester_UnitTestContext
		@pre 			iUTContext is a valid CComponentTester_UnitTestContext.
		@post			No change in the CComponentTester_TransitionSetsL_Transition
	*/
	inline CComponentTester_UnitTestContext& Context() const;
	};	// CComponentTester_TransitionSetsL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CComponentTester::AddUnitTestL method.
 */
class CComponentTester_AddUnitTestL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CComponentTester_AddUnitTestL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentTester_AddUnitTestL_Transition is fully constructed.
	*/
	CComponentTester_AddUnitTestL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentTester::AddUnitTestL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentTester::AddUnitTestL)
		@since			7.0
		@return			None
		@pre 			CComponentTester_AddUnitTestL_Transition is fully constructed.
		@post			No change in the CComponentTester_AddUnitTestL_Transition apart
						from iComponentTester, which may have changed state.
						(@see CComponentTester::AddUnitTestL post-condition) for iComponentTester's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentTester_UnitTestContext
		@pre 			iUTContext is a valid CComponentTester_UnitTestContext.
		@post			No change in the CComponentTester_AddUnitTestL_Transition
	*/
	inline CComponentTester_UnitTestContext& Context() const;
	};	// CComponentTester_AddUnitTestL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CComponentTester::AddParamUnitTestL method.
 */
class CComponentTester_AddParamUnitTestL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CComponentTester_AddParamUnitTestL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentTester_AddParamUnitTestL_Transition is fully constructed.
	*/
	CComponentTester_AddParamUnitTestL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentTester::AddParamUnitTestL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentTester::AddParamUnitTestL)
		@since			7.0
		@return			None
		@pre 			CComponentTester_AddParamUnitTestL_Transition is fully constructed.
		@post			No change in the CComponentTester_AddParamUnitTestL_Transition apart
						from iComponentTester, which may have changed state.
						(@see CComponentTester::AddParamUnitTestL post-condition) for iComponentTester's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentTester_UnitTestContext
		@pre 			iUTContext is a valid CComponentTester_UnitTestContext.
		@post			No change in the CComponentTester_AddParamUnitTestL_Transition
	*/
	inline CComponentTester_UnitTestContext& Context() const;
	};	// CComponentTester_AddParamUnitTestL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Transition test of the CComponentTester::TestComponent method.
 */
class CComponentTester_TestComponent_Transition : public CTransition
	{
public:
	/**
		@fn				CComponentTester_TestComponent_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentTester_TestComponent_Transition is fully constructed.
	*/
	CComponentTester_TestComponent_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentTester::TestComponent method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentTester::TestComponent)
		@since			7.0
		@return			None
		@pre 			CComponentTester_TestComponent_Transition is fully constructed.
		@post			No change in the CComponentTester_TestComponent_Transition apart
						from iComponentTester, which may have changed state.
						(@see CComponentTester::TestComponent post-condition) for iComponentTester's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentTester_UnitTestContext
		@pre 			iUTContext is a valid CComponentTester_UnitTestContext.
		@post			No change in the CComponentTester_TestComponent_Transition
	*/
	inline CComponentTester_UnitTestContext& Context() const;
	};	// CComponentTester_TestComponent_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CComponentTester::RunL method.
 */
class CComponentTester_RunL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CComponentTester_RunL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentTester_RunL_Transition is fully constructed.
	*/
	CComponentTester_RunL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentTester::RunL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentTester::RunL)
		@since			7.0
		@return			None
		@pre 			CComponentTester_RunL_Transition is fully constructed.
		@post			No change in the CComponentTester_RunL_Transition apart
						from iComponentTester, which may have changed state.
						(@see CComponentTester::RunL post-condition) for iComponentTester's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentTester_UnitTestContext
		@pre 			iUTContext is a valid CComponentTester_UnitTestContext.
		@post			No change in the CComponentTester_RunL_Transition
	*/
	inline CComponentTester_UnitTestContext& Context() const;
	};	// CComponentTester_RunL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CComponentTester::Complete method.
 */
class CComponentTester_Complete_Transition : public CTransition
	{
public:
	/**
		@fn				CComponentTester_Complete_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentTester_Complete_Transition is fully constructed.
	*/
	CComponentTester_Complete_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentTester::Complete method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentTester::Complete)
		@since			7.0
		@return			None
		@pre 			CComponentTester_Complete_Transition is fully constructed.
		@post			No change in the CComponentTester_Complete_Transition apart
						from iComponentTester, which may have changed state.
						(@see CComponentTester::Complete post-condition) for iComponentTester's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentTester_UnitTestContext
		@pre 			iUTContext is a valid CComponentTester_UnitTestContext.
		@post			No change in the CComponentTester_Complete_Transition
	*/
	inline CComponentTester_UnitTestContext& Context() const;
	};	// CComponentTester_Complete_Transition


// Add additional Transition class definitions here...

#include "ComponentTesterTransitions.inl"

#include "ComponentTesterUnitTestContext.inl"

#endif // __COMPONENTTESTERTRANSITIONS_H__

