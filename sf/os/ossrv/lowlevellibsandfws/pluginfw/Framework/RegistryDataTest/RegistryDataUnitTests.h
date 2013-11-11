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
// The definition of the unit stet classes for the CRegistryData testing.
// 
//

#ifndef __REGISTRYDATAUNITTESTS_H__
#define __REGISTRTDATAUNITTESTS_H__

#include "RegistryDataTransitionValidation.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CreateAndDestroy on CRegistryData, the class under test.
 */
class CRegistryData_CreateAndDestroy_UnitTest : public CUnitTest
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
		@return			CRegistryData_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CRegistryData_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CRegistryData_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CRegistryData_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CRegistryData_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CRegistryData_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CRegistryData_CreateAndDestroy_UnitTest is fully constructed.
		@post			CRegistryData_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CRegistryData_CreateAndDestroy_UnitTest();

/**
	@fn				PrepareUnitTestL()
	Intended Usage	: Called by test framework just before test is run to allow
					any test preparation to take place.
	Error Condition	: 
	@since			7.0
	@pre 			CRegistryData_CreateAndDestroy_UnitTest is fully constructed.
	@post			This unit test is ready to run.
*/
	void PrepareUnitTestL();

private:
	/**
		@fn				CRegistryData_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CRegistryData class.
		@pre 			None.
		@post			CRegistryData_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CRegistryData_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CRegistryData_CreateAndDestroy_UnitTest is fully constructed.
		@post			CRegistryData_CreateAndDestroy_UnitTest is fully initialised.
	*/
	inline void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CRegistryData class tested by this UnitTest's transitions.
	 */
	CRegistryData_UnitTestContext* iUTContext;
	TRegistryData_Ctor_TransitionValidator*		iCtorValidator;
	TRegistryData_Dtor_TransitionValidator*		iDtorValidator;
	TRegistryData_StateAccessor*	iStateAccessor;
	};	// CRegistryData_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for AddData on CRegistryData, the class under test.
 */
class CRegistryData_AddData_UnitTest : public CUnitTest
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
		@return			CRegistryData_AddData_UnitTest* The constructed object.
		@pre 			None.
		@post			CRegistryData_AddData_UnitTest is fully constructed, and initialised.
	 */
	static CRegistryData_AddData_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CRegistryData_AddData_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CRegistryData_AddData_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CRegistryData_AddData_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CRegistryData_AddData_UnitTest is fully constructed.
		@post			CRegistryData_AddData_UnitTest is fully destroyed.
	*/
	~CRegistryData_AddData_UnitTest();

private:
	/**
		@fn				CRegistryData_AddData_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CRegistryData class.
		@pre 			None.
		@post			CRegistryData_AddData_UnitTest is fully constructed.
	*/
	inline CRegistryData_AddData_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CRegistryData_AddData_UnitTest is fully constructed.
		@post			CRegistryData_AddData_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CRegistryData class tested by this UnitTest's transitions.
	 */
	CRegistryData_UnitTestContext*	iUTContext;
	TRegistryData_StateAccessor*	iStateAccessor;

	TRegistryData_Ctor_TransitionValidator*						iCtorValidator;
	TRegistryData_SetPolicyObserver_TransitionValidator*		iSetObserverValidator;
	TRegistryData_UndoTemporaryUninstall_TransitionValidator*	iUndoTemporaryUninstallValidator;
	TRegistryData_AddDllDataL_TransitionValidator*				iAddDllDataValidator;
	TRegistryData_IsRegisteredWithDate_TransitionValidator*		iIsRegisteredValidator;
	TRegistryData_UpdateDllData_TransitionValidator*			iUpdateDllDataValidator;
	TRegistryData_TemporaryUninstall_TransitionValidator*		iTemporaryUninstallValidator;
	TRegistryData_PersistNow_TransitionValidator*				iPersistNowValidator;
	TRegistryData_Dtor_TransitionValidator*						iDtorValidator;
	};	// CRegistryData_AddData_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for RetrieveInfo on CRegistryData, the class under test.
 */
