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
* Description:  HtiFtpServicePlugin implementation
*
*/


// INCLUDE FILES
#include "HtiFtpServicePlugin.h"
#include <HtiFtpBackupFakeBase.h>
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

#include <e32property.h>
#include <hash.h>

// CONSTANTS
static const TUid KFtpServiceUid = { 0x1020DEC5 };
_LIT( KBackslash, "\\" );
_LIT( KRootPathFormat, "%c:\\" );
_LIT( KHtiFtpBackupFakeDllName, "HtiFtpBackupFake.dll" );
const static TInt KMinBufferSize = 1024;
const static TUint8 KUnicodeMask = 0x1;
const static TInt KFileSizeMsgSize = 5;

_LIT(KHtiTcbHlp, "HtiTcbHlp.exe");

_LIT( KCmdAppRegInfo,           "AppRegInfo");
_LIT( KCmdAppRegInfo_Add,       "add" );
_LIT( KCmdAppRegInfo_Remove,    "remove" );
_LIT( KDelimiter, "|" );

//error description
_LIT8( KErrDescrUnknownCmd, "unknown command" );
_LIT8( KErrDescrNoSpace, "no disk space" );
_LIT8( KErrDescrFailedRead, "failed read file" );
_LIT8( KErrDescrFailedWrite, "failed write file" );
_LIT8( KErrDescrFailedGetDir, "failed to read directory" );
_LIT8( KErrDescrEmptyDirname, "directory name empty" );
_LIT8( KErrDescrInvalidDirnameLength, "invalid directory name length" );
_LIT8( KErrDescrEmptyFilename, "file name empty" );
_LIT8( KErrDescrInvalidFilenameLength, "invalid file name length" );
_LIT8( KErrDescrFailedMkDir, "failed create directory" );
_LIT8( KErrDescrFailedRmDir, "failed remove directory" );
_LIT8( KErrDescrFailedDeleFile, "failed delete file" );
_LIT8( KErrDescrFailedRenameFile, "failed rename file" );
_LIT8( KErrDescrFailedCopyFile, "failed copy file" );
_LIT8( KErrDescrFailedMoveFile, "failed move file" );
_LIT8( KErrDescrInvalidStorArgument, "invalid arguments" );
_LIT8( KErrDescrFailedCreateFile, "invalid create file" );
_LIT8( KErrDescrFailedOpenFile, "failed open file" );
_LIT8( KErrDescrFailedCloseFile, "failed close file" );
_LIT8( KErrDescrInvalidDataMessage, "invalid data message" );
_LIT8( KErrDescrNoMemory, "no memory to send file" );
_LIT8( KErrDescrNoCancel, "nothing to cancel" );
_LIT8( KErrDescrBusy, "Busy" );
_LIT8( KErrDescrFailedCopyTcb, "Failed to copy to Tcb directories" );
_LIT8( KErrDescrInvalidForceArgs, "invalid arguments for setforce" );
_LIT8( KErrDescrInvalidChecksumArgs, "invalid arguments for checksum" );
_LIT8( KErrDescrInvalidFormatArgs, "invalid arguments for format" );
_LIT8( KErrDescrInvalidDriveListArgs, "invalid arguments for drive list" );
_LIT8( KErrDescrFailedFormat, "failed to format" );
_LIT8( KErrDescrNotSupported, "command not supported" );
_LIT8( KErrDescrFailedRegisterApp, "failed to register application");
_LIT8( KErrDescrFailedUnregisterApp, "failed to unregister application");

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

/**
* CFtpHandlerAO implementation
*/
CFtpHandlerAO::CFtpHandlerAO( MFtpObserverAO* anObserver ):
    CActive( EPriorityStandard ),
    iObserver( anObserver ),
    iCancelFileMan( EFalse )
    {
    CActiveScheduler::Add( this );
    }

CFtpHandlerAO::~CFtpHandlerAO()
    {
    }

void CFtpHandlerAO::Start()
    {
    SetActive();
    }

MFileManObserver::TControl CFtpHandlerAO::NotifyFileManStarted()
    {
    return iCancelFileMan?
                MFileManObserver::EAbort :
                MFileManObserver::EContinue;
    }

MFileManObserver::TControl CFtpHandlerAO::NotifyFileManOperation()
    {
    return iCancelFileMan?
                MFileManObserver::ECancel :
                MFileManObserver::EContinue;
    }

MFileManObserver::TControl CFtpHandlerAO::NotifyFileManEnded()
    {
    return iCancelFileMan?
                MFileManObserver::EAbort :
                MFileManObserver::EContinue;
    }

void CFtpHandlerAO::RunL()
    {
    iObserver->FtpComplete( iStatus.Int() );
    }

void CFtpHandlerAO::DoCancel()
    {
    iCancelFileMan = ETrue;
    }

CProcessLogonAO::CProcessLogonAO( MFtpObserverAO* anObserver ):
    CActive( EPriorityStandard ),
    iObserver( anObserver )
    {
    CActiveScheduler::Add( this );
    }

CProcessLogonAO::~CProcessLogonAO()
    {
    Cancel();
    iProcess.Close();
    }

void CProcessLogonAO::Start( const TDesC& aCmdLine )
    {
    TInt err = iProcess.Create( KHtiFileHlp, aCmdLine );

    if ( err == KErrNone )
        {
        iProcess.Logon( iStatus );
        //start HtiFileHlp.exe
        iProcess.Resume();
        }
    else
        {
        TRequestStatus* pS = &iStatus;
        User::RequestComplete( pS, err );
        }
    SetActive();
    }

void CProcessLogonAO::RunL()
    {
    iObserver->FtpComplete( iStatus.Int() );
    }

void CProcessLogonAO::DoCancel()
    {
    iProcess.LogonCancel( iStatus );
    }

