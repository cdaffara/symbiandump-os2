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
* Description: This module contains implementation of CSettingServer 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <e32svr.h>

#include "SettingServerClient.h"
#include "SettingServer.h"

#include <StifLogger.h>
#include "Logging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// Struct to pass parameters to server thread
struct TThreadStartSetting
    {
    TFileName  iName;         // Server name
    RThread    iServerThread; // The server thread
    RSemaphore iStarted;      // Startup syncronisation semaphore   
    TInt       iStartupResult;// Start-up result
    };

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================
// None

#undef LOGGER
#define LOGGER iSettingServerLogger

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: PanicServer

    Description: Panics the server. 

    Parameters: const TSettingServerPanic aPanic :in:  Panic code

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CSettingServer::PanicServer( const TSettingServerPanic aPanic )
    {

/*#ifdef USE_LOGGER
    // Check if logger is available, if so, use it.
    CStifLogger* log = (CStifLogger*) Dll::Tls();
    if (log)
        {
        log->Log(_L("SettingServer.DLL Panic %d"), aPanic);
        }
#endif
*/
    RDebug::Print( _L( "CSettingServer::PanicServer" ) );
    _LIT( KTxtServer,"CSettingServer" );
    User::Panic( KTxtServer,aPanic );

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: NewL

    Description: Returns new CSettingServer object

    Parameters: const TName& aName: in: Server name

    Return Values: None

    Errors/Exceptions: Leaves if memory allocation, ConstructL or StartL
                       leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CSettingServer* CSettingServer::NewL( const TName& aName )

    {
    CSettingServer* self = new( ELeave ) CSettingServer();
    CleanupStack::PushL( self );

    // Construct the server
    self->ConstructL();

    // Start the server
    self->StartL( aName );

    CleanupStack::Pop( self );

    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: ConstructL

    Description: Second level constructor.
 
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CSettingServer::ConstructL()
    {
    // Create logger, in Wins use HTML in HW default logger
    TLoggerSettings loggerSettings;
    // Directory must create by hand if test engine log wanted
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
    RDebug::Print( _L( "STIF SettingServer logging forced to RDebug" ) );
    loggerSettings.iEmulatorFormat = CStifLogger::ETxt;
    loggerSettings.iHardwareOutput = CStifLogger::ERDebug;
    loggerSettings.iEmulatorOutput = CStifLogger::ERDebug;
#endif
    loggerSettings.iUnicode = EFalse;
    loggerSettings.iAddTestCaseTitle = EFalse;
    iSettingServerLogger = CStifLogger::NewL( 
                        _L( "C:\\logs\\testframework\\testengine\\"),
                        _L( "SettingServer" ),
                        loggerSettings );

    // iLoggerSettings initializations to indicate is setting in use
    iLoggerSettings.iIsDefined.iCreateLogDir = EFalse;
    iLoggerSettings.iEmulatorPath.Copy( _L( "" ) );
    iLoggerSettings.iEmulatorFormat = CStifLogger::ETxt;
    iLoggerSettings.iEmulatorOutput = CStifLogger::EFile;
    iLoggerSettings.iHardwarePath.Copy( _L( "" ) );
    iLoggerSettings.iHardwareFormat = CStifLogger::ETxt;
    iLoggerSettings.iHardwareOutput = CStifLogger::EFile;
    iLoggerSettings.iOverwrite = ETrue;
    iLoggerSettings.iTimeStamp = ETrue;
    iLoggerSettings.iLineBreak = ETrue;
    iLoggerSettings.iEventRanking = EFalse;
    iLoggerSettings.iThreadId = EFalse;
    iLoggerSettings.iUnicode = EFalse;
    iLoggerSettings.iAddTestCaseTitle = EFalse;

    iLoggerSettings.iIsDefined.iPath = EFalse;
    iLoggerSettings.iIsDefined.iHwPath = EFalse;
    iLoggerSettings.iIsDefined.iFormat = EFalse;
    iLoggerSettings.iIsDefined.iHwFormat = EFalse;
    iLoggerSettings.iIsDefined.iOutput = EFalse;
    iLoggerSettings.iIsDefined.iHwOutput = EFalse;
    iLoggerSettings.iIsDefined.iOverwrite = EFalse;
    iLoggerSettings.iIsDefined.iLineBreak = EFalse;
    iLoggerSettings.iIsDefined.iTimeStamp = EFalse;
    iLoggerSettings.iIsDefined.iEventRanking = EFalse;
    iLoggerSettings.iIsDefined.iThreadId = EFalse;
    iLoggerSettings.iIsDefined.iUnicode = EFalse;
    iLoggerSettings.iIsDefined.iAddTestCaseTitle = EFalse;

    __TRACE( KInit,( _L( "-.-.-.-.-.-.-.-.-.- SettingServer log starts -.-.-.-.-.-.-.-.-.-" ) ) );
    __TRACE( KInit,( _L( "CSettingServer::ConstructL()" ) ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: CSettingServer

    Description: Constructor.

    Initialises non-zero member variables and base class with correct
    priority.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CSettingServer::CSettingServer() :
        CServer2( CSettingServer::ESettingServerSchedulerPriority )
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: ~CSettingServer

    Description: Destructor.
   
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CSettingServer::~CSettingServer()
    {
    delete iSettingServerLogger;

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: NewSessionL

    Description: Returns new session.
    
    Parameters: const TVersion &aVersion:in :Version required

    Return Values: CSharableSession* New session

    Errors/Exceptions: Leaves if invalid version
                       Leaves if CSettingServer construction leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
CSession2* CSettingServer::NewSessionL( const TVersion& aVersion,
                                        const RMessage2& /*aMessage*/ ) const

    {
    // check version is ok
    TVersion v( KSettingServerMajorVersionNumber,
                KSettingServerMinorVersionNumber,
                KSettingServerVersionNumber
               );

    if( !User::QueryVersionSupported( v, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    return CLoggerSetting::NewL( ( CSettingServer* ) this );
    }
/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: OpenSession

    Description: Opens session. Session calls calls this when session is
    properly constructed.
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CSettingServer::OpenSession()
    {
    iSessions++;

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: Logger

    Description: Return pointer to the Logger(iSettingServerLogger)

    Parameters: None

    Return Values: CStifLogger*: Pointer to Logger

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CStifLogger* CSettingServer::Logger()
    {
    return iSettingServerLogger;

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: CloseSession

    Description: Closes session. CLoggerSetting::CloseSession calls this
    to inform server class. 

    If there are not any active sessions, then stop active scheduler and
    close whole server.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Function panics if OpenSessions() and CloseSessions()
                       does not match.

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CSettingServer::CloseSession()
    {
    __ASSERT_ALWAYS ( iSessions > 0, CSettingServer::PanicServer(ETooManyCloseSessions));
    iSessions--;  

    if ( iSessions == 0 )
        {
        // Stop the active scheduler if no-one is using server anymore
        CActiveScheduler::Stop();
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: ThreadFunction

    Description: The thread function, where Setting Server lives in
    
    Parameters: TAny* aStartInfo aName: in: Start-up information
    
    Return Values: TInt: Result from server start

    Errors/Exceptions: Panics current thread if:
                       Invalid start-up information
                       Clean-up stack can't be created
                       Setting Server can't be started

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CSettingServer::ThreadFunction( TAny* aStarted )
    {
    __UHEAP_MARK;

    // Get start-up information
    TThreadStartSetting* startInfo = ( TThreadStartSetting* ) aStarted;
    __ASSERT_ALWAYS( startInfo,PanicServer( ENoStartupInformation ) );

    // Create clean-up stack
    CTrapCleanup* tc = CTrapCleanup::New();
    __ASSERT_ALWAYS( tc, PanicServer(ECreateTrapCleanup));

    // Construct and install active scheduler
    CActiveScheduler* scheduler = new CActiveScheduler;
    __ASSERT_ALWAYS( scheduler, PanicServer( EMainSchedulerError ) );
    CActiveScheduler::Install( scheduler );

    // Construct server
    CSettingServer* server = NULL;
    RDebug::Print( startInfo->iName );
    TRAPD( err, server = CSettingServer::NewL( startInfo->iName ) );
    __ASSERT_ALWAYS( !err, PanicServer( ESvrCreateServer ) );

    // Inform that we are up and running
    startInfo->iStartupResult = KErrNone;
    startInfo->iStarted.Signal();

    // Start handling requests
    CActiveScheduler::Start();

   // Execution continues from here after CActiveScheduler::Stop

    // Delete the server. This should be deleted before scheduler,
    // if server still has any active objects
    delete server;
    server = NULL;
    
    // Delete scheduler. 
    delete scheduler;
    scheduler = NULL;

    // Delete clean-up stack
    delete tc;
    tc = NULL;

    __UHEAP_MARKEND;

    return KErrNone;

    }

// ================= OTHER EXPORTED FUNCTIONS =================================

/*
-------------------------------------------------------------------------------

    Class: -

    Method: StartNewServer

    Description: Starts a new server. Server will be running its own
    thread and this functions returns when server is up and running or
    server start-up fails.

    Parameters:  TName& aServerName: inout: The name of the server
    
    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt StartNewServer( TName& aServerName )
    {
    __UHEAP_MARK;

    _LIT(KServerName, "SettingServer");

    //Ccheck server not already started
    TFindServer findServer( KServerName );
    TFullName name;
    if( findServer.Next( name ) == KErrNone )
        {   
        // Server already started, nothing to do
        aServerName = KServerName;
        __UHEAP_MARKEND;
        return KErrAlreadyExists;
        }

    // Construct start-up information object
    TThreadStartSetting* startInfo = new TThreadStartSetting();
    if( startInfo == NULL )
        {
        __UHEAP_MARKEND;
        return KErrNoMemory;
        }

    // Fill the start-up information
    startInfo->iName = KServerName;
    startInfo->iStartupResult = KErrNone;
    startInfo->iStarted.CreateLocal( 0 );

    // Create thread    
    TInt res = startInfo->iServerThread.Create( 
        startInfo->iName ,                       // Name of thread
        CSettingServer::ThreadFunction,          // Thread function
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
    

    // Set server name
    aServerName = KServerName;

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

// End of File
