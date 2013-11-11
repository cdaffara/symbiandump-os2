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
// class CExampleInterfaceUnitTest	
// 
//

#ifndef __MAGICUNITTEST_H__
#define __MAGICUNITTEST_H__

#include <e32base.h>

#include <ecom/test_bed/unittest.h>
#include "MagicStateAccessors.h"
#include "MagicTransitions.h"
#include "MagicTransitionValidation.h"

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : UnitTest CreateAndDestroy on the CExampleInterface test class.
 */
class CExampleInterfaceCreateAndDestroyUnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave			KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CExampleInterface class.
		@return			CExampleInterfaceCreateAndDestroyUnitTest* The constructed object.
		@pre 			None.
		@post			CExampleInterfaceCreateAndDestroyUnitTest is fully constructed, and initialised.
	 */
	static CExampleInterfaceCreateAndDestroyUnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CExampleInterfaceCreateAndDestroyUnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CExampleInterfaceCreateAndDestroyUnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CExampleInterfaceCreateAndDestroyUnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CExampleInterfaceCreateAndDestroyUnitTest is fully constructed.
		@post			CExampleInterfaceCreateAndDestroyUnitTest is fully destroyed.
	*/
	inline ~CExampleInterfaceCreateAndDestroyUnitTest();

private:
	/**
		@fn				CExampleInterfaceCreateAndDestroyUnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CExampleInterface class.
		@pre 			None.
		@post			CExampleInterfaceCreateAndDestroyUnitTest is fully constructed.
	*/
	inline CExampleInterfaceCreateAndDestroyUnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CExampleInterfaceCreateAndDestroyUnitTest is fully constructed.
		@post			CExampleInterfaceCreateAndDestroyUnitTest is fully initialised.
	*/
	inline void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CExampleInterface class tested by this UintTest's transitions.
	 */
	CExampleInterface_UnitTestContext*		iUTContext;
	TExampleInterface_Ctor_TransitionValidator*		iCtorValidator;
	TExampleInterface_Dtor_TransitionValidator*		iDtorValidator;
	TExampleInterface_StateAccessor*	iStateAccessor;
	REComSession* iEComSession;
	};

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : UnitTest AltCreateAndDestroy on the CExampleInterface test class.
 */
class CExampleInterfaceAltCreateAndDestroyUnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave			KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CExampleInterface class.
		@return			CExampleInterfaceAltCreateAndDestroyUnitTest* The constructed object.
		@pre 			None.
		@post			CExampleInterfaceAltCreateAndDestroyUnitTest is fully constructed, and initialised.
	 */
	static CExampleInterfaceAltCreateAndDestroyUnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CExampleInterfaceAltCreateAndDestroyUnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CExampleInterfaceAltCreateAndDestroyUnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CExampleInterfaceAltCreateAndDestroyUnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CExampleInterfaceAltCreateAndDestroyUnitTest is fully constructed.
		@post			CExampleInterfaceAltCreateAndDestroyUnitTest is fully destroyed.
	*/
	inline ~CExampleInterfaceAltCreateAndDestroyUnitTest();

private:
	/**
		@fn				CExampleInterfaceAltCreateAndDestroyUnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CExampleInterface class.
		@pre 			None.
		@post			CExampleInterfaceAltCreateAndDestroyUnitTest is fully constructed.
	*/
	inline CExampleInterfaceAltCreateAndDestroyUnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CExampleInterfaceAltCreateAndDestroyUnitTest is fully constructed.
		@post			CExampleInterfaceAltCreateAndDestroyUnitTest is fully initialised.
	*/
	inline void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CExampleInterface class tested by this UnitTest's transitions.
	 */
	CExampleInterface_UnitTestContext*		iUTContext;
	TExampleInterface_Ctor_TransitionValidator*		iCtorValidator;
	TExampleInterface_Dtor_TransitionValidator*		iDtorValidator;
	TExampleInterface_StateAccessor*	iStateAccessor;
	REComSession* iEComSession;
	};


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for the ListI mplementations functions on CExampleInterface, 
	the class under test.
 */
