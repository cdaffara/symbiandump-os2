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
* Description:  HtiAutoStart implementation
*
*/


// INCLUDE FILES
#include "HtiAutoStart.h"
#include <apmrec.h>
#include <ecom/implementationproxy.h>

// CONSTANTS
const static TUint KHtiAutoStartImplUid = 0x10210CC5;
const static TUid KHtiAutoStartUid = {0x10210CC4};
_LIT( KHtiFrameworkExe, "HtiFramework.exe" );
_LIT( KHtiMatchPattern, "HtiFramework*" );

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KHtiAutoStartImplUid,
                                CHtiAutostart::CreateRecognizerL )
    };

// MACROS
#ifdef __ENABLE_LOGGING__

#include <flogger.h>
_LIT( KLogFolder, "hti" );
_LIT( KLogFile,   "htiautostart.txt" );

#define HTI_LOG_TEXT(a1) {_LIT(temp, a1); RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, temp);}
#define HTI_LOG_DES(a1) {RFileLogger::Write(KLogFolder, KLogFile, EFileLoggingModeAppend, a1);}
#define HTI_LOG_FORMAT(a1,a2) {_LIT(temp, a1); RFileLogger::WriteFormat(KLogFolder, KLogFile, EFileLoggingModeAppend, temp, (a2));}

#else   // !__ENABLE_LOGGING__

#define HTI_LOG_TEXT(a1)
#define HTI_LOG_DES(a1)
#define HTI_LOG_FORMAT(a1,a2)

#endif // __ENABLE_LOGGING__

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    HTI_LOG_TEXT( "ImplementationGroupProxy" );
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// ----------------------------------------------------------------------------
CApaDataRecognizerType* CHtiAutostart::CreateRecognizerL()
    {
    HTI_LOG_TEXT( "CreateRecognizerL" );
    CApaDataRecognizerType* recognizer = new CHtiAutostart();
    CHtiAutostart::StartThread();
    return recognizer;
    }

// ----------------------------------------------------------------------------
CHtiAutostart::CHtiAutostart()
    :CApaDataRecognizerType( KHtiAutoStartUid, CApaDataRecognizerType::ENormal )
    {
    HTI_LOG_TEXT( "CHtiAutostart constructor" );
    iCountDataTypes = 1;
    }

// ----------------------------------------------------------------------------
TUint CHtiAutostart::PreferredBufSize()
    {
    HTI_LOG_TEXT( "PreferredBufSize" );
    return 0;
    }

// ----------------------------------------------------------------------------
TDataType CHtiAutostart::SupportedDataTypeL( TInt /*aIndex*/ ) const
    {
    HTI_LOG_TEXT( "SupportedDataTypeL" );
    return TDataType();
    }

// ----------------------------------------------------------------------------
void CHtiAutostart::DoRecognizeL( const TDesC& /*aName*/,
                                  const TDesC8& /*aBuffer*/ )
    {
    HTI_LOG_TEXT( "DoRecognizeL" );
    }

// ----------------------------------------------------------------------------
void CHtiAutostart::StartThread()
    {
    HTI_LOG_TEXT( "StartThread" );
    //create a new thread for starting our application
    RThread* startAppThread = new RThread();

    User::LeaveIfError( startAppThread->Create(
        _L( "HtiAutostart" ),
        CHtiAutostart::StartAppThreadFunction,
        KDefaultStackSize,
        KMinHeapSize,
        KMinHeapSize,
        NULL,
        EOwnerThread ) );

    startAppThread->SetPriority( EPriorityNormal );
    startAppThread->Resume();
    startAppThread->Close();
    delete startAppThread;
    }

// ----------------------------------------------------------------------------
TInt CHtiAutostart::StartAppThreadFunction( TAny* /*aParam*/ )
    {
    HTI_LOG_TEXT( "StartAppThreadFunction" );
    // create a TRAP cleanup
    CTrapCleanup * cleanup = CTrapCleanup::New();
    TInt err;
    if ( cleanup == NULL )
        {
        err = KErrNoMemory;
        }
    else
        {
        TRAP( err, StartAppThreadFunctionL() );
        }
    delete cleanup;

    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT( "StartAppThreadFunctionL leave %d", err );
        User::Panic( _L( "HtiAutostart" ), err );
        }

    return err;
    }

// ----------------------------------------------------------------------------
void CHtiAutostart::StartAppThreadFunctionL()
    {
    HTI_LOG_TEXT( "StartAppThreadFunctionL" );
    // Check if HtiFramework is already running
    TFullName processName;
    TFindProcess finder( KHtiMatchPattern );
    TInt err = finder.Next( processName );
    if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "HtiFramework.exe already running, nothing to do" );
        return;
        }

    RProcess prs;
    err = prs.Create( KHtiFrameworkExe, KNullDesC );
    if ( err == KErrNone )
        {
        prs.Resume();
        prs.Close();
        HTI_LOG_TEXT( "HtiFramework.exe process created" );
        }
    else
        {
        HTI_LOG_FORMAT( "HtiFramework.exe process creation failed %d", err );
        User::Leave( err );
        }
    }

// End of File
