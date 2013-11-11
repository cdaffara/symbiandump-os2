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
* Description:  This file contains the implementations of the CHTIDispatcher
*                class.
*
*/


#include "HtiDispatcher.h"
#include "HtiMessage.h"
#include "HtiMessageQueue.h"
#include "HtiLogging.h"
#include "HtiCommAdapter.h"
#include "HtiServicePluginInterface.h"
#include "HtiCommPluginInterface.h"
#include "HtiSecurityManager.h"
#include "HtiVersion.h"

#include <e32debug.h>

//active objects priorities
const static TInt KHtiIdlePriority = 2;

const static TInt KDefaultMaxQueueSize = 4 * 1024 * 1024; // 4 MB

//HTI system service commands
enum THtiCommand
    {
    EHtiAuthentication = 0x01,
    EHtiVersion        = 0x02,
    EHtiServiceList    = 0x03,
    EHtiStop           = 0x04,
    EHtiReboot         = 0x05,
    EHtiFormat         = 0x06,
    EHtiReset          = 0x07,
    EHtiShowConsole    = 0x08,
    EHtiHideConsole    = 0x09,
    EHtiInstanceId     = 0x0A,
    EHtiDebugPrint     = 0x0B,
    EHtiRestartServices= 0x0C,
    EHtiError          = 0xFF
    };

//HTI error messages
const static TInt KMaxErrMessageLength = KErrorDescriptionMaxLength + 10;

//error descriptions
_LIT8( KHtiSystemCmdErrDescr, "Unknown HTI command" );
_LIT8( KErrDescrDispatchOut, "Failed to dispatch message" );
_LIT8( KErrDescrWrap, "Failed to wrap message" );
_LIT8( KErrDescrDispatchOutError, "Failed to dispatch error message" );
_LIT8( KErrDescrInvalidParameter, "Invalid command parameter" );
_LIT8( KErrDescrNotInRom, "Command supported only if HTI is running from ROM" );

const static TChar KSpChar = ' ';
const static TChar KNewLineChar = '\n';
const static TInt KHtiServiceNameLength = 124;
const static TInt KHtiServiceListRecordLength = KHtiServiceNameLength + 4;

_LIT( KHtiWatchDogMatchPattern, "HtiWatchDog*" );
_LIT( KHtiDeviceRebootExeOS,    "HtiDeviceRebootOS.exe" );
_LIT( KHtiDeviceRebootExeUI,    "HtiDeviceRebootUI.exe" );
_LIT( KHtiRestartExeName,       "HtiRestart.exe");

_LIT( KParamNormalRfs, "rfsnormal" );
_LIT( KParamDeepRfs,   "rfsdeep" );

TDesC8* THtiSystemProtocolHelper::ErrorMessageL( TInt aHtiErrorCode,
                                  const TUid aTargetServiceUid )
    {
    HBufC8* msg = HBufC8::NewL( KMaxErrMessageLength );
    msg->Des().Append( EHtiError ); //one byte
    msg->Des().Append( aHtiErrorCode ); //one byte
    msg->Des().AppendFill( 0x00, 4 ); //missed service error code
    //append uid as TInt32 by copying 4 bytes through pointer
    msg->Des().Append( ( TUint8* )( &( aTargetServiceUid.iUid ) ), 4 );
    return msg;
    }

TDesC8* THtiSystemProtocolHelper::ErrorMessageL( TInt aHtiErrorCode,
                                  const TUid aTargetServiceUid,
                                  TInt aErrorCode,
                                  const TDesC8& aErrorDescription )
    {
    HBufC8* msg = HBufC8::NewL( KMaxErrMessageLength );
    msg->Des().Append( EHtiError ); //one byte
    msg->Des().Append( aHtiErrorCode ); //one byte
    msg->Des().Append( ( TUint8* ) ( &aErrorCode ), 4 ); //4 bytes
    //append uid as TInt32 by copying 4 bytes through pointer
    msg->Des().Append( ( TUint8* )( &( aTargetServiceUid.iUid ) ), 4 );
    msg->Des().Append( aErrorDescription );
    return msg;
    }

TDesC8* THtiSystemProtocolHelper::AuthMessageL( const TDesC8& aToken )
    {
    HBufC8* msg = HBufC8::NewL( aToken.Length() + 1 );
    msg->Des().Append( EHtiAuthentication );
    msg->Des().Append( aToken );
    return msg;
    }

/*************************************************************************
*   CHtiDispatcher implementation
*
**************************************************************************/
CHtiDispatcher::CHtiDispatcher( TInt aMaxQueueMemorySize,
        TInt aReconnectDelay, TBool aShowErrorDialogs ):
    iIdleActive( EFalse ),
    iMaxQueueMemorySize( aMaxQueueMemorySize > 0 ?
                         aMaxQueueMemorySize : KDefaultMaxQueueSize ),
    iToReboot( EFalse ),
    iRfsMode( ERfsUnknown ),
    iConsole( NULL ),
    iIdleOverCommAdapter( EFalse ),
    iHtiInstanceId( 0 ),
    iShowErrorDialogs( aShowErrorDialogs ),
    iReconnectDelay(aReconnectDelay),
    iRebootReason(-1)
    {
    HTI_LOG_FORMAT( "MaxQueueMemorySize %d", iMaxQueueMemorySize );
    iQueueSizeLowThresold = ( iMaxQueueMemorySize / 2 ) / 2;
    iQueueSizeHighThresold = ( iMaxQueueMemorySize / 2 ) * 4 / 5;
    HTI_LOG_FORMAT( "QueueSizeThresholds low : %d", iQueueSizeLowThresold );
    HTI_LOG_FORMAT( "QueueSizeThresholds high: %d", iQueueSizeHighThresold );
    }

