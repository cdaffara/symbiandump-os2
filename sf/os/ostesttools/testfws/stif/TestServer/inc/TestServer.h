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
* CTestServer.
*
*/

#ifndef TEST_SERVER_H
#define TEST_SERVER_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "PrintQueue.h"
#include <StifLogger.h>
#include "Logging.h"
#include <StifTestModule.h>
#include <StifTestInterface.h>

// CONSTANTS

// KDefaultHeapSize needed for creating server thread.
const TUint KDefaultHeapSize = 0x10000;
const TUint KMaxHeapSize     = 0x20000;

// MACROS

// FUNCTION PROTOTYPES
typedef CTestModuleBase*( *CTestInterfaceFactory )();

// DATA TYPES

// Panic reasons
enum TTestServerPanic
    {
    EBadRequest,
    EBadDescriptor,
    EDescriptorNonNumeric,
    EMainSchedulerError,
    ESvrCreateServer,
    ECreateTrapCleanup,
    EBadRemove, 
    EBadSubsessionHandle,
    ENoStartupInformation,
    EEnumerateHasNotBeenCalled,
    EClosingWhileTestRunning,
    EPrintMutexNotFreed,
    EThreadMutexNotFreed,
    EEventMutexNotFreed,
    EEventArrayMutexNotFreed,
    ETestModuleMutexNotFreed,
    EInvalidCTestExecution,
    EPrintQueueBroken,
    EInvalidRequestCancel,
    EWrongEventQueueDeleteContext,
    EReInitializingTestModule,
    ETestModuleNotInitialized,
    EInvalidTestModuleOperation,
    EWrongHeapInUse,
    ENullTestModuleFunction,
    ENullTestModuleContainer,
    ENullTestModule,
    ECannotCreateEventArray,
    EPreviousRequestStillOngoing,
    ECannotOpenServerThread,
    ENullModuleSession,
    EInvalidThreadState,
    EUnknownEventCmd,
    EUnknownRemoteCmd,
    ENullTestThreadContainer,
    ENullExecution,
    EAlreadyActive,
    };


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class TEvent;
class CTestModule;
class CTestModuleContainer;
class CTestExecution;
class CTestThreadContainer;
class CErrorPrintHandler;
class CTestThreadContainerRunnerFactory;

// CLASS DECLARATION

// DESCRIPTION
// CUnderTaker class is an active object that handles thread death
// notifications.
class CUnderTaker 
        :public CActive
    {

    public:  // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CUnderTaker* NewL( CTestModuleContainer* aContainer );

        /**
        * Destructor of CUnderTaker.
        */
        virtual ~CUnderTaker();

    public: // New functions

        /**
        * Start monitoring
        */
        TInt StartL();

        /**
        * Is undertaker completed?
        */
        TBool Completed();

		/**
        * Sets or unsets local processing mode
        */
        void SetSynchronousMode(const TBool aMode);

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CUnderTaker();
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CTestModuleContainer* aContainer );

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
         CTestModuleContainer* iModuleContainer;           // Pointer to parent
         TBool iCancelNeeded;                              // Is cancel needed?
		 TBool iSynchronousMode;                           // In synchronous mode
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };




