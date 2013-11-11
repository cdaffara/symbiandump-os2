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
// The unit test class definitions for the RStringPool class.
// 
//

#ifndef __STRINGPOOLUNITTEST_H__
#define __STRINGPOOLUNITTEST_H__

#include <ecom/test_bed/testbeddefinitions.h> 
#include "StringPoolStateAccessor.h"
#include "StringPoolTransitionValidation.h"
#include "StringPoolTransitions.h"
#include <stringpool.h>


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for CaseInsensitive on StringPool, the class under test.
 */
class CStringPool_CaseInsensitive_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave		KErrNoMemory
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CStringPool_CaseInsensitive_UnitTest* The constructed object.
		@pre 			None.
		@post			CStringPool_CaseInsensitive_UnitTest is fully constructed, and initialised.
	 */
	static CStringPool_CaseInsensitive_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CStringPool_CaseInsensitive_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			6.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CStringPool_CaseInsensitive_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CStringPool_CaseInsensitive_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			6.0
		@pre 			CStringPool_CaseInsensitive_UnitTest is fully constructed.
		@post			CStringPool_CaseInsensitive_UnitTest is fully destroyed.
	*/
	~CStringPool_CaseInsensitive_UnitTest();

private:
	/**
		@fn				CStringPool_CaseInsensitive_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the StringPool class.
		@pre 			None.
		@post			CStringPool_CaseInsensitive_UnitTest is fully constructed.
	*/
	inline CStringPool_CaseInsensitive_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			6.0
		@return			None 
		@pre 			CStringPool_CaseInsensitive_UnitTest is fully constructed.
		@post			CStringPool_CaseInsensitive_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The StringPool class tested by this UnitTest's transitions.
	 */
	CStringPool_UnitTestContext* iUTContext;
	TStringPool_StateAccessor*	iStateAccessor;
	TStringPool_True_TransitionValidator*	iTrueValidator;
	TStringPool_False_TransitionValidator*	iFalseValidator;
	};	// CStringPool_CaseInsensitive_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for SmallTable on StringPool, the class under test.
 */
class CStringPool_ShortTable_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave		KErrNoMemory
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CStringPool_ShortTable_UnitTest* The constructed object.
		@pre 			None.
		@post			CStringPool_ShortTable_UnitTest is fully constructed, and initialised.
	 */
	static CStringPool_ShortTable_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CStringPool_ShortTable_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			6.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CStringPool_ShortTable_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CStringPool_ShortTable_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			6.0
		@pre 			CStringPool_ShortTable_UnitTest is fully constructed.
		@post			CStringPool_ShortTable_UnitTest is fully destroyed.
	*/
	~CStringPool_ShortTable_UnitTest();

private:
	/**
		@fn				CStringPool_ShortTable_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the StringPool class.
		@pre 			None.
		@post			CStringPool_ShortTable_UnitTest is fully constructed.
	*/
	inline CStringPool_ShortTable_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			6.0
		@return			None 
		@pre 			CStringPool_ShortTable_UnitTest is fully constructed.
		@post			CStringPool_ShortTable_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The StringPool class tested by this UnitTest's transitions.
	 */
	CStringPool_UnitTestContext* iUTContext;
	TStringPool_StateAccessor*	iStateAccessor;
	TStringPool_True_TransitionValidator*	iTrueValidator;
	TStringPool_False_TransitionValidator*	iFalseValidator;
	};	// CStringPool_ShortTable_UnitTest




// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for IrrelevantTable on StringPool, the class under test.
 */
class CStringPool_IrrelevantTable_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave		KErrNoMemory
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CStringPool_IrrelevantTable_UnitTest* The constructed object.
		@pre 			None.
		@post			CStringPool_IrrelevantTable_UnitTest is fully constructed, and initialised.
	 */
	static CStringPool_IrrelevantTable_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CStringPool_IrrelevantTable_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			6.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CStringPool_IrrelevantTable_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CStringPool_IrrelevantTable_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			6.0
		@pre 			CStringPool_IrrelevantTable_UnitTest is fully constructed.
		@post			CStringPool_IrrelevantTable_UnitTest is fully destroyed.
	*/
	~CStringPool_IrrelevantTable_UnitTest();

