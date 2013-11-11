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
// The unit test class definitions for the CUnitTest class.
// 
//

#ifndef __UNITTESTUNITTEST_H__
#define __UNITTESTUNITTEST_H__

#include <ecom/test_bed/testbeddefinitions.h>
#include "UnitTestStateAccessor.h"
#include "UnitTestTransitionValidation.h"
#include "UnitTestTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CreateAndDestroy on CUnitTest, the class under test.
 */
class CUnitTest_CreateAndDestroy_UnitTest : public CUnitTest
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
		@return			CUnitTest_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CUnitTest_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CUnitTest_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CUnitTest_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CUnitTest_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CUnitTest_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CUnitTest_CreateAndDestroy_UnitTest is fully constructed.
		@post			CUnitTest_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CUnitTest_CreateAndDestroy_UnitTest();

private:
	/**
		@fn				CUnitTest_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CUnitTest class.
		@pre 			None.
		@post			CUnitTest_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CUnitTest_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CUnitTest_CreateAndDestroy_UnitTest is fully constructed.
		@post			CUnitTest_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CUnitTest class tested by this UnitTest's transitions.
	 */
	CUnitTest_UnitTestContext* iUTContext;
	TUnitTest_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TUnitTest_Ctor_TransitionValidator*	iCtorValidator;
	TUnitTest_Dtor_TransitionValidator*	iDtorValidator;
	};	// CUnitTest_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for ManageTransitions on CUnitTest, the class under test.
 */
class CUnitTest_ManageTransitions_UnitTest : public CUnitTest
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
		@return			CUnitTest_ManageTransitions_UnitTest* The constructed object.
		@pre 			None.
		@post			CUnitTest_ManageTransitions_UnitTest is fully constructed, and initialised.
	 */
	static CUnitTest_ManageTransitions_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CUnitTest_ManageTransitions_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CUnitTest_ManageTransitions_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CUnitTest_ManageTransitions_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CUnitTest_ManageTransitions_UnitTest is fully constructed.
		@post			CUnitTest_ManageTransitions_UnitTest is fully destroyed.
	*/
	~CUnitTest_ManageTransitions_UnitTest();

private:
	/**
		@fn				CUnitTest_ManageTransitions_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CUnitTest class.
		@pre 			None.
		@post			CUnitTest_ManageTransitions_UnitTest is fully constructed.
	*/
	inline CUnitTest_ManageTransitions_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CUnitTest_ManageTransitions_UnitTest is fully constructed.
		@post			CUnitTest_ManageTransitions_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CUnitTest class tested by this UnitTest's transitions.
	 */
	CUnitTest_UnitTestContext* iUTContext;
	TUnitTest_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TUnitTest_Ctor_TransitionValidator*	iCtorValidator;
	TUnitTest_TransitionSet_TransitionValidator* iTransitionSetValidator;
	TUnitTest_SetCurrentTransition_TransitionValidator* iSetCurrentTransitionValidator;
	TUnitTest_GetCurrentTransition_TransitionValidator* iGetCurrentTransitionValidator;
	TUnitTest_Dtor_TransitionValidator*	iDtorValidator;
	};	// CUnitTest_ManageTransitions_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for RunTests on CUnitTest, the class under test.
 */
class CUnitTest_RunTests_UnitTest : public CUnitTest
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
		@return			CUnitTest_RunTests_UnitTest* The constructed object.
		@pre 			None.
		@post			CUnitTest_RunTests_UnitTest is fully constructed, and initialised.
	 */
	static CUnitTest_RunTests_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CUnitTest_RunTests_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CUnitTest_RunTests_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CUnitTest_RunTests_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CUnitTest_RunTests_UnitTest is fully constructed.
		@post			CUnitTest_RunTests_UnitTest is fully destroyed.
	*/
	~CUnitTest_RunTests_UnitTest();

private:
	/**
		@fn				CUnitTest_RunTests_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CUnitTest class.
		@pre 			None.
		@post			CUnitTest_RunTests_UnitTest is fully constructed.
	*/
	inline CUnitTest_RunTests_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CUnitTest_RunTests_UnitTest is fully constructed.
		@post			CUnitTest_RunTests_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CUnitTest class tested by this UnitTest's transitions.
	 */
	CUnitTest_UnitTestContext* iUTContext;
	TUnitTest_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TUnitTest_Ctor_TransitionValidator*	iCtorValidator;
	TUnitTest_SetParameters_TransitionValidator* iSetParametersValidator;
	TUnitTest_PrepareUnitTest_TransitionValidator* iPrepareUnitTestValidator;
	TUnitTest_RunTest_TransitionValidator* iRunTestValidator;
	TUnitTest_Dtor_TransitionValidator*	iDtorValidator;
	};	// CUnitTest_RunTests_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for AllOther on CUnitTest, the class under test.
 */
class CUnitTest_AllOther_UnitTest : public CUnitTest
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
		@return			CUnitTest_AllOther_UnitTest* The constructed object.
		@pre 			None.
		@post			CUnitTest_AllOther_UnitTest is fully constructed, and initialised.
	 */
	static CUnitTest_AllOther_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CUnitTest_AllOther_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CUnitTest_AllOther_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CUnitTest_AllOther_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CUnitTest_AllOther_UnitTest is fully constructed.
		@post			CUnitTest_AllOther_UnitTest is fully destroyed.
	*/
	~CUnitTest_AllOther_UnitTest();

private:
	/**
		@fn				CUnitTest_AllOther_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CUnitTest class.
		@pre 			None.
		@post			CUnitTest_AllOther_UnitTest is fully constructed.
	*/
	inline CUnitTest_AllOther_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CUnitTest_AllOther_UnitTest is fully constructed.
		@post			CUnitTest_AllOther_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CUnitTest class tested by this UnitTest's transitions.
	 */
	CUnitTest_UnitTestContext* iUTContext;
	TUnitTest_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TUnitTest_Ctor_TransitionValidator*	iCtorValidator;
	TUnitTest_AddTransition_TransitionValidator* iAddTransitionValidator;
	TUnitTest_AddBlockingTransition_TransitionValidator* iAddBlockingTransitionValidator;
	TUnitTest_AddLeaveErrorCode_TransitionValidator* iAddLeaveErrorCodeValidator;
	TUnitTest_UnitTestName_TransitionValidator* iUnitTestNameValidator;
	TUnitTest_Dtor_TransitionValidator*	iDtorValidator;
	};	// CUnitTest_AllOther_UnitTest


#endif	// __UNITTESTUNITTEST_H__
