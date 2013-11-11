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
* Description:  Socket reader
*
*/



// INCLUDE FILES
#include "Csocketreader.h"
#include "MSocketReaderObserver.h"
#include <badesca.h>

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSocketReader::CSocketReader
// -----------------------------------------------------------------------------
//
CSocketReader::CSocketReader( RSocket& aSocket,
        TInt aUDPRemotePort /*= -1*/ ) :
    CActive( EPriorityStandard ),
    iSocket( aSocket ),
    iUDPRemotePort( aUDPRemotePort )
    {
    }

// -----------------------------------------------------------------------------
// CSocketReader::ConstructL
// -----------------------------------------------------------------------------
//
void CSocketReader::ConstructL()
    {
    CActiveScheduler::Add( this );
    IssueRead();
    }

// -----------------------------------------------------------------------------
// CSocketReader::NewL
// -----------------------------------------------------------------------------
//
CSocketReader* CSocketReader::NewL( RSocket& aSocket,
        TInt aUDPRemotePort /*= -1*/ )
    {
    CSocketReader* self = CSocketReader::NewLC( aSocket, aUDPRemotePort );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CSocketReader::NewLC
// -----------------------------------------------------------------------------
//
CSocketReader* CSocketReader::NewLC( RSocket& aSocket,
        TInt aUDPRemotePort /*= -1*/ )
    {
    CSocketReader* self = new( ELeave ) CSocketReader( aSocket, aUDPRemotePort );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }


// Destructor
CSocketReader::~CSocketReader()
    {
    Cancel();
    }


// -----------------------------------------------------------------------------
// CSocketReader::SetObserver
// -----------------------------------------------------------------------------
//
void CSocketReader::SetObserver( MSocketReaderObserver* aObserver )
    {
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CSocketReader::IssueRead
// -----------------------------------------------------------------------------
//
void CSocketReader::IssueRead()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CSocketReader::IssueRead +" ) );

    __ASSERT_ALWAYS( !IsActive() || iStatus != KRequestPending,
        User::Panic( _L ( "s-reader" ), 1 ) );

    TProtocolDesc desc;
    TInt err = iSocket.Info( desc );
    DEBUG_PRINT( DEBUG_STRING(
        "CSocketReader::IssueRead, err = %d" ), err );
    DEBUG_PRINT( DEBUG_STRING(
        "desc.iProtocol = %d, iUDPRemotePort=%d" ), desc.iProtocol, iUDPRemotePort );

    if ( desc.iProtocol == KProtocolInetUdp && iUDPRemotePort > -1 )
        {
        // UDP
        DEBUG_PRINT( DEBUG_STRING(
            "CSocketReader::IssueRead(), UDP, local port=%d" ), iUDPRemotePort );
        iSocket.RecvFrom( iReadBuffer, iUDPRemoteAddr, 0, iStatus );
        }
    else
        {
        // TCP
        DEBUG_PRINT( DEBUG_STRING(
            "CSocketReader::IssueRead(), TCP, local port=%d" ), iSocket.LocalPort() );
        iSocket.RecvOneOrMore( iReadBuffer, 0, iStatus, iReceivedDataLength );
        }

    SetActive();
    DEBUG_PRINT( DEBUG_STRING(
        "CSocketReader::IssueRead() -" ) );
    }

// -----------------------------------------------------------------------------
// CSocketReader::Start
// -----------------------------------------------------------------------------
//
void CSocketReader::Start()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CSocketReader::Start" ) );
    if ( !IsActive() )
        {
        IssueRead();
        }
    }
// -----------------------------------------------------------------------------
// CSocketReader::RunL
// -----------------------------------------------------------------------------
//
void CSocketReader::RunL()
    {
    TInt status = iStatus.Int();
    if ( status == KErrNone )
        {
        iObserver->DataReceivedL( iReadBuffer );
        iReadBuffer.SetLength( 0 );
        IssueRead();
        }
    else
        {
        iObserver->ReaderErrorL( status );
        }
    }

// -----------------------------------------------------------------------------
// CSocketReader::DoCancel
// -----------------------------------------------------------------------------
//
void CSocketReader::DoCancel()
    {
    iSocket.CancelRead();
    }

// -----------------------------------------------------------------------------
// CSocketReader::RunError
// -----------------------------------------------------------------------------
//
TInt CSocketReader::RunError( TInt aError )
    {
    iObserver->ObserverLeaved( aError );
    iReadBuffer.SetLength( 0 );
    IssueRead();
    return KErrNone;
    }

