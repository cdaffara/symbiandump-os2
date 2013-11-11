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
// The unit test class definitions for the CTestManager class.
// 
//

#ifndef __TESTMANAGERUNITTEST_H__
#define __TESTMANAGERUNITTEST_H__

#include <ecom/test_bed/testbeddefinitions.h>
#include "TestManagerStateAccessor.h"
#include "TestManagerTransitionValidation.h"
#include "TestManagerTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CreateAndDestroy on CTestManager, the class under test.
 */
class CTestManager_CreateAndDestroy_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave  		KErrNoMemory
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CTestManager_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CTestManager_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CTestManager_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CTestManager_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			6.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CTestManager_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CTestManager_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			6.0
		@pre 			CTestManager_CreateAndDestroy_UnitTest is fully constructed.
		@post			CTestManager_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CTestManager_CreateAndDestroy_UnitTest();

private:
	/**
		@fn				CTestManager_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CTestManager class.
		@pre 			None.
		@post			CTestManager_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CTestManager_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			6.0
		@return			None 
		@pre 			CTestManager_CreateAndDestroy_UnitTest is fully constructed.
		@post			CTestManager_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CTestManager class tested by this UnitTest's transitions.
	 */
	CTestManager_UnitTestContext* iUTContext;
	TTestManager_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TTestManager_Ctor_TransitionValidator*	iCtorValidator;
	TTestManager_Dtor_TransitionValidator*	iDtorValidator;
	};	// CTestManager_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Unit Test for ManageTests on CTestManager, the class under test.
 */
class CTestManager_ManageTests_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave  		KErrNoMemory
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CTestManager_ManageTests_UnitTest* The constructed object.
		@pre 			None.
		@post			CTestManager_ManageTests_UnitTest is fully constructed, and initialised.
	 */
	static CTestManager_ManageTests_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CTestManager_ManageTests_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			6.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CTestManager_ManageTests_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CTestManager_ManageTests_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			6.0
		@pre 			CTestManager_ManageTests_UnitTest is fully constructed.
		@post			CTestManager_ManageTests_UnitTest is fully destroyed.
	*/
	~CTestManager_ManageTests_UnitTest();

private:
	/**
		@fn				CTestManager_ManageTests_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CTestManager class.
		@pre 			None.
		@post			CTestManager_ManageTests_UnitTest is fully constructed.
	*/
	inline CTestManager_ManageTests_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			6.0
		@return			None 
		@pre 			CTestManager_ManageTests_UnitTest is fully constructed.
		@post			CTestManager_ManageTests_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CTestManager class tested by this UnitTest's transitions.
	 */
	CTestManager_UnitTestContext* iUTContext;
	TTestManager_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TTestManager_Ctor_TransitionValidator*	iCtorValidator;
	TTestManager_RunTests_TransitionValidator*	iRunTestsValidator;
	TTestManager_TestComponent_TransitionValidator*	iTestComponentValidator;
	TTestManager_Dtor_TransitionValidator*	iDtorValidator;
	};	// CTestManager_RunTests_UnitTest

// ______________________________________________________________________________
//

class CTestManager_CreateAndDestroy_UnitTest_STUB : public CUnitTest
	{
public:
	static CTestManager_CreateAndDestroy_UnitTest_STUB* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	inline TInt RunError(TInt aError);

	~CTestManager_CreateAndDestroy_UnitTest_STUB();

private:
	inline CTestManager_CreateAndDestroy_UnitTest_STUB(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	void ConstructL();

	/* No data since class acts as a do nothing stub*/
	};

#endif	// __TESTMANAGERUNITTEST_H__
