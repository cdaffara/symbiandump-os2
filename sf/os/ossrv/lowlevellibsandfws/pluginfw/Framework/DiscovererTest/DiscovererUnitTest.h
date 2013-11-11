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
// Definition of the CDiscoverer Unit Test classes.
// 
//

#ifndef __DISCOVERERUNITTEST_H__
#define __DISCOVERERUNITTEST_H__


#include <test_bed/testbeddefinitions.h>
#include "DiscovererStateAccessor.h"
#include "DiscovererTransitionValidation.h"
#include "DiscovererTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CreateAndDestroy on CDiscoverer, the class under test.
 */
class CDiscoverer_CreateAndDestroy_UnitTest : public CUnitTest
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
		@return			CDiscoverer_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CDiscoverer_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDiscoverer_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDiscoverer_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDiscoverer_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDiscoverer_CreateAndDestroy_UnitTest is fully constructed.
		@post			CDiscoverer_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CDiscoverer_CreateAndDestroy_UnitTest();

/**
	@fn				PrepareUnitTestL()
	Intended Usage	: Called by test framework just before test is run to allow
					any test preparation to take place.
	Error Condition	: Leaves with an error code.
	@leave			KErrNoMemory
	@since			7.0
	@pre 			CDiscoverer_CreateAndDestroy_UnitTest is fully constructed.
	@post			This unit test is ready to run.
*/
	void PrepareUnitTestL();

private:
	/**
		@fn				CDiscoverer_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class.
		@pre 			None.
		@post			CDiscoverer_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CDiscoverer_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave   		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_CreateAndDestroy_UnitTest is fully constructed.
		@post			CDiscoverer_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDiscoverer class tested by this UnitTest's transitions.
	 */
	CDiscoverer_UnitTestContext*				iUTContext;
	TDiscoverer_Ctor_TransitionValidator*		iCtorValidator;
	TDiscoverer_Dtor_TransitionValidator*		iDtorValidator;
	TDiscoverer_StateAccessor*	iStateAccessor;
	};	// CDiscoverer_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for MountDrive on CDiscoverer, the class under test.
 */
class CDiscoverer_MountDrive_UnitTest : public CUnitTest
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
		@return			CDiscoverer_MountDrive_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_MountDrive_UnitTest is fully constructed, and initialised.
	 */
	static CDiscoverer_MountDrive_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDiscoverer_MountDrive_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDiscoverer_MountDrive_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDiscoverer_MountDrive_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDiscoverer_MountDrive_UnitTest is fully constructed.
		@post			CDiscoverer_MountDrive_UnitTest is fully destroyed.
	*/
	~CDiscoverer_MountDrive_UnitTest();

private:
	/**
		@fn				CDiscoverer_MountDrive_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class.
		@pre 			None.
		@post			CDiscoverer_MountDrive_UnitTest is fully constructed.
	*/
	inline CDiscoverer_MountDrive_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_MountDrive_UnitTest is fully constructed.
		@post			CDiscoverer_MountDrive_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDiscoverer class tested by this UnitTest's transitions.
	 */
	CDiscoverer_UnitTestContext* iUTContext;
	TDiscoverer_StateAccessor*	iStateAccessor;
	TDiscoverer_Ctor_TransitionValidator*	iCtorValidator;
	TDiscoverer_MountDriveL_TransitionValidator*	iMountValidator;
	TDiscoverer_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDiscoverer_MountDrive_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for UnmountDrive on CDiscoverer, the class under test.
 */
class CDiscoverer_UnmountDrive_UnitTest : public CUnitTest
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
		@return			CDiscoverer_UnmountDrive_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_UnmountDrive_UnitTest is fully constructed, and initialised.
	 */
	static CDiscoverer_UnmountDrive_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDiscoverer_UnmountDrive_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDiscoverer_UnmountDrive_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDiscoverer_UnmountDrive_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDiscoverer_UnmountDrive_UnitTest is fully constructed.
		@post			CDiscoverer_UnmountDrive_UnitTest is fully destroyed.
	*/
	~CDiscoverer_UnmountDrive_UnitTest();

