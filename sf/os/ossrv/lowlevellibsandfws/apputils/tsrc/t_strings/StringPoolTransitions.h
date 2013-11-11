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
// The definitions of the transition classes upon the RStringPool class methods.
// 
//

#ifndef __STRINGPOOLTRANSITIONS_H__
#define __STRINGPOOLTRANSITIONS_H__

#include <ecom/test_bed/testutilities.h>
#include <ecom/test_bed/stateaccessor.h>
#include <ecom/test_bed/leaktesttransition.h>
#include <ecom/test_bed/datalogger.h>
#include <stringpool.h>
#include "StringPoolStateAccessor.h"
#include <ecom/test_bed/unittest.h>

// use the typedef that has been commented  out if you don't want to perform  memory leak tests
typedef   CTransition CTransitionType;
//typedef  CLeakTestTransition CTransitionType;

class ExtObj : public MStringPoolCloseCallBack
	{
	public:
		void  StringPoolClosing() 
			{
			a= 1;
			}
		TBool a;
	};

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Provide all the RStringPool specific
	parameters and behaviour on the RStringPool
	test class for a transition.
 */
class CStringPool_UnitTestContext : public CUnitTestContext
	{
public:
	/**
		@fn				CStringPool_UnitTestContext(CDataLogger& aDataLogger,
														MStateAccessor& aStateAccessor,
														MTransitionObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest's Transitions.
		@param			aStateAccessor WhiteBox state access to the RStringPool class under test.
		@pre 			None.
		@post			CStringPool_UnitTestContext is fully constructed, and initialised.
	*/
	inline CStringPool_UnitTestContext(CDataLogger& aDataLogger,
										MStateAccessor& aStateAccessor,
										MTransitionObserver& aObserver);

	/**
		@fn				~CStringPool_UnitTestContext()
		Intended Usage	: Default Destructor
		@since			6.0
		@pre 			CStringPool_UnitTestContext is fully constructed.
		@post			CStringPool_UnitTestContext is fully destroyed
		*/
		virtual inline ~CStringPool_UnitTestContext();

	/** The instance of the class under test */
	RStringPool iStringPool;

	// Various strings. a and b are different strings. c is a temporary.
	// d is the same as a, but separately created
	// F denotes case insensitiveness; T denotes table strings
	RString iA, iB, iC, iD, iE, iF, iTE, iUndeletable_EX, iUndeletable_A;
	RStringF iFA, iFB, iFC, iFD, iFE, iFF, iTFE, iEmptyOne, iFAString, iFADuplicateString;

	TPtrC8 iADes, iBDes, iDDes, iFADes, iFBDes, iFDDes;
	TInt iInt, iFint,iNotFound;
	TStringTable iTestTable;
	TStringTable iTestTable2;
	TStringTable iTestTable3;
	TStringTable iTestTable4;
	ExtObj iMyExtObj,iMyExtObj2,iMyExtObj3;
	TInt cbCount;
	


	TInt iIndex1, iIndex2, iIndex3, iIndex4;

	// The result of boolean comparisons
	TBool iResult;
	}; // CStringPool_UnitTestContext

// ______________________________________________________________________________
//
/**
	@internalComponent			
	Comments : Transition test of the RStringPool::NewL method.
 */
class CStringPool_NewL_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_NewL_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_NewL_Transition is fully constructed.
	*/
	CStringPool_NewL_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the RStringPool::NewL method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see RStringPool::NewL)
		@since			6.0
		@return			None
		@pre 			CStringPool_NewL_Transition is fully constructed.
		@post			No change in the CStringPool_NewL_Transition apart
						from iStringPool, which may have changed state.
						(@see RStringPool::NewL post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_NewL_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_NewL_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent 
	Comments : Transition test of the RStringPool::Dtor method.
 */
class CStringPool_Dtor_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_Dtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_Dtor_Transition is fully constructed.
	*/
	CStringPool_Dtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the RStringPool::Dtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see RStringPool::Dtor)
		@since			6.0
		@return			None
		@pre 			CStringPool_Dtor_Transition is fully constructed.
		@post			No change in the CStringPool_Dtor_Transition apart
						from iStringPool, which may have changed state.
						(@see RStringPool::Dtor post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_Dtor_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_Dtor_Transition

// Add additional Transition class definitions here...


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the RStringPool::AeqA method.
 */
class CStringPool_AeqA_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRStringPool_AeqA_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRStringPool_AeqA_Transition is fully constructed.
	*/
	CStringPool_AeqA_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the RStringPool::AeqA method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see RStringPool::AeqA)
		@since			6.0
		@return			None
		@pre 			CRStringPool_AeqA_Transition is fully constructed.
		@post			No change in the CRStringPool_AeqA_Transition apart
						from iRStringPool, which may have changed state.
						(@see RStringPool::AeqA post-condition) for iRStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CRStringPool_UnitTestContext
		@pre 			iUTContext is a valid CRStringPool_UnitTestContext.
		@post			No change in the CRStringPool_AeqA_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CRStringPool_AeqA_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent	
	Comments : Transition test of the StringPool::AneA method.
 */