// Create instance of concrete ECOM interface implementation
CHtiFtpServicePlugin* CHtiFtpServicePlugin::NewL()
    {
    CHtiFtpServicePlugin* self = new (ELeave) CHtiFtpServicePlugin;
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Constructor
CHtiFtpServicePlugin::CHtiFtpServicePlugin():
    iState( EIdle ),
    iSendBuffer( NULL ),
    iSendBufferDes( NULL, 0 ),
    iHandlerAO( NULL ),
    iProcessLogonAO( NULL )
    {
    }

CHtiFtpServicePlugin::~CHtiFtpServicePlugin()
    {
    HTI_LOG_FUNC_IN("~CHtiFtpServicePlugin");
    delete iSendBuffer;
    delete iHandlerAO;
    delete iProcessLogonAO;
    delete iFileMan;
    delete iBackupFake;
    iBackupFakeLib.Close();
    iFs.Close();
    HTI_LOG_FUNC_OUT("~CHtiFtpServicePlugin");
    }

// Second phase construction.
void CHtiFtpServicePlugin::ConstructL()
    {
    HTI_LOG_FUNC_IN("CHtiFtpServicePlugin::ConstructL");

    User::LeaveIfError( iFs.Connect() );
    iFileMan = CFileMan::NewL( iFs );

    HTI_LOG_FUNC_OUT("CHtiFtpServicePlugin::ConstructL");
    }

void CHtiFtpServicePlugin::InitL()
    {
    if ( iDispatcher->GetFreeMemory() < KMinBufferSize )
        {
        User::Leave( KErrNoMemory );
        }
    // just guess

    iBufferSize = iDispatcher->GetFreeMemory()/10;
    iBufferSize = iBufferSize < KMinBufferSize ?
                    KMinBufferSize :
                    iBufferSize;
    //test
    iBufferSize = 3*KMinBufferSize;
    }

TInt CHtiFtpServicePlugin::SendControlMsg( TFtpCommand aCmd,
                                           const TDesC8& aMsg  )
    {
    TInt err = KErrNone;
    HBufC8* temp = NULL;
    TRAP( err, temp = HBufC8::NewL( 1 + aMsg.Size() ) );
    if ( err == KErrNone )
        {
        temp->Des().Append( aCmd );
        temp->Des().Append( aMsg );
        err = iDispatcher->DispatchOutgoingMessage( temp,
                        KFtpServiceUid,
                        EFalse,
                        EHtiPriorityControl );
        if ( err != KErrNone )
            {
            delete temp;
            }
        }
    return err;
    }

inline TInt CHtiFtpServicePlugin::SendErrorMsg( TInt anError,
                                                const TDesC8& aMsg )
    {
    SetBURFakeState( EFalse ); // errors ignored
    return iDispatcher->DispatchOutgoingErrorMessage( anError,
                                               aMsg,
                                               KFtpServiceUid);
    }

TBool CHtiFtpServicePlugin::IsBusy()
    {
    return !( iState == EIdle || iState == EStorWait || iState == ERetrWait );
    }

void CHtiFtpServicePlugin::ProcessMessageL(const TDesC8& aMessage,
                THtiMessagePriority aPriority)
    {
    HTI_LOG_FUNC_IN("CHtiFtpServicePlugin::ProcessMessage");

    if ( IsBusy() )
        {
        //should not happend - service busy,deny request
        User::Leave( KErrInUse );
        }

    switch ( aPriority )
        {
        case EHtiPriorityData:
            {
            HandleDataMessageL( aMessage );
            }
            break;
        case EHtiPriorityControl:
            {
            if ( iState == EStorWait || iState == ERetrWait )
                {
                HandleCancelL( aMessage );
                }
            else
                {
                HandleControlMessageL( aMessage );
                }
            }
            break;
        default:
            HTI_LOG_TEXT("Unknown priority");
        }

    HTI_LOG_FUNC_OUT("CHtiFtpServicePlugin::ProcessMessage");
    }

void CHtiFtpServicePlugin::HandleControlMessageL(const TDesC8& aMessage)
    {
    TInt err = KErrNone;

    if ( aMessage.Length() > 0 )
        {
        TBool unicode = aMessage[0]&KUnicodeMask;
        HTI_LOG_FORMAT("cmd %d", aMessage[0] );

        switch ( aMessage[0] )
            {
            case EFtpSTOR:
            case EFtpSTOR_u:
                {
                //receive file
                //get filesize
                if ( aMessage.Length() > 5 )
                    {
                    iFileSize = aMessage[1] +
                                ( aMessage[2] << 8  ) +
                                ( aMessage[3] << 16 ) +
                                ( aMessage[4] << 24 );

                    HTI_LOG_FORMAT( "Filesize %d", iFileSize );
                    //get fileName
                    if ( GetFileNameL( aMessage.Mid( 5 ),
                                      unicode ) )
                        {
                        HandleReceiveFileL();
                        }
                    }
                else
                    {
                    HTI_LOG_TEXT("no file size or file name");
                    User::LeaveIfError( SendErrorMsg( KErrArgument,
                                            KErrDescrInvalidStorArgument) );

                    }
                }
                break;
            case EFtpRETR:
            case EFtpRETR_u:
                {
                //send file
                //get fileName and size
                if ( GetFileNameL( aMessage.Mid(1),
                                   unicode ) )
                    {
                    HandleSendFileL();
                    }
                }
                break;
            case EFtpLIST:
            case EFtpLIST_u:
            case EFtpLISTSIZES:
            case EFtpLISTSIZES_u:
                {
                if ( GetDirectoryL( aMessage.Mid( 1 ),
                                   unicode ) )
                    {
                    HandleListL( unicode,
                        ( KEntryAttHidden| KEntryAttSystem|KEntryAttNormal ),
                        ( aMessage[0] == EFtpLISTSIZES ||
                          aMessage[0] == EFtpLISTSIZES_u ) );
                    }
                }
                break;
            case EFtpListDetail:
            case EFtpListDetail_u:
                {
                if ( GetDirectoryL( aMessage.Mid( 1 ),
                                   unicode ) )
                    {
                    HandleListDetailL( unicode,
                        KEntryAttHidden| KEntryAttSystem|KEntryAttNormal);
                    }
                }
                break;
            case EFtpLISTDIR:
            case EFtpLISTDIR_u:
                {
                if ( GetDirectoryL( aMessage.Mid( 1 ),
                                   unicode ) )
                    {
                    HandleListL( unicode,
                                (KEntryAttMatchExclusive|
                                KEntryAttHidden|KEntryAttSystem|
                                KEntryAttDir), EFalse );
                    }
                }
                break;
            case EFtpListDirDetail:
            case EFtpListDirDetail_u:
                {
                if ( GetDirectoryL( aMessage.Mid( 1 ),
                                   unicode ) )
                    {
                    HandleListDetailL( unicode,
                            KEntryAttMatchExclusive|KEntryAttHidden|
                            KEntryAttSystem|KEntryAttDir);
                    }
                }
                break;
            case EFtpMKD:
            case EFtpMKD_u:
                {
                if ( GetDirectoryL( aMessage.Mid( 1 ),
                                   unicode ) )
                    {
                    if ( IsFileTcb( iFileName ) )
                        {
                        HandleTcbMkdL( iFileName );
                        }
                    else
                        {
                        err = iFs.MkDirAll( iFileName );
                        if ( err == KErrNone || err == KErrAlreadyExists )
                            {
                            User::LeaveIfError( SendControlMsg( EFtpOK,
                                                                KNullDesC8) );
                            }
                        else
                            {
                            User::LeaveIfError( SendErrorMsg( err,
                                                        KErrDescrFailedMkDir ) );

                            }
                        }
                    }
                }
                break;
            case EFtpRMD:
            case EFtpRMD_u:
                {
                if ( GetDirectoryL( aMessage.Mid( 1 ),
                                   unicode ) )
                    {
                    if ( IsFileTcb( iFileName ) )
                        {
                        HandleTcbRmdL( iFileName );
                        }
                    else
                        {
                        delete iHandlerAO;
                        iHandlerAO = NULL;

                        iHandlerAO = new(ELeave) CFtpHandlerAO( this );
                        err = iFileMan->RmDir( iFileName, iHandlerAO->iStatus );
                        if ( err == KErrNone)
                            {
                            iState = ERmdBusy;
                            iHandlerAO->Start();
                            }
                        else
                            {
                            delete iHandlerAO;
                            iHandlerAO = NULL;
                            User::LeaveIfError( SendErrorMsg( err,
                                                        KErrDescrFailedRmDir ) );
                            }
                        }
                    }
                }
                break;
            case EFtpDELE:
            case EFtpDELE_u:
                {
                if ( GetFileNameL( aMessage.Mid( 1 ),
                                   unicode ) )
                    {
                    if ( IsFileTcb( iFileName ) )
                        {
                        HandleTcbDeleteL( iFileName );
                        }
                    else
                        {
                        HandleDeleteL( iFileName );
                        }
                    }
                }
                break;
            case EFtpCANCEL:
                {
                User::LeaveIfError( SendErrorMsg( KErrArgument,
                                                KErrDescrNoCancel ) );
                }
                break;
            case EFtpSETFORCE:
                {
                if ( aMessage.Length() != 2 )
                    {
                    User::LeaveIfError( SendErrorMsg( KErrArgument,
                                                KErrDescrInvalidForceArgs ) );
                    }
                else
                    {
                    if ( aMessage[1] )
                        {
                        // Setting forced operations on
                        err = iBackupFakeLib.Load( KHtiFtpBackupFakeDllName );
                        HTI_LOG_FORMAT( "BackupFake DLL load returned %d", err );
                        if ( err == KErrNone && iBackupFakeLib.Type()[1] ==
                                KHtiFtpBackupFakeInterfaceUid )
                            {
                            HTI_LOG_TEXT( "BackupFake DLL found" );
                            TLibraryFunction entry = iBackupFakeLib.Lookup( 1 );
                            if ( entry != NULL )
                                {
                                iBackupFake = ( CHtiFtpBackupFakeBase* ) entry();
                                TRAP( err, iBackupFake->ConstructL( &iFs ) );
                                }
                            }
                        if ( err == KErrNone )
                            {
                            SendControlMsg( EFtpOK, KNullDesC8 );
                            }
                        else
                            {
                            User::LeaveIfError( SendErrorMsg( KErrNotSupported,
                                KErrDescrNotSupported ) );
                            }
                        }
                    else
                        {
                        // Setting forced operations off
                        delete iBackupFake;
                        iBackupFake = NULL;
                        iBackupFakeLib.Close();
                        SendControlMsg( EFtpOK, KNullDesC8 );
                        }
                    }
                }
                break;
            case EFtpCHECKSUM:
            case EFtpCHECKSUM_u:
                {
                if ( aMessage.Length() < 3 )
                    {
                    User::LeaveIfError(
                        SendErrorMsg( KErrArgument,
                                      KErrDescrInvalidChecksumArgs ) );
                    }
                else if ( GetFileNameL( aMessage.Mid( 2 ), unicode ) )
                    {
                    HandleCheckSumCalcL( (TAlgorithm)aMessage[1], iFileName );
                    }
                }
                break;
            case EFtpFORMAT:
                {
                if ( aMessage.Length() != 3 )
                    {
                    User::LeaveIfError( SendErrorMsg( KErrArgument,
                            KErrDescrInvalidFormatArgs ) );
                    }
                else
                    {
                    HandleFormat( aMessage[1], aMessage[2] );
                    }
                }
                break;
            case EFtpLISTDRIVES:
            case EFtpLISTDRIVES_u:
                {
                if ( aMessage.Length() != 1 )
                    {
                    User::LeaveIfError( SendErrorMsg( KErrArgument,
                        KErrDescrInvalidDriveListArgs ) );
                    }
                else
                    {
                    HandleListDrivesL( unicode );
                    }
                }
                break;
            case EFtpRENAME:
            case EFtpRENAME_u:
                {
                HandleRenameL( aMessage, unicode );
                }
                break;
            case EFtpCOPY:
            case EFtpCOPY_u:
                {
                HandleCopyL( aMessage, unicode );
                }
                break;
            case EFtpMOVE:
            case EFtpMOVE_u:
                {
                HandleMoveL( aMessage, unicode );
                }
                break;
            default:
                {
                //Error: unknown command
                User::LeaveIfError( SendErrorMsg( KErrArgument,
                                                KErrDescrUnknownCmd ) );
                }
            }
        }
    else
        {
        //error: empty request
        User::LeaveIfError( SendErrorMsg( KErrArgument, KErrDescrUnknownCmd ) );
        }
    }

void CHtiFtpServicePlugin::HandleDeleteL( const TDesC& aFilename )
    {
    HTI_LOG_FUNC_IN( "CHtiFtpServicePlugin::HandleDeleteL" );
    
    //If the destination file is a .rsc file in apparc's pvt directory
    //remove application registration from Software Component Registry (SCR) to unregister application
    TRAPD(err,HandleAppRegFileInfoL(iFileName, EFalse));
    if(err)
        {
        SendErrorMsg(err, KErrDescrFailedUnregisterApp);
        return;
        }
    
    delete iHandlerAO;
    iHandlerAO = NULL;

    iHandlerAO = new(ELeave) CFtpHandlerAO( this );
    err = iFileMan->Delete( aFilename, 0, iHandlerAO->iStatus );
    if ( err == KErrNone)
        {
        iState = EDeleBusy;
        iHandlerAO->Start();
        }
    else
        {
        HTI_LOG_FORMAT( "Delete failed %d", err );
        delete iHandlerAO;
        iHandlerAO = NULL;
        User::LeaveIfError( SendErrorMsg( err,
                                    KErrDescrFailedDeleFile ) );
        }
    HTI_LOG_FUNC_OUT( "CHtiFtpServicePlugin::HandleDeleteL" );
    }

void CHtiFtpServicePlugin::HandleRenameL( const TDesC8& aMessage, TBool aUnicode )
    {
    delete iHandlerAO;
    iHandlerAO = NULL;

    TInt originalLenngth = aMessage[1];

    GetFileNameL( aMessage.Mid( 1, originalLenngth + 1 ), aUnicode );

    //If last character is back slash remove it
    RemoveEndBackslash(iFileName);

    TFileName origName = iFileName;

    TInt newNamePos = originalLenngth + 2 ;
    TInt newNameLength = aMessage[newNamePos];
    GetFileNameL( aMessage.Mid( newNamePos, newNameLength + 1 ), aUnicode );

    RemoveEndBackslash( iFileName );

    if ( IsFileTcb( origName ) || IsFileTcb( iFileName ) )
        {
        HandleTcbRenameL( origName, iFileName );
        }
    else
        {
        iHandlerAO = new ( ELeave ) CFtpHandlerAO( this );
        TInt err = iFileMan->Rename( origName, iFileName,
                    CFileMan::EOverWrite, iHandlerAO->iStatus );

        if ( err == KErrNone )
            {
            iState = ERenameBusy;
            iHandlerAO->Start();
            }
        else
            {
            HTI_LOG_FORMAT( "Rename failed %d", err );
            delete iHandlerAO;
            iHandlerAO = NULL;
            User::LeaveIfError( SendErrorMsg(
                    err, KErrDescrFailedRenameFile ) );
            }
        }

    }

void CHtiFtpServicePlugin::RemoveEndBackslash( TFileName& aFileName  )
    {
    //If last character is back slash remove it
    if ( aFileName.Right( 1 ) == KBackslash )
        {
        aFileName.Delete( aFileName.Length() - 1, 1 );
        }
    }

void CHtiFtpServicePlugin::HandleCopyL( const TDesC8& aMessage, TBool aUnicode )
    {
    delete iHandlerAO;
    iHandlerAO = NULL;

    TInt originalLenngth = aMessage[1];

    GetFileNameL( aMessage.Mid( 1, originalLenngth + 1 ), aUnicode );
    TFileName origName = iFileName;

    TInt newNamePos = originalLenngth + 2 ;
    TInt newNameLength = aMessage[newNamePos];
    GetFileNameL( aMessage.Mid( newNamePos, newNameLength + 1 ), aUnicode );

    TInt msglen = aMessage.Length();
    TInt recurse = 1;
    if(msglen>originalLenngth+newNameLength+3)        
        {
        recurse = aMessage[newNamePos+1+newNameLength];
        }
    
    TInt err=0;
    if ( IsFileTcb( origName ) || IsFileTcb( iFileName ) )
        {
        HandleTcbCopyL( origName, iFileName );
        }
    else{
        iHandlerAO = new ( ELeave ) CFtpHandlerAO( this );
        
        if (recurse)
            {
            err = iFileMan->Copy( origName, iFileName,
                ( CFileMan::EOverWrite | CFileMan::ERecurse ),
                iHandlerAO->iStatus );
            }
        else
            {
            err = iFileMan->Copy( origName, iFileName,
                ( CFileMan::EOverWrite ),
                iHandlerAO->iStatus );
            }

        if ( err == KErrNone)
            {
            iState = ECopyBusy;
            iHandlerAO->Start();
            }
        else
            {
            HTI_LOG_FORMAT( "Rename failed %d", err );
            delete iHandlerAO;
            iHandlerAO = NULL;
            User::LeaveIfError( SendErrorMsg( err,
                                        KErrDescrFailedCopyFile ) );
            }
        }
    }

void CHtiFtpServicePlugin::HandleMoveL( const TDesC8& aMessage, TBool aUnicode )
    {
    delete iHandlerAO;
    iHandlerAO = NULL;

    TInt originalLenngth = aMessage[1];

    GetFileNameL( aMessage.Mid( 1, originalLenngth + 1 ), aUnicode );

    RemoveEndBackslash( iFileName );

    TFileName origName = iFileName;

    TInt newNamePos = originalLenngth + 2 ;
    TInt newNameLength = aMessage[newNamePos];
    GetFileNameL( aMessage.Mid( newNamePos, newNameLength + 1 ), aUnicode );
    // make sure destination ends with backslash - destination always directory
    if ( iFileName.Right( 1 ) != KBackslash )
        {
        iFileName.Append( KBackslash );
        }

    if ( IsFileTcb( origName ) || IsFileTcb( iFileName ) )
        {
        HandleTcbMoveL( origName, iFileName );
        }
    else
        {
        iHandlerAO = new ( ELeave ) CFtpHandlerAO( this );
        TInt err = iFileMan->Move( origName, iFileName,
                ( CFileMan::EOverWrite | CFileMan::ERecurse ),
                iHandlerAO->iStatus );

        if ( err == KErrNone )
            {
            iState = EMoveBusy;
            iHandlerAO->Start();
            }
        else
            {
            HTI_LOG_FORMAT( "Move failed %d", err );
            delete iHandlerAO;
            iHandlerAO = NULL;
            User::LeaveIfError( SendErrorMsg( err,
                                        KErrDescrFailedMoveFile ) );
            }
        }
    }

void CHtiFtpServicePlugin::HandleCancelL(const TDesC8& aMessage)
    {

    if ( aMessage.Length()>0 )
        {
        HTI_LOG_FORMAT("cmd %d", aMessage[0]);

        if ( aMessage[0] == EFtpCANCEL )
            {
            //handle cancel
            iFile.Close();

            if ( iState == EStorWait )
                {
                iFs.Delete(iFileName);
                }
            else if ( iState == ERetrWait )
                {
                iDispatcher->RemoveMemoryObserver(this);

                delete iSendBuffer;
                iSendBuffer = NULL;
                }
            //other states filtered out before

            delete iHandlerAO;
            iHandlerAO = NULL;

            iState = EIdle;

            User::LeaveIfError( SendControlMsg( EFtpOK, KNullDesC8) );
            }
        else
            {
            User::LeaveIfError( SendErrorMsg( KErrServerBusy,
                            KErrDescrBusy) );
            }
        }
    else
        {
        //send err
        User::LeaveIfError( SendErrorMsg( KErrArgument,
                            KErrDescrUnknownCmd ) );
        }
    }

void CHtiFtpServicePlugin::CopyUnicode( TDes& aTo, const TDesC8& aFrom )
{
    HTI_LOG_FUNC_IN("CHtiFtpServicePlugin::CopyUnicode");
    TInt len = aFrom.Length()>>1;
    aTo.SetLength( len );
    for ( TInt i = 0; i < len; ++i )
    {
        aTo[i] = (TUint16)aFrom[i<<1] + (((TUint16)aFrom[(i<<1)+1])<<8);
    }
    HTI_LOG_FUNC_OUT("CHtiFtpServicePlugin::CopyUnicode");
}

TBool CHtiFtpServicePlugin::GetFileNameL( const TDesC8& aFilename,
                                          TBool aToUnicode )
    {
    HTI_LOG_FUNC_IN("CHtiFtpServicePlugin::GetFileNameL");
    if ( aFilename.Length() > 1 )
        {
        TInt length = aFilename[0];
        TInt size = aToUnicode ? ( length * 2 ) : length;

        if ( ( size + 1 ) == aFilename.Size() )
            {
            if ( aToUnicode )
                {
                //const TPtrC8 ptr = aFilename.Mid(1).Ptr();
                //iFileName.Copy( (TUint16*)ptr, len );
                CopyUnicode( iFileName, aFilename.Mid( 1 ) );
                }
            else
                {
                iFileName.Copy( aFilename.Mid( 1, length ) );
                }

            HTI_LOG_TEXT( "filename:" );
            HTI_LOG_DES( iFileName );
            return ETrue;
            }
        else
            {
            User::LeaveIfError( SendErrorMsg( KErrArgument,
                                              KErrDescrInvalidFilenameLength ) );
            }
        }
    else
        {
        User::LeaveIfError( SendErrorMsg( KErrArgument,
                                          KErrDescrEmptyFilename ) );
        }

    HTI_LOG_FUNC_OUT("CHtiFtpServicePlugin::GetFileNameL");
    return EFalse;
    }

TBool CHtiFtpServicePlugin::GetDirectoryL( const TDesC8& aDirname,
                                           TBool aToUnicode )
    {
    HTI_LOG_FUNC_IN("CHtiFtpServicePlugin::GetDirectoryL");
    if ( aDirname.Length() > 1 )
        {
        TInt len = aDirname[0];
        TInt size = aToUnicode ? ( len * 2 ) : len;
        if ( ( size + 1 ) == aDirname.Size() )
            {
            if ( aToUnicode )
                {
                //const TUint8* ptr = aDirname.Mid(1).Ptr();
                //iFileName.Copy( (TUint16*)ptr, len );
                CopyUnicode( iFileName, aDirname.Mid(1) );
                }
            else
                {
                iFileName.Copy( aDirname.Mid( 1, len ) );
                }

            HTI_LOG_TEXT( "dir:" );
            HTI_LOG_DES( iFileName );
            if ( iFileName.Right( 1 ) != KBackslash )
                {
                iFileName.Append( KBackslash );
                }
            HTI_LOG_FUNC_OUT("CHtiFtpServicePlugin::GetDirectoryL");
            return ETrue;
            }
        else
            {
            User::LeaveIfError( SendErrorMsg( KErrArgument,
                                              KErrDescrInvalidDirnameLength ) );
            }
        }
    else
        {
        User::LeaveIfError( SendErrorMsg( KErrArgument,
                                          KErrDescrEmptyDirname ) );
        }

    HTI_LOG_FUNC_OUT("CHtiFtpServicePlugin::GetDirectoryL");
    return EFalse;
    }

void CHtiFtpServicePlugin::HandleListL( TBool aUnicodText,
                                        TUint aReadingAtt, TBool aSizes )
    {
    HTI_LOG_FUNC_IN("HandleListL");
    CDir* dir;
    TInt err = iFs.GetDir( iFileName, aReadingAtt, ESortNone, dir );
    if ( err != KErrNone )
        {
        User::LeaveIfError( SendErrorMsg( err, KErrDescrFailedGetDir ) );
        return;
        }

    CleanupStack::PushL( dir );
    //build list
    delete iSendBuffer;
    iSendBuffer = NULL;
    TInt bufferLen = dir->Count()*KMaxFileName;
    if ( aUnicodText )
        {
        bufferLen *= 2;
        }
    bufferLen += dir->Count();
    if ( aSizes )
        {
        bufferLen += 4 * dir->Count();
        }

    iSendBuffer = HBufC8::NewL( bufferLen );
    TInt dirNameLen = 0;
    for ( TInt i = 0; i < dir->Count(); ++i)
        {
        dirNameLen = (*dir)[i].iName.Length();
        iSendBuffer->Des().Append( dirNameLen );
        if ( aUnicodText )
            {
            iSendBuffer->Des().Append( (TUint8*)((*dir)[i].iName.Ptr()),
                                       dirNameLen*2 );
            }
        else
            {
            iSendBuffer->Des().Append( (*dir)[i].iName );
            }
        if ( aSizes )
            {
            TInt size = (*dir)[i].iSize;
            iSendBuffer->Des().Append( (TUint8*)(&size), 4 );
            }
        }

    err = iDispatcher->DispatchOutgoingMessage(iSendBuffer,
                        KFtpServiceUid,
                        EFalse,
                        EHtiPriorityControl);

    if (  err != KErrNone )
        {
        //wait for a memory
        iState = EListBusy;
        iDispatcher->AddMemoryObserver( this );
        }
    else
        {
        iSendBuffer = NULL;
        }

    CleanupStack::PopAndDestroy();//dir
    HTI_LOG_FUNC_OUT("HandleListL");
    }

void CHtiFtpServicePlugin::HandleListDetailL( TBool aUnicodText, TUint aReadingAtt)
    {
    HTI_LOG_FUNC_IN("HandleListDetailL");
    CDir* dir;
    TInt err = iFs.GetDir( iFileName, aReadingAtt, ESortNone, dir );
    if ( err != KErrNone )
        {
        User::LeaveIfError( SendErrorMsg( err, KErrDescrFailedGetDir ) );
        return;
        }

    CleanupStack::PushL( dir );
    //build list
    delete iSendBuffer;
    iSendBuffer = NULL;
    TInt bufferLen = dir->Count()*KMaxFileName;
    if ( aUnicodText )
        {
        bufferLen *= 2;
        }
    // 1 bytes for name length, 4 bytes for file size, 
    // 6 bytes for date time, 3 bytes for attributes(hide, readonly and system)
    bufferLen += (1 + 4 + 6 + 3) * dir->Count();

    iSendBuffer = HBufC8::NewL( bufferLen );
    TInt dirNameLen = 0;
    for ( TInt i = 0; i < dir->Count(); ++i)
        {
        dirNameLen = (*dir)[i].iName.Length();
        iSendBuffer->Des().Append( dirNameLen );
        if ( aUnicodText )
            {
            iSendBuffer->Des().Append( (TUint8*)((*dir)[i].iName.Ptr()),
                                       dirNameLen*2 );
            }
        else
            {
            iSendBuffer->Des().Append( (*dir)[i].iName );
            }
        TInt year = (*dir)[i].iModified.DateTime().Year();
        iSendBuffer->Des().Append((TUint8*)(&year), 2 );
        iSendBuffer->Des().Append( (*dir)[i].iModified.DateTime().Month()+1);
        iSendBuffer->Des().Append( (*dir)[i].iModified.DateTime().Day()+1);
        iSendBuffer->Des().Append( (*dir)[i].iModified.DateTime().Hour());
        iSendBuffer->Des().Append( (*dir)[i].iModified.DateTime().Minute());
        iSendBuffer->Des().Append( (*dir)[i].iModified.DateTime().Second());
        iSendBuffer->Des().Append( (*dir)[i].IsHidden());
        iSendBuffer->Des().Append( (*dir)[i].IsReadOnly());
        iSendBuffer->Des().Append( (*dir)[i].IsSystem());
        if((*dir)[i].IsDir() == EFalse)
            {
            TInt size = (*dir)[i].iSize;
            iSendBuffer->Des().Append( (TUint8*)(&size), 4 );
            }
        }

    err = iDispatcher->DispatchOutgoingMessage(iSendBuffer,
                        KFtpServiceUid,
                        EFalse,
                        EHtiPriorityControl);

    if (  err != KErrNone )
        {
        //wait for a memory
        iState = EListBusy;
        iDispatcher->AddMemoryObserver( this );
        }
    else
        {
        iSendBuffer = NULL;
        }

    CleanupStack::PopAndDestroy();//dir
    HTI_LOG_FUNC_OUT("HandleListDetailL");
    }

void CHtiFtpServicePlugin::HandleDataMessageL( const TDesC8& aMessage )
    {
    switch ( iState )
        {
        case EStorWait:
            {
            iState = EStorBusy;
            iCurrentOffset += aMessage.Size();

            TInt anError = iFile.Write( aMessage );
            if ( anError == KErrNone  )
                {
                HTI_LOG_FORMAT("received %d", iCurrentOffset);
                if ( iCurrentOffset == iFileSize )
                    {
                    HTI_LOG_TEXT("receiving is over");
                    //receiving is over
                    TInt err = iFile.Flush();
                    iFile.Close();
                    iState = EIdle;
                    if ( err != KErrNone )
                        {
                        //err
                        HTI_LOG_TEXT("failed to close file");
                        iFs.Delete(iFileName);
                        SendErrorMsg( err, KErrDescrFailedCloseFile );
                        }
                    else
                        {
                        //if file should be copied to TCB
                        //copy it to to TCB from temp location
                        if ( IsFileTcb( iFileName ) )
                            {
                            HandleTcbCopyL( KTmpFileName, iFileName );
                            }
                        else
                            {
                            SetBURFakeState( EFalse );
                            
                            //If the received file is a .rsc file and the destination is apparc's pvt directory
                            //add application registration into Software Component Registry (SCR) to register application
                            TRAPD(error,HandleAppRegFileInfoL(iFileName, ETrue));
                            
                            if(error)
                                {
                                SendErrorMsg( err, KErrDescrFailedRegisterApp );
                                }
                            else
                                {
                                SendControlMsg( EFtpOK, KNullDesC8 );
                                }
                            }
                        }
                    }
                else
                    {
                    iState = EStorWait;
                    }
                }
            else
                {
                HTI_LOG_FORMAT("error writing file %d", anError);
                //abort operation and send err msg

                iFile.Close();
                iFs.Delete(iFileName);
                iState = EIdle;
                SendErrorMsg( anError, KErrDescrFailedWrite );
                }
            }
            break;
        default:
            //do nothing
            User::LeaveIfError( SendErrorMsg( KErrArgument,
                                              KErrDescrInvalidDataMessage ) );
            break;
        }
    }

void CHtiFtpServicePlugin::HandleReceiveFileL()
    {
    HTI_LOG_FUNC_IN("HandleReceiveFileL");

    // create file
    TInt err = KErrNone;

    //if file should be copied to TCB
    //first copy it to temp location
    if ( IsFileTcb( iFileName ) )
        {
        err = iFile.Replace( iFs, KTmpFileName, EFileWrite );
        }
        else
        {
        err = iFile.Replace(
            iFs, iFileName, EFileWrite | EFileShareExclusive );
        }

    if ( err != KErrNone )
        {
        err = iFile.Replace(
            iFs, iFileName, EFileWrite | EFileShareAny );
        }

    if ( err != KErrNone )
        {
        err = iFile.Replace(
            iFs, iFileName, EFileWrite | EFileShareReadersOrWriters );
        }

    if ( err != KErrNone )
        {
        if ( SetBURFakeState( ETrue ) == KErrNone )
            {
            err = iFile.Replace(
                iFs, iFileName, EFileWrite | EFileShareExclusive );
            }
        }

    if ( err != KErrNone )
        {
        HTI_LOG_TEXT("failed create file");
        SendErrorMsg( err, KErrDescrFailedCreateFile );
        return;
        }

    //check that there is enough disk space
    err = iFile.SetSize( iFileSize );
    if ( err != KErrNone )
        {
        HTI_LOG_TEXT("not enough space");
        SendErrorMsg( err, KErrDescrNoSpace );
        iFile.Close();
        iFs.Delete( iFileName );
        return;
        }

    // init receiving byte counter
    iCurrentOffset = 0;
    iState = EStorWait;

    //send ok and
    User::LeaveIfError( SendControlMsg( EFtpOK, KNullDesC8 ) );
    //...wait for data messages
    HTI_LOG_FUNC_OUT("HandleReceiveFileL");
    }

void CHtiFtpServicePlugin::HandleSendFileL()
    {
    //open it
    TInt err = iFile.Open( iFs, iFileName, EFileRead | EFileShareAny );

    if ( err != KErrNone )
        {
        err = iFile.Open( iFs, iFileName, EFileRead | EFileShareReadersOnly );
        }

    if ( err != KErrNone )
        {
        err = iFile.Open( iFs, iFileName, EFileRead | EFileShareReadersOrWriters );
        }

    if ( err != KErrNone )
        {
        if ( SetBURFakeState( ETrue ) == KErrNone )
            {
            err = iFile.Open(
                iFs, iFileName, EFileRead | EFileShareReadersOnly );
            }
        }

    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT("failed open file %d", err);
        SendErrorMsg( err, KErrDescrFailedOpenFile );
        return;
        }

    //send file size
    err = iFile.Size( iFileSize );
    if ( err != KErrNone )
        {
        HTI_LOG_TEXT("failed get filesize");
        User::LeaveIfError( SendErrorMsg( err, KErrDescrFailedOpenFile ) );
        }

    TBuf8<KFileSizeMsgSize> fileSizeMsg;
    fileSizeMsg.Append( (TUint8*)(&iFileSize), 4 );
    User::LeaveIfError( SendControlMsg( EFtpFILESIZE, fileSizeMsg ) );

    iCurrentOffset = 0;
    //start sending
    //always observe when sending
    iDispatcher->AddMemoryObserver( this );

    //create handler for following RFile::Write
    delete iHandlerAO;
    iHandlerAO = NULL;
    iHandlerAO = new(ELeave) CFtpHandlerAO( this );

    //start operation
    ReadToBuffer();
    }

