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
* Description:  ECOM plugin for communication over IP port
*
*/


// INCLUDE FILES
#include "HtiIPCommEcomPlugin.h"
#include "HtiConnectionManager.h"
#include "HtiCfg.h"
#include "HtiIPCommLogging.h"

#include <commdb.h>             // CCommsDatabase
#include <HtiCfg.h>


// CONSTANTS
const static TInt KMaxHtiNotifierLength = 128;

_LIT( KHtiIPCommError, "HtiIpCommError" );
_LIT( KHtiOkButton, "OK" );

_LIT( KHtiCfgPath,      "\\" ); // root of drive
_LIT( KHtiIPCommCfg,    "HTIIPComm.cfg" );
_LIT8( KIAPId,          "IAPId" );
_LIT8( KIAPName,        "IAPName" );
_LIT8( KLocalPort,      "LocalPort" );
_LIT8( KRemoteHost,     "RemoteHost" );
_LIT8( KRemotePort,     "RemotePort" );
_LIT8( KConnectTimeout, "ConnectTimeout" );


void LogLocalHost( RSocket& aSocket )
    {
#ifdef __ENABLE_LOGGING__
    // print the local ip to log
    HTI_LOG_TEXT( "LocalHost:" );
    TBuf<0x20> tmp;
    TInetAddr localHost;
    aSocket.LocalName( localHost );
    localHost.Output( tmp );
    tmp.AppendFormat( _L(":%d"), localHost.Port() );
    HTI_LOG_DES( tmp );
#else
    aSocket.LocalPort();  // avoid compiler warning
#endif
    }

void LogRemoteHost( RSocket& aSocket )
    {
#ifdef __ENABLE_LOGGING__
    // print the local ip to log
    HTI_LOG_TEXT( "RemoteHost:" );
    TBuf<0x20> tmp;
    TInetAddr remoteHost;
    aSocket.RemoteName( remoteHost );
    remoteHost.Output( tmp );
    tmp.AppendFormat( _L(":%d"), remoteHost.Port() );
    HTI_LOG_DES( tmp );
#else
    aSocket.LocalPort();  // avoid compiler warning
#endif
    }


CHtiConnectionManager* CHtiConnectionManager::NewL( CHtiIPCommServer* aServer )
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::NewL" );
    CHtiConnectionManager* self = new (ELeave) CHtiConnectionManager( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    HTI_LOG_FUNC_OUT( "CHtiConnectionManager::NewL" );
    return self;
    }

CHtiConnectionManager::CHtiConnectionManager( CHtiIPCommServer* aServer ):
    CActive( EPriorityStandard ),
    iServer( aServer ),
    iCfg( NULL ),
    iListenPort( 0 ),
    iState( EDisconnected ),
    iReceiveRequestComplete( ETrue ),
    iSendRequestComplete( ETrue ),
    iSendMonitor( NULL ),
    iReceiveMonitor( NULL ),
    iConnectTimer( NULL )
    {
    iReceiveBuffer.Zero();
    iSendBuffer.Zero();
    CActiveScheduler::Add(this);
    }

CHtiConnectionManager::~CHtiConnectionManager()
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::~CHtiConnectionManager" );

    if ( iCfg )
        delete iCfg;

    if ( iConnectTimer )
        delete iConnectTimer;

    if ( iDataSocket.SubSessionHandle() )
        iDataSocket.Close();

    if ( iListenSocket.SubSessionHandle() )
        iListenSocket.Close();

    if ( iSendMonitor )
        delete iSendMonitor;

    if ( iReceiveMonitor )
        delete iReceiveMonitor;

    if ( iConnection.SubSessionHandle() )
        iConnection.Close();

    if ( iSocketServ.Handle() )
        iSocketServ.Close();

    // NOTE: If this is done in the beginning the server will never die!
    Cancel();

    HTI_LOG_FUNC_OUT( "CHtiConnectionManager::~CHtiConnectionManager" );
    }


