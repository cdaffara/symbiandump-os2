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
// The definitions of the transition classes upon the CUnitTest class methods.
// 
//

#ifndef __UNITTESTTRANSITIONS_H__
#define __UNITTESTTRANSITIONS_H__

#include "TestUtilities.h"
#include "StateAccessor.h"
#include "LeakTestTransition.h"
#include <ecom/test_bed/datalogger.h>
#include <ecom/test_bed/unittest.h>
#include "UnitTestStateAccessor.h"
#include <ecom/test_bed/unittest.h>

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CUnitTest specific
	parameters and behaviour on the CUnitTest
	test class for a transition.
 */
class CUnitTest_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CUnitTest_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CUnitTest class under test.
		@pre 			None.
		@post			CUnitTest_UnitTestContext is fully constructed, and initialised.
	*/
	inline CUnitTest_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CUnitTest_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CUnitTest_UnitTestContext is fully constructed.
		@post			CUnitTest_UnitTestContext is fully destroyed
		*/
		virtual inline ~CUnitTest_UnitTestContext();

	
	CUnitTest* iUnitTest;
//	CUnitTestTestDerivation* iUnitTestDerived;
	const TDesC* iName;
	CDataLogger* iDataLogger;
	MUnitTestObserver* iUnitTestObserver;
	TTimeIntervalMicroSeconds32 iTimeAfter;
	CTransition* iTransition;
	TAny* iParams;
	CUnitTestTestDerivation* iUnitTestUnderTest;
	CTransition* iCurrentTransition;
	TInt iLeaveErrorCode;

	}; // CUnitTest_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CUnitTest::NewL method.
 */
class CUnitTest_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CUnitTest_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_NewL_Transition is fully constructed.
	*/
	CUnitTest_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::NewL)
		@since			7.0
		@return			None
		@pre 			CUnitTest_NewL_Transition is fully constructed.
		@post			No change in the CUnitTest_NewL_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::NewL post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_NewL_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CUnitTest::Dtor method.
 */
class CUnitTest_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CUnitTest_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_Dtor_Transition is fully constructed.
	*/
	CUnitTest_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::Dtor)
		@since			7.0
		@return			None
		@pre 			CUnitTest_Dtor_Transition is fully constructed.
		@post			No change in the CUnitTest_Dtor_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::Dtor post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_Dtor_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CUnitTest::TransitionSetL method.
 */
class CUnitTest_TransitionSetL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CUnitTest_TransitionSetL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_TransitionSetL_Transition is fully constructed.
	*/
	CUnitTest_TransitionSetL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::TransitionSetL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::TransitionSetL)
		@since			7.0
		@return			None
		@pre 			CUnitTest_TransitionSetL_Transition is fully constructed.
		@post			No change in the CUnitTest_TransitionSetL_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::TransitionSetL post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_TransitionSetL_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_TransitionSetL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CUnitTest::GetCurrentTransition method.
 */
class CUnitTest_GetCurrentTransition_Transition : public CTransition
	{
public:
	/**
		@fn				CUnitTest_GetCurrentTransition_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_GetCurrentTransition_Transition is fully constructed.
	*/
	CUnitTest_GetCurrentTransition_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::GetCurrentTransition method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::GetCurrentTransition)
		@since			7.0
		@return			None
		@pre 			CUnitTest_GetCurrentTransition_Transition is fully constructed.
		@post			No change in the CUnitTest_GetCurrentTransition_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::GetCurrentTransition post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_GetCurrentTransition_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_GetCurrentTransition_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Transition test of the CUnitTest::SetCurrentTransition method.
 */
class CUnitTest_SetCurrentTransition_Transition : public CTransition
	{
public:
	/**
		@fn				CUnitTest_SetCurrentTransition_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_SetCurrentTransition_Transition is fully constructed.
	*/
	CUnitTest_SetCurrentTransition_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::SetCurrentTransition method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::SetCurrentTransition)
		@since			7.0
		@return			None
		@pre 			CUnitTest_SetCurrentTransition_Transition is fully constructed.
		@post			No change in the CUnitTest_SetCurrentTransition_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::SetCurrentTransition post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_SetCurrentTransition_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_SetCurrentTransition_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CUnitTest::SetParametersL method.
 */