private:
	/**
		@fn				CDiscoverer_UnmountDrive_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class.
		@pre 			None.
		@post			CDiscoverer_UnmountDrive_UnitTest is fully constructed.
	*/
	inline CDiscoverer_UnmountDrive_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_UnmountDrive_UnitTest is fully constructed.
		@post			CDiscoverer_UnmountDrive_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDiscoverer class tested by this UnitTest's transitions.
	 */
	CDiscoverer_UnitTestContext* iUTContext;
	TDiscoverer_StateAccessor*	iStateAccessor;
	TDiscoverer_Ctor_TransitionValidator*	iCtorValidator;
	TDiscoverer_UnmountDrive_TransitionValidator*	iUnmountValidator;
	TDiscoverer_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDiscoverer_UnmountDrive_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for IsValidEntryL on CDiscoverer, the class under test.
 */
class CDiscoverer_IsValidEntryL_UnitTest : public CUnitTest
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
		@return			CDiscoverer_IsValidEntryL_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_IsValidEntryL_UnitTest is fully constructed, and initialised.
	 */
	static CDiscoverer_IsValidEntryL_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDiscoverer_IsValidEntryL_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDiscoverer_IsValidEntryL_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDiscoverer_IsValidEntryL_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDiscoverer_IsValidEntryL_UnitTest is fully constructed.
		@post			CDiscoverer_IsValidEntryL_UnitTest is fully destroyed.
	*/
	~CDiscoverer_IsValidEntryL_UnitTest();

/**
	@fn				PrepareUnitTestL()
	Intended Usage	: Called by test framework just before test is run to allow
					any test preparation to take place.
	Error Condition	: 
	@since			7.0
	@pre 			CDiscoverer_CreateAndDestroy_UnitTest is fully constructed.
	@post			This unit test is ready to run.
*/
	void PrepareUnitTestL();

private:
	/**
		@fn				CDiscoverer_IsValidEntryL_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class.
		@pre 			None.
		@post			CDiscoverer_IsValidEntryL_UnitTest is fully constructed.
	*/
	inline CDiscoverer_IsValidEntryL_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_IsValidEntryL_UnitTest is fully constructed.
		@post			CDiscoverer_IsValidEntryL_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDiscoverer class tested by this UnitTest's transitions.
	 */
	CDiscoverer_UnitTestContext* iUTContext;
	TDiscoverer_StateAccessor*	iStateAccessor;
	TDiscoverer_Ctor_TransitionValidator*	iCtorValidator;
	TDiscoverer_IsValidEntry_TransitionValidator*	iIsValidEntryValidator;
	TDiscoverer_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDiscoverer_IsValidEntryL_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for ProcessEntryL on CDiscoverer, the class under test.
 */
class CDiscoverer_ProcessEntryL_UnitTest : public CUnitTest
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
		@return			CDiscoverer_ProcessEntryL_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_ProcessEntryL_UnitTest is fully constructed, and initialised.
	 */
	static CDiscoverer_ProcessEntryL_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDiscoverer_ProcessEntryL_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDiscoverer_ProcessEntryL_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDiscoverer_ProcessEntryL_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDiscoverer_ProcessEntryL_UnitTest is fully constructed.
		@post			CDiscoverer_ProcessEntryL_UnitTest is fully destroyed.
	*/
	~CDiscoverer_ProcessEntryL_UnitTest();

private:
	/**
		@fn				CDiscoverer_ProcessEntryL_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class.
		@pre 			None.
		@post			CDiscoverer_ProcessEntryL_UnitTest is fully constructed.
	*/
	inline CDiscoverer_ProcessEntryL_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_ProcessEntryL_UnitTest is fully constructed.
		@post			CDiscoverer_ProcessEntryL_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDiscoverer class tested by this UnitTest's transitions.
	 */
	CDiscoverer_UnitTestContext* iUTContext;
	TDiscoverer_StateAccessor*	iStateAccessor;
	TDiscoverer_Ctor_TransitionValidator*	iCtorValidator;
	TDiscoverer_ProcessEntryL_TransitionValidator*	iProcessEntryValidator;
	TDiscoverer_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDiscoverer_ProcessEntryL_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CompleteNotificationProcessing on CDiscoverer, the class under test.
 */
class CDiscoverer_CompleteNotificationProcessing_UnitTest : public CUnitTest
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
		@return			CDiscoverer_CompleteNotificationProcessing_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_CompleteNotificationProcessing_UnitTest is fully constructed, and initialised.
	 */
	static CDiscoverer_CompleteNotificationProcessing_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDiscoverer_CompleteNotificationProcessing_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDiscoverer_CompleteNotificationProcessing_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDiscoverer_CompleteNotificationProcessing_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDiscoverer_CompleteNotificationProcessing_UnitTest is fully constructed.
		@post			CDiscoverer_CompleteNotificationProcessing_UnitTest is fully destroyed.
	*/
	~CDiscoverer_CompleteNotificationProcessing_UnitTest();