class CStringPool_AneA_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_AneA_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_AneA_Transition is fully constructed.
	*/
	CStringPool_AneA_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::AneA method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::AneA)
		@since			6.0
		@return			None
		@pre 			CStringPool_AneA_Transition is fully constructed.
		@post			No change in the CStringPool_AneA_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::AneA post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_AneA_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_AneA_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::AeqB method.
 */
class CStringPool_AeqB_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_AeqB_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_AeqB_Transition is fully constructed.
	*/
	CStringPool_AeqB_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::AeqB method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::AeqB)
		@since			6.0
		@return			None
		@pre 			CStringPool_AeqB_Transition is fully constructed.
		@post			No change in the CStringPool_AeqB_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::AeqB post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_AeqB_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_AeqB_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::AneB method.
 */
class CStringPool_AneB_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_AneB_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_AneB_Transition is fully constructed.
	*/
	CStringPool_AneB_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::AneB method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::AneB)
		@since			6.0
		@return			None
		@pre 			CStringPool_AneB_Transition is fully constructed.
		@post			No change in the CStringPool_AneB_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::AneB post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_AneB_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_AneB_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::AeqD method.
 */
class CStringPool_AeqD_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_AeqD_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_AeqD_Transition is fully constructed.
	*/
	CStringPool_AeqD_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::AeqD method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::AeqD)
		@since			6.0
		@return			None
		@pre 			CStringPool_AeqD_Transition is fully constructed.
		@post			No change in the CStringPool_AeqD_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::AeqD post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_AeqD_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_AeqD_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::AneD method.
 */
class CStringPool_AneD_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_AneD_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_AneD_Transition is fully constructed.
	*/
	CStringPool_AneD_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::AneD method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::AneD)
		@since			6.0
		@return			None
		@pre 			CStringPool_AneD_Transition is fully constructed.
		@post			No change in the CStringPool_AneD_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::AneD post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_AneD_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_AneD_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::Desc method.
 */
class CStringPool_Desc_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_Desc_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_Desc_Transition is fully constructed.
	*/
	CStringPool_Desc_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::Desc method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::Desc)
		@since			6.0
		@return			None
		@pre 			CStringPool_Desc_Transition is fully constructed.
		@post			No change in the CStringPool_Desc_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::Desc post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_Desc_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_Desc_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::Copy method.
 */
class CStringPool_Copy_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_Copy_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_Copy_Transition is fully constructed.
	*/
	CStringPool_Copy_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::Copy method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::Copy)
		@since			6.0
		@return			None
		@pre 			CStringPool_Copy_Transition is fully constructed.
		@post			No change in the CStringPool_Copy_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::Copy post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_Copy_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_Copy_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::CDesC method.
 */
class CStringPool_CDesC_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_CDesC_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_CDesC_Transition is fully constructed.
	*/
	CStringPool_CDesC_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::CDesC method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::CDesC)
		@since			6.0
		@return			None
		@pre 			CStringPool_CDesC_Transition is fully constructed.
		@post			No change in the CStringPool_CDesC_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::CDesC post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_CDesC_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_CDesC_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::Close method.
 */
class CStringPool_Close_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_Close_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_Close_Transition is fully constructed.
	*/
	CStringPool_Close_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::Close method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::Close)
		@since			6.0
		@return			None
		@pre 			CStringPool_Close_Transition is fully constructed.
		@post			No change in the CStringPool_Close_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::Close post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_Close_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_Close_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent	
	Comments : Transition test of the StringPool::Int method.
 */
