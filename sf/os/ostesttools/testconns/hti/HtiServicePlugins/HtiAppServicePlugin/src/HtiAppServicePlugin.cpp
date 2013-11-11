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
* Description:  HtiAppServicePlugin implementation
*
*/


// INCLUDE FILES
#include <badesca.h>
#include <f32file.h>

#include "HtiAppServicePlugin.h"
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

// CONSTANTS
const static TUid KAppServiceUid = { 0x1020DEC7 };

//error descriptions
_LIT8( KErrDescrNoMemory, "No memory" );
_LIT8( KErrDescrInvalidCmd, "Invalid command" );
_LIT8( KErrDescrInvalidArguments, "Invalid arguments" );
_LIT8( KErrDescrFailedCreateProcess, "Failed create process" );
_LIT8( KErrDescrFailedOpenProcess, "Failed open process" );
_LIT8( KErrDescrInvalidProcessId, "Invalid process id" );
_LIT8( KErrDescrNotSupported, "Command not supported" );
_LIT( KHtiAppControlDll, "HtiAppControl.dll" );

const static TUint8 KUnicodeMask = 0x01;
const static TInt KTerminateReason = 0;
const static TInt KTUintLength = sizeof(TUint);

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// Create instance of concrete ECOM interface implementation
CHtiAppServicePlugin* CHtiAppServicePlugin::NewL()
    {
    CHtiAppServicePlugin* self = new ( ELeave ) CHtiAppServicePlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Constructor
CHtiAppServicePlugin::CHtiAppServicePlugin()
    {
    }

CHtiAppServicePlugin::~CHtiAppServicePlugin()
    {
    HTI_LOG_FUNC_IN( "~CHtiAppServicePlugin" );

    for ( TInt i = 0; i < iProcessHandleArray.Count(); i++ )
        {
        iProcessHandleArray[i].Close();
        }
    iProcessHandleArray.Close();

    delete iAppServiceUiPlugin;

    iLibrary.Close();

    HTI_LOG_FUNC_OUT( "~CHtiAppServicePlugin" );
    }

 //Second phase construction.
void CHtiAppServicePlugin::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiAppServicePlugin::ConstructL" );

    TInt err = iLibrary.Load( KHtiAppControlDll );
    HTI_LOG_FORMAT( "HtiAppControlDll library load returned %d", err );

    if ( err == KErrNone &&
            iLibrary.Type()[1] == KHTIServiceInterfaceUid )
        {
        HTI_LOG_TEXT( "HtiAppControlDll DLL found" );
        TLibraryFunction entry = iLibrary.Lookup( 1 );
        if ( entry != NULL )
            {
            iAppServiceUiPlugin = ( CHTIServicePluginInterface* ) entry();
            }
        }
    else if ( err == KErrNotFound )
        {
        //No HtiAppControl.dll found so CHtiAppServicePlugin can handle only
        //process control commands
        HTI_LOG_TEXT( "HtiAppControlDll DLL not found" );
        }

    HTI_LOG_FUNC_OUT( "CHtiAppServicePlugin::ConstructL" );
    }

void CHtiAppServicePlugin::InitL()
    {
    if ( iAppServiceUiPlugin )
        {
        iAppServiceUiPlugin->SetDispatcher( iDispatcher );
        }
    }

