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
* Description:  Listens TCP port
*
*/


// INCLUDE FILES
#include "Ctcpportlistener.h"
#include "MTCPPortListenerObserver.h"
#include <in_sock.h>

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"

// LOCAL CONSTANTS
const TInt KMaxConnectionsInQueue = 8;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTCPPortListener::CTCPPortListener
// -----------------------------------------------------------------------------
//
CTCPPortListener::CTCPPortListener( TInt aPort,
                                    MTCPPortListenerObserver* aObserver ) :
    CActive( EPriorityStandard ), iObserver( aObserver ), iPort( aPort )
    {
    __ASSERT_DEBUG( iObserver, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CTCPPortListener::ConstructL
// -----------------------------------------------------------------------------
//
void CTCPPortListener::ConstructL()
    {
    //Message slots needed is two per socket (read and write) + 1 for
    //synchronous requests.
    User::LeaveIfError(
        iSocketServer.Connect( 2 * KMaxConnectionsInQueue + 1 ) );

    User::LeaveIfError( iListeningSocket.Open(
        iSocketServer, KAfInet, KSockStream, KProtocolInetTcp ) );

    iAcceptedSocket = new (ELeave) RSocket;
    User::LeaveIfError( iAcceptedSocket->Open( iSocketServer ) );

    TSockAddr addr;
    addr.SetPort( iPort );
    User::LeaveIfError( iListeningSocket.Bind( addr ) );

    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CTCPPortListener::NewL
// -----------------------------------------------------------------------------
//
CTCPPortListener* CTCPPortListener::NewL( TInt aPort,
                                          MTCPPortListenerObserver* aObserver )
    {
    CTCPPortListener* self = CTCPPortListener::NewLC( aPort,
                                                      aObserver );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CTCPPortListener::NewLC
// -----------------------------------------------------------------------------
//
CTCPPortListener* CTCPPortListener::NewLC( TInt aPort,
                                           MTCPPortListenerObserver* aObserver )
    {
    CTCPPortListener* self = new( ELeave ) CTCPPortListener( aPort,
                                                             aObserver );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }


// Destructor
CTCPPortListener::~CTCPPortListener()
    {
    Cancel();
    iListeningSocket.Close();
    if ( iAcceptedSocket )
        {
        iAcceptedSocket->Close();
        delete iAcceptedSocket;
        }
    iSocketServer.Close();
    }

// -----------------------------------------------------------------------------
// CTCPPortListener::IssueListen
// -----------------------------------------------------------------------------
//
void CTCPPortListener::IssueListen()
    {
    __ASSERT_DEBUG( iAcceptedSocket, User::Invariant() );

    iListeningSocket.Listen( KMaxConnectionsInQueue );
    iListeningSocket.Accept( *iAcceptedSocket, iStatus );
    SetActive();

    DEBUG_PRINT( DEBUG_STRING(
        "CTCPPortListener::IssueListen completed, port %d" ), iPort );
    }

// -----------------------------------------------------------------------------
// CTCPPortListener::Port
// -----------------------------------------------------------------------------
//
TInt CTCPPortListener::Port() const
    {
    return iPort;
    }

// -----------------------------------------------------------------------------
// CTCPPortListener::RunL
// -----------------------------------------------------------------------------
//
void CTCPPortListener::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        // Allocate new RSocket first
        RSocket* next = new ( ELeave ) RSocket;
        CleanupStack::PushL( next );
        User::LeaveIfError( next->Open( iSocketServer ) );
        CleanupClosePushL( *next );

        // temp will be passed to observer
        RSocket* temp = iAcceptedSocket;

        // Takes ownership of temp immediately
        iObserver->ConnectionAcceptedL( temp );

        CleanupStack::Pop();        // *next
        CleanupStack::Pop( next );
        iAcceptedSocket = next;

        iListeningSocket.Accept( *iAcceptedSocket, iStatus );
        SetActive();
        DEBUG_PRINT( DEBUG_STRING(
            "CTCPPortListener::RunL(), connection accepted, port %d" ), iPort );
        }
    }

// -----------------------------------------------------------------------------
// CTCPPortListener::DoCancel
// -----------------------------------------------------------------------------
//
void CTCPPortListener::DoCancel()
    {
    iListeningSocket.CancelAccept();
    }

// -----------------------------------------------------------------------------
// CTCPPortListener::RunError
// -----------------------------------------------------------------------------
//
TInt CTCPPortListener::RunError( TInt aError )
    {
    iObserver->ObserverLeaved( aError );
    return KErrNone;
    }


//  End of File