void CHtiFtpServicePlugin::ReadToBuffer()
    {
    HTI_LOG_FUNC_IN("ReadToBuffer");
    //check that iHandlerAO valid
    if ( iHandlerAO )
        {
        if ( !iHandlerAO->IsActive() )
            {
            //dispatch messages in the outgoing queue until run out of memory
            TInt err;
            //allocate memory and read from file
            delete iSendBuffer;
            iSendBuffer = NULL;

            TRAP( err, iSendBuffer = HBufC8::NewL( iBufferSize ) );
            if ( err == KErrNone )
                {
                HTI_LOG_TEXT("read file");
                iState = ERetrBusy;
                iSendBufferDes.Set( iSendBuffer->Des() );
                iFile.Read( iSendBufferDes, iHandlerAO->iStatus  );
                iHandlerAO->Start();
                }
            else
                {
                //go to idle state
                HTI_LOG_TEXT("impossible to alloc mem");
                iDispatcher->RemoveMemoryObserver( this );

                delete iHandlerAO;
                iHandlerAO = NULL;

                iFile.Close();
                iState = EIdle;
                //try to send err message
                SendErrorMsg( KErrNoMemory, KErrDescrNoMemory );
                }
            }
        else
            {
            //error, ignore
            HTI_LOG_TEXT("ERROR: Call ReadBuffer with active handler");
            }
        }
    else
        {
        //error
        HTI_LOG_TEXT("ERROR: Call ReadBuffer with no handler");
        //go to idle
        iState = EIdle;
        }
    HTI_LOG_FUNC_OUT("ReadToBuffer");
    }

