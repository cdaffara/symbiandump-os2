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
* Description: This module contains implementation of 
* CTestEngineServer class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <e32svr.h>
#include <f32file.h>
#include "TestEngine.h"
#include "TestEngineClient.h"
#include "TestEngineCommon.h"
#include <stifinternal/TestServerClient.h>
#include "TestModuleController.h"
#include "TestCaseController.h"
#include "TestReport.h"
#include "Logging.h"
#include "SettingServerClient.h"
//--PYTHON-- begin
#include "StifPython.h"
#include "StifPythonFunEng.h"
//--PYTHON-- end

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
_LIT( KTestModule, "TestModule=" );
_LIT( KTestCaseFile, "TestCaseFile=" );
_LIT( KTestCaseNumber, "TestCaseNumber=" );
_LIT( KTestCaseTitle, "TestCaseTitle=" );
_LIT( KStateCode, "StateCode=" );
_LIT( KStateName, "StateName=" );
    
// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
struct TThreadStartTestEngine
    {   
    RThread    iEngineThread; // The server thread
    RSemaphore iStarted;      // Startup syncronisation semaphore   
    };

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestEngine
   
    Function: ErrorPrint

    Description: ErrorPrint

    Parameters: const TInt aPriority: in: Priority of error
                TPtrC aError: in: Error description

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::ErrorPrint( const TInt aPriority, 
                              TPtrC aError )
    {
    TErrorNotification error;
    TErrorNotificationPckg errorPckg ( error );

    error.iModule = _L("TestEngine");
    error.iPriority = aPriority;
    error.iText = aError;

    ErrorPrint( errorPckg );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: ErrorPrint

    Description: Sends error notification

    Parameters: TErrorNotificationPckg aError: in: Error

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::ErrorPrint( TErrorNotificationPckg aError )
    {
    // Add new item to end of queue
     if ( iErrorQueue.Append ( aError() ) != KErrNone )
            {
            RDebug::Print (_L("Error message lost"));
            return;
            }

    ProcessErrorQueue();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModule

    Method: ProcessErrorQueue

    Description: Process error queue

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::ProcessErrorQueue()
    {
    // If message is available, complete first item from queue
    if ( iErrorMessageAvailable )
        {
        
        if ( iErrorQueue.Count() > 0 )
            {
            TErrorNotification err = iErrorQueue[0];
            TErrorNotificationPckg errPckg(err);
            iErrorQueue.Remove(0);
        
        	TInt r( KErrNone );
        
            TRAP( r, iErrorMessage.WriteL( 0, errPckg ) );

            // Do not handle errors

            iErrorMessageAvailable = EFalse;
            iErrorMessage.Complete( KErrNone );
            }
        }

    }

/*
-------------------------------------------------------------------------------
   
    Function: LeaveIfErrorWithNotify

    Description: If error leave with notify

    Parameters: TInt aCode: in: Error code
    
    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::LeaveIfErrorWithNotify( TInt aCode )
      {
      LeaveIfErrorWithNotify ( aCode, _L("Check testengine log") );

      }

/*
-------------------------------------------------------------------------------
   
    Function: LeaveIfErrorWithNotify

    Description: If error leave with notify

    Parameters: TInt aCode: in: Error code
                const TDesC& aText: in: Descriptive text

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::LeaveIfErrorWithNotify( TInt aCode,
                                          const TDesC& aText )
    {
    if ( aCode != KErrNone )
        {
           LeaveWithNotifyL ( aCode, aText );
        }

    }

/*
-------------------------------------------------------------------------------

    Function: LeaveWithNotifyL

    Description: Leave with notify

    Parameters: TInt aCode: in: Error code

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::LeaveWithNotifyL( TInt aCode )
    {
    LeaveWithNotifyL ( aCode, _L("Check testengine log") );

    }

/*
-------------------------------------------------------------------------------

    Function: LeaveWithNotifyL

    Description: Leave with notify

    Parameters: TInt aCode: in: Error code
                const TDesC& aText: in: Descriptive text

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::LeaveWithNotifyL( TInt aCode,
                                   const TDesC& aText )
    {
    ErrorPrint ( 0, aText );
    User::Leave ( aCode );

    }

#define LOGGER iLogger

/*
-------------------------------------------------------------------------------

    Function: ThreadFunction

    Description: The thread function, where Test Engine lives in.

    Parameters: TAny* aStarted: in: Start info

    Return Values: TInt: Returns always KErrNone

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestEngineServer::ThreadFunction( TAny* aStarted )
    {
//    __UHEAP_MARK;

    // Create cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();
    __ASSERT_ALWAYS( cleanup, PanicServer( ECreateTrapCleanup ) );

    // Get start-up information
    TThreadStartTestEngine* startInfo = ( TThreadStartTestEngine* ) aStarted;
    __ASSERT_ALWAYS( startInfo, PanicServer( ENoStartupInformation ) );

    // Construct and install active scheduler
    CActiveScheduler* scheduler = new CActiveScheduler;
    __ASSERT_ALWAYS( scheduler, PanicServer( EMainSchedulerError ) );
    CActiveScheduler::Install( scheduler );

    // Construct server, an active object
    CTestEngineServer* server = NULL;
    TRAPD( err, server = CTestEngineServer::NewL() );
    __ASSERT_ALWAYS( !err, PanicServer( ESvrCreateServer ) );

    // Inform that we are up and running
    startInfo->iStarted.Signal();

    // Global mutex(handling according to the name)
    RMutex startupMutex;
    // Try to create global mutex, CREATE
    TInt ret = startupMutex.CreateGlobal( KStifTestServerStartupMutex );
    if( ret != KErrNone )
        {
        // Mutex already create, OPEN
        ret = startupMutex.OpenGlobal( KStifTestServerStartupMutex );
        }
    if( ret != KErrNone )
        {
        // Not able to create or open mutex
        return ret;
        }

    // Start handling requests
    CActiveScheduler::Start();

    startupMutex.Close();

    // Should come here after RTestEngineServer is closed
    delete server;
    delete scheduler;
    delete cleanup;

//    __UHEAP_MARKEND;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineServer

    Method: CTestEngineServer

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEngineServer::CTestEngineServer() :
    CServer2( CTestEngineServer::ETestEngineServerPriority ),
    iSessionCount( 0 )
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineServer

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if CObjectConIx::NewL leaves
                       Leaves if CStifLogger::NewL leaves
                       Leaves if StartL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngineServer::ConstructL()
    {
    // Create report setting
    iTestReportSettings = CTestReportSettings::NewL();
    // Create container
    iContainerIndex = CObjectConIx::NewL();

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
    loggerSettings.iEmulatorOutput = CStifLogger::EFile;
    loggerSettings.iHardwareOutput = CStifLogger::EFile;
#else
    RDebug::Print( _L( "STIF Test Engine logging forced to RDebug" ) );
    loggerSettings.iEmulatorFormat = CStifLogger::ETxt;
    loggerSettings.iEmulatorOutput = CStifLogger::ERDebug;
    loggerSettings.iHardwareOutput = CStifLogger::ERDebug;
#endif
    loggerSettings.iUnicode = EFalse;
    loggerSettings.iAddTestCaseTitle = EFalse;

    iLogger = CStifLogger::NewL( _L( "C:\\logs\\testframework\\testengine\\"),
                                    _L( "testengine" ),
                                    loggerSettings );
    // Start Server
    StartL( KTestEngineName );

    __TRACE(KVerbose, (_L( "CTestEngineServer::ConstructL: Server Created" ) ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineServer

    Method: NewL

    Description: Two-phased constructor.

    Parameters: None

    Return Values: CTestEngineServer* : pointer to CTestEngineServer object

    Errors/Exceptions: Leaves if construction of CBufferArray fails

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEngineServer* CTestEngineServer::NewL()
    {
    CTestEngineServer* self = new ( ELeave ) CTestEngineServer();
    CleanupStack::PushL( self );      
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineServer

    Method: ~CTestEngineServer

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEngineServer::~CTestEngineServer()
    {
    delete iContainerIndex; 

    __TRACE(KAlways, ( _L( "---------------- Log Ended ----------------" ) ) );
    delete iLogger;

    delete iTestReportSettings;
    iTestReportSettings = NULL;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineServer

    Method: NewContainerL

    Description: Create new container

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if called CreateL method leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CObjectCon* CTestEngineServer::NewContainerL()
    {
    CObjectCon* container = iContainerIndex->CreateL();

    iSessionCount++;

    return container;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineServer

    Method: DeleteContainer

    Description: Deletes a container

    Parameters: CObjectCon* 

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngineServer::DeleteContainer( CObjectCon* aContainer )
    {
    iContainerIndex->Remove( aContainer );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineServer

    Method: SessionClosed

    Description: Inform Server that session is closed.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngineServer::SessionClosed()
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

    Class: CTestEngineServer

    Method: NewSessionL

    Description: Create a new client session for this server.

    Parameters: const TVersion& aVersion: in: Client side version number

    Return Values: CSharableSession* : pointer to CSharableSession object

    Errors/Exceptions: Leaves if given version is incorrect
                       Leaves if CTestEngine::NewL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CSession2* CTestEngineServer::NewSessionL( const TVersion &aVersion,
                                         const RMessage2& /*aMessage*/ ) const
    {
    // Check that version is ok
    TVersion v( KTestEngineMajorVersionNumber,
                KTestEngineMinorVersionNumber,
                KTestEngineBuildVersionNumber );
    if ( !User::QueryVersionSupported( v, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    return CTestEngine::NewL( ( CTestEngineServer* ) this );
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineServer

    Method: PanicServer

    Description: Panic the server

    Parameters: TTestEnginePanic aPanic: in: Panic number

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngineServer::PanicServer( TTestEnginePanic aPanic )
    {
    _LIT( KTestServer, "CTestEngineServer" );
    User::Panic( KTestServer, aPanic );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineServer

    Method: Logger

    Description: Return the pointer to iLogger.

    Parameters: None

    Return Values: CStifLogger*: Pointer to StifLogger

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CStifLogger* CTestEngineServer::Logger()
    {
    return iLogger;

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestEngine class member functions.

-------------------------------------------------------------------------------
*/
#undef LOGGER
#define LOGGER Logger()

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: CTestEngine

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: RThread& aClient: in: Client's thread

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
    CTestEngine::CTestEngine() :
        CSession2(),
    iReportMode( CTestReport::ETestReportFull ),
    iReportOutput( CTestReport::ETestReportNone ),
    iEnumerateModuleCount( 0 ),
    iIsTestReportGenerated( EFalse ),
    iDisableMeasurement( EEnableAll)
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: CTestEngineServer* aServer: in: Pointer to CTestEngineServer

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::ConstructL( CTestEngineServer* aServer )
    {
    // Log version info
    TInt majorV;
    TInt minorV;
    TInt buildV;
    TBuf<30> relDate;
    TStifUtil::STIFVersion(majorV, minorV, buildV, relDate);
    RDebug::Print(_L( "STIF startup... version %d.%d.%d (%S)"), majorV, minorV, buildV, &relDate);


    // Second-phase construct base class
    //CSession2::CreateL();

    iTestEngineServer = aServer;

    // Create new object index
    iTestEngineSubSessions = CObjectIx::NewL();

    // Create new object index
    iTestCases = CObjectIx::NewL();
    
    // Initialize the object container from Server
    iContainer = iTestEngineServer->NewContainerL();

    __TRACE(KInit, (_L( "STIF startup... version %d.%d.%d (%S)"), majorV, minorV, buildV, &relDate));
    __TRACE( KInit, ( _L( "CTestEngine::ConstructL: Test Engine Created" ) ) );

   
    RFs iFs;
    User::LeaveIfError( iFs.Connect() );
    _LIT(KCSteve,"C:\\Testframework\\");
    iFs.MkDirAll(KCSteve);
      
    iFs.Close();
    
    iRebootDefaultPath.Set( _L( "C:\\TestFramework\\" ) );
    iRebootDefaultFilename.Set( _L( "Reboot.txt" ) );

    TPtrC dllName;
    dllName.Set( _L( "StifHWResetStub.dll" ) );
    iDeviceResetDllName = dllName.AllocL();

    iDefaultTimeout = 0;
    iUITestingSupport = EFalse;
    iSeparateProcesses = EFalse;
    
    iIndexTestModuleControllers = 1;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: NewL

    Description: Two-phased constructor.

    Parameters: RThread& aClient : Client's thread
                CTestEngineServer* aServer : Pointer to CTestEngineServer

    Return Values: CTestEngine* : pointer to CTestEngine object

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEngine* CTestEngine::NewL( CTestEngineServer* aServer )
    {
    CTestEngine* self = new ( ELeave ) CTestEngine();
    CleanupStack::PushL( self );
    self->ConstructL( aServer );
    CleanupStack::Pop();
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: CloseSession

    Description: Close client server session to Test Engine

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::CloseSession() 
    {
    __TRACE( KInit, ( _L( "CTestEngine::CloseSession" ) ) );

    iErrorQueue.Close();

    // Delete test report
    delete iTestReport;
    iTestReport = NULL;

    // Delete device reset module's DLL name
    delete iDeviceResetDllName;
    iDeviceResetDllName = NULL;

    // Delete state events
    iStateEvents.ResetAndDestroy();
    iStateEvents.Close();

    //Delete Client events
    iClientEvents.ResetAndDestroy();
    iClientEvents.Close();

    // Remove contents of iTestCases
    if ( iTestCases )
        {
        TInt handle;
        CObject* object = NULL;
        TInt count = iTestCases->Count();

        for ( TInt i = 0; i < count; i++ )
            {
            object = iTestCases->operator[](i);
            if ( object )
                {
                handle = iTestCases->At( object );
                iTestCases->Remove( handle );       
                }
            }

        delete iTestCases;
        iTestCases = NULL;
        }
    
    iTestCaseArray.Close();    
    delete iTestEngineSubSessions;
    iTestEngineSubSessions = NULL;

    // Delete the object container
    // This provides unique ids for the objects of this session
    iTestEngineServer->DeleteContainer( iContainer );

    // Inform server that session is closed
    iTestEngineServer->SessionClosed();

    delete iIniFile;
    iIniFile = NULL;

    delete iRebootPath;
    iRebootPath = NULL;

    delete iRebootFilename;
    iRebootFilename = NULL;

    delete iRebootParams;
    iRebootParams = 0;

    __TRACE(KVerbose, ( _L( "CTestEngine::CloseSession finished" ) ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: CountResources

    Description: Resource counding

    Parameters: None

    Return Values: TInt Resource count

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestEngine::CountResources()
    {
    return iResourceCount;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: NumResources

    Description: Get resources, writes to Message()

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::NumResources( const RMessage2& aMessage )
    {

    TInt ret( 0 );

    TPckgBuf<TInt> countPckg( iResourceCount );

    TRAP( ret, aMessage.WriteL( 0, countPckg ) );

    if ( ret != KErrNone )
        {
        PanicClient( EBadDescriptor, aMessage );
        }


    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: PanicClient

    Description: Panic the client

    Parameters: TTestEnginePanic aPanic: in: Panic number

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::PanicClient( TTestEnginePanic aPanic,
                                const RMessage2& aMessage ) const
    {
    iTestEngineServer->Logger()->Log( CStifLogger::ERed, _L( "CTestEngine::PanicClient [%d]" ), aPanic );

    _LIT( KTestEngine, "CTestEngine" );

    aMessage.Panic( KTestEngine, aPanic );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: ServiceL

    Description: Message handling method that calls trapped DispatchMessageL

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Error from DispatchMessageL is trapped and handled

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::ServiceL( const RMessage2& aMessage )
    {
    TRAPD( ret, DispatchMessageL( aMessage ) );
    if ( ret != KErrNone )
        {
         __TRACE(KError, ( CStifLogger::ERed, _L( "CTestEngine::DispatchMessageL Function=[%d], left with [%d]" ),
                                                aMessage.Function(), ret ) );
        if( ret == KErrNoMemory )
            {
            __TRACE( KError, ( CStifLogger::ERed, _L( "No memory available. Some possibility to do: 1. Reduce test case count. 2. Increase StifTestEngine's heap size." ) ) );
            }
        aMessage.Complete( ret );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: DispatchMessageL

    Description: Actual message handling

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::DispatchMessageL( const RMessage2& aMessage )
    {
    iComplete = ETrue;
    iReturn = KErrNone;

    switch ( aMessage.Function() )
        {
        case ETestEngineServerCloseSession:
            {
            if ( !iIsTestReportGenerated  && iTestReport )
                {
                iTestReport->GenerateReportL();
                iIsTestReportGenerated = ETrue;
                }               
            CloseSession();
            break;
            }
        case ETestEngineCreateSubSession:
            {
            InitEngineL( aMessage );
            break;
            }
        case ETestEngineCloseSubSession:
            {
            CloseTestEngineL( aMessage.Int3() );
            break;
            }
        case ETestEngineSetAttribute:
            {
            SetAttributeL( aMessage );
            break;
            }

        case ETestEngineAddTestModule:
            {
            AddTestModuleL( aMessage );
            break;
            }
        case ETestEngineRemoveTestModule:
            {
            iReturn = RemoveTestModuleL( aMessage );
            break;
            }
        case ETestEngineAddConfigFile:
            {
            AddConfigFileL( aMessage );
            break;
            }
        case ETestEngineRemoveConfigFile:
            {
            RemoveConfigFileL( aMessage );
            break;
            }
        case ETestEngineEnumerateTestCases:
            {
            EnumerateTestCasesL( aMessage );
            break;
            }
        case ETestEngineGetTestCases:
            {
            GetTestCasesL( aMessage );
            break;
            }
        case ETestEngineCancelAsyncRequest:
            {
            CancelAsyncRequest( aMessage );
            break;
            }
        case ETestEngineEvent:
            {
            EventControlL( aMessage );
            iComplete = EFalse;
            break;
            }
        case ETestEngineErrorNotification:
            {
            HandleErrorNotificationL( aMessage );
            iComplete = EFalse;
            break;
            }
        case ETestEngineLoggerSettings:
            {
            LoggerSettings( aMessage );
            break;
            }
        case ETestEngineCloseLoggerSettings:
            {
            CloseLoggerSettings();
            break;
            }
        case ETestEngineReportTestCase:
            {
            iReturn = AddTestCaseToTestReport(aMessage);
            break;
            }
        // Subsession specific functions
        case ETestCaseCreateSubSession:
            {
            NewTestCaseL( aMessage );
            break;
            }
        case ETestCaseCloseSubSession:
            {
            DeleteTestCase( aMessage.Int3() );
            break;
            }
        case ETestCaseRunTestCase:
            {
            TestCaseByHandle( aMessage.Int3(), aMessage )->RunTestCaseL( aMessage );
            iComplete = EFalse;
            break;
            }
        case ETestCasePause:
            {
            iReturn = TestCaseByHandle( aMessage.Int3(), aMessage )->Pause();
            break;
            }
        case ETestCaseResume:
            {
            iReturn = TestCaseByHandle( aMessage.Int3(), aMessage )->Resume();
            break;
            }
        case ETestCaseNotifyProgress:
            {
            TestCaseByHandle( aMessage.Int3(), aMessage )->NotifyProgressL( aMessage );
            iComplete = EFalse;
            break;
            }
        case ETestCaseNotifyRemoteType:
            {
            TestCaseByHandle( aMessage.Int3(), aMessage )->NotifyRemoteTypeL( aMessage );
            iComplete = EFalse;
            break;
            }
        case ETestCaseNotifyRemoteMsg:
            {
            TestCaseByHandle( aMessage.Int3(), aMessage )->NotifyRemoteMsgL( aMessage );
            iComplete = EFalse;
            break;
            }
        case ETestCaseCancelAsyncRequest:
            {
            TestCaseByHandle( 
                aMessage.Int3(), aMessage )->CancelAsyncRequest( aMessage );
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    
    if ( iComplete )
        {
        aMessage.Complete( iReturn );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: InitTestReportAndLoggerVarL

    Description: Initialize Test report and Logger's overwrite parameters

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if memory allocation fails

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::InitTestReportAndLoggerVarL()
    {
    // Test report settings initialization
    iTestEngineServer->iTestReportSettings->iCreateTestReport = ETrue;
    _LIT( path, "C:\\LOGS\\TestFramework\\");
    _LIT( name, "TestReport");
    delete iTestEngineServer->iTestReportSettings->iPath;
    iTestEngineServer->iTestReportSettings->iPath = NULL;
    iTestEngineServer->iTestReportSettings->iPath = path().AllocL();
    delete iTestEngineServer->iTestReportSettings->iName;
    iTestEngineServer->iTestReportSettings->iName = NULL;
    iTestEngineServer->iTestReportSettings->iName = name().AllocL();
    iTestEngineServer->iTestReportSettings->iFormat = CStifLogger::ETxt;
    iTestEngineServer->iTestReportSettings->iOutput = CStifLogger::EFile;
    iTestEngineServer->iTestReportSettings->iOverwrite = ETrue;

    // Initializations to indicator is setting in use
    iTestEngineServer->iLoggerSettings.iIsDefined.iCreateLogDir = EFalse;
    iTestEngineServer->iLoggerSettings.iIsDefined.iPath = EFalse;
    iTestEngineServer->iLoggerSettings.iIsDefined.iHwPath = EFalse;
    iTestEngineServer->iLoggerSettings.iIsDefined.iFormat = EFalse;
    iTestEngineServer->iLoggerSettings.iIsDefined.iHwFormat = EFalse;
    iTestEngineServer->iLoggerSettings.iIsDefined.iOutput = EFalse;
    iTestEngineServer->iLoggerSettings.iIsDefined.iHwOutput = EFalse;
    iTestEngineServer->iLoggerSettings.iIsDefined.iOverwrite = EFalse;
    iTestEngineServer->iLoggerSettings.iIsDefined.iLineBreak = EFalse;
    iTestEngineServer->iLoggerSettings.iIsDefined.iTimeStamp = EFalse;
    iTestEngineServer->iLoggerSettings.iIsDefined.iEventRanking = EFalse;
    iTestEngineServer->iLoggerSettings.iIsDefined.iThreadId = EFalse;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: InitEngineL

    Description: Init the test engine

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::InitEngineL( const RMessage2& aMessage )
    {
  
    // Parse reboot file 
    ParseRebootParamsL();

    TName iniFileName;

    // Read ini file name from aMessage
    aMessage.ReadL( 0, iniFileName );
    TStifUtil::CorrectFilePathL( iniFileName );

    iIniFile = iniFileName.AllocL();

    // HBufC to TPtrC
    TPtrC iniFile( iIniFile->Des() );

    __TRACE( KInit, ( CStifLogger::EBold, _L( "CTestEngine::InitEngineL\t iIniFile=[%S]" ), iIniFile ) );

    // Connect to the Setting server and open handle(Handle will close when
    // closing TestEngine).
    TInt ret = iSettingServer.Connect();
    if ( ret != KErrNone )
        {
        User::Leave( ret );
        }

    InitTestReportAndLoggerVarL();

    if ( iniFile.Length() > 0 )
        {
        Logger()->WriteDelimiter();
        __TRACE( KInit,( _L( "Starting to parse initialization file" ) ) );

        // Set initialization file information to Setting server's side.
        ret = iSettingServer.SetIniFileInformation( iniFileName );
        if ( ret != KErrNone )
            {
            User::Leave( ret );
            }
        // Struct to Logger settigs.
        TLoggerSettings loggerSettings; 
        // Parse Logger defaults from STIF initialization file.
        ret = iSettingServer.ReadLoggerSettingsFromIniFile( loggerSettings );
        if ( ret != KErrNone )
            {
            User::Leave( ret );
            }

        // Create parser for parsing ini file
        CStifParser* parser = NULL;
        TRAPD( r, parser = CStifParser::NewL( _L(""), iniFile ) );
        if ( r != KErrNone )
            {
            __TRACE( KError,( CStifLogger::ERed, _L( "Can't open ini-file [%S], code %d" ), &iniFile, r ) );
            LeaveWithNotifyL(r);
            }

        CleanupStack::PushL( parser );

        CSTIFTestFrameworkSettings* settings = NULL;
        TRAPD( settings_ret, settings = CSTIFTestFrameworkSettings::NewL() );
        CleanupStack::PushL( settings );
        if ( settings_ret != KErrNone )
            {
            __TRACE( KError,( CStifLogger::ERed, _L( "CSTIFTestFrameworkSettings class object creation fails") ) );
            LeaveWithNotifyL( settings_ret );
            }

        ReadEngineDefaultsL( parser, settings );

        SetLoggerSettings( loggerSettings ) ;

        TRAP(r, ReadTestModulesL( parser ));
        if(r != KErrNone)
            {
            __TRACE(KError, (CStifLogger::ERed, _L("Reading test modules finished with error [%d]"), r));
            User::Leave(r);
            }

        CleanupStack::PopAndDestroy( settings );
        CleanupStack::PopAndDestroy( parser );
        __TRACE( KInit, ( _L( "Configuration file reading finished" ) ) );
        }
    else
        {
        __TRACE( KInit,( CStifLogger::EBold, _L( "Initialisation file not specified." ) ) );
        }

    Logger()->WriteDelimiter();
    
    // Create Test Reporter if allowed
    if ( iTestEngineServer->iTestReportSettings->iCreateTestReport )
        {
        TRAPD(err, iTestReport = 
            CTestReport::NewL( *(iTestEngineServer->iTestReportSettings), 
                               ( CTestReport::TTestReportMode ) iReportMode ));
        if(err!=KErrNone)
            {
            iTestReport = NULL;
            __TRACE( KInit,( CStifLogger::ERed, _L( "Test report creation failed with error: %d, test report not created." ), err ) );
            __TRACE( KInit,( CStifLogger::ERed, _L( "Check your testreport settings from testframework.ini file (e.g. TestReportFilePath)." ) ) );
            }
        }
    
    // Create test engine subsession object
    CTestEngineSubSession* testEngineSubSession =
    CTestEngineSubSession::NewL( this );
    CleanupStack::PushL( testEngineSubSession );

    // Add object to object container to generate unique id
    iContainer->AddL( testEngineSubSession );

    // Add object to object index
    // This returns a unique handle so we can get it again
    TInt handle = iTestEngineSubSessions->AddL( testEngineSubSession );

    // Write the handle to client
    TPckg<TInt> handlePckg( handle );

    TRAPD( res, aMessage.WriteL( 3, handlePckg ) );
    if ( res != KErrNone )
        {
        iTestEngineSubSessions->Remove( handle );
        PanicClient( EBadDescriptor, aMessage );
        return;
        }

    CleanupStack::Pop( testEngineSubSession );

    // Notch up another resource
    iResourceCount++;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: ReadEngineDefaults

    Description: Parse Test Engine defaults from STIF 
                 initialization file.

    Parameters: CStifParser& parser: in: CStifParser object
                CSTIFTestFrameworkSettings* aSettings: in:
                CSTIFTestFrameworkSettings object

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::ReadEngineDefaultsL( CStifParser* aParser,
                                        CSTIFTestFrameworkSettings* aSettings )
    {
    __TRACE( KInit,( _L( "" ) ) );
    __TRACE( KInit,( _L( "Start parsing engine defaults" ) ) );
    TInt get_ret( KErrNone );

    CStifSectionParser* sectionParser = NULL;

    // Parse Engine's default values
    _LIT( KDefaultsStart, "[Engine_Defaults]" );
    _LIT( KDefaultsEnd, "[End_Defaults]" );
    __TRACE( KInit,( _L( "Starting to search sections" ) ) );
    sectionParser = aParser->SectionL( KDefaultsStart, KDefaultsEnd );
    CleanupStack::PushL( sectionParser );
    if ( sectionParser )
        {
        __TRACE( KInit,( _L( "Found '%S' and '%S' sections" ), &KDefaultsStart, &KDefaultsEnd ) );

        // Get Test Report Mode
        __TRACE( KInit,( _L( "Parsing Test report mode" ) ) );
        TUint reportMode( 0 );
        get_ret = aSettings->GetReportModeL( sectionParser,
                                        _L( "TestReportMode=" ), reportMode );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Report mode: %d"), reportMode ) );
            iReportMode = reportMode;
            }
        else
            {
            __TRACE( KInit,( _L( "Report mode not found or not given" ) ) );
            }

        // Indicator to test report creation
        __TRACE( KInit,( _L( "Parsing Test report creation indicator" ) ) );
        TBool createTestReport( 0 );
        get_ret = aSettings->GetBooleanSettingsL( sectionParser,
                                _L( "CreateTestReport=" ), createTestReport );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Test report creation indicator: %d"), createTestReport ) );
            iTestEngineServer->iTestReportSettings->iCreateTestReport = createTestReport;
            }
        else
            {
            __TRACE( KInit,( _L( "Creation indicator not found or not given" ) ) );
            }

        // Get Test report path settings
        if ( iTestEngineServer->iTestReportSettings->iCreateTestReport )
            {
            __TRACE( KInit,( _L( "Parsing Test report path" ) ) );
            TPtrC path;
            get_ret = aSettings->GetFileSetting( sectionParser,
                                        _L( "TestReportFilePath=" ), path );
            if ( get_ret == KErrNone )
                {
                __TRACE( KInit,( _L( "Test report path: %S"), &path ) );
                // Delete old one before setting new one
                delete iTestEngineServer->iTestReportSettings->iPath;
				iTestEngineServer->iTestReportSettings->iPath = NULL;
                iTestEngineServer->iTestReportSettings->iPath = path.AllocL();
                }
            else
                {
                __TRACE( KInit,( _L( "Path not found or not given" ) ) );
                }
            }

        // Get Test report name settings
        if ( iTestEngineServer->iTestReportSettings->iCreateTestReport )
            {
            __TRACE( KInit,( _L( "Parsing Test report filename" ) ) );
            TPtrC name;
            get_ret = aSettings->GetFileSetting( sectionParser,
                                        _L( "TestReportFileName=" ), name );
            if ( get_ret == KErrNone )
                {
                __TRACE( KInit,( _L( "Test report filename: %S"), &name ) );
                // Delete old one before setting new one
                delete iTestEngineServer->iTestReportSettings->iName;
				iTestEngineServer->iTestReportSettings->iName = NULL;
                iTestEngineServer->iTestReportSettings->iName = name.AllocL();
                }
            else
                {
                __TRACE( KInit,( _L( "Filename not found or not given" ) ) );
                }
            }

        // Get Test report format settings
        if ( iTestEngineServer->iTestReportSettings->iCreateTestReport )
            {
            __TRACE( KInit,( _L( "Parsing Test report format" ) ) );
            CStifLogger::TLoggerType type;
            TBool xml;
            get_ret = aSettings->GetFormatL( sectionParser,
                                            _L( "TestReportFormat=" ), type, xml );
            if ( get_ret == KErrNone )
                {
                __TRACE( KInit,( _L( "Test report format: %d, xml: %d"), type, xml ) );
                iTestEngineServer->iTestReportSettings->iFormat = type;
                iTestEngineServer->iTestReportSettings->iXML = xml;
                }
            else
                {
                __TRACE( KInit,( _L( "Format not found or not given" ) ) );
                }
            }

        // Get Test report output settings
        if ( iTestEngineServer->iTestReportSettings->iCreateTestReport )
            {
            __TRACE( KInit,( _L( "Parsing Test report output" ) ) );
            CStifLogger::TOutput output;
            get_ret = aSettings->GetOutputL( sectionParser,
                                        _L( "TestReportOutput=" ), output );
            if ( get_ret == KErrNone )
                {
                __TRACE( KInit,( _L( "Test report output: %d"), output ) );
                iTestEngineServer->iTestReportSettings->iOutput = output;
                }
            else
                {
                __TRACE( KInit,( _L( "Output not found or not given" ) ) );
                }
            }

        // Get Test report file creation mode (overwrite settings)
        if ( iTestEngineServer->iTestReportSettings->iCreateTestReport )
            {
            __TRACE( KInit,( _L( "Parsing Test report file writing mode" ) ) );
            TBool overwrite;
            get_ret = aSettings->GetOverwriteL( sectionParser,
                            _L( "TestReportFileCreationMode=" ), overwrite );
            if ( get_ret == KErrNone )
                {
                __TRACE( KInit,( _L( "Test report file creation mode: %d"), overwrite ) );
                iTestEngineServer->iTestReportSettings->iOverwrite = overwrite;
                }
            else
                {
                __TRACE( KInit,( _L( "Writing mode not found or not given" ) ) );
                }
            }
        // Get device reset module's DLL name
        __TRACE( KInit,( _L( "Parsing device reset module's DLL name" ) ) );
        TPtrC deviceResetName;
        get_ret = aSettings->GetFileSetting( sectionParser,
                                        _L( "DeviceResetDllName=" ), deviceResetName );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Device reset module's name: %S"), &deviceResetName ) );
            // Delete old one before setting new one
            delete iDeviceResetDllName;
			iDeviceResetDllName = NULL;
            iDeviceResetDllName = deviceResetName.AllocL();
            }
        else
            {
            __TRACE( KInit,( _L( "Device reset module's name not found or not given" ) ) );
            }

        // Get measurement disable option
        CStifItemParser* item = NULL;
        TRAPD( ret, item = sectionParser->GetItemLineL( _L( "DisableMeasurement=" ), ENoTag ) );
        if( ( ret != KErrNone ) || ( item == NULL ) )
            {
             __TRACE( KInit,( _L( "Measurement 'DiableMeasurement=' not found or not given" ) ) );
			delete item; 
            item = NULL;
            }
        else
            {
			CleanupStack::PushL( item );			
            TPtrC string;
            ret = item->GetString( KNullDesC(), string );
            while( ret == KErrNone )
                {
                HBufC* stringHbuf = string.AllocL();
                TPtr modifiableString = stringHbuf->Des();
                modifiableString.LowerCase();

                if( modifiableString == KStifMeasurementDisableNone ||
                    modifiableString == KStifMeasurementDisableAll ||
                    modifiableString == KStifMeasurement01 ||
                    modifiableString == KStifMeasurement02 ||
                    modifiableString == KStifMeasurement03 ||
                    modifiableString == KStifMeasurement04 ||
                    modifiableString == KStifMeasurement05 ||
                    modifiableString == KStifMeasurementBappea )
                    {
                    __TRACE( KInit,( _L( "Measurement disable option: %S"), &modifiableString ) );
                    DisableStifMeasurement( modifiableString );
                    }
                else if( modifiableString == _L( "#" ) )
                    {
					delete stringHbuf;
					stringHbuf = NULL;
                    break;
                    }
                else
                    {
                    __TRACE( KInit,( _L( "Measurement disable option not found or not given" ) ) );
                    delete stringHbuf;                    
					stringHbuf = NULL;
                    break;
                    }
                delete stringHbuf;
                ret = item->GetNextString( string );
                }
			CleanupStack::PopAndDestroy( item );
			item = NULL;
            }

        // Get timeout value option
        __TRACE(KInit, (_L("Parsing default timeout value")));
        iDefaultTimeout = 0;
        item = NULL;
        TRAP(ret, item = sectionParser->GetItemLineL(_L("Timeout=" ), ENoTag));
        if( ( ret != KErrNone ) || ( item == NULL ) )
            {
            __TRACE(KInit, (_L("'Timeout' option not found or not given")));
            delete item;
            item = NULL;
            }
        else
            {
            CleanupStack::PushL( item );
            TPtrC string;
            ret = item->GetString(KNullDesC(), string);
            if(string.Length() > 0)
                {
                TLex lex(string);
                ret = lex.Val(iDefaultTimeout);
                if(ret != KErrNone)
                    {
                    __TRACE(KError, (_L("Could not convert timeout value '%S' to integer. Error %d. Ignoring this setting."), &string, ret));
                    }
                if(iDefaultTimeout < 0)
                    {
                    __TRACE(KError, (_L("'Timeout' value cannot be negative. Resetting to 0.")));
                    iDefaultTimeout = 0;
                    }
                }
            else
                {
                __TRACE(KInit, (_L("'Timeout' value not given")));
                }
            CleanupStack::PopAndDestroy( item );
            item = NULL;
            }
        __TRACE(KInit, (_L("'Timeout' value set to %d"), iDefaultTimeout));
        iDefaultTimeout *= 1000;
        
        // Get UITestingSupport value option
        __TRACE(KInit, (_L("Parsing UITestingSupport value")));
        iUITestingSupport = EFalse;
        item = NULL;
        TRAP(ret, item = sectionParser->GetItemLineL(_L("UITestingSupport=" ), ENoTag));
        if((ret != KErrNone) || (item == NULL))
            {
            __TRACE(KInit, (_L("'UITestingSupport' option not found")));
            delete item;
            item = NULL;
            }
        else
            {
            CleanupStack::PushL(item);
            TPtrC string;
            ret = item->GetString(KNullDesC(), string);
            if(string.Length() > 0)
                {
                iUITestingSupport = (string == _L("YES"));
                }
            else
                {
                __TRACE(KInit, (_L("'UITestingSupport' value not given")));
                }
            CleanupStack::PopAndDestroy(item);
            item = NULL;
            }
        __TRACE(KInit, (_L("'UITestingSupport' value set to %d"), iUITestingSupport));

        // Get SeparateProcesses value option
        __TRACE(KInit, (_L("Parsing SeparateProcesses value")));
        iSeparateProcesses = EFalse;
        item = NULL;
        TRAP(ret, item = sectionParser->GetItemLineL(_L("SeparateProcesses=" ), ENoTag));
        if((ret != KErrNone) || (item == NULL))
            {
            __TRACE(KInit, (_L("'SeparateProcesses' option not found")));
            delete item;
            item = NULL;
            }
        else
            {
            CleanupStack::PushL(item);
            TPtrC string;
            ret = item->GetString(KNullDesC(), string);
            if(string.Length() > 0)
                {
                iSeparateProcesses = (string == _L("YES"));
                }
            else
                {
                __TRACE(KInit, (_L("'SeparateProcesses' value not given")));
                }
            CleanupStack::PopAndDestroy(item);
            item = NULL;
            }
        __TRACE(KInit, (_L("'SeparateProcesses' value set to %d"), iSeparateProcesses));
        }
    else
        {
        __TRACE( KInit,( _L( "Not found '%S' and '%S' sections" ), &KDefaultsStart, &KDefaultsEnd ) );
        }
    __TRACE( KInit,( _L( "End parsing engine defaults" ) ) );
    CleanupStack::PopAndDestroy( sectionParser );
    
    // Store engine settings to SettingServer
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        __TRACE(KError, (_L("Could not connect to SettingServer [%d] and store engine settings"), ret));
        }
    else
        {
        TEngineSettings settings;
        settings.iUITestingSupport = iUITestingSupport;
        settings.iSeparateProcesses = iSeparateProcesses;
        ret = settingServer.StoreEngineSettings(settings);
        if(ret != KErrNone)
            {
            __TRACE(KError, (_L("Could not store engine settings to SettingServer [%d]"), ret));
            }
            else
            {
            __TRACE(KInit, (_L("Engine settings succesfully stored to SettingServer")));
            }
        settingServer.Close();
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: SetLoggerSettings

    Description: Set parsed logger's settings to TestEngine side.

    Parameters: TLoggerSettings& aLoggerSettings: in: Parsed logger settings

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestEngine::SetLoggerSettings( TLoggerSettings& aLoggerSettings )
    {
    iTestEngineServer->iLoggerSettings = aLoggerSettings;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: ParseTestModulesL

    Description: Parse and search for module info and fill list of modules.

    Parameters: CStifParser*     aParser:       in: CStifParser object
                CTestModuleList* aModuleList:   in: list of modules
                TPtrC&           aSectionStart: in: descriptor with start of section string
                TPTrC&           aSectionEnd:   in: descriptor with end of section string

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::ParseTestModulesL(CStifParser* aParser, CTestModuleList* aModuleList, const TDesC& aSectionStart, const TDesC& aSectionEnd)
    {
    //First let's find all modules given in Stif's ini file and store that info in CTestModuleList object
    CStifSectionParser* sectionParser = NULL;
    CStifItemParser* item = NULL;

    sectionParser = aParser->SectionL(aSectionStart, aSectionEnd);

    TBool sectionOK;

    while(sectionParser)
        {
        sectionOK = ETrue;
        __TRACE(KInit, (_L("Found '%S' and '%S' sections"), &aSectionStart, &aSectionEnd));
        CleanupStack::PushL(sectionParser);
        __TRACE(KInit, (_L("Starting to read module information")));

        // Get name of module
        _LIT(KModuleName, "ModuleName=");
        item = sectionParser->GetItemLineL(KModuleName);
        CleanupStack::PushL(item);
        if(!item)
            {
            __TRACE(KError, (CStifLogger::ERed, _L("'%S' not found from Module section. Skipping whole section."), &KModuleName));
            sectionOK = EFalse;
            }
        else
            {
            __TRACE(KInit, (_L("'%S' found"), &KModuleName));
            }

        TName moduleName;
        TInt ret(KErrNone);

        if(sectionOK)
            {
            TPtrC name;
            ret = item->GetString(KModuleName, name);
            if(ret != KErrNone)
                {
                __TRACE(KError, (CStifLogger::ERed, _L("Module name parsing ended with error [%d]. Skipping whole section"), ret));
                sectionOK = EFalse;
                }
            else
                {
                __TRACE(KInit, (_L("Module '%S' found from ini-file"), &name));
                moduleName.Copy(name);
                moduleName.LowerCase();
                ret = aModuleList->AddTestModule(moduleName);
                if(ret != KErrNone && ret != KErrAlreadyExists)
                    {
                    CleanupStack::PopAndDestroy(item);
                    __TRACE(KError, (CStifLogger::ERed, _L("Could not add module to list of modules. Error %d"), ret));
                    LeaveIfErrorWithNotify(ret);
                    }
                }
            }
        CleanupStack::PopAndDestroy(item);

        //Get pointer to added module
        if(sectionOK)
            {
            CTestModuleInfo* moduleInfo = aModuleList->GetModule(moduleName);
            if(!moduleInfo)
                {
                __TRACE(KError, (CStifLogger::ERed, _L("Could not add get module info from list")));
                LeaveIfErrorWithNotify(KErrNotFound);
                }
    
            // Get ini file, if it exists
            __TRACE(KInit, (_L("Start parsing ini file")));
            _LIT(KIniFile, "IniFile=");
            item = sectionParser->GetItemLineL(KIniFile);
            if(item)
                {
                __TRACE(KInit, (_L("'%S' found"), &KIniFile));
                CleanupStack::PushL(item);
                TPtrC iniFile;
                ret = item->GetString(KIniFile, iniFile);
                if(ret == KErrNone)
                    {
                    // Module inifile (might be empty) OK
                    TFileName filename;
                    filename.Copy(iniFile);
                    TStifUtil::CorrectFilePathL( filename );
                    filename.LowerCase();
                    __TRACE(KInit, (CStifLogger::EBold, _L("Initialization file '%S' found, file can be empty"), &iniFile));
                    moduleInfo->SetIniFile(filename);
                    }
                else
                    {
                    __TRACE(KInit, (_L("Initialization file not found")));
                    }
                CleanupStack::PopAndDestroy(item);
                }
            else
                {
                __TRACE(KInit, (_L("'%S' not found"), &KIniFile));
                }
    
            // Get config (testcase) file
            __TRACE(KInit, (_L("Start parsing cfg files")));
            TPtrC cfgTag;
            for(TInt i = 0; i < 2; i++)
                {
                //Set tag for config files
                if(i == 0)
                    {
                    cfgTag.Set(_L("ConfigFile="));
                    }
                    else
                    {
                    cfgTag.Set(_L("TestCaseFile="));
                    }
                //Read data
                item = sectionParser->GetItemLineL(cfgTag);
                while(item)
                    {
                    CleanupStack::PushL(item);
                    __TRACE(KInit, (_L("Item '%S' found"), &cfgTag));
                    TPtrC cfgFile;
                    ret = item->GetString(cfgTag, cfgFile);
                    if(ret == KErrNone)
                        {
                        TFileName ifile;
                        ifile.Copy(cfgFile);
                        TStifUtil::CorrectFilePathL( ifile );
                        ifile.LowerCase();
                        __TRACE(KInit, (_L("Configuration file '%S' found"), &ifile));
                        moduleInfo->AddCfgFile(ifile);
                        }
                    else
                        {
                        __TRACE(KInit, (_L("Configuration file not found")));
                        }
                    CleanupStack::PopAndDestroy(item);
                    item = sectionParser->GetNextItemLineL(cfgTag);
                    }
                }
    
            __TRACE(KInit, (_L("Module '%S' information read correctly"), &moduleName));
            }
        else
            {
            __TRACE(KError, (_L("Module '%S' information skipped"), &moduleName));
            }

        // Get next section...
        CleanupStack::PopAndDestroy(sectionParser);
        sectionParser = aParser->NextSectionL(aSectionStart, aSectionEnd);
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: ReadTestModules

    Description: Parse Test modules and module information from
                 STIF initialization file.

    Parameters: CStifParser& parser: in: CStifParser object

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::ReadTestModulesL(CStifParser* aParser)
    {
    __TRACE( KInit, (_L("")));
    __TRACE( KInit, (CStifLogger::EBold, _L("Start parsing test modules")));

    //Create CTestModuleList object. It will keep information about all found test modules,
    //its initialization files and test case (config) files.
    CTestModuleList* moduleList = CTestModuleList::NewL(Logger());
    CleanupStack::PushL(moduleList);

    // Parse Test Module information
    _LIT(KTestModuleStart, "[New_Module]");
    _LIT(KTestModuleEnd, "[End_Module]");

    __TRACE(KInit, (_L("Starting to search module sections")));
    TRAPD(err, ParseTestModulesL(aParser, moduleList, KTestModuleStart, KTestModuleEnd));
    if(err != KErrNone)
        {
        __TRACE(KError, (CStifLogger::ERed, _L("Parsing test modules returned error [%d]"), err));
        User::Leave(err);
        }
    __TRACE(KInit, (CStifLogger::EBold, _L("End parsing test modules")));
    __TRACE(KInit, (_L("")));

    //Now, find included test modules and also add them to CTestModuleList object.
    _LIT(KIncludeModuleStart, "[New_Include_Module]");
    _LIT(KIncludeModuleEnd, "[End_Include_Module]");

    __TRACE(KInit, (CStifLogger::EBold, _L("Start parsing included modules")));
    CTestCaseFileInfo* finfo = moduleList->GetUncheckedCfgFile();
    while(finfo)
        {
        TFileName fname;
        finfo->GetCfgFileName(fname);

        __TRACE(KInit, (_L("checking file: '%S'"), &fname));
        finfo->SetChecked();

        CStifParser* parser = NULL;

        TRAPD(err, parser = CStifParser::NewL(_L(""), fname));
        if(err == KErrNotFound)
            {
            __TRACE(KError, (CStifLogger::ERed, _L("Could not open file '%S'"), &fname));
            }
        else if(err != KErrNone)
            {
            __TRACE(KError, (CStifLogger::ERed, _L("Could not create parser for file '%S'"), &fname));
            LeaveIfErrorWithNotify(err);
            }
        else
            {
            CleanupStack::PushL(parser);

            ParseTestModulesL(parser, moduleList, KIncludeModuleStart, KIncludeModuleEnd);

            CleanupStack::PopAndDestroy(parser);
            }

        finfo = moduleList->GetUncheckedCfgFile();
        }

    __TRACE(KInit, (CStifLogger::EBold, _L("End parsing included modules")));
    __TRACE(KInit, (_L("")));

    //Now, when all modules have been found, create them
    __TRACE(KInit, (CStifLogger::EBold, _L("Start creating modules")));
    TBool afterReset = EFalse;
    if(iRebootParams)
        {
        afterReset = ETrue;
        }
    CTestModuleInfo* moduleInfo = NULL;
    TInt i;
    TInt modCnt = moduleList->Count();

    for(i = 0; i < modCnt; i++)
        {
        moduleInfo = moduleList->GetModule(i);
        if(!moduleInfo)
            {
            __TRACE(KInit, (CStifLogger::ERed, _L("Could not get module info at index %d"), i));
            TName error;
            error.AppendFormat(_L("Could not get module info at index %d"), i);
            ErrorPrint(1, error);
            continue;
            }

        TName moduleName;
        moduleInfo->GetModuleName(moduleName);

        // Create module controller
        __TRACE(KInit, (_L("Creating module controller for '%S'"), &moduleName));
		CTestModuleController* module = NULL;
		if( moduleName == _L( "testscripter" ) )
			{
			module = CTestModuleController::NewL(this, moduleName, afterReset, ETrue);
			}
		else
			{
			module = CTestModuleController::NewL(this, moduleName, afterReset);
			}
        CleanupStack::PushL(module);
        __TRACE(KInit, (_L("Module controller created")));

        // Get ini file, if exists
        __TRACE(KInit, (_L("Checking ini file")));
        TFileName ini;
        moduleInfo->GetIniFileName(ini);
        if(ini.Length() == 0)
            {
            __TRACE(KInit, (_L("Ini file not found")));
            }

        TRAPD(err, module->InitL(ini, KNullDesC));
        if(err != KErrNone)
            {
            __TRACE(KInit, (CStifLogger::ERed, _L("Module '%S' loading failed: %d"), &moduleName, err));
            TName error;
            error.AppendFormat(_L("Module '%S' loading failed: %d"), &moduleName, err);
            ErrorPrint(1, error);

            CleanupStack::PopAndDestroy(module);
            continue;
            }

        //Add test case files
        __TRACE(KInit, (_L("Checking cfg files")));
        TInt cfgCnt = moduleInfo->CountCfgFiles();
        TInt j;
        TFileName cfgFile;
        for(j = 0; j < cfgCnt; j++)
            {
            moduleInfo->GetCfgFileName(j, cfgFile);
            if(cfgFile.Length() > 0)
                {
                __TRACE(KInit, (_L("Adding config file '%S'"), &cfgFile));
                module->AddConfigFileL(cfgFile);
                }
            else
                {
                __TRACE(KInit, (_L("Got empty config filename")));
                }
            }
        if(cfgCnt == 0)
            {
            __TRACE(KInit, (_L("Cfg file not found")));
            }

        __TRACE(KInit, (_L("Module '%S' created correctly"), &moduleName));

        // Store module for later use
        User::LeaveIfError(iModules.Append(module));
        CleanupStack::Pop(module);
        }

    __TRACE(KInit, (CStifLogger::EBold, _L("End creating test modules")));
    __TRACE(KInit, (_L("")));

    //Check if there are any modules added to Stif
    if (iModules.Count() == 0)
        {
        __TRACE(KInit, (_L("Not found '%S' and '%S' sections"), &KTestModuleStart, &KTestModuleEnd));
        __TRACE(KInit, (CStifLogger::EBold, _L("Test module(s) not defined in initialisation file")));
        // Note is printed from UI, not from here anymore
        // ErrorPrint( 0 , _L("Test modules not found. Check testengine log"));
        }

    //Delete CTestModuleList object. It is not needed any more
    CleanupStack::PopAndDestroy(moduleList);
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: TestModuleConfigFileL

    Description: Add config file to test module

    Parameters: CTestModuleController* aModule: in: CTestModuleController
                                                    object.
                CStifSectionParser* aSectionParser: in: CStifSectionParser
                object.
                TDesC& aTag :in: Founded tag.

    Return Values: None

    Errors/Exceptions: Leaves if GetNextItemLineL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::TestModuleConfigFileL( CTestModuleController* aModule,
                                            CStifSectionParser* aSectionParser,
                                            TDesC& aTag )
    {
    // Add config files to Test Module
    TPtrC configFile;
    CStifItemParser* item = NULL;
    item = aSectionParser->GetItemLineL( aTag );

    TInt cfgFiles = 0;
    TFileName config;

    TInt ret( 0 );

    while ( item )
        {
        __TRACE( KInit,( _L( "Found '%S' section" ), &aTag ) );
        CleanupStack::PushL( item );
        ret = item->GetString( aTag, configFile );
        if ( ret != KErrNone )
            {
            CleanupStack::PopAndDestroy( item );
            __TRACE( KError, ( CStifLogger::ERed, _L( "File parsing left with error [%d]" ), ret ) );
            }
        else
            {
            config = configFile;
            aModule->AddConfigFileL( config );

            // Module configfile (might be empty) OK
            __TRACE( KInit,( _L( "Adding configuration file [%S]" ), &config ) );
            cfgFiles++;
            CleanupStack::PopAndDestroy( item );
            }
        item = aSectionParser->GetNextItemLineL( aTag );
        }

    // Print trace
    if ( cfgFiles == 0)
        {  
        __TRACE( KInit,( _L( "Module does not have '%S' files") , &aTag ) );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: ModuleControllerByName

    Description: Return the module controller specified by given parameter.

    Parameters: const TName& aModuleName: in: Test module name

    Return Values: CTestModuleController* : pointer to CTestModuleController

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModuleController* CTestEngine::ModuleControllerByName(
                                                    const TName& aModuleName )
    {
    TInt modules = iModules.Count();

    for ( TInt i = 0; i < modules; i++ )
        {
        if ( iModules[i]->ModuleName( aModuleName ) == aModuleName )
            {
            return iModules[i];
            }
        }
    return NULL;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: GetFreeOrCreateModuleControllerL

    Description: Return free module controller specified by given parameter.
                 If it can't be found, new one will be created.

    Parameters: TTestInfo& aTestInfo: test info structure
                CTestModuleController** aRealModuleController: for test scripter a real module controller will be returned

    Return Values: CTestModuleController* : pointer to CTestModuleController

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModuleController* CTestEngine::GetFreeOrCreateModuleControllerL(TTestInfo& aTestInfo,
                                                                     CTestModuleController** aRealModuleController)
    {
    TInt i, j;
    *aRealModuleController = NULL;
    
    TInt modules = iModules.Count();

    //Find out which way it should be processed
    //New way means that Stif is going to find free test module controller
    //(free means that it does not run any test case)
    TBool oldWay = ETrue;
    if((iUITestingSupport && aTestInfo.iModuleName.Find(KTestScripterName) == 0) 
        || (aTestInfo.iModuleName == KPythonScripter)
        || (iSeparateProcesses))
        {
        __TRACE(KInit, (_L("Module controllers handling mode: exclusive")));
        //Exclusive mode means that every test case must be run in separate module controller.
        oldWay = EFalse;
        }
    else
        {
        __TRACE(KInit, (_L("Module controllers handling mode: normal")));
        oldWay = ETrue;
        }
        
    //If option is not set and it is not python scripter case
    //(python scripter always should look for free module controller)
    //This is the old way
    if(oldWay) 
        {
        __TRACE(KInit, (_L("Find test module controller for [%S]"), &aTestInfo.iModuleName));
        for(i = 0; i < modules; i++)
            {
            if(iModules[i]->ModuleName(aTestInfo.iModuleName) == aTestInfo.iModuleName)
                {
                __TRACE(KInit, (_L("Found test module controller for [%S]"), &aTestInfo.iModuleName));
                return iModules[i];
                }
            }
        __TRACE(KInit, (_L("Test module controller for [%S] NOT FOUND"), &aTestInfo.iModuleName));
        return NULL;
        }
    
    //This is a new way
    __TRACE(KInit, (_L("Find free test module controller for [%S]"), &aTestInfo.iModuleName));

    TName moduleName(aTestInfo.iModuleName);
    CTestModuleController* parentController = NULL;

    //First find original test module controller (parent)
    for(TInt i = 0; i < modules; i++)
        {
        if(iModules[i]->ModuleName(moduleName) ==  moduleName)
            {
            parentController = iModules[i];
            __TRACE(KInit, (_L("Original (parent) module controller found [%S]"), &moduleName));
            break;
            }
        }

    //Search free module controller among parent's children
    if(parentController)
        {
        //TestScripter is handled in other way
        if(moduleName.Find(KTestScripterName) == 0)
            {
            __TRACE(KInit, (_L("This is testscripter case. Searching real module controller.")));
            *aRealModuleController = parentController->GetFreeOrCreateModuleControllerL(aTestInfo, iUITestingSupport);
            return parentController;                 
            }

        //When UITestingSupport always create new module controller!
        TInt childrenCount = parentController->iChildrenControllers.Count();
        for(TInt i = 0; i < childrenCount; i++)
            {
            if(parentController->iChildrenControllers[i]->iTestCaseCounter == 0)
                {
                if(iUITestingSupport && aTestInfo.iModuleName.Find(KTestScripterName) == 0)
                    {
                    __TRACE(KInit, (_L("Free module controller found but in UITestingSupport mode always new one will be created")));
                    }
                else
                    {
                    __TRACE(KInit, (_L("Free module controller found [%S]"), parentController->iChildrenControllers[i]->iName));
                    return parentController->iChildrenControllers[i];
                    }
                }
            else
                {
                __TRACE(KInit, (_L("Module controller [%S] found but it is not free (it runs %d test cases)"), parentController->iChildrenControllers[i]->iName, parentController->iChildrenControllers[i]->iTestCaseCounter));
                }
            }
        }
    else
        {
        __TRACE(KError, (_L("Parent module controller NOT found [%S]"), &moduleName));
        User::Leave(KErrNotFound);
        }

    //No free module controller has been found. Create new one.
    TBuf<10> ind;
    ind.Format(_L("%d"), GetIndexForNewTestModuleController());
    moduleName.Append(_L("@"));
    moduleName.Append(ind);
    __TRACE(KInit, (_L("Free module controller not found. Creating new one [%S]."), &moduleName));
            
    //Create server and active object (This uses CTestModuleController::InitL())
    CTestModuleController* module = CTestModuleController::NewL(this, moduleName, parentController->iAfterReboot);
    CleanupStack::PushL(module);
    
    TRAPD(err, module->InitL(parentController->iInifile, aTestInfo.iConfig));
    if(err != KErrNone)
        {
        __TRACE(KInit, (_L("InitL fails with error: %d for module [%S]" ), err, &moduleName));
        User::Leave(err);
        }
    __TRACE(KInit, (_L("New module controller created [%S]."), &moduleName));

    // Store module for later use
    User::LeaveIfError(parentController->iChildrenControllers.Append(module));
    CleanupStack::Pop(module);
    __TRACE(KInit, (_L("Child added to [%S] controller. Currently it has %d children:"), parentController->iName, parentController->iChildrenControllers.Count()));
    for(j = 0; j < parentController->iChildrenControllers.Count(); j++)
        {
        __TRACE(KInit, (_L("    %d. [%S]"), j + 1, parentController->iChildrenControllers[j]->iName));
        }

    return module;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: SetAttributeL

    Description: Sets attributes to Test Framework

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Leave if ReadL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::SetAttributeL( const RMessage2& aMessage )
    {
    __TRACE( KVerbose, ( _L( "CTestEngine::SetAttributeL" ) ) );

    TAttribute attribute;
    TName genericTName;

    // Read attribute from aMessage
    attribute = (TAttribute)aMessage.Int0();

    switch( attribute )
        {
        // Path attribute
        case ELogPath:
            {
            // Read path from aMessage. 
            // NOTE!If message length is over TName, ReadL will cut the message
            // to allowed size and won't return any error code or leave code.
            aMessage.ReadL( 1, genericTName );

            // Handle to Setting server.
            RSettingServer settingServer;
            // Connect to the Setting server and create session
            TInt ret = settingServer.Connect();
            if ( ret != KErrNone )
                {
                User::Leave( ret );
                }
            settingServer.SetNewIniFileSetting( genericTName );
            // Close Setting server session
            settingServer.Close();
            break;
            }
        // Reboot directory
        case ERebootPath:
            {
            // Read reboot path from aMessage. 
            // NOTE!If message length is over TName, ReadL will cut the message
            // to allowed size and won't return any error code or leave code.
            aMessage.ReadL( 1, genericTName );
            // Allocated dynamically iRebootPath size and copies aMessage path.
            iRebootPath = genericTName.AllocL();
            break;
            }
        // Reboot path
        case ERebootFilename:
            {
            // Read reboot path from aMessage. 
            // NOTE!If message length is over TName, ReadL will cut the message
            // to allowed size and won't return any error code or leave code.
            aMessage.ReadL( 1, genericTName );
            // Allocated dynamically iRebootFilename size and copies aMessage
            // filename.
            iRebootFilename = genericTName.AllocL();
            break;
            }
        // Measurement configuration info
        case EStifMeasurementOn:
            {
            // Read Measurement configuration info from aMessage. 
            aMessage.ReadL( 1, genericTName );

            // Set info to test engine
            User::LeaveIfError( EnableStifMeasurement( genericTName ) );
            break;
            }
        // Measurement configuration info
        case EStifMeasurementOff:
            {
            // Read Measurement configuration info from aMessage. 
            aMessage.ReadL( 1, genericTName );

            // Set info to test engine
            User::LeaveIfError( DisableStifMeasurement( genericTName ) );
            break;
            }
        default:
            {
            __TRACE( KVerbose, ( _L( "Not valid attribute" ) ) );
            }
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: AddTestModuleL

    Description: Adds new Test Module

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves
                       Leaves if name length is zero

    Status: Approved: Proposal

-------------------------------------------------------------------------------
*/
void CTestEngine::AddTestModuleL( const RMessage2& aMessage )
    {
    __TRACE( KVerbose, ( _L( "CTestEngine::AddTestModuleL" ) ) );
    TName name;
    TFileName iniFile;

    // Read test module name from aMessage
    aMessage.ReadL( 0, name );

    if( name.Length() == 0 )
        {
        LeaveWithNotifyL( KErrArgument );
        }
    // Read ini file name from aMessage
    aMessage.ReadL( 1, iniFile );
    
    name.LowerCase();
    // Remove optional '.DLL' from file name
    TParse parse;
    parse.Set( name, NULL, NULL );
    
    if ( parse.Ext() == _L(".dll") )
        {
        const TInt len = parse.Ext().Length();
        name.Delete ( name.Length()-len, len );
        }

    __TRACE( KInit, ( 
        _L( "CTestEngine::AddTestModuleL, Adding Module:[%S]"), &name ) );

    // Check if test module already exists
    CTestModuleController* testModule = ModuleControllerByName( name );
    if ( testModule == NULL )
        {
        TBool afterReset = EFalse;
        if( iRebootParams && iRebootParams->iTestModule == name )
            {
            afterReset = ETrue;
            }
        
        // Create module controller
		CTestModuleController* module = NULL;
		if( name == _L( "testscripter" ) )
			{
			module = CTestModuleController::NewL( this, name, afterReset, ETrue );
			}
		else
			{
			//CTestModuleController* module = 
			module = CTestModuleController::NewL( this, name, afterReset );
			}
        CleanupStack::PushL( module );

        module->InitL( iniFile, KNullDesC );

        // Store module for later use
        User::LeaveIfError( iModules.Append( module ) );
        CleanupStack::Pop( module );
        // Done
        iReturn = KErrNone;

        __TRACE( KInit, ( 
        _L( "CTestEngine::AddTestModuleL, module added correctly") ) );
        }
    else
        {
        iReturn = KErrAlreadyExists;
        __TRACE( KInit, ( 
        _L( "CTestEngine::AddTestModuleL, module already added, all ok.") ) );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: RemoveTestModuleL

    Description: Removes Test Module

    Parameters: const RMessage& aMessage: in: Server message
    
    Return Values: TInt KErrNone: No errors occurred
                        KErrNotFound: Test module not found
                        KErrInUse: Test module is in use, cannot be removed

    Errors/Exceptions: Leaves if some of called leaving methods leaves
                       Leaves if moduleName length is zero

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestEngine::RemoveTestModuleL( const RMessage2& aMessage )
    {
    __TRACE( KInit, ( _L( "CTestEngine::RemoveTestModuleL" ) ) );
    // Read Module name from message
    TName moduleName;
    aMessage.ReadL( 0, moduleName );
    if( moduleName.Length() == 0 )
        {
        LeaveWithNotifyL( KErrArgument );
        }

    moduleName.LowerCase();
    // Remove optional '.DLL' from file name
    TParse parse;
    parse.Set( moduleName, NULL, NULL );

    if ( parse.Ext() == _L(".dll") )
        {
        const TInt len = parse.Ext().Length();
        moduleName.Delete ( moduleName.Length()-len, len );
        }

    __TRACE(KInit, (_L("Going to remove module controller [%S]"), &moduleName));
    // Check that the module that will be removed exists
    TInt moduleCount = iModules.Count();
    TBool found = EFalse;
    TInt moduleIndex;
    for ( moduleIndex = 0; moduleIndex < moduleCount; moduleIndex++ )
        {
        if ( iModules[moduleIndex]->ModuleName( moduleName ) == moduleName )
            {
            found = ETrue;
            break;
            }
        }

    if ( !found )
        {
        // Test Module does not exists
        __TRACE(KInit, (_L("Module controller [%S] to be removed NOT FOUND"), &moduleName));
        return KErrNotFound;
        }
    // Check module controller and its children
    if(iModules[moduleIndex]->iTestCaseCounter > 0)
        {
        __TRACE(KInit, (_L("Cannot remove module controller [%S], it is running %d test cases"), &moduleName, iModules[moduleIndex]->iTestCaseCounter));
        return KErrInUse;
        }
    TInt j;
    for(j = 0; j < iModules[moduleIndex]->iChildrenControllers.Count(); j++)
        {
        if(iModules[moduleIndex]->iChildrenControllers[j]->iTestCaseCounter > 0)
            {
            __TRACE(KInit, (_L("Cannot remove module controller [%S], its child [%S] is running %d test cases"), &moduleName, iModules[moduleIndex]->iChildrenControllers[j]->iName, iModules[moduleIndex]->iChildrenControllers[j]->iTestCaseCounter));
            return KErrInUse;
            }
        }
    // Test cases not running so we can remove the Test Module
    __TRACE(KInit, (_L("Removing module controller [%S]"), &moduleName));
    CTestModuleController* module = iModules[moduleIndex];
    iModules.Remove(moduleIndex);
    delete module;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: CloseTestEngineL

    Description: Close test engine

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if GenerateReportL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::CloseTestEngineL( TUint aHandle )
    {
    __TRACE( KInit, ( _L( "CTestEngine::CloseTestEngineL" ) ) );
    // Remove all module controllers
    iModules.ResetAndDestroy();
    iModules.Close();

    // If test report is created
    if( iTestReport )
        {
        iTestReport->GenerateReportL();
        iIsTestReportGenerated = ETrue;
        }

    // Close Setting server, no handle available anymore
    iSettingServer.Close();

    // Get test case from container
    CTestEngineSubSession* testEngineSubSession = ( CTestEngineSubSession* )iTestEngineSubSessions->At( aHandle );

    // Do nothing if invalid handle
    if ( testEngineSubSession == NULL )
        {
        // Handle might be already deleted, so do nothing.
        return;
        }

    iTestEngineSubSessions->Remove( aHandle );

    // Decrement resource count
    iResourceCount--;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: AddConfigFileL

    Description: Adds new config file to Test Module

    Parameters: const RMessage& aMessage: in: Server Message

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves
                       Leaves if module length is zero
                       Leaves if configFile length is zero

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::AddConfigFileL( const RMessage2& aMessage )
    {
    TName module;
    TFileName configFile;

    // Read module name from aMessage
    aMessage.ReadL( 0, module );

    if( module.Length() == 0 )
        {
        LeaveWithNotifyL( KErrArgument );
        }
    // Read config file name from aMessage
    aMessage.ReadL( 1, configFile );

    if( configFile.Length() == 0 )
        {
        LeaveWithNotifyL( KErrArgument );
        }

    module.LowerCase();
    // Remove optional '.DLL' from file name
    TParse parse;
    parse.Set( module, NULL, NULL );
    
    if ( parse.Ext() == _L(".dll") )
        {
        const TInt len = parse.Ext().Length();
        module.Delete ( module.Length()-len, len );
        }
        
    __TRACE( KInit, ( _L( "Adding config file [%S] to [%S] module"), &configFile, &module ) );
    
    // Get correct test module controller
    CTestModuleController* testModule = ModuleControllerByName( module );
    if ( testModule == NULL )
        {        
        __TRACE( KError, ( CStifLogger::ERed, _L( "Added configure file has an invalid module:[%S]"), &module ) );
        LeaveWithNotifyL ( KErrNotFound );
        }

    // Add config file to Module
    testModule->AddConfigFileL( configFile );

    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: RemoveConfigFileL

    Description: Removes a config file from test module

    Parameters: const RMessage& aMessage: in: Server Message

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves
                       Leaves if module length is zero
                       Leaves if configFile length is zero

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestEngine::RemoveConfigFileL( const RMessage2& aMessage )
    {
    TName module;
    TFileName configFile;

    // Read module name from aMessage
    aMessage.ReadL( 0, module );

    if( module.Length() == 0 )
        {
        LeaveWithNotifyL( KErrArgument );
        }
    // Read config file name from aMessage
    aMessage.ReadL( 1, configFile );

    if( configFile.Length() == 0 )
        {
        LeaveWithNotifyL( KErrArgument );
        }

    module.LowerCase();
    // Remove optional '.DLL' from file name
    TParse parse;
    parse.Set( module, NULL, NULL );
    
    if ( parse.Ext() == _L(".dll") )
        {
        const TInt len = parse.Ext().Length();
        module.Delete ( module.Length()-len, len );
        }
        
    __TRACE( KInit, ( _L( "Removing config file [%S] from [%S] module"), &configFile, &module ) );
    
    // Get correct test module controller
    CTestModuleController* testModule = ModuleControllerByName( module );
    if ( testModule == NULL )
        {        
        __TRACE( KError, ( CStifLogger::ERed, _L( "Added configure file has an invalid module:[%S]"), &module ) );
        LeaveWithNotifyL ( KErrNotFound );
        }

    // Remove config file from Module
    testModule->RemoveConfigFileL( configFile );

    }


/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: EnumerateTestCasesL

    Description: Enumerates test cases

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Panics the client if enumeration is already pending
                       Leaves if called StartEnumerateL method leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestEngine::EnumerateTestCasesL( const RMessage2& aMessage )
    {
    __TRACE( KInit, ( _L( "CTestEngine::EnumerateTestCasesL" ) ) );
    // Check if enumerate is ongoing!!
    __ASSERT_ALWAYS( iEnumerateModuleCount == 0, PanicClient( EReqPending, aMessage ) );

    iCaseCount = 0;
    iEnumError = KErrNone;

    TInt count = iModules.Count();
    if ( count == 0 )
        {

        __TRACE( KInit, ( CStifLogger::EBold, _L( "Test module(s) not loaded - returning 0 test cases" ) ) );
        
        // Note is printed from UI
        // ErrorPrint(0, _L("No test modules, check testengine log"));
        // Write test case count to client
        TCaseCount countPckg( 0 );
        TRAPD( err, aMessage.WriteL( 0, countPckg ) );

        // Request will be completed in DispatchMessageL
        iReturn = err;
        }
    else
        {
        for ( TInt i = 0; i < count; i++ )
            {
            // Send enumerate request to each test module.
            iModules[i]->StartEnumerateL();
            iEnumerateModuleCount++;
            }
    
        // Message is completed later from EnumerationCompleted function
        iComplete = EFalse;

        // Store the message
        iEnumerationMessage = aMessage;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: EnumerationCompleted

    Description: Return the module controller specified by given parameter.

    Parameters: TInt aCount: in: Count of test cases
                TInt aError: in: Symbian OS Error code: Error from Test Module

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestEngine::EnumerationCompleted( TInt aCount, TInt aError )
    {
    // Increment test case count by aCount
    iCaseCount+= aCount;

    if ( aError != KErrNone )
        {
        iEnumError = aError;
        }

    iEnumerateModuleCount--;
    
    if ( iEnumerateModuleCount == 0 )
        {
       
        TInt attrib = CStifLogger::ENoStyle;
        if ( iCaseCount == 0 )
            {
            attrib = CStifLogger::EBold;
            }
        __TRACE( KInit, ( attrib, _L( "Test case enumeration completed, testcase count %d"), iCaseCount ) );

        // Write test case count to client
        TCaseCount countPckg( iCaseCount );
        TRAPD( err, iEnumerationMessage.WriteL( 0, countPckg ) );

        if ( err != KErrNone )
            {
            iEnumError = err;
            __TRACE( KError, ( CStifLogger::ERed,  _L( "CTestEngine::EnumerationCompleted: Failed %d"), iEnumError ) );
            }
        
        // Complete request
        iEnumerationMessage.Complete( iEnumError );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: GetTestCasesL

    Description: Get test cases

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Leaves if enumeration is not yet complete

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::GetTestCasesL( const RMessage2& aMessage )
    {
    __TRACE( KVerbose, ( _L( "CTestEngine::GetTestCasesL" ) ) );

    const TInt len = sizeof( TTestInfo );

    // Loop through all test modules
    TInt moduleCount = iModules.Count();
    TInt pos = 0;
    TInt totalCount = 0;
    for ( TInt i = 0; i < moduleCount; i++ )
        {
        CTestModuleController* module = iModules[i];

        // Check that test cases are enumerated first
        if ( !module->EnumerationComplete() )
            {
            __TRACE( KError, ( CStifLogger::ERed, _L( "CTestEngine::GetTestCasesL, Test Cases not yet enumerated!" ) ) );
            LeaveIfErrorWithNotify( KErrNotFound );
            }

        CFixedFlatArray<TTestInfo>* testCases = module->TestCasesL();
        CleanupStack::PushL( testCases );
        
        // Loop through all test cases got from test module
        const TInt caseCount = testCases->Count();
        totalCount+=caseCount;

        for ( TInt j = 0; j < caseCount; j++ )
            {
            if(((*testCases)[j]).iTestCaseInfo.iTimeout == 0 && iDefaultTimeout > 0)
                {
                ((*testCases)[j]).iTestCaseInfo.iTimeout = iDefaultTimeout;
                }

            // Construct package for source data
            TTestInfoPckg tmpPackage( ( *testCases )[j] );

            // Copy test case package to client's memory
            aMessage.WriteL( 0, tmpPackage, pos ) ;

            pos = pos + len;
            }
        CleanupStack::PopAndDestroy( testCases );

        // Free allocated test cases because not needed anymore
        module->FreeTestCases();

        }

    __TRACE( KVerbose, ( _L( "CTestEngine::GetTestCasesL, case count %d" ), totalCount ) );

    // Finished
    iReturn = KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: CancelAsyncRequest

    Description: Asynchronous requests are canceled by this function.

    Parameters: const RMessage aMessage

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestEngine::CancelAsyncRequest( const RMessage2& aMessage )
    {
    switch ( aMessage.Int0() )
        {
        case ETestEngineEnumerateTestCases:
            {
            TInt count = iModules.Count();
            for ( TInt i=0; i < count; i++ )
                {
                // Cancel enumerate  
                iModules[i]->Cancel();
                }
            break;
            }

        case ETestEngineErrorNotification:
            {
            if ( iErrorMessageAvailable )
                {
                iErrorMessageAvailable = EFalse;
                iErrorMessage.Complete ( KErrCancel );
                }
            }
            break;
        case ETestEngineEvent:
            // Event command cannot be cancelled
            // Only EWaitEvent can be cancelled with ECancelWait
        default:
            PanicClient( EBadRequest, aMessage );
            break;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: EventControlL

    Description: Controls event system.

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestEngine::EventControlL( const RMessage2& aMessage )

    {
    __TRACE( KVerbose, ( _L( "CTestEngine::EventControlL" ) ) );
    TInt ret = KErrNone;

    // Read EventIf from aMessage
    TEventIf event;
    TEventIfPckg eventIfPckg( event );
    aMessage.ReadL( 0, eventIfPckg );

    // EWaitEvent, EReqEvent and ERelEvent need processing here
    if( ( event.Type() == TEventIf::EWaitEvent ) ||
        ( event.Type() == TEventIf::EReqEvent ) ||
        ( event.Type() == TEventIf::ERelEvent ) ||
        ( event.Type() == TEventIf::ECancelWait ) )
        {
        TInt ind = 0; 
        TInt count = iClientEvents.Count();
        const TDesC& eventName = event.Name();
        // Search event from client event list
        for( ind=0; ind < count; ind++ )
            {
            if( iClientEvents[ind]->Name() == eventName )
                {
                break;
                }
            }

        switch( event.Type() )
            {
            // Handle event waiting  
            case TEventIf::EWaitEvent:
                {
                // Check that we found event from client events list
                if( ind == count )
                    {
                    ret = KErrNotFound;
                    }
                else
                    {
                    TEventMsg* eventEntry = iClientEvents[ind];
                    
                    __TRACE( KVerbose, ( _L( "CTestEngine::EventControlL: Waiting %S (count %i)"), 
                        &event.Name(), count ) );

                    // Waiting is completed either from here in Wait method 
                    // if event is already pending,
                    // or from Set method in CtlEvent when event is set.
                    eventEntry->Wait( aMessage );
                    return;
                    }
                }
                break;
            case TEventIf::ECancelWait:
                {
                // Check that we found event from client events list
                if( ind == count )
                    {
                    ret = KErrNotFound;
                    }
                else
                    {
                    TEventMsg* eventEntry = iClientEvents[ind];
                    
                    __TRACE( KVerbose, ( _L( "CTestEngine::EventControlL: Cancel waiting %S (count %i)"), 
                        &event.Name(), count ) );

                    eventEntry->CancelWait();
                    }
                }
                break;
            // Handle event request
            case TEventIf::EReqEvent:
                {
                // Check that event is not already requested
                if( ind < count )
                    {
                    // Already exists
                    ret = KErrArgument;
                    }
                else
                    { 
                    // Requested events are added to iClientEvents
                    TEventMsg* eventEntry = new TEventMsg;
                    if( eventEntry == NULL )
                        {
                        ret = KErrNoMemory;
                        }
                    else
                        {
                        CleanupStack::PushL( eventEntry );
                        eventEntry->Copy( event );
                        // Check if state event is set already
                        if( IsStateEventAndSet( event.Name() ) )
                            { 
                            // If it was set already, set the event
                            eventEntry->Set( TEventIf::EState );
                            }
                        ret = iClientEvents.Append( eventEntry ); 
                        if( ret != KErrNone )
                            {
                            CleanupStack::PopAndDestroy( eventEntry );
                            }
                        else
                            {   
                            CleanupStack::Pop( eventEntry );
                               
                            __TRACE( KVerbose, ( _L( "CTestEngine::EventControlL: Req added %S (count %i)"),
                                &event.Name(), iClientEvents.Count() ) );
                            } 
                        }
                    }
                }
                break;
            // Release event 
            case TEventIf::ERelEvent:
                {
                // Released events are deleted from iClientEvents
                // Check that we found entry from client events list
                if( ind == count )
                    {
                    ret = KErrNotFound;
                    }
                else
                    {
                    __TRACE( KVerbose, ( _L( "CTestEngine::EventControlL: Release event %S (count %i)"), 
                        &event.Name(), iClientEvents.Count() ) );

                    TEventMsg* eventEntry = iClientEvents[ind];
                    iClientEvents.Remove( ind );
                    eventEntry->Release();
                    delete eventEntry;
                    }
                }
                break;
            default: // This should never happen!!!
                { 
                _LIT( KEngine, "CTestEngine" );
                User::Panic( KEngine, KErrGeneral );
                }
                break;
            }
        }
    else
        {
        // ESetEvent and EUnsetEvent are only forwarded
        TRequestStatus req;
        CtlEventL( event, req );
        User::WaitForRequest( req );
        }
        
    aMessage.Complete( ret );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: HandleErrorNotificationL

    Description: Handle error notifications.

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::HandleErrorNotificationL( const RMessage2& aMessage )
    {
    iErrorMessage = aMessage;
    iErrorMessageAvailable = ETrue;

    ProcessErrorQueue();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: LoggerSettings

    Description: Get Logger's overwrite parameters

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Panics if WriteL fails

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::LoggerSettings( const RMessage2& aMessage )
    {
    __TRACE( KInit, ( _L( "CTestEngine::LoggerSettings" ) ) );

    // Copies logger settings to the package
    TPckg<TLoggerSettings> loggerSettingsPckg( iTestEngineServer->iLoggerSettings );

    // Writes a packege that includes the logger overwrite settings to aMessage
    TRAPD( err, aMessage.WriteL( 0, loggerSettingsPckg ) );
    if ( err != KErrNone )
        {
        PanicClient( EBadDescriptor, aMessage );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: CloseLoggerSettings

    Description: Close logger settings

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::CloseLoggerSettings()
    {
    __TRACE( KInit, ( _L( "CTestEngine::CloseLoggerSettings" ) ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: TestCaseByHandle

    Description: Return test case specified by aHandle

    Parameters: TUint aHandle : in : TUint : Handle to TestCase subsession

    Return Values: CTestCase* : pointer to CTestCase object

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCase* CTestEngine::TestCaseByHandle( TUint aHandle, const RMessage2& aMessage )
    {
    // Try to get test case from test case container
    CTestCase* testCase =
        ( CTestCase* )iTestCases->At( aHandle );
    if ( testCase == NULL )
        {
        PanicClient( EBadSubsessionHandle, aMessage ); 
        }

    return testCase;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: NewTestCaseL

    Description: Create new test execution subsession

    Parameters: const RMessage& aMessage: in: Server Message

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving method leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestEngine::NewTestCaseL( const RMessage2& aMessage )
    {
    __TRACE( KVerbose, ( _L( "CTestEngine::NewTestCaseL start" ) ) );

    // Get data from message
    TTestInfo testInfo;
    TTestInfoPckg testInfoPckg = testInfo;

    TRAPD( res, aMessage.ReadL( 0, testInfoPckg ) );
    if ( res != KErrNone )
        {
        PanicClient( EBadDescriptor, aMessage );
        return;
        }

    testInfo.iModuleName.LowerCase();
    // Remove optional '.DLL' from file name
    TParse parse;
    parse.Set( testInfo.iModuleName, NULL, NULL );

    if ( parse.Ext() == _L(".dll") )
        {
        const TInt len = parse.Ext().Length();
        testInfo.iModuleName.Delete ( testInfo.iModuleName.Length()-len, len );
        }

    // Get correct test module controller
    CTestModuleController* scrModule = NULL;
    CTestModuleController* module = GetFreeOrCreateModuleControllerL(testInfo, &scrModule);
    if ( module == NULL )
        {
        //@spe PanicClient( EInvalidModuleName, aMessage );
        //@spe return;
        __TRACE( KError, ( CStifLogger::ERed, _L( "Invalid module:[%S]"), &testInfo.iModuleName ) );
        LeaveWithNotifyL ( KErrNotFound );
        }

    // Create test case object
    CTestCase* testCase =
        CTestCase::NewL( this, module, iTestReport, testInfo, scrModule);
    CleanupStack::PushL( testCase );

    // Add object to object container to generate unique id
    iContainer->AddL( testCase );

    // Add object to object index
    // This returns a unique handle so we can get it again
    TInt handle = iTestCases->AddL( testCase );

    // Write the handle to client
    TPckg<TInt> handlePckg( handle );

    TRAP( res, aMessage.WriteL( 3, handlePckg ) );
    if ( res != KErrNone )
        {
        iTestCases->Remove( handle );
        PanicClient( EBadDescriptor, aMessage );
        return;
        }

    // Add new test case to testcase array
    User::LeaveIfError( iTestCaseArray.Append( testCase ) );
    CleanupStack::Pop( testCase );

    // Notch up another resource
    iResourceCount++;

    iReturn = KErrNone;

    __TRACE( KVerbose, ( _L( "CTestEngine::NewTestCaseL done" ) ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: DeleteTestCase

    Description: Delete test case from container list

    Parameters: TUint aHandle: in: Handle to test case to be removed

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::DeleteTestCase( TUint aHandle ) 
    {
    __TRACE( KVerbose, ( _L( "CTestEngine::DeleteTestCase" ) ) );

    // Get test case from container
    CTestCase* testCase = ( CTestCase* )iTestCases->At( aHandle );

    // Do nothing if invalid handle
    if ( testCase == NULL )
        {
        // Handle might be already deleted, so do nothing.
        return;
        }

    //Get controllers
    CTestModuleController* moduleController;
    CTestModuleController* realModuleController;
    
    moduleController = testCase->GetModuleControllers(&realModuleController);
    
    // Check if module controllers have crashed
    TBool moduleControllerCrashed = EFalse;
    TBool realModuleControllerCrashed = EFalse;

    if(moduleController)
        {
        moduleControllerCrashed = moduleController->iTestModuleCrashDetected;
        }
    if(realModuleController)
        {
        realModuleControllerCrashed = realModuleController->iTestModuleCrashDetected;
        }

    //__TRACE(KInit, (_L("CTestEngine::DeleteTestCase moduleController=[%x] crashed=[%d] realModuleController=[%x] crashed=[%d]"), moduleController, moduleControllerCrashed, realModuleController, realModuleControllerCrashed));
    // For UITestingSupport, SeparateProcesses and PythonSupport, when module is crashed,
    // remove it from module lists, because it will be deleted when closing test case.
    if(moduleController)
        {
        if(moduleControllerCrashed && iUITestingSupport && moduleController->iName->Find(KTestScripterName) == 0
           || moduleControllerCrashed && iSeparateProcesses
           || moduleControllerCrashed &&  moduleController->iName->Find(KPythonScripter) == 0
          )
            {
            __TRACE(KInit, (_L("Removing module controller from module list because of crash")));
            //Look for specific module controller and delete it
            TInt i;
            TInt j, children;
            TInt modules = iModules.Count();
            for(i = 0; i < modules; i++)
                {
                if(iModules[i] == moduleController)
                    {
                    __TRACE(KInit, (_L("Module controller found - removing")));
                    iModules.Remove(i);
                    break;
                    }

                //Check children of the module and if module to be deleted found there, remove it
                children = iModules[i]->iChildrenControllers.Count();
                __TRACE(KInit, (_L("Checking %d children of [%S]"), children, iModules[i]->iName));
                for(j = 0; j < children; j++)
                    {
                    if(iModules[i]->iChildrenControllers[j] == moduleController)
                        {
                        __TRACE(KInit, (_L("Module controller found (child) - removing")));
                        
                        iModules[i]->iChildrenControllers.Remove(j);

                        __TRACE(KInit, (_L("Child removed from [%S] controller. Currently it has %d children:"), iModules[i]->iName, iModules[i]->iChildrenControllers.Count()));
                        for(TInt k = 0; k < iModules[i]->iChildrenControllers.Count(); k++)
                            {
                            __TRACE(KInit, (_L("    %d. [%S]"), k + 1, iModules[i]->iChildrenControllers[k]->iName));
                            }                            
                        break;
                        }
                    }
                }
            }
        }
    if(moduleController && realModuleController)
        {
        if(realModuleControllerCrashed && iUITestingSupport && realModuleController->iName->Find(KTestScripterName) == 0
           || realModuleControllerCrashed && iSeparateProcesses
           || realModuleControllerCrashed &&  realModuleController->iName->Find(KPythonScripter) == 0
          )
            {
            __TRACE(KInit, (_L("Removing real module controller from module list because of crash")));
            //Remove module controller from scripter controller
            moduleController->RemoveModuleController(realModuleController);
            }
        }

    // Close test case and remove it from container
    //testCase->CloseTestCase();
    iTestCases->Remove( aHandle );

    // Decrement resource count
    iResourceCount--;

    // Remove from testcase array
    iTestCaseArray.Remove( iTestCaseArray.Find( testCase ) );

    //If this is UITestingSupport and module is not crashed, delete controller 
    //(in other words, kill process, because we need somehow reset the test server).
    if(moduleController && !moduleControllerCrashed)
        {        
        if(iUITestingSupport && moduleController->iName->Find(KTestScripterName) == 0)
            {
            if(realModuleController)
                {
                if(!realModuleControllerCrashed)
                    {
                    __TRACE(KInit, (_L("Delete module controller (and kill process in which test case was run).")));
                    __TRACE(KInit, (_L("Real module controller provided - processing")));
                    //Remove module controller from scripter controller
                    moduleController->DeleteModuleController(realModuleController);
                    }
                }
            else
                {
                 __TRACE(KInit, (_L("Delete module controller (and kill process in which test case was run).")));
                 __TRACE(KInit, (_L("Real module controller not provided, checking normal controllers")));
                //Look for specific module controller and delete it
                TInt i;
                TInt j, children;
                TInt modules = iModules.Count();
                for(i = 0; i < modules; i++)
                    {
                    if(iModules[i] == moduleController)
                        {
                        __TRACE(KInit, (_L("Module controller found - deleting")));
                        delete iModules[i];
                        iModules.Remove(i);
                        break;
                        }

                    //Check children of the module and if module to be deleted found there, remove it
                    children = iModules[i]->iChildrenControllers.Count();
                    __TRACE(KInit, (_L("Checking %d children of [%S]"), children, iModules[i]->iName));
                    for(j = 0; j < children; j++)
                        {
                        if(iModules[i]->iChildrenControllers[j] == moduleController)
                            {
                            __TRACE(KInit, (_L("Module controller found (child) - deleting")));
                            
                            delete iModules[i]->iChildrenControllers[j];
                            iModules[i]->iChildrenControllers.Remove(j);
    
                            __TRACE(KInit, (_L("Child removed from [%S] controller. Currently it has %d children:"), iModules[i]->iName, iModules[i]->iChildrenControllers.Count()));
                            for(TInt k = 0; k < iModules[i]->iChildrenControllers.Count(); k++)
                                {
                                __TRACE(KInit, (_L("    %d. [%S]"), k + 1, iModules[i]->iChildrenControllers[k]->iName));
                                }                            
                            break;
                            }
                        }
                    }
                }
            }
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: IsStateEventAndSet

    Description: Callback to check state event status.

    Parameters: const TName& aEventName: in: Event name

    Return Values: ETrue: event is set 
                   EFalse: event is not set

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TBool CTestEngine::IsStateEventAndSet( const TName& aEventName )
    {
    TInt count = iStateEvents.Count();
    for( TInt i = 0; i < count; i++ ) 
        {
        TPtrC name = iStateEvents[i]->Des();
        if( name == aEventName )
            {
            // Requested state event set already
            return ETrue;
            }
        }
    return EFalse;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: CtlEventL

    Description: Callback to control events.

    Parameters: const TEventIf& aEvent: in: event information
                TRequestStatus& aStatus: in: Request status

    Return Values: CTestEventController*: CTestEventController object

    Errors/Exceptions: Leaves if CtlEventL leaves
                       Leaves if memory allocation fails
                       Leaves if unset event not found from pending 
                       state event list

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEventController* CTestEngine::CtlEventL( const TEventIf& aEvent, 
                                              TRequestStatus& aStatus )
    {
    __TRACE( KVerbose, ( _L( "CTestEngine::CtlEventL" ) ) );
    
    aStatus = KRequestPending;

    UpdateEventL( aEvent );
    return CTestEventController::NewL( this, aEvent, &aStatus );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: UpdateEventL

    Description: Update event lists.

    Parameters: const TEventIf& aEvent: in: event information

    Return Values: None

    Errors/Exceptions: Leaves if CtlEventL leaves
                       Leaves if memory allocation fails
                       Leaves if unset event not found from pending
                       state event list

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::UpdateEventL( const TEventIf& aEvent )
    {
    // Handle ESetEvent and EUnsetEvent for state events here
    if( aEvent.EventType() == TEventIf::EState )
        {
        // Find event from the list
        TInt count = iStateEvents.Count();
        TInt index = 0;
        const TDesC& eventName = aEvent.Name();
        for( ; index < count; index++ ) 
            {
            TPtrC name = iStateEvents[index]->Des();
            if( name == eventName )
                {
                break;
                }
            }

        if( aEvent.Type() == TEventIf::ESetEvent )
            {
            // Check that event is not already pending
            if( index < count )
                {
                User::Leave( KErrAlreadyExists );
                }

            // Add set event to pending state event list
            HBufC* name = aEvent.Name().AllocLC();
            User::LeaveIfError( iStateEvents.Append( name ) );
            CleanupStack::Pop( name );
            }
        else if( aEvent.Type() == TEventIf::EUnsetEvent )
            {
            if( index == count )
                {
                // Not found from state event list
                User::Leave( KErrNotFound );
                }
            HBufC* tmp = iStateEvents[index];
            iStateEvents.Remove( index );
            delete tmp;
            }
        }
    else if( aEvent.Type() == TEventIf::EUnsetEvent )
        {
        // Can not give Unset for indication event
        User::Leave( KErrNotSupported );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: Logger

    Description: Return the pointer to Logger.

    Parameters: None

    Return Values: CStifLogger*: Pointer to StifLogger

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CStifLogger* CTestEngine::Logger()
    {
    return iTestEngineServer->Logger();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: WriteRebootParams

    Description: Write the Reboot's state parameters.

    Parameters: TTestInfo& aTestInfo: in: Test case information.
                TInt& aCode: in: Reboot related integer information.
                TDesC& aName: in: Reboot related string information.

    Return Values: TInt: Symbian OS error

    Errors/Exceptions: Error code returned if Logger creation fails.

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestEngine::WriteRebootParams( TTestInfo& aTestInfo,
                                        TInt& aCode,
                                        TDesC& aName )
    {
    __TRACE( KVerbose, ( _L( "WriteRebootParams() starts..." ) ) );
    
    // Logger's setting definitions
    TLoggerSettings loggerSettings;

    loggerSettings.iCreateLogDirectories = ETrue;
    loggerSettings.iOverwrite = ETrue;
    loggerSettings.iTimeStamp = EFalse;
    loggerSettings.iLineBreak = EFalse;
    loggerSettings.iEventRanking = EFalse;
    loggerSettings.iThreadId = EFalse;
    // EData format because now we don't have to check file type when
    // parsing this.
    loggerSettings.iHardwareFormat = CStifLogger::EData;
    loggerSettings.iHardwareOutput = CStifLogger::EFile;
    loggerSettings.iEmulatorFormat = CStifLogger::EData;
    loggerSettings.iEmulatorOutput = CStifLogger::EFile;
    loggerSettings.iUnicode = EFalse;
    loggerSettings.iAddTestCaseTitle = EFalse;

    // Use default setting if path or filename are not set by SetAttribute()
    if( iRebootPath == NULL )
        {
        iRebootPath= iRebootDefaultPath.Alloc();
        }
    if( iRebootFilename == NULL )
        {
        iRebootFilename= iRebootDefaultFilename.Alloc();
        }

    CStifLogger* logger = NULL;
    TRAPD( ret, logger = CStifLogger::NewL( *iRebootPath,
                                        *iRebootFilename,
                                        loggerSettings ) );
    if( ret != KErrNone )
        {
        __TRACE( KError, ( _L( "WriteRebootParams() Logger creation fails with error: " ), ret ) );
        delete logger;
        return ret;
        }

    // Write reboot parameters
    // Because logging format is EData we need add line breaks by hand.
    logger->Log( _L( "\r\n" ) );
    logger->Log( _L( "This is TestFramework's file which includes Reboot related informations" ) );
    logger->Log( _L( "\r\n" ) );
    logger->Log( _L( "\r\n" ) );
    logger->Log( _L( "Reboot case's related information:" ) );
    logger->Log( _L( "\r\n" ) );
    logger->Log( _L( "%S %S" ), &KTestModule, &aTestInfo.iModuleName );
    logger->Log( _L( "\r\n" ) );
    logger->Log( _L( "%S %S" ), &KTestCaseFile, &aTestInfo.iConfig );
    logger->Log( _L( "\r\n" ) );
    logger->Log( _L( "%S %d" ), &KTestCaseNumber, aTestInfo.iTestCaseInfo.iCaseNumber );
    logger->Log( _L( "\r\n" ) );
    logger->Log( _L( "%S %S" ), &KTestCaseTitle, &aTestInfo.iTestCaseInfo.iTitle );
    logger->Log( _L( "\r\n" ) );
    logger->Log( _L( "\r\n" ) );
    logger->Log( _L( "%S %d" ), &KStateCode, aCode );
    logger->Log( _L( "\r\n" ) );
    logger->Log( _L( "%S " ), &KStateName );
    logger->Log( aName );
    logger->Log( _L( "\r\n" ) );
    logger->Log( _L( "\r\n" ) );

    delete logger;

    __TRACE( KVerbose, ( _L( "WriteRebootParams() ends" ) ) );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: ReadRebootParams

    Description: Read the Reboot parameters.

    Parameters: TTestInfo& aTestInfo: in: Test case information.

    Return Values: TInt: Symbian OS error

    Errors/Exceptions: Error code returned if Parser creation fails.
                       Error code returned if section creation fails.
                       Error code returned if item creation fails.
                       Error code returned if parsing operation fails.
                       KErrArgument returned if parsed values and test case
                       values are different.

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestEngine::ReadRebootParams( TTestInfo& aTestInfo, 
                                    TDes& aStateDes, 
                                    TInt& aState )
    {
    __TRACE( KVerbose, ( _L( "ReadRebootParams() starts..." ) ) );

    if( iRebootParams == NULL )
        {
        __TRACE( KVerbose, ( _L( "ReadRebootParams(): Reboot not done" )) );
        return KErrNotFound;
        }
        
    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    // Checks parsed values and test case values
    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    if( iRebootParams->iTestModule != aTestInfo.iModuleName ||
        iRebootParams->iTestCaseFile != aTestInfo.iConfig ||
        iRebootParams->iCaseNumber != aTestInfo.iTestCaseInfo.iCaseNumber )
        {
        __TRACE( KVerbose, 
            ( _L( "ReadRebootParams(): Reboot not done by %S" ), 
                &aTestInfo.iModuleName) );
        return KErrArgument;
        }
   
    aStateDes.Copy( iRebootParams->iStateName );
    aState = iRebootParams->iStateCode;
   
    // delete reboot params, to ensure that same test case 
    // does not get indication about reboot again
    delete iRebootParams;
    iRebootParams = 0;

    __TRACE( KVerbose, ( _L("ReadRebootParams() ends" ) ) );
    
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: ParseRebootParamsL

    Description: Parse the Reboot parameters.

    Parameters: None

    Return Values: TInt: Symbian OS error

    Errors/Exceptions: Error code returned if Parser creation fails.
                       Error code returned if section creation fails.
                       Error code returned if item creation fails.
                       Error code returned if parsing operation fails.
                       KErrArgument returned if parsed values and test case
                       values are different.

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestEngine::ParseRebootParamsL()
    {
    __TRACE( KVerbose, ( _L( "ParseRebootParamsL() starts..." ) ) );

    TInt ret_parsing( 0 );
    TPtrC tmp;

    // Use default setting if path or filename are not set by SetAttribute()
    if( iRebootPath == NULL )
        {
        iRebootPath= iRebootDefaultPath.Alloc();
        }
    if( iRebootFilename == NULL )
        {
        iRebootFilename= iRebootDefaultFilename.Alloc();
        }

    // Create parser object
    CStifParser* parser = NULL;
    TRAPD( ret, parser = CStifParser::NewL( *iRebootPath,
                                        *iRebootFilename ) );
    if( ret != KErrNone )
        {
        __TRACE( KError, ( _L( "ReadRebootParams(): Reboot file not found" )) );
        return ret;        
        }
    CleanupStack::PushL( parser );
    
    // Create section object
    CStifSectionParser* section = parser->SectionL( KNullDesC, KNullDesC );
    CleanupStack::PushL( section );
    
    // Delete old reboot params
    delete iRebootParams;
    iRebootParams = 0;
    iRebootParams = CRebootParams::NewL();
    
    // ----------------------Test module parsing
    CStifItemParser* item = section->GetItemLineL( KTestModule );
    CleanupStack::PushL( item );

    // Parsing integer
    ret_parsing = item->GetString( KTestModule, tmp );
    if( ret_parsing == KErrNone || ret_parsing == KErrNotFound )
        {
        __TRACE( KVerbose, ( _L("ReadRebootParams(): TestModule: %S" ), &tmp ) );
        }
    else
        {
        __TRACE( KError, 
            ( _L("ReadRebootParams(): TestModule parsing fails with error: " ), 
                ret_parsing ) );
        User::Leave( ret_parsing );        
        }
    iRebootParams->SetTestModuleNameL( tmp );
    CleanupStack::PopAndDestroy( item );

    // ----------------------Testcase parsing
    item = section->GetItemLineL( KTestCaseFile );
    CleanupStack::PushL( item );

    // Parsing string
    ret_parsing = item->GetString( KTestCaseFile, tmp );
    if( ret_parsing == KErrNone ) 
        {
        __TRACE( KVerbose, ( _L("ReadRebootParams(): TestCaseFile: %S" ), &tmp ) );
        iRebootParams->SetTestCaseFileNameL( tmp );
        }
    else if( ret_parsing == KErrNotFound )
        {
        __TRACE( KVerbose, 
            ( _L("ReadRebootParams(): No testcasefile defined for test module" )) );
        }
    else
        {
        __TRACE( KError, 
            ( _L("ReadRebootParams(): TestCaseFile parsing fails with error: %d " ), 
                ret_parsing ) );
        User::Leave( ret_parsing );  
        }
    CleanupStack::PopAndDestroy( item );

    // ----------------------Testcase number parsing
    item = section->GetItemLineL( KTestCaseNumber );
    CleanupStack::PushL( item );    
    
    // Parsing integer
    ret_parsing = item->GetInt( KTestCaseNumber, iRebootParams->iCaseNumber );
    if( ret_parsing == KErrNone || ret_parsing == KErrNotFound )
        {
        __TRACE( KVerbose, ( _L("ReadRebootParams(): TestCaseNumber: %d" ), 
            iRebootParams->iCaseNumber ) );
        }
    else
        {
        __TRACE( KError,
            ( _L("ReadRebootParams(): TestCaseNumber parsing fails with error: %d " ), 
                ret_parsing ) );
         User::Leave( ret_parsing );  
        }
    CleanupStack::PopAndDestroy( item );

    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--
    // Next state code and name parsing if no error found
    // --.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--.--

    // ----------------------State code parsing
    item = section->GetItemLineL( KStateCode );
    CleanupStack::PushL( item );   

    // Parsing integer
    ret_parsing = item->GetInt( KStateCode, iRebootParams->iStateCode );
    if( ret_parsing == KErrNone || ret_parsing == KErrNotFound )
        {
        __TRACE( KVerbose, ( _L("ReadRebootParams(): StateCode: %d" ), 
            iRebootParams->iStateCode ) );
        }
    else
        {
        __TRACE( KError, 
            ( _L("ReadRebootParams(): StateCode parsing fails with error: %d " ), 
            ret_parsing ) );
        User::Leave( ret_parsing );  
        }
    CleanupStack::PopAndDestroy( item );
    
    // ----------------------State name parsing
    ret_parsing = section->GetLine( KStateName, tmp, ENoTag );
    if( ret_parsing != KErrNone )
        {
        __TRACE( KError, 
            ( _L("ReadRebootParams(): State name parsing fails with error: %d " ), 
            ret_parsing ) );
        User::Leave( ret_parsing );         
        }

    iRebootParams->SetTestCaseStateL( tmp );

    __TRACE( KVerbose, ( _L("ReadRebootParams() ends" ) ) );

    CleanupStack::PopAndDestroy( section );
    CleanupStack::PopAndDestroy( parser );

    // Delete file
    RFs rf;
    TInt retVal = rf.Connect();
	if( retVal != KErrNone )
		{
		User::Leave( retVal ); 
		}
		
    TFileName file( *iRebootPath );
    file.Append( *iRebootFilename );
    rf.Delete( file );
    rf.Close();
    
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: PauseAllTestCases

    Description: Pause all test case(s) which are/is running.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestEngine::PauseAllTestCases()
    {
    __TRACE( KVerbose, ( _L( "CTestEngine::PauseAllTestCases()" ) ) );

    TInt count( 0 );
    count = iTestCaseArray.Count();

    for( TInt a = 0; a < count; a++ )
        {
         // Pause test case
         iTestCaseArray[a]->Pause();
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: FlushAtsLogger

    Description: Flush ATS logger's

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestEngine::FlushAtsLogger()
    {
    __TRACE( KVerbose, ( _L( "CTestEngine::FlushAtsLogger()" ) ) );

    TInt count( 0 );
    count = iModules.Count();

    for( TInt a = 0; a < count; a++ )
        {
         // Pause test case
         iModules[a]->AtsLogger().SaveForRebootL();
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: GetDeviceResetDllName

    Description: Get device reset module's DLL name.

    Parameters: None

    Return Values: TPtrC

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TPtrC CTestEngine::GetDeviceResetDllName()
    {
    __TRACE( KVerbose, ( _L( "CTestEngine::GetDeviceResetDllName()" ) ) );

    return iDeviceResetDllName->Des();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: EnableStifMeasurement

    Description: Set measurements related information, enable measurement.

    Parameters: const TName& aInfoType: in: Enabled measurement type

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestEngine::EnableStifMeasurement( const TDesC& aInfoType )
    {
    __TRACE( KInit, ( _L( "Measurement [%S] enabled" ), &aInfoType ) );

    if( aInfoType == KStifMeasurement01 )
        {
        iDisableMeasurement &= ~EMeasurement01;
        }
    else if( aInfoType == KStifMeasurement02 )
        {
        iDisableMeasurement &= ~EMeasurement02;
        }
    else if( aInfoType == KStifMeasurement03 )
        {
        iDisableMeasurement &= ~EMeasurement03;
        }
    else if( aInfoType == KStifMeasurement04 )
        {
        iDisableMeasurement &= ~EMeasurement04;
        }
    else if( aInfoType == KStifMeasurement05 )
        {
        iDisableMeasurement &= ~EMeasurement05;
        }
    else if( aInfoType == KStifMeasurementBappea )
        {
        iDisableMeasurement &= ~EBappea;
        }
    else if( aInfoType == KStifMeasurementDisableAll )
        {
        __TRACE( KInit, ( _L( "All measurements are disabled" ) ) );
        iDisableMeasurement &= EDisableAll; // Does change anything !!!
        }
    else if( aInfoType ==  KStifMeasurementEnableAll )
        {
        __TRACE( KInit, ( _L( "All measurements types are enabled" ) ) );
        iDisableMeasurement &= EEnableAll;
        }
    else
        {
        __TRACE( KInit, ( _L( "Measurement [%S] not recognized" ), &aInfoType ) );
        return KErrArgument;
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: DisableStifMeasurement

    Description: Set measurements related information, disable measurement.

    Parameters: const TName& aInfoType: in: Disabled measurement type 

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestEngine::DisableStifMeasurement( const TDesC& aInfoType )
    {
    __TRACE( KInit, ( _L( "Measurement [%S] disabled" ), &aInfoType ) );

    if( aInfoType == KStifMeasurement01 )
        {
        iDisableMeasurement |=  EMeasurement01;
        }
    else if( aInfoType ==  KStifMeasurement02 )
        {
        iDisableMeasurement |=  EMeasurement02;
        }
    else if( aInfoType ==  KStifMeasurement03 )
        {
        iDisableMeasurement |= EMeasurement03;
        }
    else if( aInfoType ==  KStifMeasurement04 )
        {
        iDisableMeasurement |=  EMeasurement04;
        }
    else if( aInfoType ==  KStifMeasurement05 )
        {
        iDisableMeasurement |=  EMeasurement05;
        }
    else if( aInfoType ==  KStifMeasurementBappea )
        {
        iDisableMeasurement |=  EBappea;
        }
    else if( aInfoType ==  KStifMeasurementDisableAll )
        {
        __TRACE( KInit, ( _L( "All measurements are disabled" ) ) );
        iDisableMeasurement |=  EDisableAll;
        }
    else if( aInfoType ==  KStifMeasurementEnableAll )
        {
        __TRACE( KInit, ( _L( "All measurements types are enabled" ) ) );
        iDisableMeasurement |=  EEnableAll; // Does change anything !!!
        }
    // This option can be defined in TestFramework.ini file
    else if( aInfoType ==  KStifMeasurementDisableNone )
        {
        __TRACE( KInit, ( _L( "All measurements types are enabled" ) ) );
        iDisableMeasurement =  EEnableAll; // Sets to 0
        }
    else
        {
        __TRACE( KInit, ( _L( "Measurement [%S] not recognized" ), &aInfoType ) );
        return KErrArgument;
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: StifMeasurement

    Description: Get measurements related information. Returns is measurement
                 disable.

    Parameters: None

    Return Values: TInt: Indication what measurement types is/are disabled.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestEngine::StifMeasurement()
    {
    return iDisableMeasurement;

    }
/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: TestModuleCrash

    Description: This method is called only when test module crashed 
                 with KErrServerTerminated (-15). 
                 Clones new TestModuleController:
                 - Find crashed testmodulecontroller
                 - Creates new copy of that testmodulecontroller
                 - Replaces old crashed testmodulecontroller with this new one

    Parameters: CTestModuleController* aTestModuleController: in: TestModuleController
    Return Values: None

    Errors/Exceptions: Leaves if error happens when adding clone to the list

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::TestModuleCrash(CTestModuleController* aTestModuleController)
    {
    TName crashModuleName = aTestModuleController->ModuleName(KNullDesC);
    __TRACE(KInit, (_L("Handling crashed test module [%S]"), &crashModuleName));
    TBool isTestScripter(EFalse);
    
    // Find crashed test module controller
    CTestScripterController* testScripterController = aTestModuleController->iTestScripterController;
    if(testScripterController)
        {
        isTestScripter = ETrue;
        }

    // If UITestingSupport or SeparateProcesses is enabled, there is no need
    // to create clone of Test Module Controller, because it will be created
    // automatically when needed.
    if(iUITestingSupport && crashModuleName.Find(KTestScripterName) == 0 
       || iSeparateProcesses
       || (crashModuleName.Find(KPythonScripter) == 0)
      )
        {
        __TRACE(KInit, (_L("Handling crashed test module with enabled UITestingSupport or SeparateProcesses is not needed")));
        aTestModuleController->iTestModuleCrashDetected = ETrue;
        return;
        }
        
    // Clone crashed module
    TBool afterReset = EFalse;
    if(iRebootParams && iRebootParams->iTestModule == crashModuleName)
        {
        afterReset = ETrue;
        }  
    
    CTestModuleController* clone;    
    clone = aTestModuleController->CloneL(aTestModuleController, afterReset, testScripterController);  
    
    // Replaces crashed testmodulecontroller with this new one    
    // Note: Old Testmodulecontroller is deleted in CTestModuleController::CaseFinished 
    //       that is called from  CTestCase::~CTestCase() 

    TInt index = KErrNotFound;
    if(isTestScripter)
        {
        index = testScripterController->iTestScripter.Find(aTestModuleController);
        __TRACE(KInit, (_L("Crashed module index [%d]"), index));
        if(index != KErrNotFound)
            {
            testScripterController->iTestScripter.Remove(index);
            }
        testScripterController->iTestScripter.Append(clone);            
        }
    else
        {
        index = iModules.Find(aTestModuleController);
        __TRACE(KInit, (_L("Crashed module index [%d]"), index));
        if(index != KErrNotFound)
            {
            iModules.Remove(index);
            }
        iModules.Append(clone);
        }
    __TRACE(KInit, (_L("End of handling crashed test module")));
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: ExecuteCommandL

    Description: Executes command received from test case.
                 The method was created to allow test case to kill itself.

    Parameters: aTestCaseHandle: handler to test case
    Return Values: None

    Errors/Exceptions: Leaves if error happens when adding clone to the list

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngine::ExecuteCommandL(TCommand aCommand, TDesC8& aParamsPckg)
    {
    TInt testCaseHandle = 0;
    // Get params
    switch(aCommand)
        {
        case EStopExecution:
            {
            //Unpack received parameters
            TStopExecutionCommandParams par;
            TStopExecutionCommandParamsPckg parPack(par);
            parPack.Copy(aParamsPckg);

            __TRACE(KInit, (_L("CTestEngine::ExecuteCommandL received command [%d] type [%d] code [%d] test handle [%d]"), TInt(aCommand), TInt(par.iType), TInt(par.iCode), par.iTestCaseHandle));

            //Get test case handle
            testCaseHandle = par.iTestCaseHandle;

            break;
            }
        case ESendTestModuleVersion:
        	{
        	TSendTestModuleVesionCommandParams par;
        	TSendTestModuleVesionCommandParamsPckg parPack(par);
        	parPack.Copy(aParamsPckg);

        	TTestModuleVersionInfo testModuleVersionInfo;
        	testModuleVersionInfo.iMajor = par.iMajor;
        	testModuleVersionInfo.iMinor = par.iMinor;
        	testModuleVersionInfo.iBuild = par.iBuild;
        	testModuleVersionInfo.iTestModuleName = par.iTestModuleName;
        	if(iTestReport)
        	    {
        	    iTestReport->AddTestModuleVersion(testModuleVersionInfo);
        	    }
        	
        	return;
        	}
        default:
            __TRACE(KError, (_L("CTestEngine::ExecuteCommandL Unknown command [%d]"), TInt(aCommand)));
            return;
        }

    // Get test case from container
    CTestCase* testCase = (CTestCase*)iTestCases->At(testCaseHandle);

    // Log some info if invalid handle
    if(testCase == NULL)
        {
        __TRACE(KInit, ( _L( "CTestEngine::ExecuteCommandL - invalid handle. Unable to get test case to execute command [%d]"), TInt(aCommand)));
        return;
        }

    testCase->ExecuteCommandL(aCommand, aParamsPckg);

    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: GetIndexForNewTestModuleController

    Description: Returns new index for test module controller.
                 This number is appended to module controller name.
                 This method is used when option to run every test case in 
                 separate process is set to on.

    Parameters: aTestCaseHandle: handler to test case
    Return Values: None

    Errors/Exceptions: Leaves if error happens when adding clone to the list

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestEngine::GetIndexForNewTestModuleController(void)
    {
    return iIndexTestModuleControllers++;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngine

    Method: AddTestCaseToTestReport

    Description: Get parameters from message and add test case to test report.

    Parameters: aMessage: message
    Return Values: error id

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestEngine::AddTestCaseToTestReport(const RMessage2& aMessage)
    {
    if(iTestReport)
        {
        // Read test data
        TTestInfo testInfo;
        TTestInfoPckg testInfoPckg(testInfo);
    
        TFullTestResult fullTestResult;
        TFullTestResultPckg fullTestResultPckg(fullTestResult);
        
        TRAPD(err, aMessage.ReadL(0, testInfoPckg));
        if(err)
            {
            __TRACE(KError, (_L("Leave when reading TTestInfo in AddTestCaseToTestReport [%d]"), err));
            return err;
            }
    
        TRAP(err, aMessage.ReadL(1, fullTestResultPckg));
        if(err)
            {
            __TRACE(KError, (_L("Leave when reading TFullTestResult in AddTestCaseToTestReport [%d]"), err));
            return err;
            }
            
        TRAP(err, iTestReport->AddTestCaseResultL(testInfo, fullTestResult, aMessage.Int2()));
        if(err)
            {
            __TRACE(KError, (_L("Leave from test report in AddTestCaseToTestReport [%d]"), err));
            return err;
            }
        }
    else
        {
        __TRACE(KError, (_L("TestReport not initialized in AddTestCaseToTestReport")));
        return KErrNotReady;
        }

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestEngineSubSession class member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestEngineSubSession

    Method: CTestEngineSubSession

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters: CTestEngine* aEngine: in: Pointer to Test Engine

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestEngineSubSession::CTestEngineSubSession( CTestEngine* aEngine ) :
    iTestEngine( aEngine )
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineSubSession

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestEngineSubSession::ConstructL()
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineSubSession

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestEngine* aEngine: in: Pointer to Test Engine

    Return Values: CTestEngineSubSession* : pointer to created CTestEngineSubSession object

    Errors/Exceptions: Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEngineSubSession* CTestEngineSubSession::NewL( CTestEngine* aEngine )    
    {
    CTestEngineSubSession* self = new ( ELeave ) CTestEngineSubSession( aEngine );
    CleanupClosePushL( *self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestEngineSubSession

    Method: ~CTestEngineSubSession

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestEngineSubSession::~CTestEngineSubSession()
    {
    }


/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CTestCase class member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: CTestCase

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters: CTestEngine* aEngine: in: Pointer to Test Engine
                CTestModuleController* aModuleController: in: Pointer to 
                  Module Controller
                TTestInfo& aTestInfo: in: Test info for this test case
                CTestModuleController* aRealModuleController: in: Pointer to
                  module controller used inside in scripter controller

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CTestCase::CTestCase( CTestEngine* aEngine,
                     CTestModuleController* aModuleController,
                     TTestInfo& aTestInfo,
                     CTestModuleController* aRealModuleController ) :
    iTestEngine( aEngine ),
    iTestModule( aModuleController ),
    iRealModuleController (aRealModuleController)
    {
    
    // This method must be called to find out how
    // many testcases are currently ongoing by increasing iTestCaseCounter. 
    // This is a part of the implementation for 
    //  supporting test module crashing with -15
    iTestModule->CaseCreated();  

    // Store handle to RTestServer
    //If test scripter is used (it uses internally module controllers)
    //then use it (real module controller). 
    if(iRealModuleController)
        {
        iTestServer = iRealModuleController->Server(aTestInfo); 
        }
    else
        {
        iTestServer = iTestModule->Server( aTestInfo );
        }

    //If real test module controller is provided, increase test case count
    if(iRealModuleController)
        {
        iRealModuleController->CaseCreated();
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: CTestReport* aTestReport: in: Pointer to Test Report
                TTestInfo& aTestInfo: in: Test Info for this test case

    Return Values: None

    Errors/Exceptions: Leaves if RTestExecution::Open returns error
                       Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCase::ConstructL( CTestReport* aTestReport,
                           TTestInfo& aTestInfo )
    {
    __TRACE( KVerbose, ( _L( "CTestCase::ConstructL" ) ) );

    // Open handle to RTestExecution
    User::LeaveIfError( iTestExecution.Open( iTestServer,
        aTestInfo.iTestCaseInfo.iCaseNumber, aTestInfo.iConfig ) );

    // Make new test case runner
    iTestCaseController = CTestCaseController::NewL( iTestEngine,
        aTestReport, iTestModule->AtsLogger(), iTestExecution, aTestInfo );

    // Make new test case printer
    iTestCasePrint = CTestProgressNotifier::NewL( iTestEngine,
                                                  iTestExecution );

    iTestCaseEvent = CTestEventNotifier::NewL( iTestEngine, iTestExecution );

    iTestCaseRemoteCmd = CTestRemoteCmdNotifier::NewL( iTestEngine,
                                                    iTestExecution,
                                                    iTestCaseController,
                                                    iTestModule->AtsLogger() );

    iTestCaseCommand = CTestCommandNotifier::NewL(iTestEngine, iTestExecution);
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestEngine* aEngine: in: Pointer to Test Engine
                CTestModuleController* aModuleController: in: Pointer to
                 Module Controller
                CTestReport* aTestReport: in: Pointer to Test Report
                TTestInfo& aTestInfo: in: Test Info for this test case

    Return Values: CTestCase* : pointer to created CTestCase object

    Errors/Exceptions: Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCase* CTestCase::NewL( CTestEngine* aEngine,
            CTestModuleController* aModuleController,
            CTestReport* aTestReport,
            TTestInfo& aTestInfo,
            CTestModuleController* aRealModuleController )    
    {
    CTestCase* self = new ( ELeave ) CTestCase( aEngine, aModuleController, aTestInfo, aRealModuleController );
    CleanupClosePushL( *self );
    self->ConstructL( aTestReport, aTestInfo );
    CleanupStack::Pop();
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: ~CTestCase

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestCase::~CTestCase()
    {
    CloseTestCase();
    
    // This added method call is a part of the implementation for 
    // supporting test module crashing with -15.
    // It checks that can old Testmodulecontroller be deletd or not 
	if( iTestModule != NULL )
		{
		iTestModule->CaseFinished();    
		}
	
    //If real test module controller is provided, decrease test case count
    if( iRealModuleController != NULL )
        {
        iRealModuleController->CaseFinished();
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: CloseTestCase

    Description: Close session

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCase::CloseTestCase()
    {
    __TRACE( KVerbose, ( _L( "CTestCase::CloseTestCase" ) ) );

    // Free allocated resources
    delete iTestCaseController;
    iTestCaseController = NULL;
    delete iTestCasePrint;
    iTestCasePrint = NULL;
    delete iTestCaseRemoteCmd;
    iTestCaseRemoteCmd = NULL;

    delete iTestCaseEvent;
    iTestCaseEvent = NULL;

    delete iTestCaseCommand;
    iTestCaseCommand = NULL;

    iTestExecution.Close();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: RunTestCaseL

    Description: Enumerates test cases

    Parameters: const RMessage& aMessage: in: Server Message

    Return Values: None

    Errors/Exceptions: Leaves if some of called leaving methods leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCase::RunTestCaseL( const RMessage2& aMessage )
    {
    __TRACE( KVerbose, ( _L( "CTestCase::RunTestCaseL" ) ) );
    // Start active objects for running test case
    iTestCaseController->StartL( aMessage );
    iTestCaseEvent->Start();
    iTestCaseCommand->Start();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: Pause

    Description: Suspend the test case execution

    Parameters: None

    Return Values: TInt: Return value from RTestExecution::Pause

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestCase::Pause()
    {
    __TRACE( KVerbose, ( _L( "CTestCase::Pause" ) ) );

    return iTestExecution.Pause();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: Resume

    Description: Resume the suspended test case execution

    Parameters: None

    Return Values: TInt: Return value from RTestExecution::Resume

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestCase::Resume()
    {
    __TRACE( KVerbose, ( _L( "CTestCase::Resume" ) ) );

    return iTestExecution.Resume();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: NotifyProgressL

    Description: Notifies progresses from Test Module

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Leaves if called StartL method leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCase::NotifyProgressL( const RMessage2& aMessage )
    {
    __TRACE( KVerbose, ( _L( "CTestCase::NotifyProgressL" ) ) );
    iTestCasePrint->StartL( aMessage );

    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: NotifyRemoteTypeL

    Description: Notifies remote commands from Test Module

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Leaves if called StartL method leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCase::NotifyRemoteTypeL( const RMessage2& aMessage )
    {
    
    __TRACE( KVerbose, ( _L( "CTestCase::NotifyRemoteTypeL" ) ) );
    iTestCaseRemoteCmd->EnableReceive( aMessage );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: NotifyRemoteMsgL

    Description: Notifies remote commands from Test Module

    Parameters: const RMessage& aMessage: in: Server message

    Return Values: None

    Errors/Exceptions: Leaves if called StartL method leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCase::NotifyRemoteMsgL( const RMessage2& aMessage )
    {
    
    __TRACE( KVerbose, ( _L( "CTestCase::NotifyRemoteMsgL" ) ) );
    switch( aMessage.Int1() )
        {
        case EStifCmdSend:
            {
            iTestCaseRemoteCmd->GetReceivedMsg( aMessage );
            }
            break;
        case EStifCmdReceive:
            {
            TInt len = aMessage.Int2();
            if( len <= 0 )
                {
                User::Leave( KErrGeneral );
                }
            HBufC8* buf = HBufC8::NewLC( len );

            TPtr8 tmp = buf->Des();
            aMessage.ReadL( 0, tmp );

            TInt ret = iTestExecution.ReadRemoteCmdInfo( tmp, EStifCmdReceive );

            CleanupStack::PopAndDestroy( buf );
            
            aMessage.Complete( ret );
            }
            break;
        case EStifCmdRebootProceed:
            {
            TInt value = 0;
            TPckg<TInt> tmp( value );
            aMessage.ReadL( 0, tmp );

            TInt ret = iTestExecution.ReadRemoteCmdInfo( tmp, 
                                                         EStifCmdRebootProceed,
                                                         value );        
            aMessage.Complete( ret );
            }
            break;
        default:
            {
            User::Leave( KErrGeneral );
            }  
        }            
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: CancelAsyncRequest

    Description: Asynchronous requests are canceled by this function.

    Parameters: const RMessage aMessage

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCase::CancelAsyncRequest( const RMessage2& aMessage )
    {
    switch ( aMessage.Int0() )
        {
        case ETestCaseRunTestCase:
            {
            iTestCaseController->Cancel();
            iTestCaseEvent->Cancel();
            break;
            }
        case ETestCaseNotifyProgress:
            {
            iTestCasePrint->Cancel();
            break;
            }
        case ETestCaseNotifyRemoteType:
            {
            iTestCaseRemoteCmd->CancelReq();
            break;
            }
        case ETestCaseNotifyCommand:
            {
            iTestCaseCommand->Cancel();
            break;
            }
        default:
            iTestEngine->PanicClient( EBadRequest, aMessage );
            break;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: ModuleName

    Description: Return the name of Test Module.

    Parameters: None

    Return Values: const TFileName& : Test Module owning this test case

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
const TDesC& CTestCase::ModuleName()
    {
    return iTestModule->ModuleName( KNullDesC );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: CtlEvent

    Description: Control events

    Parameters: const TEventIf& aEvent: in: Event
                TRequestStatus& aStatus: in: Request status

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCase::CtlEvent( const TEventIf& aEvent, TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( iTestCaseEvent, User::Panic( _L( "TestEngine event panic" ), KErrArgument ) );
        
    iTestCaseEvent->CtlEvent( aEvent, aStatus );
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: CheckCtlEvent

    Description: Check if CtlEvent should be called

    Parameters: const TEventIf& aEvent: in: Event

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TBool CTestCase::CheckCtlEvent( const TEventIf& aEvent )
    {
    return iTestCaseEvent->CheckCtlEvent( aEvent );
    }


/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: Logger

    Description: Return the pointer to Logger.

    Parameters: None

    Return Values: CStifLogger*: Pointer to StifLogger

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CStifLogger* CTestCase::Logger()
    {
    return iTestEngine->Logger();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: ExecuteCommaandL

    Description: Executes command received from test case.

    Parameters: aStifCommand command to be executed
                aParam1      parameter to command

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCase::ExecuteCommandL(TCommand aCommand, TDesC8& aParamsPckg)
    {
    switch(aCommand)
        {
        case EStopExecution:
            {
            //Unpack received parameters
            TStopExecutionCommandParams par;
            TStopExecutionCommandParamsPckg parPack(par);
            parPack.Copy(aParamsPckg);

            __TRACE(KVerbose, (_L("CTestCase::ExecuteCommandL command [%d] type [%d] code [%d]"), TInt(aCommand), TInt(par.iType), par.iCode));

            iTestCaseController->Suicide(par.iType, par.iCode);
            break;
            }
        default:
            __TRACE(KVerbose, (_L("CTestCase::ExecuteCommandL unknown command [%d]"), TInt(aCommand)));
            return;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: GetModuleControllers

    Description: Return module controller and real module controller

    Parameters: aRealModuleController: out: real module controller

    Return Values: module controller

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestModuleController* CTestCase::GetModuleControllers(CTestModuleController** aRealModuleController)
    {
    *aRealModuleController = iRealModuleController;
    return iTestModule;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: ResetModuleController

    Description: Set new module controller for test case (only in case when 
                 original controller crashed)

    Parameters: aModuleController: in: new module controller

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCase::ResetModuleController(CTestModuleController* aModuleController)
    {
    iTestModule = aModuleController;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestCase

    Method: ResetRealModuleController

    Description: Set new real module controller for test case (only in case 
                 when original controller crashed).

    Parameters: aRealModuleController: in: new real module controller

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestCase::ResetRealModuleController(CTestModuleController* aRealModuleController)
    {
    iRealModuleController = aRealModuleController;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    Default constructor

-------------------------------------------------------------------------------
*/
CTestReportSettings::CTestReportSettings()
    {
    iCreateTestReport = ETrue;
    iPath = NULL;
    iName = NULL;
    iFormat = CStifLogger::ETxt;
    iOutput = CStifLogger::EFile;
    iOverwrite = ETrue; 
    iXML = EFalse;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    Two-phased constructor.

-------------------------------------------------------------------------------
*/
CTestReportSettings* CTestReportSettings::NewL()
    {
    CTestReportSettings* self = new ( ELeave ) CTestReportSettings();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    Symbian OS second phase constructor

-------------------------------------------------------------------------------
*/
void CTestReportSettings::ConstructL()
    {
    
    }
/*
-------------------------------------------------------------------------------

    DESCRIPTION

    Destructor

-------------------------------------------------------------------------------
*/
CTestReportSettings::~CTestReportSettings()
    { 
    delete iPath; 
    iPath = NULL;
    delete iName;
    iName = NULL;
    } 

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of CRebootParams class member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CRebootParams

    Method: CRebootParams

    Description: Default constructor
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CRebootParams::CRebootParams()
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CRebootParams

    Method: ConstructL

    Description: Symbian OS second phase constructor


    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if RTestExecution::Open returns error
                       Leaves if some of called leaving methods leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
void CRebootParams::ConstructL()
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CRebootParams

    Method: NewL

    Description: Two-phased constructor.

    Parameters: none
    
    Return Values: CRebootParams* : pointer to created CRebootParams object

    Errors/Exceptions: Leaves if ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
CRebootParams* CRebootParams::NewL()    
    {
    
    CRebootParams* self = new ( ELeave ) CRebootParams();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CRebootParams

    Method: ~CRebootParams

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CRebootParams::~CRebootParams()
    {
    
    delete iTestModuleBuf;
    iTestModuleBuf = 0;
    delete iTestCaseFileBuf;
    iTestCaseFileBuf = 0;
    delete iTestCaseTitleBuf;
    iTestCaseTitleBuf = 0;
    delete iStateNameBuf;
    iStateNameBuf = 0;
    
    }


/*
-------------------------------------------------------------------------------

    Class: CRebootParams

    Method: SetTestModuleNameL

    Description: Setter

    Parameters: const TDesC& aName: in: name to set

    Return Values: None

    Errors/Exceptions: Leaves if memory allocation fails
    
    Status: Draft

-------------------------------------------------------------------------------
*/        
void CRebootParams::SetTestModuleNameL( const TDesC& aName )
    {
    
    delete iTestModuleBuf;
    iTestModuleBuf = 0;
    iTestModuleBuf = aName.AllocLC();
    iTestModule.Set( iTestModuleBuf->Des() );
    CleanupStack::Pop( iTestModuleBuf );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CRebootParams

    Method: SetTestModuleNameL

    Description: Setter

    Parameters: const TDesC& aName: in: name to set

    Return Values: None

    Errors/Exceptions:  Leaves if memory allocation fails

    Status: Draft

-------------------------------------------------------------------------------
*/        
void CRebootParams::SetTestCaseFileNameL( const TDesC& aName )
    {
    
    delete iTestCaseFileBuf;
    iTestCaseFileBuf = 0;
    iTestCaseFileBuf = aName.AllocLC();
    iTestCaseFile.Set( iTestCaseFileBuf->Des() );
    CleanupStack::Pop( iTestCaseFileBuf );
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CRebootParams

    Method: SetTestModuleNameL

    Description: Setter

    Parameters: const TDesC& aName: in: name to set

    Return Values: None

    Errors/Exceptions:  Leaves if memory allocation fails

    Status: Draft

-------------------------------------------------------------------------------
*/        
void CRebootParams::SetTestCaseTitleL( const TDesC& aName )
    {
    
    delete iTestCaseTitleBuf;
    iTestCaseTitleBuf = 0;
    iTestCaseTitleBuf = aName.AllocLC();
    iTestCaseTitle.Set( iTestCaseTitleBuf->Des() );
    CleanupStack::Pop( iTestCaseTitleBuf );
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CRebootParams

    Method: SetTestModuleNameL

    Description: Setter

    Parameters: const TDesC& aName: in: name to set

    Return Values: None

    Errors/Exceptions: Leaves if memory allocation fails

    Status: Draft

-------------------------------------------------------------------------------
*/        
void CRebootParams::SetTestCaseStateL( const TDesC& aName )
    {
    
    delete iStateNameBuf;
    iStateNameBuf = 0;
    iStateNameBuf = aName.AllocLC();
    iStateName.Set( iStateNameBuf->Des() );
    CleanupStack::Pop( iStateNameBuf );
    
    }

    
/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains implementation of TEventMsg class member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: TEventMsg

    Method: TEventMsg

    Description: Default constructor
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/    
TEventMsg::TEventMsg():
    TEventIf(),
    iWaitPending(EFalse), 
    iStateEventPending(EFalse),
    iStatus( NULL )
    {
    }

/*
-------------------------------------------------------------------------------

    Class: TEventMsg

    Method: ~TEventMsg

    Description: Destructor
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/    
TEventMsg::~TEventMsg()
    {
    if( iWaitPending )
        {
        iWaitMsg.Complete( KErrCancel );
        iWaitPending = EFalse;
        }
        
    if( iStatus )
        {
        User::RequestComplete( iStatus, KErrCancel );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: TEventMsg

    Method: Set

    Description: Set event.
    
    Parameters: TEventType aEventType: in: Event type
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/    
void TEventMsg::Set( TEventType aEventType )
    {
    iEventType = aEventType;
    if( aEventType == EState )
        {
        iStateEventPending = ETrue;
        }
    if( iWaitPending )
        {
        TEventIf event;
        event.Copy( *this );
        TEventIfPckg eventIfPckg( event );
        iWaitMsg.WriteL( 0, eventIfPckg );

        iWaitMsg.Complete( KErrNone );
        iWaitPending = EFalse;
        }
    } 

/*
-------------------------------------------------------------------------------

    Class: TEventMsg

    Method: Wait

    Description: Wait event.
    
    Parameters: const RMessage& aMessage: in: Message
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void TEventMsg::Wait( const RMessage2& aMessage )
    { 
    if( iStateEventPending )
        {
        TEventIf event;
        event.Copy( *this );
        TEventIfPckg eventIfPckg( event );
        aMessage.WriteL( 0, eventIfPckg );
        aMessage.Complete( KErrNone );
        }
    else
        {
        iWaitMsg = aMessage;
        iWaitPending = ETrue;
        }
    }

/*
-------------------------------------------------------------------------------

    Class: TEventMsg

    Method: CancelWait

    Description: Cancel pending Wait
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/    
void TEventMsg::CancelWait()
    {
    if( iWaitPending )
        {
        iWaitMsg.Complete( KErrCancel );
        iWaitPending = EFalse;
        } 
    } 
             
/*
-------------------------------------------------------------------------------

    Class: TEventMsg

    Method: Release

    Description: Release event. Unset released.
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/    
void TEventMsg::Release()
    { 
    
    if( iStatus )
        {
        User::RequestComplete( iStatus, KErrNone );
        }
        
    } 
/*
-------------------------------------------------------------------------------

    Class: TEventMsg

    Method: Unset

    Description: Unset event. Blocks until Release is called.
    
    Parameters: TRequestStatus& aStatus: in: Status 
    
    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/    
void TEventMsg::Unset( TRequestStatus& aStatus )
    { 
    
    iStatus = &aStatus;
    
    } 

// ================= OTHER EXPORTED FUNCTIONS =================================

/*
-------------------------------------------------------------------------------

    Function: StartEngine

    Description: This is called from the client.

    Parameters: None

    Return Values: TInt KErrNone: No errors occured
                        KErrNoMemory: Memory is too low to create Test Engine
                        Other error code: Error got from iEngineThread.Create()

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt StartEngine()
    {
    __UHEAP_MARK;

    // check server not already started
    TFindServer findTestEngineServer( KTestEngineName );
    TFullName name;
    if ( findTestEngineServer.Next( name ) == KErrNone )
        {   
        // Server already started, nothing to do    
        __UHEAP_MARKEND;
        return KErrNone;
        }

    // Construct start-up information object
    TThreadStartTestEngine* startInfo = new TThreadStartTestEngine();
    if ( startInfo == NULL )
        {
        __UHEAP_MARKEND;
        return KErrNoMemory;
        }

    startInfo->iStarted.CreateLocal( 0 );     // Create start-up semaphore

    // Create thread    
    TInt ret = startInfo->iEngineThread.Create(
        KTestEngineName ,                               // name of thread
        CTestEngineServer::ThreadFunction,              // thread function
        KDefaultStackSize*4,                            // stack
        KTestEngineMinHeapSize,KTestEngineMaxHeapSize*4,// Heap
        startInfo                                       // parameter to thread
                                                        // function
        );

    if ( ret != KErrNone )
        {
        startInfo->iStarted.Close();
        delete startInfo;
        __UHEAP_MARKEND;
        return ret;
        }

    // Now start thread
    startInfo->iEngineThread.SetPriority( EPriorityMuchMore );  // set its
                                                                // priority
    startInfo->iEngineThread.Resume();                          // kick it
                                                                // into life

    // Wait until the thread is started
    startInfo->iStarted.Wait(); 

    // Clean-up 
    startInfo->iEngineThread.Close();
    startInfo->iStarted.Close(); 


    delete startInfo;

    __UHEAP_MARKEND;

    return KErrNone;

    }

// End of File
