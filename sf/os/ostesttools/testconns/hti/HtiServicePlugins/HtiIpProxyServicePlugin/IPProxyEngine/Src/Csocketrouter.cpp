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
* Description:  Routes data from peers to host and vice versa
*
*/



// INCLUDE FILES
#include "Csocketrouter.h"
#include "CommRouterDefinitions.h"
#include "MSocketRouterObserver.h"
#include "MBPProtocol.h"
#include "CProtocolTCP.h"
#include "CProtocolUDP.h"
#include "CLocalTCPConnection.h"
#include "CUDPSender.h"
#include "IPProxyEngine.pan"

#include <es_sock.h>


#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"

// LOCAL CONSTANTS
const TInt KSocketRouterSlots = 60;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSocketRouter::CSocketRouter
// -----------------------------------------------------------------------------
//
CSocketRouter::CSocketRouter( MSocketRouterObserver* aObserver )
    : CActive( EPriorityIdle ), iObserver( aObserver )
    {
    __ASSERT_DEBUG( iObserver, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CSocketRouter::ConstructL
// -----------------------------------------------------------------------------
//
void CSocketRouter::ConstructL()
    {
    User::LeaveIfError( iSocketServ.Connect( KSocketRouterSlots ) );

    iReceiveBuffer = HBufC8::NewL( 0 );

    iWriteEventArray = new (ELeave) CArrayPtrSeg<CWriteEvent> ( 10 );
    iPeerSocketArray = new (ELeave) CArrayPtrFlat<CSocket> ( 10 );
    iProtocolArray = new (ELeave) CArrayPtrFlat<MBPProtocol> ( 2 );

    // Add the protocols
    iProtocolTCP = CProtocolTCP::NewL( this );
    // takes the ownership
    iProtocolArray->AppendL( iProtocolTCP );

    iProtocolUDP = CProtocolUDP::NewL( this );
    // takes the ownership
    iProtocolArray->AppendL( iProtocolUDP );

    iUDPSender = CUDPSender::NewL( this );

    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSocketRouter::NewL
// -----------------------------------------------------------------------------
//
CSocketRouter* CSocketRouter::NewL( MSocketRouterObserver* aObserver )
    {
    CSocketRouter* self = CSocketRouter::NewLC( aObserver );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CSocketRouter::NewLC
// -----------------------------------------------------------------------------
//
CSocketRouter* CSocketRouter::NewLC( MSocketRouterObserver* aObserver )
    {
    CSocketRouter* self = new( ELeave ) CSocketRouter( aObserver );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }


// Destructor
CSocketRouter::~CSocketRouter()
    {
    Cancel();

    delete iUDPSender;
    delete iReceiveBuffer;

    if ( iProtocolArray )
        {
        iProtocolArray->ResetAndDestroy();
        delete iProtocolArray;
        }
    if ( iPeerSocketArray )
        {
        iPeerSocketArray->ResetAndDestroy();
        delete iPeerSocketArray;
        }
    if ( iWriteEventArray )
        {
        iWriteEventArray->ResetAndDestroy();
        delete iWriteEventArray;
        }

    iSocketServ.Close();
    }

// -----------------------------------------------------------------------------
// CSocketRouter::AddPeerSocketL
// -----------------------------------------------------------------------------
//
void CSocketRouter::AddPeerSocketL( RSocket* aSocket )
    {
    CSocket* newSocket = CSocket::NewLC( aSocket );
    newSocket->SetObserver( this );
    iPeerSocketArray->AppendL( newSocket );
    newSocket->SetSocketOwnershipMode( ETrue );

    DEBUG_PRINT( DEBUG_STRING(
        "CSocketRouter::AddPeerSocketL(), localport=%d, remote port=%d" ),
        newSocket->LocalPort(),
        newSocket->RemotePort() );

    CleanupStack::Pop( newSocket );
    }


// -----------------------------------------------------------------------------
// CSocketRouter::AddUDPSocketL
// -----------------------------------------------------------------------------
//
void CSocketRouter::AddUDPSocketL( TUint aPort )
    {
    RSocket* newSocket = new (ELeave) RSocket();
    CleanupStack::PushL( newSocket );

    // Open the socket
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::AddUDPSocketL, Trying to open UDP port %d" ), aPort );

    User::LeaveIfError( newSocket->Open( iSocketServ, KAfInet, KSockDatagram,
        KProtocolInetUdp  ) );

    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::AddUDPSocketL, UDP port opened." ), aPort );

    // Bind the  socket to the correct port.
    TInetAddr anyAddrOnPort( KInetAddrAny, aPort );
    newSocket->Bind( anyAddrOnPort );

    CSocket* cSocket = CSocket::NewLC( newSocket, aPort );
    cSocket->SetObserver( this );
    iPeerSocketArray->AppendL( cSocket );
    cSocket->SetSocketOwnershipMode( ETrue );
    CleanupStack::Pop( cSocket );

    CleanupStack::Pop( newSocket );
    }

// -----------------------------------------------------------------------------
// CSocketRouter::RemovePeerSocket
// -----------------------------------------------------------------------------
//
void CSocketRouter::RemovePeerSocket( TInt aIndex )
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::RemovePeerSocket()" ) );
    CSocket* socket = iPeerSocketArray->At( aIndex );
    iPeerSocketArray->Delete( aIndex );
    delete socket;
    }

// -----------------------------------------------------------------------------
// CSocketRouter::RemovePeerSocket
// -----------------------------------------------------------------------------
//
void CSocketRouter::RemovePeerSocket( RSocket* aSocket )
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::RemovePeerSocket()" ) );
    TInt count = iPeerSocketArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iPeerSocketArray->At( i )->GetRSocket() == aSocket )
            {
            iPeerSocketArray->Delete( i );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSocketRouter::FindPeerSocket
// -----------------------------------------------------------------------------
//
TInt CSocketRouter::FindPeerSocket( TUint aPort )
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::FindPeerSocket()" ) );
    TInt count = iPeerSocketArray->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iPeerSocketArray->At( i )->RemotePort() == aPort 
                || iPeerSocketArray->At( i )->LocalPort() == aPort)
            {
            return i;
            }
        }
    return -1;
    }

