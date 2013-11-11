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
// The unit test class definitions for the CTestController class.
// 
//

#ifndef __TESTCONTROLLERUNITTEST_H__
#define __TESTCONTROLLERUNITTEST_H__

#include <ecom/test_bed/testbeddefinitions.h>
#include "TestControllerStateAccessor.h"
#include "TestControllerTransitionValidation.h"
#include "TestControllerTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CreateAndDestroy on CTestController, the class under test.
 */
class CTestController_CreateAndDestroy_UnitTest : public CUnitTest
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
		@return			CTestController_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CTestController_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CTestController_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CTestController_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CTestController_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CTestController_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CTestController_CreateAndDestroy_UnitTest is fully constructed.
		@post			CTestController_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CTestController_CreateAndDestroy_UnitTest();

private:
	/**
		@fn				CTestController_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CTestController class.
		@pre 			None.
		@post			CTestController_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CTestController_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CTestController_CreateAndDestroy_UnitTest is fully constructed.
		@post			CTestController_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CTestController class tested by this UnitTest's transitions.
	 */
	CTestController_UnitTestContext* iUTContext;
	TTestController_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TTestController_Ctor_TransitionValidator*	iCtorValidator;
	TTestController_Dtor_TransitionValidator*	iDtorValidator;
	RFs iFs;
	};	// CTestController_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for FindTests on CTestController, the class under test.
 */
class CTestController_FindTests_UnitTest : public CUnitTest
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
		@return			CTestController_FindTests_UnitTest* The constructed object.
		@pre 			None.
		@post			CTestController_FindTests_UnitTest is fully constructed, and initialised.
	 */
	static CTestController_FindTests_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CTestController_FindTests_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CTestController_FindTests_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CTestController_FindTests_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CTestController_FindTests_UnitTest is fully constructed.
		@post			CTestController_FindTests_UnitTest is fully destroyed.
	*/
	~CTestController_FindTests_UnitTest();

private:
	/**
		@fn				CTestController_FindTests_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CTestController class.
		@pre 			None.
		@post			CTestController_FindTests_UnitTest is fully constructed.
	*/
	inline CTestController_FindTests_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CTestController_FindTests_UnitTest is fully constructed.
		@post			CTestController_FindTests_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CTestController class tested by this UnitTest's transitions.
	 */
	CTestController_UnitTestContext* iUTContext;
	TTestController_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TTestController_Ctor_TransitionValidator*	iCtorValidator;
	TTestController_FindComponents_TransitionValidator* iFindComponentsValidator;
	TTestController_Start_TransitionValidator* iStartValidator;
	TTestController_Dtor_TransitionValidator*	iDtorValidator;
	RFs iFs;
	};	// CTestController_FindTests_UnitTest

// ______________________________________________________________________________
//

class CTestController_CreateAndDestroy_UnitTest_STUB : public CUnitTest
	{
public:
	static CTestController_CreateAndDestroy_UnitTest_STUB* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	inline TInt RunError(TInt aError);

	~CTestController_CreateAndDestroy_UnitTest_STUB();

private:
	inline CTestController_CreateAndDestroy_UnitTest_STUB(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	void ConstructL();

	/* No data since class acts as a do nothing stub*/
	};

#endif	// __TESTCONTROLLERUNITTEST_H__