private:
	/**
		@fn				CStringPool_IrrelevantTable_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the StringPool class.
		@pre 			None.
		@post			CStringPool_IrrelevantTable_UnitTest is fully constructed.
	*/
	inline CStringPool_IrrelevantTable_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			6.0
		@return			None 
		@pre 			CStringPool_IrrelevantTable_UnitTest is fully constructed.
		@post			CStringPool_IrrelevantTable_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The StringPool class tested by this UnitTest's transitions.
	 */
	CStringPool_UnitTestContext* iUTContext;
	TStringPool_StateAccessor*	iStateAccessor;
	TStringPool_True_TransitionValidator*	iTrueValidator;
	TStringPool_False_TransitionValidator*	iFalseValidator;
	};	// CStringPool_IrrelevantTable_UnitTest


// ______________________________________________________________________________
//
/**
	@internalComponent
	Comments : Unit Test for LargeTable on StringPool, the class under test.
 */
class CStringPool_LongTable_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave		KErrNoMemory
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CStringPool_LongTable_UnitTest* The constructed object.
		@pre 			None.
		@post			CStringPool_LongTable_UnitTest is fully constructed, and initialised.
	 */
	static CStringPool_LongTable_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CStringPool_LargeTable_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			6.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CStringPool_LongTable_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CStringPool_LongTable_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			6.0
		@pre 			CStringPool_LongTable_UnitTest is fully constructed.
		@post			CStringPool_LongTable_UnitTest is fully destroyed.
	*/
	~CStringPool_LongTable_UnitTest();

private:
	/**
		@fn				CStringPool_LongTable_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			6.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the StringPool class.
		@pre 			None.
		@post			CStringPool_LongTable_UnitTest is fully constructed.
	*/
	inline CStringPool_LongTable_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			6.0
		@return			None 
		@pre 			CStringPool_LongTable_UnitTest is fully constructed.
		@post			CStringPool_LongTable_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The StringPool class tested by this UnitTest's transitions.
	 */
	CStringPool_UnitTestContext* iUTContext;
	TStringPool_StateAccessor*	iStateAccessor;
	TStringPool_True_TransitionValidator*	iTrueValidator;
	TStringPool_False_TransitionValidator*	iFalseValidator;
	};	// CStringPool_LongTable_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CaseSensitive on CStringPool, the class under test.
 */
class CStringPool_CaseSensitive_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CStringPool_CaseSensitive_UnitTest* The constructed object.
		@pre 			None.
		@post			CStringPool_CaseSensitive_UnitTest is fully constructed, and initialized.
	 */
	static CStringPool_CaseSensitive_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CStringPool_CaseSensitive_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CStringPool_CaseSensitive_UnitTest is fully constructed, and initialized.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CStringPool_CaseSensitive_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CStringPool_CaseSensitive_UnitTest is fully constructed.
		@post			CStringPool_CaseSensitive_UnitTest is fully destroyed.
	*/
	~CStringPool_CaseSensitive_UnitTest();

private:
	/**
		@fn				CStringPool_CaseSensitive_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CStringPool class.
		@pre 			None.
		@post			CStringPool_CaseSensitive_UnitTest is fully constructed.
	*/
	inline CStringPool_CaseSensitive_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CStringPool_CaseSensitive_UnitTest is fully constructed.
		@post			CStringPool_CaseSensitive_UnitTest is fully initialized.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CStringPool class tested by this UnitTest's transitions.
	 */
	CStringPool_UnitTestContext* iUTContext;
	TStringPool_StateAccessor*	iStateAccessor;
	TStringPool_True_TransitionValidator*	iTrueValidator;
	TStringPool_False_TransitionValidator*	iFalseValidator;
	};	// CStringPool_CaseSensitive_UnitTest

// ______________________________________________________________________________
//
/**
	@internalAll

	Comments : Unit Test for SmallTableCS on CStringPool, the class under test.
 */
