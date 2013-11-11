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
* Description:  Local TCP connection for emulator testing
*
*/



// INCLUDE FILES
#include "CLocalTCPConnection.h"
#include "MLocalTCPConnectionObserver.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLocalTCPConnection::CLocalTCPConnection
// -----------------------------------------------------------------------------
//
CLocalTCPConnection::CLocalTCPConnection(
    MLocalTCPConnectionObserver* aObserver, TInt aPort ) :
    CActive( EPriorityStandard )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalTCPConnection::CLocalTCPConnection() aPort=%d" ), aPort );

    iOwnsSocket = ETrue;
    iObserver = aObserver;
    iAddr.SetPort( aPort );
    iAddr.SetAddress( KInetAddrLoop );
    }

// -----------------------------------------------------------------------------
// CLocalTCPConnection::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLocalTCPConnection::ConstructL()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalTCPConnection::ConstructL, opening socket..." ) );

    iSocket = new (ELeave) RSocket;
    User::LeaveIfError( iSocketServer.Connect() );
    User::LeaveIfError( iSocket->Open(
        iSocketServer, KAfInet, KSockStream, KProtocolInetTcp ) );
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalTCPConnection::ConstructL, socket opened." ) );
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CLocalTCPConnection::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLocalTCPConnection* CLocalTCPConnection::NewL(
    MLocalTCPConnectionObserver* aObserver,
    TInt aPort  )
    {
    CLocalTCPConnection* self =
        CLocalTCPConnection::NewLC( aObserver, aPort );
    CleanupStack::Pop();

    return self;
    }

CLocalTCPConnection* CLocalTCPConnection::NewLC(
    MLocalTCPConnectionObserver* aObserver,
    TInt aPort  )
    {
    CLocalTCPConnection* self =
        new( ELeave ) CLocalTCPConnection( aObserver, aPort );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }

// Destructor
CLocalTCPConnection::~CLocalTCPConnection()
    {
    Cancel();

    if ( iOwnsSocket )
        {
        if ( iSocket )
            {
            iSocket->Close();
            delete iSocket;
            }
        }

    iSocketServer.Close();
    }

// -----------------------------------------------------------------------------
// CLocalTCPConnection::RunL
// -----------------------------------------------------------------------------
//
void CLocalTCPConnection::RunL()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalTCPConnection::RunL(), iStatus=%d" ),
        iStatus.Int() );

    if ( iStatus.Int() == KErrNone )
        {
        switch ( iState )
            {
            case ELTCStateConnecting:
                {
                DEBUG_PRINT( DEBUG_STRING(
                    "CLocalTCPConnection::RunL(), ELCStateConnecting" ) );
                iState = ELTCStateConnected;
                iObserver->LocalTCPConnectionEstablishedL( Port() );
                break;
                }
            case ELTCStateDisconnecting:
                {
                DEBUG_PRINT( DEBUG_STRING(
                    "CLocalTCPConnection::RunL(), ELCStateDisconnecting" ) );
                iState = ELTCStateDisconnected;

                iSocket->Close();
                User::LeaveIfError( iSocket->Open(
                    iSocketServer, KAfInet, KSockStream, KProtocolInetTcp ) );
                break;
                }
            }
        }
    else
        {
        iObserver->LocalTCPConnectionErrorL( Port(), iStatus.Int() );
        iSocket->Close();
        User::LeaveIfError( iSocket->Open(
            iSocketServer, KAfInet, KSockStream, KProtocolInetTcp ) );
        }
    }

// -----------------------------------------------------------------------------
// CLocalTCPConnection::DoCancel
// -----------------------------------------------------------------------------
//
void CLocalTCPConnection::DoCancel()
    {
    iSocket->CancelConnect();
    }

TInt CLocalTCPConnection::RunError( TInt aError )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalTCPConnection::RunError( %d )" ), aError );

    iObserver->LocalTCPConnectionObserverLeaved( Port(), aError );
    return KErrNone;
    }

TInt CLocalTCPConnection::Port()
    {
    return iAddr.Port();
    }

void CLocalTCPConnection::IssueConnectL()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalTCPConnection::IssueConnectL()" ) );

    iSocket->Connect( iAddr, iStatus );
    SetActive();
    iState = ELTCStateConnecting;
    }

void CLocalTCPConnection::IssueDisconnect()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalTCPConnection::IssueDisconnect()" ) );
    iSocket->CancelAll();
    Cancel();
    iSocket->Shutdown( RSocket::ENormal, iStatus );
    SetActive();
    iState = ELTCStateDisconnecting;
    }

void CLocalTCPConnection::SetObserver( MLocalTCPConnectionObserver* aObserver )
    {
    iObserver = aObserver;
    }

RSocket* CLocalTCPConnection::Socket()
    {
    return iSocket;
    }

void CLocalTCPConnection::SetSocketOwnership( TBool aOwns )
    {
    iOwnsSocket = aOwns;
    }


TBool CLocalTCPConnection::IsConnected()
    {
    return ( iState == ELTCStateConnected || iState == ELTCStateDisconnecting );
    }

//  End of File