void CHtiFtpServicePlugin::SendBuffer()
    {
    HTI_LOG_FUNC_IN("SendBuffer");
    if ( iSendBuffer )
        {
        HTI_LOG_FORMAT("iCurrentOffset %d bytes", iCurrentOffset);
        //send buffer
        TInt err = iDispatcher->DispatchOutgoingMessage( iSendBuffer,
                        KFtpServiceUid,
                        EFalse,
                        EHtiPriorityData);

        if ( err == KErrNone )
            {
            HTI_LOG_TEXT("message was dispatched");
            //message was dispatched
            iSendBuffer = NULL;
            //check do we have to continue
            if ( iCurrentOffset >= iFileSize )
                {
                HTI_LOG_TEXT( "over sending" );

                delete iHandlerAO;
                iHandlerAO = NULL;

                iFile.Close();
                iState = EIdle;

                iDispatcher->RemoveMemoryObserver( this );
                SetBURFakeState( EFalse );
                }
            else
                {
                //read and send next package
                ReadToBuffer();
                }
            }
        else if ( err == KErrNoMemory )
            {
            //wait for memory
            //observer should be active
            iState = ERetrWait;
            }
        else if ( err != KErrNone )
            {
            HTI_LOG_FORMAT("error dispatching outgoing message %d", err );
            //some err, abort operation
            delete iSendBuffer;
            iSendBuffer = NULL;

            delete iHandlerAO;
            iHandlerAO = NULL;

            iFile.Close();

            iState = EIdle;

            iDispatcher->RemoveMemoryObserver( this );
            SetBURFakeState( EFalse );
            }
        }
    else
        {
        //really weird error, go to idle
        HTI_LOG_TEXT("ERROR: SendBuffer with empty iSendBuffer");
        iState = EIdle;
        SetBURFakeState( EFalse );
        }
    HTI_LOG_FUNC_OUT("SendBuffer");
    }