class CStringPool_ShortTableCS_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CStringPool_ShortTableCS_UnitTest* The constructed object.
		@pre 			None.
		@post			CStringPool_ShortTableCS_UnitTest is fully constructed, and initialized.
	 */
	static CStringPool_ShortTableCS_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CStringPool_ShortTableCS_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CStringPool_ShortTableCS_UnitTest is fully constructed, and initialized.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CStringPool_ShortTableCS_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CStringPool_ShortTableCS_UnitTest is fully constructed.
		@post			CStringPool_ShortTableCS_UnitTest is fully destroyed.
	*/
	~CStringPool_ShortTableCS_UnitTest();

private:
	/**
		@fn				CStringPool_ShortTableCS_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CStringPool class.
		@pre 			None.
		@post			CStringPool_ShortTableCS_UnitTest is fully constructed.
	*/
	inline CStringPool_ShortTableCS_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CStringPool_ShortTableCS_UnitTest is fully constructed.
		@post			CStringPool_ShortTableCS_UnitTest is fully initialized.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CStringPool class tested by this UnitTest's transitions.
	 */
	CStringPool_UnitTestContext* iUTContext;
	TStringPool_StateAccessor*	iStateAccessor;
	TStringPool_True_TransitionValidator*	iTrueValidator;
	TStringPool_False_TransitionValidator*	iFalseValidator;
	};	// CStringPool_ShortTableCS_UnitTest

// ______________________________________________________________________________
//
/**
	@internalAll

	Comments : Unit Test for IrrelevantTableCS on CStringPool, the class under test.
 */
class CStringPool_IrrelevantTableCS_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CStringPool_IrrelevantTableCS_UnitTest* The constructed object.
		@pre 			None.
		@post			CStringPool_IrrelevantTableCS_UnitTest is fully constructed, and initialized.
	 */
	static CStringPool_IrrelevantTableCS_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CStringPool_IrrelevantTableCS_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CStringPool_IrrelevantTableCS_UnitTest is fully constructed, and initialized.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CStringPool_IrrelevantTableCS_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CStringPool_IrrelevantTableCS_UnitTest is fully constructed.
		@post			CStringPool_IrrelevantTableCS_UnitTest is fully destroyed.
	*/
	~CStringPool_IrrelevantTableCS_UnitTest();

private:
	/**
		@fn				CStringPool_IrrelevantTableCS_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CStringPool class.
		@pre 			None.
		@post			CStringPool_IrrelevantTableCS_UnitTest is fully constructed.
	*/
	inline CStringPool_IrrelevantTableCS_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CStringPool_IrrelevantTableCS_UnitTest is fully constructed.
		@post			CStringPool_IrrelevantTableCS_UnitTest is fully initialized.
	*/
	void ConstructL();

	CStringPool_UnitTestContext* iUTContext;
	TStringPool_StateAccessor*	iStateAccessor;
	TStringPool_True_TransitionValidator*	iTrueValidator;
	TStringPool_False_TransitionValidator*	iFalseValidator;
	};	// CStringPool_IrrelevantTableCS_UnitTest

// ______________________________________________________________________________
//
/**
	@internalAll

	Comments : Unit Test for LargeCS on CStringPool, the class under test.
 */
class CStringPool_LongTableCS_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CStringPool_LongTableCS_UnitTest* The constructed object.
		@pre 			None.
		@post			CStringPool_LongTableCS_UnitTest is fully constructed, and initialized.
	 */
	static CStringPool_LongTableCS_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CStringPool_LongTableCS_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CStringPool_LongTableCS_UnitTest is fully constructed, and initialized.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CStringPool_LongTableCS_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CStringPool_LongTableCS_UnitTest is fully constructed.
		@post			CStringPool_LongTableCS_UnitTest is fully destroyed.
	*/
	~CStringPool_LongTableCS_UnitTest();

private:
	/**
		@fn				CStringPool_LongTableCS_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CStringPool class.
		@pre 			None.
		@post			CStringPool_LongTableCS_UnitTest is fully constructed.
	*/
	inline CStringPool_LongTableCS_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CStringPool_LongTableCS_UnitTest is fully constructed.
		@post			CStringPool_LongTableCS_UnitTest is fully initialized.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CStringPool class tested by this UnitTest's transitions.
	 */
	CStringPool_UnitTestContext* iUTContext;
	TStringPool_StateAccessor*	iStateAccessor;
	TStringPool_True_TransitionValidator*	iTrueValidator;
	TStringPool_False_TransitionValidator*	iFalseValidator;
	};	// CStringPool_LongTableCS_UnitTest