void CHtiDispatcher::ConstructL( const TDesC8& aCommPlugin,
                                 TInt aMaxMsgSize,
                                 TBool aShowConsole )
    {
    HTI_LOG_FUNC_IN( "CHTIDispatcher::ConstructL()" );
#ifdef __ENABLE_LOGGING__
    DebugListPlugins();
#endif

    if ( aShowConsole )
        {
        // Create the HTI console
        iConsole = Console::NewL( _L( "HtiFramework" ),
                                  TSize( KConsFullScreen, KConsFullScreen ) );
        iConsole->Printf( _L( "HTI Framework\n" ) );
        iConsole->Printf( _L( "=============\n\n" ) );
        iConsole->Printf( _L( "Version %u.%u\n" ),
                KHtiVersionMajor, KHtiVersionMinor );
        iConsole->Printf( _L( "Starting up...\n" ) );
        }

    //create queues
    iIncomingQueue = CHtiMessageQueue::NewL();
    iOutgoingQueue = CHtiMessageQueue::NewL();

    //security manager init
    iSecurityManager = CHtiSecurityManager::NewL();

    //plugins array
    iLoadedServices = new (ELeave)
                        RArray<TServiceItem>( KServiceArrayGranularity );

    iMemoryObservers = new (ELeave)
                        RPointerArray<MHtiMemoryObserver>( KServiceArrayGranularity );

    //try to load default plugin if specified can't be found
    if ( aCommPlugin.Length() == 0 )
        {
        HTI_LOG_TEXT( "load default comm plugin" );
        iCommPlugin = CHTICommPluginInterface::NewL();
        }
    else
        {
        HTI_LOG_TEXT( "load Comm plugin" );
        HTI_LOG_DES( aCommPlugin );
        iCommPlugin = CHTICommPluginInterface::NewL( aCommPlugin );
        }

    if ( iConsole )
        {
        iConsole->Printf( _L( "Communication plugin loaded.\n" ) );
        }

    iListener = CHtiCommAdapter::NewL( iCommPlugin, this, aMaxMsgSize );
    iSender = CHtiCommAdapter::NewL( iCommPlugin, this, aMaxMsgSize );

    //!!!!!!!!!!!!! -=IMPORTANT=- !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // idle should be created (added to AS) AFTER comm adapters
    //!!!!!!!!!!!!! -=IMPORTANT=- !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //create CIdle object
    iIdle = CIdle::NewL( KHtiIdlePriority );

    // start listening for incoming messages
    Reset();

    HTI_LOG_FUNC_OUT( "CHTIDispatcher::ConstructL()" );
    }

CHtiDispatcher::~CHtiDispatcher()
    {
    HTI_LOG_FUNC_IN( "CHTIDispatcher::~CHTIDispatcher" );

    if ( iIdle )
    {
        iIdle->Cancel();
        delete iIdle;
    }

    UnloadAllServices();

    if ( iLoadedServices )
        {
        iLoadedServices->Close();
        delete iLoadedServices;
        }

    if ( iMemoryObservers )
    {
        iMemoryObservers->Close();
        delete iMemoryObservers;
    }

    delete iIncomingQueue;
    delete iOutgoingQueue;

    delete iListener;
    delete iSender;

    delete iCommPlugin;

    REComSession::FinalClose();

    delete iSecurityManager;

    if ( iRfsMode == ERfsNormal || iRfsMode == ERfsDeep )
        {
        HTI_LOG_FORMAT( "Activating Restore Factory Settings %d", iRfsMode );
        RestoreFactorySettings();
        }

    if ( iToReboot )
        {
        HTI_LOG_TEXT( "Reboot now" );
        Reboot();
        }

    delete iConsole;

    HTI_LOG_FUNC_OUT( "CHTIDispatcher::~CHTIDispatcher" );
    }


CHtiDispatcher* CHtiDispatcher::NewLC( const TDesC8& aCommPlugin,
                                TInt aMaxMsgSize,
                                TInt aMaxQueueMemory,
                                TInt aReconnectDelay,
                                TBool aShowConsole,
                                TBool aShowErrorDialogs )
    {
    CHtiDispatcher* obj = new (ELeave) CHtiDispatcher(
            aMaxQueueMemory, aReconnectDelay,aShowErrorDialogs );
    CleanupStack::PushL( obj );
    obj->ConstructL( aCommPlugin, aMaxMsgSize, aShowConsole );
    return obj;
    }

CHtiDispatcher* CHtiDispatcher::NewL( const TDesC8& aCommPlugin,
                                TInt aMaxMsgSize,
                                TInt aMaxQueueMemory,
                                TInt aReconnectDelay,
                                TBool aShowConsole,
                                TBool aShowErrorDialogs )
    {
    CHtiDispatcher* obj = NewLC( aCommPlugin, aMaxMsgSize, aMaxQueueMemory,
            aReconnectDelay,aShowConsole, aShowErrorDialogs );
    CleanupStack::Pop();
    return obj;
    }

CConsoleBase* CHtiDispatcher::GetConsole()
    {
    return iConsole;
    }

TBool CHtiDispatcher::GetShowErrorDialogs()
    {
    return iShowErrorDialogs;
    }