void CHtiFtpServicePlugin::FtpComplete( TInt anError)
    {
    HTI_LOG_FUNC_IN("FtpComplete");
    HTI_LOG_FORMAT("error %d", anError);

    //NOTE: can't leave from here
    switch ( iState )
        {
        case ERmdBusy:
            {
            //send OK message
            if ( anError == KErrNone )
                {
                SendControlMsg( EFtpOK, KNullDesC8 );
                }
            else
                {
                SendErrorMsg( anError, KErrDescrFailedRmDir );
                }
            delete iHandlerAO;
            iHandlerAO = NULL;

            iState = EIdle;
            }
            break;
        case EDeleBusy:
            {
            //send OK message
            if ( anError == KErrNone )
                {
                SetBURFakeState( EFalse );
                SendControlMsg( EFtpOK, KNullDesC8 );
                }
            else
                {
                if ( SetBURFakeState( ETrue ) != KErrNone )
                    {
                    // Force was not requested, not supported OR already tried.
                    SendErrorMsg( anError, KErrDescrFailedDeleFile );
                    }
                else
                    {
                    // try delete again
                    TRAPD( err, HandleDeleteL( iFileName ) );
                    if ( err == KErrNone )
                        {
                        break;
                        }
                    else
                        {
                        SendErrorMsg( err, KErrDescrFailedDeleFile );
                        }
                    }
                }
            delete iHandlerAO;
            iHandlerAO = NULL;

            iState = EIdle;
            }
            break;
        case ERenameBusy:
            {
            //send OK message
            if ( anError == KErrNone )
                {
                SetBURFakeState( EFalse );
                SendControlMsg( EFtpOK, KNullDesC8 );
                }
            else
                {
                SendErrorMsg( anError, KErrDescrFailedRenameFile );
                }

            delete iHandlerAO;
            iHandlerAO = NULL;

            iState = EIdle;
            }
            break;
        case ECopyBusy:
            {
            //send OK message
            if ( anError == KErrNone )
                {
                SetBURFakeState( EFalse );
                SendControlMsg( EFtpOK, KNullDesC8 );
                }
            else
                {
                SendErrorMsg( anError, KErrDescrFailedCopyFile );
                }

            delete iHandlerAO;
            iHandlerAO = NULL;

            iState = EIdle;
            }
            break;
        case EMoveBusy:
            {
            //send OK message
            if ( anError == KErrNone )
                {
                SetBURFakeState( EFalse );
                SendControlMsg( EFtpOK, KNullDesC8 );
                }
            else
                {
                SendErrorMsg( anError, KErrDescrFailedMoveFile );
                }

            delete iHandlerAO;
            iHandlerAO = NULL;

            iState = EIdle;
            }
            break;
        case EMoveTcbBusy:
            {
            //send OK message
            if ( anError == KErrNone )
                {
                SetBURFakeState( EFalse );
                SendControlMsg( EFtpOK, KNullDesC8 );
                }
            else
                {
                SendErrorMsg( anError, KErrDescrFailedMoveFile );
                }

            delete iProcessLogonAO;
            iProcessLogonAO = NULL;

            iState = EIdle;
            }
            break;
        case ERenameTcbBusy:
            {
            //send OK message
            if ( anError == KErrNone )
                {
                SetBURFakeState( EFalse );
                SendControlMsg( EFtpOK, KNullDesC8 );
                }
            else
                {
                SendErrorMsg( anError, KErrDescrFailedRenameFile );
                }

            delete iProcessLogonAO;
            iProcessLogonAO = NULL;

            iState = EIdle;
            }
            break;
        case EDeleTcbBusy:
            {
            //send OK message
            if ( anError == KErrNone )
                {
                SetBURFakeState( EFalse );
                SendControlMsg( EFtpOK, KNullDesC8 );
                }
            else
                {
                if ( SetBURFakeState( ETrue ) != KErrNone )
                    {
                    // Force was not requested, not supported OR already tried.
                    SendErrorMsg( anError, KErrDescrFailedDeleFile );
                    }
                else
                    {
                     // try delete again
                    TRAPD( err, HandleTcbDeleteL( iFileName ) );
                    if ( err == KErrNone )
                        {
                        break;
                        }
                    else
                        {
                        SendErrorMsg( err, KErrDescrFailedDeleFile );
                        }
                    }
                }
            delete iProcessLogonAO;
            iProcessLogonAO = NULL;

            iState = EIdle;
            }
            break;
        case EMkdTcbBusy:
            {
            if ( anError == KErrNone || anError == KErrAlreadyExists )
                {
                SendControlMsg( EFtpOK, KNullDesC8);
                }
            else
                {
                SendErrorMsg( anError, KErrDescrFailedMkDir );
                }
            delete iProcessLogonAO;
            iProcessLogonAO = NULL;

            iState = EIdle;

            }
            break;
        case ERmdTcbBusy:
            {
            if ( anError == KErrNone || anError == KErrAlreadyExists )
                {
                SendControlMsg( EFtpOK, KNullDesC8);
                }
            else
                {
                SendErrorMsg( anError, KErrDescrFailedRmDir );
                }
            delete iProcessLogonAO;
            iProcessLogonAO = NULL;

            iState = EIdle;

            }
            break;

        case EStorBusy:
            {
            if ( anError == KErrNone  )
                {
                HTI_LOG_FORMAT("received %d", iCurrentOffset);
                if ( iCurrentOffset == iFileSize )
                    {
                    HTI_LOG_TEXT("receiveing is over");
                    //receiveing is over
                    delete iHandlerAO;
                    iHandlerAO = NULL;

                    iState = EIdle;

                    TInt err = iFile.Flush();
                    iFile.Close();
                    if ( err != KErrNone )
                        {
                        //err
                        HTI_LOG_TEXT("failed to close file");
                        iFs.Delete(iFileName);
                        SendErrorMsg( err, KErrDescrFailedCloseFile );
                        }
                    else
                        {
                        //if file should be copied to TCB
                        //copy it to to TCB from temp location
                        if ( IsFileTcb( iFileName ) )
                            {
                            TRAP( err, HandleTcbCopyL(
                                KTmpFileName, iFileName ) );
                            if ( err != KErrNone )
                                {
                                SendErrorMsg( anError, KErrDescrFailedCopyTcb );
                                iFs.Delete( KTmpFileName );
                                delete iProcessLogonAO;
                                iProcessLogonAO = NULL;
                                iState = EIdle;
                                }
                            }
                        else
                            {
                            SetBURFakeState( EFalse );
                            SendControlMsg( EFtpOK, KNullDesC8 );
                            }
                        }
                    }
                else
                    {
                    iState = EStorWait;
                    //busy state is over
                    iDispatcher->Notify(KErrNone);
                    }
                }
            else
                {
                HTI_LOG_FORMAT("error writing file %d", anError);
                //abort operation and send err msg
                delete iHandlerAO;
                iHandlerAO = NULL;

                iFile.Close();
                iFs.Delete(iFileName);
                iState = EIdle;
                SendErrorMsg( anError, KErrDescrFailedWrite );
                }
            }
            break;

        case EStorTcbBusy:
            {
            if ( anError == KErrNone )
                {
                SetBURFakeState( EFalse );
                SendControlMsg( EFtpOK, KNullDesC8 );
                }
            else
                {
                if ( SetBURFakeState( ETrue ) != KErrNone )
                    {
                    // Force was not requested, not supported OR already tried.
                    HTI_LOG_FORMAT("error copy to tcb %d", anError);
                    //abort operation and send err msg
                    SendErrorMsg( anError, KErrDescrFailedCopyTcb );
                    }
                else
                    {
                    // try copy again
                    TRAPD( err, HandleTcbCopyL( KTmpFileName, iFileName ) );
                    if ( err == KErrNone )
                        {
                        break;
                        // don't continue, this method will be called again
                        // by the AO after copying is tried
                        }
                    else
                        {
                        SendErrorMsg( err, KErrDescrFailedCopyTcb );
                        }
                    }
                }

            //delete temp file
            HTI_LOG_TEXT("delete tmp file");
            TInt err = iFs.Delete(KTmpFileName);
            if ( err != KErrNone )
                {
                HTI_LOG_FORMAT("error delete tmp file %d", err);
                }

            delete iProcessLogonAO;
            iProcessLogonAO = NULL;
            iState = EIdle;
            }
            break;

        case ERetrBusy:
            {
            if ( anError == KErrNone )
                {
                HTI_LOG_FORMAT("read %d bytes", iSendBuffer->Size());
                iCurrentOffset += iSendBuffer->Size();

                SendBuffer();
                }
            else
                {
                HTI_LOG_FORMAT("failed read file %d", anError);
                //error reading file
                //abort operation and send err msg
                delete iSendBuffer;
                iSendBuffer = NULL;

                delete iHandlerAO;
                iHandlerAO = NULL;

                iFile.Close();

                iState = EIdle;
                iDispatcher->RemoveMemoryObserver( this );

                SendErrorMsg( anError, KErrDescrFailedRead );
                }
            }
            break;
        default:
            {
            //some error, should not be called
            HTI_LOG_TEXT("invalid state for FtpComplete");
            }
        }
    HTI_LOG_FUNC_OUT("FtpComplete");
    }

