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
* Description: This file contains the header file of the 
* TestCombiner module of STIF Test Framework.
*
*/

#ifndef TESTCOMBINER_H
#define TESTCOMBINER_H

//  INCLUDES
#include <e32std.h>

#include <StifParser.h>
#include <StifLogger.h>
#include <StifTestEventInterface.h>
#include <StifTestModule.h>
#include <stifinternal/TestServerClient.h>
#include "TestKeywords.h"
#include "TestCombinerEvent.h" 
#include "TestCombinerUtils.h" 
#include "STIFMeasurement.h"

// CONSTANTS
// Printing priorities
const TInt KPrintPriExec = 400;
const TInt KPrintPriLow  = 300;
const TInt KPrintPriNorm = 200;
const TInt KPrintPriHigh = 100;
// Log dir and file
_LIT( KTestCombinerLogDir, "\\Logs\\TestFramework\\TestCombiner\\" );
_LIT( KTestCombinerLogFile, "TestCombiner.txt" );
_LIT( KTestCombinerLogFileWithTitle, "TestCombiner_[%S].txt" );
// Configuration file tags
_LIT( KTestStartTag, "[Test]" );
_LIT( KTestEndTag, "[Endtest]" );
// Define tags
_LIT( KDefineStartTag, "[Define]" );
_LIT( KDefineEndTag, "[Enddefine]" );
// Initialization tags
_LIT( KInitStartTag, "[Init]" );
_LIT( KInitEndTag, "[Endinit]" );
_LIT( KInitRcpTimeout, "rcptimeout" );

// Remote protocol timeout 
const TInt KRemoteProtocolTimeout = 30000000;
// Remote protocol Master id
const TUint32 KRemoteProtocolMasterId = 0xdeadbeef;

// Loop counter macro
_LIT( KLoopCounter, "LOOP_COUNTER" );

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CTestRunner;
class CTestCase;
class CTCTestCase;
class CRemoteTestCase;
class CTCTestModule;
class CRemoteTimer;
class CRemoteReceiver;
class CRemoteSendReceive;
class TTestMeasurement;
    
// CLASS DECLARATION

// DESCRIPTION
// CTestCombiner contains the Test Module interface of TestCombiner 
// for STIF Test Framework