void CHtiDispatcher::CheckPriorities()
    {
    // If incoming queue reaches some high limit then lower its priority
    // below idle object priority.
    // Make opposite when incoming queue size reaches some low limit
    if ( iIncomingQueue->QueueSize() > iQueueSizeHighThresold )
        {
        if ( !( iListener->IsActive() || iIdleOverCommAdapter ) )
            {
            HTI_LOG_TEXT( "Set listener priority low" );
            iListener->Deque();
            CActiveScheduler::Add( iListener );
            iIdleOverCommAdapter = ETrue;
            }
        }
    }

void CHtiDispatcher::DispatchIncomingMessage( CHtiMessage* aMessage )
    {
    HTI_LOG_FUNC_IN( "DispatchIncomingMessage" );

    iIncomingQueue->Add( *aMessage );

    //start CIdle if needed
    Start();
    CheckPriorities();

    HTI_LOG_FUNC_OUT( "DispatchIncomingMessage" );
    }

TInt CHtiDispatcher::DispatchOutgoingMessage( TDesC8* aMessage,
                    const TUid aTargetServiceUid,
                    TBool aWrappedFlag,
                    THtiMessagePriority aPriority )
    {
    HTI_LOG_FUNC_IN( "DispatchOutgoingMessage" );
    HTI_LOG_TEXT( "Construct HTI message" );

    //send only if enough memory
    TInt returnErr = KErrNone;
    if ( aMessage->Size() <= GetFreeMemory() )
        {
        //call here wrapping
        CHtiMessage* msg = NULL;
        if ( aWrappedFlag )
            {
            TDesC8* wrapped = NULL;
            TRAP( returnErr, wrapped = iSecurityManager->WrapL( *aMessage ) );
            if ( returnErr == KErrNone )
                {
                TRAP( returnErr,
                      msg = CHtiMessage::NewL( wrapped, aTargetServiceUid,
                            aWrappedFlag, aPriority ) );
                if ( returnErr != KErrNone )
                    {
                    UrgentReboot( returnErr, KErrDescrDispatchOut );
                    }
                //wrapped message is kept, original is deleted
                delete aMessage;
                }
            else
                {
                UrgentReboot( returnErr, KErrDescrWrap );
                }
            }
        else
            {
            TRAP( returnErr, msg = CHtiMessage::NewL( aMessage,
                                aTargetServiceUid, aWrappedFlag, aPriority ) );

            if ( returnErr != KErrNone )
                {
                UrgentReboot( returnErr, KErrDescrDispatchOut );
                }
            }

        // put in a queue
        if ( msg )
            iOutgoingQueue->Add( *msg );

        //start CIdle if needed
        Start();
        }
    else
        {
        returnErr = KErrNoMemory;
        }

    HTI_LOG_FUNC_OUT( "DispatchOutgoingMessage" );
    return returnErr;
    }

TInt CHtiDispatcher::DispatchOutgoingErrorMessage( TInt aErrorCode,
                    const TDesC8& aErrorDescription,
                    const TUid aTargetServiceUid )
    {
    HTI_LOG_FUNC_IN( "DispatchError" );

    if ( aTargetServiceUid == TUid::Null() )
        {
        return KErrArgument;
        }

    HTI_LOG_FORMAT( "ErrorCode %d", aErrorCode );
    TInt err = KErrNone;
    TDesC8* msg = NULL;
    TRAP( err, msg = THtiSystemProtocolHelper::ErrorMessageL(
                        EHtiErrServiceError,
                        aTargetServiceUid,
                        aErrorCode,
                        aErrorDescription ) );

    if ( err != KErrNone )
        {
        UrgentReboot( err, KErrDescrDispatchOutError );
        }

    err = DispatchOutgoingMessage( msg, KHtiSystemServiceUid,
                             EFalse, EHtiPriorityDefault );

    if ( err == KErrNoMemory )
        {
        delete msg;
        }

    HTI_LOG_FUNC_OUT( "DispatchError" );
    return err;
    }

TInt CHtiDispatcher::DispatchOutgoingErrorMessage( THtiError aHtiErroreCode,
                                    TInt aLeaveCode,
                                    const TUid aTargetServiceUid )
    {
    HTI_LOG_FORMAT( "leaveCode %d", aLeaveCode );
    TInt err = KErrNone;
    TDesC8* msg = NULL;

    TRAP( err, msg = THtiSystemProtocolHelper::ErrorMessageL(
                        aHtiErroreCode,
                        aTargetServiceUid,
                        aLeaveCode,
                        KNullDesC8 ) );

    if ( err != KErrNone )
        {
        UrgentReboot( err, KErrDescrDispatchOutError );
        }

    err = DispatchOutgoingMessage( msg, KHtiSystemServiceUid,
                             EFalse, EHtiPriorityDefault );

    if ( err == KErrNoMemory )
        {
        delete msg;
        }

    return err;
    }

TInt CHtiDispatcher::DispatchOutgoingErrorMessage( THtiError aHtiErrorCode,
                                    const TUid aTargetServiceUid )
    {
    HTI_LOG_FORMAT( "HtiErrorCode %d", aHtiErrorCode );
    TInt err = KErrNone;
    TDesC8* msg = NULL;

    TRAP( err, msg = THtiSystemProtocolHelper::ErrorMessageL( aHtiErrorCode,
                                                aTargetServiceUid ) );

    if ( err != KErrNone )
        {
        UrgentReboot( err, KErrDescrDispatchOutError );
        }

    err = DispatchOutgoingMessage( msg, KHtiSystemServiceUid,
                             EFalse, EHtiPriorityDefault );

    if ( err == KErrNoMemory )
        {
        delete msg;
        }

    return err;
    }