void CHtiFtpServicePlugin::NotifyMemoryChange( TInt aAvailableMemory )
    {
    HTI_LOG_FUNC_IN("NotifyMemoryChange");
    switch ( iState )
        {
        case ERetrWait:
            {
            if ( iSendBuffer )
                {
                if ( aAvailableMemory >= iSendBuffer->Size() )
                    {
                    //continue sending
                    SendBuffer();
                    }
                }
            else
                {//impossible
                //nothing to send
                //just reset
                HTI_LOG_TEXT("invalid state for mem");

                delete iHandlerAO;
                iHandlerAO = NULL;
                iFile.Close();
                iDispatcher->RemoveMemoryObserver(this);
                iState = EIdle;
                }
            }
            break;
        case EListBusy:
            {
            if ( iSendBuffer )
                {
                if ( aAvailableMemory >= iSendBuffer->Size() )
                    {
                    //send buffer
                    TInt err = iDispatcher->DispatchOutgoingMessage(
                                    iSendBuffer,
                                    KFtpServiceUid,
                                    EFalse,
                                    EHtiPriorityData);
                    if ( err != KErrNone )
                        {
                        //error, reset
                        HTI_LOG_TEXT("failed to send LIST");
                        delete iSendBuffer;
                        }
                    iSendBuffer = NULL;
                    iDispatcher->RemoveMemoryObserver(this);
                    iState = EIdle;
                    }
                }
            else
                {//impossible
                //nothing to send
                //just reset
                HTI_LOG_TEXT("invalid state for mem");
                iDispatcher->RemoveMemoryObserver(this);
                iState = EIdle;
                }
            }
            break;
        default:
            //some error, should not be called
            HTI_LOG_TEXT("invalid state for mem");
            //iDispatcher->RemoveMemoryObserver(this);
            //iState = EIdle;
        }
    HTI_LOG_FUNC_OUT("NotifyMemoryChange");
    }

TBool CHtiFtpServicePlugin::IsFileTcb( const TDesC& aFilename )
    {
    return aFilename.FindF( KHtiTcbSys ) == KPathOffset ||
           aFilename.FindF( KHtiTcbResource ) == KPathOffset;
    }

void CHtiFtpServicePlugin::HandleTcbDeleteL( const TDesC& aFilename )
    {
    HTI_LOG_FUNC_IN("HandleTcbDeleteL");
    HBufC* cmdLine = HBufC::NewLC( aFilename.Length() + 2 );

    //make command line "d|filename"
    cmdLine->Des().Copy( KHtiFileHlpDeleteCmd );
    cmdLine->Des().Append( KHtiFileHlpDelim );
    cmdLine->Des().Append( aFilename );

    HTI_LOG_DES( *cmdLine );

    delete iProcessLogonAO;
    iProcessLogonAO = NULL;

    iProcessLogonAO = new(ELeave) CProcessLogonAO( this );
    iProcessLogonAO->Start( *cmdLine );

    iState = EDeleTcbBusy;

    CleanupStack::PopAndDestroy( cmdLine );
    HTI_LOG_FUNC_OUT("HandleTcbDeleteL");
    }

