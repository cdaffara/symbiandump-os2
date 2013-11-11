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
* Description: This file contains the header file of the CTestEngine.
*
*/

#ifndef TEST_ENGINE_H
#define TEST_ENGINE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <StifLogger.h>
#include <StifParser.h>
#include <stifinternal/TestServerClient.h>
#include "TestEngineClient.h"
#include "TestEngineEvent.h"
#include "STIFTestFrameworkSettings.h"
#include "SettingServerClient.h"
#include "STIFMeasurement.h"
#include "TestModuleInfo.h"

// CONSTANTS
// Heap sizes needed for creating server thread.
const TUint KTestEngineMinHeapSize = 0x10000;
const TUint KTestEngineMaxHeapSize = 0x100000;

// MACROS
// None

// DATA TYPES

// Panic reasons
enum TTestEnginePanic
    {
    EBadRequest,
    EBadDescriptor,
    EMainSchedulerError,
    ESvrCreateServer,
    ECreateTrapCleanup,
    EBadSubsessionHandle,
    ENoStartupInformation,
    EInvalidModuleName,
    EReqPending,
    EDoCancelDisorder,
    ECancelAsyncIPCError,
    EEventPanic
    };

// Struct to Test report settings
NONSHARABLE_CLASS(CTestReportSettings)
	:public CBase
    {
    public:
    
        /**
        * NewL is two-phased constructor.
        */
        static CTestReportSettings* NewL();


       ~CTestReportSettings();
       
    private:
       CTestReportSettings();
       /**
       * By default Symbian OS constructor is private.
       */
       void ConstructL();

    public:

        TBool                   iCreateTestReport; // Test report creation mode
        HBufC*                  iPath;             // Test report path
        HBufC*                  iName;             // Test report name
        CStifLogger::TLoggerType iFormat;          // Test report file type
        CStifLogger::TOutput    iOutput;           // Test report output
        TBool                   iOverwrite;        // Test report overwrite
        TBool                   iXML;              // Test report in xml format

    };

class CRebootParams
    :public CBase
    {
    public:     // Constructors and destructor

        /**
        * NewL is two-phased constructor.
        */
        static CRebootParams* NewL();

        /**
        * Destructor of CTestEngineServer.
        */
        ~CRebootParams();
        
        // Setters
        void SetTestModuleNameL( const TDesC& aName ); 
        void SetTestCaseFileNameL( const TDesC& aName ); 
        void SetTestCaseTitleL( const TDesC& aName ); 
        void SetTestCaseStateL( const TDesC& aName ); 

     
     private:    // New functions

        /** 
        * C++ default constructor.
        */
        CRebootParams();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
        
      public: // Data
        TInt iCaseNumber;
        TInt iStateCode;

        TPtrC iTestModule;
        TPtrC iTestCaseFile;
        TPtrC iTestCaseTitle;
        TPtrC iStateName;
              
      private: // Data   
        HBufC* iTestModuleBuf;
        HBufC* iTestCaseFileBuf;
        HBufC* iTestCaseTitleBuf;
        HBufC* iStateNameBuf;
        
    };

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CTestModuleController;
class CTestReport;

// CLASS DECLARATION

// DESCRIPTION
// CTestEngineServer is a server class.
// Contains an object container index which provides subsessions with object
// containers.

