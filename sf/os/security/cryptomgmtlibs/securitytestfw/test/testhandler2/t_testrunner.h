/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef __T_TESTRUNNER_H__
#define __T_TESTRUNNER_H__

#include <e32base.h>

class Output;
class CTestAction;

/**
 * A class that is used to invoke individual tests.  This can be derived to
 * implement OOM testing, cancel testing and the like.  The default
 * implementation just runs the test.
 *
 * One instance of this class is used to run multiple tests.  A CTestHandler
 * owns an instance of this class (or a class derived from it).
 *
 * Note that this class provides a synchronous interface, which is implemented
 * in terms of CTestActions's asynchronous interface.
 */
class CTestRunner : public CActive
    {
 public:
    IMPORT_C CTestRunner(Output& aOut);
    IMPORT_C virtual ~CTestRunner();

    // The following methods return the status code from test action, but leave
    // if there is an error in the test handler.
    
    IMPORT_C virtual TInt PerformPrerequisiteL(CTestAction* aAction);
    IMPORT_C virtual TInt PerformActionL(CTestAction* aAction);
    IMPORT_C virtual TInt PerformPostrequisiteL(CTestAction* aAction, TInt aInitialStatus);

 protected:
    /// Typedef for any of CTestAction's async methods
    typedef void (CTestAction::*TTestMethod)(TRequestStatus&);
        
    /// Run one of CTestAction's async methods synchronously and return the result
    TInt RunAsyncMethodL(TTestMethod aMethod, CTestAction* aAction, TInt aInitialStatus);

	/// Run the active scheduler until our request is completed
	void RunSchedulerL();

	/// Run the active scheduler to process one outstanding request
	/// @return If our request has been completed yet
	TBool StepScheduler();
	
 protected:
    Output& iOut;

 private:
    IMPORT_C virtual TInt RunError(TInt aError);
    IMPORT_C virtual void DoCancel();
    IMPORT_C virtual void RunL();

 private:
    /// Whether an async method is currently being executed by the active scheduler
    TBool iSchedulerRunning;  
    };

/**
 * Abstract base class for a test runner that implements OOM testing.
 */
class COOMTestRunnerBase : public CTestRunner
    {
 public:
    IMPORT_C virtual ~COOMTestRunnerBase();

 protected:
    IMPORT_C COOMTestRunnerBase(Output& aOut);

	/// Called at the start of the OOM test
	virtual void StartOOMTestL() = 0;	
	/// Increment the simulated heap fail point
	virtual void IncHeapFailPoint() = 0;
	/// Reset simulated heap failures
	virtual void ResetHeapFail() = 0;
	/// Get the number of cells currently allocated
	virtual TInt AllocCount() = 0;
	/// Called at the end of the OOM test
	virtual void EndOOMTestL() = 0;
	
 private:
	// Override CTestRunner interface
    IMPORT_C virtual TInt PerformActionL(CTestAction* aAction);
    };

/**
 * Implementation of an test runner for OOM testing.
 */
class COOMTestRunner : public COOMTestRunnerBase
	{
 public:
	COOMTestRunner(Output& aOut);
	virtual ~COOMTestRunner();
	
 private:
	// Implement interface defined by COOMTestRunnerBase
	virtual void StartOOMTestL();	
	virtual void IncHeapFailPoint();
	virtual void ResetHeapFail();
	virtual TInt AllocCount();
	virtual void EndOOMTestL();

 private:
	TInt iFailPoint;
	};

/// A test runner that performs cancellation tests.
class CCancelTestRunner : public CTestRunner
    {
 public:
    CCancelTestRunner(Output& aOut);
    virtual ~CCancelTestRunner();

    virtual TInt PerformActionL(CTestAction* aAction);

 private:
    TInt RunAndCancelPeformActionMethod(CTestAction* aAction, TInt aInitialStatus,
                                        TInt aCancelStep, TInt& aStep);
    TInt RunAndCancelTestAction(CTestAction* aAction, TInt aCancelStep);

 private:
    TBool iAbort;         ///< Set when we want to abort the test
    };

#endif