// DESCRIPTION
// CTestModuleContainer is a class, which contains the test execution thread
// and interface functions to do operations in that thread context.
class CTestModuleContainer 
       :public CActive  
    {

    public: // Enumerations
        enum TOperation
            {
            EInitializeModule,
            EEnumerateInThread,
            EExecuteTestInThread,
            EFreeEnumerationData,
            EExit,
            };
        
        enum TRequestType
            {
            ERqTestCase,
            ERqErrorPrint,
            };

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CTestModuleContainer* NewL( const TDesC& aName, 
                                           CTestModule* aSession,
                                           const TDesC& aConfig );

        /**
        * Destructor of CTestModuleContainer.
        */
        virtual ~CTestModuleContainer();


    public: // New functions

        /**
        * Initialize the test module
        */
        TInt Initialize( const TFileName& aName,
                         const TBool aFirstTime );

        /**
        * Enumerate test cases
        */
        TInt EnumerateTestCases( const TFileName& aName );


        /**
        * Frees the enumeration data
        */
        TInt FreeEnumerationData();

        /**
        * Run a test case
        */
        void RunTestCase( const TFileName& aName,
                          const TInt aCaseNumber,
                          const RMessage2& aMessage );


        /**
        * Changes the operation, can fail
        */
        TInt ChangeOperation( const TOperation aOperation, 
                              const TFileName& aNameBuffer,
                              const TInt aInt );

        /**
        * Changes the operation
        */
		void ChangeOperationNoError( const TOperation aOperation,
                                     const TInt aInt );

        /**
        * Return the operation
        */
        //@spe const TOperation Operation();
        TOperation Operation();

        /**
        * Set a new execution subsession
        */
        void SetExecutionSubSession( CTestExecution* aExecution );

        /**
        * Pauses the thread
        */
        TInt PauseThread();

        /**
        * Resumes the thread
        */
        TInt ResumeThread();

        /**
        * Returns pointer to test cases
        */
        const RPointerArray<TTestCaseInfo>* TestCases() const;

        /**
        * Returns error code from operation
        */
        TInt& OperationErrorResult();

        /**
        * Returns error code from module
        */
        TInt& ModuleResult();

        /**
        * Kill the thread
        */
        void KillThread( const TInt aCode );
        
        /**
        * Error print handler.
        */
        void DoErrorPrint();

        /**
        * Make sure that any of the test interference thread's won't stay
        * to run if test case is crashed of test interference object is not
        * deleted.
        */ 
        void KillTestinterferenceThread();

        /**
        * Make sure that any of the test measurement process's won't stay
        * to run if test case is crashed of test measurement object is not
        * deleted.
        */
        void KillTestMeasurement();

    public: 
    
        /**
        * Get semaphore handles.
        */
        TInt OperationStartSemHandle(){ return iOperationStartSemaphore.Handle(); };
        TInt OperationChangeSemHandle(){ return iOperationChangeSemaphore.Handle(); };
    
        /** 
        * Access to "shared" memory.
        * Called from test thread'
        */
        void SetThreadContainer( CTestThreadContainer* aThread )
            { iThreadContainer = aThread; };
        
        const TDesC& OperationName(){ return iOperationName; };

        TDes& OperationText(){ return iOperationText; };
        
        TInt& OperationIntBuffer(){ return iOperationIntBuffer; };
        
        TOperation OperationType(){ return iOperationType; };
        
        const TDesC& TestModuleName();
        const TDesC& TestModuleIniFile();
        
        CTestModuleIf::TExitReason& AllowedExitReason()
            { return iAllowedExitReason; };   

        TInt& AllowedExitCode(){ return iAllowedExitCode; };        
      
        TThreadId ServerThreadId(){ return iServerThreadId; };
        
        CTestExecution* TestExecution()
            { return iCTestExecution; };

        TRequestStatus* GetRequest( TRequestType aType );
        
        TErrorNotification& ErrorNotification(){ return iErrorPrint; };
        
        TInt ErrorPrintSemHandle(){ return iErrorPrintSem.Handle(); };
        
        /**
        * Get title of currently running test case
        */ 
        void GetTestCaseTitleL(TDes& aTestCaseTitle);

        /**
         * Gets pointer to test module
         */
        CTestModule* GetTestModule();        
        
    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const TDesC& aName, 
                         CTestModule* aSession,
                         const TDesC& aConfig );

	private:  // New functions

		/**
        * Start and wait until operation is completed.
        */
        TInt StartAndWaitOperation();

        /**
        * Completes operation from test execution thread or from undertaker
        */
        void Complete(  const TInt aCompletionCode );

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
        * Delete a test module instance
        */
		void DeleteTestModule();

        /**
        * Function that resolves test case completion code
        */
        TInt ResolveCompletionCode( TInt aLeaveCode,
                                    TInt aErrorCode );

        /**
        * Test case execution thread thread function
        */
        static TInt ExecutionThread( TAny* aParams );

        /**
        * Parse stack and heap information for TestScripter's
        * test case (config) file.
        */
		TInt ReadParametersFromScriptFileL( const TDesC& aConfig,
                                            TInt& aStackSize,
											TInt& aHeapMinSize,
											TInt& aHeapMaxSize );

        /**
        * Loads dynamically testmodule and calls SetRequirements()-
		* method for test module parameter handling.
        */
        TInt ReadParametersFromTestModule( const TDesC& aModuleName,
										CTestModuleParam*& aTestModuleParam );

        /**
        * Load test module dynamically. If loading is done succesfully
		* then get test module's parameters according to version.
        * Verifies received parameters also.
        */
        TInt GetTestModuleParams( const TDesC& aModuleName,
                                    const TDesC& aConfig,
									TInt& aStackSize,
									TInt& aHeapMinSize,
									TInt& aHeapMaxSize );

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CTestModuleContainer();

    public:     // Data

    protected:  // Data
        // None

    private:    // Data     
        CTestExecution* iCTestExecution;         // Handle to "parent" test execution
        CTestModule* iCTestModule;               // Handle to "parent"  module
        CUnderTaker* iUnderTaker;                // Pointer to thread undertaker
        CTestThreadContainer*   iThreadContainer;// Pointer thread container

        RThread iThread;                         // Thread where test module lives in
        TBool iUpAndRunning;                     // Is the thread running
        TOperation iOperationType;               // What to do?

        RSemaphore iOperationStartSemaphore;     // Used to wait until something to do
        RSemaphore iOperationChangeSemaphore;    // Protects the operation changes

        TPtr iOperationName;                     // Operation descriptor buffer
        HBufC* iModuleNameBuffer;                // HBuffer for module name file;
     
        TInt iOperationIntBuffer;                // Transfers operation int to thread

        RMessage2 iMessage;                      // Message to complete, used in RunTestCase

        // Results from test module
        TInt iModuleResult;                      // Low priority result from test module
        TInt iErrorResult;                       // High priority error from operation
        TRequestStatus iOperationStatus;         // Operation ongoing?
        TThreadId iServerThreadId;               // Server thread Id. Used in completion
		TBuf<30> iOperationText;                 // Human readable current operation for
		
        CTestModuleIf::TExitReason iAllowedExitReason;          // Allowed exit reason
        TInt iAllowedExitCode;                   // Allowed exit code
        
        // Error print handler	    
        CErrorPrintHandler*         iErrorPrintHandler;
        
        RSemaphore      iErrorPrintSem;          // Synchonize error messages
        TErrorNotification  iErrorPrint;

        // Indicates is pause operation executed. This value will be checked
        // when test case is executed. Resume operation will be reset this
        // value.
        TBool  iIsPaused;

		// This active scheduler is uses as a nested one        
        CActiveSchedulerWait*       iNestedActiveScheduler;
    
    public:     // Friend classes
        friend class CUnderTaker;                // Handles the thread deaths
        friend class CErrorPrintHandler;

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None
    };