// -----------------------------------------------------------------------------
// CSocketRouter::RemoveAllPeers
// -----------------------------------------------------------------------------
//
void CSocketRouter::RemoveAllPeers()
    {
    iPeerSocketArray->ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSocketRouter::SocketCount
// -----------------------------------------------------------------------------
//
TInt CSocketRouter::SocketCount() const
    {
    return iPeerSocketArray->Count();
    }

// -----------------------------------------------------------------------------
// CSocketRouter::SetHostSocketL
// -----------------------------------------------------------------------------
//
void CSocketRouter::SetHostSocketL( MSocket* aSocket )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CSocketRouter::SetHostSocketL()" ) );

    iHostSocket = aSocket;
    iHostSocket->SetObserver( this );
    }


// -----------------------------------------------------------------------------
// CSocketRouter::RunL
// -----------------------------------------------------------------------------
//
void CSocketRouter::RunL()
    {
    WriteQueueL();
    }

// -----------------------------------------------------------------------------
// CSocketRouter::DoCancel
// -----------------------------------------------------------------------------
//
void CSocketRouter::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CSocketRouter::StartRouting
// -----------------------------------------------------------------------------
//
void CSocketRouter::StartRouting()
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::StartRouting()" ) );

    iRouting = ETrue;
    iHostSocket->IssueRead();
    TInt peerSocketArrayCount = iPeerSocketArray->Count();
    for ( TInt i = 0;i < peerSocketArrayCount; i++ )
        {
        CSocket* socket = iPeerSocketArray->At( i );
        socket->IssueRead();
        }
    IssueHandleQueue();
    }

// -----------------------------------------------------------------------------
// CSocketRouter::IssueHandleQueue
// -----------------------------------------------------------------------------
//
void CSocketRouter::IssueHandleQueue()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );

        SetActive();
        }
    }


// -----------------------------------------------------------------------------
// CSocketRouter::StopRouting
// -----------------------------------------------------------------------------
//
void CSocketRouter::StopRouting()
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::StopRouting()" ) );

    Cancel();   //If HandleQueue is issued

    iRouting = EFalse;
    if ( iHostSocket )
        {
        iHostSocket->Cancel();
        }
    TInt peerSocketArrayCount = iPeerSocketArray->Count();
    for ( TInt i = 0;i < peerSocketArrayCount; i++ )
        {
        CSocket* socket = iPeerSocketArray->At( i );
        socket->Cancel();
        }
    }


// -----------------------------------------------------------------------------
// CSocketRouter::IsRouting
// -----------------------------------------------------------------------------
//
TBool CSocketRouter::IsRouting() const
    {
    return iRouting;
    }

