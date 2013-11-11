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
* Description:  Implementation of the main class for IPProxyEngine
*
*/



// INCLUDE FILES
#include "CIPProxyEngine.h"
#include "Ctcpportlistener.h"
#include "CLocalTCPConnection.h"
#include "Csocketrouter.h"
#include "MIPProxyEngineObserver.h"
#include "MHostConnection.h"
#include "MAbstractConnection.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"

const TInt KPeerDisconnectDelay = 60000000;  //60 seconds

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CleanupSocket RSocket pointer cleanup operation
// -----------------------------------------------------------------------------
//
LOCAL_C void CleanupSocket( TAny* aPtr )
    {
    RSocket* socket = ( RSocket* ) aPtr;
    if ( socket )
        {
        socket->Close();
        delete socket;
        }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CIPProxyEngine::CIPProxyEngine
// -----------------------------------------------------------------------------
//
CIPProxyEngine::CIPProxyEngine( MAbstractConnection* aConnection )
    : CActive( EPriorityStandard ), iAbstractConnection( aConnection )
    {
    __ASSERT_DEBUG( iAbstractConnection, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::ConstructL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::ConstructL()
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::ConstructL()" ) );

    User::LeaveIfError( iTimer.CreateLocal() );
    User::LeaveIfError( iSocketServ.Connect() );

    iHostConnection = iAbstractConnection->GetHostConnection();
    __ASSERT_DEBUG( iHostConnection, User::Invariant() );
    iHostConnection->SetObserver( this );

    // Peer array. Granularity 10 suits fine.
    iPeerListenerArray = new (ELeave) CArrayPtrFlat<CTCPPortListener> ( 10 );

    // Local TCP connections array
    iLocalConnArray = new (ELeave) CArrayPtrFlat<CLocalTCPConnection> ( 5 );

    iSocketRouter = CSocketRouter::NewL( this );

    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::NewL
// -----------------------------------------------------------------------------
//
CIPProxyEngine* CIPProxyEngine::NewL( MAbstractConnection* aConnection )
    {
    CIPProxyEngine* self = CIPProxyEngine::NewLC( aConnection );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::NewLC
// -----------------------------------------------------------------------------
//
CIPProxyEngine* CIPProxyEngine::NewLC( MAbstractConnection* aConnection )
    {
    CIPProxyEngine* self = new( ELeave ) CIPProxyEngine( aConnection );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }


// Destructor
CIPProxyEngine::~CIPProxyEngine()
    {
    Cancel();
    iTimer.Close();
    delete iSocketRouter;

    if ( iPeerListenerArray )
        {
        iPeerListenerArray->ResetAndDestroy();
        delete iPeerListenerArray;
        }
    if ( iLocalConnArray )
        {
        iLocalConnArray->ResetAndDestroy();
        delete iLocalConnArray;
        }

    iSocketServ.Close();
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::RunL
// This is called when the timer expires
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::RunL()
    {
    DEBUG_PRINT( DEBUG_STRING( "CIPProxyEngine::RunL()" ) );
    DEBUG_PRINT( DEBUG_STRING( "    peer count = %d" ),
        iSocketRouter->SocketCount() );

    if ( iSocketRouter->SocketCount() == 0 )
        {
        iHostConnection->IssueDisconnect();
        }
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::DoCancel
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::DoCancel()
    {
    iTimer.Cancel();
    DEBUG_PRINT( DEBUG_STRING( "Timeout timer cancelled" ) );
    }


// -----------------------------------------------------------------------------
// CIPProxyEngine::SetObserver
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::SetObserver( MIPProxyEngineObserver* aObserver )
    {
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::AddPeerListeningPortL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::AddPeerListeningPortL( TInt aPort )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::AddPeerListeningPortL(%d)" ), aPort );

    CTCPPortListener* newListener = CTCPPortListener::NewLC( aPort, this );
    iPeerListenerArray->AppendL( newListener );
    if ( iListening )
        {
        newListener->IssueListen();
        }
    CleanupStack::Pop( newListener );

    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::AddPeerListeningPortL, Adding also UDP listener(%d)" ),
         aPort );

    // Adding also UDP listener for the port
    iSocketRouter->AddUDPSocketL( aPort );
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::StartListening
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::StartListening()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::StartListening()" ) );
    TInt peerListenerArrayCount = iPeerListenerArray->Count();
    for ( TInt i = 0; i < peerListenerArrayCount; i++ )
        {
        iPeerListenerArray->At( i )->IssueListen();
        }
    iListening = ETrue;

    AssureConnectionL();
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::StopListening
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::StopListening()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::StopListening()" ) );

    TInt peerListenerArrayCount = iPeerListenerArray->Count();
    for ( TInt i = 0; i < peerListenerArrayCount; i++ )
        {
        iPeerListenerArray->At( i )->Cancel();
        }
    iListening = EFalse;
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::DisconnectAllConnections
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::DisconnectAllConnections()
    {
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::ConnectionAcceptedL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::ConnectionAcceptedL( RSocket* aSocket )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::ConnectionAcceptedL()" ) );

    Cancel();   //Possible timeout timer

    CleanupStack::PushL( TCleanupItem( CleanupSocket, aSocket ) );
    iSocketRouter->AddPeerSocketL( aSocket );
    CleanupStack::Pop(); //aSocket

    AssureConnectionL();
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::ErrorL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::ErrorL( TInt aErrorCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::ErrorL(%d)" ), aErrorCode );
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::ObserverLeaved
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::ObserverLeaved( TInt aLeaveCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::ObserverLeaved(%d)" ), aLeaveCode );
    if ( iObserver )
        {
        iObserver->ObserverLeaved( aLeaveCode );
        }
    }


// -----------------------------------------------------------------------------
// CIPProxyEngine::ConnectionEstablishedL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::ConnectionEstablishedL()
    {
    // Connection to host established via bluetooth

    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::ConnectionEstablishedL()" ) );

    MSocket* socket = iAbstractConnection->GetSocket();
    if ( socket )
        {
        iSocketRouter->SetHostSocketL( socket );
        iSocketRouter->StartRouting();
        }
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::HostConnectionErrorL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::HostConnectionErrorL( TInt aErrorCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::HostConnectionErrorL(%d)" ), aErrorCode );
    iHostConnection->IssueDisconnect();
    iSocketRouter->RemoveAllPeers();
    iSocketRouter->StopRouting();
    iSocketRouter->ResetQueue();
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::HostConnectionObserverLeaved
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::HostConnectionObserverLeaved( TInt aLeaveCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::HostConnectionObserverLeaved(%d)" ), aLeaveCode );
    if ( iObserver )
        {
        iObserver->ObserverLeaved( aLeaveCode );
        }
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::LocalTCPConnectionEstablishedL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::LocalTCPConnectionEstablishedL( TUint aPort )
    {
    // Connection to local host (via TCP connection) established.

    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::LocalTCPConnectionEstablishedL(), port=%d" ), aPort );

    // TCP connection that was initiated from the PC side, got connected
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::ConnectionEstablishedL() -\
         Local conn. was connected, adding the socket to socket router" ) );

    TInt connIndex = FindLocalTCPConn( aPort );
    if ( connIndex >= 0 )
        {
        CLocalTCPConnection* conn = iLocalConnArray->At( connIndex );
        iSocketRouter->AddPeerSocketL( conn->Socket() );
        // Socket router took ownership of the socket
        conn->SetSocketOwnership( EFalse );
        // Connection was established,
        conn->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::LocalTCPConnectionErrorL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::LocalTCPConnectionErrorL( TInt aPort, TInt aErrorCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::LocalTCPConnectionErrorL(%d)" ), aErrorCode );

    TInt connIndex = FindLocalTCPConn( aPort );
    if ( connIndex >= 0 )
        {
        RSocket* localSocket = iLocalConnArray->At( connIndex )->Socket();
        iSocketRouter->RemovePeerSocket( localSocket );
        iLocalConnArray->Delete( connIndex );
        }
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::LocalTCPConnectionObserverLeaved
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::LocalTCPConnectionObserverLeaved( TInt aPort,
                                                       TInt aLeaveCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::HostConnectionObserverLeaved(%d), port%d" ),
        aLeaveCode, aPort );
    if ( iObserver )
        {
        iObserver->ObserverLeaved( aLeaveCode );
        }
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::SocketRouterErrorL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::SocketRouterErrorL(
    const MSocket* /*aSocket*/, TInt aErrorCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::SocketRouterErrorL(%d)" ), aErrorCode );

    if ( aErrorCode == -6305 )  //BT disconnected
        {
        iHostConnection->IssueDisconnect();
        iSocketRouter->RemoveAllPeers();
        iSocketRouter->StopRouting();
        iSocketRouter->ResetQueue();
        }
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::ObserverLeaved
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::ObserverLeaved(
    const MSocket* /*aSocket*/, TInt aLeaveCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::ObserverLeaved(%d)" ), aLeaveCode );
    if ( iObserver )
        {
        iObserver->ObserverLeaved( aLeaveCode );
        }
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::PeerDisconnectedL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::PeerDisconnectedL( const MSocket* aSocket )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::PeerDisconnectedL(), localPort= %d. remotePort=%d" ),
        aSocket->LocalPort(), aSocket->RemotePort() );

    // Check if the socket was a local TCP connection socket
    TInt indexOfConn = -1;
    indexOfConn = FindLocalTCPConn( aSocket->RemotePort() );
    if ( indexOfConn >= 0 )
        {
        // Notify PC side that the connection was disconnected
        iSocketRouter->SendCloseTCPConnection( aSocket->RemotePort() );

        DEBUG_PRINT( DEBUG_STRING(
            "CIPProxyEngine::PeerDisconnectedL(), Deleting local connection." ),
            aSocket->LocalPort(), aSocket->RemotePort() );
        iLocalConnArray->Delete( indexOfConn );
        }

    Cancel();
    iTimer.After( iStatus, KPeerDisconnectDelay );
    SetActive();
    DEBUG_PRINT( DEBUG_STRING( "Timeout timer activated" ) );

    if ( iSocketRouter->SocketCount() == 0 )
        {
        iSocketRouter->StopRouting();
        }
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::HostDisconnectedL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::HostDisconnectedL( const MSocket* /*aSocket*/ )
    {
    iHostConnection->IssueDisconnect();
    if ( iSocketRouter->IsRouting() )
        {
        iSocketRouter->StopRouting();
        }
    iSocketRouter->ResetQueue();
    iSocketRouter->RemoveAllPeers();
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::OpenLocalTCPConnectionL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::OpenLocalTCPConnectionL( TUint aPort )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::OpenLocalTCPConnectionL(), port=%d"), aPort );

    CLocalTCPConnection* newConn =
        CLocalTCPConnection::NewLC( this, aPort );

    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::OpenLocalTCPConnectionL(), Issuing connection") );
    iLocalConnArray->AppendL( newConn );
    CleanupStack::Pop( newConn );

    newConn->IssueConnectL();
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::OpenListeningTCPConnectionL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::OpenListeningTCPConnectionL( TUint aPort )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::OpenListeningTCPConnectionL(), port=%d"), aPort );

    TInt count = iPeerListenerArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iPeerListenerArray->At( i )->Port() == aPort )
            {
            // Port already listening
            return;
            }
        }

    // AddPeerListeningPortL will call IssueListen() only if iListening is set
    // to ETrue
    iListening = ETrue;
    AddPeerListeningPortL( aPort );
    AssureConnectionL();
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::CloseTCPConnection
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::CloseTCPConnection( TUint aPort )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::CloseTCPConnection(), port=%d"), aPort );

    // Delete local TCP connections
    TInt index = FindLocalTCPConn( aPort );
    if ( index > -1 )
        {
        CLocalTCPConnection* conn = iLocalConnArray->At( index );
        iLocalConnArray->Delete( index );
        delete conn;

        DEBUG_PRINT( DEBUG_STRING(
            "CIPProxyEngine::CloseTCPConnection(), conn deleted.") );
        }
    
    // stop listening on this port
    TInt peerListenerArrayCount = iPeerListenerArray->Count();
    for ( TInt i = 0; i < peerListenerArrayCount; i++ )
        {
        CTCPPortListener* listener = iPeerListenerArray->At( i );
        if(listener->Port() == aPort)
            {
            listener->Cancel();
            iPeerListenerArray->Delete(i);
            delete listener;
            break;
            }
        }
    
    if(iPeerListenerArray->Count() == 0)
        {
        iListening = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::CloseAllTCPConnections
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::CloseAllTCPConnections()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::CloseAllTCPConnections()" ) );

    iSocketRouter->ResetQueue();
    iSocketRouter->RemoveAllPeers();

    iLocalConnArray->ResetAndDestroy();
    
    StopListening();

    if ( iPeerListenerArray )
        {
        iPeerListenerArray->ResetAndDestroy();
        }
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::FindLocalTCPConn
// -----------------------------------------------------------------------------
//
TInt CIPProxyEngine::FindLocalTCPConn( TUint aPort )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CIPProxyEngine::FindLocalTCPConn(), port=%d"), aPort );
    for ( TInt i = 0; i < iLocalConnArray->Count(); i++ )
        {
        if ( iLocalConnArray->At(i)->Port() == aPort )
            {
            return i;
            }
        }
    return -1;
    }

// -----------------------------------------------------------------------------
// CIPProxyEngine::AssureConnectionL
// -----------------------------------------------------------------------------
//
void CIPProxyEngine::AssureConnectionL()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "AssureConnectionL()" ) );
    if ( iHostConnection->IsConnected() )
        {
        DEBUG_PRINT( DEBUG_STRING(
            "AssureConnectionL, connected" ) );
        if ( !iSocketRouter->IsRouting() )
            {
            DEBUG_PRINT( DEBUG_STRING(
                "AssureConnectionL, starting routing" ) );
            iSocketRouter->StartRouting();
            }
        }
    else
        {
        DEBUG_PRINT( DEBUG_STRING(
            "AssureConnectionL, not connected, connecting" ) );
        iHostConnection->IssueConnectL();
        }
    }
//  End of File