TInt CHtiAppServicePlugin::ParseString( const TDesC8& aRequest,
                                        TInt anOffset,
                                        TBool aUnicode,
                                        TDes& aResult )
    {
    HTI_LOG_FUNC_IN( "CHtiAppServicePlugin::ParseString" );
    //validate parameters
    //if offset outside the string return empty string
    if ( anOffset >= aRequest.Size() )
        {
        return anOffset;
        }

    TInt len = aRequest[anOffset];
    HTI_LOG_FORMAT( "length %d", len );

    if ( len > aResult.MaxLength() )
        {
        return KErrBadDescriptor;
        }

    TInt nextOffset = ( aUnicode ? len * 2 : len ) + anOffset + 1;
    HTI_LOG_FORMAT( "nextOffset %d", nextOffset );
    HTI_LOG_FORMAT( "reqSize %d", aRequest.Size() );
    if ( nextOffset > aRequest.Size() )
        {
        return KErrArgument;
        }

    if ( aUnicode )
        {
        const TPtrC8 aFrom( aRequest.Mid( anOffset + 1, len * 2 ) );
        aResult.SetLength( len );
        for ( TInt i = 0; i < len; ++i )
            {
            aResult[i] = ( TUint16 ) aFrom[i << 1] +
                ( ( ( TUint16 ) aFrom[( i << 1 ) + 1] ) << 8 );
            }
        }
    else
        {
        aResult.Copy( aRequest.Mid( anOffset + 1, len ) );
        }

    HTI_LOG_FUNC_OUT( "CHtiAppServicePlugin::ParseString" );
    return nextOffset;
    }

void CHtiAppServicePlugin::ProcessMessageL( const TDesC8& aMessage,
                THtiMessagePriority aPriority )
    {
    HTI_LOG_FUNC_IN( "CHtiAppServicePlugin::ProcessMessage" );
    if ( aMessage.Length() < 1 )
        {
        // no command
        SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
        return;
        }

    if ( aMessage.Length() < 2 &&
         aMessage[0] != EListProcesses &&
         aMessage[0] != EListProcesses_u )
        {
         // parameter is required with all command except listing processes
        SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
        return;
        }

    HTI_LOG_FORMAT( "cmd %d", aMessage[0] );
    if ( aMessage[0] < EProcessLastCommand )
        {
        HandleProcessControlL( aMessage );
        }
    else if ( aMessage[0] <= EUnInstallName )
        {
        if ( iAppServiceUiPlugin )
            {
            iAppServiceUiPlugin->ProcessMessageL( aMessage, aPriority );
            }
        else
            {
            SendErrorMsg( KErrNotSupported, KErrDescrNotSupported );
            }
        }
    else
        {
        SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
        }
    HTI_LOG_FUNC_OUT( "CHtiAppServicePlugin::ProcessMessage" );
    }