class CExampleInterface_ListImplementations_UnitTest : public CUnitTest
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
		@return			CExampleInterface_ListImplementations_UnitTest* The constructed 
						object.
		@pre 			None.
		@post			CExampleInterface_ListImplementations_UnitTest is fully 
						constructed, and initialised.
	 */
	static CExampleInterface_ListImplementations_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CExampleInterface_ListImplementations_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CExampleInterface_ListImplementations_UnitTest is fully 
						constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CExampleInterface_ListImplementations_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CExampleInterface_ListImplementations_UnitTest is 
						fully constructed.
		@post			CExampleInterface_ListImplementations_UnitTest is 
						fully destroyed.
	*/
	~CExampleInterface_ListImplementations_UnitTest();

private:
	/**
		@fn				CExampleInterface_ListImplementations_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CExampleInterface class.
		@pre 			None.
		@post			CExampleInterface_ListImplementations_UnitTest is fully 
						constructed.
	*/
	inline CExampleInterface_ListImplementations_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CExampleInterface_ListImplementations_UnitTest is 
						fully constructed.
		@post			CExampleInterface_ListImplementations_UnitTest is 
						fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CExampleInterface class tested by this UnitTest's transitions.
	 */
	CExampleInterface_UnitTestContext* iUTContext;
	TExampleInterface_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TExampleInterface_Ctor_TransitionValidator*	iCtorValidator;
	TExampleInterface_ListImplementations_TransitionValidator*	iListImplementationsValidator;
	TExampleInterface_Dtor_TransitionValidator*	iDtorValidator;
	REComSession* iEComSession;
	};	// CExampleInterface_ListImplementations_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for DefectHAN4WZHSY on CExampleInterface, the class under test.
 */
class CExampleInterface_DefectHAN4WZHSY_UnitTest : public CUnitTest
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
		@return			CExampleInterface_DefectHAN4WZHSY_UnitTest* The constructed object.
		@pre 			None.
		@post			CExampleInterface_DefectHAN4WZHSY_UnitTest is fully constructed, and initialised.
	 */
	static CExampleInterface_DefectHAN4WZHSY_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CExampleInterface_DefectHAN4WZHSY_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CExampleInterface_DefectHAN4WZHSY_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CExampleInterface_DefectHAN4WZHSY_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CExampleInterface_DefectHAN4WZHSY_UnitTest is fully constructed.
		@post			CExampleInterface_DefectHAN4WZHSY_UnitTest is fully destroyed.
	*/
	~CExampleInterface_DefectHAN4WZHSY_UnitTest();

private:
	/**
		@fn				CExampleInterface_DefectHAN4WZHSY_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CExampleInterface class.
		@pre 			None.
		@post			CExampleInterface_DefectHAN4WZHSY_UnitTest is fully constructed.
	*/
	inline CExampleInterface_DefectHAN4WZHSY_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CExampleInterface_DefectHAN4WZHSY_UnitTest is fully constructed.
		@post			CExampleInterface_DefectHAN4WZHSY_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CExampleInterface class tested by this UnitTest's transitions.
	 */
	CExampleInterface_UnitTestContext* iUTContext;
	TExampleInterface_StateAccessor*	iStateAccessor;

	// C'tor, d'tor, and method transition validators
	TExampleInterface_Ctor_TransitionValidator*	iCtorValidator;
	TExampleInterface_Dtor_TransitionValidator*	iDtorValidator;
	};	// CExampleInterface_DefectHAN4WZHSY_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for DefectCUO4YCEUE on CExampleInterface, the class under test.
 */
class CExampleInterface_DefectCUO4YCEUE_UnitTest : public CUnitTest
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
		@return			CExampleInterface_DefectCUO4YCEUE_UnitTest* The constructed object.
		@pre 			None.
		@post			CExampleInterface_DefectCUO4YCEUE_UnitTest is fully constructed, and initialised.
	 */
	static CExampleInterface_DefectCUO4YCEUE_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CExampleInterface_DefectHAN4WZHSY_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CExampleInterface_DefectCUO4YCEUE_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CExampleInterface_DefectCUO4YCEUE_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CExampleInterface_DefectCUO4YCEUE_UnitTest is fully constructed.
		@post			CExampleInterface_DefectCUO4YCEUE_UnitTest is fully destroyed.
	*/
	~CExampleInterface_DefectCUO4YCEUE_UnitTest();

