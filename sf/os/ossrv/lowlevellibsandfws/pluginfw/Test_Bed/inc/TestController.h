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
// This file contains the definition of the class CTestController
// This file comment is for DOxygen only and is ignored by EDoc.
// 
//

/**
 @test
*/

#ifndef __TESTCONTROLLER_H__
#define __TESTCONTROLLER_H__

#include <e32base.h>
#include <e32test.h>

#include <ecom/test_bed/componentinfo.h>
#include <ecom/test_bed/testmanager.h>
#include <ecom/test_bed/transitionobserver.h>
#include <ecom/test_bed/transition.h>
#include <ecom/test_bed/managerobserver.h>

class CComponentTester;
class CDataLogger;

// Global function typedef declaration of function provided by a test module of the component
// under test and is used as an entry point to kick start test bed. Provided as argument to CTestController.
typedef CComponentTester* (*ComponentTesterInitialiserLC)(CDataLogger&, MComponentTestObserver&);

/**
	@internalAll
	Comments : Manages the whole test procedure.
 */

class CTestController : public CBase, private MManagerObserver
	{
public:
/**
	@fn				static IMPORT_C CTestController* NewL(CActiveScheduler* aScheduler,
														  ComponentTesterInitialiserLC aEntryPoint,
														  RTest* aRTest = NULL,
														  TLoggingInfo*		aLogInfo = NULL)
	Intended Usage	: Standardized safe construction which leaves nothing on the cleanup stack.
	@leave  		KErrNoMemory.
	@since			7.0
	@param 			aScheduler The active scheduler to use,  pass NULL if no scheduler exists
	@param			aEntryPoint The global function used to create the derived CComponentTester object.
	@param			aRTest Optional RTest object to use in check unit test results
	@param			aLogInfo The logging info to use, defaults to NULL
	@return			CTestController* A pointer to the newly created class.
	@pre 			None
	@post			Nothing is on the CleanupStack
 */
	
	static IMPORT_C CTestController* NewL(CActiveScheduler* aScheduler,
										  ComponentTesterInitialiserLC aEntryPoint,
										  RTest* aRTest = NULL,
										  TLoggingInfo*	aLogInfo = NULL);

/**
	@fn				static IMPORT_C CTestController* NewLC(CActiveScheduler* aScheduler,
														   ComponentTesterInitialiserLC aEntryPoint,
														   RTest* aRTest = NULL,
														   TLoggingInfo*	 aLogInfo = NULL)
	Intended Usage	: Standardized safe construction which leaves CTestController* on the cleanup stack.
	@leave  		KErrNoMemory.
	@since			7.0
	@param 			aScheduler The active scheduler, if one exists, otherwise NULL
	@param			aEntryPoint The global function used to create the derived CComponentTester object.
	@param			aRTest Optional RTest object to use in check unit test results
	@param			aLogInfo The logging configuration information
	@return			CTestController* A pointer to the newly created class.
	@pre 			None
	@post			CTestController is on the CleanupStack
 */
	
	static IMPORT_C CTestController* NewLC(CActiveScheduler* aScheduler,
										   ComponentTesterInitialiserLC aEntryPoint,
										   RTest* aRTest = NULL,
										   TLoggingInfo* aLogInfo = NULL);

/**
	@fn				 ~CTestController()
	Intended Usage	: Standardized virtual destruction method
	@since			7.0
 */
	
	virtual IMPORT_C ~CTestController();

/**
	@fn				Start()
	Intended Usage	:	Starts the testbed and runs all tests on all components.  This 
					function runs synchronously and does not return until all tests 
					are complete.
	@since			7.0
	@pre 			This object is constructed
	@post			Starts the active scheduler and therefore doesn't complete until the 
					active scheduler is stopped.
 */
	IMPORT_C void Start();

/**
	@fn				Start(RPointerArray<TTestInfo>* aTests)
	Intended Usage	:	Starts the specified tests.This function runs 
					synchronously and does not return until all tests are complete.
	@since			7.0
	@param			aTests The list of tests to be run.
	@pre 			This object is constructed
	@post			Starts the active scheduler and therefore doesn't complete until the 
					active scheduler is stopped.
 */
	
	IMPORT_C void Start(RPointerArray<TTestInfo>* aTests);

/**
	@fn				Start(TRequestStatus* aStatus)
	Intended Usage	:	Starts the testbed and runs all tests on all components.  This 
					function runs asynchronously and completes aStatus when all tests 
					are complete.
	@since			7.0
	@param			aStatus Status word for the calling function.  Is completed when all 
					tests are complete.
	@pre 			This object is constructed
	@post			An asynchronous request is issued to run all tests.
 */
	IMPORT_C void Start(TRequestStatus* aStatus);

/**
	@fn				Start(TRequestStatus* aStatus, RPointerArray<TTestInfo>* aTests)
	Intended Usage	:	Starts the specified tests.  If the default for aTest is used
					then all tests are run on all components.  This function runs 
					asynchronously and completes aStatus when all tests are complete.
	@since			7.0
	@param			aStatus Status word for the calling function.  Is completed when all 
					tests are complete.
	@param			aTests The list of tests to be run.
	@pre 			This object is constructed.
	@post			An asynchronous request is issued to run the specified tests.
 */
	IMPORT_C void Start(TRequestStatus* aStatus, RPointerArray<TTestInfo>* aTests);

/**
	@fn				RPointerArray<CComponentInfo>& FindComponents() const
	Intended Usage	:	Returns an array of the components available for testing
	Error Condition	:	
	@since			7.0
	@return			RPointerArray<CComponentInfo>& Information on the components which 
					are available for testing.
	@pre 			The object is fully constructed
	@post			Returns an array of the available components which can be used to select
					which test to run.
 */
	
	IMPORT_C const RPointerArray<CComponentInfo>& FindComponents() const;

/**
	@fn				IMPORT_C CDataLogger& DataLogger()
	Intended Usage	:	Returns a reference to the file logging functionality for use 
					by the user interface component.
	@since			7.0
	@return			CDataLogger& The current data logger to allow external logging
	@pre 			The CTestController has been created so that the data logger exists
	@post			Unspecified
 */
	
	static IMPORT_C CDataLogger& DataLogger();

/**
	@fn				Cancel()
	Intended Usage	:	Cancels any outstanding tests.
	@since			7.0
	@pre 			This object has been created and the asynchronous version of 
					Start has been called.
	@post			Any outstanding tests have been cancelled.
 */
	IMPORT_C void Cancel();

	friend class TTestController_StateAccessor;

private:
/**
	@fn				 CTestController(CActiveScheduler* aScheduler)
	Intended Usage	: Constructor
	@since			7.0
	@param 			aScheduler The existing active scheduler or NULL
	@param			aRTest Optional RTest object to use in check unit test results
 */
	
	CTestController(CActiveScheduler* aScheduler, RTest* aRTest = NULL);

/**
	@fn				void ConstructL(TLoggingInfo* aLogInfo, ComponentTesterInitialiserLC aEntryPoint)
	Intended Usage	: Completes the safe construction of the CTestController object
	@leave  		KErrNoMemory.
	@since			7.0
	@param			aLogInfo The logging configuration information
	@param			aEntryPoint The global function used to create the derived CComponentTester object.
	@pre 			First phase of construction is complete
	@post			Object is fully constructed
 */
	
	void ConstructL(TLoggingInfo* aLogInfo, ComponentTesterInitialiserLC aEntryPoint);

/**
	@fn				void InitialiseComponentTesterL(ComponentTesterInitialiserLC aEntryPointLC)
	@leave  		KErrNoMemory.
	Intended Usage	:	Called during construction to find all test modules
	Error Condition	:	
	@since			7.0
	@param			aEntryPointLC The global function used to create the derived CComponentTester object.
	@pre 			None
	@post			A list of all tests is available
 */
	void InitialiseComponentTesterL(ComponentTesterInitialiserLC aEntryPointLC);

/**
	@fn				TestsComplete()
	Intended Usage	:	Called by the CTestManager to indicate that all tests
					are complete.  Either stops the active scheduler if in 
					synchronous mode or completes the client if in async mode.
	@since			7.0
	@pre 			This object is constructed
	@post			Appropriate action is taken to complete the tests.
 */
	void TestsComplete();

private:
/** Starts the tests and stops the active scheduler when finished*/
	
	CTestManager*			iTestManager;
/** A list of the available tests*/
	
	RPointerArray<CComponentInfo>	iTestList;
/** Placeholder for an active scheduler if one is passed in on creation */
	
	CActiveScheduler*	iScheduler;
/** Flag indicating if we own the active scheduler in iScheduler */
	
	TBool				iOwnScheduler;
/** Provides the file logging capability */
	
	CDataLogger*		iDataLogger;
/** The status word of the client (if tests were run asynchronously). Will be completed
	when all tests have finished running. */
	TRequestStatus* iClientStatus;
/** Optional reference to the RTest object used in the EXE test harness code which
	kicked off this test framework */
	RTest* iRTest;
	};  // End of CTestController definition


#endif // _INC_TESTCONTROLLER_3A34E468034A_INCLUDED
