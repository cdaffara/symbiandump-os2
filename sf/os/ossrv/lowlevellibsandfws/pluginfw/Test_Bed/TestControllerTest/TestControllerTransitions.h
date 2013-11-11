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
// The definitions of the transition classes upon the CTestController class methods.
// 
//

#ifndef __TESTCONTROLLERTRANSITIONS_H__
#define __TESTCONTROLLERTRANSITIONS_H__

#include "TestUtilities.h"
#include "StateAccessor.h"
#include "LeakTestTransition.h"
#include <ecom/test_bed/datalogger.h>
#include "TestController.h"
#include "TestControllerStateAccessor.h"
#include <ecom/test_bed/unittest.h>

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CTestController specific
	parameters and behaviour on the CTestController
	test class for a transition.
 */
class CTestController_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CTestController_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver,
														RFs& aFs)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CTestController class under test.
		@param			aFs A handle to the file server
		@pre 			None.
		@post			CTestController_UnitTestContext is fully constructed, and initialised.
	*/
	inline CTestController_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver,
										RFs& aFs);

	/**
		@fn				~CTestController_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CTestController_UnitTestContext is fully constructed.
		@post			CTestController_UnitTestContext is fully destroyed
		*/
		virtual inline ~CTestController_UnitTestContext();

	/** The instance of the class under test */
	CTestController* iTestController;
	/** The active scheduler for the TestController to use */
	CActiveScheduler* iScheduler;
	/** An array of tests to be run */
	RPointerArray<TTestInfo> iTests;
	/** The logging info for the TestController to use */
	TLoggingInfo* iLogInfo;
	/** File server handle */
	RFs iFs;

	}; // CTestController_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTestController::NewL method.
 */
class CTestController_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CTestController_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTestController_NewL_Transition is fully constructed.
	*/
	CTestController_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTestController::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTestController::NewL)
		@since			7.0
		@return			None
		@pre 			CTestController_NewL_Transition is fully constructed.
		@post			No change in the CTestController_NewL_Transition apart
						from iTestController, which may have changed state.
						(@see CTestController::NewL post-condition) for iTestController's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTestController_UnitTestContext
		@pre 			iUTContext is a valid CTestController_UnitTestContext.
		@post			No change in the CTestController_NewL_Transition
	*/
	inline CTestController_UnitTestContext& Context() const;
	};	// CTestController_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
 
	Comments : Transition test of the CTestController::Dtor method.
 */
class CTestController_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CTestController_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTestController_Dtor_Transition is fully constructed.
	*/
	CTestController_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTestController::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTestController::Dtor)
		@since			7.0
		@return			None
		@pre 			CTestController_Dtor_Transition is fully constructed.
		@post			No change in the CTestController_Dtor_Transition apart
						from iTestController, which may have changed state.
						(@see CTestController::Dtor post-condition) for iTestController's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTestController_UnitTestContext
		@pre 			iUTContext is a valid CTestController_UnitTestContext.
		@post			No change in the CTestController_Dtor_Transition
	*/
	inline CTestController_UnitTestContext& Context() const;
	};	// CTestController_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTestController::ScanDriveL method.
 */
class CTestController_ScanDriveL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CTestController_ScanDriveL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTestController_ScanDriveL_Transition is fully constructed.
	*/
	CTestController_ScanDriveL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTestController::ScanDriveL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTestController::ScanDriveL)
		@since			7.0
		@return			None
		@pre 			CTestController_ScanDriveL_Transition is fully constructed.
		@post			No change in the CTestController_ScanDriveL_Transition apart
						from iTestController, which may have changed state.
						(@see CTestController::ScanDriveL post-condition) for iTestController's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTestController_UnitTestContext
		@pre 			iUTContext is a valid CTestController_UnitTestContext.
		@post			No change in the CTestController_ScanDriveL_Transition
	*/
	inline CTestController_UnitTestContext& Context() const;
	};	// CTestController_ScanDriveL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTestController::DiscoverTestsL method.
 */
class CTestController_DiscoverTestsL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CTestController_DiscoverTestsL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTestController_DiscoverTestsL_Transition is fully constructed.
	*/
	CTestController_DiscoverTestsL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTestController::DiscoverTestsL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTestController::DiscoverTestsL)
		@since			7.0
		@return			None
		@pre 			CTestController_DiscoverTestsL_Transition is fully constructed.
		@post			No change in the CTestController_DiscoverTestsL_Transition apart
						from iTestController, which may have changed state.
						(@see CTestController::DiscoverTestsL post-condition) for iTestController's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTestController_UnitTestContext
		@pre 			iUTContext is a valid CTestController_UnitTestContext.
		@post			No change in the CTestController_DiscoverTestsL_Transition
	*/
	inline CTestController_UnitTestContext& Context() const;
	};	// CTestController_DiscoverTestsL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTestController::FindComponents method.
 */
class CTestController_FindComponents_Transition : public CTransition
	{
public:
	/**
		@fn				CTestController_FindComponents_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTestController_FindComponents_Transition is fully constructed.
	*/
	CTestController_FindComponents_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTestController::FindComponents method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTestController::FindComponents)
		@since			7.0
		@return			None
		@pre 			CTestController_FindComponents_Transition is fully constructed.
		@post			No change in the CTestController_FindComponents_Transition apart
						from iTestController, which may have changed state.
						(@see CTestController::FindComponents post-condition) for iTestController's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTestController_UnitTestContext
		@pre 			iUTContext is a valid CTestController_UnitTestContext.
		@post			No change in the CTestController_FindComponents_Transition
	*/
	inline CTestController_UnitTestContext& Context() const;
	};	// CTestController_FindComponents_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CTestController::Start method.
 */
class CTestController_Start_Transition : public CTransition
	{
public:
	/**
		@fn				CTestController_Start_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CTestController_Start_Transition is fully constructed.
	*/
	CTestController_Start_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CTestController::Start method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CTestController::StartL)
		@since			7.0
		@return			None
		@pre 			CTestController_Start_Transition is fully constructed.
		@post			No change in the CTestController_Start_Transition apart
						from iTestController, which may have changed state.
						(@see CTestController::Start post-condition) for iTestController's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CTestController_UnitTestContext
		@pre 			iUTContext is a valid CTestController_UnitTestContext.
		@post			No change in the CTestController_Start_Transition
	*/
	inline CTestController_UnitTestContext& Context() const;
	};	// CTestController_Start_Transition


// Add additional Transition class definitions here...

#include "TestControllerTransitions.inl"

#include "TestControllerUnitTestContext.inl"

#endif // __TESTCONTROLLERTRANSITIONS_H__

