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
// The definitions of the Unit tests upon the CLoadManager class
// 
//

#ifndef __LOADMANAGERUNITTEST_H__
#define __LOADMANAGERUNITTEST_H__


#include <ecom/test_bed/testbeddefinitions.h>
#include "LoadManagerStateAccessor.h"
#include "LoadManagerTransitionValidation.h"
#include "LoadManagerTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for CreateAndDestroy on CLoadManager, the class under test.
 */
class CLoadManager_CreateAndDestroy_UnitTest : public CUnitTest
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
		@return			CLoadManager_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CLoadManager_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CLoadManager_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CLoadManager_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CLoadManager_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CLoadManager_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CLoadManager_CreateAndDestroy_UnitTest is fully constructed.
		@post			CLoadManager_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CLoadManager_CreateAndDestroy_UnitTest();

/**
	@fn				PrepareUnitTestL()
	Intended Usage	: Called by test framework just before test is run to allow
					any test preparation to take place.
	Error Condition	: 
	@since			7.0
	@pre 			CLoadManager_CreateAndDestroy_UnitTest is fully constructed.
	@post			This unit test is ready to run.
*/
	void PrepareUnitTestL();

private:
	/**
		@fn				CLoadManager_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CLoadManager class.
		@pre 			None.
		@post			CLoadManager_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CLoadManager_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CLoadManager_CreateAndDestroy_UnitTest is fully constructed.
		@post			CLoadManager_CreateAndDestroy_UnitTest is fully initialised.
	*/
	inline void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CLoadManager class tested by this UnitTest's transitions.
	 */
	CLoadManager_UnitTestContext* iUTContext;
	TLoadManager_StateAccessor*	iStateAccessor;
	/* Example c'tor and d'tor transition validators */
	TLoadManager_Ctor_TransitionValidator*	iCtorValidator;
	TLoadManager_Dtor_TransitionValidator*	iDtorValidator;
	};	// CLoadManager_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for FindInstantiationAndDestroy on CLoadManager, the class under test.
 */
class CLoadManager_FindInstantiationAndDestroy_UnitTest : public CUnitTest
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
		@return			CLoadManager_FindInstantiationAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CLoadManager_FindInstantiationAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CLoadManager_FindInstantiationAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CLoadManager_FindInstantiationAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CLoadManager_FindInstantiationAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CLoadManager_FindInstantiationAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CLoadManager_FindInstantiationAndDestroy_UnitTest is fully constructed.
		@post			CLoadManager_FindInstantiationAndDestroy_UnitTest is fully destroyed.
	*/
	~CLoadManager_FindInstantiationAndDestroy_UnitTest();

	void PrepareUnitTestL();

private:
	/**
		@fn				CLoadManager_FindInstantiationAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CLoadManager class.
		@pre 			None.
		@post			CLoadManager_FindInstantiationAndDestroy_UnitTest is fully constructed.
	*/
	inline CLoadManager_FindInstantiationAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CLoadManager_FindInstantiationAndDestroy_UnitTest is fully constructed.
		@post			CLoadManager_FindInstantiationAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CLoadManager class tested by this UnitTest's transitions.
	 */
	CLoadManager_UnitTestContext* iUTContext;
	TLoadManager_StateAccessor*	iStateAccessor;
	/* C'tor, d'tor, and method transition validators */
	TLoadManager_Ctor_TransitionValidator*	iCtorValidator;
	TLoadManager_InstantiationMethod_TransitionValidator*	iInstantiationMethodValidator;
	TLoadManager_DestroyThis_TransitionValidator*	iDestroyThisValidator;
	TLoadManager_Dtor_TransitionValidator*	iDtorValidator;
	};	// CLoadManager_FindInstantiationAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for Find Instantiation failure UnitTest on CLoadManager, the class under test.
 */
class CLoadManager_FindInstantiationFailure_UnitTest : public CUnitTest
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
		@return			CLoadManager_FindInstantiationAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CLoadManager_FindInstantiationAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CLoadManager_FindInstantiationFailure_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CLoadManager_FindInstantiationAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CLoadManager_FindInstantiationFailure_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CLoadManager_FindInstantiationFailure_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CLoadManager_FindInstantiationFailure_UnitTest is fully constructed.
		@post			CLoadManager_FindInstantiationFailure_UnitTest is fully destroyed.
	*/
	~CLoadManager_FindInstantiationFailure_UnitTest();

private:
	/**
		@fn				CLoadManager_FindInstantiationFailure_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CLoadManager class.
		@pre 			None.
		@post			CLoadManager_FindInstantiationAndDestroy_UnitTest is fully constructed.
	*/
	inline CLoadManager_FindInstantiationFailure_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CLoadManager_FindInstantiationFailure_UnitTest is fully constructed.
		@post			CLoadManager_FindInstantiationFailure_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CLoadManager class tested by this UnitTest's transitions.
	 */
	CLoadManager_UnitTestContext* iUTContext;
	TLoadManager_StateAccessor*	iStateAccessor;
	/* C'tor, d'tor, and method transition validators */
	TLoadManager_Ctor_TransitionValidator*	iCtorValidator;
	TLoadManager_InstantiationFailure_TransitionValidator*	iInstantiationMethodValidator;
	TLoadManager_Dtor_TransitionValidator*	iDtorValidator;
	};	// CLoadManager_FindInstantiationFailure_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : 
 */
class CLoadManager_DefectFOT56ULPM_UnitTest : public CUnitTest
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
		@return			CLoadManager_DefectFOT56ULPM_UnitTest* The constructed object.
		@pre 			None.
		@post			CLoadManager_DefectFOT56ULPM_UnitTest is fully constructed, and initialised.
	 */
	static CLoadManager_DefectFOT56ULPM_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CLoadManager_FindInstantiationAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CLoadManager_DefectFOT56ULPM_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CLoadManager_DefectFOT56ULPM_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CLoadManager_DefectFOT56ULPM_UnitTest is fully constructed.
		@post			CLoadManager_DefectFOT56ULPM_UnitTest is fully destroyed.
	*/
	~CLoadManager_DefectFOT56ULPM_UnitTest();

private:
	/**
		@fn				CLoadManager_DefectFOT56ULPM_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CLoadManager class.
		@pre 			None.
		@post			CLoadManager_DefectFOT56ULPM_UnitTest is fully constructed.
	*/
	inline CLoadManager_DefectFOT56ULPM_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CLoadManager_DefectFOT56ULPM_UnitTest is fully constructed.
		@post			CLoadManager_DefectFOT56ULPM_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CLoadManager class tested by this UnitTest's transitions.
	 */
	CLoadManager_UnitTestContext* iUTContext;
	TLoadManager_StateAccessor*	iStateAccessor;
	/* C'tor, d'tor, and method transition validators */
	TLoadManager_Ctor_TransitionValidator*	iCtorValidator;
	TLoadManager_InstantiationMethod_TransitionValidator*	iInstantiationMethodValidator;
	TLoadManager_DestroyThis_TransitionValidator*	iDestroyThisValidator;
	TLoadManager_Dtor_TransitionValidator*	iDtorValidator;
	};	// CLoadManager_DefectFOT56ULPM_UnitTest

#include "LoadManagerUnitTest.inl"

#endif // __LOADMANAGERUNITTEST_H__
