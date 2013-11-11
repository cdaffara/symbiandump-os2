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
// The definition of the unit tests for the CRegistrar class
// 
//

#ifndef __REGISTRARUNITTESTS_H__
#define __REGISTRARUNITTESTS_H__

#include "RegistrarTransitionValidation.h"
#include "RegistrarObserver.h"

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for CreateAndDestroy on CRegistrar, the class under test.
 */
class CRegistrar_CreateAndDestroy_UnitTest : public CUnitTest, private MRegistrarObserver
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
		@return			CRegistrar_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CRegistrar_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CRegistrar_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CRegistrar_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CRegistrar_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CRegistrar_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CRegistrar_CreateAndDestroy_UnitTest is fully constructed.
		@post			CRegistrar_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CRegistrar_CreateAndDestroy_UnitTest();

/**
	@fn				PrepareUnitTestL()
	Intended Usage	: Called by test framework just before test is run to allow
					any test preparation to take place.
	Error Condition	: 
	@since			7.0
	@pre 			CRegistrar_CreateAndDestroy_UnitTest is fully constructed.
	@post			This unit test is ready to run.
*/
	void PrepareUnitTestL();

private:
	/**
		@fn				CRegistrar_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CRegistrar class.
		@pre 			None.
		@post			CRegistrar_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CRegistrar_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CRegistrar_CreateAndDestroy_UnitTest is fully constructed.
		@post			CRegistrar_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	void Notification(TInt aNotificationCode);

	/**
	The context of the Unit Test.
	i.e The CRegistrar class tested by this UnitTest's transitions.
	 */
	CRegistrar_UnitTestContext* iUTContext;
	TRegistrar_StateAccessor*	iStateAccessor;

/** Registry data which is required for constructing a CRegistrar */
	CRegistryData*	iRegistryData;

/** C'tor transition validator */
	TRegistrar_Ctor_TransitionValidator*	iCtorValidator;
/** D'tor transition validator */
	TRegistrar_Dtor_TransitionValidator*	iDtorValidator;
	};	// CRegistrar_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent	
	Comments : Unit Test for BeginComplete on CRegistrar, the class under test.
 */
class CRegistrar_BeginComplete_UnitTest : public CUnitTest, private MRegistrarObserver
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
		@return			CRegistrar_BeginComplete_UnitTest* The constructed object.
		@pre 			None.
		@post			CRegistrar_BeginComplete_UnitTest is fully constructed, and initialised.
	 */
	static CRegistrar_BeginComplete_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CRegistrar_BeginComplete_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CRegistrar_BeginComplete_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CRegistrar_BeginComplete_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CRegistrar_BeginComplete_UnitTest is fully constructed.
		@post			CRegistrar_BeginComplete_UnitTest is fully destroyed.
	*/
	~CRegistrar_BeginComplete_UnitTest();

private:
	/**
		@fn				CRegistrar_BeginComplete_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CRegistrar class.
		@pre 			None.
		@post			CRegistrar_BeginComplete_UnitTest is fully constructed.
	*/
	inline CRegistrar_BeginComplete_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CRegistrar_BeginComplete_UnitTest is fully constructed.
		@post			CRegistrar_BeginComplete_UnitTest is fully initialised.
	*/
	void ConstructL();

	void Notification(TInt aNotificationCode);

	/**
	The context of the Unit Test.
	i.e The CRegistrar class tested by this UnitTest's transitions.
	 */
	CRegistrar_UnitTestContext* iUTContext;
	TRegistrar_StateAccessor*	iStateAccessor;

/** Registry data which is required for constructing a CRegistrar */
	CRegistryData*	iRegistryData;

	// C'tor, d'tor, and default transition validators
	TRegistrar_Ctor_TransitionValidator*	iCtorValidator;
	TRegistrar_Default_TransitionValidator*	iDefaultValidator;
	TRegistrar_Dtor_TransitionValidator*	iDtorValidator;
	};	// CRegistrar_BeginComplete_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for RegisterDiscovery on CRegistrar, the class under test.
 */
class CRegistrar_RegisterDiscovery_UnitTest : public CUnitTest, private MRegistrarObserver
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
		@return			CRegistrar_RegisterDiscovery_UnitTest* The constructed object.
		@pre 			None.
		@post			CRegistrar_RegisterDiscovery_UnitTest is fully constructed, and initialised.
	 */
	static CRegistrar_RegisterDiscovery_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CRegistrar_RegisterDiscovery_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CRegistrar_RegisterDiscovery_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CRegistrar_RegisterDiscovery_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CRegistrar_RegisterDiscovery_UnitTest is fully constructed.
		@post			CRegistrar_RegisterDiscovery_UnitTest is fully destroyed.
	*/
	~CRegistrar_RegisterDiscovery_UnitTest();

