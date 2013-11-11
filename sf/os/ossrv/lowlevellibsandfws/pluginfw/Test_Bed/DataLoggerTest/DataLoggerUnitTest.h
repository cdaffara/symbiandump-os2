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
// The unit test class definitions for the CDataLogger class.
// 
//

#ifndef __DATALOGGERUNITTEST_H__
#define __DATALOGGERUNITTEST_H__

#include <ecom/test_bed/testbeddefinitions.h>
#include "DataLoggerStateAccessor.h"
#include "DataLoggerTransitionValidation.h"
#include "DataLoggerTransitions.h"

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for CreateAndDestroy on CDataLogger, the class under test.
 */
class CDataLogger_CreateAndDestroy_UnitTest : public CUnitTest
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
		@return			CDataLogger_CreateAndDestroy_UnitTest* The constructed object.
		@pre 			None.
		@post			CDataLogger_CreateAndDestroy_UnitTest is fully constructed, and initialised.
	 */
	static CDataLogger_CreateAndDestroy_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDataLogger_CreateAndDestroy_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDataLogger_CreateAndDestroy_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDataLogger_CreateAndDestroy_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDataLogger_CreateAndDestroy_UnitTest is fully constructed.
		@post			CDataLogger_CreateAndDestroy_UnitTest is fully destroyed.
	*/
	~CDataLogger_CreateAndDestroy_UnitTest();

private:
	/**
		@fn				CDataLogger_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDataLogger class.
		@pre 			None.
		@post			CDataLogger_CreateAndDestroy_UnitTest is fully constructed.
	*/
	inline CDataLogger_CreateAndDestroy_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDataLogger_CreateAndDestroy_UnitTest is fully constructed.
		@post			CDataLogger_CreateAndDestroy_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDataLogger class tested by this UnitTest's transitions.
	 */
	CDataLogger_UnitTestContext* iUTContext;
	TDataLogger_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TDataLogger_Ctor_TransitionValidator*	iCtorValidator;
	TDataLogger_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDataLogger_CreateAndDestroy_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for DumpMemoryBlock on CDataLogger, the class under test.
 */
class CDataLogger_DumpMemoryBlock_UnitTest : public CUnitTest
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
		@return			CDataLogger_DumpMemoryBlock_UnitTest* The constructed object.
		@pre 			None.
		@post			CDataLogger_DumpMemoryBlock_UnitTest is fully constructed, and initialised.
	 */
	static CDataLogger_DumpMemoryBlock_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDataLogger_DumpMemoryBlock_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDataLogger_DumpMemoryBlock_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDataLogger_DumpMemoryBlock_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDataLogger_DumpMemoryBlock_UnitTest is fully constructed.
		@post			CDataLogger_DumpMemoryBlock_UnitTest is fully destroyed.
	*/
	~CDataLogger_DumpMemoryBlock_UnitTest();

private:
	/**
		@fn				CDataLogger_DumpMemoryBlock_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDataLogger class.
		@pre 			None.
		@post			CDataLogger_DumpMemoryBlock_UnitTest is fully constructed.
	*/
	inline CDataLogger_DumpMemoryBlock_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDataLogger_DumpMemoryBlock_UnitTest is fully constructed.
		@post			CDataLogger_DumpMemoryBlock_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDataLogger class tested by this UnitTest's transitions.
	 */
	CDataLogger_UnitTestContext* iUTContext;
	TDataLogger_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TDataLogger_Ctor_TransitionValidator*	iCtorValidator;
	TDataLogger_DumpMemoryBlock_TransitionValidator*	iDumpMemoryBlockValidator;
	TDataLogger_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDataLogger_DumpMemoryBlock_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent

	Comments : Unit Test for LogInformation on CDataLogger, the class under test.
 */
class CDataLogger_LogInformation_UnitTest : public CUnitTest
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
		@return			CDataLogger_LogInformation_UnitTest* The constructed object.
		@pre 			None.
		@post			CDataLogger_LogInformation_UnitTest is fully constructed, and initialised.
	 */
	static CDataLogger_LogInformation_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDataLogger_LogInformation_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDataLogger_LogInformation_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDataLogger_LogInformation_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDataLogger_LogInformation_UnitTest is fully constructed.
		@post			CDataLogger_LogInformation_UnitTest is fully destroyed.
	*/
	~CDataLogger_LogInformation_UnitTest();

