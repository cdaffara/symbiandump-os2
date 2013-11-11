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
// The unit test class definitions for the CComponentInfo class.
// 
//

#ifndef __COMPONENTINFOUNITTEST_H__
#define __COMPONENTINFOUNITTEST_H__

#include <ecom/test_bed/testbeddefinitions.h>
#include "ComponentInfoStateAccessor.h"
#include "ComponentInfoTransitionValidation.h"
#include "ComponentInfoTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CreateAndDestroy on CComponentInfo, the class under test.
 */
class CComponentInfo_CreateAndDestroy_UnitTest : public CUnitTest
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
		@return			CComponentInfo_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CComponentInfo_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CComponentInfo_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CComponentInfo_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CComponentInfo_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CComponentInfo_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CComponentInfo_CreateAndDestroy_UnitTest is fully constructed.
		@post			CComponentInfo_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CComponentInfo_CreateAndDestroy_UnitTest();

private:
	/**
		@fn				CComponentInfo_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CComponentInfo class.
		@pre 			None.
		@post			CComponentInfo_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CComponentInfo_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CComponentInfo_CreateAndDestroy_UnitTest is fully constructed.
		@post			CComponentInfo_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CComponentInfo class tested by this UnitTest's transitions.
	 */
	CComponentInfo_UnitTestContext* iUTContext;
	TComponentInfo_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TComponentInfo_Ctor_TransitionValidator*	iCtorValidator;
	TComponentInfo_Dtor_TransitionValidator*	iDtorValidator;
	};	// CComponentInfo_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for RetrieveTestNames on CComponentInfo, the class under test.
 */
class CComponentInfo_RetrieveTestNames_UnitTest : public CUnitTest
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
		@return			CComponentInfo_RetrieveTestNames_UnitTest* The constructed object.
		@pre 			None.
		@post			CComponentInfo_RetrieveTestNames_UnitTest is fully constructed, and initialised.
	 */
	static CComponentInfo_RetrieveTestNames_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CComponentInfo_RetrieveTestNames_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CComponentInfo_RetrieveTestNames_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CComponentInfo_RetrieveTestNames_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CComponentInfo_RetrieveTestNames_UnitTest is fully constructed.
		@post			CComponentInfo_RetrieveTestNames_UnitTest is fully destroyed.
	*/
	~CComponentInfo_RetrieveTestNames_UnitTest();

private:
	/**
		@fn				CComponentInfo_RetrieveTestNames_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CComponentInfo class.
		@pre 			None.
		@post			CComponentInfo_RetrieveTestNames_UnitTest is fully constructed.
	*/
	inline CComponentInfo_RetrieveTestNames_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CComponentInfo_RetrieveTestNames_UnitTest is fully constructed.
		@post			CComponentInfo_RetrieveTestNames_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CComponentInfo class tested by this UnitTest's transitions.
	 */
	CComponentInfo_UnitTestContext* iUTContext;
	TComponentInfo_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TComponentInfo_Ctor_TransitionValidator*	iCtorValidator;
	TComponentInfo_Entry_TransitionValidator*	iEntryValidator;
	TComponentInfo_UnitTestsInfo_TransitionValidator*		iUnitTestsInfoValidator;
	TComponentInfo_Dtor_TransitionValidator*	iDtorValidator;
	};	// CComponentInfo_RetrieveTestNames_UnitTest


#endif	// __COMPONENTINFOUNITTEST_H__