void CHtiConnectionManager::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::ConstructL" );

    // Load configuration file
    iCfg = CHtiCfg::NewL();
    TRAPD( err, iCfg->LoadCfgL( KHtiCfgPath, KHtiIPCommCfg ) );
    if ( err )
        {
        HTI_LOG_FORMAT( "LoadCfgL err %d", err );
        ShowErrorNotifierL( _L( "Could not load config file" ), err );
        User::Leave( err );
        }

    // Get IAP
    ReadIAPConfigL();

    // Create monitors and timers
    iSendMonitor    = CHtiSocketMonitor::NewL( MHtiSocketObserver::EWriteSocket, this);
    iReceiveMonitor = CHtiSocketMonitor::NewL( MHtiSocketObserver::EReadSocket, this);
    iConnectTimer   = CHtiTimer::NewL( *this );

    // Open socket server
    err = iSocketServ.Connect();
    if ( err )
        {
        HTI_LOG_FORMAT( "error connecting to socket server %d", err);
        User::Leave( err );
        }

/*
    HTI_LOG_TEXT( "Supported protocols:" );
    TUint numOfProtocols;
    iSocketServ.NumProtocols( numOfProtocols );
    for ( TInt i = 1; i <= numOfProtocols; i++ )
        {
        TProtocolDesc desc;
        iSocketServ.GetProtocolInfo( i, desc  );
        HTI_LOG_DES( desc.iName );
        HTI_LOG_FORMAT( "AddrFamily : %d", desc.iAddrFamily );
        HTI_LOG_FORMAT( "SockType   : %d", desc.iSockType );
        HTI_LOG_FORMAT( "Protocol   : %d", desc.iProtocol );
        }
*/

    // Start IAP
    err = iConnection.Open( iSocketServ );
    if ( err )
        {
        HTI_LOG_FORMAT( "error opening connection %d", err);
        User::Leave( err );
        }

    HTI_LOG_TEXT( "Starting IAP" );
    iConnPref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
    iConnPref.SetIapId( iIAPId );
    iConnection.Start( iConnPref, iStatus );
    iState = EStartingIAP;
    SetActive();

    HTI_LOG_FUNC_IN( "CHtiConnectionManager::ConstructL" );
    }


void CHtiConnectionManager::ReadIAPConfigL()
    {
    // Try to read IAP id first
    TRAPD( err, iIAPId = iCfg->GetParameterIntL( KIAPId ) );
    if ( err == KErrNone )
        {
        HTI_LOG_FORMAT( "Using IAP id %d", iIAPId);
        }
    else
        {
        // IAP id not defined try reading IAP name
        TBuf8<KMaxParameterValueLength> IAPNameCfg;
        TRAP( err, IAPNameCfg = iCfg->GetParameterL( KIAPName ) );

        // If IAP name is not defined it wont be found from commsdb...

        HTI_LOG_TEXT( "Searching for IAP:" );
        HTI_LOG_DES( IAPNameCfg );

        // open commdb
        CCommsDatabase* commDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
        CleanupStack::PushL(commDb);

        // open IAP table

        // Using all bearers from TCommDbBearer because when
        // using KCommDbBearerUnknown it just leaves
        TUint32 bearerSet = KCommDbBearerCSD|KCommDbBearerPSD|KCommDbBearerLAN|
                            KCommDbBearerVirtual|KCommDbBearerPAN|
                            KCommDbBearerWLAN;

        CCommsDbTableView* commView =
            commDb->OpenIAPTableViewMatchingBearerSetLC(bearerSet,
                                                        ECommDbConnectionDirectionUnknown);

        // search all IAPs
        HTI_LOG_TEXT( "IAP records:" );
        if (commView->GotoFirstRecord() == KErrNone)
            {
            do
                {
                TBuf8<KCommsDbSvrMaxFieldLength> iapName;
                commView->ReadTextL( TPtrC(COMMDB_NAME), iapName );
                HTI_LOG_DES( iapName );

                if ( iapName == IAPNameCfg )
                    {
                    commView->ReadUintL( TPtrC(COMMDB_ID), iIAPId );
                    HTI_LOG_FORMAT( "Found it! IAP id %d", iIAPId );
                    }

                }
            while ( commView->GotoNextRecord() == KErrNone );
            }
        CleanupStack::PopAndDestroy(2); // commView, commDb
        }

    // Cannot start if Internet Access Point is not defined
    if ( iIAPId == 0 )
        {
        HTI_LOG_TEXT( "IAP not defined" );
        ShowErrorNotifierL( _L( "IAP not defined" ), KErrNotFound );
        User::Leave( KErrNotFound );
        }
    }