class CTestEngineServer
        : public CServer2
    {
    public:     // Enumerations
        enum { ETestEngineServerPriority = CActive::EPriorityHigh };

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * NewL is two-phased constructor.
        */
        static CTestEngineServer* NewL();

        /**
        * Destructor of CTestEngineServer.
        */
        ~CTestEngineServer();

    public:     // New functions

        /**
        * ThreadFunction is used to create new thread.
        */
        static TInt ThreadFunction( TAny* aStarted );

        /**
        * Return object container, guaranteed to produce object containers with
        * unique ids within the server.
        */
        CObjectCon* NewContainerL();

        /**
        * Delete container.
        */
        void DeleteContainer( CObjectCon* aContainer );

        /**
        * Inform Server that session is closed.
        */
        void SessionClosed();

        /**
        * PanicServer panics the CTestEngineServer
        */
        static void PanicServer( TTestEnginePanic aPanic );

        /**
        * Return the pointer to iLogger
        */
        CStifLogger* Logger();

    public:     // Functions from base classes

        /**
        * NewSessionL creates new CTestEngineServer session.
        */
        CSession2* NewSessionL( const TVersion &aVersion,
                                const RMessage2& aMessage ) const;

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

        /** 
        * C++ default constructor.
        */
        CTestEngineServer();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();


    public:     // Data
        // Test report settings
        CTestReportSettings*     iTestReportSettings;

        // Logger overwrite settings
        TLoggerSettings         iLoggerSettings;

    protected:  // Data

    private:    // Data
        // The server has an object container index which
        // creates an object container for each session
        CObjectConIx*           iContainerIndex; 

        // Logger instance
        CStifLogger*            iLogger;

        // Container count
        TInt                    iSessionCount;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

// DESCRIPTION
// Class used to store version information of test module.
// iTestModuleName field keeps name of test module dll
// iVersion keeps version of test module dll
        
    struct TTestModuleVersionInfo
    {
    	TFileName iTestModuleName;
    	TInt iMajor;
    	TInt iMinor;
    	TInt iBuild;
    };

// DESCRIPTION
// CTestEngine is a session class.
// Session for the CTestEngineServer server, to a single client-side session
// a session may own any number of CCounter objects

class CTestCase;
class CTestEventController;
class CTestEngine
        : public CSession2
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * NewL is two-phased constructor.
        */
        static CTestEngine* NewL( CTestEngineServer* aServer );

        /**
        * Leave with notify
        */
        void LeaveWithNotifyL( TInt aCode, const TDesC& aText );

        /**
        * If error leave with notify
        */
        void LeaveIfErrorWithNotify( TInt aCode, const TDesC& aText );

        /**
        * Leave with notify
        */
        void LeaveWithNotifyL( TInt aCode );

        /**
        * If error leave with notify
        */
        void LeaveIfErrorWithNotify( TInt aCode );

        /**
        * Close the session to CTestEngineServer.
        */
        void CloseSession();

    public:     // New functions

        /**
        * PanicClient panics the user of CTestEngine ( RTestEngine user )
        */
        void PanicClient( TTestEnginePanic aPanic, const RMessage2& aMessage ) const;

        /**
        * NumResources is used to provide reserver resources to client.
        */
        void NumResources( const RMessage2& aMessage );

        /**
        * Return the number of resources owned by the session. 
        * Required by CSession if derived class implements resource
        * mark-start and mark-end protocol.
        */
        TInt CountResources();

        /**
        * Callback to inform that enumeration of test cases is completed
        */
        void EnumerationCompleted( TInt aCount, TInt aError = KErrNone );


        /**
        * Callback to check state event status.
        */
        TBool IsStateEventAndSet( const TName& aEventName );

        /**
        * Callback to control events.
        */
        CTestEventController* CtlEventL( const TEventIf& aEvent, 
                                         TRequestStatus& aStatus );

        /**
        * Return the pointer to CStifLogger owned by CTestEngineServer
        */
        CStifLogger* Logger();

        /**
        * Return testcases.
        */
        RPointerArray<CTestCase>& TestCaseArray(){ return iTestCaseArray; }

        /**
        * Return client events.
        */
        RPointerArray<TEventMsg>& ClientEvents(){ return iClientEvents; }

        /**
        * ErrorPrint print errors. This version is used to print engine own
        * errors.
        */
        void ErrorPrint( const TInt aPriority, 
                         TPtrC aError );

        /**
        * ErrorPrint print errors. This version is used to forward errors.
        */
        void ErrorPrint( TErrorNotificationPckg aError );

        /**
        * Write the Reboot related information to file.
        */
        TInt WriteRebootParams( TTestInfo& aTestInfo,
                                TInt& aCode, TDesC& aName );

        /**
        * Read the Reboot related information from the file.
        */
        TInt ReadRebootParams( TTestInfo& aTestInfo, 
                               TDes& aStateDes,
                               TInt& aState );

        /**
        * Pause all test case(s) which are/is running.
        */
        void PauseAllTestCases();
        
        /**
        * Flush ATS logger buffers.
        */ 
        void FlushAtsLogger();
        
        /**
        * Get device reset module's DLL name.
        */
        TPtrC GetDeviceResetDllName();

        /**
        * Get measurements related information. Returns is measurement disable.
        */
        TInt StifMeasurement();
        
        /**
        * For Recovering from serious testmodulecrash situations (KErrserverTerminated (-15))
        * Clones TestModuleController
        */
        void TestModuleCrash( CTestModuleController* aTestModuleController );

        /**
        * Executes command received from test case.
        * It was created to allow test case to kill itself.
        */
        void ExecuteCommandL(TCommand aCommand, TDesC8& aParamsPckg);

        /**
        * Returns new index for test module controller.
        * This number is appended to module controller name.
        * This method is used when option to run every test case in separate
        * process is set to on.                        
        */
        TInt GetIndexForNewTestModuleController(void);

        /**
        * Return free module controller specified by given parameter.
        * If it can't be found, new one will be created.                        
        */
        CTestModuleController* GetFreeOrCreateModuleControllerL(TTestInfo& aTestInfo,
                                                                CTestModuleController** aRealModuleController);

    public:     // Functions from base classes

        /**
        * ServiceL handles the messages to CTestEngineServer
        */
        virtual void ServiceL( const RMessage2& aMessage );
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

        /** 
        * C++ default constructor.
        */
        CTestEngine();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CTestEngineServer* aServer );

        /**
        * DispatchMessageL handles the messages to CTestEngineServer
        */
        void DispatchMessageL( const RMessage2& aMessage );

        /**
        * Get module controller by name
        */
        CTestModuleController* ModuleControllerByName(
                                                const TName& aModuleName );

        /**
        * Initialize test report and logger's overwrite variables
        */
        void InitTestReportAndLoggerVarL();

        /**
        * Initialize the Test Engine.
        */
        void InitEngineL( const RMessage2& aMessage );

        /**
        * Parse Test Engine defaults from STIF initialization
        * file.
        */
        void ReadEngineDefaultsL( CStifParser* aParser,
                                    CSTIFTestFrameworkSettings* aSettings );

        /**
        * Set parsed logger's settings to TestEngine side.
        */
        void SetLoggerSettings( TLoggerSettings& aLoggerSettings );

        /**
        * Parse Test modules and module information from STIFTestFramework
        * initialization file.
        */
        void ReadTestModulesL( CStifParser* aParser );

        /**
        * 
        */
        void TestModuleConfigFileL( CTestModuleController* aModule,
                                        CStifSectionParser* aSectionParser,
                                        TDesC& aTag );

        /**
        * Sets attributes to Test Framework
        */
        void SetAttributeL( const RMessage2& aMessage );

        /**
        * Adds new Test Module
        */
        void AddTestModuleL( const RMessage2& aMessage );

        /**
        * Removes Test Module
        */
        TInt RemoveTestModuleL( const RMessage2& aMessage );
        
        /**
        * Close Test Engine
        */
        void CloseTestEngineL( TUint aHandle );

        /**
        * Adds new config file to Test Module
        */
        void AddConfigFileL( const RMessage2& aMessage );

        /**
        * Removes config file from test module
        */
        void RemoveConfigFileL( const RMessage2& aMessage );
        
        /**
        * Enumerates test cases
        */
        void EnumerateTestCasesL( const RMessage2& aMessage );

        /**
        * Get test cases
        */
        void GetTestCasesL( const RMessage2& aMessage );

        /**
        * Cancel outstanding asynchronous request
        */
        void CancelAsyncRequest( const RMessage2& aMessage );

        /**
        * Event system control.
        */
        void EventControlL( const RMessage2& aMessage );

        /**
        * Update state event list.
        */ 
        void UpdateEventL( const TEventIf& aEvent );

        /**
        * Handles error notifications
        */
        void HandleErrorNotificationL( const RMessage2& iMessage );

        /**
        * Get Logger's overwrite parameters
        */
        void LoggerSettings( const RMessage2& iMessage );

        /**
        * Close logger settings
        */
        void CloseLoggerSettings();

        /**
        * Process error queue.
        */
        void ProcessErrorQueue();

        /**
        * Resolve the correct CTestCase object
        */
        CTestCase* TestCaseByHandle( TUint aHandle, const RMessage2& aMessage );

        /**
        * Create CTestCase subsession.
        */
        void NewTestCaseL( const RMessage2& aMessage );

        /**
        * Destroy the created CTestCase subsession.
        * Cannot fail - can panic client.
        */
        void DeleteTestCase( TUint aHandle );
     
        /**
        * Parse the Reboot related information from the file.
        */
        TInt ParseRebootParamsL();

        /**
        * Set measurements related information, enable measurement.
        */
        TInt EnableStifMeasurement( const TDesC& aInfoType );

        /**
        * Set measurements related information, disable measurement.
        */
        TInt DisableStifMeasurement( const TDesC& aInfoType );

        /**
        * Parse and search for module info and fill list of modules.
        */
        void ParseTestModulesL(CStifParser* aParser, CTestModuleList* aModuleList, const TDesC& aSectionStart, const TDesC& aSectionEnd);

        /**
        * Add test case info to test report.
        */
        TInt AddTestCaseToTestReport(const RMessage2& aMessage);

    public:     // Data

    protected:  // Data

    private:    // Data

        // Test module array
        RPointerArray<CTestModuleController> iModules;

        // Object container for this session
        CObjectCon*                 iContainer;

        // Object index which stores objects (CTestCase instances)
        // for this session
        CObjectIx*                  iTestCases;
        
        // Object index which stores objects (CTestEngineSubSession instances)
        // for this session
        CObjectIx*                  iTestEngineSubSessions;
        
        // Array of CTestCase instances
        RPointerArray<CTestCase>    iTestCaseArray;

        // Pointer to owning server
        CTestEngineServer*          iTestEngineServer;

        // Test Report
        CTestReport*                iTestReport;
        // Test Report Mode
        TUint                       iReportMode;
        // Test Report Output Type
        TUint                       iReportOutput;

        // Device reset module's DLL name(Reboot)
        HBufC*                      iDeviceResetDllName;

        // Total number of resources allocated
        TInt                        iResourceCount;

        // Test Engine's initialization file
        HBufC*                      iIniFile;

        // RMessage for ongoing operation
        RMessage2                   iEnumerationMessage;

        // Test case count used in EnumerateTestCases
        TInt                        iCaseCount;
        // Error code used in EnumerateTestCases
        TInt                        iEnumError;

        // Tell if message can be completed
        TBool                       iComplete;

        // Error code for complete of message
        TInt                        iReturn;

        // State event Array
        RPointerArray<HBufC>        iStateEvents;
        
        // Client event Array
        RPointerArray<TEventMsg>    iClientEvents;

        // Number of enumerated test modules currently
        TInt                        iEnumerateModuleCount;

        // RMessage for error notifications
        TBool                       iErrorMessageAvailable;
        RMessage2                   iErrorMessage;
        RArray<TErrorNotification>    iErrorQueue;

        // Reboot path definition
        HBufC*                      iRebootPath;
        // Reboot file name definition
        HBufC*                      iRebootFilename;
        // Reboot default path definition
        TPtrC                       iRebootDefaultPath;
        // Reboot default file name definition
        TPtrC                       iRebootDefaultFilename;

        // Handle to Setting server.
        RSettingServer              iSettingServer;
        
        CRebootParams*              iRebootParams;
        TBool                       iIsTestReportGenerated;

        // For indication is measurement disable
        TInt                        iDisableMeasurement;
        
        // Default timeout value. 0 - no timeout value
        TInt64                      iDefaultTimeout;

        // Counter for created TestModuleControllers
        TInt                        iIndexTestModuleControllers;
        
        // Is this UI testing?
        // If yes, then Test Engine works in a specific way.
        // It creates new process for every test scripter's test case
        // and deletes it after test case has finished.
        TBool                       iUITestingSupport;

        // Should every test case be executed in separate process?
        // This option is similar to iUITestingSupport, however
        // it does not delete the process after test case is finished.
        TBool                       iSeparateProcesses;
        
        //RPointerArray of versions of test modules
        RPointerArray<TTestModuleVersionInfo> iTestModulesVersionInfo;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

