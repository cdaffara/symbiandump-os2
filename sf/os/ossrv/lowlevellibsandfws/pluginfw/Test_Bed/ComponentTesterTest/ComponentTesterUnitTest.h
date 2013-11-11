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
// The unit test class definitions for the CComponentTester class.
// 
//

#ifndef __COMPONENTTESTERUNITTEST_H__
#define __COMPONENTTESTERUNITTEST_H__

#include <ecom/test_bed/testbeddefinitions.h>
#include "ComponentTesterStateAccessor.h"
#include "ComponentTesterTransitionValidation.h"
#include "ComponentTesterTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CreateAndDestroy on CComponentTester, the class under test.
 */
class CComponentTester_CreateAndDestroy_UnitTest : public CUnitTest
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
		@return			CComponentTester_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CComponentTester_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CComponentTester_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CComponentTester_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CComponentTester_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CComponentTester_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CComponentTester_CreateAndDestroy_UnitTest is fully constructed.
		@post			CComponentTester_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CComponentTester_CreateAndDestroy_UnitTest();

private:
	/**
		@fn				CComponentTester_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CComponentTester class.
		@pre 			None.
		@post			CComponentTester_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CComponentTester_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CComponentTester_CreateAndDestroy_UnitTest is fully constructed.
		@post			CComponentTester_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CComponentTester class tested by this UnitTest's transitions.
	 */
	CComponentTester_UnitTestContext* iUTContext;
	TComponentTester_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TComponentTester_Ctor_TransitionValidator*	iCtorValidator;
	TComponentTester_Dtor_TransitionValidator*	iDtorValidator;
	};	// CComponentTester_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for ManageUnitTests on CComponentTester, the class under test.
 */
class CComponentTester_ManageUnitTests_UnitTest : public CUnitTest
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
		@return			CComponentTester_ManageUnitTests_UnitTest* The constructed object.
		@pre 			None.
		@post			CComponentTester_ManageUnitTests_UnitTest is fully constructed, and initialised.
	 */
	static CComponentTester_ManageUnitTests_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CComponentTester_ManageUnitTests_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CComponentTester_ManageUnitTests_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CComponentTester_ManageUnitTests_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CComponentTester_ManageUnitTests_UnitTest is fully constructed.
		@post			CComponentTester_ManageUnitTests_UnitTest is fully destroyed.
	*/
	~CComponentTester_ManageUnitTests_UnitTest();

private:
	/**
		@fn				CComponentTester_ManageUnitTests_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CComponentTester class.
		@pre 			None.
		@post			CComponentTester_ManageUnitTests_UnitTest is fully constructed.
	*/
	inline CComponentTester_ManageUnitTests_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave          KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CComponentTester_ManageUnitTests_UnitTest is fully constructed.
		@post			CComponentTester_ManageUnitTests_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CComponentTester class tested by this UnitTest's transitions.
	 */
	CComponentTester_UnitTestContext* iUTContext;
	TComponentTester_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TComponentTester_Ctor_TransitionValidator*	iCtorValidator;
	TComponentTester_TransitionSets_TransitionValidator*	iTransitionSetsValidator;
	TComponentTester_AddUnitTest_TransitionValidator* iAddUnitTestValidator;
	TComponentTester_AddParamUnitTest_TransitionValidator* iAddParamUnitTestValidator;
	TComponentTester_Dtor_TransitionValidator*	iDtorValidator;
	};	// CComponentTester_ManageUnitTests_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for RunTests on CComponentTester, the class under test.
 */
class CComponentTester_RunTests_UnitTest : public CUnitTest
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
		@return			CComponentTester_RunTests_UnitTest* The constructed object.
		@pre 			None.
		@post			CComponentTester_RunTests_UnitTest is fully constructed, and initialised.
	 */
	static CComponentTester_RunTests_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CComponentTester_RunTests_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CComponentTester_RunTests_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CComponentTester_RunTests_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CComponentTester_RunTests_UnitTest is fully constructed.
		@post			CComponentTester_RunTests_UnitTest is fully destroyed.
	*/
	~CComponentTester_RunTests_UnitTest();

private:
	/**
		@fn				CComponentTester_RunTests_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CComponentTester class.
		@pre 			None.
		@post			CComponentTester_RunTests_UnitTest is fully constructed.
	*/
	inline CComponentTester_RunTests_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CComponentTester_RunTests_UnitTest is fully constructed.
		@post			CComponentTester_RunTests_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CComponentTester class tested by this UnitTest's transitions.
	 */
	CComponentTester_UnitTestContext* iUTContext;
	TComponentTester_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TComponentTester_Ctor_TransitionValidator*	iCtorValidator;
	TComponentTester_TestComponent_TransitionValidator*	iTestComponentValidator;
	TComponentTester_Run_TransitionValidator* iRunValidator;
	TComponentTester_Complete_TransitionValidator* iCompleteValidator;
	TComponentTester_Dtor_TransitionValidator*	iDtorValidator;
	};	// CComponentTester_RunTests_UnitTest


#endif	// __COMPONENTTESTERUNITTEST_H__