void CHtiConnectionManager::ReadConnectionConfigL()
    {
    // Read listening port number from config file
    TRAPD( err, iListenPort = iCfg->GetParameterIntL( KLocalPort ) );

    if ( iListenPort == 0)
        {
        // ...or remote host to connect

        TBuf8<KMaxParameterValueLength> remoteHostCfg;
        TRAP( err, remoteHostCfg = iCfg->GetParameterL( KRemoteHost ) );
        if ( err )
            {
            HTI_LOG_TEXT( "No remote host specified!" )
            ShowErrorNotifierL( _L( "No remote host specified!" ), err );
            User::Leave( err );
            }

        // Convert TDesC8 -> TDesC
        TBuf<KMaxParameterValueLength> tmp;
        tmp.Copy( remoteHostCfg );

        // Check remote host if its a plain ip address
        if ( iRemoteHost.Input( tmp ) )
            {
            // ...its not. Do a DNS-lookup request
            HTI_LOG_TEXT( "Do a DSN-lookup request" );

            RHostResolver resolver;
            err = resolver.Open( iSocketServ, KAfInet, KProtocolInetUdp, iConnection );
            if ( err )
                {
                HTI_LOG_FORMAT( "error opening resolver %d", err );
                User::Leave( err );
                }

            TNameEntry entry;
            err = resolver.GetByName( tmp, entry );
            if ( err )
                {
                HTI_LOG_FORMAT( "error getting address by name %d", err );
                ShowErrorNotifierL(
                    _L( "Could not resolve remote host!" ), err );
                User::Leave( err );
                }

            iRemoteHost = entry().iAddr;

            resolver.Close();
            }

        // Get remote host port
        TRAP( err, iRemoteHost.SetPort( iCfg->GetParameterIntL( KRemotePort ) ) );
        if ( err )
            {
            HTI_LOG_TEXT( "No remote port specified!" )
            ShowErrorNotifierL( _L( "No remote port specified!" ), err );
            User::Leave( err );
            }

        // Get connect timeout
        TRAP( err, iConnectTimeout = iCfg->GetParameterIntL( KConnectTimeout ) );
        if ( err )
            {
            // default is 30 seconds
            iConnectTimeout = 30;
            }

        HTI_LOG_FORMAT( "Connect timeout %d", iConnectTimeout );
        }
    }

void CHtiConnectionManager::StartConnectingL()
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::StartConnectingL" );
    CancelAllRequests();

    // close if open
    if ( iDataSocket.SubSessionHandle() )
        iDataSocket.Close();

    // open data socket
    TInt err = iDataSocket.Open( iSocketServ,
                        KAfInet,
                        KSockStream,
                        KProtocolInetTcp,
                        iConnection );
    if ( err )
        {
        HTI_LOG_FORMAT( "error opening data socket %d", err );
        User::Leave( err );
        }

    iDataSocket.Connect( iRemoteHost, iStatus );
    iState = EConnecting;

    // Set a timeout for this operation if timer is not
    // already active and there is a timeout defined
    if ( iConnectTimeout && !iConnectTimer->IsActive() )
        {
        // iConnectTimeout is in seconds
        iConnectTimer->After( iConnectTimeout*1000000 );
        }

    SetActive();
    HTI_LOG_FUNC_OUT( "CHtiConnectionManager::StartConnectingL" );
    }


void CHtiConnectionManager::StartListeningL()
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::StartListeningL" );
    HTI_LOG_FORMAT( "Port %d", iListenPort );

    CancelAllRequests();

    // close if open
    if ( iDataSocket.SubSessionHandle() )
        iDataSocket.Close();

    // open empty socket
    TInt err = iDataSocket.Open( iSocketServ  );
    if ( err )
        {
        HTI_LOG_FORMAT( "error opening blank socket %d", err );
        User::Leave( err );
        }

    // start listening
    iListenSocket.Accept( iDataSocket, iStatus );
    iState = EWaitingConnection;

    SetActive();
    HTI_LOG_FUNC_OUT( "CHtiConnectionManager::StartListeningL" );
    }

