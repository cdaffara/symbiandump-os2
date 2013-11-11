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
#include "CLocalHostConnection.h"
#include "MHostConnectionObserver.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLocalHostConnection::CLocalHostConnection
// -----------------------------------------------------------------------------
//
CLocalHostConnection::CLocalHostConnection(
    RSocketServ& aSocketServer,  TInt aPort ) :
    CActive( EPriorityStandard ),
    iSocketServer( aSocketServer )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalHostConnection::CLocalHostConnection() aPort=%d" ), aPort );

    iAddr.SetPort( aPort );
    iAddr.SetAddress( KInetAddrLoop );
    }

// -----------------------------------------------------------------------------
// CLocalHostConnection::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLocalHostConnection::ConstructL()
    {

    User::LeaveIfError( iClientSocket.Open(
        iSocketServer, KAfInet, KSockStream, KProtocolInetTcp ) );
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CLocalHostConnection::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLocalHostConnection* CLocalHostConnection::NewL(
    RSocketServ& aSocketServer,  TInt aPort  )
    {
    CLocalHostConnection* self =
        CLocalHostConnection::NewLC( aSocketServer, aPort );
    CleanupStack::Pop();

    return self;
    }

CLocalHostConnection* CLocalHostConnection::NewLC(
    RSocketServ& aSocketServer,  TInt aPort  )
    {
    CLocalHostConnection* self =
        new( ELeave ) CLocalHostConnection( aSocketServer, aPort );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }

// Destructor
CLocalHostConnection::~CLocalHostConnection()
    {
    Cancel();
    iClientSocket.Close();
    }

// -----------------------------------------------------------------------------
// CLocalHostConnection::RunL
// -----------------------------------------------------------------------------
//
void CLocalHostConnection::RunL()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalHostConnection::RunL(), iStatus=%d" ),
        iStatus.Int() );

    if ( iStatus.Int() == KErrNone )
        {
        switch ( iState )
            {
            case ELCStateConnecting:
                {
                DEBUG_PRINT( DEBUG_STRING(
                    "CLocalHostConnection::RunL(), ELCStateConnecting" ) );
                iState = ELCStateConnected;
                iObserver->ConnectionEstablishedL();
                break;
                }
            case ELCStateDisconnecting:
                {
                DEBUG_PRINT( DEBUG_STRING(
                    "CLocalHostConnection::RunL(), ELCStateDisconnecting" ) );
                iState = ELCStateDisconnected;

                iClientSocket.Close();
                User::LeaveIfError( iClientSocket.Open(
                    iSocketServer, KAfInet, KSockStream, KProtocolInetTcp ) );
                break;
                }
            }
        }
    else
        {
        iObserver->HostConnectionErrorL( iStatus.Int() );
        iClientSocket.Close();
        User::LeaveIfError( iClientSocket.Open(
            iSocketServer, KAfInet, KSockStream, KProtocolInetTcp ) );
        }
    }

// -----------------------------------------------------------------------------
// CLocalHostConnection::DoCancel
// -----------------------------------------------------------------------------
//
void CLocalHostConnection::DoCancel()
    {
    iClientSocket.CancelConnect();
    }

TInt CLocalHostConnection::RunError( TInt aError )
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalHostConnection::RunError( %d )" ), aError );

    iObserver->HostConnectionObserverLeaved( aError );
    return KErrNone;
    }

TInt CLocalHostConnection::Port()
    {
    return iAddr.Port();
    }

void CLocalHostConnection::IssueConnectL()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalHostConnection::IssueConnectL()" ) );

    iClientSocket.Connect( iAddr, iStatus );
    SetActive();
    iState = ELCStateConnecting;
    }

void CLocalHostConnection::IssueDisconnect()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CLocalHostConnection::IssueDisconnect()" ) );
    iClientSocket.CancelAll();
    Cancel();
    iClientSocket.Shutdown( RSocket::ENormal, iStatus );
    SetActive();
    iState = ELCStateDisconnecting;
    }

void CLocalHostConnection::SetObserver( MHostConnectionObserver* aObserver )
    {
    iObserver = aObserver;
    }

RSocket* CLocalHostConnection::Socket()
    {
    return &iClientSocket;
    }

TBool CLocalHostConnection::IsConnected()
    {
    return ( iState == ELCStateConnected || iState == ELCStateDisconnecting );
    }

//  End of File