TInt CHtiDispatcher::DispatchOutgoingMessage(TDesC8* aMessage,
                    const TUid aTargetServiceUid)
    {
    return DispatchOutgoingMessage( aMessage, aTargetServiceUid,
                             EFalse, EHtiPriorityDefault );
    }

void CHtiDispatcher::AddMemoryObserver( MHtiMemoryObserver* anObserver )
    {
    if ( iMemoryObservers->FindInAddressOrder( anObserver ) == KErrNotFound )
        {
        iMemoryObservers->InsertInAddressOrder( anObserver );
        }
    }

void CHtiDispatcher::RemoveMemoryObserver( MHtiMemoryObserver* anObserver )
    {
    TInt removeIndex = iMemoryObservers->FindInAddressOrder( anObserver );
    if ( removeIndex != KErrNotFound )
        {
        iMemoryObservers->Remove( removeIndex );
        }
    }

void CHtiDispatcher::DoMemoryNotification()
    {
    if ( iMemoryObservers->Count() > 0 )
        {
        TInt memory = GetFreeMemory();
        for ( TInt i = 0; i < iMemoryObservers->Count(); ++i )
            {
            ( *iMemoryObservers )[i]->NotifyMemoryChange( memory );
            }
        }
    }

TInt CHtiDispatcher::GetFreeMemory()
    {
    return iMaxQueueMemorySize
           - iIncomingQueue->QueueSize()
           - iOutgoingQueue->QueueSize();
    }

void CHtiDispatcher::Start()
    {
    if ( !( iIdle->IsActive() || iIdleActive ) )
        {
        HTI_LOG_TEXT( "Start CIdle" );
        iIdleActive = ETrue;
        iIdle->Start( TCallBack( DispatchCallback, this ) );
        }
    }

void CHtiDispatcher::Notify( TInt anError )
    {
    HTI_LOG_FORMAT( "CHtiDispatcher::Notify: %d", anError );
    anError = anError;
    //start CIdle to check for messages
    Start();
    }

TInt CHtiDispatcher::DoDispatch()
    {
    HTI_LOG_FUNC_IN( "DoDispatch" );

    TBool isFailed = ETrue;

    //dispatch
    CHtiMessage* msg = NULL;

    //Process message from the queues
    if ( !iSender->IsActive() )
        {
        msg = iOutgoingQueue->Remove();
        if ( msg )
            {
            //process outgoing
            iSender->SendMessage( msg );
            isFailed = EFalse;
            }

        //after some messages removed do memory notification
        DoMemoryNotification();
        }

    iIncomingQueue->StartServiceIteration();
    TBool msgProcessed = EFalse;
    while ( ( msg = iIncomingQueue->GetNext() ) != NULL && !msgProcessed )
        {
        //processing of incoming HTI message
        HTI_LOG_TEXT( "incoming msg" );

        //1. find service
        HTI_LOG_TEXT( "service uid" );
        TUid cmd = msg->DestinationServiceUid();
        THtiMessagePriority msgPriority = ( msg->Priority() && KPriorityMask ) ?
                                            EHtiPriorityControl:
                                            EHtiPriorityData;
        HTI_LOG_FORMAT( "UID: %d", cmd.iUid );

        //check if it's a system message
        if ( cmd == KHtiSystemServiceUid )
            {
            TPtrC8 body = msg->Body();
            TRAPD( err, HandleSystemMessageL( body ) );
            if ( err != KErrNone )
                {
                //do nothing
                HTI_LOG_FORMAT( "Failed handle HTI service, err %d", err );
                }
            msgProcessed = ETrue;
            }
        else
            {
            if ( iSecurityManager->IsContextEstablashed() )
                {
                CHTIServicePluginInterface* service = GetService( cmd );
                if ( service )
                    {
                    //2. call service
                    if ( !service->IsBusy() )
                        {
                        TInt err;
                        if ( msg->IsWrapped() )
                            {
                            TDesC8* unwrapped = NULL;
                            TRAP( err,
                                  unwrapped = iSecurityManager->UnwrapL(
                                                msg->Body() ) );
                            if ( err == KErrNone && unwrapped )
                                {
                                TRAP( err, service->ProcessMessageL(
                                                *unwrapped,
                                                msgPriority ) );

                                delete unwrapped;
                                }
                            else
                                {
                                HTI_LOG_FORMAT( "ERROR: Unwrap %d", err );
                                DispatchOutgoingErrorMessage( EHtiErrUnwrap,
                                    err, cmd );
                                err = KErrNone;
                                }
                            }
                        else
                            {
                            TPtrC8 body = msg->Body();
                            TRAP( err, service->ProcessMessageL( body,
                                                                msgPriority ) );
                            }

                        if ( err != KErrNone )
                            {
                            HTI_LOG_FORMAT( "ERROR: Service Error %d", err );
                            DispatchOutgoingErrorMessage( EHtiErrServiceError,
                                                err, cmd );
                            }

                        msgProcessed = ETrue;
                        }
                    else
                        {
                        HTI_LOG_TEXT( "service is busy" );
                        }
                    }
                else
                    {
                    //send error message ServiceNotFound
                    HTI_LOG_TEXT( "ERROR: service not found" );
                    DispatchOutgoingErrorMessage( EHtiErrServiceNotFound, cmd );
                    msgProcessed = ETrue;
                    }
                }
            else
                {
                //not authorized acces
                HTI_LOG_TEXT( "ERROR: not authorized acces" );
                DispatchOutgoingErrorMessage( EHtiErrNotAuthorized, cmd );
                msgProcessed = ETrue;
                }
            }
        if ( msgProcessed )
            {
            //remove msg from dispatcher
            if ( iIncomingQueue->Remove( msg ) )
                {
                delete msg;
                }

            isFailed = EFalse;
            }
        }

    HTI_LOG_FORMAT( "IQ:%d", iIncomingQueue->QueueSize() );
    HTI_LOG_FORMAT( "OQ:%d", iOutgoingQueue->QueueSize() );

    HTI_LOG_FREE_MEM();
    HTI_LOG_ALLOC_HEAP_MEM();

    //if queues are empty & listener stopped - stop all
    if ( iIncomingQueue->IsEmpty() && iOutgoingQueue->IsEmpty() &&
        !iListener->IsActive() )
        {
        CActiveScheduler::Stop();
        }

    if ( iIncomingQueue->QueueSize() < iQueueSizeLowThresold && iIdleOverCommAdapter )
        {
        HTI_LOG_TEXT( "set listener priority high" );
        iIdle->Deque();
        CActiveScheduler::Add( iIdle );
        iIdleOverCommAdapter = EFalse;
        }

    if ( isFailed )
        {
        HTI_LOG_TEXT( "dispatch failed, stop CIdle" );
        //stop iIdle if there are long outgoing requests
        iIdleActive = EFalse;
        }
    else
        {
        iIdleActive = !iIncomingQueue->IsEmpty() || !iOutgoingQueue->IsEmpty();
        }

    HTI_LOG_FUNC_OUT( "DoDispatch" );
    return iIdleActive;
    }

