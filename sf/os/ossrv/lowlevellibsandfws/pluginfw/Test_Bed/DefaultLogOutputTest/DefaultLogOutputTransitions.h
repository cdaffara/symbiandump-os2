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
// The definitions of the transition classes upon the CDefaultLogOutput class methods.
// 
//

#ifndef __DEFAULTLOGOUTPUTTRANSITIONS_H__
#define __DEFAULTLOGOUTPUTTRANSITIONS_H__

#include "TestUtilities.h"
#include "StateAccessor.h"
#include "LeakTestTransition.h"
#include <ecom/test_bed/datalogger.h>
#include "DefaultLogOutput.h"
#include "DefaultLogOutputStateAccessor.h"
#include <ecom/test_bed/unittest.h>

#define CTransitionType CLeakTestTransition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Provide all the CDefaultLogOutput specific
	parameters and behaviour on the CDefaultLogOutput
	test class for a transition.
 */
class CDefaultLogOutput_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CDefaultLogOutput_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the CDefaultLogOutput class under test.
		@pre 			None.
		@post			CDefaultLogOutput_UnitTestContext is fully constructed, and initialised.
	*/
	inline CDefaultLogOutput_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CDefaultLogOutput_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			7.0
		@pre 			CDefaultLogOutput_UnitTestContext is fully constructed.
		@post			CDefaultLogOutput_UnitTestContext is fully destroyed
		*/
		virtual inline ~CDefaultLogOutput_UnitTestContext();

	/** The instance of the class under test */
	CDefaultLogOutput* iDefaultLogOutput;
	/** The text to output to the test log file */ 
	const TDesC* iOutput;
	}; // CDefaultLogOutput_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Transition test of the CDefaultLogOutput::NewL method.
 */
class CDefaultLogOutput_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDefaultLogOutput_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDefaultLogOutput_NewL_Transition is fully constructed.
	*/
	CDefaultLogOutput_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDefaultLogOutput::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDefaultLogOutput::NewL)
		@since			7.0
		@return			None
		@pre 			CDefaultLogOutput_NewL_Transition is fully constructed.
		@post			No change in the CDefaultLogOutput_NewL_Transition apart
						from iDefaultLogOutput, which may have changed state.
						(@see CDefaultLogOutput::NewL post-condition) for iDefaultLogOutput's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDefaultLogOutput_UnitTestContext
		@pre 			iUTContext is a valid CDefaultLogOutput_UnitTestContext.
		@post			No change in the CDefaultLogOutput_NewL_Transition
	*/
	inline CDefaultLogOutput_UnitTestContext& Context() const;
	};	// CDefaultLogOutput_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Transition test of the CDefaultLogOutput::Dtor method.
 */
class CDefaultLogOutput_Dtor_Transition : public CTransition
	{
public:
	/**
		@fn				CDefaultLogOutput_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDefaultLogOutput_Dtor_Transition is fully constructed.
	*/
	CDefaultLogOutput_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDefaultLogOutput::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDefaultLogOutput::Dtor)
		@since			7.0
		@return			None
		@pre 			CDefaultLogOutput_Dtor_Transition is fully constructed.
		@post			No change in the CDefaultLogOutput_Dtor_Transition apart
						from iDefaultLogOutput, which may have changed state.
						(@see CDefaultLogOutput::Dtor post-condition) for iDefaultLogOutput's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDefaultLogOutput_UnitTestContext
		@pre 			iUTContext is a valid CDefaultLogOutput_UnitTestContext.
		@post			No change in the CDefaultLogOutput_Dtor_Transition
	*/
	inline CDefaultLogOutput_UnitTestContext& Context() const;
	};	// CDefaultLogOutput_Dtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Transition test of the CDefaultLogOutput::OpenL method.
 */
class CDefaultLogOutput_OpenL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CDefaultLogOutput_OpenL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDefaultLogOutput_OpenL_Transition is fully constructed.
	*/
	CDefaultLogOutput_OpenL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDefaultLogOutput::OpenL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDefaultLogOutput::OpenL)
		@since			7.0
		@return			None
		@pre 			CDefaultLogOutput_OpenL_Transition is fully constructed.
		@post			No change in the CDefaultLogOutput_OpenL_Transition apart
						from iDefaultLogOutput, which may have changed state.
						(@see CDefaultLogOutput::OpenL post-condition) for iDefaultLogOutput's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDefaultLogOutput_UnitTestContext
		@pre 			iUTContext is a valid CDefaultLogOutput_UnitTestContext.
		@post			No change in the CDefaultLogOutput_OpenL_Transition
	*/
	inline CDefaultLogOutput_UnitTestContext& Context() const;
	};	// CDefaultLogOutput_OpenL_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CDefaultLogOutput::Close method.
 */
class CDefaultLogOutput_Close_Transition : public CTransition
	{
public:
	/**
		@fn				CDefaultLogOutput_Close_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDefaultLogOutput_Close_Transition is fully constructed.
	*/
	CDefaultLogOutput_Close_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDefaultLogOutput::Close method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDefaultLogOutput::Close)
		@since			7.0
		@return			None
		@pre 			CDefaultLogOutput_Close_Transition is fully constructed.
		@post			No change in the CDefaultLogOutput_Close_Transition apart
						from iDefaultLogOutput, which may have changed state.
						(@see CDefaultLogOutput::Close post-condition) for iDefaultLogOutput's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDefaultLogOutput_UnitTestContext
		@pre 			iUTContext is a valid CDefaultLogOutput_UnitTestContext.
		@post			No change in the CDefaultLogOutput_Close_Transition
	*/
	inline CDefaultLogOutput_UnitTestContext& Context() const;
	};	// CDefaultLogOutput_Close_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CDefaultLogOutput::Write method.
 */
class CDefaultLogOutput_Write_Transition : public CTransition
	{
public:
	/**
		@fn				CDefaultLogOutput_Write_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CDefaultLogOutput_Write_Transition is fully constructed.
	*/
	CDefaultLogOutput_Write_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CDefaultLogOutput::Write method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, (@see CDefaultLogOutput::Write)
		@since			7.0
		@return			None
		@pre 			CDefaultLogOutput_Write_Transition is fully constructed.
		@post			No change in the CDefaultLogOutput_Write_Transition apart
						from iDefaultLogOutput, which may have changed state.
						(@see CDefaultLogOutput::Write post-condition) for iDefaultLogOutput's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CDefaultLogOutput_UnitTestContext
		@pre 			iUTContext is a valid CDefaultLogOutput_UnitTestContext.
		@post			No change in the CDefaultLogOutput_Write_Transition
	*/
	inline CDefaultLogOutput_UnitTestContext& Context() const;
	};	// CDefaultLogOutput_Write_Transition

#include "DefaultLogOutputTransitions.inl"

#include "DefaultLogOutputUnitTestContext.inl"

#endif // __DEFAULTLOGOUTPUTTRANSITIONS_H__