// DESCRIPTION
// CTestServer is a server class.
// Contains an object container index which provides subsessions with object containers.
class CTestServer
        :public CServer2
    {
    public: // Enumerations
        enum { ETestServerPriority = CActive::EPriorityHigh };

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CTestServer* NewL( const TFileName& aName );

        /**
        * Destructor of CTestServer.
        */
        ~CTestServer();

    public: // New functions

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
        * Removes a container. Needed to close session properly.
        */
        void DeleteContainer( CObjectCon* aContainer );
                    
        /**
        * Inform Server that session is closed.
        */
        void SessionClosed();
        
        /**
        * Returns module name
        */
        const TDesC& ModuleName() const;

        /**
        * Is module inside this server already initialised?
        */
        //@spe const TBool FirstTime() const;
        TBool FirstTime() const;

        /**
        * Clears the module first time flag.
        */
        void ClearFirstTime();

        /**
        * PanicServer panics the CTestServer
        */
        static void PanicServer( const TTestServerPanic aPanic );

        /**
        * GetServerThreadId for ServerStateHandler Active object usage
        */
        TInt GetServerThreadId ();
        
        /**
         * Gets CTestThreadContainerRunner pointer which is used in UI testing
         */
        CTestThreadContainerRunnerFactory* GetTestThreadContainerRunnerFactory();
                
        /**
         * Gets information that testserver supports UI testing or not
         */
        TBool UiTesting();
        
        /**
         * Gets pointer to UiEnvProxy.
         */
        CUiEnvProxy* GetUiEnvProxy();
    public: // Functions from base classes

        /**
        * NewSessionL creates new CTestServer session.
        */
        CSession2* NewSessionL( const TVersion &aVersion,
                                const RMessage2& aMessage ) const;

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CTestServer();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const TFileName& aName );

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        CObjectConIx* iContainerIndex;           // Pointer to container
        TPtr iModuleName;                        // Server name
        HBufC* iModuleNameBuffer;                // HBuffer for module name file;
        TBool iFirstTime;                        // Is module previously initialized?

        // Container count
        TInt                    iSessionCount;

        TBool iUiTesting;						 // Indicates if it is testserver which supports UI testing
        CTestThreadContainerRunnerFactory* iTestThreadContainerRunnerFactory; // Pointer to CTestThreadContainerRunnerFactory
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// DESCRIPTION
// CTestCasesList is a class, which contains list of test cases assosiated with
// specified config file.
NONSHARABLE_CLASS( CTestCasesList ): public CBase
    {
public:
    /**
    * NewL is first phase of two-phased constructor.
    */
    static CTestCasesList* NewL( const TDesC& aConfigFileName );
public:
    /**
    * Destructor of CTestServer.
    */
    ~CTestCasesList();

    /**
     * Appends test case.
     */
    void AppendTestCaseL( const TDesC& aTestCaseTitle );
    
    /**
     * Returns specified test case title.
     */
    const TDesC& GetTestCaseTitleL( TInt aIndex ) const;
    
    /**
     * Returns config file name.
     */
    const TDesC& GetConfigFileName() const;
    
    /**
     * Returns count of test cases.
     */
    TInt Count() const;
    
    /**
     * Resets list of test cases.
     */
    void ResetAndDestroy();
private:
    /** 
    * C++ default constructor.
    */
    CTestCasesList();
    
    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL( const TDesC& aConfigFileName );
private:
    // Config file name
    HBufC* iConfigFileName;
    // List of test cases
    RPointerArray<HBufC> iTestCases;
    };

        