NONSHARABLE_CLASS(CTestCombiner) 
    :public CTestModuleBase
    {
    public: // Enumerations

    private: // Enumerations

    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTestCombiner* NewL();

        /**
        * Destructor.
        */
        ~CTestCombiner();

    public: // New functions
        /**
        * Test Case completion from CTCTestCase.
        */
        void Complete( CTestCase* aTestCase, TInt aError=KErrNone );

        /**
        * Set event request pending.
        */
         void SetEventPendingL( TEventIf aEvent );

        /**
        * Handles timeouts.
        */
        void RemoteTimeout();

        /**
        * Handles responce received from slave
        */
        void ReceiveResponse( TDesC& aMsg );

    public: // Functions from base classes
        /**
        * Test module initialization.
        */ 
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

        /**
        * GetTestCasesL is used to inquire testcases. 
        */
        TInt GetTestCasesL( const TFileName& aConfigFile, 
                            RPointerArray<TTestCaseInfo>& aTestCases );
        /**
        * RunTestCase is used to run an individual test case specified 
        * by aTestCase. 
        */
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aConfig, 
                           TTestResult& aResult );

    protected: // New functions

    protected: // Functions from base classes

    private:
        /**
        * C++ default constructor.
        */
        CTestCombiner();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * GetTestCaseL gets the specified test case section 
        * from the configfile.
        */
        CStifSectionParser* GetTestCaseL( TInt aCaseNumber, 
                                            const TFileName& aConfig );

        /**
        * RunTestL runs the testcase specified in section.
        */
        void RunTestL();
      
        /**
        * StartTestL starts a specified testcase from another test module.
        */
        TInt StartTestL( CStartInfo& aStartInfo );

        /**
        * GetTestModuleL returns a specified test module description.
        */
        CTCTestModule* GetTestModuleL( TDesC& aModule,
                                       TDesC& aIniFile,
                                       const TDesC& aConfigFile  );

        /**
        * Gets a testcase identified with testid from test case array.
        */
        CTestCase* GetTest( TDesC& aTestId );
     
        /**
        * Gets a running testcase identified with testid from test case array.
        */
        CTestCase* GetRunningTest( TDesC& aTestId );
        
        /**
        * Gets a testcase identified with testid from test case array.
        */
        CTCTestCase* GetLocalTest( TDesC& aTestId );
     
        /**
        * Gets a running testcase identified with testid from test case array.
        */
        CTCTestCase* GetLocalRunningTest( TDesC& aTestId );

        /**
        * Gets a testcase identified with aSlaveId from slave array.
        */
        CRemoteTestCase* GetRemoteTest( TDesC& aTestId );

        /**
        * Gets a testcase identified with aSlaveId from slave array.
        */
        CRemoteTestCase* GetRemoteTest( TUint32 aSlaveId );
        
        CRemoteTestCase* GetRemoteTestRunSent( TUint32 aSlaveId );
        /**
        * Gets a running testcase identified with aSlaveId from slave array.
        */
        CRemoteTestCase* GetRunningRemoteTest( TUint32 aSlaveId );

        /**
        * Gets a testcase running on slave with aSlaveDevId.
        */
        CRemoteTestCase* GetRemoteRunningTestOnSlave( TUint32 aSlaveDevId );

        /**
        * Gets a correct CRemoteSendReceive object on slave with aSlaveId.
        */
        CRemoteSendReceive* GetRemoteSendReceive( TUint32 aSlaveId );        

        /**
        * Get running slave.
        */
        CSlaveInfo* CTestCombiner::GetSlave( TUint32 aSlaveId );
        CSlaveInfo* CTestCombiner::GetSlave( TDesC& aSlaveName );

        /**
        * Check requested events and send unset to first requested.
        */
        TBool UnsetEvent( TEventIf& aEvent, TRequestStatus& aStatus );

        /**
        * Read initialization from file.
        */
        void ReadInitializationL( const TDesC& aIniFile );

        /**
        * Executes measurement script line.
        */
        void ExecuteMeasurementL( CStifItemParser* aItem );

        /**
        * Starts measurement.
        */
        void StartMeasurementL( const TDesC& aType, CStifItemParser* aItem );

        /**
        * Stops measurementplugin's measurement.
        */
        void StopMeasurementL( const TDesC& aType );
        
        /**
        * Returns new index for test module controller.
        * This number is appended to module controller name.
        * This method is used when option to run every test case in separate
        * process is set to on.                        
        */
        TInt GetIndexForNewTestModuleController(void);
       
        /**
        * Append to TC's result description (if possible due to length)
        * limitation provided text in [] brackets.
        */
        void AppendTestResultToResultDes(TDes& aResultDescr, const TDesC& aTestCaseResultDescr);


    public:     // Data
        // Logger 
        CStifLogger*                    iLog;

    protected:  // Data
          
    private:    // Data
        // Test case array of running/runned test cases 
        RPointerArray<CTestCase>        iTestCases;

        // SendReceive array(asynchronous sendreveive)
        RPointerArray<CRemoteSendReceive> iSendReceive;

        // Array of test modules
        RPointerArray<CTCTestModule>    iTestModules;

        // TestId of the waited (by complete keyword) test case 
        TName                           iWaitTestCase;

        // Flag to indicate that activescheduler is started    
        TBool                           iSchedulerActive;

        // Number of currently running test cases
        TInt                            iRunningTests;

        // Test case result of the TestCombiner
        TInt                            iResult;

        // Error code in case when execution of script fails
        TInt                            iScriptFailed;
        
        // Error description in case when execution of script fails
        TName                           iScriptFailedDescription;
        
        // Sectionparser for the current testcase
        CStifSectionParser*             iSectionParser;

        // Pointer to used testrunner
        CTestRunner*                    iTestRunner;

        // Pointer to used testrunner
        CRemoteReceiver*                iRemoteReceiver;

        // Event for async NotifyEvent
        TEventIf                        iEvent;
        TEventIfPckg                    iEventPckg;

        RPointerArray<TEventTc>         iEventArray;

         // Predefined values are listed here
        RPointerArray<CDefinedValue>    iDefined;

        // CSlaveInfo 
        RPointerArray<CSlaveInfo>       iSlaveArray;

        // Remote protocol timeout
        TInt                            iRemoteTimeout;

        // Variable for abortimmediately keyword @js
        TBool                           iCancelIfError;

        // Number of the failed test case, needed when using 
        // abortimmediately keyword @js
        TInt                            iFailedTestCase;

        // CStifTestMeasurement object
        RPointerArray<TTestMeasurement> iTestMeasurementArray;

        // Indicate that loop is used in test case file.
        // This is used for reseting and allowing allocated id's etc.
        // (E.g. testid values can be same in loop).
        TBool                           iLoopIsUsed;
        RArray<TAny*>                   iLoopAllocationArray;

        // Standard Logger 
        CStifLogger*                    iStdLog;

        // Test case Logger 
        CStifLogger*                    iTCLog;

        // Flag which says if test case title should be appended to the
        // log file name. This is read from SettingServer
        TBool                               iAddTestCaseTitleToLogName;
        
        // Counter for created TCModuleControllers
        TInt                            iIndexTestModuleControllers;

        // Is this UI testing?
        // If yes, then Test Engine works in a specific way.
        // It creates new process for every test scripter's test case
        // and deletes it after test case has finished.
        TBool                           iUITestingSupport;
        
        // Should every test case be executed in separate process?
        // This option is similar to iUITestingSupport, however
        // it does not delete the process after test case is finished.
        TBool                           iSeparateProcesses;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
        friend class CTestRunner;
        friend class CTestEventNotifier;

    };