class CStringPool_Int_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_Int_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_Int_Transition is fully constructed.
	*/
	CStringPool_Int_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::Int method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::Int)
		@since			6.0
		@return			None
		@pre 			CStringPool_Int_Transition is fully constructed.
		@post			No change in the CStringPool_Int_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::Int post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_Int_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_Int_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::StringToken method.
 */
class CStringPool_StringToken_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_StringToken_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_StringToken_Transition is fully constructed.
	*/
	CStringPool_StringToken_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::StringToken method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::StringToken)
		@since			6.0
		@return			None
		@pre 			CStringPool_StringToken_Transition is fully constructed.
		@post			No change in the CStringPool_StringToken_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::StringToken post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_StringToken_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_StringToken_Transition













































// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the RStringPool::AeqA method.
 */
class CStringPool_FAeqA_Transition : public CTransitionType
	{
public:
	/**
		@fn				CRStringPool_AeqA_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CRStringPool_AeqA_Transition is fully constructed.
	*/
	CStringPool_FAeqA_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the RStringPool::AeqA method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see RStringPool::AeqA)
		@since			6.0
		@return			None
		@pre 			CRStringPool_AeqA_Transition is fully constructed.
		@post			No change in the CRStringPool_AeqA_Transition apart
						from iRStringPool, which may have changed state.
						(@see RStringPool::AeqA post-condition) for iRStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CRStringPool_UnitTestContext
		@pre 			iUTContext is a valid CRStringPool_UnitTestContext.
		@post			No change in the CRStringPool_AeqA_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CRStringPool_AeqA_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::AneA method.
 */
class CStringPool_FAneA_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_FAneA_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_FAneA_Transition is fully constructed.
	*/
	CStringPool_FAneA_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::AneA method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::AneA)
		@since			6.0
		@return			None
		@pre 			CStringPool_FAneA_Transition is fully constructed.
		@post			No change in the CStringPool_FAneA_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::AneA post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_FAneA_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_FAneA_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::AeqB method.
 */
class CStringPool_FAeqB_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_FAeqB_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_FAeqB_Transition is fully constructed.
	*/
	CStringPool_FAeqB_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::AeqB method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::AeqB)
		@since			6.0
		@return			None
		@pre 			CStringPool_FAeqB_Transition is fully constructed.
		@post			No change in the CStringPool_FAeqB_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::AeqB post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_AeqB_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_FAeqB_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::AneB method.
 */
class CStringPool_FAneB_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_FAneB_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_FAneB_Transition is fully constructed.
	*/
	CStringPool_FAneB_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::AneB method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::AneB)
		@since			6.0
		@return			None
		@pre 			CStringPool_FAneB_Transition is fully constructed.
		@post			No change in the CStringPool_FAneB_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::AneB post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_AneB_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_FAneB_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent	
	Comments : Transition test of the StringPool::AeqD method.
 */
class CStringPool_FAeqD_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_FAeqD_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_FAeqD_Transition is fully constructed.
	*/
	CStringPool_FAeqD_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::AeqD method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::AeqD)
		@since			6.0
		@return			None
		@pre 			CStringPool_FAeqD_Transition is fully constructed.
		@post			No change in the CStringPool_FAeqD_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::AeqD post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_AeqD_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_FAeqD_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::AneD method.
 */
class CStringPool_FAneD_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_FAneD_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_FAneD_Transition is fully constructed.
	*/
	CStringPool_FAneD_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::AneD method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::AneD)
		@since			6.0
		@return			None
		@pre 			CStringPool_FAneD_Transition is fully constructed.
		@post			No change in the CStringPool_FAneD_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::AneD post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_AneD_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_FAneD_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::Desc method.
 */
class CStringPool_FDesc_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_FDesc_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_FDesc_Transition is fully constructed.
	*/
	CStringPool_FDesc_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::Desc method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::Desc)
		@since			6.0
		@return			None
		@pre 			CStringPool_FDesc_Transition is fully constructed.
		@post			No change in the CStringPool_FDesc_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::Desc post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_FUnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_Desc_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_FDesc_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent	
	Comments : Transition test of the StringPool::Copy method.
 */
class CStringPool_FCopy_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_FCopy_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_FCopy_Transition is fully constructed.
	*/
	CStringPool_FCopy_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::Copy method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::Copy)
		@since			6.0
		@return			None
		@pre 			CStringPool_FCopy_Transition is fully constructed.
		@post			No change in the CStringPool_FCopy_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::Copy post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_Copy_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_FCopy_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::CDesC method.
 */