// ______________________________________________________________________________
//
/**
	@internalAll

	Comments : Unit Test for MultipleTableShortCI on RStringPool, the class under test.
 */
class CStringPool_MultipleTableShortCI_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CStringPool_MultipleTableShortCI_UnitTest* The constructed object.
		@pre 			None.
		@post			CStringPool_MultipleTableShortCI_UnitTest is fully constructed, and initialized.
	 */
	static CStringPool_MultipleTableShortCI_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CStringPool_MultipleTableShortCI_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CStringPool_MultipleTableShortCI_UnitTest is fully constructed, and initialized.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CStringPool_MultipleTableShortCI_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CStringPool_MultipleTableShortCI_UnitTest is fully constructed.
		@post			CStringPool_MultipleTableShortCI_UnitTest is fully destroyed.
	*/
	~CStringPool_MultipleTableShortCI_UnitTest();

private:
	/**
		@fn				CStringPool_MultipleTableShortCI_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the RStringPool class.
		@pre 			None.
		@post			CStringPool_MultipleTableShortCI_UnitTest is fully constructed.
	*/
	inline CStringPool_MultipleTableShortCI_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CStringPool_MultipleTableShortCI_UnitTest is fully constructed.
		@post			CStringPool_MultipleTableShortCI_UnitTest is fully initialized.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The RStringPool class tested by this UnitTest's transitions.
	 */
	CStringPool_UnitTestContext* iUTContext;
	TStringPool_StateAccessor*	iStateAccessor;
	TStringPool_True_TransitionValidator*	iTrueValidator;
	TStringPool_False_TransitionValidator*	iFalseValidator;
	};	// CStringPool_MultipleTableShortCI_UnitTest


// ______________________________________________________________________________
//
/**
	@internalAll

	Comments : Unit Test for MultipleTableShortCS on RStringPool, the class under test.
 */
class CStringPool_MultipleTableShortCS_UnitTest : public CUnitTest
	{
public:
	/**
		@fn				NewL(CDataLogger& aDataLogger,
							MUnitTestObserver& aObserver)
		Intended Usage	: Standard two-phase construction which leaves nothing on the
						cleanup stack.
		Error Condition	: Leaves with the error code.
		@leave		KErrNoMemory
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@return			CStringPool_MultipleTableShortCS_UnitTest* The constructed object.
		@pre 			None.
		@post			CStringPool_MultipleTableShortCS_UnitTest is fully constructed, and initialized.
	 */
	static CStringPool_MultipleTableShortCS_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CStringPool_MultipleTableShortCS_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CStringPool_MultipleTableShortCS_UnitTest is fully constructed, and initialized.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CStringPool_MultipleTableShortCS_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CStringPool_MultipleTableShortCS_UnitTest is fully constructed.
		@post			CStringPool_MultipleTableShortCS_UnitTest is fully destroyed.
	*/
	~CStringPool_MultipleTableShortCS_UnitTest();

private:
	/**
		@fn				CStringPool_MultipleTableShortCS_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the RStringPool class.
		@pre 			None.
		@post			CStringPool_MultipleTableShortCS_UnitTest is fully constructed.
	*/
	inline CStringPool_MultipleTableShortCS_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CStringPool_MultipleTableShortCS_UnitTest is fully constructed.
		@post			CStringPool_MultipleTableShortCS_UnitTest is fully initialized.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The RStringPool class tested by this UnitTest's transitions.
	 */
	CStringPool_UnitTestContext* iUTContext;
	TStringPool_StateAccessor*	iStateAccessor;
	TStringPool_True_TransitionValidator*	iTrueValidator;
	TStringPool_False_TransitionValidator*	iFalseValidator;
	};	// CStringPool_MultipleTableShortCS_UnitTest


#endif	// __STRINGPOOLUNITTEST_H__