// -----------------------------------------------------------------------------
// CSocketRouter::ResetQueue
// -----------------------------------------------------------------------------
//
void CSocketRouter::ResetQueue()
    {
    iWriteEventArray->ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSocketRouter::SendCloseTCPConnection
// -----------------------------------------------------------------------------
//
void CSocketRouter::SendCloseTCPConnection( TUint aPort )
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::SendCloseTCPConnection" ) );

    iProtocolTCP->SendCloseTCPConnection( *iHostSocket, aPort );
    }

// -----------------------------------------------------------------------------
// CSocketRouter::WriteCorrectFrame
// -----------------------------------------------------------------------------
//
void CSocketRouter::WriteCorrectFrameL( TProtocolDesc aProtocolDesc,
                        TUint aPeerPort,
                        TUint aOriginalPort,
                        const TDesC8& aData )
    {
    DEBUG_PRINT( DEBUG_STRING(
            "CSocketRouter::WriteCorrectFrameL(), protocol = %d" ),
            aProtocolDesc.iProtocol );

    MBPProtocol* protocol;

    if ( aProtocolDesc.iProtocol == KProtocolInetUdp )
        {
        // UDP frame
        DEBUG_PRINT( DEBUG_STRING(
            "CSocketRouter::WriteCorrectFrameL(), UDP" ) );
        protocol = iProtocolUDP;
        }
    else
        {
        // TCP frame
        DEBUG_PRINT( DEBUG_STRING(
            "CSocketRouter::WriteCorrectFrameL(), TCP" ) );
        protocol = iProtocolTCP;
        }

    protocol->WriteFrameL(
        *iHostSocket, aPeerPort,
        aOriginalPort, aData );
    }

// -----------------------------------------------------------------------------
// CSocketRouter::DataReceivedL
// -----------------------------------------------------------------------------
//
void CSocketRouter::DataReceivedL( const MSocket* aSocket, const TDesC8& aData )
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::DataReceivedL()" ) );

    __ASSERT_ALWAYS( aSocket, Panic( IPProxyEngineNullSocket ) );

    if ( iRouting )
        {
        //Route all data received from the host socket to protocol data handler.
        if ( aSocket == iHostSocket )
            {
            // join the previous buffer if there's something left
            TInt neededLength = iReceiveBuffer->Length() + aData.Length();
            HBufC8* joinedBuffers = HBufC8::NewLC( neededLength );
            TPtr8 joinedBuffersPtr( joinedBuffers->Des() );
            joinedBuffersPtr.Append( *iReceiveBuffer );
            joinedBuffersPtr.Append( aData );

            // Check if there's a protocol that can handle this message
            TInt length = 0; // length of the full msg
            TInt startPos = 0; // where the msg prefix starts
            TBool msgHandled = EFalse;
            TInt protCount = iProtocolArray->Count();

            for ( TInt i = 0; i < protCount && !msgHandled; i++ )
                {
                msgHandled = iProtocolArray->At( i )->HandleReceivedDataL(
                                                        joinedBuffersPtr,
                                                        startPos,
                                                        length  );
                }
            if ( msgHandled )
                {
                if ( length > 0 )
                    {
                    // Length is set only when whole msg is processed
                    // and correct.
                    // Store the rest of the msg for the next round
                    delete iReceiveBuffer;
                    iReceiveBuffer = NULL;
                    iReceiveBuffer =
                            joinedBuffers->Mid( startPos + length ).AllocL();
                    CleanupStack::PopAndDestroy( joinedBuffers );

                    if ( iReceiveBuffer->Length() > 0 )
                        {
                        // Start again to process the rest
                        iState = EStateWaitingFrameStart;
                        DataReceivedL( aSocket, KNullDesC8 );
                        }
                    }
                else
                    {
                    delete iReceiveBuffer;
                    iReceiveBuffer = joinedBuffers;
                    CleanupStack::Pop( joinedBuffers );
                    }
                }
            else
                {
                // Could not find a valid prefix for this protocol
                // store data for the next round
                HBufC8* newBuffer = HBufC8::NewLC( KMaxPrefixLength -1 );
                TPtr8 newBufferPtr( newBuffer->Des() );
                if ( joinedBuffersPtr.Length() > 3 )
                    {
                    newBufferPtr =
                        joinedBuffersPtr.Right( KMaxPrefixLength - 1 );
                    }
                else
                    {
                    iState = EStateWaitingFrameStart;
                    newBufferPtr = joinedBuffersPtr;
                    }
                delete iReceiveBuffer;
                iReceiveBuffer = newBuffer;
                CleanupStack::Pop( newBuffer );
                CleanupStack::PopAndDestroy( joinedBuffers );
                return;
                }
            }
        else    //Route all data received from any other socket to frame writer.
            {
            WriteQueueL();
            TProtocolDesc info;
            aSocket->SocketInfo( info );
            WriteCorrectFrameL(
                info, aSocket->RemotePort(),
                aSocket->LocalPort(), aData );

            }
        }
    else
        {
        //Add to queue
        TProtocolDesc desc;
        aSocket->SocketInfo( desc );
        iWriteEventArray->AppendL( CWriteEvent::NewLC( desc,
            aData, aSocket->RemotePort(), aSocket->LocalPort() ) );

        DEBUG_PRINT( DEBUG_STRING(
            "Add to queue: peerPort: %d. origPort: %d" ),
            aSocket->RemotePort(),
            aSocket->LocalPort() );

        CleanupStack::Pop(); // CWriteEvent
        }
    }