// CLASS DECLARATION

// DESCRIPTION
// CTestRunner parses the TestCombiner configuration file and 
// runs a testcase. CTestRunner is the friend of CTestCombiner
// and uses its private member variables directly.

NONSHARABLE_CLASS(CTestRunner) 
    :public CActive
    {
    public: // Enumerations

    private: // Enumerations
        enum TRunnerState{
            ERunnerIdle,
            ERunnerRunning,
            ERunnerWaitTestCase,
            ERunnerWaitEvent,
            ERunnerWaitUnset,
            ERunnerWaitTimeout,
            ERunnerCancel,
            ERunnerError,
            ERunnerAllocate,
            ERunnerFree,
            ERunnerRemote,
            ERunnerReady,
            };

    public:  // Constructors and destructor  
        /**
        * Two-phased constructor.
        */
        static CTestRunner* NewL( CTestCombiner* aTestCombiner );

        /**
        * Destructor.
        */
        ~CTestRunner();

    public: // New functions
        /**
        * Calls SetActive() from CActive.
        */
        void SetRunnerActive();

        /**
        * Handles received response from slave.
        */
        TBool ReceiveResponseL( TDesC& aMsg );

        /**
        * Cancel testcases.
        */
        void CancelTestCases();

    public: // Functions from base classes     
        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel.
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions

    protected:  // Functions from base classes

    private:
        /**
        * C++ default constructor.
        */
        CTestRunner( CTestCombiner* aTestCombiner );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Executes one script line.
        */
        TBool ExecuteLineL( CStifItemParser* aItem );

        /**
        * Executes run script line.
        */
        TBool ExecuteRunL( CStifItemParser* aItem );

        /**
        * Parses run parameters.
        */
        void ParseRunParamsL( CStifItemParser* aItem, CStartInfo& aStartInfo );

        /**
        * Executes test control script line.
        */
        TBool ExecuteTestCtlL( CStifItemParser* aItem, 
                               TTCKeywords::TKeywords aKeyword );
                               
        /**
        * Executes pause in combiner
        */
        TBool ExecuteCombinerPauseL( CStifItemParser* aItem );

        /**
        * Executes pause script line.
        */
        TBool ExecutePauseL( CStifItemParser* aItem, CTestCase* aTestCase );

        /**
        * Executes complete script line.
        */
        TBool ExecuteCompleteL( CStifItemParser* aItem, CTestCase* aTestCase );

        /**
        * Executes event set script line.
        */
        TBool ExecuteEventSetL( CStifItemParser* aItem );

        /**
        * Executes event unset script line.
        */
        TBool ExecuteEventUnsetL( CStifItemParser* aItem );

        /**
        * Executes event control command.
        */
        TBool ExecuteEventCtlL( CStifItemParser* aItem, 
                                TTCKeywords::TKeywords aKeyword );

        /**
        * Executes allocate script line.
        */
        TBool ExecuteAllocateL( CStifItemParser* aItem );

        /**
        * Executes free script line.
        */
        TBool ExecuteFreeL( CStifItemParser* aItem );

        /**
        * Frees slave.
        */
        void ExecuteFreeL( CSlaveInfo* aSlave );

        /**
        * Executes remote script line.
        */
        TBool ExecuteRemoteL( CStifItemParser* aItem );

        /**
        * Executes remote run script line.
        */
        TBool ExecuteRemoteRunL( CStifItemParser* aItem, CSlaveInfo* aSlave,
                                 HBufC *aSetUnsetEvent = NULL,
                                 TInt aCaseNumber = 0 );

        /**
        * Executes remote test control script line.
        */
        TBool ExecuteRemoteTestCtlL( CStifItemParser* aItem, 
                                     CTestCase* aTestCase, 
                                     TInt aCmd  );

        /**
        * Executes remote event control script line.
        */
        TBool ExecuteRemoteEventCtlL( CStifItemParser* aItem, 
                                      CSlaveInfo* aSlave, 
                                      TInt aCmd  );
                                      
        /**
        * Executes remote event set unset line
        */
        TBool ExecuteRemoteSetUnsetEventL( CStifItemParser* aItem,
                                      CSlaveInfo* aSlave,
                                      TInt aCmd  );

        /**
        * Executes asynchronous remote sendreceive control script line.
        */
        TBool ExecuteRemoteSendReceiveL( CStifItemParser* aItem, 
                                      CSlaveInfo* aSlave );

        /**
        * Executes remote unknown control script line.
        */
        TBool ExecuteRemoteUnknownL( CStifItemParser* aItem, 
                                     CSlaveInfo* aSlave,
                                     TDesC& aCommand );
        /**
        * Handles received run responses from slave.
        */
        TBool ReceiveResponseRunL( CStifTFwIfProt& aMsg );

        /**
        * Handles received test control responses from slave.
        */
        TBool ReceiveResponseTestCtlL( CStifTFwIfProt& aMsg );

        /**
        * Handles received Event system control responses from slave.
        */
        TBool ReceiveResponseEventCtlL( CStifTFwIfProt& aMsg );

        /**
        * Handles received asynchronous sendreceive responses from slave.
        */
        TBool ReceiveResponseSendReceiveL( CStifTFwIfProt& aMsg );

        /**
        * Handles received responses for unspecified commands from slave.
        */
        TBool ReceiveResponseUnknownL( CStifTFwIfProt& aMsg );

        /**
        * Check unset event we are waiting.
        */
        TBool CheckUnsetEvent();

        /**
        * Parses optional argument.
        */
        void ParseOptArgL( const TDesC& aOptArg, TPtrC& aArg, TPtrC& aVal);

        /**
        * Preprocess hardcoded values in test case.
        */                    
        CStifItemParser* PreprocessLineL( TDesC& line );

        /**
        * Checks if aWord is defined value.
        */                    
        TInt CheckDefined( TPtrC& aWord );

        /**
        * Handle the loop keyword operations.
        */                    
        void ExecuteLoopL( CStifItemParser* aItem );

        /**
        * Handle the endloop keyword operations.
        */                    
        TBool ExecuteEndLoopL();

    public:     //Data

    protected:  // Data

    private:    // Data
        // Runner state
        TRunnerState                iState;

        // Backpointer to CTestCombiner
        CTestCombiner*              iTestCombiner;

        // Event system control
        TEventIf                    iEvent;

        // Pause timer
        RTimer                      iPauseTimer;
        TName                       iPausedTestCase;
        TInt                        iPauseTime;
        
        // Pause combiner timer
        RTimer                      iPauseCombTimer;
        TInt                        iPauseCombRemainingTime;

        // Temporary line buffer 
        HBufC*                      iLine;

        // Remote control protocol timer
        CRemoteTimer*               iRemoteTimer;

        // Loop times, counter and start position
        TInt                    iLoopTimes;
        TInt                    iLoopCounter;
        TInt                    iLoopStartPos;
        TBuf<10>                iLoopCounterDes;
        TInt                    iEndLoopStartPos;

        // Loop passlimit (endurance) feature
        TInt                    iPasslimit;
        TBool                   iPasslimitEnabled;
        TInt                    iPassedIterationCnt;
        
        // Loop (timed loop option)
        TBool                   iTimedLoop;
        TTime                   iStartTime;
        TTimeIntervalMicroSeconds iExpectedLoopTime;

		// Message displayed when leave occurs during script parsing
		TBuf<256> iRunErrorMessage;
    public:      // Friend classes

    protected:  // Friend classes

    private:     // Friend classes
        friend class CTestCombiner;

    };