CHTIServicePluginInterface* CHtiDispatcher::GetService(
    const TUid aServiceUid )
    {
    CHTIServicePluginInterface* result = NULL;
    for ( TInt i = 0; i < iLoadedServices->Count(); ++i )
        {
        if ( aServiceUid == (*iLoadedServices)[i].iServiceUid )
            return (*iLoadedServices)[i].iService;
        }

    HTI_LOG_FORMAT( "Load service: %d", aServiceUid.iUid );

    TRAPD( err, result = CHTIServicePluginInterface::NewL(
        MapServicePluginUid( aServiceUid ) ) );
    if ( err == KErrNone )
        {
        //set dispatcher
        result->SetDispatcher( this );
        //call InitL()
        TRAP( err, result->InitL() );
        if ( err != KErrNone )
            {
            HTI_LOG_TEXT( "Failed at InitL()" );
            delete result;
            result = NULL;
            }
        else
            {
            //add service to the array
            TServiceItem serviceItem;
            serviceItem.iServiceUid = aServiceUid;
            serviceItem.iService = result;

            if ( iLoadedServices->Append( serviceItem ) != KErrNone )
                {
                HTI_LOG_TEXT( "Failed to load service" );
                delete result;
                result = NULL;
                }
            }
        }
    else
        {
        result = NULL;
        HTI_LOG_FORMAT( "Failed to load service %d", err );
        }

    return result;
    }

void CHtiDispatcher::UnloadAllServices()
    {
    if ( iLoadedServices )
        {
        for ( TInt i=0; i < iLoadedServices->Count(); ++i )
            {
            delete (*iLoadedServices)[i].iService;
            }
        iLoadedServices->Reset();
        }
    if ( iMemoryObservers )
        {
        iMemoryObservers->Reset();
        }
    }

TInt CHtiDispatcher::DispatchCallback( TAny* aObj )
    {
    return ( reinterpret_cast<CHtiDispatcher*>( aObj ) )->DoDispatch();
    }

void CHtiDispatcher::Reset()
    {
    UnloadAllServices();
    iIncomingQueue->RemoveAll();
    iOutgoingQueue->RemoveAll();
    iSecurityManager->ResetSecurityContext();

    iListener->Reset();
    iSender->Reset();

    iListener->ReceiveMessage();
    }