// DESCRIPTION
// CTestEngineSubSession represents a subsession object in the CTestEngineServer.
// A session may own any number of CTestEngineSubSession objects

class CTestEngineSubSession
        : public CObject
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * NewL is two-phased constructor.
        */
        static CTestEngineSubSession* NewL( CTestEngine* aEngine );

        /**
        * Destructor of CTestCase.
        */
        ~CTestEngineSubSession();

    public:     // New functions

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

        /** 
        * C++ default constructor.
        */
    	CTestEngineSubSession( CTestEngine* aEngine);

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
    public:     // Data
    
    protected:  // Data

        // Session owning us
        CTestEngine*            iTestEngine;

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes
    };

    
// DESCRIPTION
// CTestCase represents a subsession object in the CTestEngineServer.
// A session may own any number of CTestCase objects

class CTestProgressNotifier;
class CTestCaseController;
class CTestEventNotifier;
class CTestRemoteCmdNotifier;
class CTestCommandNotifier;
class CTestCase
        : public CObject
    {
    public:     // Enumerations

    private:    // Enumerations

    public:     // Constructors and destructor

        /**
        * NewL is two-phased constructor.
        */
        static CTestCase* NewL( CTestEngine* aEngine,
            CTestModuleController* aModuleController,
            CTestReport* aTestReport,
            TTestInfo& aTestInfo,
            CTestModuleController* aRealModuleController);

        /**
        * Destructor of CTestCase.
        */
        ~CTestCase();

        /**
        * Close Test Case.
        */
        void CloseTestCase();

    public:     // New functions

        /**
        * Run test case.
        */
        void RunTestCaseL( const RMessage2& aMessage );

        /**
        * Suspend the test case execution.
        */
        TInt Pause();

        /**
        * Resume the suspended test case execution.
        */
        TInt Resume();

        /**
        * Notify progress from Test Module.
        */
        void NotifyProgressL( const RMessage2& aMessage );

        /**
        * Notify remote commands from Test Module.
        */
        void NotifyRemoteTypeL( const RMessage2& aMessage );

        /**
        * Notify remote commands from Test Module.
        */
        void NotifyRemoteMsgL( const RMessage2& aMessage );
        
        /**
        * Cancel outstanding asynchronous request.
        */
        void CancelAsyncRequest( const RMessage2& aMessage );

        /**
        * Return the name of Test Module.
        */
        const TDesC& ModuleName();

        /**
        * For event control.
        */
        void CtlEvent( const TEventIf& aEvent, TRequestStatus& aStatus );

		/**
		* Check if CtlEvent should be called
		*/
        TBool CheckCtlEvent( const TEventIf& aEvent );

        /**
        * Executes command received from test case.
        */
        void ExecuteCommandL(TCommand aCommand, TDesC8& aParamsPckg);
        
        /**
        * Return module controller and real module controller.
        */
        CTestModuleController* GetModuleControllers(CTestModuleController** aRealModuleController);
        
        /**
        * Set new module controller for test case (only in case when original controller crashed).
        */
        void ResetModuleController(CTestModuleController* aModuleController);

        /**
        * Set new real module controller for test case (only in case when original controller crashed).
        */
        void ResetRealModuleController(CTestModuleController* aRealModuleController);

    public:     // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // New functions

        /** 
        * C++ default constructor.
        */
        CTestCase( CTestEngine* aEngine,
            CTestModuleController* aModuleController,
            TTestInfo& aTestInfo,
            CTestModuleController* aRealModuleController);

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CTestReport* aTestReport, TTestInfo& aTestInfo );

        /**
        * Return the pointer to CStifLogger owned by CTestEngineServer
        */
        CStifLogger* Logger();

    public:     // Data
    
    protected:  // Data

        // Session owning us
        CTestEngine*            iTestEngine;

    private:    // Data

        // Test Module
        CTestModuleController*  iTestModule;

        // Test case controller
        CTestCaseController*    iTestCaseController;

        // Notifier for print
        CTestProgressNotifier*  iTestCasePrint;

        // Notifier for event
        CTestEventNotifier*     iTestCaseEvent;

        // Notifier for remote commands
        CTestRemoteCmdNotifier* iTestCaseRemoteCmd;

        // Handle to RTestServer
        RTestServer             iTestServer;

        // Handle to RTestExecution
        RTestExecution          iTestExecution;

        // Notifier for commands
        CTestCommandNotifier*   iTestCaseCommand;
        
        // Real Test Module (when test module controller is testscripter)
        CTestModuleController*  iRealModuleController;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // TEST_ENGINE_H

// End of File