// CLASS DECLARATION
typedef TBuf<0x200> TRemoteProtMsg;

// DESCRIPTION
// CTestRemoteReceiver handles remote protocol receiving
NONSHARABLE_CLASS(CRemoteReceiver) 
    :public CActive
    {
    public: // Enumerations
         
    private: // Enumerations
        enum TReceiverState
            {
            EReceiverIdle,
            EReceiverPending,
            };

    public:  // Constructors and destructor  
        /**
        * Two-phased constructor.
        */
        static CRemoteReceiver* NewL( CTestCombiner* aTestCombiner );

        /**
        * Destructor.
        */
        ~CRemoteReceiver();

    public: // New functions
        /**
        * Calls SetActive() from CActive.
        */
        void Start();

    public: // Functions from base classes     
        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel.
        */
        void DoCancel();

    protected:  // New functions

    protected:  // Functions from base classes

    private:
        /**
        * C++ default constructor.
        */
        CRemoteReceiver( CTestCombiner* aTestCombiner );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:     //Data

    protected:  // Data

    private:    // Data
        // Runner state
        TReceiverState              iState;

        // Backpointer to CTestCombiner
        CTestCombiner*              iTestCombiner;

        // Remote protocol message
        TRemoteProtMsg              iRemoteMsg;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

// CLASS DECLARATION

// DESCRIPTION
// CRemoteTimer handles remote protocol timeouts
NONSHARABLE_CLASS(CRemoteTimer) 
    :public CActive
    {
    public: // Enumerations
         
    private: // Enumerations
        enum TTimerState
            {
            ETimerIdle,
            ETimerPending,
            };

    public:  // Constructors and destructor  
        /**
        * Two-phased constructor.
        */
        static CRemoteTimer* NewL( CTestCombiner* aTestCombiner );

        /**
        * Destructor.
        */
        ~CRemoteTimer();

    public: // New functions
        /**
        * Starts timer.
        */
        void SetTimerActive( TTimeIntervalMicroSeconds32 anInterval );

    public: // Functions from base classes     
        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel.
        */
        void DoCancel();

    protected:  // New functions
          
    protected:  // Functions from base classes

    private:
        /**
        * C++ default constructor.
        */
        CRemoteTimer( CTestCombiner* aTestCombiner );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:     //Data
     
    protected:  // Data

    private:    // Data
        // Runner state
        TTimerState     iState;

        // Backpointer to CTestCombiner
        CTestCombiner*  iTestCombiner;

        // Pause timer
        RTimer          iTimer;

    public:      // Friend classes

    protected:  // Friend classes

    private:     // Friend classes

    };


// DESCRIPTION
// TTestMeasurement contains test measurement module information
class TTestMeasurement
    {
    public: 
        TTestMeasurement(){ iMeasurement = NULL; };
        ~TTestMeasurement(){ delete iMeasurement; };

    public:

    public:
        // "object" name given in test case file.
        TName                   iName;
        // MSTIFTestInterference object.
        CSTIFTestMeasurement*  iMeasurement;

    private:

    };

#endif        // TESTCOMBINER_H

// End of File