// -----------------------------------------------------------------------------
// CSocketRouter::WriteQueueL
// -----------------------------------------------------------------------------
//
void CSocketRouter::WriteQueueL()
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::WriteQueueL()" ) );

    TInt writeEventArrayCount = iWriteEventArray->Count();
    for ( TInt i = 0; i < writeEventArrayCount; i++ )
        {
        const CWriteEvent* writeEvent = iWriteEventArray->At( 0 );
        WriteCorrectFrameL( writeEvent->ProtocolDesc(), writeEvent->PeerPort(),
            writeEvent->OriginalPort(), writeEvent->Data() );
        DEBUG_PRINT( DEBUG_STRING(
            "WriteQueueL(): peerPort: %d. origPort: %d" ),
            writeEvent->PeerPort(),
            writeEvent->OriginalPort() );
        iWriteEventArray->Delete( 0 );
        delete writeEvent;
        }
    }

// -----------------------------------------------------------------------------
// CSocketRouter::ErrorL
// -----------------------------------------------------------------------------
//
void CSocketRouter::ErrorL( const MSocket* aSocket, TInt aErrorCode )
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::ErrorL()" ) );

    iObserver->SocketRouterErrorL( aSocket, aErrorCode );
    }

// -----------------------------------------------------------------------------
// CSocketRouter::ObserverLeaved
// -----------------------------------------------------------------------------
//
void CSocketRouter::ObserverLeaved( const MSocket* aSocket, TInt aLeaveCode )
    {
    iObserver->ObserverLeaved( aSocket, aLeaveCode );
    }