private:
	/**
		@fn				CExampleInterface_DefectCUO4YCEUE_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CExampleInterface class.
		@pre 			None.
		@post			CExampleInterface_DefectCUO4YCEUE_UnitTest is fully constructed.
	*/
	inline CExampleInterface_DefectCUO4YCEUE_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CExampleInterface_DefectCUO4YCEUE_UnitTest is fully constructed.
		@post			CExampleInterface_DefectCUO4YCEUE_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CExampleInterface class tested by this UnitTest's transitions.
	 */
	CExampleInterface_UnitTestContext* iUTContext;
	TExampleInterface_StateAccessor*	iStateAccessor;

	// C'tor, d'tor, and method transition validators
	TExampleInterface_Ctor_TransitionValidator*	iCtorValidator;
	TExampleInterface_Dtor_TransitionValidator*	iDtorValidator;
	};	// CExampleInterface_DefectCUO4YCEUE_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for DefectEVS-4Z9BPG on CExampleInterface, the class under test.
 */
class CExampleInterface_DefectEVS4Z9BPG_UnitTest : public CUnitTest
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
		@return			CExampleInterface_DefectEVS4Z9BPG_UnitTest* The constructed object.
		@pre 			None.
		@post			CExampleInterface_DefectEVS4Z9BPG_UnitTest is fully constructed, and initialised.
	 */
	static CExampleInterface_DefectEVS4Z9BPG_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CExampleInterface_DefectHAN4WZHSY_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CExampleInterface_DefectEVS4Z9BPG_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CExampleInterface_DefectEVS4Z9BPG_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CExampleInterface_DefectEVS4Z9BPG_UnitTest is fully constructed.
		@post			CExampleInterface_DefectEVS4Z9BPG_UnitTest is fully destroyed.
	*/
	~CExampleInterface_DefectEVS4Z9BPG_UnitTest();

private:
	/**
		@fn				CExampleInterface_DefectEVS4Z9BPG_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CExampleInterface class.
		@pre 			None.
		@post			CExampleInterface_DefectEVS4Z9BPG_UnitTest is fully constructed.
	*/
	inline CExampleInterface_DefectEVS4Z9BPG_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CExampleInterface_DefectEVS4Z9BPG_UnitTest is fully constructed.
		@post			CExampleInterface_DefectEVS4Z9BPG_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CExampleInterface class tested by this UnitTest's transitions.
	 */
	CExampleInterface_UnitTestContext* iUTContext;
	TExampleInterface_StateAccessor*	iStateAccessor;

	// C'tor, d'tor, and method transition validators
	TExampleInterface_Ctor_TransitionValidator*	iCtorValidator;
	TExampleInterface_Default_TransitionValidator*	iDefaultValidator;
	TExampleInterface_Dtor_TransitionValidator*	iDtorValidator;
	};	// CExampleInterface_DefectEVS4Z9BPG_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for DefectKRN-53SL4Q on CExampleInterface, the class under test.
 */
class CExampleInterface_DefectKRN53SL4Q_UnitTest : public CUnitTest
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
		@return			CExampleInterface_DefectKRN53SL4Q_UnitTest* The constructed object.
		@pre 			None.
		@post			CExampleInterface_DefectKRN53SL4Q_UnitTest is fully constructed, and initialised.
	 */
	static CExampleInterface_DefectKRN53SL4Q_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CExampleInterface_DefectKRN53SL4Q_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CExampleInterface_DefectKRN53SL4Q_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CExampleInterface_DefectKRN53SL4Q_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CExampleInterface_DefectKRN53SL4Q_UnitTest is fully constructed.
		@post			CExampleInterface_DefectKRN53SL4Q_UnitTest is fully destroyed.
	*/
	~CExampleInterface_DefectKRN53SL4Q_UnitTest();

private:
	/**
		@fn				CExampleInterface_DefectKRN53SL4Q_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CExampleInterface class.
		@pre 			None.
		@post			CExampleInterface_DefectKRN53SL4Q_UnitTest is fully constructed.
	*/
	inline CExampleInterface_DefectKRN53SL4Q_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CExampleInterface_DefectKRN53SL4Q_UnitTest is fully constructed.
		@post			CExampleInterface_DefectKRN53SL4Q_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CExampleInterface class tested by this UnitTest's transitions.
	 */
	CExampleInterface_UnitTestContext* iUTContext;
	TExampleInterface_StateAccessor*	iStateAccessor;

	// C'tor, d'tor, and method transition validators
	TExampleInterface_Ctor_TransitionValidator*	iCtorValidator;
	TExampleInterface_Default_TransitionValidator*	iDefaultValidator;
	TExampleInterface_Dtor_TransitionValidator*	iDtorValidator;
	};	// CExampleInterface_DefectKRN53SL4Q_UnitTest

#endif