class CRegistryData_RetrieveInfo_UnitTest : public CUnitTest
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
		@return			CRegistryData_RetrieveInfo_UnitTest* The constructed object.
		@pre 			None.
		@post			CRegistryData_RetrieveInfo_UnitTest is fully constructed, and initialised.
	 */
	static CRegistryData_RetrieveInfo_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CRegistryData_RetrieveInfo_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CRegistryData_RetrieveInfo_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CRegistryData_RetrieveInfo_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CRegistryData_RetrieveInfo_UnitTest is fully constructed.
		@post			CRegistryData_RetrieveInfo_UnitTest is fully destroyed.
	*/
	~CRegistryData_RetrieveInfo_UnitTest();

private:
	/**
		@fn				CRegistryData_RetrieveInfo_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CRegistryData class.
		@pre 			None.
		@post			CRegistryData_RetrieveInfo_UnitTest is fully constructed.
	*/
	inline CRegistryData_RetrieveInfo_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CRegistryData_RetrieveInfo_UnitTest is fully constructed.
		@post			CRegistryData_RetrieveInfo_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CRegistryData class tested by this UnitTest's transitions.
	 */
	CRegistryData_UnitTestContext*	iUTContext;
	TRegistryData_StateAccessor*	iStateAccessor;

	// C'tor, d'tor, and default transition validators
	TRegistryData_Ctor_TransitionValidator*		iCtorValidator;
	TRegistryData_ListImplementations_TransitionValidator*	iListImplementationsValidator;
	TRegistryData_GetImplementationInformation_TransitionValidator* iGetImplementationInformationValidator;
	TRegistryData_Dtor_TransitionValidator*		iDtorValidator;
	};	// CRegistryData_RetrieveInfo_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
 
	Comments : Unit Test for ConstructIndex on CRegistryData, the class under test.
 */
class CRegistryData_GetImplInfoWhileDiscovering_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger, MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave  		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CRegistryData_GetImplInfoWhileDiscovering_UnitTest* The 
						constructed object.
		@pre 			None.
		@post			CRegistryData_GetImplInfoWhileDiscovering_UnitTest is fully 
						constructed, and initialised.
	 */
	static CRegistryData_GetImplInfoWhileDiscovering_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CRegistryData_ConstructIndex_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CRegistryData_GetImplInfoWhileDiscovering_UnitTest is fully 
						constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CRegistryData_GetImplInfoWhileDiscovering_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CRegistryData_GetImplInfoWhileDiscovering_UnitTest is fully constructed.
		@post			CRegistryData_GetImplInfoWhileDiscovering_UnitTest is fully destroyed.
	*/
	~CRegistryData_GetImplInfoWhileDiscovering_UnitTest();

private:
	/**
		@fn				CRegistryData_GetImplInfoWhileDiscovering_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CRegistryData class.
		@pre 			None.
		@post			CRegistryData_GetImplInfoWhileDiscovering_UnitTest is fully constructed.
	*/
	inline CRegistryData_GetImplInfoWhileDiscovering_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CRegistryData_GetImplInfoWhileDiscovering_UnitTest is fully constructed.
		@post			CRegistryData_GetImplInfoWhileDiscovering_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CRegistryData class tested by this UnitTest's transitions.
	 */
	CRegistryData_UnitTestContext*	iUTContext;
	TRegistryData_StateAccessor*	iStateAccessor;

	// C'tor, d'tor, and method transition validators
	TRegistryData_Ctor_TransitionValidator*	iCtorValidator;
	TRegistryData_GetImplementationInformation_TransitionValidator*	iGetImplInfoValidator;
	TRegistryData_DiscoveriesBeginning_TransitionValidator* iDiscoveriesBeginningValidator;
	TRegistryData_GetSecondImplInfo_TransitionValidator*	iGetSecondImplInfoValidator;
	TRegistryData_Dtor_TransitionValidator*	iDtorValidator;

	};	// CRegistryData_GetImplInfoWhileDiscovering_UnitTest

#include "RegistryDataUnitTests.inl"

#endif		// __REGISTRYDATAUNITTESTS_H__
