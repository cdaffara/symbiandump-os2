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
* Description: This file contains the header file of the CTestServer.
*
*/

#ifndef TEST_THREAD_CONTAINER_H
#define TEST_THREAD_CONTAINER_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <StifLogger.h>
#include <StifTestModule.h>
#include <StifTestInterface.h>
#include "TestServerModuleIf.h"

// CONSTANTS
const TInt KStifMacroMax = 128;         // Maximum description length for
                                        // STIF TF's macro cases
const TInt KStifMacroMaxFile = 50;      // Maximum test result description
                                        // length for STIF TF's macro cases
const TInt KStifMacroMaxFunction = 30;  // Maximum test result description
                                        // length for STIF TF's macro cases

typedef TBuf<KStifMacroMax> TStifMacroDes;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CUiEnvProxy;

// CLASS DECLARATION

// DESCRIPTION
// CTestThreadContainer is a class, which contains the test execution thread
// and interface functions to do operations in that thread context.
class CTestThreadContainer 
       :public CBase 
    {

    public: // Enumerations

    private: // Enumerations
        enum TPanicReason
            {
            EUnknownEventCmd,
            EInvalidCTestThreadContainer,
            ECreateTrapCleanup,
            ENullTestThreadContainer,
            EReInitializingTestModule,
            ETestModuleNotInitialized,
            EInvalidTestModuleOperation,
            ENullRequest,            
            EDuplicateFail,
            EServerDied,
            ENullExecution,
            EThreadHandleOpenFail,
            };

    public: // Structured classes

        /**
        * For STIF TF's macro information
        */
        struct TTestMacro
            {
            TBool   iIndication;
            TName   iFileDes;
            TName   iFunctionDes;
            TInt    iLine;
            TInt    iReceivedError;
            };


    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CTestThreadContainer* NewL( 
            CTestModuleContainer* aModuleContainer,
            TThreadId aServerThreadId );

        /**
        * Destructor of CTestThreadContainer.
        */
        virtual ~CTestThreadContainer();


    public: // New functions
    
        /**
        * Test case execution thread thread function
        */
        static TInt ExecutionThread( TAny* aParams );        

        /**
         * UI Test case execution thread thread function
         */
         static TInt UIExecutionThread( TAny* aParams );        
        
        /**
        * Returns pointer to test cases
        */
        const RPointerArray<TTestCaseInfo>* TestCases() const;
        
        /**
        * Completes a print request. 
        * This function is called from test execution thread.
        */
        virtual void DoNotifyPrint( const TInt aPriority, 
                                    const TStifInfoName& aDes, 
                                    const TName& aBuffer
                                    );

        /**
        * Enables remote command receiving. 
        * This function is called from test execution thread.
        */
        virtual void DoRemoteReceive( TStifCommand aRemoteCommand,
                                      TParams aParams,
                                      TInt aLen,
                                      TRequestStatus& aStatus );
        
        /**
        * Cancel remote command receiving. 
        * This function is called from test execution thread.
        */
        virtual TInt DoRemoteReceiveCancel();

        /**
        * Completes a event request. 
        * This function is called from test execution thread.
        */
        virtual TInt DoNotifyEvent( TEventIf& aEvent,
                                    TRequestStatus* aStatus = NULL );
                                    
        /**
        * Cancels pending asynchronous event request. 
        * This function is called from test execution thread.
        */
        virtual void CancelEvent( TEventIf& aEvent, 
                                  TRequestStatus* aStatus );
                          
        /**
        * Set exit reason.
        */
        virtual void SetExitReason( const CTestModuleIf::TExitReason aExitReason, 
                                    const TInt aExitCode );
                                    
        /**
        * Set test behavior.
        */
        virtual TInt SetBehavior( const CTestModuleIf::TTestBehavior aType, 
                                  TAny* aPtr );

        /**
        * Get exit reason.
        */
        virtual void ExitReason (  CTestModuleIf::TExitReason& aExitReason, 
                                   TInt& aExitCode );

        /**
        * STIF TF's macro. Initialized TTestMacro.
        */
        virtual void StifMacroErrorInit();

        /**
        * STIF TF's(TL, T1L, T2L, etc )macro. Saves information for later use.
        */ 
        virtual TInt StifMacroError( TInt aMacroType,
                                     const TText8* aFile,
                                     const char* aFunction,
                                     TInt aLine,
                                     TInt aResult,
                                     TInt aExpected1,
                                     TInt aExpected2,
                                     TInt aExpected3,
                                     TInt aExpected4,
                                     TInt aExpected5 );

        /**
        * With this can be store information about test interference
        * thread to client space.
        */ 
        virtual TInt AddInterferenceThread( RThread aSTIFTestInterference );

        /**
        * With this can be remove information about test interference
        * thread from client space.
        */ 
        virtual TInt RemoveInterferenceThread( RThread aSTIFTestInterference );

        /**
        * With this can be stored information about test measurement
        * to TestServer space.
        */ 
        virtual TInt HandleMeasurementProcess(
                    const CSTIFTestMeasurement::TStifMeasurementStruct& aSTIFMeasurementInfo );

        /**
        * Completes a command request.
        * This function is called from test execution thread.
        */
        virtual void DoNotifyCommand(TCommand aCommand, const TDesC8& aParamsPckg);

        /**
         * Get test case execution arguments
         */
        virtual const TDesC& GetTestCaseArguments() const;        
        
        
        /**
        * Obtain title of currently running test case.
        */
        virtual void GetTestCaseTitleL(TDes& aTestCaseTitle);        

        /**
         * Sets thread logger.
         */
        void SetThreadLogger( CStifLogger* aThreadLogger );
        
        /**
         * Gets thread logger.
         */
        CStifLogger* GetThreadLogger();
        
        /**
         * Gets information if testserver supports UI testing.
         */
        IMPORT_C TBool UITesting();
                
        /**
         * Gets UIEnvProxy.
         */
        IMPORT_C CUiEnvProxy* GetUiEnvProxy();
    public: // Functions from base classes

    protected:  // New functions

    private:  // New functions

        /**
        * Function that initialise module in execution thread context.
        */
        TInt InitializeModuleInThread( RLibrary& aModule );

        /**
        * Function that enumerates test cases in execution thread context.
        */
        TInt EnumerateInThread();

        /**
        * Function that frees the enumeration data in execution thread context.
        */
        void FreeEnumerationDataInThread();

        /**
        * Function that executes test case in execution thread context.
        */
        TInt ExecuteTestCaseInThread();

        
        /**
        * Complete testcase.
        */
        void TestComplete( TInt aCompletionCode );
        
        /**
        * Delete a test module instance
        */
        void DeleteTestModule();
        
        /**
        * Store TRequestStatus.
        */
        void SetEventReq(  TEventDef::TEventCmdType aType, 
                           TEventIf& aEvent, 
                           TRequestStatus* aStatus );

        /**
        * Destroys the event queue. 
        * This function is called from test execution thread.
        */
        void DestroyEventQueue();
        
        /**
        * Error print.
        */
        void ErrorPrint( const TInt aPriority, TPtrC aError );

        /**
        * Panicing function for test thread.
        */ 
        static void Panic( TPanicReason aReason );
     
        /**
        * Check that server is alive.
        */ 
        void IsServerAlive() const;

        /**
        * Exception handler
        */
        static void ExceptionHandler( TExcType );

         /**
        * Modifies aRecFile and aRecFunction lenghts if nesessarily.
        */ 
        void SetMacroInformation( TInt aFileMaxLength,
                                  TInt aFuntionMaxLength,
                                  const TText8* aRecFile,
                                  const char* aRecFunction,
                                  TDes& aFile,
                                  TDes& aFunction );
        /**
        * Duplicates mutexes
        */                                 
        TInt DuplicateMutexHandles( RThread& aThread );
        
        /**
        * Executes test case using OOM simulation
        */       
        TInt ExecuteOOMTestCase( TInt aTestCaseNumber,                
                                 TInt aFirst, 
                                 TInt aLast, 
                                 TInt& aResult, 
                                 TTestResult& caseResult );
        /**
        * Leak checks for test case
        */       

        void LeakChecksForTestCase( TBool aReturnLeakCheckFail,
                            TInt aThreadHandleCountBeforeTest,
                            TInt aThreadHandleCountAfterTest,
                            TInt aRequestCountBeforeTest, 
                            TInt aRequestCountAfterTest );                           

    protected:  // Functions from base classes
        // None

    private:
        
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TThreadId aServerThreadId );

        /** 
        * C++ default constructor.
        */
        CTestThreadContainer( CTestModuleContainer* aModuleContainer );                              

        /**
         * Get module container.
         */
        CTestModuleContainer& ModuleContainer();        

        /**
        * Handles to parent.
        */
        CTestExecution& TestExecution() const;
        
    public:     // Data
        
    protected:  // Data
        // None

    private:    // Data     
        // Pointers to server 
        CTestModuleContainer*           iModuleContainer;
        
        // Pointer to test module
        CTestModuleBase*                iTestModule;            
        
        // Test cases
        RPointerArray<TTestCaseInfo>*   iCases;    
        
        // Resource checking
        TUint                           iCheckResourceFlags;

        // Internal if none provoded from testmodule        
        TRequestStatus                  iReqStatus;

         // StifLogger used in test module thread
        CStifLogger* iThreadLogger;             
        
        // Handles to server
        RThread     iServerThread;

        // Mutexes, duplicated handles
        RMutex  iPrintMutex;                // For iPrintSem semaphore handling
        //RMutex  iErrorPrintMutex;         // For iErrorPrintSem semaphore handling
        RMutex  iEventMutex;                // For iEventSem semaphore handling
        RMutex  iSndMutex;                  // For iSndSem semaphore handling
        RMutex  iRcvMutex;                  // For iRcvSem semaphore handling
        RMutex  iInterferenceMutex;         // For iInterferenceSem semaphore handling
        RMutex  iMeasurementMutex;          // For iMeasurementSem semaphore handling
        RMutex  iCommandMutex;              // For iCommandSem semaphore handling
        
        // Mutex, duplicated handle. This mutex meaning is to make sure that
        // test complete and test case cancel operations are not executed at
        // the same time.
        RMutex          iTestThreadMutex;                 
                
        // Semaphores, duplicated handles
        RSemaphore  iPrintSem;          // Signal print request availability
        RSemaphore  iErrorPrintSem;     // Signal error request availability
        RSemaphore  iEventSem;          // Signal event request availability
        RSemaphore  iSndSem;            // Signal Send request availability
        RSemaphore  iRcvSem;            // Signal Receive request availability
        RSemaphore  iInterferenceSem;   // Synchronize test interference access
        RSemaphore  iMeasurementSem;    // Synchronize test measurement access
        RSemaphore  iCommandSem;        // Signal command request availability

        // Macro information
        TTestMacro  iTestMacroInfo;

    public:     // Friend classes

    protected:  // Friend classes
        // None

    private:    // Friend classes
    	friend class CTestThreadContainerRunner;
    };

#endif // TEST_THREAD_CONTAINER_H

// End of File