void CHtiAppServicePlugin::HandleProcessControlL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiAppServicePlugin::HandleProcessControl" );
    TBool unicode = aMessage[0] & KUnicodeMask;

    HTI_LOG_FORMAT( "unicode %d", unicode );

    TFileName programName;

    switch ( aMessage[0] )
        {
        case EStartProcess:
        case EStartProcess_u:
            {
            TFileName cmdLine;
            TInt offset = ParseString( aMessage, 1, unicode, programName );
            if ( offset >= 0 )
                {
                offset = ParseString( aMessage, offset, unicode, cmdLine );
                if ( offset >= 0 )
                    {
                    HandleStartProcessL( programName, cmdLine, EFalse );
                    }
                }
            if ( offset < 0 )
                {
                SendErrorMsg( offset, KErrDescrInvalidArguments );
                }
            }
            break;
        case EStopProcess:
        case EStopProcess_u:
            {
            TInt err = ParseString( aMessage, 1, unicode, programName );
            if ( err >= 0 )
                {
                RProcess process;
                err = OpenProcessL( process, programName );
                if ( err == KErrNone )
                    {
                    CleanupClosePushL( process );

                    HandleStopProcessL( process );

                    CleanupStack::PopAndDestroy();
                    }
                else if ( err == KErrNotFound )
                    {
                    SendMessageL( ENotFound );
                    }
                else
                    {
                    SendErrorMsg( err , KErrDescrFailedOpenProcess );
                    }
                }
            else
                {
                SendErrorMsg( err , KErrDescrInvalidArguments );
                }
            }
            break;
        case EStopProcess_id:
            {
            TPtrC8 processId8 = aMessage.Mid( 1 );
            if ( processId8.Length() == 4 )
                {
                RProcess process;
                TUint32 processId = Parse32<TUint32>( processId8 );
                TInt err = process.Open( TProcessId( processId ) );
                if ( err == KErrNone )
                    {
                    CleanupClosePushL( process );

                    HandleStopProcessL( process );

                    CleanupStack::PopAndDestroy();
                    }
                else if ( err == KErrNotFound )
                    {
                    SendMessageL( ENotFound );
                    }
                else
                    {
                    SendErrorMsg( err , KErrDescrFailedOpenProcess );
                    }
                }
            else
                {
                SendErrorMsg( KErrArgument , KErrDescrInvalidProcessId );
                }
            }
            break;
        case EStatusProcess:
        case EStatusProcess_u:
            {
            TInt err = ParseString( aMessage, 1, unicode, programName );
            if ( err >= 0 )
                {
                RProcess process;
                err = OpenProcessL( process, programName );
                if ( err == KErrNone )
                    {
                    CleanupClosePushL( process );

                    HandleStatusProcessL( process );

                    CleanupStack::PopAndDestroy();
                    }
                else if ( err == KErrNotFound )
                    {
                    SendMessageL( ENotFound );
                    }
                else
                    {
                    SendErrorMsg( err , KErrDescrFailedOpenProcess );
                    }
                }
            else
                {
                SendErrorMsg( err , KErrDescrInvalidArguments );
                }
            }
            break;
        case EStatusProcess_id:
            {
            TPtrC8 processId8 = aMessage.Mid( 1 );
            if ( processId8.Length() == 4 )
                {
                RProcess process;
                TUint32 processId = Parse32<TUint32>( processId8 );
                TInt err = process.Open( TProcessId( processId ) );
                if ( err == KErrNone )
                    {
                    CleanupClosePushL( process );

                    HandleStatusProcessL( process );

                    CleanupStack::PopAndDestroy();
                    }
                else if ( err == KErrNotFound )
                    {
                    SendMessageL( ENotFound );
                    }
                else
                    {
                    SendErrorMsg( err , KErrDescrFailedOpenProcess );
                    }
                }
            else
                {
                SendErrorMsg( KErrArgument , KErrDescrInvalidProcessId );
                }
            }
            break;
        case EListProcesses:
        case EListProcesses_u:
            {
            if ( aMessage.Length() > 1 )
                {
                // there is a match pattern as a parameter
                TInt err = ParseString( aMessage, 1, unicode, programName );
                if ( err >= 0 )
                    {
                    HandleListProcessesL( programName );
                    }
                else
                    {
                    SendErrorMsg( err , KErrDescrInvalidArguments );
                    }
                }
            else
                {
                // no match pattern defined
                HandleListProcessesL( KNullDesC );
                }

            }
            break;

        case EStartProcessRetVal:
        case EStartProcessRetVal_u:
            {
            TFileName cmdLine;
            TInt offset = ParseString( aMessage, 1, unicode, programName );
            if ( offset >= 0 )
                {
                offset = ParseString( aMessage, offset, unicode, cmdLine );
                if ( offset >= 0 )
                    {
                    HandleStartProcessL( programName, cmdLine, ETrue );
                    }
                }
            if ( offset < 0 )
                {
                SendErrorMsg( offset, KErrDescrInvalidArguments );
                }
            }
            break;

        case EGetProcessExitCode:
            {
            HTI_LOG_TEXT( "EGetProcessExitCode" );
            TPtrC8 processId8 = aMessage.Mid( 1 );
            if ( processId8.Length() == 4 )
                {
                TBool processFound = EFalse;
                TUint processId = Parse32<TUint>( processId8 );
                RProcess process;
                TBuf8<1 + 4 + 1 + KMaxExitCategoryName> response;

                HTI_LOG_FORMAT( "Starting to search process with id: %d", processId );

                for ( TInt i = 0; i < iProcessHandleArray.Count(); i++ )
                    {
                    process = iProcessHandleArray[i];
                    TUint id = process.Id();

                    if ( id == processId )
                        {
                        HTI_LOG_TEXT( "Matching process found" );

                        TInt exitReason = process.ExitReason();
                        response.Append( process.ExitType() );
                        response.Append( (TUint8*)(&exitReason), 4 );
                        response.Append( process.ExitCategory().Length() );
                        response.Append( process.ExitCategory() );

                        SendMessageL( EOk, response );

                        // Close the process handle and remove it from the array
                        // if the process is not running anymore and the exit
                        // code has been queried.
                        if ( process.ExitType() != EExitPending )
                            {
                            HTI_LOG_TEXT( "Closing and removing the handle" );
                            iProcessHandleArray[i].Close();
                            iProcessHandleArray.Remove( i );
                            }

                        processFound = ETrue;
                        break;
                        }
                    }

                if ( !processFound )
                    {
                    SendErrorMsg( KErrNotFound , KErrDescrInvalidProcessId );
                    }
                }
            else
                {
                SendErrorMsg( KErrArgument , KErrDescrInvalidProcessId );
                }
            }
            break;

        default:
            {
            SendErrorMsg( KErrArgument, KErrDescrInvalidCmd );
            }
        }
    HTI_LOG_FUNC_OUT( "CHtiAppServicePlugin::HandleProcessControl" );
    }

