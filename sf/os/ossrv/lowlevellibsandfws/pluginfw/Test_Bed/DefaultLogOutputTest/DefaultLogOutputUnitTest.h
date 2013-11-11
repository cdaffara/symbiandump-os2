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
// The unit test class definitions for the CDefaultLogOutput class.
// 
//

#ifndef __DEFAULTLOGOUTPUTUNITTEST_H__
#define __DEFAULTLOGOUTPUTUNITTEST_H__

#include <ecom/test_bed/testbeddefinitions.h>
#include "DefaultLogOutputStateAccessor.h"
#include "DefaultLogOutputTransitionValidation.h"
#include "DefaultLogOutputTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CreateAndDestroy on CDefaultLogOutput, the class under test.
 */
class CDefaultLogOutput_CreateAndDestroy_UnitTest : public CUnitTest
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
		@return			CDefaultLogOutput_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CDefaultLogOutput_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CDefaultLogOutput_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDefaultLogOutput_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDefaultLogOutput_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDefaultLogOutput_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDefaultLogOutput_CreateAndDestroy_UnitTest is fully constructed.
		@post			CDefaultLogOutput_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CDefaultLogOutput_CreateAndDestroy_UnitTest();

private:
	/**
		@fn				CDefaultLogOutput_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDefaultLogOutput class.
		@pre 			None.
		@post			CDefaultLogOutput_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CDefaultLogOutput_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDefaultLogOutput_CreateAndDestroy_UnitTest is fully constructed.
		@post			CDefaultLogOutput_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDefaultLogOutput class tested by this UnitTest's transitions.
	 */
	CDefaultLogOutput_UnitTestContext* iUTContext;
	TDefaultLogOutput_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TDefaultLogOutput_Ctor_TransitionValidator*	iCtorValidator;
	TDefaultLogOutput_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDefaultLogOutput_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for OpenAndClose on CDefaultLogOutput, the class under test.
 */
class CDefaultLogOutput_OpenAndClose_UnitTest : public CUnitTest
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
		@return			CDefaultLogOutput_OpenAndClose_UnitTest* The constructed object.
		@pre 			None.
		@post			CDefaultLogOutput_OpenAndClose_UnitTest is fully constructed, and initialised.
	 */
	static CDefaultLogOutput_OpenAndClose_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDefaultLogOutput_OpenAndClose_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDefaultLogOutput_OpenAndClose_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDefaultLogOutput_OpenAndClose_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDefaultLogOutput_OpenAndClose_UnitTest is fully constructed.
		@post			CDefaultLogOutput_OpenAndClose_UnitTest is fully destroyed.
	*/
	~CDefaultLogOutput_OpenAndClose_UnitTest();

private:
	/**
		@fn				CDefaultLogOutput_OpenAndClose_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDefaultLogOutput class.
		@pre 			None.
		@post			CDefaultLogOutput_OpenAndClose_UnitTest is fully constructed.
	*/
	inline CDefaultLogOutput_OpenAndClose_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDefaultLogOutput_OpenAndClose_UnitTest is fully constructed.
		@post			CDefaultLogOutput_OpenAndClose_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDefaultLogOutput class tested by this UnitTest's transitions.
	 */
	CDefaultLogOutput_UnitTestContext* iUTContext;
	TDefaultLogOutput_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TDefaultLogOutput_Ctor_TransitionValidator*		iCtorValidator;
	TDefaultLogOutput_Open_TransitionValidator*		iOpenValidator;
	TDefaultLogOutput_Close_TransitionValidator*	iCloseValidator;
	TDefaultLogOutput_Dtor_TransitionValidator*		iDtorValidator;
	};	// CDefaultLogOutput_OpenAndClose_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for OpenWriteClose on CDefaultLogOutput, the class under test.
 */
class CDefaultLogOutput_OpenWriteClose_UnitTest : public CUnitTest
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
		@return			CDefaultLogOutput_OpenWriteClose_UnitTest* The constructed object.
		@pre 			None.
		@post			CDefaultLogOutput_OpenWriteClose_UnitTest is fully constructed, and initialised.
	 */
	static CDefaultLogOutput_OpenWriteClose_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDefaultLogOutput_OpenWriteClose_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDefaultLogOutput_OpenWriteClose_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDefaultLogOutput_OpenWriteClose_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDefaultLogOutput_OpenWriteClose_UnitTest is fully constructed.
		@post			CDefaultLogOutput_OpenWriteClose_UnitTest is fully destroyed.
	*/
	~CDefaultLogOutput_OpenWriteClose_UnitTest();

private:
	/**
		@fn				CDefaultLogOutput_OpenWriteClose_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDefaultLogOutput class.
		@pre 			None.
		@post			CDefaultLogOutput_OpenWriteClose_UnitTest is fully constructed.
	*/
	inline CDefaultLogOutput_OpenWriteClose_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDefaultLogOutput_OpenWriteClose_UnitTest is fully constructed.
		@post			CDefaultLogOutput_OpenWriteClose_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDefaultLogOutput class tested by this UnitTest's transitions.
	 */
	CDefaultLogOutput_UnitTestContext* iUTContext;
	TDefaultLogOutput_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TDefaultLogOutput_Ctor_TransitionValidator*		iCtorValidator;
	TDefaultLogOutput_Open_TransitionValidator*		iOpenValidator;
	TDefaultLogOutput_Write_TransitionValidator*	iWriteValidator;
	TDefaultLogOutput_Close_TransitionValidator*	iCloseValidator;
	TDefaultLogOutput_Dtor_TransitionValidator*		iDtorValidator;
	};	// CDefaultLogOutput_OpenWriteClose_UnitTest


#endif	// __DEFAULTLOGOUTPUTUNITTEST_H__