private:
	/**
		@fn				CRegistrar_RegisterDiscovery_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CRegistrar class.
		@pre 			None.
		@post			CRegistrar_RegisterDiscovery_UnitTest is fully constructed.
	*/
	inline CRegistrar_RegisterDiscovery_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CRegistrar_RegisterDiscovery_UnitTest is fully constructed.
		@post			CRegistrar_RegisterDiscovery_UnitTest is fully initialised.
	*/
	void ConstructL();

	void Notification(TInt aNotificationCode);

	/**
	The context of the Unit Test.
	i.e The CRegistrar class tested by this UnitTest's transitions.
	 */
	CRegistrar_UnitTestContext* iUTContext;
	TRegistrar_StateAccessor*	iStateAccessor;

/** Registry data which is required for constructing a CRegistrar */
	CRegistryData*	iRegistryData;

	// C'tor, d'tor, and method transition validators
	TRegistrar_Ctor_TransitionValidator*				iCtorValidator;
	TRegistrar_RegisterDiscoveryL_TransitionValidator*	iRegisterDiscoveryValidator;
	TRegistrar_Dtor_TransitionValidator*				iDtorValidator;
	};	// CRegistrar_RegisterDiscovery_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for RegisterDiscovery on CRegistrar, the class under test.
 */
class CRegistrar_RegisterTwice_UnitTest : public CUnitTest, private MRegistrarObserver
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
		@return			CRegistrar_RegisterTwice_UnitTest* The constructed object.
		@pre 			None.
		@post			CRegistrar_RegisterTwice_UnitTest is fully constructed, and initialised.
	 */
	static CRegistrar_RegisterTwice_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CRegistrar_RegisterTwice_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CRegistrar_RegisterTwice_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CRegistrar_RegisterTwice_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CRegistrar_RegisterTwice_UnitTest is fully constructed.
		@post			CRegistrar_RegisterTwice_UnitTest is fully destroyed.
	*/
	~CRegistrar_RegisterTwice_UnitTest();

private:
	/**
		@fn				CRegistrar_RegisterTwice_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CRegistrar class.
		@pre 			None.
		@post			CRegistrar_RegisterTwice_UnitTest is fully constructed.
	*/
	inline CRegistrar_RegisterTwice_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CRegistrar_RegisterTwice_UnitTest is fully constructed.
		@post			CRegistrar_RegisterTwice_UnitTest is fully initialised.
	*/
	void ConstructL();

	void Notification(TInt aNotificationCode);

	/**
	The context of the Unit Test.
	i.e The CRegistrar class tested by this UnitTest's transitions.
	 */
	CRegistrar_UnitTestContext* iUTContext;
	TRegistrar_StateAccessor*	iStateAccessor;

/** Registry data which is required for constructing a CRegistrar */
	CRegistryData*	iRegistryData;

	// C'tor, d'tor, and method transition validators
	TRegistrar_Ctor_TransitionValidator*				iCtorValidator;
	TRegistrar_RegisterDiscoveryL_TransitionValidator*	iRegisterDiscoveryValidator;
	TRegistrar_Default_TransitionValidator*				iDefaultValidator;
	TRegistrar_Dtor_TransitionValidator*				iDtorValidator;
	};	// CRegistrar_RegisterTwice_UnitTest


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for RegisterDiscovery on CRegistrar, the class under test.
 */
class CRegistrar_UseWhileDiscovering_UnitTest : public CUnitTest, private MRegistrarObserver
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
		@return			CRegistrar_UseWhileDiscovering_UnitTest* The constructed object.
		@pre 			None.
		@post			CRegistrar_UseWhileDiscovering_UnitTest is fully constructed, and initialised.
	 */
	static CRegistrar_UseWhileDiscovering_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CRegistrar_UseWhileDiscovering_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CRegistrar_UseWhileDiscovering_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CRegistrar_UseWhileDiscovering_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CRegistrar_UseWhileDiscovering_UnitTest is fully constructed.
		@post			CRegistrar_UseWhileDiscovering_UnitTest is fully destroyed.
	*/
	~CRegistrar_UseWhileDiscovering_UnitTest();

private:
	/**
		@fn				CRegistrar_UseWhileDiscovering_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CRegistrar class.
		@pre 			None.
		@post			CRegistrar_UseWhileDiscovering_UnitTest is fully constructed.
	*/
	inline CRegistrar_UseWhileDiscovering_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CRegistrar_UseWhileDiscovering_UnitTest is fully constructed.
		@post			CRegistrar_UseWhileDiscovering_UnitTest is fully initialised.
	*/
	void ConstructL();

	void Notification(TInt aNotificationCode);

	/**
	The context of the Unit Test.
	i.e The CRegistrar class tested by this UnitTest's transitions.
	 */
	CRegistrar_UnitTestContext* iUTContext;
	TRegistrar_StateAccessor*	iStateAccessor;

/** Registry data which is required for constructing a CRegistrar */
	CRegistryData*	iRegistryData;

	// C'tor, d'tor, and method transition validators
	TRegistrar_Ctor_TransitionValidator*				iCtorValidator;
	TRegistrar_CheckRegWhileDiscovering_TransitionValidator*	iCheckRegValidator;
	TRegistrar_Dtor_TransitionValidator*				iDtorValidator;
	};	// CRegistrar_UseWhileDiscovering_UnitTest

#include "RegistrarUnitTests.inl"

#endif		// __REGISTRARUNITTESTS_H__
