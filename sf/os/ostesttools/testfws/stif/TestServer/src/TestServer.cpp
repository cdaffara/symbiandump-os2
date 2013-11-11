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
* Description: This module contains implementation of CTestServer 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <e32svr.h>
#include "TestEngineClient.h"
#include <StifTestModule.h>
#include <stifinternal/TestServerClient.h>
#include "TestServer.h"
#include "TestServerCommon.h"
#include <stifinternal/TestThreadContainerRunnerFactory.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// Struct to pass parameters to server thread
struct TThreadStartTestServer
    {
    TFileName  iName;         // Server name
    RThread    iServerThread; // The server thread
    RSemaphore iStarted;      // Startup syncronisation semaphore   
    TBool      iInNewThread;  // Is thread running in new process?
    TInt       iStartupResult;// Start-up result
    TBool      iUiTesting;    // Is it testserver for UI testing
    CTestThreadContainerRunnerFactory* iTestThreadContainerRunnerFactory; // Pointer to CTestThreadContainerRunner. Defined when
																		  // iUiTesting is true
    };

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: PanicServer

    Description: Panics the server. 

    Parameters: const TTestServerPanic aPanic: in: Panic code

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestServer::PanicServer( const TTestServerPanic aPanic )
    {
    
#ifdef USE_LOGGER
    // Check if logger is available, if so, use it.
    CStifLogger* log = (CStifLogger*) Dll::Tls();
    if ( log )
        {
        log->Log( CStifLogger::ERed, _L("TestServer.DLL Panic %d"), aPanic);
        }
#endif

    RDebug::Print( _L( "CTestServer::PanicServer" ) );
    _LIT( KTxtTestServer,"CTestServer" );
    User::Panic( KTxtTestServer,aPanic );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: NewL

    Description: Returns new CTestServer object

    Parameters: const TFileName& aName: in: Server name

    Return Values: None

    Errors/Exceptions: Leaves if memory allocation, ConstructL or StartL leaves.

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestServer* CTestServer::NewL( const TFileName& aName )
    {

    CTestServer* self = new( ELeave ) CTestServer();
    CleanupStack::PushL( self );

    // Construct the server
    self->ConstructL( aName );

    // Start the server
    self->StartL( aName );

    CleanupStack::Pop( self );

    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: ConstructL

    Description: Second level constructor. Obtains pointer to library
    entrypoint.

    Parameters: const TFileName& aName: in: Server name

    Return Values: None

    Errors/Exceptions: Leaves if entrypoint can't be obtained.

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestServer::ConstructL( const TFileName& aName )
    {
    // Construct heap buffer for configuration file
    iModuleNameBuffer = HBufC::NewL( aName.Length() );
    iModuleName.Set ( iModuleNameBuffer->Des() );
    iModuleName.Copy ( aName );

    iContainerIndex = CObjectConIx::NewL();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: CTestServer

    Description: Constructor.

    Initialises non-zero member variables and base class with correct
    priority.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestServer::CTestServer() : CServer2( CTestServer::ETestServerPriority ),
                             iModuleName(0, 0),
                             iSessionCount( 0 )
    {
    iFirstTime = ETrue;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: ~CTestServer

    Description: Destructor
    Frees memory.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestServer::~CTestServer()
    {

    delete iModuleNameBuffer;
    iModuleNameBuffer = NULL;

    delete iContainerIndex;
    iContainerIndex = NULL;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: NewContainerL

    Description: Returns new container. Used to store subsessions

    Parameters: None

    Return Values: CObjectCon* New object container

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CObjectCon* CTestServer::NewContainerL()
    {
     
    CObjectCon* container = iContainerIndex->CreateL();

    iSessionCount++;

    return container;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: DeleteContainer

    Description: Deletes a container.

    Parameters: CObjectCon* aContainer: in: Container to be removed

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestServer::DeleteContainer( CObjectCon* aContainer )
    {
    iContainerIndex->Remove( aContainer );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: SessionClosed

    Description: Inform Server that session is closed.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestServer::SessionClosed()
    {
    // Decrease session count
    iSessionCount--;

    // Check if last session is closed
    if ( iSessionCount <= 0 )
        {
        // Stop the active scheduler
        // Execution will continue in ThreadFunction()
        CActiveScheduler::Stop();
        }

    }
/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: NewSessionL

    Description: Returns new session.
    
    Parameters: const TVersion &aVersion: in: Version required

    Return Values: CSharableSession* New session

    Errors/Exceptions: Leaves if invalid version or CTestModule construction
                       leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CSession2* CTestServer::NewSessionL( const TVersion& aVersion,
                                        const RMessage2& /*aMessage*/ ) const
    {
    // check version is ok
    TVersion v( KTestServerMajorVersionNumber,
                KTestServerMinorVersionNumber,
                KTestServerBuildVersionNumber
               );
    if( !User::QueryVersionSupported( v,aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    return CTestModule::NewL( ( CTestServer* ) this );
    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: ModuleName

    Description: Returns module name

    Parameters: None

    Return Values: const TDesC&* Module name

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
const TDesC& CTestServer::ModuleName() const
    {
    return iModuleName;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: FirstTime

    Description: Is module already once initialised.

    Parameters: None

    Return Values: TBool Has module initialized?

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
//@spe const TBool CTestServer::FirstTime() const
TBool CTestServer::FirstTime() const
    {
    return iFirstTime;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: ClearFirstTime

    Description: Clear module first time flag. 

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestServer::ClearFirstTime()
    {
    iFirstTime = EFalse;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: ThreadFunction

    Description: The thread function, where Test Server lives in
    
    Parameters: TAny* aStarted: in: Start-up information
    
    Return Values: TInt Result from test module

    Errors/Exceptions: Clean-up stack can't be created because cannot
                       leave, error checks are done locally.
                       Panics if:
                       invalid start-up information
                       Test Server can't be started

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestServer::ThreadFunction( TAny* aStarted )
    {

    __UHEAP_MARK;

    TInt error( KErrNone );

    // Get start-up information
    TThreadStartTestServer* startInfo = ( TThreadStartTestServer* ) aStarted;
    __ASSERT_ALWAYS( startInfo,PanicServer( ENoStartupInformation ) );

    // Create clean-up stack
    CTrapCleanup* tc = CTrapCleanup::New();
    __ASSERT_ALWAYS( tc, PanicServer(ECreateTrapCleanup));

    // Construct the logger
    TName path = _L("C:\\logs\\testframework\\testserver\\");
    TFileName name = _L("testserver_");
    name.Append ( startInfo->iName );

    // Create logger, in Wins use HTML in HW default logger
    TLoggerSettings loggerSettings;

    // Directory must create by hand if test server log wanted
    loggerSettings.iCreateLogDirectories = EFalse;

    loggerSettings.iOverwrite = ETrue;
    loggerSettings.iTimeStamp = ETrue;
    loggerSettings.iLineBreak = ETrue;
    loggerSettings.iEventRanking = EFalse;
    loggerSettings.iThreadId = EFalse;
    loggerSettings.iHardwareFormat = CStifLogger::ETxt;
#ifndef FORCE_STIF_INTERNAL_LOGGING_TO_RDEBUG
    loggerSettings.iEmulatorFormat = CStifLogger::EHtml;
    loggerSettings.iHardwareOutput = CStifLogger::EFile;
    loggerSettings.iEmulatorOutput = CStifLogger::EFile;
#else
    RDebug::Print( _L( "STIF Test Server logging forced to RDebug" ) );
    loggerSettings.iEmulatorFormat = CStifLogger::ETxt;
    loggerSettings.iHardwareOutput = CStifLogger::ERDebug;
    loggerSettings.iEmulatorOutput = CStifLogger::ERDebug;
#endif
    loggerSettings.iUnicode = EFalse;
    loggerSettings.iAddTestCaseTitle = EFalse;

    CStifLogger* logger = NULL;
    TRAP ( error, logger = CStifLogger::NewL( path, name, loggerSettings ) );

    // Thread Local Storage is used for get pointer to logger.
    Dll::SetTls ( logger );

    __TRACE( KInit,( _L( "TestServer.DLL server starting" ) ) );
    __TRACE( KInit,( CStifLogger::EBold, _L( "Loading module: %S"), &startInfo->iName ) );

    RLibrary module;
    TInt ret = KErrNone;

    TFileName newNameBuffer;
    TInt check = CheckModuleName( startInfo->iName, newNameBuffer );
    if( check == KErrNone )
        {
        // Load the module(TestScripter)
        ret = module.Load( newNameBuffer );
        }
    else
        {
        RemoveOptionalIndex(startInfo->iName, newNameBuffer);
        __TRACE(KInit, (CStifLogger::EBold, _L( "Valid module name is [%S] (extracted from [%S])"), &newNameBuffer, &startInfo->iName));
        // Load the module(Others)
        ret = module.Load(newNameBuffer);
        }

    // If test module loading fails, do not start server
    if( ret != KErrNone )
        {
         __TRACE( KError,( CStifLogger::ERed, _L( "Test module loading failed, code = %d" ), ret ) );
         __TRACE( KError,( _L( "Check that module is compiled properly and stored to correct directory and all DLLs that it requires are available" ) ) );                  

         // Error will be handled in StartNewServer
        startInfo->iStartupResult = ret;
        startInfo->iStarted.Signal();
        if ( !startInfo->iInNewThread )
            {
            startInfo->iStarted.Close();
            }
        module.Close();
        Dll::FreeTls();
        // Delete logger
        delete logger;
        logger = NULL;
        // Delete clean-up stack
        delete tc;
        tc = NULL;
        __UHEAP_MARKEND;
        return ret;
        }
    else
        {
        __TRACE( KInit,( _L( "Test module loaded correctly" ) ) );
        }

    // Verify that there is function
    CTestInterfaceFactory libEntry = ( CTestInterfaceFactory ) module.Lookup( 1 );
    if( libEntry == NULL )
        {
         // Error will be handled in StartNewServer
        __TRACE( KError,( CStifLogger::ERed, _L( "Can't find entrypoint from test module" ) ) );

        startInfo->iStartupResult = KErrNotFound;
        startInfo->iStarted.Signal();
        if ( !startInfo->iInNewThread )
            {
            startInfo->iStarted.Close();
            }
        module.Close();
        Dll::FreeTls();
        // Delete logger
        delete logger;
        logger = NULL;
        // Delete clean-up stack
        delete tc;
        tc = NULL;
        __UHEAP_MARKEND;
        return KErrNotFound;
        }

    module.Close();

    // Construct and install active scheduler
    CActiveScheduler* scheduler = new CActiveScheduler;
    __ASSERT_ALWAYS( scheduler, PanicServer( EMainSchedulerError ) );
    CActiveScheduler::Install( scheduler );

    // Construct server
    CTestServer* server = NULL;
    TRAPD( err, server = CTestServer::NewL( startInfo->iName ) );
    __ASSERT_ALWAYS( !err, PanicServer( ESvrCreateServer ) );

    server->iUiTesting = startInfo->iUiTesting;
    server->iTestThreadContainerRunnerFactory = startInfo->iTestThreadContainerRunnerFactory;
    
    // Inform that we are up and running
    startInfo->iStartupResult = KErrNone;
    startInfo->iStarted.Signal();
    if ( !startInfo->iInNewThread )
        {
        startInfo->iStarted.Close();
        }

    // Start handling requests
    CActiveScheduler::Start();

    // Execution continues from here after CActiveScheduler::Stop

    __TRACE( KVerbose,( _L( "TestServer.DLL active scheduler stopped" ) ) );

    // Delete the server
    delete server;
    server = NULL;
    __TRACE( KVerbose,( _L( "TestServer.DLL server object deleted" ) ) );

    delete scheduler;
    scheduler = NULL;
    __TRACE( KVerbose,( _L( "Active scheduler deleted" ) ) );

    __TRACE ( KInit, (_L("TestServer.DLL ThreadFunction exiting, server closing")) );

    Dll::FreeTls();
    // Delete logger
    delete logger;
    logger = NULL;
    // Delete clean-up stack
    delete tc;
    tc = NULL;

    __UHEAP_MARKEND;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: GetServerThreadId

    Description: Returns server thread id

    Parameters: None

    Return Values: TInt : thread id

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestServer::GetServerThreadId()
    {
    RThread thread; 
    return thread.Id();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: GetTestThreadContainerRunnerFactory

    Description: Returns server thread id

    Parameters: None

    Return Values: TInt : thread id

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestThreadContainerRunnerFactory* CTestServer::GetTestThreadContainerRunnerFactory()
	{
	
	return iTestThreadContainerRunnerFactory;
	}

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: UiTesting

    Description: Gets information if testserver supports UI testing

    Parameters: None

    Return Values: True if testserver supports UI testing, False if not.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TBool CTestServer::UiTesting()
	{
	
	return iUiTesting;
	}

/*
-------------------------------------------------------------------------------

    Class: CTestServer

    Method: GetUiEnvProxy

    Description: Gets UIEnvProxy

    Parameters: None

    Return Values: Pointer to UIEnvProxy.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CUiEnvProxy* CTestServer::GetUiEnvProxy()
	{
	
	return iTestThreadContainerRunnerFactory->GetUiEnvProxy();
	}


// ================= OTHER EXPORTED FUNCTIONS =================================

/*
-------------------------------------------------------------------------------

    Class: -

    Method: StartNewServer

    Description: Starts a new server. Server will be running its own
    thread and this functions returns when server is up and running or
    server start-up fails.

    Parameters: const TFileName& aModuleFileName: in: Module name
                TFileName& aServerName: in: Server name
                const TBool aInNewThread: in: Is new thread
                RSemaphore aSynchronisation: in: For synchronisation
                TBool aUiTestingServer: in: Indicates if testserver should support UI testing
                CTestThreadContainerRunnerFactory* aTestThreadContainerRunnerFactory: in: Pointer to runner factory

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt StartNewServer( const TFileName& aModuleFileName,
                              TFileName& aServerName,
                              const TBool aInNewThread,
                              RSemaphore aSynchronisation,
                              TBool aUiTestingServer,
                              CTestThreadContainerRunnerFactory* aTestThreadContainerRunnerFactory
                            )
    {

    __UHEAP_MARK;

    //Check server not already started
    TFindServer findServer( aModuleFileName );
    TFullName name;
    if( findServer.Next( name ) == KErrNone )
        {   
        // Server already started, nothing to do
        aServerName = aModuleFileName;
        __UHEAP_MARKEND;
        return KErrAlreadyExists;
        }

    // Construct start-up information object
    TThreadStartTestServer* startInfo = new TThreadStartTestServer();
    if( startInfo == NULL )
        {
        __UHEAP_MARKEND;
        return KErrNoMemory;
        }

    // Fill the start-up information
    startInfo->iName = aModuleFileName;
    startInfo->iStartupResult = KErrNone;
    startInfo->iStarted = aSynchronisation;
    startInfo->iInNewThread = aInNewThread;
	startInfo->iUiTesting = aUiTestingServer;
	startInfo->iTestThreadContainerRunnerFactory = aTestThreadContainerRunnerFactory;

    // EKA1
    if ( aInNewThread )
        {
        // Create thread    
        TInt res = startInfo->iServerThread.Create( 
            startInfo->iName ,                       // Name of thread
            CTestServer::ThreadFunction,             // Thread function
            KDefaultStackSize,                       // Stack size
            KDefaultHeapSize,                        // Heap initial size
            KMaxHeapSize,                            // Heap start max size
            startInfo                                // Parameter to thread function
            );

        // If thread creation failed
        if( res != KErrNone )
            {
            startInfo->iStarted.Close();             // Close semaphore
            delete startInfo;
            startInfo = NULL;
            __UHEAP_MARKEND;
            return res;
            }

         // Now start thread
        startInfo->iServerThread.SetPriority( EPriorityMuchMore ); 
        startInfo->iServerThread.Resume();

        // Wait until the thread is started
        startInfo->iStarted.Wait();

        // Server is started( or it has returned error )
    

        }

    // EKA2 and EKA1's HW
    else
        {
        // Call directly thread function, this starts server and
        // blocks this thread.
        // Priority is default, not known reason why should be greater
        // than default priority (Work over a year).
        CTestServer::ThreadFunction( startInfo );
        }

    // Set server name
    aServerName = aModuleFileName;

    // Free memory
    TInt r = startInfo->iStartupResult;
    startInfo->iServerThread.Close();
    startInfo->iStarted.Close();
    delete startInfo;
    startInfo = NULL;

    __UHEAP_MARKEND;

    // Return start-up result.
    return r;

    }

/*
-------------------------------------------------------------------------------

    Class: -

    Method: StartNewServer

    Description: Starts a new server. Server will be running its own
    thread and this functions returns when server is up and running or
    server start-up fails.

    Parameters: const TFileName& aModuleFileName: in: Module name
                TFileName& aServerName: in: Server name
                const TBool aInNewThread: in: Is new thread
                RSemaphore aSynchronisation: in: For synchronisation

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt StartNewServer( const TFileName& aModuleFileName,
                              TFileName& aServerName,
                              const TBool aInNewThread,
                              RSemaphore aSynchronisation
                            )
	{
	
	return StartNewServer( aModuleFileName, aServerName, aInNewThread, aSynchronisation, false, NULL );
	}


/*
-------------------------------------------------------------------------------

    Class: -

    Method: StartNewServer

    Description: Starts a new server. Server will be running its own
    thread and this functions returns when server is up and running or
    server start-up fails.

    Parameters: const TFileName& aName: in: Module name to be used
                TFileName& aServerName: out: The name of the server

    Return Values: TInt Error code / KErrNone

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt StartNewServer( const TFileName& aModuleFileName,
                              TFileName& aServerName
                            )
    {

    __UHEAP_MARK;

    RMutex startupMutex;
    TInt ret( KErrNone );
    // Global mutex already created(see CTestEngineServer::ThreadFunction).
    // Open global mutex.
    ret = startupMutex.OpenGlobal( KStifTestServerStartupMutex );
    if( ret != KErrNone )
        {
        // Not able to open mutex
        return ret;
        }

    startupMutex.Wait();

    //Check server not already started
    TFindServer findServer( aModuleFileName );
    TFullName name;
    if( findServer.Next( name ) == KErrNone )
        {
        // Server already started, nothing to do
        aServerName = aModuleFileName;
        
        // release startupmutex
        startupMutex.Signal();
        startupMutex.Close();

        __UHEAP_MARKEND;
        return KErrAlreadyExists;
        }

    RSemaphore startupSemaphore;
    startupSemaphore.CreateLocal( 0 );

    // Start server in new thread
    TInt r = StartNewServer ( aModuleFileName, aServerName, ETrue, startupSemaphore );

    // startupSemaphore will be closed inside StartNewServer when start-up is done.

    // release startupmutex
    startupMutex.Signal();
    startupMutex.Close();

    __UHEAP_MARKEND;

    // Return start-up result.
    return r;

    }

// End of File