void CHtiDispatcher::HandleSystemMessageL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "HandleSystemMessage" );

    if ( aMessage.Length() > 0 )
        {
        HTI_LOG_FORMAT( "cmd %d", aMessage[0] );
        if ( aMessage[0] == EHtiAuthentication )
            {
            //pass token to security manager
            TPtrC8 token = aMessage.Mid( 1 );//token start at the second byte

            TDesC8* replyToken = iSecurityManager->SetSecurityContext( token );
            CleanupStack::PushL( replyToken );
            //prepare reply message
            TDesC8* reply = THtiSystemProtocolHelper::AuthMessageL(
                                *replyToken );

            CleanupStack::PushL( reply );

            User::LeaveIfError( DispatchOutgoingMessage(
                                    reply,
                                    KHtiSystemServiceUid ) );

            CleanupStack::Pop(); //reply
            CleanupStack::PopAndDestroy(); //replyToken
            }
        else if ( iSecurityManager->IsContextEstablashed() )
            {
            switch ( aMessage[0] )
                {
                case EHtiServiceList:
                    {
                    TDesC8* list = ServicePluginsListL();
                    CleanupStack::PushL( list );
                    User::LeaveIfError( DispatchOutgoingMessage( list,
                                        KHtiSystemServiceUid ) );
                    CleanupStack::Pop();
                    }
                    break;

                case EHtiVersion:
                    {
                    HBufC8* msg =  HBufC8::NewLC( 2 );
                    msg->Des().Append( KHtiVersionMajor );
                    msg->Des().Append( KHtiVersionMinor );
                    User::LeaveIfError(
                        DispatchOutgoingMessage( msg, KHtiSystemServiceUid ) );
                    CleanupStack::Pop(); // msg
                    }
                    break;

                case EHtiInstanceId:
                    {
                    if ( iHtiInstanceId == 0 )
                        {
                        CreateInstanceId();
                        }
                    HBufC8* msg = HBufC8::NewLC( sizeof( TUint32 ) );
                    msg->Des().Append( ( TUint8* ) ( &iHtiInstanceId ), sizeof( TUint32 ) );
                    User::LeaveIfError(
                        DispatchOutgoingMessage( msg, KHtiSystemServiceUid ) );
                    CleanupStack::Pop(); // msg
                    if ( iConsole )
                        {
                        iConsole->Printf( _L( "Instance ID = %u\n" ), iHtiInstanceId );
                        }
                    }
                    break;

                case EHtiReboot:
                    {
                    if(aMessage.Length() == 2)
                        {
                        iRebootReason = aMessage[1];
                        }
                    else if(aMessage.Length() == 1)
                        {
                        iRebootReason = -1;
                        }
                    else
                        {
                        User::LeaveIfError(DispatchOutgoingErrorMessage( KErrArgument,
                                      KErrDescrInvalidParameter,
                                      KHtiSystemServiceUid ) );
                        break;
                        }
                    ShutdownAndRebootDeviceL();
                    }
                    break;

                case EHtiStop:
                    {
                    HTI_LOG_TEXT( "STOP" );
                    //stop all requests
                    //cancel just incoming request
                    //after all outgoing messages sent system will go down
                    iListener->Cancel();

                    // kill the watchdog, so HTI stays stopped
                    KillHtiWatchDogL();
                    }
                    break;

                case EHtiRestartServices:
                    {
                    HTI_LOG_TEXT("RESTARTSERVISE");
                    if(aMessage.Length() != 1 && aMessage.Length() != 5)
                        {
                        User::LeaveIfError(DispatchOutgoingErrorMessage( KErrArgument,
                                                    KErrDescrInvalidParameter,
                                                    KHtiSystemServiceUid ) );
                        break;
                        }
                    
                    //stop all requests
                    //cancel just incoming request
                    //after all outgoing messages sent system will go down
                    iListener->Cancel();

                    // kill the watchdog, so HTI stays stopped
                    KillHtiWatchDogL();
                    
                    TUint milliseconds = 0;
                    if(aMessage.Length() == 5)
                        {
                        milliseconds = aMessage[1] + ( aMessage[2] << 8 )
                                + ( aMessage[3] << 16 )
                                + ( aMessage[4] << 24 );
                        }
                    
                    TBuf<20> buf;
                    buf.Format(_L("%d"), milliseconds * 1000);
                    
                    RProcess htiProcess;
                    User::LeaveIfError( htiProcess.Create(
                            KHtiRestartExeName, buf ) );
                    htiProcess.Resume();
                    htiProcess.Close();
                    break;
                    }
                case EHtiReset:
                    {
                    HTI_LOG_TEXT( "RESET" );
                    Reset();
                    }
                    break;

                case EHtiFormat:
                    {
                    HTI_LOG_TEXT( "RESET FACTORY SETTINGS" );
                    if ( aMessage.Length() == 2 )
                        {
                        //set the flag to do rfs
                        iRfsMode = ( TRfsType ) aMessage[1];
                        }
                    if ( iRfsMode != ERfsNormal && iRfsMode != ERfsDeep )
                        {
                        iRfsMode = ERfsUnknown;
                        User::LeaveIfError(
                            DispatchOutgoingErrorMessage( KErrArgument,
                                          KErrDescrInvalidParameter,
                                          KHtiSystemServiceUid ) );
                        }
                    else
                        {
                        RProcess thisProcess;
                        // ERfsDeep is supported only if HTI running from ROM
                        if ( iRfsMode == ERfsNormal ||
                                 IsFileInRom( thisProcess.FileName() ) )
                            {
                            //stop
                            iListener->Cancel();
                            }
                        else
                            {
                            iRfsMode = ERfsUnknown;
                            User::LeaveIfError(
                                DispatchOutgoingErrorMessage( KErrNotSupported,
                                              KErrDescrNotInRom,
                                              KHtiSystemServiceUid ) );
                            }
                        }
                    }
                    break;

                case EHtiShowConsole:
                    {
                    HTI_LOG_TEXT( "SHOW CONSOLE" );
                    if ( !iConsole )
                        {
                        iConsole = Console::NewL( _L( "HtiFramework" ),
                                                  TSize( KConsFullScreen,
                                                         KConsFullScreen ) );
                        iConsole->Printf( _L( "HTI Framework\n" ) );
                        iConsole->Printf( _L( "=============\n\n" ) );
                        }

                    HBufC8* msg =  HBufC8::NewLC( 1 );
                    msg->Des().Append( 0 );
                    User::LeaveIfError(
                        DispatchOutgoingMessage( msg, KHtiSystemServiceUid ) );
                    CleanupStack::Pop(); // msg
                    }
                    break;

                case EHtiHideConsole:
                    {
                    HTI_LOG_TEXT( "HIDE CONSOLE" );
                    delete iConsole;
                    iConsole = NULL;

                    HBufC8* msg =  HBufC8::NewLC( 1 );
                    msg->Des().Append( 0 );
                    User::LeaveIfError(
                        DispatchOutgoingMessage( msg, KHtiSystemServiceUid ) );
                    CleanupStack::Pop(); // msg
                    }
                    break;

                case EHtiDebugPrint:
                    {
                    if ( aMessage.Length() > 1 )
                        {
                        RDebug::RawPrint( aMessage.Mid( 1 ) );
                        }
                    HBufC8* msg =  HBufC8::NewLC( 1 );
                    msg->Des().Append( 0 );
                    User::LeaveIfError(
                        DispatchOutgoingMessage( msg, KHtiSystemServiceUid ) );
                    CleanupStack::Pop(); // msg
                    }
                    break;

                default:
                    {
                    //unknown command
                    HTI_LOG_TEXT( "Error: Unknown HTI system command:" );
                    DispatchOutgoingErrorMessage( KErrArgument,
                        KHtiSystemCmdErrDescr,
                        KHtiSystemServiceUid );
                    }
                }
            }
        else
            {
            HTI_LOG_TEXT( "ERROR: not authorized acces" );
            DispatchOutgoingErrorMessage( EHtiErrNotAuthorized );
            }
        }
    else
        {
        HTI_LOG_TEXT( "Error: empty command" );
        DispatchOutgoingErrorMessage( KErrArgument,
            KHtiSystemCmdErrDescr,
            KHtiSystemServiceUid );
        }
    HTI_LOG_FUNC_OUT( "HandleSystemMessage" );
    }