void CHtiConnectionManager::CancelAllRequests()
    {
    HTI_LOG_TEXT( "Cancelling all active server requests" );
    CancelReceive();
    CancelSend();
    }

void CHtiConnectionManager::RunL()
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::RunL" );
    HTI_LOG_FORMAT( "status %d", iStatus.Int() );

    TInt err;

    switch ( iState )
        {
        case EStartingIAP:

            HTI_LOG_TEXT( "EStartingIAP" );

            if ( iStatus.Int() )
                {
                HTI_LOG_FORMAT( "error starting IAP %d", iStatus.Int() );
                ShowErrorNotifierL( _L( "Error starting IAP" ), iStatus.Int() );
                User::Leave( iStatus.Int() );
                }

            ReadConnectionConfigL();

            // remote host is defined - start connecting to it
            if ( iListenPort == 0 )
                {
                StartConnectingL();
                }
            // remote host not defined - start listening
            else
                {
                HTI_LOG_TEXT( "Setting up listen socket" );

                // open listening socket
                err = iListenSocket.Open( iSocketServ,
                                    KAfInet,
                                    KSockStream,
                                    KProtocolInetTcp,
                                    iConnection );
                if ( err )
                    {
                    HTI_LOG_FORMAT( "error opening listen socket %d", err );
                    User::Leave( err );
                    }

                // set the port to listen
                err = iListenSocket.SetLocalPort( iListenPort );
                if ( err )
                    {
                    HTI_LOG_FORMAT( "error setting local port %d", err );
                    User::Leave( err );
                    }

                // set listen queue size
                err = iListenSocket.Listen( 5 );
                if ( err )
                    {
                    HTI_LOG_FORMAT( "error settig up listening socket %d", err );
                    User::Leave( err );
                    }

                StartListeningL();
                }


            break;

        case EWaitingConnection:
            HTI_LOG_TEXT( "EWaitingConnection" );

            if ( iStatus.Int() )
                {
                HTI_LOG_FORMAT( "error accepting connection %d", iStatus.Int() );
                ShowErrorNotifierL(
                    _L( "Error accepting connection!" ), iStatus.Int() );
                User::Leave( iStatus.Int() );
                }

            iState = EConnected;
            HTI_LOG_TEXT( "Connected!" );
            User::InfoPrint( _L("HtiIPComm: connected!") );

            if ( !iReceiveRequestComplete )
                {
                // There is a pending read request
                HTI_LOG_TEXT( "Pending read request" );
                ReadSocket();
                }

            if ( !iSendRequestComplete )
                {
                // There is a pending write request
                HTI_LOG_TEXT( "Pending write request" );
                WriteSocket();
                }

            break;

        case EConnecting:
            HTI_LOG_TEXT( "EConnecting" );

            if ( iStatus.Int() )
                {
                HTI_LOG_FORMAT( "error connecting to remote host %d", iStatus.Int() );
                HTI_LOG_TEXT( "trying again..." );
                User::After( 1000000 ); // wait 1 second before trying again
                StartConnectingL();
                }
            else
                {
                // Cancel the timer
                iConnectTimer->Cancel();

                iState = EConnected;
                HTI_LOG_TEXT( "Connected!" );
                User::InfoPrint( _L("HtiIPComm: connected!") );

                //LogLocalHost( iDataSocket );
                //LogRemoteHost( iDataSocket );

                if ( !iReceiveRequestComplete )
                    {
                    // There is a pending read request
                    HTI_LOG_TEXT( "Pending read request" );
                    ReadSocket();
                    }

                if ( !iSendRequestComplete )
                    {
                    // There is a pending write request
                    HTI_LOG_TEXT( "Pending write request" );
                    WriteSocket();
                    }
                }

            break;

        case EConnected:
            HTI_LOG_TEXT( "EConnected" );
            break;

        case EDisconnecting:
            HTI_LOG_TEXT( "EDisconnecting" );
            break;

        case EDisconnected:
            HTI_LOG_TEXT( "EDisconnected" );
            break;

        default:
            HTI_LOG_TEXT( "Unknown" );
            User::Panic( _L("HtiIPComm"), EUnknownState );
        }

    HTI_LOG_FUNC_OUT( "CHtiConnectionManager::RunL" );
    }


