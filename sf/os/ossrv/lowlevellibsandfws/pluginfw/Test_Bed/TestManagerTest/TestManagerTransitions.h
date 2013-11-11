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

#ifndef __TESTMANAGERTRANSITIONS_H__
#define __TESTMANAGERTRANSITIONS_H__

#include "TestUtilities.h"
#include "StateAccessor.h"
#include "LeakTestTransition.h"
#include <ecom/test_bed/datalogger.h>
#include "TestManager.h"
#include "TestManagerStateAccessor.h"
#include <ecom/test_bed/unittest.h>
#include "TestController.h"
#include "TestManagerObserverStub.h"

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CTestManager specific
	parameters and behaviour on the CTestManager
	test class for a transition.
 */
class CTestManager_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CTestManager_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CTestManager class under test.
		@pre 			None.
		@post			CTestManager_UnitTestContext is fully constructed, and initialised.
	*/
	inline CTestManager_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CTestManager_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			6.0
		@pre 			CTestManager_UnitTestContext is fully constructed.
		@post			CTestManager_UnitTestContext is fully destroyed
		*/
	virtual inline ~CTestManager_UnitTestContext();

	inline MManagerObserver& ManagerObserver();

	/** The instance of the class under test */
	CTestManager* iTestManager;
	/** A component info to be used by the TestManager */
	CComponentInfo* iComponentInfo;
	/** A reference to the observer stub of the CTestManager */
	CTestManagerObserverStub* iTestManagerObserverStub;
	/** A test controller to be used by the TestManager */
	CTestController* iTestController;
	/** A component tester to be used by the TestManager */
	CComponentTester* iTester;
	/** An array of all the tests avaialble to be run */
	RPointerArray<CComponentInfo>* iTestListArray;
	/** An array of all the available components */
	const RPointerArray<CComponentInfo>* iComponents;
	/** An array of all the tests that are to be run */
	RPointerArray<TTestInfo> iTests;
	/** The index of the component to test */
	TInt iComponentIndex;

	}; // CTestManager_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTestManager::NewL method.
 */
class CTestManager_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CTestManager_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTestManager_NewL_Transition is fully constructed.
	*/
	CTestManager_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTestManager::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTestManager::NewL)
		@since			6.0
		@return			None
		@pre 			CTestManager_NewL_Transition is fully constructed.
		@post			No change in the CTestManager_NewL_Transition apart
						from iTestManager, which may have changed state.
						(@see CTestManager::NewL post-condition) for iTestManager's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CTestManager_UnitTestContext
		@pre 			iUTContext is a valid CTestManager_UnitTestContext.
		@post			No change in the CTestManager_NewL_Transition
	*/
	inline CTestManager_UnitTestContext& Context() const;
	};	// CTestManager_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTestManager::Dtor method.
 */
class CTestManager_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CTestManager_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTestManager_Dtor_Transition is fully constructed.
	*/
	CTestManager_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTestManager::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTestManager::Dtor)
		@since			6.0
		@return			None
		@pre 			CTestManager_Dtor_Transition is fully constructed.
		@post			No change in the CTestManager_Dtor_Transition apart
						from iTestManager, which may have changed state.
						(@see CTestManager::Dtor post-condition) for iTestManager's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CTestManager_UnitTestContext
		@pre 			iUTContext is a valid CTestManager_UnitTestContext.
		@post			No change in the CTestManager_Dtor_Transition
	*/
	inline CTestManager_UnitTestContext& Context() const;
	};	// CTestManager_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTestManager::RunTests method.
 */
class CTestManager_RunTests_Transition : public CTransition
	{
public:
	/**
		@fn				CTestManager_RunTests_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTestManager_RunTests_Transition is fully constructed.
	*/
	CTestManager_RunTests_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTestManager::RunTests method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTestManager::RunTests)
		@since			6.0
		@return			None
		@pre 			CTestManager_RunTests_Transition is fully constructed.
		@post			No change in the CTestManager_RunTests_Transition apart
						from iTestManager, which may have changed state.
						(@see CTestManager::RunTests post-condition) for iTestManager's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CTestManager_UnitTestContext
		@pre 			iUTContext is a valid CTestManager_UnitTestContext.
		@post			No change in the CTestManager_RunTests_Transition
	*/
	inline CTestManager_UnitTestContext& Context() const;
	};	// CTestManager_RunTests_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTestManager::TestComponentL method.
 */
class CTestManager_TestComponentL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CTestManager_TestComponentL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTestManager_TestComponentL_Transition is fully constructed.
	*/
	CTestManager_TestComponentL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTestManager::TestComponentL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTestManager::TestComponentL)
		@since			6.0
		@return			None
		@pre 			CTestManager_TestComponentL_Transition is fully constructed.
		@post			No change in the CTestManager_TestComponentL_Transition apart
						from iTestManager, which may have changed state.
						(@see CTestManager::TestComponentL post-condition) for iTestManager's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CTestManager_UnitTestContext
		@pre 			iUTContext is a valid CTestManager_UnitTestContext.
		@post			No change in the CTestManager_TestComponentL_Transition
	*/
	inline CTestManager_UnitTestContext& Context() const;
	};	// CTestManager_TestComponentL_Transition


// Add additional Transition class definitions here...

#include "TestManagerTransitions.inl"

#include "TestManagerUnitTestContext.inl"

#endif // __TESTMANAGERTRANSITIONS_H__