void CHtiDispatcher::UrgentReboot( TInt aReason, const TDesC8& aReasonDescr )
    {
    HTI_LOG_FORMAT( "UrgentReboot: %d", aReason );
    HTI_LOG_DES( aReasonDescr );
    aReason = aReason;
    aReasonDescr.Size();
    //empty queues
    delete iIncomingQueue;
    iIncomingQueue = NULL;
    delete iOutgoingQueue;
    iOutgoingQueue = NULL;
    //stop
    iListener->Cancel();
    //reboot
    Reboot();
    }

void CHtiDispatcher::Reboot()
    {
    if ( iConsole )
        {
        iConsole->Printf( _L( "Reboot requested.\n" ) );
        }
    TInt err = KErrNone;
    RProcess rebootProcess;
    // First try the UI layer rebooter
    if(iRebootReason == -1)
        {
        err = rebootProcess.Create( KHtiDeviceRebootExeUI, KNullDesC );
        }
    else
        {
        TBuf<8> reasonNumber;
        reasonNumber.Num(iRebootReason);
        err = rebootProcess.Create( KHtiDeviceRebootExeUI, reasonNumber );
        }
    if ( err != KErrNone )
        {
        HTI_LOG_FORMAT( "UI layer rebooter failed with %d", err );
        // Try if there is OS layer rebooter present
        err = rebootProcess.Create( KHtiDeviceRebootExeOS, KNullDesC );
        }
    if ( err == KErrNone )
        {
        rebootProcess.Resume();
        rebootProcess.Close();
        }
    else
        {
        HTI_LOG_FORMAT( "Reboot err %d", err );
        if ( iConsole )
            {
            iConsole->Printf( _L( "Reboot error %d.\n" ), err );
            User::After( 3000000 ); // to let the console display a while
            }
        // Can't send any error message here - communications have been stopped
        }
    }

void CHtiDispatcher::RestoreFactorySettings()
    {
    HTI_LOG_FUNC_IN( "CHtiDispatcher::RestoreFactorySettings" );
    if ( iConsole )
        {
        iConsole->Printf( _L( "RFS requested, type %d.\n" ), iRfsMode );
        }
    TInt err = KErrNone;
    RProcess rebootProcess;
    if ( iRfsMode == ERfsNormal )
        {
        err = rebootProcess.Create( KHtiDeviceRebootExeUI, KParamNormalRfs );
        }
    else if ( iRfsMode == ERfsDeep )
        {
        err = rebootProcess.Create( KHtiDeviceRebootExeUI, KParamDeepRfs );
        }


    if ( err == KErrNone )
        {
        rebootProcess.Resume();
        rebootProcess.Close();
        }
    else
        {
        HTI_LOG_FORMAT( "RFS err %d", err );
        if ( iConsole )
            {
            iConsole->Printf( _L( "RFS error %d.\n" ), err );
            User::After( 3000000 ); // to let the console display a while
            }
        // Can't send any error message here - communications have been stopped
        }
    HTI_LOG_FUNC_OUT( "CHtiDispatcher::RestoreFactorySettings" );
    }

TBool CHtiDispatcher::IsFileInRom( const TDesC& aFileName )
    {
    HTI_LOG_FUNC_IN( "CHtiDispatcher::IsFileInRom" );
    HTI_LOG_DES( aFileName );
    TBool isInRom = EFalse;
    _LIT( KDriveZ, "z:" );
    if ( aFileName.FindF( KDriveZ ) == 0 )
        {
        isInRom = ETrue;
        }

    HTI_LOG_FORMAT( "IsFileInRom returning %d", isInRom );
    HTI_LOG_FUNC_OUT( "CHtiDispatcher::IsFileInRom" );
    return isInRom;
    }

void CleanupRArray( TAny* object )
    {
    ( ( RImplInfoPtrArray* ) object )->ResetAndDestroy();
    }