void CHtiAppServicePlugin::HandleStartProcessL( const TDesC& aProgramName,
                                                const TDesC& aCmdLine,
                                                TBool aStoreProcessHandle )
    {
    HTI_LOG_FUNC_IN( "CHtiAppServicePlugin::HandleStartProcessL" );
    HTI_LOG_FORMAT( "progr name %d", aProgramName.Length() );
    HTI_LOG_DES( aProgramName );

    RProcess process;
    TInt err = process.Create( aProgramName, aCmdLine ); // command parameters

    if ( err == KErrNone )
        {
        CleanupClosePushL( process );

        //convert process id to binary des
        TUint processId = process.Id();
        HTI_LOG_FORMAT( "process id %d", processId );

        TBuf8<KTUintLength> processIdDes;
        processIdDes.Append(
                ( TUint8* )( &processId ), KTUintLength );

        SendMessageL( EOk, processIdDes );

        process.Resume();

        if ( aStoreProcessHandle )
            {
            HTI_LOG_TEXT( "Storing the process handle" );
            iProcessHandleArray.Append( process );
            CleanupStack::Pop();
            }
        else
            {
            CleanupStack::PopAndDestroy();
            }
        }
    else if ( err == KErrNotFound )
        {
        SendMessageL( ENotFound );
        }
    else
        {
        SendErrorMsg( err ,KErrDescrFailedCreateProcess );
        }

    HTI_LOG_FUNC_OUT( "CHtiAppServicePlugin::HandleStartProcessL" );
    }

void CHtiAppServicePlugin::HandleStopProcessL( RProcess& aProcess )
    {
    if ( aProcess.ExitType() == EExitPending )
        {
        aProcess.Kill( KTerminateReason );
        SendMessageL( EOk );
        }
    else
        {
        SendMessageL( EAlreadyStopped );
        }
    }

void CHtiAppServicePlugin::HandleStatusProcessL( RProcess& aProcess )
    {
    TExitType exT = aProcess.ExitType();

    switch ( exT )
        {
        case EExitPending:
            {
            SendMessageL( ERunning );
            }
            break;
        case EExitKill:
        case EExitTerminate:
            {
            SendMessageL( EKilled );
            }
            break;
        case EExitPanic:
            {
            SendMessageL( EPanic );
            }
            break;
        };
    }