void CHtiFtpServicePlugin::HandleTcbCopyL( const TDesC& aFromFilename,
                                           const TDesC& aToFilename )
    {
    HTI_LOG_FUNC_IN("HandleTcbCopyL");
    HBufC* cmdLine = HBufC::NewLC( aFromFilename.Length() + aToFilename.Length() + 3 );

    //make command line "c|fromfilename|tofilename"
    cmdLine->Des().Copy( KHtiFileHlpCopyCmd );
    cmdLine->Des().Append( KHtiFileHlpDelim );
    cmdLine->Des().Append( aFromFilename );
    cmdLine->Des().Append( KHtiFileHlpDelim );
    cmdLine->Des().Append( aToFilename );

    HTI_LOG_DES( *cmdLine );

    delete iProcessLogonAO;
    iProcessLogonAO = NULL;

    iProcessLogonAO = new(ELeave) CProcessLogonAO( this );
    iProcessLogonAO->Start( *cmdLine );

    iState = EStorTcbBusy;

    CleanupStack::PopAndDestroy( cmdLine );
    HTI_LOG_FUNC_OUT("HandleTcbCopyL");
    }

void CHtiFtpServicePlugin::HandleTcbMkdL( const TDesC& aDirname )
    {
    HTI_LOG_FUNC_IN("HandleTcbMkdL");
    HBufC* cmdLine = HBufC::NewLC( aDirname.Length() + 2 );

    //make command line "m|dirname"
    cmdLine->Des().Copy( KHtiFileHlpMkdCmd );
    cmdLine->Des().Append( KHtiFileHlpDelim );
    cmdLine->Des().Append( aDirname );

    HTI_LOG_DES( *cmdLine );

    delete iProcessLogonAO;
    iProcessLogonAO = NULL;

    iProcessLogonAO = new(ELeave) CProcessLogonAO( this );
    iProcessLogonAO->Start(*cmdLine);

    iState = EMkdTcbBusy;

    CleanupStack::PopAndDestroy(cmdLine);
    HTI_LOG_FUNC_OUT("HandleTcbMkdL");
    }

void CHtiFtpServicePlugin::HandleTcbRmdL( const TDesC& aDirname )
    {
    HTI_LOG_FUNC_IN("HandleTcbRmdL");
    HBufC* cmdLine = HBufC::NewLC( aDirname.Length() + 2 );

    //make command line "r|dirname"
    cmdLine->Des().Copy( KHtiFileHlpRmdCmd );
    cmdLine->Des().Append( KHtiFileHlpDelim );
    cmdLine->Des().Append( aDirname );

    HTI_LOG_DES( *cmdLine );

    delete iProcessLogonAO;
    iProcessLogonAO = NULL;

    iProcessLogonAO = new ( ELeave ) CProcessLogonAO( this );
    iProcessLogonAO->Start( *cmdLine );

    iState = ERmdTcbBusy;

    CleanupStack::PopAndDestroy( cmdLine );
    HTI_LOG_FUNC_OUT("HandleTcbRmdL");
    }

void CHtiFtpServicePlugin::HandleTcbRenameL(
        const TDesC& aTargetName, const TDesC& aDestName )
    {
    HTI_LOG_FUNC_IN("HandleTcbRenameL");
    HBufC* cmdLine = HBufC::NewLC( aTargetName.Length() + 4 + aDestName.Length());

    //make command line "r|dirname"
    cmdLine->Des().Copy( KHtiFileHlpRenameCmd );
    cmdLine->Des().Append( KHtiFileHlpDelim );
    cmdLine->Des().Append( aTargetName );
    cmdLine->Des().Append( KHtiFileHlpDelim );
    cmdLine->Des().Append( aDestName );

    HTI_LOG_DES( *cmdLine );

    delete iProcessLogonAO;
    iProcessLogonAO = NULL;

    iProcessLogonAO = new ( ELeave ) CProcessLogonAO( this );
    iProcessLogonAO->Start( *cmdLine );

    iState = ERenameTcbBusy;

    CleanupStack::PopAndDestroy( cmdLine );
    HTI_LOG_FUNC_OUT("HandleTcbRenameL");
    }

void CHtiFtpServicePlugin::HandleTcbMoveL(
        const TDesC& aTargetName, const TDesC& aDestName )
    {
    HTI_LOG_FUNC_IN("HandleTcbRenameL");
    HBufC* cmdLine = HBufC::NewLC( aTargetName.Length() + 4 + aDestName.Length());

    //make command line "r|dirname"
    cmdLine->Des().Copy( KHtiFileHlpMoveCmd );
    cmdLine->Des().Append( KHtiFileHlpDelim );
    cmdLine->Des().Append( aTargetName );
    cmdLine->Des().Append( KHtiFileHlpDelim );
    cmdLine->Des().Append( aDestName );

    HTI_LOG_DES( *cmdLine );

    delete iProcessLogonAO;
    iProcessLogonAO = NULL;

    iProcessLogonAO = new ( ELeave ) CProcessLogonAO( this );
    iProcessLogonAO->Start( *cmdLine );

    iState = EMoveTcbBusy;

    CleanupStack::PopAndDestroy( cmdLine );
    HTI_LOG_FUNC_OUT("HandleTcbRenameL");
    }

TInt CHtiFtpServicePlugin::SetBURFakeState( TBool aOn )
    {
    HTI_LOG_FUNC_IN( "CHtiFtpServicePlugin::SetBURFakeStateL" );
    TInt err = KErrNone;

    if ( iBackupFake == NULL )
        {
        // Foreced operations not requested or not supported
        err = KErrNotSupported;
        }

    else if ( aOn )
        {
        HTI_LOG_TEXT( "Calling ActivateBackup()" );
        err = iBackupFake->ActivateBackup();
        }

    else
        {
        HTI_LOG_TEXT( "Calling DeactivateBackup()" );
        err = iBackupFake->DeactivateBackup();
        }

    HTI_LOG_FUNC_OUT( "CHtiFtpServicePlugin::SetBURFakeStateL" );
    return err;
    }

void CHtiFtpServicePlugin::HandleCheckSumCalcL( TAlgorithm aAlgorithm,
                                                const TDesC& aFilename )
    {
    HTI_LOG_FUNC_IN( "CHtiFtpServicePlugin::HandleCheckSumCalcL" );

    RFile file;
    TInt err = file.Open( iFs, aFilename, EFileRead | EFileShareReadersOnly );
    if ( err )
        {
        HTI_LOG_FORMAT( "Error opening file, err: %d", err );
        SendErrorMsg( err, KErrDescrFailedOpenFile );
        return;
        }

    CleanupClosePushL( file );
    switch ( aAlgorithm )
        {
        case EMD5:
            {
            HTI_LOG_TEXT( "Using MD5 algorithm" );
            const TInt KBufSize( 1024 );
            TInt size = 0;
            TInt offset = 0;

            file.Size( size );

            HBufC8* buf = HBufC8::NewMaxLC( KBufSize );
            TPtr8 filePtr( buf->Des() );

            CMD5* md5 = CMD5::NewL();
            CleanupStack::PushL( md5 );

            while ( offset < size - KBufSize )
                {
                User::LeaveIfError( file.Read( filePtr, KBufSize ) );
                md5->Hash( filePtr );
                offset += filePtr.Length();
                }

            file.Read( filePtr, size - offset );
            filePtr.SetLength( size - offset );
            md5->Hash( filePtr );

            filePtr.Set( NULL, 0, 0 );
            HBufC8* result = md5->Hash( filePtr ).AllocL();
            CleanupStack::PushL( result );

            HTI_LOG_TEXT( "Got following MD5:" );
            HTI_LOG_DES( *result );

            User::LeaveIfError(
                iDispatcher->DispatchOutgoingMessage( result,
                                                      KFtpServiceUid,
                                                      EFalse,
                                                      EHtiPriorityControl ) );
            CleanupStack::Pop( result ); // do not delete, ownership transfered
            CleanupStack::PopAndDestroy( 2 ); // md5, buf
            break;
            }

        default:
            {
            HTI_LOG_FORMAT( "Invalid algorithm: %d", aAlgorithm );
            SendErrorMsg( KErrNotFound, KErrDescrInvalidChecksumArgs );
            break;
            }
        }
    CleanupStack::PopAndDestroy(); // file

    HTI_LOG_FUNC_OUT( "CHtiFtpServicePlugin::HandleCheckSumCalcL" );
    }

void CHtiFtpServicePlugin::HandleFormat( const TUint8 aDrive, const TUint8 aMode )
    {
    HTI_LOG_FUNC_IN( "CHtiFtpServicePlugin::HandleFormat" );

    // Convert the format mode: in HTI protocol 0 means full format and 1
    // (or anything else currently) means quick format
    TUint formatMode = 0;
    if ( aMode )
        formatMode = ESpecialFormat | EQuickFormat;
    else
        formatMode = ESpecialFormat | EFullFormat;

    // Create the drive name (drive letter and colon)
    TDriveName drive;
    drive.Append( aDrive );
    drive.Append( _L( ":" ) );

    // Check that HTI is not running from the drive that is to be formatted
    RProcess thisProcess;
    if ( thisProcess.FileName().FindF( drive ) == 0 )
        {
        HTI_LOG_FORMAT( "HTI running from drive %S cannot format", &drive );
        SendErrorMsg( KErrInUse, KErrDescrFailedFormat );
        HTI_LOG_FUNC_OUT( "CHtiFtpServicePlugin::HandleFormat" );
        return;
        }

    // Activate backup/restore to close apps that might have files open.
    // Ignore errors - try to format even if backup fake is not supported.
    SetBURFakeState( ETrue );

    // Start the format
    HTI_LOG_FORMAT( "Starting to format drive %S", &drive );
    HTI_LOG_FORMAT( "Format mode = %d", formatMode );
    TInt remainingTracks = 0;
    RFormat format;
    TInt err = format.Open( iFs, drive, formatMode, remainingTracks );
    if ( err == KErrNone )
        {
        HTI_LOG_FORMAT( "Formatting %d tracks", remainingTracks );
        while ( remainingTracks && err == KErrNone )
            {
            err = format.Next( remainingTracks );
            HTI_LOG_FORMAT( "Tracks remaining: %d", remainingTracks );
            }
        }
    format.Close();

    // Deactivate backup/restore - errors ignored.
    SetBURFakeState( EFalse );

    if ( err == KErrNone )
        {
        SendControlMsg( EFtpOK, KNullDesC8 );
        }

    else
        {
        SendErrorMsg( err, KErrDescrFailedFormat );
        }

    HTI_LOG_FUNC_OUT( "CHtiFtpServicePlugin::HandleFormat" );
    }

