/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_TESTHANDLER_H__
#define __T_TESTHANDLER_H__

#include <e32base.h>
#include <f32file.h>

class MTestSpec;
class Output;
class CTestHandlerSettings;
class CTestSetup;
class CTestAction;
class CTestRunner;

class TTestSummary
	{
public:
	TTestSummary();
	TBool AllTestsPassed();
	void PrintL(Output& aOut);
	
public:
	TInt iTestsRun;
	TInt iTestsFailed;
	};

/**
 * CTestHandler is the class which runs the tests, and outputs results
 */
class CTestHandler : public CBase
	{
public:
	/*
	* MTestSpec is the interface class, providing GetNextTest
	* TTestHandlerSettings provides the command line switches
	*
	* 
	*/

	IMPORT_C static CTestHandler* NewLC(RFs& aFs, MTestSpec& aTestSpec, 
										CTestHandlerSettings& aSettings,
										CConsoleBase* aConsole,
										Output* aOut);
	~CTestHandler();

private:
	CTestHandler(RFs& aFs, MTestSpec& aTestSpec, CConsoleBase* aConsole,
				 Output* aOut);
	void ConstructL(CTestHandlerSettings& aSettings);

public:
    // API for test programs
    
	/// Run the test sequence
	IMPORT_C void RunTestsL();

    /**
     * Set the test runner to use.  This object takes ownership.  Passing NULL
     * means use the default test runner.
     */
    IMPORT_C void SetTestRunnerL(CTestRunner *aTestRunner);

public:
    // API for test actions

	/// Access to shared data
	CBase* SharedData() const;
	void SetSharedData(CBase* aData);

    /// Set the heap mark
    void SetHeapMark(TInt aAllocCount);

    /// Get the heap mark
    TInt HeapMark();

	/// Get the count of tests run/failed
	TTestSummary Summary();

private:
    /// The different possible locations a test can fail in.
    enum TTestFailLocation
        {
        EFailInTestHandler,
        EFailInPrerequisite,
        EFailInAction
        };

    /// Get the human readable name for a fail location
    const TDesC& GetFailLocationName(TTestFailLocation aLoc);

private:
    /**
     * Run one test.  Called repeatedly by RunTestsL().
     */
	void RunTestL(CTestAction* aAction);
    /**
     * Record a test failure. 
     */
    void FailTestL(CTestAction* aAction, TTestFailLocation aLoc, TInt aErr);
	/**
	 * Add a test to the list of failed tests.
	 */
	void AppendFailedTestL(TInt aIndex, const TDesC8& aName);

private:
	/**
	 * This function prints the number of tests passed, failed, etc at the end of
	 * the test series.
	 */
	void DisplaySummary();

private:
	/**
	 * Used by the test handler itself and the iManager
	 */
	RFs& iFs;

	/**
	 * The settings that define the behaviour of the handler.
	 */
	CTestHandlerSettings *iSettings;

	CConsoleBase* iConsole;
	Output* iOut;

	CTestSetup* iTestSetup;
	CActiveScheduler* iScheduler;

	MTestSpec& iTestSpec;

	TInt iActionNumber;
	TInt iActionCount;
	
	/**
	 * After completion of the series of tests, this array contains the numbers of
	 * the failed tests.
	 */
	RArray<TInt> iFailedTests;
	/**
	 * After completion of the series of tests, this array contains the names of
	 * the failed tests.
	 */
	RPointerArray<HBufC> iFailedTestNames;
	/**
	 * After completion of the series of tests, this array contains the names
	 * of tests that failed because of known defects	 */

	RPointerArray<HBufC> iKnownDefects;

	/**
	 * Pointer to shared data.  This is initially NULL.  It can be accessed by
	 * any of the test actions.  If it is not NULL when this class is destroyed,
	 * it is deleted.
	 */
	CBase* iSharedData;

    /// Pointer to the current test runner.  We own this.
    CTestRunner* iTestRunner;

    /// Pointer to the test runner to install when we've finished the current
    /// test.  We own this.
    CTestRunner* iNextTestRunner;

    /// The heap mark, used by heap mark actions 
    TInt iHeapMark;
	};

#endif
