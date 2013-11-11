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
// The definition of the CResolver unit test classes.
// 
//

#ifndef __RESOLVERUNITTESTS_H__
#define __RESOLVERUNITTESTS_H__

#include "ResolverTransitions.h"
#include "ResolverStateAccessor.h"
#include "ResolverTransitionValidation.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CreateAndDestroy on CDefaultResolver, the class under test.
 */
class CDefaultResolver_CreateAndDestroy_UnitTest : public CUnitTest
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
		@return			CDefaultResolver_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CDefaultResolver_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CDefaultResolver_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDefaultResolver_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDefaultResolver_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDefaultResolver_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDefaultResolver_CreateAndDestroy_UnitTest is fully constructed.
		@post			CDefaultResolver_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CDefaultResolver_CreateAndDestroy_UnitTest();

/**
	@fn				PrepareUnitTestL()
	Intended Usage	: Called by test framework just before test is run to allow
					any test preparation to take place.
	Error Condition	: 
	@since			7.0
	@pre 			CDefaultResolver_CreateAndDestroy_UnitTest is fully constructed.
	@post			This unit test is ready to run.
*/
	void PrepareUnitTestL();

private:
	/**
		@fn				CDefaultResolver_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDefaultResolver class.
		@pre 			None.
		@post			CDefaultResolver_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CDefaultResolver_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDefaultResolver_CreateAndDestroy_UnitTest is fully constructed.
		@post			CDefaultResolver_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDefaultResolver class tested by this UnitTest's transitions.
	 */
	CDefaultResolver_UnitTestContext* iUTContext;
	TDefaultResolver_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TDefaultResolver_Ctor_TransitionValidator*	iCtorValidator;
	TDefaultResolver_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDefaultResolver_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for IdentifyImplementation on CDefaultResolver, the class under test.
 */
class CDefaultResolver_IdentifyImplementation_UnitTest : public CUnitTest
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
		@return			CDefaultResolver_IdentifyImplementation_UnitTest* The constructed object.
		@pre 			None.
		@post			CDefaultResolver_IdentifyImplementation_UnitTest is fully constructed, and initialised.
	 */
	static CDefaultResolver_IdentifyImplementation_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDefaultResolver_IdentifyImplementation_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDefaultResolver_IdentifyImplementation_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDefaultResolver_IdentifyImplementation_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDefaultResolver_IdentifyImplementation_UnitTest is fully constructed.
		@post			CDefaultResolver_IdentifyImplementation_UnitTest is fully destroyed.
	*/
	~CDefaultResolver_IdentifyImplementation_UnitTest();

private:
	/**
		@fn				CDefaultResolver_IdentifyImplementation_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDefaultResolver class.
		@pre 			None.
		@post			CDefaultResolver_IdentifyImplementation_UnitTest is fully constructed.
	*/
	inline CDefaultResolver_IdentifyImplementation_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDefaultResolver_IdentifyImplementation_UnitTest is fully constructed.
		@post			CDefaultResolver_IdentifyImplementation_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDefaultResolver class tested by this UnitTest's transitions.
	 */
	CDefaultResolver_UnitTestContext* iUTContext;
	TDefaultResolver_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TDefaultResolver_Ctor_TransitionValidator*		iCtorValidator;
	TDefaultResolver_Default_TransitionValidator*	iDefaultValidator;
	TDefaultResolver_Match_TransitionValidator*		iMatchValidator;
	TDefaultResolver_IdentifyImplementationL_TransitionValidator*	iIdentifyImplementationValidator;
	TDefaultResolver_Dtor_TransitionValidator*		iDtorValidator;
	};	// CDefaultResolver_IdentifyImplementation_UnitTest

#include "ResolverUnitTests.inl"

#endif		// __RESOLVERUNITTESTS_H__