class CStringPool_FCDesC_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_FCDesC_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_FCDesC_Transition is fully constructed.
	*/
	CStringPool_FCDesC_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::CDesC method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::CDesC)
		@since			6.0
		@return			None
		@pre 			CStringPool_FCDesC_Transition is fully constructed.
		@post			No change in the CStringPool_FCDesC_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::CDesC post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_CDesC_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_FCDesC_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::Close method.
 */
class CStringPool_FClose_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_FClose_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_FClose_Transition is fully constructed.
	*/
	CStringPool_FClose_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::Close method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::Close)
		@since			6.0
		@return			None
		@pre 			CStringPool_FClose_Transition is fully constructed.
		@post			No change in the CStringPool_FClose_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::Close post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_FUnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_Close_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_FClose_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::Int method.
 */
class CStringPool_FInt_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_FInt_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_FInt_Transition is fully constructed.
	*/
	CStringPool_FInt_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::Int method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::Int)
		@since			6.0
		@return			None
		@pre 			CStringPool_FInt_Transition is fully constructed.
		@post			No change in the CStringPool_FInt_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::Int post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_FUnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_Int_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_FInt_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::StringToken method.
 */
class CStringPool_FStringToken_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_FStringToken_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_FStringToken_Transition is fully constructed.
	*/
	CStringPool_FStringToken_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::StringToken method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::StringToken)
		@since			6.0
		@return			None
		@pre 			CStringPool_FStringToken_Transition is fully constructed.
		@post			No change in the CStringPool_FStringToken_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::StringToken post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_FUnitTestContext
		@pre 			iUTContext is a valid CStringPool_FUnitTestContext.
		@post			No change in the CStringPool_StringToken_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_FStringToken_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::ShortCtor method.
 */
class CStringPool_ShortCtor_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_ShortCtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_ShortCtor_Transition is fully constructed.
	*/
	CStringPool_ShortCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::SmallCtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::SmallCtor)
		@since			6.0
		@return			None
		@pre 			CStringPool_ShortCtor_Transition is fully constructed.
		@post			No change in the CStringPool_ShortCtor_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::SmallCtor post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_ShortCtor_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_ShortCtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::IrrelevantCtor method.
 */
class CStringPool_IrrelevantCtor_Transition : public CTransition
	{
public:
	/**
		@fn				CStringPool_IrrelevantCtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_IrrelevantCtor_Transition is fully constructed.
	*/
	CStringPool_IrrelevantCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::IrrelevantCtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::IrrelevantCtor)
		@since			6.0
		@return			None
		@pre 			CStringPool_IrrelevantCtor_Transition is fully constructed.
		@post			No change in the CStringPool_IrrelevantCtor_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::IrrelevantCtor post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_IrrelevantCtor_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_IrrelevantCtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Transition test of the StringPool::LongCtor method.
 */
class CStringPool_LongCtor_Transition : public CTransition
	{
public:
	/**
		@fn				CStringPool_LongCtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			6.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_LongCtor_Transition is fully constructed.
	*/
	CStringPool_LongCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::LongCtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::LongCtor)
		@since			6.0
		@return			None
		@pre 			CStringPool_LongCtor_Transition is fully constructed.
		@post			No change in the CStringPool_LongCtor_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::LongCtor post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			6.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_LongCtor_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_LongCtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::StringF_Index_Table method.
 */
class CStringPool_StringF_Index_Table_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_StringF_Index_Table_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_StringF_Index_Table_Transition is fully constructed.
	*/
	CStringPool_StringF_Index_Table_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::StringF_Index_Table method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::StringF_Index_Table)
		@since			7.0
		@return			None
		@pre 			CStringPool_StringF_Index_Table_Transition is fully constructed.
		@post			No change in the CStringPool_StringF_Index_Table_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::StringF_Index_Table post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_StringF_Index_Table_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_StringF_Index_Table_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::shortCSCtor method.
 */
class CStringPool_ShortCSCtor_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_shortCSCtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_shortCSCtor_Transition is fully constructed.
	*/
	CStringPool_ShortCSCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::shortCSCtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::shortCSCtor)
		@since			7.0
		@return			None
		@pre 			CStringPool_shortCSCtor_Transition is fully constructed.
		@post			No change in the CStringPool_shortCSCtor_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::shortCSCtor post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_shortCSCtor_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_ShortCSCtor_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::String_Index_Table method.
 */