// DESCRIPTION
// CTestModule is a session class.
// Session for the CTestServer server, to a single client-side session
// a session may own any number of CCounter objects
class CTestModule
        :public CSession2
    {    
    public:
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor

        /**
        * Construct a test module object.
        */
        static CTestModule* NewL( CTestServer* aServer );

        /**
        * Destructor
        */
        virtual ~CTestModule();

    public: // New functions

        /**
        * Dispatch message
        */
        TInt DispatchMessageL( const RMessage2& aMessage );

        /**
        * Create Module Session
        */
        TInt CreateModuleSessionL( const RMessage2& aMessage );

        /**
        * Close the session to CTestServer.
        */
        TInt CloseSession( const RMessage2& aMessage );


        /**
        * Create CTestExecution subsession.
        */
        TInt NewTestExecutionL( const RMessage2& aMessage );

        /**
        * Destroy the created CTestExecution subsession.
        * Cannot fail - can panic client.
        */
        void DeleteTestExecution( const TUint aHandle, const RMessage2& aMessage );

        /**
        * NumResources is used to provide reserver resources to client.
        */
        void NumResources( const RMessage2& aMessage );

        /**
        * Return the number of resources owned by the session.
        */
        TInt CountResources();

        /**
        * Enumerates test cases
        */
        TInt EnumerateTestCasesL( const RMessage2& aMessage );

        /**
        * Get test cases
        */
        TInt GetTestCasesL( const RMessage2& aMessage );

        /**
        * Handles error notifications
        */
        TInt HandleErrorNotificationL( const RMessage2& aMessage );

        /**
        * Get Server Thread Id
        */
        TInt GetServerThreadIdL( const RMessage2& aMessage );


        /**
        * Prints error notification
        */
        void ErrorPrint( const TInt aPriority, 
                         TPtrC aError );

        /**
        * Cancels asynchronous request
        */
        TInt CancelAsyncRequestL( const RMessage2& aMessage );

        /**
        * Frees a test case specific data owned by module( i.e enumeration data )
        */
        TInt FreeCaseData();

        /**
        * Returns case corresponding handle
        */
        CTestExecution* CaseFromHandle( const TUint aHandle, const RMessage2& aMessage ) const;

        /**
        * Returns module initialisation file name
        */
        const TDesC& IniName() const;

		/**
        * Returns module name
        */
		const TDesC& Name() const;

        /**
        * Returns a test module.
        */
        TInt GetTestModule( CTestModuleContainer*& aContainer,
                            const TDesC& aConfig );

        /**
        * Frees a test module.
        */
        TInt FreeTestModule( CTestModuleContainer* aModule );

        /**
        * Write is utility method.
        */
        void Write( const TAny* aPtr, const TDesC8& aDes, TInt anOffset = 0 );

        /**
        * PanicClient panics the user of CTestModule( RTestModule user )
        */
        void PanicClient( TInt aPanic, const RMessage2& aMessage ) const;

        /**
        * Get title of currently running test case
        */ 
        void GetTestCaseTitleL(TInt aTestCaseNumber, const TDesC& aConfigFile, TDes& aTestCaseTitle);

        /**
         * Get pointer to test server
         */ 
         CTestServer* GetTestServer();        
        
    public: // Functions from base classes

        /**
        * ServiceL handles the messages to CTestServer
        */
        virtual void ServiceL( const RMessage2& aMessage );


    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CTestModule();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CTestServer* aServer );

    public:   //Data
        // None

    
    protected:  // Data
        // None

    private:    // Data
        RPointerArray<CTestModuleContainer>* iTestModuleInstances; // List of FREE test modules
        CTestServer* iTestServer;                    // Pointer to "parent"
        CObjectCon* iContainer;                  // Object container for this session
        CObjectIx* iTestExecutionHandle;         // Object index( stores CTestExecutions,
                                                 // (Unclosed)subsession handle )

        TInt iResourceCount;                     // Total number of resources allocated
        TPtr iIni;                               // Module initialisation( file ) name
        HBufC* iIniBuffer;                       // HBuffer for ini name file;

        CTestModuleContainer* iEnumerateModule;  // Module used in enumeration

        RMessage2 iErrorMessage;                 // Error message

        TBool iErrorMessageAvailable;            // Error message available?
        
        RPointerArray<CTestCasesList> iTestCaseTitlesMap;    //Test case titles
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };


// CLASS DECLARATION

// DESCRIPTION
// CPrintHandler class is an active object that handles print
// indication from test thread.
class CPrintHandler 
    : public CActive
    {

    public:  // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CPrintHandler* NewL( CTestExecution& aExecution );

        /**
        * Destructor of CPrintHandler.
        */
        virtual ~CPrintHandler();

    public: // New functions

        /**
        * Start monitoring
        */
        void StartL();

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CPrintHandler( CTestExecution& aExecution );
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
         CTestExecution&        iExecution;     // Pointer to parent
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CLASS DECLARATION

// DESCRIPTION
// CEventHandler class is an active object that handles print
// indication from test thread.
class CEventHandler 
    : public CActive
    {

    public:  // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CEventHandler* NewL( CTestExecution& aExecution );

        /**
        * Destructor of CEventHandler.
        */
        virtual ~CEventHandler();

    public: // New functions

        /**
        * Start monitoring
        */
        void StartL();

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CEventHandler( CTestExecution& aExecution );
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
         CTestExecution&        iExecution;     // Pointer to parent
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CLASS DECLARATION

// DESCRIPTION
// CSndHandler class is an active object that handles print
// indication from test thread.
class CSndHandler 
    : public CActive
    {

    public:  // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CSndHandler* NewL( CTestExecution& aExecution );

        /**
        * Destructor of CSndHandler.
        */
        virtual ~CSndHandler();

    public: // New functions

        /**
        * Start monitoring
        */
        void StartL();

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CSndHandler( CTestExecution& aExecution );
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
         CTestExecution&        iExecution;     // Pointer to parent
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CLASS DECLARATION

// DESCRIPTION
// CRcvHandler class is an active object that handles print
// indication from test thread.
class CRcvHandler 
    : public CActive
    {

    public:  // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CRcvHandler* NewL( CTestExecution& aExecution );

        /**
        * Destructor of CRcvHandler.
        */
        virtual ~CRcvHandler();

    public: // New functions

        /**
        * Start monitoring
        */
        void StartL();

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CRcvHandler( CTestExecution& aExecution );
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
         CTestExecution&        iExecution;     // Pointer to parent
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };
        
// CLASS DECLARATION

// DESCRIPTION
// CErrorPrintHandler class is an active object that handles print
// indication from test thread.
class CErrorPrintHandler 
    : public CActive
    {

    public:  // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CErrorPrintHandler* NewL( CTestModuleContainer& aContainer );

        /**
        * Destructor of CErrorPrintHandler.
        */
        virtual ~CErrorPrintHandler();

    public: // New functions

        /**
        * Start monitoring
        */
        void StartL();

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CErrorPrintHandler( CTestModuleContainer& aContainer );
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
         CTestModuleContainer&      iContainer;     // Pointer to parent
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CLASS DECLARATION

// DESCRIPTION
// CTestInterferenceHandler class is an active object that handles test
// interference
NONSHARABLE_CLASS( CInterferenceHandler ) 
    : public CActive
    {

    public:  // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CInterferenceHandler* NewL( CTestExecution& aExecution );

        /**
        * Destructor of CInterferenceHandler.
        */
        virtual ~CInterferenceHandler();

    public: // New functions

        /**
        * Start monitoring
        */
        void StartL();

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CInterferenceHandler( CTestExecution& aExecution );
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
         CTestExecution&        iExecution;     // Pointer to parent
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };


// DESCRIPTION
// CMeasurementHandler class is an active object that handles test
// measurement
NONSHARABLE_CLASS( CMeasurementHandler ) 
    : public CActive
    {

    public:  // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CMeasurementHandler* NewL( CTestExecution& aExecution );

        /**
        * Destructor of CMeasurementHandler.
        */
        virtual ~CMeasurementHandler();

    public: // New functions

        /**
        * Start monitoring
        */
        void StartL();

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CMeasurementHandler( CTestExecution& aExecution );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data
        // None

    protected:  // Data
        // None

    private:    // Data

        // Pointer to test execution side
        CTestExecution&        iExecution;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

/**
* Check is module TestScripter. Does parsing and returns new module name and
* error codes(Needed operations when creating server sessions to TestScripter).
*/
TInt CheckModuleName( const TFileName& aModuleName, TFileName& aNewModuleName );

/**
* Remove optional index appended to module name.
* If it is found (@ char) then set new module name without it.
* This feature is used when iSeparateProcesses is set in TestEngine.
*/
void RemoveOptionalIndex(const TDesC& aModuleName, TDes& aNewModuleName);

// DESCRIPTION
// CEventHandler class is an active object that handles print
// indication from test thread.
NONSHARABLE_CLASS(CCommandHandler)
    : public CActive
    {

    public:  // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CCommandHandler* NewL(CTestExecution& aExecution);

        /**
        * Destructor of CCommandHandler.
        */
        virtual ~CCommandHandler();

    public: // New functions

        /**
        * Start monitoring
        */
        void StartL();

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError(TInt aError);

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /**
        * C++ default constructor.
        */
        CCommandHandler(CTestExecution& aExecution);

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data
        // None

    protected:  // Data
        // None

    private:    // Data
         CTestExecution&        iExecution;     // Pointer to parent

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

#endif // TEST_SERVER_H

// End of File