void CHtiConnectionManager::DoCancel()
    {
    HTI_LOG_TEXT( "CHtiConnectionManager::DoCancel" );
    }


TInt CHtiConnectionManager::RunError(TInt aError)
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::RunError" );
    HTI_LOG_FORMAT( "error %d closing server...", aError );
    iServer->CloseServer();
    HTI_LOG_FUNC_OUT( "CHtiConnectionManager::RunError" );
    aError = aError;
    return KErrNone;
    }


void CHtiConnectionManager::Receive( const RMessage2& aMessage )//( TDes8& aRawdataBuf, TRequestStatus& aStatus )
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::Receive" );

    if ( !iReceiveRequestComplete )
        {
        HTI_LOG_TEXT( "complete with KErrServerBusy" );
        aMessage.Complete( KErrServerBusy );
        }
    else
        {
        iReceiveRequestComplete = EFalse;
        iReceiveRequest = aMessage;

        if ( iState == EConnected )
            {
            ReadSocket();
            }
        else
            {
            HTI_LOG_TEXT( "not connected" );
            }
        }

    HTI_LOG_FUNC_OUT( "CHtiConnectionManager::Receive" );
    }


void CHtiConnectionManager::Send( const RMessage2& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::Send" );

    if ( !iSendRequestComplete )
        {
        HTI_LOG_TEXT( "complete with KErrServerBusy" );
        aMessage.Complete( KErrServerBusy );
        }
    else
        {
        iSendRequestComplete = EFalse;
        iSendRequest = aMessage;
        TInt err = aMessage.Read( 0, iSendBuffer );
        if ( err )
            {
            User::Panic( _L("HtiIPComm"), EBadDescriptor );
            }

        if ( iState == EConnected )
            {
            WriteSocket();
            }
        else
            {
            HTI_LOG_TEXT( "not connected" );
            }
        }

    HTI_LOG_FUNC_OUT( "CHtiConnectionManager::Send" );
    }

void CHtiConnectionManager::ReadSocket()
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::ReadSocket" );

    iReceiveMonitor->Activate();
    iReceiveBuffer.Zero();
    iDataSocket.RecvOneOrMore( iReceiveBuffer, 0,
        iReceiveMonitor->iStatus, iRecvLen );

    // This works fine with the emulator...
    //iDataSocket.RecvOneOrMore( *(TDes8*) iReceiveRequest.Ptr0(),
    //                       0, iReceiveMonitor->iStatus, iRecvLen );

    HTI_LOG_FUNC_OUT( "CHtiConnectionManager::ReadSocket" );
    }

void CHtiConnectionManager::WriteSocket()
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::WriteSocket" );

    iSendMonitor->Activate();
    iDataSocket.Write( iSendBuffer, iSendMonitor->iStatus );

    // This works fine with the emulator...
    //iDataSocket.Write( *(TDesC8*) iSendRequest.Ptr0(),
    //               iSendMonitor->iStatus );

    HTI_LOG_FUNC_OUT( "CHtiConnectionManager::WriteSocket" );
    }

void CHtiConnectionManager::CancelReceive()
    {
    HTI_LOG_TEXT( "CHtiConnectionManager::CancelReceive" );

    if ( !iReceiveRequestComplete )
        {
        // Is there an active socket receive?
        if ( iReceiveMonitor->IsActive() )
            {
            // ReportComplete() should complete this
            HTI_LOG_TEXT( "CancelRecv" );
            iDataSocket.CancelRecv();
            }
        else
            {
            HTI_LOG_TEXT( "complete with KErrCancel" );
            iReceiveRequest.Complete( KErrCancel );
            iReceiveRequestComplete = ETrue;
            iReceiveBuffer.Zero();
            }
        }
    }