TDesC8* CHtiDispatcher::ServicePluginsListL()
    {
    RImplInfoPtrArray aImplInfoArray;
    CleanupStack::PushL( TCleanupItem( CleanupRArray, &aImplInfoArray ) );

    REComSession::ListImplementationsL(
        KHTIServiceInterfaceUid,
        aImplInfoArray );

    //alloc memory for the list
    TInt maxMemory = aImplInfoArray.Count() * KHtiServiceListRecordLength;
    HBufC8* list = HBufC8::NewLC( maxMemory );

    for ( TInt i = 0; i < aImplInfoArray.Count(); ++i )
        {
        //add uid
        TUid uid = MapServicePluginUid(
            aImplInfoArray[i]->ImplementationUid() );

        list->Des().Append( ( TUint8* )( &( uid.iUid ) ), 4 );

        //add display name, converted to 8-bit text
        TBuf8<KHtiServiceNameLength> serviceName8;
        serviceName8.Copy(
            aImplInfoArray[i]->DisplayName().Left( KHtiServiceNameLength ) );

        list->Des().Append( serviceName8 );
        list->Des().AppendFill( 0,
                       KHtiServiceNameLength - serviceName8.Length() );
        }

    CleanupStack::Pop();//list
    CleanupStack::PopAndDestroy();//aImplInfoArray

    return list;
    }

void CHtiDispatcher::KillHtiWatchDogL()
    {
    HTI_LOG_FUNC_IN( "CHtiDispatcher::KillHtiWatchDogL" );

    TFullName processName;
    TFindProcess finder( KHtiWatchDogMatchPattern );
    TInt err = finder.Next( processName );
    if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "HTI watchdog process found. Trying to open and kill it..." );
        RProcess prs;
        User::LeaveIfError( prs.Open( finder ) );
        prs.Kill( 1 );
        prs.Close();
        HTI_LOG_TEXT( "HTI watchdog killed" );
        }

    HTI_LOG_FUNC_OUT( "CHtiDispatcher::KillHtiWatchDogL" );
    }


#ifdef __ENABLE_LOGGING__
void CHtiDispatcher::DebugListPlugins()
    {
    HTI_LOG_FUNC_IN( "ListPlugins" );
    RImplInfoPtrArray aImplInfoArray;
    HTI_LOG_TEXT( "COMM PLUGINS" );
    REComSession::ListImplementationsL( KHTICommInterfaceUid, aImplInfoArray );
    HTI_LOG_FORMAT( "Num of implementations: %d", aImplInfoArray.Count() );
    TInt i;
    for ( i = 0; i < aImplInfoArray.Count(); ++i )
        {
        HTI_LOG_FORMAT( "uid: %d", aImplInfoArray[i]->ImplementationUid().iUid );
        HTI_LOG_DES( aImplInfoArray[i]->DataType() );
        HTI_LOG_DES( aImplInfoArray[i]->DisplayName() );
        }
    aImplInfoArray.ResetAndDestroy();

    HTI_LOG_TEXT( "SERVICE PLUGINS" );
    REComSession::ListImplementationsL( KHTIServiceInterfaceUid, aImplInfoArray );
    HTI_LOG_FORMAT( "Num of implementations: %d", aImplInfoArray.Count() );
    for ( i = 0; i < aImplInfoArray.Count(); ++i )
        {
        HTI_LOG_FORMAT( "uid: %d", aImplInfoArray[i]->ImplementationUid().iUid );
        HTI_LOG_DES( aImplInfoArray[i]->DataType() );
        HTI_LOG_DES( aImplInfoArray[i]->DisplayName() );

        }
    aImplInfoArray.ResetAndDestroy();
    HTI_LOG_FUNC_OUT( "ListPlugins" );
    }
#endif

void CHtiDispatcher::ShutdownAndRebootDeviceL()
    {
    HTI_LOG_TEXT( "REBOOT" );
    //stop
    iListener->Cancel();
    //and set flag to reboot
    iToReboot = ETrue;
    }

void CHtiDispatcher::CreateInstanceId()
    {
    iHtiInstanceId = User::FastCounter();
    HTI_LOG_FORMAT( "Generated instance ID %u", iHtiInstanceId );
    }

TUid CHtiDispatcher::MapServicePluginUid( const TUid aUid )
    {
    TUid mappedUid = TUid::Uid( aUid.iUid );
    switch ( aUid.iUid )
        {
        case 0x10210CCD:
            mappedUid.iUid = 0x200212C4;
            break;
        case 0x10210CCF:
            mappedUid.iUid = 0x200212C6;
            break;
        case 0x10210CD1:
            mappedUid.iUid = 0x200212C8;
            break;
        case 0x10210CD3:
            mappedUid.iUid = 0x200212CA;
            break;
        case 0x200212C4:
            mappedUid.iUid = 0x10210CCD;
            break;
        case 0x200212C6:
            mappedUid.iUid = 0x10210CCF;
            break;
        case 0x200212C8:
            mappedUid.iUid = 0x10210CD1;
            break;
        case 0x200212CA:
            mappedUid.iUid = 0x10210CD3;
            break;
        default:
            break;
        }
    return mappedUid;
    }

TBool CHtiDispatcher::CommReconnect()
    {
    if(iReconnectDelay == 0)
        {
        return EFalse;
        }
    
    //Delay
    HTI_LOG_FORMAT( "Reconnect deley : %d seconds", iReconnectDelay);
    User::After(iReconnectDelay * 1000 * 1000);
    
    //Reconnect
    iIncomingQueue->RemoveAll();
    iOutgoingQueue->RemoveAll();
    
    iListener->Reset();
    iSender->Reset();
    iListener->ReceiveMessage();

    return ETrue;
    }