private:
	/**
		@fn				CDiscoverer_CompleteNotificationProcessing_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class.
		@pre 			None.
		@post			CDiscoverer_CompleteNotificationProcessing_UnitTest is fully constructed.
	*/
	inline CDiscoverer_CompleteNotificationProcessing_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_CompleteNotificationProcessing_UnitTest is fully constructed.
		@post			CDiscoverer_CompleteNotificationProcessing_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDiscoverer class tested by this UnitTest's transitions.
	 */
	CDiscoverer_UnitTestContext* iUTContext;
	TDiscoverer_StateAccessor*	iStateAccessor;
	TDiscoverer_Ctor_TransitionValidator*	iCtorValidator;
	TDiscoverer_CompleteNotificationProcessing_TransitionValidator*	iCompleteNotificationProcessingValidator;
	TDiscoverer_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDiscoverer_CompleteNotificationProcessing_UnitTest


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for Notification on CDiscoverer, the class under test.
 */
class CDiscoverer_Notification_UnitTest : public CUnitTest
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
		@return			CDiscoverer_Notification_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_Notification_UnitTest is fully constructed, and initialised.
	 */
	static CDiscoverer_Notification_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDiscoverer_Notification_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDiscoverer_Notification_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDiscoverer_Notification_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDiscoverer_Notification_UnitTest is fully constructed.
		@post			CDiscoverer_Notification_UnitTest is fully destroyed.
	*/
	~CDiscoverer_Notification_UnitTest();

private:
	/**
		@fn				CDiscoverer_Notification_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class.
		@pre 			None.
		@post			CDiscoverer_Notification_UnitTest is fully constructed.
	*/
	inline CDiscoverer_Notification_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_Notification_UnitTest is fully constructed.
		@post			CDiscoverer_Notification_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDiscoverer class tested by this UnitTest's transitions.
	 */
	CDiscoverer_UnitTestContext* iUTContext;
	TDiscoverer_StateAccessor*	iStateAccessor;
	TDiscoverer_Ctor_TransitionValidator*	iCtorValidator;
	TDiscoverer_Notification_TransitionValidator*	iNotificationValidator;
	TDiscoverer_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDiscoverer_Notification_UnitTest



// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for SuspendResume on CDiscoverer, the class under test.

 */
class CDiscoverer_SuspendResume_UnitTest : public CUnitTest
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
		@return			CDiscoverer_SuspendResume_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_SuspendResume_UnitTest is fully constructed, and initialised.
	 */
	static CDiscoverer_SuspendResume_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDiscoverer_SuspendResume_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDiscoverer_SuspendResume_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDiscoverer_SuspendResume_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDiscoverer_SuspendResume_UnitTest is fully constructed.
		@post			CDiscoverer_SuspendResume_UnitTest is fully destroyed.
	*/
	~CDiscoverer_SuspendResume_UnitTest();

private:
	/**
		@fn				CDiscoverer_SuspendResume_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class.
		@pre 			None.
		@post			CDiscoverer_SuspendResume_UnitTest is fully constructed.
	*/
	inline CDiscoverer_SuspendResume_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_SuspendResume_UnitTest is fully constructed.
		@post			CDiscoverer_SuspendResume_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDiscoverer class tested by this UnitTest's transitions.
	 */
	CDiscoverer_UnitTestContext* iUTContext;
	TDiscoverer_StateAccessor*	iStateAccessor;
	TDiscoverer_Ctor_TransitionValidator*	iCtorValidator;
	TDiscoverer_Notification_TransitionValidator*	iNotificationValidator;
	TDiscoverer_Suspend_TransitionValidator*	iSuspendValidator;
	TDiscoverer_Resume_TransitionValidator*	iResumeValidator;
	TDiscoverer_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDiscoverer_SuspendResume_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for ScanDirectory on CDiscoverer, the class under test.
 */
class CDiscoverer_ScanDirectory_UnitTest : public CUnitTest
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
		@return			CDiscoverer_ScanDirectory_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_ScanDirectory_UnitTest is fully constructed, and initialised.
	 */
	static CDiscoverer_ScanDirectory_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDiscoverer_ScanDirectory_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDiscoverer_ScanDirectory_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDiscoverer_ScanDirectory_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDiscoverer_ScanDirectory_UnitTest is fully constructed.
		@post			CDiscoverer_ScanDirectory_UnitTest is fully destroyed.
	*/
	~CDiscoverer_ScanDirectory_UnitTest();

