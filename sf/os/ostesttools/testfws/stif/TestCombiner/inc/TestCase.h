/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description: This file contains the header file of the CTCTestCase 
* and TTCTestModule.
*
*/

#ifndef TESTCASE_H
#define TESTCASE_H

//  INCLUDES
#include <e32base.h>
#include <stifinternal/TestServerClient.h>
#include <StifTestModule.h>
#include "TestCaseNotify.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CTestCombiner;

// CLASS DECLARATION

// DESCRIPTION
// CTestCase is a parent class for the test case runner in TestCombiner.
NONSHARABLE_CLASS(CTestCase)
    :public CActive
    {
    public: // Enumerations
        enum TTestCaseStatus
            {
            ETestCaseIdle,
            ETestCaseRunning,
            ETestCaseCompleted
            };
        
        enum TCaseType
            {
            ECaseLocal,
            ECaseRemote,
            };

    private: // Enumerations

    public: // Destructor
        /**
        * Destructor.
        */
        virtual ~CTestCase();

    public: // New functions
        /**
        * Calls SetActive() from CActive.
        */
        virtual void StartL();
        
        /**
        * Get state of the test case.
        */
        TTestCaseStatus State(){ return iState; }
                
        /**
        * Get reference to TestId.
        */
        const TDesC& TestId(){ return *iTestId; }

        /**
        * Get expected result.
        */
        TInt ExpectedResult(){ return iExpectedResult; }

        /**
        * Get expected result category.
        */
        TFullTestResult::TCaseExecutionResult
            ExpectedResultCategory(){ return iExpectedResultCategory; }

        /**
        * Get test case type.
        */
        TCaseType Type(){ return iType; }

        //--PYTHON-- begin
        /**
        * Get TestModule.
        */
        CTCTestModule* TestModule(void){ return iTestModule; }
        //--PYTHON-- end

        /**
        * Checks state and additional conditions
        */
        virtual TBool IsCompletelyFinished(void);

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

    protected: // New functions

        /**
        * Completes testcase.
        */
        virtual void Complete( TInt aError );

    protected: // Constructors
        /**
        * C++ default constructor.
        */
        CTestCase( CTestCombiner* testCombiner,
                   TInt aExpectedResult,
                   TFullTestResult::TCaseExecutionResult aCategory,
                   TCaseType aType,
                   CTCTestModule* aModule ); //--PYTHON--

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TDesC& aTestId );

    private:

    public:  // Data

        // Test case result
        TFullTestResult         iResult;

    protected: // Data
        // Test case type
        TCaseType               iType;

        // Backpointer
        CTestCombiner*          iTestCombiner;

        // Internal state
        TTestCaseStatus         iState;

        // Unique test id
        HBufC*                  iTestId;

        // Expected result for the test case
        TInt                    iExpectedResult;

        // Expected result category
        TFullTestResult::TCaseExecutionResult iExpectedResultCategory;

    private: // Data
        //--PYTHON-- begin
        CTCTestModule*			iTestModule;
        //--PYTHON-- end

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes
        friend class CTestRunner;

    };

// CLASS DECLARATION

// DESCRIPTION
// CTCTestCase handles the running of a single testcase in TestCombiner.

NONSHARABLE_CLASS(CTCTestCase)
    :public CTestCase
    {
    public: // Enumerations

    private: // Enumerations

    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTCTestCase* NewL( CTestCombiner* testCombiner,
                                  TDesC& aModuleName,
                                  TDesC& aTestId,
                                  TInt aExpectedResult,
                                  TFullTestResult::TCaseExecutionResult aCategory,
                                  const TDesC& aTestCaseArguments,
                                  CTCTestModule* aModule ); //--PYTHON--

        /**
        * Destructor.
        */
        ~CTCTestCase();

    public: // New functions
        /**
        * Calls SetActive() from CActive.
        */
        void StartL();

        /**
        * Get reference to ModuleName.
        */
        const TDesC& ModuleName(){ return *iModuleName; }

        /**
         * Get test case arguments
         */
        const TDesC& TestCaseArguments() const;
        
        /**
        * Get reference to RTestExecution.
        */
        RTestExecution& TestExecution(){ return iTestExecution; }

        /**
        * Get reference to event array.
        */
        RPointerArray<CTestEventNotify>& EventArray(){ return iEventArray; }

        /**
        * Get reference to state event array.
        */
        RPointerArray<TEventIf>& StateEventArray(){ return iStateEventArray; }

         /**
        * Final complete of the testcase.
        */
        void Complete2();

        /**
        * Checks state and progress notifier
        */
        TBool IsCompletelyFinished(void);

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes
        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

    private:
        /**
        * C++ default constructor.
        */
        CTCTestCase( CTestCombiner* testCombiner,
                     TInt aExpectedResult,
                     TFullTestResult::TCaseExecutionResult aCategory,
                     CTCTestModule* aModule ); //--PYTHON--

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TDesC& aModuleName, TDesC& aTestId, const TDesC& aTestCaseArguments );

        /**
        * Start complete the testcase(Complete2 make the final complete).
        */
        void Complete( TInt aError );

    public:  // Data
        // Test case result package
        TFullTestResultPckg iResultPckg;

    protected: // Data

    private: // Data

        // Progress notifier
        CTestProgressNotifier*  iProgress;

        // Event notifier
        CTestEventNotifier*     iEvent;

        // Module name
        HBufC*                  iModuleName;

        // Test case arguments
        HBufC*                  iTestCaseArguments;
        
        // Handle to test case execution
        RTestExecution          iTestExecution;

        // Event array for events requested by test case
        RPointerArray<CTestEventNotify> iEventArray;

        // Event array for state events set by test case
        RPointerArray<TEventIf>         iStateEventArray;
        
        // Command notifier
        CTestCommandNotifier*    iCommand;

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes

    };