class CStringPool_String_Index_Table_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_String_Index_Table_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_String_Index_Table_Transition is fully constructed.
	*/
	CStringPool_String_Index_Table_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::String_Index_Table method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::String_Index_Table)
		@since			7.0
		@return			None
		@pre 			CStringPool_String_Index_Table_Transition is fully constructed.
		@post			No change in the CStringPool_String_Index_Table_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::String_Index_Table post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_String_Index_Table_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_String_Index_Table_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CStringPool::IrrelevantCSCtor method.
 */
class CStringPool_IrrelevantCSCtor_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_IrrelevantCSCtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_IrrelevantCSCtor_Transition is fully constructed.
	*/
	CStringPool_IrrelevantCSCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CStringPool::IrrelevantCSCtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see CStringPool::IrrelevantCSCtor)
		@since			7.0
		@return			None
		@pre 			CStringPool_IrrelevantCSCtor_Transition is fully constructed.
		@post			No change in the CStringPool_IrrelevantCSCtor_Transition apart
						from iStringPool, which may have changed state.
						(@see CStringPool::IrrelevantCSCtor post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_IrrelevantCSCtor_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_IrrelevantCSCtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CStringPool::LargeCSCtor method.
 */
class CStringPool_LargeCSCtor_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_LargeCSCtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_LargeCSCtor_Transition is fully constructed.
	*/
	CStringPool_LargeCSCtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CStringPool::LargeCSCtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see CStringPool::LargeCSCtor)
		@since			7.0
		@return			None
		@pre 			CStringPool_LargeCSCtor_Transition is fully constructed.
		@post			No change in the CStringPool_LargeCSCtor_Transition apart
						from iStringPool, which may have changed state.
						(@see CStringPool::LargeCSCtor post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_LargeCSCtor_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_LargeCSCtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CStringPool::ShortCtor_CB method.
 */
class CStringPool_Short_CB_Transition : public CTransitionType
	{
public:
	static TInt Test();
	/**
		@fn				CStringPool_Short_CB_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_Short_CB_Transition is fully constructed.
	*/
	CStringPool_Short_CB_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CStringPool::ShortCtor_CB method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see CStringPool::ShortCtor_CB)
		@since			7.0
		@return			None
		@pre 			CStringPool_Short_CB_Transition is fully constructed.
		@post			No change in the CStringPool_ShortCtor_CB_Transition apart
						from iStringPool, which may have changed state.
						(@see CStringPool::ShortCtor_CB post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_Short_CB_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_Short_CB_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::MultipleTableCICtor method.
 */
class CStringPool_MultipleTableCICtor_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_MultipleTableCICtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_MultipleTableCICtor_Transition is fully constructed.
	*/
	CStringPool_MultipleTableCICtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::MultipleTableCICtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::MultipleTableCICtor)
		@since			7.0
		@return			None
		@pre 			CStringPool_MultipleTableCICtor_Transition is fully constructed.
		@post			No change in the CStringPool_MultipleTableCICtor_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::MultipleTableCICtor post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_MultipleTableCICtor_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_MultipleTableCICtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::MutlipleCICeqC method.
 */
class CStringPool_MutlipleCICeqC_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_MutlipleCICeqC_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_MutlipleCICeqC_Transition is fully constructed.
	*/
	CStringPool_MutlipleCICeqC_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::MutlipleCICeqC method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::MutlipleCICeqC)
		@since			7.0
		@return			None
		@pre 			CStringPool_MutlipleCICeqC_Transition is fully constructed.
		@post			No change in the CStringPool_MutlipleCICeqC_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::MutlipleCICeqC post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_MutlipleCICeqC_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_MutlipleCICeqC_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::MultipleCIStringToIndex method.
 */
class CStringPool_MultipleCIStringToIndex_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_MultipleCIStringToIndex_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_MultipleCIStringToIndex_Transition is fully constructed.
	*/
	CStringPool_MultipleCIStringToIndex_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::MultipleCIStringToIndex method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::MultipleCIStringToIndex)
		@since			7.0
		@return			None
		@pre 			CStringPool_MultipleCIStringToIndex_Transition is fully constructed.
		@post			No change in the CStringPool_MultipleCIStringToIndex_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::MultipleCIStringToIndex post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_MultipleCIStringToIndex_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_MultipleCIStringToIndex_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::MultipleCIDynamicString method.
 */