void CHtiConnectionManager::CancelSend()
    {
    HTI_LOG_TEXT( "CHtiConnectionManager::CancelSend" );

    if ( !iSendRequestComplete )
        {
        // Is there an active socket send?
        if ( iSendMonitor->IsActive() )
            {
            // ReportComplete() should complete this
            HTI_LOG_TEXT( "CancelWrite" );
            iDataSocket.CancelWrite();
            }
        else
            {
            HTI_LOG_TEXT( "complete with KErrCancel" );
            iSendRequest.Complete( KErrCancel );
            iSendRequestComplete = ETrue;
            iSendBuffer.Zero();
            }
        }
    }

void CHtiConnectionManager::ReportComplete( MHtiSocketObserver::TRequestType aType, TInt aError )
    {
    HTI_LOG_FUNC_IN( "CHtiConnectionManager::ReportComplete" );

    HTI_LOG_FORMAT( "error %d", aError );

    TInt err = 0;

    switch ( aType )
        {
        case MHtiSocketObserver::EReadSocket:
            HTI_LOG_TEXT( "EReadSocket" );
            err = iReceiveRequest.Write( 0 , iReceiveBuffer );
            if ( err )
                {
                HTI_LOG_FORMAT( "Panic! Error writing received data to client buffer %d", err );
                User::Panic( _L("HtiIPComm"), EBadDescriptor );
                }

            iReceiveBuffer.Zero();
            iReceiveRequestComplete = ETrue;
            iReceiveRequest.Complete( aError );

            break;

        case MHtiSocketObserver::EWriteSocket:
            HTI_LOG_TEXT( "EWriteSocket" );
            iSendBuffer.Zero();
            iSendRequestComplete = ETrue;
            iSendRequest.Complete( aError );
            break;

        default:
            User::Panic( _L("HtiIPComm"), EUnknownCompletion );
        }


    // Disconnect if there is an error.
    if ( aError &&
        ( aError != KErrCancel ) ) // ...except when there is a cancel
        {
        iState = EDisconnected;
        HTI_LOG_TEXT( "Disconnected!" );
        User::InfoPrint( _L("HtiIPComm: Disconnected!") );

        // If disconnected try to listen or connect again
        if ( iListenPort == 0 )
            {
            // wait 1 second before trying again
            User::After( 1000000 );
            StartConnectingL();
            }
        else
            {
            StartListeningL();
            }
        }

    HTI_LOG_FUNC_OUT( "CHtiConnectionManager::ReportComplete" );
    }

CHtiTimer* CHtiTimer::NewL( MHtiTimerObserver& aObserver )
    {
    CHtiTimer* self = new (ELeave) CHtiTimer( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CHtiTimer::CHtiTimer( MHtiTimerObserver& aObserver ):
    CTimer( EPriorityUserInput ),
    iObserver( aObserver)
    {
    }

CHtiTimer::~CHtiTimer()
    {
    Cancel();
    }

void CHtiTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

void CHtiTimer::RunL()
    {
    iObserver.TimerExpiredL();
    }

TInt CHtiTimer::RunError(TInt aError)
    {
    HTI_LOG_FORMAT( "CHtiTimer::RunError %d", aError );
    aError = aError;
    return KErrNone;
    }

void CHtiConnectionManager::TimerExpiredL()
    {
    HTI_LOG_TEXT( "Timed out! Closing IPCommServer..." );
    ShowErrorNotifierL(
        _L( "Timed out connecting to remote host!" ), KErrTimedOut );
    iServer->CloseServer();
    }

void CHtiConnectionManager::ShowErrorNotifierL( const TDesC& aText,
                                                   TInt aErr )
    {
    RNotifier notifier;
    User::LeaveIfError( notifier.Connect() );

    TBuf<KMaxHtiNotifierLength> errorMsg;
    // aText is cut if it's too long - leaving some space also for error code
    errorMsg.Append( aText.Left( errorMsg.MaxLength() - 10 ) );
    errorMsg.Append( _L("\n") );
    errorMsg.AppendNum( aErr );

    TRequestStatus status;
    TInt button;
    notifier.Notify( KHtiIPCommError, errorMsg,
                     KHtiOkButton, KNullDesC, button, status );
    User::WaitForRequest( status );
    notifier.Close();
    }