private:
	/**
		@fn				CDataLogger_LogInformation_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDataLogger class.
		@pre 			None.
		@post			CDataLogger_LogInformation_UnitTest is fully constructed.
	*/
	inline CDataLogger_LogInformation_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDataLogger_LogInformation_UnitTest is fully constructed.
		@post			CDataLogger_LogInformation_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDataLogger class tested by this UnitTest's transitions.
	 */
	CDataLogger_UnitTestContext* iUTContext;
	TDataLogger_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TDataLogger_Ctor_TransitionValidator*	iCtorValidator;
	TDataLogger_LogInformation_TransitionValidator*	iLogInformationValidator;
	TDataLogger_LogInformationNarrow_TransitionValidator* iLogInformationNarrowValidator;
	TDataLogger_LogInformationWithParameters_TransitionValidator* iLogInformationWithParametersValidator;
	TDataLogger_LogInformationWithParametersNarrow_TransitionValidator* iLogInformationWithParametersNarrowValidator;
	TDataLogger_Dtor_TransitionValidator*	iDtorValidator;
	};	// CDataLogger_LogInformation_UnitTest

// ______________________________________________________________________________
//
/**
	@internalComponent
	
	Comments : Unit Test for ReportInformation on CDataLogger, the class under test.
 */
class CDataLogger_ReportInformation_UnitTest : public CUnitTest
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
		@return			CDataLogger_ReportInformation_UnitTest* The constructed object.
		@pre 			None.
		@post			CDataLogger_ReportInformation_UnitTest is fully constructed, and initialised.
	 */
	static CDataLogger_ReportInformation_UnitTest* NewL(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				RunError(TInt aError)
		Intended Usage	: Intercept the panic caused by a RunL leave,
						to restore the CDataLogger_ReportInformation_UnitTest
						object to a sensible state.
						(called by the Active Scheduler immediately before the Panic).
		Error Condition	: @see CUnitTest::RunError().
		@since			7.0
		@return			TInt KErrNone if cleanup successful, otherwise
						@see CUnitTest::RunError()
		@pre 			CDataLogger_ReportInformation_UnitTest is fully constructed, and initialised.
		@post			The object has been restored to a sensible state.
	 */
	inline TInt RunError(TInt aError);

	/**
		@fn				~CDataLogger_ReportInformation_UnitTest()
		Intended Usage	: Standard Destructor.
		Error Condition	: None.	
		@since			7.0
		@pre 			CDataLogger_ReportInformation_UnitTest is fully constructed.
		@post			CDataLogger_ReportInformation_UnitTest is fully destroyed.
	*/
	~CDataLogger_ReportInformation_UnitTest();

private:
	/**
		@fn				CDataLogger_ReportInformation_UnitTest(CDataLogger& aDataLogger,
														MUnitTestObserver& aObserver)
		Intended Usage	: Default constructor.
		Error Condition	: None. 
		@since			7.0
		@param			aDataLogger The output logging object.
		@param			aObserver The observer of this UnitTest.
		@param			aStateAccessor WhiteBox state access to the CDataLogger class.
		@pre 			None.
		@post			CDataLogger_ReportInformation_UnitTest is fully constructed.
	*/
	inline CDataLogger_ReportInformation_UnitTest(CDataLogger& aDataLogger,
											MUnitTestObserver& aObserver);

	/**
		@fn				void ConstructL()
		Intended Usage	: Second phase of safe two phase construction, 
		to complete the object initialisation.
		Error Condition	: Leaves with an error code.
		@leave  		KErrNoMemory, and @see CUnitTest::BaseNewL().
		@since			7.0
		@return			None 
		@pre 			CDataLogger_ReportInformation_UnitTest is fully constructed.
		@post			CDataLogger_ReportInformation_UnitTest is fully initialised.
	*/
	void ConstructL();

	/**
	The context of the Unit Test.
	i.e The CDataLogger class tested by this UnitTest's transitions.
	 */
	CDataLogger_UnitTestContext* iUTContext;
	TDataLogger_StateAccessor*	iStateAccessor;
	// C'tor, d'tor, and method transition validators
	TDataLogger_Ctor_TransitionValidator*	iCtorValidator;
	TDataLogger_ReportInformation_TransitionValidator*	iReportInformationValidator;
	TDataLogger_ReportInformationWithParameters_TransitionValidator* iReportInformationWithParametersValidator;
	TDataLogger_Dtor_TransitionValidator*	iDtorValidator;

	};	// CDataLogger_ReportInformation_UnitTest


#endif	// __DATALOGGERUNITTEST_H__
