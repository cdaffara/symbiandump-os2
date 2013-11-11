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
* Description:  HtiWatchDog implementation.
*
*/


// INCLUDE FILES
#include <e32base.h>

#ifdef __ENABLE_LOGGING__

#include <flogger.h>
_LIT( KLogFolder, "hti" );
_LIT( KLogFile,   "htirestart.txt" );

#define HTI_LOG_TEXT(a1) {_LIT(temp, a1); RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, temp);}
#define HTI_LOG_DES(a1) {RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, a1);}
#define HTI_LOG_FORMAT(a1,a2) {_LIT(temp, a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, temp, (a2));}

#else   // !__ENABLE_LOGGING__

#define HTI_LOG_TEXT(a1)
#define HTI_LOG_DES(a1)
#define HTI_LOG_FORMAT(a1,a2)

#endif // __ENABLE_LOGGING__

// CONSTANTS
_LIT( KHtiFrameworkExeName,    "HtiFramework.exe" );
_LIT( KHtiMainThreadName,      "HtiMain" );
_LIT( KHtiRestartName,        "HtiRestart" );
_LIT( KHtiAdminStartParameter, "admin" );


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ LOCAL FUNCTIONS ===============================

LOCAL_C void StartHtiProcessL()
    {
    RProcess htiProcess;
    User::LeaveIfError( htiProcess.Create(
            KHtiFrameworkExeName, KHtiAdminStartParameter ) );
    htiProcess.Resume();
    htiProcess.Close();
    }


LOCAL_C TInt StartL()
    {
    HTI_LOG_TEXT( "HtiRestart starting..." );
    TFullName threadName;
    TFullName matchPattern;
    matchPattern.Append(_L( "*" ));
    matchPattern.Append(KHtiMainThreadName);
    matchPattern.Append(_L( "*" ));

    // Use thread finder to find the HTI main thread
    TFindThread threadFinder;
    threadFinder.Find(matchPattern);
    HTI_LOG_TEXT( "Trying to find HTI main thread" );
    TInt err = threadFinder.Next(threadName);

    if (err == KErrNone)
        {
        HTI_LOG_TEXT( "HTI main thread found, opening it" );
        RThread thread;
        err = thread.Open(threadName);
        if (err)
            {
            HTI_LOG_FORMAT( "Could not open HTI main thread, err: %d", err );
            User::Panic(_L( "HTI open err" ), err);
            }

        // Logon to HTI main thread and wait for its death
        HTI_LOG_TEXT( "HTI main thread opened, waiting for its death" );
        TRequestStatus status;
        thread.Logon(status);
        User::WaitForRequest(status);
        thread.Close();

        HTI_LOG_TEXT( "HTI died");
        }

    TBuf<0x20> cmd;
    User::CommandLine(cmd);

    TLex lex(cmd);
    TInt microseconds = 0;
    lex.Val(microseconds);
    HTI_LOG_FORMAT("After %d milliseconds...", microseconds);
    User::After(microseconds);

    // try to restart HTI
    HTI_LOG_TEXT( "Trying to restart it" );
    TRAP( err, StartHtiProcessL() );
    if (err)
        {
        HTI_LOG_FORMAT( "Could not restart HTI, err: %d", err );
        User::Panic(_L( "HTI start err" ), err);
        }

    HTI_LOG_TEXT( "HtiRestart shutting down" );
    return KErrNone;
    }

GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;

    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler *scheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install( scheduler );

    User::RenameThread( KHtiRestartName );

    TRAPD( err, StartL() );

    delete scheduler;
    delete cleanup;

   __UHEAP_MARKEND;

    return err;
    }


// End of File