private:
	/**
		@fn				CDiscoverer_ScanDirectory_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class.
		@pre 			None.
		@post			CDiscoverer_ScanDirectory_UnitTest is fully constructed.
	*/
	inline CDiscoverer_ScanDirectory_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_ScanDirectory_UnitTest is fully constructed.
		@post			CDiscoverer_ScanDirectory_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDiscoverer class tested by this UnitTest's transitions.
	 */
	CDiscoverer_UnitTestContext* iUTContext;
	TDiscoverer_StateAccessor*	iStateAccessor;
	TDiscoverer_Ctor_TransitionValidator*	iCtorValidator;
	TDiscoverer_ScanDirectory_TransitionValidator*	iScanDirectoryValidator;
	TDiscoverer_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDiscoverer_ScanDirectory_UnitTest


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for ScanDirectoryCancel on CDiscoverer, the class under test.

 */
class CDiscoverer_ScanDirectoryCancel_UnitTest : public CUnitTest
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
		@return			CDiscoverer_ScanDirectoryCancel_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_ScanDirectoryCancel_UnitTest is fully constructed, and initialised.
	 */
	static CDiscoverer_ScanDirectoryCancel_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDiscoverer_ScanDirectoryCancel_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDiscoverer_ScanDirectoryCancel_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDiscoverer_ScanDirectoryCancel_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDiscoverer_ScanDirectoryCancel_UnitTest is fully constructed.
		@post			CDiscoverer_ScanDirectoryCancel_UnitTest is fully destroyed.
	*/
	~CDiscoverer_ScanDirectoryCancel_UnitTest();

private:
	/**
		@fn				CDiscoverer_ScanDirectoryCancel_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class.
		@pre 			None.
		@post			CDiscoverer_ScanDirectoryCancel_UnitTest is fully constructed.
	*/
	inline CDiscoverer_ScanDirectoryCancel_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_ScanDirectoryCancel_UnitTest is fully constructed.
		@post			CDiscoverer_ScanDirectoryCancel_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDiscoverer class tested by this UnitTest's transitions.
	 */
	CDiscoverer_UnitTestContext* iUTContext;
	TDiscoverer_StateAccessor*	iStateAccessor;
	TDiscoverer_Ctor_TransitionValidator*	iCtorValidator;
	TDiscoverer_ScanDirectoryCancel_TransitionValidator*	iScanDirectoryCancelValidator;
	TDiscoverer_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDiscoverer_ScanDirectoryCancel_UnitTest


// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for ScanDirectoryIncrement on CDiscoverer, the class under test.
 */
class CDiscoverer_ScanDirectoryIncrement_UnitTest : public CUnitTest
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
		@return			CDiscoverer_ScanDirectoryIncrement_UnitTest* The constructed object.
		@pre 			None.
		@post			CDiscoverer_ScanDirectoryIncrement_UnitTest is fully constructed, and initialised.
	 */
	static CDiscoverer_ScanDirectoryIncrement_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDiscoverer_ScanDirectoryIncrement_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDiscoverer_ScanDirectoryIncrement_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDiscoverer_ScanDirectoryIncrement_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDiscoverer_ScanDirectoryIncrement_UnitTest is fully constructed.
		@post			CDiscoverer_ScanDirectoryIncrement_UnitTest is fully destroyed.
	*/
	~CDiscoverer_ScanDirectoryIncrement_UnitTest();

private:
	/**
		@fn				CDiscoverer_ScanDirectoryIncrement_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDiscoverer class.
		@pre 			None.
		@post			CDiscoverer_ScanDirectoryIncrement_UnitTest is fully constructed.
	*/
	inline CDiscoverer_ScanDirectoryIncrement_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave			KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDiscoverer_ScanDirectoryIncrement_UnitTest is fully constructed.
		@post			CDiscoverer_ScanDirectoryIncrement_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDiscoverer class tested by this UnitTest's transitions.
	 */
	CDiscoverer_UnitTestContext* iUTContext;
	TDiscoverer_StateAccessor*	iStateAccessor;
	TDiscoverer_Ctor_TransitionValidator*	iCtorValidator;
	TDiscoverer_ScanDirectoryIncrement_TransitionValidator*	iScanDirectoryIncrementValidator;
	TDiscoverer_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDiscoverer_ScanDirectoryIncrement_UnitTest

#include "DiscovererUnitTest.inl"

#endif // __DISCOVERERUNITTEST_H__

