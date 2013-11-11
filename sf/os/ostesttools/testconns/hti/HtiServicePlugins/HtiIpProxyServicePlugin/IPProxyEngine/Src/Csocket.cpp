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
* Description:  General purpose socket implementetion
*
*/



// INCLUDE FILES
#include "Csocket.h"
#include <es_sock.h>
#include <in_sock.h>
#include "CSocketWriter.h"
#include "Csocketreader.h"
#include "MSocketObserver.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSocket::CSocket
// -----------------------------------------------------------------------------
//
CSocket::CSocket( RSocket* aSocket, TInt aUDPRemotePort /*= -1*/ ) :
    iSocket( aSocket ),
    iHasOwnership( EFalse ),
    iUDPRemotePort( aUDPRemotePort )
    {
    }

// -----------------------------------------------------------------------------
// CSocket::ConstructL
// -----------------------------------------------------------------------------
//
void CSocket::ConstructL()
    {
    iSocketWriter = CSocketWriter::NewL( *iSocket, iUDPRemotePort );
    iSocketWriter->SetObserver( this );
    iSocketReader = CSocketReader::NewL( *iSocket, iUDPRemotePort );
    iSocketReader->SetObserver( this );
    }

// -----------------------------------------------------------------------------
// CSocket::NewL
// -----------------------------------------------------------------------------
//
CSocket* CSocket::NewL( RSocket* aSocket, TInt aUDPRemotePort /*= -1*/ )
    {
    CSocket* self = CSocket::NewLC( aSocket, aUDPRemotePort );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CSocket::NewLC
// -----------------------------------------------------------------------------
//
CSocket* CSocket::NewLC( RSocket* aSocket, TInt aUDPRemotePort /*= -1*/ )
    {
    CSocket* self = new( ELeave ) CSocket( aSocket, aUDPRemotePort );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }


// Destructor
CSocket::~CSocket()
    {
    delete iSocketWriter;
    delete iSocketReader;
    if ( iHasOwnership && iSocket )
        {
        TRequestStatus status;
        iSocket->Shutdown( RSocket::EImmediate, status );
        User::WaitForRequest( status );
        iSocket->Close();
        delete iSocket;
        }
    }


// -----------------------------------------------------------------------------
// CSocket::WriteL
// -----------------------------------------------------------------------------
//
void CSocket::WriteL( const TDesC8& aData )
    {
    iSocketWriter->IssueWriteL( aData );
    }

// -----------------------------------------------------------------------------
// CSocket::GetRSocket
// -----------------------------------------------------------------------------
//
RSocket* CSocket::GetRSocket()
    {
    return iSocket;
    }

// -----------------------------------------------------------------------------
// CSocket::RemotePort
// -----------------------------------------------------------------------------
//
TUint CSocket::RemotePort() const
    {
    if ( IsUDP() )
        {
        return iUDPRemotePort;
        }
    else
        {
        TSockAddr addr;
        iSocket->RemoteName( addr );
        return addr.Port();
        }

    }

// -----------------------------------------------------------------------------
// CSocket::LocalPort
// -----------------------------------------------------------------------------
//
TUint CSocket::LocalPort() const
    {
    return iSocket->LocalPort();
    }

// -----------------------------------------------------------------------------
// CSocket::SetObserver
// -----------------------------------------------------------------------------
//
void CSocket::SetObserver( MSocketObserver* aObserver )
    {
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CSocket::SetSocketOwnershipMode
// -----------------------------------------------------------------------------
//
void CSocket::SetSocketOwnershipMode( TBool aHasOwnership )
    {
    iHasOwnership = aHasOwnership;
    }

// -----------------------------------------------------------------------------
// CSocket::IssueRead
// -----------------------------------------------------------------------------
//
void CSocket::IssueRead()
    {
    iSocketReader->Start();
    }

// -----------------------------------------------------------------------------
// CSocket::Cancel
// -----------------------------------------------------------------------------
//
void CSocket::Cancel()
    {
    iSocketReader->Cancel();
    iSocketWriter->Cancel();
    }

// -----------------------------------------------------------------------------
// CSocket::DataReceivedL
// -----------------------------------------------------------------------------
//
void CSocket::SocketInfo( TProtocolDesc& aDesc) const
    {
    iSocket->Info( aDesc );
    }

// -----------------------------------------------------------------------------
// CSocket::IsUDP
// -----------------------------------------------------------------------------
//
TBool CSocket::IsUDP() const
    {
    TProtocolDesc desc;
    iSocket->Info( desc );
    if ( desc.iProtocol == KProtocolInetUdp && iUDPRemotePort > -1 )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CSocket::DataReceivedL
// -----------------------------------------------------------------------------
//
void CSocket::DataReceivedL( const TDesC8& aData )
    {
    iObserver->DataReceivedL( this, aData );
    }

// -----------------------------------------------------------------------------
// CSocket::BufferUnderrunL
// -----------------------------------------------------------------------------
//
void CSocket::BufferUnderrunL()
    {
    //No implementation
    }

// -----------------------------------------------------------------------------
// CSocket::WriterErrorL
// -----------------------------------------------------------------------------
//
void CSocket::WriterErrorL( TInt aErrorCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CSocket::WriterErrorL( %d )" ), aErrorCode );

    iObserver->ErrorL( this, aErrorCode );
    }

// -----------------------------------------------------------------------------
// CSocket::ReaderErrorL
// -----------------------------------------------------------------------------
//
void CSocket::ReaderErrorL( TInt aErrorCode )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CSocket::ReaderErrorL( %d ), UDP port=%d" ), aErrorCode, iUDPRemotePort );

    if ( aErrorCode == KErrEof || aErrorCode == KErrCancel ||
        aErrorCode == KErrDisconnected || aErrorCode == KErrNotReady )
        {
        iObserver->DisconnectedL( this );
        }
    else
        {
        iObserver->ErrorL( this, aErrorCode );
        }
    }


// -----------------------------------------------------------------------------
// CSocket::ObserverLeaved
// -----------------------------------------------------------------------------
//
void CSocket::ObserverLeaved( TInt aLeaveCode )
    {
    iObserver->ObserverLeaved( this, aLeaveCode );
    }


//  End of File