class CStringPool_MultipleCIDynamicString_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_MultipleCIDynamicString_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_MultipleCIDynamicString_Transition is fully constructed.
	*/
	CStringPool_MultipleCIDynamicString_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::MultipleCIDynamicString method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::MultipleCIDynamicString)
		@since			7.0
		@return			None
		@pre 			CStringPool_MultipleCIDynamicString_Transition is fully constructed.
		@post			No change in the CStringPool_MultipleCIDynamicString_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::MultipleCIDynamicString post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_MultipleCIDynamicString_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_MultipleCIDynamicString_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::MultipleCIIndexToStringDynamic method.
 */
class CStringPool_MultipleCIIndexToStringDynamic_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_MultipleCIIndexToStringDynamic_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_MultipleCIIndexToStringDynamic_Transition is fully constructed.
	*/
	CStringPool_MultipleCIIndexToStringDynamic_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::MultipleCIIndexToStringDynamic method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::MultipleCIIndexToStringDynamic)
		@since			7.0
		@return			None
		@pre 			CStringPool_MultipleCIIndexToStringDynamic_Transition is fully constructed.
		@post			No change in the CStringPool_MultipleCIIndexToStringDynamic_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::MultipleCIIndexToStringDynamic post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_MultipleCIIndexToStringDynamic_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_MultipleCIIndexToStringDynamic_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::MutlipleCSCeqC method.
 */
class CStringPool_MutlipleCSCeqC_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_MutlipleCSCeqC_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_MutlipleCSCeqC_Transition is fully constructed.
	*/
	CStringPool_MutlipleCSCeqC_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::MutlipleCSCeqC method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::MutlipleCSCeqC)
		@since			7.0
		@return			None
		@pre 			CStringPool_MutlipleCSCeqC_Transition is fully constructed.
		@post			No change in the CStringPool_MutlipleCSCeqC_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::MutlipleCSCeqC post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_MutlipleCSCeqC_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_MutlipleCSCeqC_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::MultipleCSStringToIndex method.
 */
class CStringPool_MultipleCSStringToIndex_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_MultipleCSStringToIndex_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_MultipleCSStringToIndex_Transition is fully constructed.
	*/
	CStringPool_MultipleCSStringToIndex_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::MultipleCSStringToIndex method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::MultipleCSStringToIndex)
		@since			7.0
		@return			None
		@pre 			CStringPool_MultipleCSStringToIndex_Transition is fully constructed.
		@post			No change in the CStringPool_MultipleCSStringToIndex_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::MultipleCSStringToIndex post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_MultipleCSStringToIndex_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_MultipleCSStringToIndex_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::MultipleCSDynamicString method.
 */
class CStringPool_MultipleCSDynamicString_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_MultipleCSDynamicString_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_MultipleCSDynamicString_Transition is fully constructed.
	*/
	CStringPool_MultipleCSDynamicString_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::MultipleCSDynamicString method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::MultipleCSDynamicString)
		@since			7.0
		@return			None
		@pre 			CStringPool_MultipleCSDynamicString_Transition is fully constructed.
		@post			No change in the CStringPool_MultipleCSDynamicString_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::MultipleCSDynamicString post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_MultipleCSDynamicString_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_MultipleCSDynamicString_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::MultipleCSIndexToStringDynamic method.
 */
class CStringPool_MultipleCSIndexToStringDynamic_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_MultipleCSIndexToStringDynamic_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_MultipleCSIndexToStringDynamic_Transition is fully constructed.
	*/
	CStringPool_MultipleCSIndexToStringDynamic_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::MultipleCSIndexToStringDynamic method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::MultipleCSIndexToStringDynamic)
		@since			7.0
		@return			None
		@pre 			CStringPool_MultipleCSIndexToStringDynamic_Transition is fully constructed.
		@post			No change in the CStringPool_MultipleCSIndexToStringDynamic_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::MultipleCSIndexToStringDynamic post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_MultipleCSIndexToStringDynamic_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_MultipleCSIndexToStringDynamic_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the StringPool::MultipleTableDtor method.
 */