// -----------------------------------------------------------------------------
// CSocketRouter::DisconnectedL
// -----------------------------------------------------------------------------
//
void CSocketRouter::DisconnectedL( const MSocket* aSocket )
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::DisconnectedL()" ) );

    if ( aSocket == iHostSocket )
        {
        DEBUG_PRINT( DEBUG_STRING( "aSocket == iHostSocket" ) );
        iObserver->HostDisconnectedL( aSocket );
        iHostSocket = NULL;
        }
    else
        {
        TInt peerSocketArrayCount = iPeerSocketArray->Count();
        for ( TInt i = 0;i < peerSocketArrayCount; i++ )
            {
            CSocket* socket = iPeerSocketArray->At( i );
            if ( socket == aSocket )
                {
                SendCloseTCPConnection( socket->RemotePort() );
                iObserver->PeerDisconnectedL( socket );
                RemovePeerSocket( i );
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CSocketRouter::FrameStarted
// -----------------------------------------------------------------------------
//
void CSocketRouter::FrameStarted()
    {
    iState = EStateFrameStartFound;
    }

// -----------------------------------------------------------------------------
// CSocketRouter::TCPFrameParsedL
// -----------------------------------------------------------------------------
//
void CSocketRouter::TCPFrameParsedL( TUint aPort, const TDesC8& aData )
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::TCPFrameParsedL()" ) );
    DEBUG_PRINT( DEBUG_STRING( "    aPort=%d" ), aPort );
    DEBUG_PRINT( DEBUG_STRING( "    aData.Length=%d" ), aData.Length() );

    for ( TInt i = 0; i < iPeerSocketArray->Count(); i++ )
        {
        CSocket* socket = iPeerSocketArray->At( i );
        if ( socket->RemotePort() == aPort && !socket->IsUDP() )
            {
            socket->WriteL( aData );
            break; //for
            }
        }
    }

// -----------------------------------------------------------------------------
// CSocketRouter::OpenLocalTCPConnectionL
// -----------------------------------------------------------------------------
//
void CSocketRouter::OpenLocalTCPConnectionL( TUint aPort )
    {
    DEBUG_PRINT( DEBUG_STRING( "OpenLocalTCPConnectionL, port = %d" ), aPort );
    if ( iObserver )
        {
        iObserver->OpenLocalTCPConnectionL( aPort );
        }
    }

// -----------------------------------------------------------------------------
// CSocketRouter::OpenListeningTCPConnectionL
// -----------------------------------------------------------------------------
//
void CSocketRouter::OpenListeningTCPConnectionL( TUint aPort )
    {
    DEBUG_PRINT( DEBUG_STRING( "OpenListeningTCPConnectionL, port = %d" ), aPort );
    if ( iObserver )
        {
        iObserver->OpenListeningTCPConnectionL( aPort );
        }
    }

// -----------------------------------------------------------------------------
// CSocketRouter::CloseTCPConnection
// -----------------------------------------------------------------------------
//
void CSocketRouter::CloseTCPConnectionL( TUint aPort )
    {
    DEBUG_PRINT( DEBUG_STRING( "CloseTCPConnection, port = %d" ), aPort );
    
    // Close the socket if it exists
    CArrayPtr<CSocket>* socketArrayForDestroy = new (ELeave) CArrayPtrFlat<CSocket> ( 10 );
    CleanupStack::PushL(socketArrayForDestroy);
    TInt index = FindPeerSocket( aPort );
    while ( index > -1 )
        {
        socketArrayForDestroy->AppendL(iPeerSocketArray->At( index ));
        iPeerSocketArray->Delete(index);
        index = FindPeerSocket( aPort );
        }
    socketArrayForDestroy->ResetAndDestroy();
    CleanupStack::PopAndDestroy(socketArrayForDestroy);

    if ( iObserver )
        {
        iObserver->CloseTCPConnection( aPort );
        }
    }

// -----------------------------------------------------------------------------
// CSocketRouter::CloseAllTCPConnections
// -----------------------------------------------------------------------------
//
void CSocketRouter::CloseAllTCPConnections()
    {
    DEBUG_PRINT( DEBUG_STRING( "CloseAllTCPConnections" ) );
    if ( iObserver )
        {
        iObserver->CloseAllTCPConnections();
        }
    }

// -----------------------------------------------------------------------------
// CSocketRouter::UDPFrameParsedL
// -----------------------------------------------------------------------------
//
void CSocketRouter::UDPFrameParsedL( TUint aPort, const TDesC8& aData )
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::UDPFrameParsedL()" ) );
    DEBUG_PRINT( DEBUG_STRING( "    aPort=%d" ), aPort );
    DEBUG_PRINT( DEBUG_STRING( "    aData.Length=%d" ), aData.Length() );
    iUDPSender->IssueWriteL( aPort, aData );
    }

// -----------------------------------------------------------------------------
// CSocketRouter::ProtocolErrorL
// -----------------------------------------------------------------------------
//
void CSocketRouter::ProtocolErrorL(
    TInt aErrorCode, const TDesC8& /* aReceivedData */ )
    {
    DEBUG_PRINT( DEBUG_STRING( "CSocketRouter::ProtocolErrorL()" ) );
    DEBUG_PRINT( DEBUG_STRING( "    aErrorCode=%d" ), aErrorCode );

    if ( iObserver )
        {
        iObserver->SocketRouterErrorL( iHostSocket, aErrorCode );
        }
    }

// -----------------------------------------------------------------------------
// CSocketRouter::UDPSenderErrorL
// -----------------------------------------------------------------------------
//
void CSocketRouter::UDPSenderErrorL( TInt aErrorCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CSocketRouter::UDPSenderErrorL(), error code %d" ), aErrorCode );
    iObserver->SocketRouterErrorL( NULL, aErrorCode );
    }

// -----------------------------------------------------------------------------
// CSocketRouter::UDPSenderLeavedL
// -----------------------------------------------------------------------------
//
void CSocketRouter::UDPSenderLeavedL( TInt aLeaveCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CSocketRouter::UDPSenderLeavedL(), leavecode %d" ), aLeaveCode );
    iObserver->ObserverLeaved( NULL, aLeaveCode );
    }

//  End of File