void CHtiAppServicePlugin::HandleListProcessesL( const TDesC& aMatch )
    {
    HTI_LOG_FUNC_IN( "CHtiAppServicePlugin::HandleListProcessesL" );

    RProcess process;
    TFullName processName;
    TUint processId;
    TBuf8<KTUintLength> processIdDes;
    TExitType exitType;

    TBuf8<128> buf;
    CBufFlat* processListBuf = NULL;
    TRAPD( err, processListBuf = CBufFlat::NewL( 128 ) );
    if ( err )
        {
        SendErrorMsg( err ,KErrDescrNoMemory );
        return;
        }

    CleanupStack::PushL( processListBuf );

    // The default match pattern is the single character *
    TFindProcess finder;

    // if some real match pattern is defined, use it
    if ( aMatch.Length() > 0 )
        {
        HTI_LOG_TEXT( "Match pattern was given:" );
        HTI_LOG_DES( aMatch );
        finder.Find( aMatch );
        }

    TInt pos = 0;
    TUint numberOfEntries = 0;

    while ( finder.Next( processName ) == KErrNone )
        {
        err = process.Open( finder );

        //convert process id to binary des
        processId = process.Id();
        processIdDes.Append( ( TUint8* ) ( &processId ), KTUintLength );
        buf.Append( processIdDes );

        // status
        exitType = process.ExitType();
        switch ( exitType )
            {
            case EExitPending:
                {
                buf.Append( ERunning );
                }
                break;
            case EExitKill:
            case EExitTerminate:
                {
                buf.Append( EKilled );
                }
                break;
            case EExitPanic:
                {
                buf.Append( EPanic );
                }
                break;
            };

        // name length
        buf.Append( processName.Length() );

        // name
        buf.Append( processName );

        process.Close();

        TRAP( err, processListBuf->ExpandL( pos, buf.Length() ) );
        if ( err )
            {
            SendErrorMsg( err , KErrDescrNoMemory );
            delete processListBuf;
            return;
            }
        processListBuf->Write( pos, buf, buf.Length() );

        pos += buf.Length();
        buf.Zero();
        processIdDes.Zero();
        numberOfEntries++;
        }

    // insert the number of entries in the beginning
    TBuf8<2> entries;
    entries.Append( ( TUint8* ) ( &numberOfEntries ), 2 );
    processListBuf->ExpandL( 0, 2 );
    processListBuf->Write( 0, entries, 2 );

    SendMessageL( EOk, processListBuf->Ptr( 0 ) );

    CleanupStack::PopAndDestroy( processListBuf );

    HTI_LOG_FUNC_OUT( "CHtiAppServicePlugin::HandleListProcessesL" );
    }

TInt CHtiAppServicePlugin::OpenProcessL( RProcess& aProcess,
                                         const TDesC& aMatch )
    {
    HTI_LOG_FUNC_IN( "CHtiAppServicePlugin::OpenProcessL" );
    TFullName processName;
    TInt err = KErrNone;
    TFindProcess finder( aMatch );

    err = finder.Next( processName );
    if ( err == KErrNone )
        {
        err = aProcess.Open( finder );
        }

    HTI_LOG_FUNC_OUT( "CHtiAppServicePlugin::OpenProcessL" );
    return err;
    }

template<class T> T CHtiAppServicePlugin::Parse32(
                        const TDesC8& a32int )
    {
    //manually construct TUint or TInt32
    return T( a32int[0] + ( a32int[1] << 8 ) +
                      ( a32int[2] << 16) + ( a32int[3] << 24) );
    }

void CHtiAppServicePlugin::SendMessageL( TAppCommand aResponseCode,
                                         const TDesC8& aMsg )
    {
    HTI_LOG_FORMAT( "SendMessage %d", aResponseCode );
    HTI_LOG_FORMAT( "Message len %d", aMsg.Length() );
    HBufC8* sendMsg = HBufC8::NewL( 1 + aMsg.Length() );
    CleanupStack::PushL( sendMsg );
    sendMsg->Des().Append( aResponseCode );
    sendMsg->Des().Append( aMsg );

    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
                            sendMsg,
                            KAppServiceUid ) );
    CleanupStack::Pop();
    }

inline TInt CHtiAppServicePlugin::SendErrorMsg( TInt anError,
                                                const TDesC8& aMsg )
    {
    return iDispatcher->DispatchOutgoingErrorMessage( anError,
                                               aMsg,
                                               KAppServiceUid );
    }


// End of File