// CLASS DECLARATION

// DESCRIPTION
// Testcase starting information
NONSHARABLE_CLASS(CRemoteTestCase)
    :public CTestCase
    {

    public: // Enumerations
        enum TRemoteState
            {
            ECaseIdle,
            ECaseRunSent,
            ECaseRunning,
            ECasePauseSent,
            ECasePaused,
            ECaseResumeSent,
            ECaseCancelSent,
            ECaseCancelled,
            ECaseCompleted,
            };

    private: // Enumerations

    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CRemoteTestCase* NewL( CTestCombiner* testCombiner,
                                      TDesC& aTestId,
                                      TInt aExpectedResult,
                                      TFullTestResult::TCaseExecutionResult
                                            aCategory );

        /**
        * Destructor.
        */
        ~CRemoteTestCase();

    public: // New functions

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:
        /**
        * C++ default constructor.
        */
        CRemoteTestCase( CTestCombiner* testCombiner,
                         TInt aExpectedResult,
                         TFullTestResult::TCaseExecutionResult aCategory );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TDesC& aTestId );

    public:  // Data
        // Remote slave state
        TRemoteState    iRemoteState;

        // Slave identifier
        TUint32         iSlaveId;

        // Free slave after completed
        TBool           iFreeSlave;

    protected: // Data

    private: // Data

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes

    };

// CLASS DECLARATION

// DESCRIPTION
// CRemoteSendReceive handles the running of an asynchronous 'sendreveive'
// in TestCombiner.
// Note: If want to improve sendreceive for allowing use like 'run'
// then this must inherit from CActive
NONSHARABLE_CLASS(CRemoteSendReceive) :public CBase
    {
    public: // Enumerations

    public: // Enumerations
        enum TRemoteState
            {
            ECaseSend,
            ECaseCompleted,
            };

    private: // Enumerations

    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CRemoteSendReceive* NewL( CTestCombiner* aTestCombiner );

        /**
        * Destructor.
        */
        ~CRemoteSendReceive();

    public: // Constructors and destructor

    public: // New functions


    public: // Functions from base classes

    protected:  // New functions

    protected: // Functions from base classes

    private:
        /**
        * C++ default constructor.
        */
        CRemoteSendReceive( CTestCombiner* aTestCombiner );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:  // Data

       // Remote slave state
        TRemoteState    iRemoteState;

        // Slave identifier
        TUint32         iSlaveId;

    protected: // Data

    private: // Data

        // Backpointer
        CTestCombiner*      iTestCombiner;

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes

    };

// CLASS DECLARATION


// DESCRIPTION
// TTCTestModule encapsulates information about loaded test module.

NONSHARABLE_CLASS(CTCTestModule)
    :public CBase
    {
    public: // Enumerations

    private: // Enumerations

    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTCTestModule* NewL( CTestCombiner* testCombiner,
                                    TDesC& aModule,
                                    TDesC& aIni,
                                    const TDesC& aConfigFile );

        /**
        * Destructor.
        */
        ~CTCTestModule();

    public: // Constructors and destructor

    public: // New functions
         /**
         * Get module name.
         */
         const TDesC& ModuleName(){ return *iModuleName; }

         /**
         * Get module inifile name.
         */
         const TDesC& IniFile(){ return *iIniFile; }

         /**
        * Get reference to RTestServer.
        */
        RTestServer& TestServer(){ return iTestServer; }

        /**
        * Get reference to RTestModule.
        */
        RTestModule& TestModule(){ return iTestModule; }

        /**
        * Get test cases from module.
        */
        void GetTestCasesForCombiner(const TDesC& aConfigFile);

        /**
        * Get index of given test case.
        */
        TInt GetCaseNumByTitle(TDesC& aTitle, TInt& aCaseNum);

    public: // Functions from base classes

    protected:  // New functions

    protected: // Functions from base classes

    private:
        /**
        * C++ default constructor.
        */
        CTCTestModule( CTestCombiner* testCombiner );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TDesC& aModule,
                         TDesC& aIni,
                         const TDesC& aConfigFile );

    public:  // Data

    protected: // Data

    private: // Data
        // Backpointer
        CTestCombiner*      iTestCombiner;
        
        // Module name
        HBufC*              iModuleName;
        // Module initialization file
        HBufC*              iIniFile;

        // Handle to test server
        RTestServer         iTestServer;
        // Handle to module 
        RTestModule         iTestModule;
        
        // Error notifier
        CTestErrorNotifier* iErrorPrinter;

        // Array of test cases used to find test case number by title
        CFixedFlatArray<TTestCaseInfo>* iTestCases;

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes

    };
    
#endif        // TESTCASE_H  

// End of File