void CHtiFtpServicePlugin::HandleListDrivesL( TBool aUnicode )
    {
    HTI_LOG_FUNC_IN( "CHtiFtpServicePlugin::HandleListDrivesL" );
    TInt driveCount = 0;
    TDriveList driveList;

    User::LeaveIfError( iFs.DriveList( driveList, KDriveAttAll ) );

    // Buffer for the drive list that is returned
    CBufFlat* driveListBuf = CBufFlat::NewL( 256 );
    CleanupStack::PushL( driveListBuf );
    TInt bufPos = 0;

    for ( TInt i = 0; i < KMaxDrives; i++ )
        {
        if ( driveList[i] )
            {
            HTI_LOG_FORMAT( "Found drive number %d", i );
            TVolumeInfo volInfo;
            if ( iFs.Volume( volInfo, i ) == KErrNone )
                {
                driveCount++;
                // root path with length byte
                TChar driveLetter;
                iFs.DriveToChar( i, driveLetter );
                HTI_LOG_FORMAT( "Got volume info for drive %c",
                        TUint( driveLetter ) );
                TBuf<3> rootPathBuf;
                rootPathBuf.AppendFormat( KRootPathFormat,
                        TUint( driveLetter ) );
                TBuf8<1> lengthBuf;
                lengthBuf.Append( rootPathBuf.Length() );
                TBuf8<6> rootPathBuf8;
                if ( aUnicode )
                    {
                    rootPathBuf8.Append( ( TUint8* ) rootPathBuf.Ptr(),
                            rootPathBuf.Length() * 2 );
                    }
                else
                    {
                    rootPathBuf8.Copy( rootPathBuf );
                    }
                driveListBuf->ExpandL( bufPos, rootPathBuf8.Length() + 1 );
                driveListBuf->Write( bufPos, lengthBuf, 1 );
                bufPos++;
                driveListBuf->Write( bufPos, rootPathBuf8 );
                bufPos += rootPathBuf8.Length();

                // media type 1 byte
                TBuf8<8> tmpBuf;
                tmpBuf.Append( volInfo.iDrive.iType );
                driveListBuf->ExpandL( bufPos, tmpBuf.Length() );
                driveListBuf->Write( bufPos, tmpBuf );
                bufPos += tmpBuf.Length();

                // UID 4 bytes
                tmpBuf.Copy( ( TUint8* ) ( &volInfo.iUniqueID ), 4 );
                driveListBuf->ExpandL( bufPos, tmpBuf.Length() );
                driveListBuf->Write( bufPos, tmpBuf );
                bufPos += tmpBuf.Length();

                // size 8 bytes
                tmpBuf.Copy( ( TUint8* ) ( &volInfo.iSize ), 8 );
                driveListBuf->ExpandL( bufPos, tmpBuf.Length() );
                driveListBuf->Write( bufPos, tmpBuf );
                bufPos += tmpBuf.Length();

                // free space 8 bytes
                tmpBuf.Copy( ( TUint8* ) ( &volInfo.iFree ), 8 );
                driveListBuf->ExpandL( bufPos, tmpBuf.Length() );
                driveListBuf->Write( bufPos, tmpBuf );
                bufPos += tmpBuf.Length();

                // name with length byte
                HBufC8* driveNameBuf8 = NULL;
                TInt driveNameLength = volInfo.iName.Length();
                lengthBuf.Zero();
                lengthBuf.Append( driveNameLength );
                driveListBuf->ExpandL( bufPos, 1 );
                driveListBuf->Write( bufPos, lengthBuf, 1 );
                bufPos++;
                if ( driveNameLength > 0 )
                    {
                    if ( aUnicode )
                        {
                        driveNameBuf8 = HBufC8::NewL( driveNameLength * 2 );
                        driveNameBuf8->Des().Append(
                                ( TUint8* ) volInfo.iName.Ptr(),
                                driveNameLength * 2 );
                        }
                    else
                        {
                        driveNameBuf8 = HBufC8::NewL( driveNameLength );
                        driveNameBuf8->Des().Append( volInfo.iName );
                        }
                    HTI_LOG_FORMAT( "Drive name: %S", &volInfo.iName );
                    driveListBuf->ExpandL( bufPos, driveNameBuf8->Length() );
                    driveListBuf->Write( bufPos, driveNameBuf8->Ptr(),
                        driveNameBuf8->Length() );
                    bufPos += driveNameBuf8->Length();
                    delete driveNameBuf8;
                    driveNameBuf8 = NULL;
                    }
                }
            }
        }

    // All drives added - write number of drives to the beginning of buffer
    TBuf8<1> countBuf;
    countBuf.Append( ( TUint8* ) ( &driveCount ), 1 );
    driveListBuf->InsertL( 0, countBuf, 1 );

    // Make a copy of the buffer to iSendBuffer
    delete iSendBuffer;
    iSendBuffer = NULL;
    iSendBuffer = driveListBuf->Ptr( 0 ).AllocL();
    CleanupStack::PopAndDestroy(); // driveListBuf

    // ...and send it away
    TInt err = iDispatcher->DispatchOutgoingMessage(
        iSendBuffer, KFtpServiceUid, EFalse, EHtiPriorityControl );

    if (  err != KErrNone )
        {
        //wait for memory
        iState = EListBusy;
        iDispatcher->AddMemoryObserver( this );
        }
    else
        {
        iSendBuffer = NULL; // ownership transferred
        }

    HTI_LOG_FUNC_OUT( "CHtiFtpServicePlugin::HandleListDrivesL" );
    }


void CHtiFtpServicePlugin::HandleAppRegFileInfoL(const TDesC& aAppRegFile, TBool aAdded)
    {
    HTI_LOG_FUNC_IN( "CHtiFtpServicePlugin::HandleAppRegFileInfoL" );
    
    //check if the file is a reg resource file, if not, return.
    _LIT(KApparcRegDir, "\\private\\10003a3f\\import\\apps\\");
    _LIT(KApparcRegExt, ".rsc");
    TParsePtrC filename(aAppRegFile);
    if(filename.Ext().Compare(KApparcRegExt) != 0 ||
            filename.Path().Left(KApparcRegDir().Length()).CompareF(KApparcRegDir) != 0)
        {
        HTI_LOG_TEXT("Not a registration resource file");
        return;
        }

    HBufC* cmdLine = NULL;
    if(aAdded)
        {
        cmdLine = HBufC::NewLC( KCmdAppRegInfo().Length() + KCmdAppRegInfo_Add().Length()
                + aAppRegFile.Length() + 2 );
        //make command line "AppRegInfo|add|aAppRegFile"
        cmdLine->Des().Copy( KCmdAppRegInfo );
        cmdLine->Des().Append( KDelimiter );
        cmdLine->Des().Append( KCmdAppRegInfo_Add() );
        cmdLine->Des().Append( KDelimiter );
        cmdLine->Des().Append( aAppRegFile );
        }
    else
        {
        cmdLine = HBufC::NewLC( KCmdAppRegInfo().Length() + KCmdAppRegInfo_Remove().Length()
                + aAppRegFile.Length() + 2 );
        //make command line "AppRegInfo|remove|aAppRegFile"
        cmdLine->Des().Copy( KCmdAppRegInfo );
        cmdLine->Des().Append( KDelimiter );
        cmdLine->Des().Append( KCmdAppRegInfo_Remove() );
        cmdLine->Des().Append( KDelimiter );
        cmdLine->Des().Append( aAppRegFile );
        }

    HTI_LOG_DES( *cmdLine );
    
    RProcess process;
    TInt error = process.Create(KHtiTcbHlp, *cmdLine);
    if(error == KErrNotFound)
        {
        CleanupStack::PopAndDestroy( cmdLine );
        HTI_LOG_TEXT("return without registration");
        return;
        }
    
    User::LeaveIfError(error);
    
    TRequestStatus status;
    process.Logon(status);
    //start HtiTcbHlp.exe
    process.Resume();
    User::WaitForRequest(status);
    process.Close();
    CleanupStack::PopAndDestroy( cmdLine );
    
    User::LeaveIfError(status.Int());
    
    HTI_LOG_FUNC_OUT( "CHtiFtpServicePlugin::HandleAppRegFileInfoL" );
    }
// End of File
