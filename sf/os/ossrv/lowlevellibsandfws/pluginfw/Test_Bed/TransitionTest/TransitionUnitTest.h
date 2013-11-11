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
// The unit test class definitions for the CTransition class.
// 
//

#ifndef __TRANSITIONUNITTEST_H__
#define __TRANSITIONUNITTEST_H__

#include <ecom/test_bed/testbeddefinitions.h>
#include "TransitionStateAccessor.h"
#include "TransitionTransitionValidation.h"
#include "TransitionTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Unit Test for CreateAndDestroy on CTransition, the class under test.
 */
class CTransition_CreateAndDestroy_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave  		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CTransition_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CTransition_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CTransition_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CTransition_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CTransition_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CTransition_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CTransition_CreateAndDestroy_UnitTest is fully constructed.
		@post			CTransition_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CTransition_CreateAndDestroy_UnitTest();

private:
	/**
		@fn				CTransition_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CTransition class.
		@pre 			None.
		@post			CTransition_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CTransition_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CTransition_CreateAndDestroy_UnitTest is fully constructed.
		@post			CTransition_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CTransition class tested by this UnitTest's transitions.
	 */
	CTransition_UnitTestContext* iUTContext;
	TTransition_StateAccessor*	iStateAccessor;
	/* C'tor, d'tor, and method transition validators */
	TTransition_Ctor_TransitionValidator*	iCtorValidator;
	TTransition_Dtor_TransitionValidator*	iDtorValidator;
	};	// CTransition_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for TransitMethodL on CTransition, the class under test.
 */
class CTransition_TransitMethodL_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave  		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CTransition_TransitMethodL_UnitTest* The constructed object.
		@pre 			None.
		@post			CTransition_TransitMethodL_UnitTest is fully constructed, and initialised.
	 */
	static CTransition_TransitMethodL_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CTransition_TransitMethodL_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CTransition_TransitMethodL_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CTransition_TransitMethodL_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CTransition_TransitMethodL_UnitTest is fully constructed.
		@post			CTransition_TransitMethodL_UnitTest is fully destroyed.
	*/
	~CTransition_TransitMethodL_UnitTest();

private:
	/**
		@fn				CTransition_TransitMethodL_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CTransition class.
		@pre 			None.
		@post			CTransition_TransitMethodL_UnitTest is fully constructed.
	*/
	inline CTransition_TransitMethodL_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CTransition_TransitMethodL_UnitTest is fully constructed.
		@post			CTransition_TransitMethodL_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CTransition class tested by this UnitTest's transitions.
	 */
	CTransition_UnitTestContext* iUTContext;
	TTransition_StateAccessor*	iStateAccessor;
	/* C'tor, d'tor, and method transition validators */
	TTransition_Ctor_TransitionValidator*	iCtorValidator;
	TTransition_RepeatOnce_TransitionValidator*	iRepeatOnceValidator;
	TTransition_TransitionId_TransitionValidator* iTransitionIdValidator;
	TTransition_TransitionInfo_TransitionValidator* iTransitionInfoValidator;
	TTransition_IsBlocking_TransitionValidator* iIsBlockingValidator;
	TTransition_SetBlocking_TransitionValidator* iSetBlockingValidator;
	TTransition_TransitMethodL_TransitionValidator*	iTransitMethodLValidator;
	TTransition_Dtor_TransitionValidator*	iDtorValidator;
	};	// CTransition_TransitMethodL_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for AsyncOperation on CTransition, the class under test.
 */
class CTransition_AsyncOperation_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave  		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CTransition_AsyncOperation_UnitTest* The constructed object.
		@pre 			None.
		@post			CTransition_AsyncOperation_UnitTest is fully constructed, and initialised.
	 */
	static CTransition_AsyncOperation_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CTransition_AsyncOperation_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CTransition_AsyncOperation_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CTransition_AsyncOperation_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CTransition_AsyncOperation_UnitTest is fully constructed.
		@post			CTransition_AsyncOperation_UnitTest is fully destroyed.
	*/
	~CTransition_AsyncOperation_UnitTest();

private:
	/**
		@fn				CTransition_AsyncOperation_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CTransition class.
		@pre 			None.
		@post			CTransition_AsyncOperation_UnitTest is fully constructed.
	*/
	inline CTransition_AsyncOperation_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CTransition_AsyncOperation_UnitTest is fully constructed.
		@post			CTransition_AsyncOperation_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CTransition class tested by this UnitTest's transitions.
	 */
	CTransition_UnitTestContext* iUTContext;
	TTransition_StateAccessor*	iStateAccessor;
	/* C'tor, d'tor, and method transition validators */
	TTransition_Ctor_TransitionValidator*	iCtorValidator;
	TTransition_RunTransition_TransitionValidator* iRunTransitionValidator;
	TTransition_Cancel_TransitionValidator* iCancelValidator;
	TTransition_GetErrorCode_TransitionValidator* iGetErrorCodeValidator;
	TTransition_Dtor_TransitionValidator*	iDtorValidator;
	};	// CTransition_AsyncOperation_UnitTest


#endif	// __TRANSITIONUNITTEST_H__
