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
* Description:  This file contains the implementations of the
*               CHtiFramework class.
*
*/


// INCLUDE FILES
#include "HtiFramework.h"
#include "HtiLogging.h"
#include <badesca.h>
#include <e32std.h>
#include <f32file.h>
#include <HtiCfg.h>
#include <HtiStartupWaitInterface.h>

// CONSTANTS
_LIT( KHtiFrameworkMatchPattern, "HtiFramework*" );
_LIT( KHtiMainThreadName,        "HtiMain" );
_LIT( KHtiWatchDogExeName,       "HtiWatchDog.exe" );
_LIT( KHtiWatchDogMatchPattern,  "HtiWatchDog*" );
_LIT( KHtiAdminStartParameter,   "admin" );
_LIT( KEComServerMatchPattern,   "EComServer*" );
_LIT( KHtiStartupWaitDllName,    "HtiStartupWait.dll" );

// config file parameters
_LIT( KCfgFilePath,         "\\");
_LIT( KHtiCfgFileName,      "hti.cfg" );
_LIT8( KCommPlugin,         "CommPlugin" );
_LIT8( KMaxMsgSize,         "MaxMsgSize" );
_LIT8( KMaxQueueSize,       "MaxQueueSize" );
_LIT8( KMaxHeapSize,        "MaxHeapSize" );
_LIT8( KPriority,           "Priority" );
_LIT8( KShowConsole,        "ShowConsole" );
_LIT8( KMaxWaitTime,        "MaxWaitTime" );
_LIT8( KStartUpDelay,       "StartUpDelay" );
_LIT8( KEnableHtiWatchDog,  "EnableHtiWatchDog" );
_LIT8( KEnableHtiAutoStart, "EnableHtiAutoStart" );
_LIT8( KShowErrorDialogs,   "ShowErrorDialogs" );
_LIT8( KReconnectDelay,     "ReconnectDelay");

const static TInt KDefaultMaxWaitTime  = 90; // seconds
const static TInt KDefaultStartUpDelay = 5;  // seconds
const static TInt KHtiDefaultPriority = 3;
const static TInt KHtiWatchDogEnabledDefault = 0;
const static TInt KHtiConsoleEnabledDefault = 0;
const static TInt KHtiAutoStartEnabledDefault = 0;
const static TInt KHtiShowErrorDialogsDefault = 1;
const static TInt KHtiReconnectDelay = 0;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiFramework::CHtiFramework
// Constructor
// -----------------------------------------------------------------------------
CHtiFramework::CHtiFramework():
    iCfg( NULL ),
    iDispatcher( NULL ),
    iOriginalHeap( NULL )
    {
    }


