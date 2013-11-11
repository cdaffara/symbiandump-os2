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
// This file contains the definition of the class CComponentTester
// This file comment is for DOxygen only and is ignored by EDoc.
// 
//

/**
 @test
*/

#ifndef __COMPONENTTESTER_H__
#define __COMPONENTTESTER_H__

#include <e32base.h>
#include <e32test.h>

#include <ecom/test_bed/unittestinfo.h>
#include <ecom/test_bed/unittest.h>
#include <ecom/test_bed/datalogger.h>
#include <ecom/test_bed/componenttestobserver.h>

class CDataLogger;

/**
	@internalAll
	@struct TTestInfo
	Comments : Used by the UI component to pass in a list of the tests which are to run
	with information about each one.
 */

struct TTestInfo
	{
/** The zero-based component Id based on the list passed out from 
	CTestController::FindComponents */
	
	TInt	iComponentId;
/** The zero-based unit test Id within the above component */
	
	TInt	iUnitTestId;
/** The time at which this unit test should be scheduled to run */
	
	TTimeIntervalMicroSeconds32 iRunTime;
/** Parameter block which is to be passed to the unit test */
	
	TAny*	iParameters;
	};


// Global function typedef declaration of function provided by a test module of the component
// under test and is used as an entry point to kick start test bed. Provided as argument to CTestController.
typedef CComponentTester* (*ComponentTesterInitialiserLC)(CDataLogger&, MComponentTestObserver&);


/**
	@internalAll
	Comments : Abstract class from which developers can derive their own component testing
	classes.  To write a derived class just implement a NewLC() and a ConstructL() from 
	which ComponentTesterConstructL() should be the first thing called.
 */

class CComponentTester : public CActive, public MUnitTestObserver
	{
public:
/**
	@fn				 ~CComponentTester()
	Intended Usage	: Standardized virtual destruction method 
	@since			7.0
 */
	
	IMPORT_C ~CComponentTester();

/**
	@fn				IMPORT_C virtual void TestComponent(RPointerArray<TTestInfo>* aTests)
	Intended Usage	:	Called to perform a full test of the component. Runs the listed 
					tests or all unit tests if aTests == 0
	Error Condition	:	
	@since			7.0
	@param			aTests The list of tests to run, 0 means run all tests
	@pre 			This CComponentTester is fully initialized
	@post			All unit tests will be set up to run
 */
	
	IMPORT_C virtual void TestComponent(RPointerArray<TTestInfo>* aTests);

/**
	@fn				IMPORT_C RPointerArray<CUnitTestInfo>* TransitionSetsL() const
	Intended Usage	:	Called to provide a list of the transition sets in this component test.
						Ownership of the array is passed to the calling object.
	Error Condition	:	
	@since			7.0
	@return			RPointerArray<CUnitTestInfo>* Information on the unit tests contained
	in this component.  Ownership of the array is passed to the calling object.
	@pre 			None
 */
	
	IMPORT_C RPointerArray<CUnitTestInfo>* TransitionSetsL() const;

/**
	@fn				IMPORT_C void Complete(CUnitTest* aUnitTest)
	Intended Usage	:	Used, by the observed unit test, to indicate that the 
					test has completed
	Error Condition	:	
	@since			7.0
	@param			aUnitTest	The unit test which has completed.
	@pre 			None
	@post			If this was the last unit test then iObserver is informed that this 
	component is complete.
 */
	
	IMPORT_C void Complete(CUnitTest* aUnitTest);

/**
	@fn				IMPORT_C void SetRtest(RTest* aRTest)
	Intended Usage	:	Used by Controller or Manager to initialise the iRTest
					attribute of the component & unit test objects in t
	Error Condition	:	
	@since			7.0s
	@param			aRTest	Pointer to RTest object 
	@pre 			None
	@post			None
 */
	IMPORT_C void SetRTest(RTest* aRTest);

protected:
/**
	@fn				 IMPORT_C CComponentTester(CDataLogger&				aDataLogger, 
											   MComponentTestObserver&	aObserver)
	Intended Usage	: Default Constructor	
	@since			7.0
	@param 			aDataLogger Used for file logging capability
	@param 			aObserver To inform of completion of all tests
 */
	IMPORT_C CComponentTester(CDataLogger& aDataLogger, MComponentTestObserver& aObserver);

/**
	@fn				IMPORT_C void AddUnitTestL(const CUnitTest* aUnitTest)
	Intended Usage	:	Used by derived class to add UnitTests to the list
	Error Condition	:	
	@since			7.0
	@param			aUnitTest The UnitTest to be added
	@pre 			None
	@post			The specified unit test is added to the list of tests to be executed.
 */
	
	IMPORT_C void AddUnitTestL(const CUnitTest* aUnitTest);

/**
	@fn				IMPORT_C void AddParamUnitTestL(const CUnitTest* aUnitTest)
	Intended Usage	:	Used by derived classes to add UnitTests which can only be run when
	they are supplied with parameters from the UI.
	Error Condition	:
	@since			7.0
	@param			aUnitTest The unit test to add to the list
	@pre 			None
	@post			The specified unit test is added to the list of test to be executed and 
	is flagged as requiring parameters
 */
	
	IMPORT_C void AddParamUnitTestL(const CUnitTest* aUnitTest);

/**
	@fn				IMPORT_C virtual void ConstructL() = 0
	Intended Usage	:	Standard two-phase construction method.  To be implemented 
	in derived class
	Error Condition	:	
	@since			7.0
	@pre 			First phase of construction is complete
	@post			Object is fully constructed
 */
	
	IMPORT_C virtual void ConstructL() = 0;

/**
	@fn				IMPORT_C void ComponentTesterConstructL()
	Intended Usage	:	Must be called by derived class ConstructL() to perform base class 
					initialisation.
	Error Condition	:	
	@since			7.0
	@pre 			None
	@post			Object is fully constructed
 */
	
	IMPORT_C void ComponentTesterConstructL();

/**
	@fn				IMPORT_C void RunL()
	Intended Usage	:	
	Error Condition	:	
	@since			7.0
	@pre 			This CComponentTester is fully constructed
	@post			One of the CUnitTest contained in this CComponentTester has been run
 */
	
	IMPORT_C void RunL();

/**
	@fn				IMPORT_C void DoCancel()
	Intended Usage	:	
	Error Condition	:	
	@since			7.0
	@pre 			This CComponentTester is fully constructed.
	@post			Any outstanding asynchronous requests are cancelled.
 */
	
	IMPORT_C void DoCancel();

protected:
/** List of all the unit tests which make up this component test*/
	
	RPointerArray<CUnitTest>* iUnitTests;
/** The test logging mechanism*/
	
	CDataLogger&			iDataLogger;
/** The observer to inform when we have completed the test */
	
	MComponentTestObserver&		iObserver;
/** The index of the unittest which is to be run next */
	
	TInt						iCurrentUnitTest;
/** The list of tests to run, NULL means run all tests. We take ownership of the list but 
	not the items on it.*/
	
	RPointerArray<TTestInfo>*	iUnitTestsToRun;
/** A list of the test which cannot run without a parameter set from the UI */
	
	RArray<TInt>				iParameterizedTests;
/** A placeholder which is only used to ensure correct object cleanup when 
	TransitionSetsL() leaves */
	
	mutable RPointerArray<CUnitTestInfo>* iTransitionSets;
/** Optional reference to the RTest object used in the EXE test harness code which
	kicked off this test framework */
	RTest* iRTest;

	friend class TComponentTester_StateAccessor;
	};

#endif