class CUnitTest_SetParametersL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CUnitTest_SetParametersL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_SetParametersL_Transition is fully constructed.
	*/
	CUnitTest_SetParametersL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::SetParametersL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::SetParametersL)
		@since			7.0
		@return			None
		@pre 			CUnitTest_SetParametersL_Transition is fully constructed.
		@post			No change in the CUnitTest_SetParametersL_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::SetParametersL post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_SetParametersL_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_SetParametersL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CUnitTest::PrepareUnitTestL method.
 */
class CUnitTest_PrepareUnitTestL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CUnitTest_PrepareUnitTestL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_PrepareUnitTestL_Transition is fully constructed.
	*/
	CUnitTest_PrepareUnitTestL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::PrepareUnitTestL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::PrepareUnitTestL)
		@since			7.0
		@return			None
		@pre 			CUnitTest_PrepareUnitTestL_Transition is fully constructed.
		@post			No change in the CUnitTest_PrepareUnitTestL_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::PrepareUnitTestL post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_PrepareUnitTestL_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_PrepareUnitTestL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CUnitTest::RunTest method.
 */
class CUnitTest_RunTest_Transition : public CTransition
	{
public:
	/**
		@fn				CUnitTest_RunTest_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_RunTest_Transition is fully constructed.
	*/
	CUnitTest_RunTest_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::RunTest method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::RunTest)
		@since			7.0
		@return			None
		@pre 			CUnitTest_RunTest_Transition is fully constructed.
		@post			No change in the CUnitTest_RunTest_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::RunTest post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_RunTest_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_RunTest_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CUnitTest::AddTransitionL method.
 */
class CUnitTest_AddTransitionL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CUnitTest_AddTransitionL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_AddTransitionL_Transition is fully constructed.
	*/
	CUnitTest_AddTransitionL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::AddTransitionL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::AddTransitionL)
		@since			7.0
		@return			None
		@pre 			CUnitTest_AddTransitionL_Transition is fully constructed.
		@post			No change in the CUnitTest_AddTransitionL_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::AddTransitionL post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_AddTransitionL_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_AddTransitionL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
 
	Comments : Transition test of the CUnitTest::AddBlockingTransitionL method.
 */
class CUnitTest_AddBlockingTransitionL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CUnitTest_AddBlockingTransitionL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_AddBlockingTransitionL_Transition is fully constructed.
	*/
	CUnitTest_AddBlockingTransitionL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::AddBlockingTransitionL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::AddBlockingTransitionL)
		@since			7.0
		@return			None
		@pre 			CUnitTest_AddBlockingTransitionL_Transition is fully constructed.
		@post			No change in the CUnitTest_AddBlockingTransitionL_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::AddBlockingTransitionL post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_AddBlockingTransitionL_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_AddBlockingTransitionL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CUnitTest::AddLeaveErrorCodeL method.
 */
class CUnitTest_AddLeaveErrorCodeL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CUnitTest_AddLeaveErrorCodeL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_AddLeaveErrorCodeL_Transition is fully constructed.
	*/
	CUnitTest_AddLeaveErrorCodeL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::AddLeaveErrorCodeL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::AddLeaveErrorCodeL)
		@since			7.0
		@return			None
		@pre 			CUnitTest_AddLeaveErrorCodeL_Transition is fully constructed.
		@post			No change in the CUnitTest_AddLeaveErrorCodeL_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::AddLeaveErrorCodeL post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_AddLeaveErrorCodeL_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_AddLeaveErrorCodeL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CUnitTest::UnitTestName method.
 */
class CUnitTest_UnitTestName_Transition : public CTransition
	{
public:
	/**
		@fn				CUnitTest_UnitTestName_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CUnitTest_UnitTestName_Transition is fully constructed.
	*/
	CUnitTest_UnitTestName_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CUnitTest::UnitTestName method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CUnitTest::UnitTestName)
		@since			7.0
		@return			None
		@pre 			CUnitTest_UnitTestName_Transition is fully constructed.
		@post			No change in the CUnitTest_UnitTestName_Transition apart
						from iUnitTest, which may have changed state.
						(@see CUnitTest::UnitTestName post-condition) for iUnitTest's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CUnitTest_UnitTestContext
		@pre 			iUTContext is a valid CUnitTest_UnitTestContext.
		@post			No change in the CUnitTest_UnitTestName_Transition
	*/
	inline CUnitTest_UnitTestContext& Context() const;
	};	// CUnitTest_UnitTestName_Transition


// Add additional Transition class definitions here...

#include "UnitTestTransitions.inl"

#include "UnitTestUnitTestContext.inl"

#endif // __UNITTESTTRANSITIONS_H__