class CStringPool_MultipleTableDtor_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_MultipleTableDtor_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_MultipleTableDtor_Transition is fully constructed.
	*/
	CStringPool_MultipleTableDtor_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the StringPool::MultipleTableDtor method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see StringPool::MultipleTableDtor)
		@since			7.0
		@return			None
		@pre 			CStringPool_MultipleTableDtor_Transition is fully constructed.
		@post			No change in the CStringPool_MultipleTableDtor_Transition apart
						from iStringPool, which may have changed state.
						(@see StringPool::MultipleTableDtor post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_MultipleTableDtor_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_MultipleTableDtor_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CStringPool::BEU55DJG3 method.
 */
class CStringPool_BEU55DJG3_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_BEU55DJG3_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_BEU55DJG3_Transition is fully constructed.
	*/
	CStringPool_BEU55DJG3_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CStringPool::BEU55DJG3 method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see CStringPool::BEU55DJG3)
		@since			7.0
		@return			None
		@pre 			CStringPool_BEU55DJG3_Transition is fully constructed.
		@post			No change in the CStringPool_BEU55DJG3_Transition apart
						from iStringPool, which may have changed state.
						(@see CStringPool::BEU55DJG3 post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_BEU55DJG3_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_BEU55DJG3_Transition

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CStringPool::KRN56NDEZ method.
 */
class CStringPool_KRN56NDEZ_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_KRN56NDEZ_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_KRN56NDEZ_Transition is fully constructed.
	*/
	CStringPool_KRN56NDEZ_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CStringPool::KRN56NDEZ method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see CStringPool::KRN56NDEZ)
		@since			7.0
		@return			None
		@pre 			CStringPool_KRN56NDEZ_Transition is fully constructed.
		@post			No change in the CStringPool_KRN56NDEZ_Transition apart
						from iStringPool, which may have changed state.
						(@see CStringPool::KRN56NDEZ post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_KRN56NDEZ_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_KRN56NDEZ_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CStringPool::APY57TEH3 method.
 */
class CStringPool_APY57TEH3_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_APY57TEH3_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_APY57TEH3_Transition is fully constructed.
	*/
	CStringPool_APY57TEH3_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CStringPool::APY57TEH3 method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see CStringPool::APY57TEH3)
		@since			7.0
		@return			None
		@pre 			CStringPool_APY57TEH3_Transition is fully constructed.
		@post			No change in the CStringPool_APY57TEH3_Transition apart
						from iStringPool, which may have changed state.
						(@see CStringPool::APY57TEH3 post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_APY57TEH3_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_APY57TEH3_Transition


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Transition test of the CStringPool::HAD57SK27 method.
 */
class CStringPool_HAD57SK27_Transition : public CTransitionType
	{
public:
	/**
		@fn				CStringPool_HAD57SK27_Transition(CUnitTestContext& aUTContext,
																	TTransitionValidator& aValidator)
		Intended Usage	: Standard c'tor method.
		Error Condition	: None.
		@since			7.0
		@param			aUTContext The context this transition is operating in.
		@param			aValidator Used for checking the pre & post conditions of the test object.
		@pre 			None.
		@post			CStringPool_HAD57SK27_Transition is fully constructed.
	*/
	CStringPool_HAD57SK27_Transition(CUnitTestContext& aUTContext,
													TTransitionValidator& aValidator);
	/**
		@fn				TransitMethodL()
		Intended Usage	: To execute the CStringPool::HAD57SK27 method for the test harness.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, (@see CStringPool::HAD57SK27)
		@since			7.0
		@return			None
		@pre 			CStringPool_HAD57SK27_Transition is fully constructed.
		@post			No change in the CStringPool_HAD57SK27_Transition apart
						from iStringPool, which may have changed state.
						(@see CStringPool::HAD57SK27 post-condition) for iStringPool's new state.
	*/
	inline void TransitMethodL();

	/**
		@fn				Context() const
		Intended Usage	: To provide access to the unit test context cast to the correct type.
		Error Condition	: None.
		@since			7.0
		@return			The unit test context cast to a CStringPool_UnitTestContext
		@pre 			iUTContext is a valid CStringPool_UnitTestContext.
		@post			No change in the CStringPool_HAD57SK27_Transition
	*/
	inline CStringPool_UnitTestContext& Context() const;
	};	// CStringPool_HAD57SK27_Transition



#include "StringPoolTransitions.inl"

#include "StringPoolUnitTestContext.inl"

#endif // __STRINGPOOLTRANSITIONS_H__

