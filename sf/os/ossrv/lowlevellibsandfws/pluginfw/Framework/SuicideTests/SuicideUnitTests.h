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
// This file contains the definition of the 
// class CSuicideInterfaceUnitTest	
// 
//

#ifndef __SUICIDEUNITTEST_H__
#define __SUICIDEUNITTEST_H__

#include <e32base.h>

#include <test_bed/unittest.h>
#include "SuicideStateAccessors.h"
#include "SuicideTransitions.h"
#include "SuicideTransitionValidation.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : UnitTest CreateAndDestroy on the CExampleInterface test class.
 */
class CSuicideInterfaceCreateAndDestroyUnitTest : public CUnitTest
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
		@param			aStateAccessor WhiteBox state access to the CExampleInterface class.
		@return			CSuicideInterfaceCreateAndDestroyUnitTest* The constructed object.
		@pre 			None.
		@post			CSuicideInterfaceCreateAndDestroyUnitTest is fully constructed, and initialised.
	 */
	static CSuicideInterfaceCreateAndDestroyUnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CSuicideInterfaceCreateAndDestroyUnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CSuicideInterfaceCreateAndDestroyUnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CSuicideInterfaceCreateAndDestroyUnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CSuicideInterfaceCreateAndDestroyUnitTest is fully constructed.
		@post			CSuicideInterfaceCreateAndDestroyUnitTest is fully destroyed.
	*/
	inline ~CSuicideInterfaceCreateAndDestroyUnitTest();

private:
	/**
		@fn				CSuicideInterfaceCreateAndDestroyUnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CExampleInterface class.
		@pre 			None.
		@post			CSuicideInterfaceCreateAndDestroyUnitTest is fully constructed.
	*/
	inline CSuicideInterfaceCreateAndDestroyUnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CSuicideInterfaceCreateAndDestroyUnitTest is fully constructed.
		@post			CSuicideInterfaceCreateAndDestroyUnitTest is fully initialised.
	*/
	inline void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CExampleInterface class tested by this UintTest's transitions.
	 */
	CSuicideInterface_UnitTestContext*		iUTContext;
	TSuicideInterface_Ctor_TransitionValidator*		iCtorValidator;
	TSuicideInterface_Dtor_TransitionValidator*		iDtorValidator;
	TSuicideInterface_StateAccessor*	iStateAccessor;
	REComSession* iEComSession;
	};

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : UnitTest of the CExampleInterface test class.
 */
class CSuicideInterfaceFireAndForgetUnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger, 
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack
		Error Condition	: Leaves with the error code.
		@leave  		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor The WhiteBox state access for the CExampleInterface class.
		@return			CSuicideInterfaceFireAndForgetUnitTest* The constructed object.
		@pre 			None
		@post			CSuicideInterfaceFireAndForgetUnitTest is fully constructed, and initialised.
	 */
	static CSuicideInterfaceFireAndForgetUnitTest* NewL(CDataLogger& aDataLogger, 
												MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Trap on the RunL leave, called by the Active Scheduler.
		Error Condition	: @see CUnitTest::RunError()	
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise @see CUnitTest::RunError()
		@pre 			CSuicideInterfaceFireAndForgetUnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CSuicideInterfaceFireAndForgetUnitTest()
		Intended Usage	: Standard Destructor
		Error Condition	: None.	
		@since			7.0
		@pre 			CSuicideInterfaceFireAndForgetUnitTest is fully constructed.
		@post			CSuicideInterfaceFireAndForgetUnitTest is fully destroyed.
	*/
	inline ~CSuicideInterfaceFireAndForgetUnitTest();

private:
	/**
		@fn				CSuicideInterfaceFireAndForgetUnitTest(CDataLogger& aDataLogger, 
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor
		Error Condition	: None	
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor The WhiteBox state access for the CExampleInterface class.
		@pre 			None
		@post			CSuicideInterfaceFireAndForgetUnitTest is fully constructed.
	*/
	inline CSuicideInterfaceFireAndForgetUnitTest(CDataLogger& aDataLogger, 
										MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CSuicideInterfaceFireAndForgetUnitTest is fully constructed.
		@post			CSuicideInterfaceFireAndForgetUnitTest is fully initialised.
	*/
	inline void ConstructL();

/** The context of the Unit Test i.e a ClassName class */
	CSuicideInterface_UnitTestContext*		iUTContext;
	TSuicideInterface_Ctor_TransitionValidator*		iCtorValidator;
	TSuicideInterface_FireAndForget_TransitionValidator*	iFireAndForgetValidator;
	TSuicideInterface_StateAccessor*	iStateAccessor;
	REComSession* iEComSession;
	};

#endif	// __SUICIDEUNITTEST_H__