// -----------------------------------------------------------------------------
// CHtiFramework::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
void CHtiFramework::ConstructL()
    {
    HTI_LOG_FUNC_IN("CHtiFramework::ConstructL");

    if ( IsHtiRunning() )
        {
        HTI_LOG_TEXT( "HTI already running - not starting second instance" );
        User::Leave( KErrAlreadyExists );
        }

    TInt priority = KHtiDefaultPriority;
    TInt enableHtiWatchDog = KHtiWatchDogEnabledDefault;
    TInt showConsole = KHtiConsoleEnabledDefault;
    TInt enableHtiAutoStart = KHtiAutoStartEnabledDefault;
    TInt showErrorDialogs = KHtiShowErrorDialogsDefault;
    TInt reconnectDelay = KHtiReconnectDelay;

    TRAPD( err, iCfg = CHtiCfg::NewL() );
    if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "CHtiCfg constructed" );
        TRAP( err, iCfg->LoadCfgL( KCfgFilePath, KHtiCfgFileName ) );
        if ( err == KErrNone )
            {
            HTI_LOG_TEXT( "Cfg file loaded" );

            TBuf8<64> commPlugin( KNullDesC8 );
            TRAPD( paramErr, commPlugin = iCfg->GetParameterL( KCommPlugin ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KCommPlugin );
                }

            TInt maxMsgSize = 0;
            TRAP( paramErr, maxMsgSize = iCfg->GetParameterIntL( KMaxMsgSize ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KMaxMsgSize );
                }

            TInt maxQueueSize = 0;
            TRAP( paramErr, maxQueueSize = iCfg->GetParameterIntL( KMaxQueueSize ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KMaxQueueSize );
                }

            TInt maxHeapSize = 0;
            TRAP( paramErr, maxHeapSize = iCfg->GetParameterIntL( KMaxHeapSize ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KMaxHeapSize );
                }
            else
                {
                //create new heap and switch heaps
                RHeap* newHeap = UserHeap::ChunkHeap( NULL, //local
                                                      KMinHeapSize, //min size
                                                      maxHeapSize //max size
                                                     );
                if ( newHeap )
                    {
                    HTI_LOG_TEXT( "SwitchHeap" );
                    iOriginalHeap = User::SwitchHeap( newHeap );
                    HTI_LOG_ALLOC_HEAP_MEM();
                    }
                else
                    {
                    HTI_LOG_TEXT( "Failed create new heap" );
                    User::Leave( KErrNoMemory );
                    }
                }

            TRAP( paramErr, priority = iCfg->GetParameterIntL( KPriority ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KPriority );
                }

            TRAP( paramErr, showConsole = iCfg->GetParameterIntL( KShowConsole ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KShowConsole );
                }

            TInt maxWaitTime = KDefaultMaxWaitTime;
            TRAP( paramErr, maxWaitTime = iCfg->GetParameterIntL( KMaxWaitTime ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KMaxWaitTime );
                }

            TInt startUpDelay = KDefaultStartUpDelay;
            TRAP( paramErr, startUpDelay = iCfg->GetParameterIntL( KStartUpDelay ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KStartUpDelay );
                }

            TRAP( paramErr, enableHtiWatchDog = iCfg->GetParameterIntL( KEnableHtiWatchDog ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KEnableHtiWatchDog );
                }

            TRAP( paramErr, enableHtiAutoStart = iCfg->GetParameterIntL( KEnableHtiAutoStart ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KEnableHtiAutoStart );
                }

            TRAP( paramErr, showErrorDialogs = iCfg->GetParameterIntL( KShowErrorDialogs ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KShowErrorDialogs );
                }
            
            TRAP( paramErr, reconnectDelay = iCfg->GetParameterIntL( KReconnectDelay ) );
            if ( paramErr != KErrNone )
                {
                HTI_LOG_TEXT( "The following parameter not defined in cfg, using default value:" );
                HTI_LOG_DES( KReconnectDelay );
                }
            
            if ( !IsStartAcceptedL( enableHtiAutoStart ) )
                {
                User::Leave( KErrAbort );
                }

            WaitNormalState( maxWaitTime, startUpDelay );
            iDispatcher = CHtiDispatcher::NewL( commPlugin, maxMsgSize,
                    maxQueueSize, reconnectDelay, showConsole != 0, showErrorDialogs != 0 );
            }
        }


    // cfg file was not found or it could not be opened
    if ( err )
        {
        HTI_LOG_TEXT( "Error loading cfg file, use default values" );

        if ( !IsStartAcceptedL( enableHtiAutoStart ) )
            {
            User::Leave( KErrAbort );
            }

        WaitNormalState( KDefaultMaxWaitTime, KDefaultStartUpDelay );

        //create with default values
        iDispatcher = CHtiDispatcher::NewL(
            KNullDesC8, 0, 0, 0, showConsole != 0, showErrorDialogs != 0 );
        }

    HTI_LOG_FORMAT( "Priority setting = %d", priority );
    // Set HTI priority
    switch ( priority )
        {
        case 1:
            RProcess().SetPriority( EPriorityBackground ); // 250
            break;
        case 2:
            RProcess().SetPriority( EPriorityForeground ); // 350
            break;
        // case 3: goes to default
        case 4:
            RThread().SetPriority( EPriorityAbsoluteHigh ); // 500
            break;
        default:
            RProcess().SetPriority( EPriorityHigh );        // 450
            break;
        }

    // start hti watchdog, if it is enabled in config file
    if ( enableHtiWatchDog != 0 ) StartHtiWatchDogL();

    HTI_LOG_FORMAT( "Process priority %d", RProcess().Priority() );
    HTI_LOG_FORMAT( "Thread priority %d", RThread().Priority() );
    HTI_LOG_FUNC_OUT("CHtiFramework::ConstructL");
    }


