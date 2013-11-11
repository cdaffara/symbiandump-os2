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

#ifndef __COMPONENTINFOTRANSITIONS_H__
#define __COMPONENTINFOTRANSITIONS_H__

#include "TestUtilities.h"
#include "StateAccessor.h"
#include "LeakTestTransition.h"
#include <ecom/test_bed/datalogger.h>
#include "ComponentInfo.h"
#include "ComponentInfoStateAccessor.h"
#include <ecom/test_bed/unittest.h>

// Global function typedef declaration of function provided by a test module of the component
// under test and is used as an entry point to kick start test bed. Provided as argument to CTestController.
typedef CComponentTester* (*ComponentTesterInitialiserLC)(CDataLogger&, MComponentTestObserver&);

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CComponentInfo specific
	parameters and behaviour on the CComponentInfo
	test class for a transition.
 */
class CComponentInfo_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CComponentInfo_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CComponentInfo class under test.
		@pre 			None.
		@post			CComponentInfo_UnitTestContext is fully constructed, and initialised.
	*/
	inline CComponentInfo_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CComponentInfo_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CComponentInfo_UnitTestContext is fully constructed.
		@post			CComponentInfo_UnitTestContext is fully destroyed
		*/
		virtual inline ~CComponentInfo_UnitTestContext();

	/** The instance of the class under test */
	CComponentInfo* iComponentInfo;
	/** The directory entry for a dll */
	ComponentTesterInitialiserLC iEntryFunc;
	/** A TEntry to check that the correct directory for the test component is found */
	ComponentTesterInitialiserLC iEntryFuncPostCheck;
	/** The array of unit tests for the component under test */
	RPointerArray<CUnitTestInfo>* iTestInfoArray;
	/** Used to check that the correct list of unit tests is found for a particular component */
	const RPointerArray<CUnitTestInfo>* iTestInfoArrayPostCheck;
	/** Flag indicating whether we have passed ownership of iEntry & iTestInfoArray
	to the CComponentInfo */
	TBool iOwnParams;
	}; // CComponentInfo_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CComponentInfo::NewL method.
 */
class CComponentInfo_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CComponentInfo_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentInfo_NewL_Transition is fully constructed.
	*/
	CComponentInfo_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentInfo::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentInfo::NewL)
		@since			7.0
		@return			None
		@pre 			CComponentInfo_NewL_Transition is fully constructed.
		@post			No change in the CComponentInfo_NewL_Transition apart
						from iComponentInfo, which may have changed state.
						(@see CComponentInfo::NewL post-condition) for iComponentInfo's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentInfo_UnitTestContext
		@pre 			iUTContext is a valid CComponentInfo_UnitTestContext.
		@post			No change in the CComponentInfo_NewL_Transition
	*/
	inline CComponentInfo_UnitTestContext& Context() const;
	};	// CComponentInfo_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CComponentInfo::Dtor method.
 */
class CComponentInfo_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CComponentInfo_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentInfo_Dtor_Transition is fully constructed.
	*/
	CComponentInfo_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentInfo::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentInfo::Dtor)
		@since			7.0
		@return			None
		@pre 			CComponentInfo_Dtor_Transition is fully constructed.
		@post			No change in the CComponentInfo_Dtor_Transition apart
						from iComponentInfo, which may have changed state.
						(@see CComponentInfo::Dtor post-condition) for iComponentInfo's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentInfo_UnitTestContext
		@pre 			iUTContext is a valid CComponentInfo_UnitTestContext.
		@post			No change in the CComponentInfo_Dtor_Transition
	*/
	inline CComponentInfo_UnitTestContext& Context() const;
	};	// CComponentInfo_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CComponentInfo::Entry method.
 */
class CComponentInfo_Entry_Transition : public CTransition
	{
public:
	/**
		@fn				CComponentInfo_Entry_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentInfo_Entry_Transition is fully constructed.
	*/
	CComponentInfo_Entry_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentInfo::Entry method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentInfo::Entry)
		@since			7.0
		@return			None
		@pre 			CComponentInfo_Entry_Transition is fully constructed.
		@post			No change in the CComponentInfo_Entry_Transition apart
						from iComponentInfo, which may have changed state.
						(@see CComponentInfo::Entry post-condition) for iComponentInfo's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentInfo_UnitTestContext
		@pre 			iUTContext is a valid CComponentInfo_UnitTestContext.
		@post			No change in the CComponentInfo_Entry_Transition
	*/
	inline CComponentInfo_UnitTestContext& Context() const;
	};	// CComponentInfo_Entry_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CComponentInfo::UnitTestsInfo method.
 */
class CComponentInfo_UnitTestsInfo_Transition : public CTransition
	{
public:
	/**
		@fn				CComponentInfo_UnitTestsInfo_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CComponentInfo_UnitTestsInfo_Transition is fully constructed.
	*/
	CComponentInfo_UnitTestsInfo_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CComponentInfo::UnitTestsInfo method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CComponentInfo::UnitTestsInfo)
		@since			7.0
		@return			None
		@pre 			CComponentInfo_UnitTestsInfo_Transition is fully constructed.
		@post			No change in the CComponentInfo_UnitTestsInfo_Transition apart
						from iComponentInfo, which may have changed state.
						(@see CComponentInfo::UnitTestsInfo post-condition) for iComponentInfo's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CComponentInfo_UnitTestContext
		@pre 			iUTContext is a valid CComponentInfo_UnitTestContext.
		@post			No change in the CComponentInfo_UnitTestsInfo_Transition
	*/
	inline CComponentInfo_UnitTestContext& Context() const;
	};	// CComponentInfo_UnitTestsInfo_Transition


// Add additional Transition class definitions here...

#include "ComponentInfoTransitions.inl"

#include "ComponentInfoUnitTestContext.inl"

#endif // __COMPONENTINFOTRANSITIONS_H__