// -----------------------------------------------------------------------------
// CHtiFramework::~CHtiFramework
// Destructor.
// -----------------------------------------------------------------------------
CHtiFramework::~CHtiFramework()
    {
    HTI_LOG_FUNC_IN("~CHTIServer");
    delete iDispatcher;

    HTI_LOG_ALLOC_HEAP_MEM();
    //switch before deleting iCfg
    //'cause iCfg was allocated in the original heap
    if ( iOriginalHeap )
        {
        User::SwitchHeap( iOriginalHeap );
        }

    delete iCfg;
    HTI_LOG_FUNC_OUT("~CHTIServer");
    }


// -----------------------------------------------------------------------------
// CHtiFramework::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CHtiFramework* CHtiFramework::NewL()
    {
    CHtiFramework* obj = new ( ELeave ) CHtiFramework;
    CleanupStack::PushL( obj );
    obj->ConstructL();
    CleanupStack::Pop();
    return obj;
    }


// -----------------------------------------------------------------------------
// CHtiFramework::StartL
// The method that gets the show going.
// -----------------------------------------------------------------------------
TInt CHtiFramework::StartL()
    {
    HTI_LOG_FUNC_IN( "CHtiFramework::StartL" );

    CConsoleBase* console = iDispatcher->GetConsole();
    if ( console )
        {
        console->Printf( _L( "HTI up and running.\n" ) );
        }

    // start scheduler
    CActiveScheduler::Start();

    HTI_LOG_FUNC_OUT( "CHtiFramework::StartL" );
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// CHtiFramework::StartHtiWatchDogL
// Launches the HTI Watchdog process if necessary.
// -----------------------------------------------------------------------------
void CHtiFramework::StartHtiWatchDogL()
    {
    HTI_LOG_FUNC_IN( "CHtiFramework::StartHtiWatchDogL" );

    TFullName processName;
    TFindProcess finder( KHtiWatchDogMatchPattern );
    TInt err = finder.Next( processName );
    if ( err == KErrNone )
        {
        // ok, already running
        HTI_LOG_TEXT( "HtiWatchDog already running" );
        }
    else
        {
        // start watchdog
        HTI_LOG_TEXT( "Starting HtiWatchDog..." );
        RProcess watchDogProcess;
        err = watchDogProcess.Create( KHtiWatchDogExeName, KNullDesC );
        if ( err == KErrNone )
            {
            watchDogProcess.Resume();
            watchDogProcess.Close();
            HTI_LOG_TEXT( "HtiWatchDog up and running" );
            }
        else
            {
            HTI_LOG_FORMAT( "Could not start HtiWatchDog, err: %d", err );
            User::Leave( err );
            }
        }
    HTI_LOG_FUNC_OUT( "CHtiFramework::StartHtiWatchDogL" );
    }


// -----------------------------------------------------------------------------
// CHtiFramework::WaitNormalState
// Delays HTI startup until device reaches normal state.
// -----------------------------------------------------------------------------
void CHtiFramework::WaitNormalState( TInt aMaxWaitTime, TInt aStartUpDelay )
    {
    HTI_LOG_FUNC_IN("CHtiFramework::WaitNormalState");
    // First make sure that EComServer is running before continuing
    TFullName processName;
    TFindProcess finder( KEComServerMatchPattern );
    while ( finder.Next( processName ) != KErrNone )
        {
        HTI_LOG_TEXT( "HTI waiting for EComServer startup" );
        finder.Find( KEComServerMatchPattern );
        User::After( 1000000 ); // wait 1 second
        }
    HTI_LOG_TEXT( "EComServer process found - HTI startup continuing" );

    if ( aMaxWaitTime > 0 )
        {
        TInt err = KErrNone;
        RFs fs;
        err = fs.Connect();
        if ( err == KErrNone )
            {
            RLibrary library;
            err = library.Load( KHtiStartupWaitDllName );
            HTI_LOG_FORMAT( "StartupWait library load returned %d", err );
            if ( err == KErrNone &&
                 library.Type()[1] == KHtiStartupWaitInterfaceUid )
                {
                HTI_LOG_TEXT( "StartupWait DLL found" );
                TLibraryFunction entry = library.Lookup( 1 );
                if ( entry != NULL )
                    {
                    MHtiStartupWaitInterface* startupWait =
                        ( MHtiStartupWaitInterface* ) entry();
                    err = startupWait->WaitForStartup( aMaxWaitTime );
                    HTI_LOG_FORMAT( "StartupWait returned %d", err );
                    delete startupWait;
                    startupWait = NULL;
                    }
                }
            library.Close();
            }
        }

    HTI_LOG_FORMAT( "HTI Starting after %d seconds", aStartUpDelay );
    User::After( aStartUpDelay * 1000 * 1000 );

    HTI_LOG_FUNC_OUT( "CHtiFramework::WaitNormalState" );
    }


// -----------------------------------------------------------------------------
// CHtiFramework::IsHtiRunning
// Checks whether HTI Framework process is already running.
// -----------------------------------------------------------------------------
TBool CHtiFramework::IsHtiRunning()
    {
    HTI_LOG_FUNC_IN( "CHtiFramework::IsHtiRunning" );
    TInt htiInstanceCount = 0;
    TBool isRunning = EFalse;
    TFullName processName;
    TFindProcess finder( KHtiFrameworkMatchPattern );
    TInt err = finder.Next( processName );
    while ( err == KErrNone && processName.Length() > 0 )
        {
        HTI_LOG_FORMAT( "Found process %S", &processName );
        RProcess process;
        err = process.Open( finder );
        if ( err == KErrNone )
            {
            if ( process.ExitType() == EExitPending )
                {
                HTI_LOG_TEXT( "Process is running" );
                htiInstanceCount++;
                }
            process.Close();
            }
        err = finder.Next( processName );
        }
    if ( htiInstanceCount > 1 )
        {
        isRunning = ETrue;
        }
    HTI_LOG_FUNC_OUT( "CHtiFramework::IsHtiRunning" );
    return isRunning;
    }


// -----------------------------------------------------------------------------
// CHtiFramework::IsStartAccepted
// Checks whether HTI Framework should start or not. If automatic startup is
// disabled HTI should start only when started by HtiAdmin.
// -----------------------------------------------------------------------------
TBool CHtiFramework::IsStartAcceptedL( TBool aIsAutoStartEnabled )
    {
    HTI_LOG_FUNC_IN( "CHtiFramework::IsStartAccepted" );
    TBool isStartAccepted = EFalse;

    if ( aIsAutoStartEnabled )
        {
        // If auto start is enabled, no additional checks needed. OK to start.
        isStartAccepted = ETrue;
        }

    else
        {
        // If we have "admin" parameter given from command line, start is
        // requested by HtiAdmin or HtiWatchDog and we are OK to start -
        // otherwise this is an auto start and we deny it.
        TInt cmdLen = User::CommandLineLength();
        HBufC* cmdLine = HBufC::NewL( cmdLen );
        TPtr ptrCmdLine = cmdLine->Des();
        User::CommandLine( ptrCmdLine );
        TLex parser( *cmdLine );
        parser.SkipCharacters();
        if ( parser.MarkedToken().Compare( KHtiAdminStartParameter ) == 0  )
            {
            isStartAccepted = ETrue;
            }
        delete cmdLine;
        cmdLine = NULL;
        }

    HTI_LOG_FORMAT( "HTI start accepted = %d", isStartAccepted );
    HTI_LOG_FUNC_OUT( "CHtiFramework::IsStartAccepted" );
    return isStartAccepted;
    }


// ============================ LOCAL FUNCTIONS ===============================

LOCAL_C TInt StartL()
    {
    User::__DbgMarkStart( RHeap::EUser );

    User::RenameThread( KHtiMainThreadName );

    TInt error;
    CHtiFramework* server = NULL;
    TRAP( error, server = CHtiFramework::NewL() );

    if ( error == KErrNone )
        {
        CleanupStack::PushL( server );
        TRAP( error, server->StartL() );

        if ( error )
            {
            HTI_LOG_FORMAT( "Leave happened in StartL(): %d", error );
            }

        CleanupStack::PopAndDestroy();
        }
    else
        {
        HTI_LOG_FORMAT( "Leave when constructing: %d", error );
        }

    User::__DbgMarkEnd( RHeap::EUser, 0 );
    HTI_LOG_TEXT( "NO memory leaks: past __UHEAP_MARKEND" );
    return error;
    }


GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    HTI_LOG_ALLOC_HEAP_MEM();

    _LIT( KHTIServer, "CHtiFramework" );
    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler *scheduler = new ( ELeave ) CActiveScheduler;
    CActiveScheduler::Install( scheduler );

    TRAPD( error, StartL() );

    delete scheduler;
    delete cleanup;
    __UHEAP_MARKEND;
    HTI_LOG_ALLOC_HEAP_MEM();

    __ASSERT_ALWAYS( !error, User::Panic( KHTIServer, error ) );
    return KErrNone;
    }
