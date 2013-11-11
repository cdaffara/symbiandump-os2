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
* Description:  Socket writer
*
*/



// INCLUDE FILES
#include "CSocketWriter.h"
#include "MSocketWriterObserver.h"
#include <in_sock.h>
#include <badesca.h>

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"


const TInt KServerBusyWaiting = 200000;  //200 ms delay

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSocketWriter::CSocketWriter
// -----------------------------------------------------------------------------
//
CSocketWriter::CSocketWriter( RSocket& aSocket,
        TInt aUDPRemotePort /*= -1*/ ) :
    CActive( EPriorityStandard ),
    iSocket( aSocket ),
    iUDPRemotePort( aUDPRemotePort )
    {
    }

// -----------------------------------------------------------------------------
// CSocketWriter::ConstructL
// -----------------------------------------------------------------------------
//
void CSocketWriter::ConstructL()
    {
    iTransferBufferArray = new (ELeave) CDesC8ArraySeg( 10 );
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSocketWriter::NewL
// -----------------------------------------------------------------------------
//
CSocketWriter* CSocketWriter::NewL( RSocket& aSocket,
        TInt aUDPRemotePort /* = -1 */ )
    {
    CSocketWriter* self = CSocketWriter::NewLC( aSocket,
        aUDPRemotePort );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CSocketWriter::NewLC
// -----------------------------------------------------------------------------
//
CSocketWriter* CSocketWriter::NewLC( RSocket& aSocket,
        TInt aUDPRemotePort /* = -1 */ )
    {
    CSocketWriter* self = new( ELeave ) CSocketWriter( aSocket,
        aUDPRemotePort );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }


// Destructor
CSocketWriter::~CSocketWriter()
    {
    Cancel();
    iTimer.Close();
    delete iTransferBufferArray;
    }


// -----------------------------------------------------------------------------
// CSocketWriter::SetObserver
// -----------------------------------------------------------------------------
//
void CSocketWriter::SetObserver( MSocketWriterObserver* aObserver )
    {
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CSocketWriter::IssueWriteL
// -----------------------------------------------------------------------------
//
void CSocketWriter::IssueWriteL( const TDesC8& aData )
    {
    // slice the size of data if bigger than KWriteBufferSize

    TInt dsize = aData.Size();

    for ( TInt i = 0; i < dsize; i+=KWriteBufferSize )
        {
        if ( i + KWriteBufferSize - 1 >= dsize )
            {
            __ASSERT_DEBUG( i + aData.Mid( i ).Size() == dsize ,
                User::Panic( _L( "writer" ), 100 ) );
            iTransferBufferArray->AppendL( aData.Mid(i) );
            }
        else
            {
            iTransferBufferArray->AppendL( aData.Mid( i, KWriteBufferSize ) );
            }
        }

    if ( !IsActive() )
        {
        IssueWrite();
        }
    }

// -----------------------------------------------------------------------------
// CSocketWriter::IssueWrite
// -----------------------------------------------------------------------------
void CSocketWriter::IssueWrite()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CSocketWriter::IssueWrite" ) );
    iWriteBuffer = (*iTransferBufferArray)[ 0 ];

    TProtocolDesc desc;
    iSocket.Info( desc );

    if ( desc.iProtocol == KProtocolInetUdp && iUDPRemotePort > -1 )
        {
        // UDP
        DEBUG_PRINT( DEBUG_STRING(
            "CSocketWriter::IssueWrite(), UDP, remote port=%d" ),
            iUDPRemotePort );

        TInetAddr addrLocalHost( KInetAddrLoop, iUDPRemotePort );
        iSocket.SendTo( iWriteBuffer, addrLocalHost, 0, iStatus );
        }
    else
        {
        // TCP
        iSocket.Write( iWriteBuffer, iStatus );
        }

    SetActive();
    }

// -----------------------------------------------------------------------------
// CSocketWriter::ContinueAfterError
// -----------------------------------------------------------------------------
void CSocketWriter::ContinueAfterError()
    {
    if ( iTransferBufferArray->Count() > 0 )
        {
        IssueWrite();
        }
    }

// -----------------------------------------------------------------------------
// CSocketWriter::ResetTransferBuffer
// -----------------------------------------------------------------------------
void CSocketWriter::ResetTransferBuffer()
    {
    iTransferBufferArray->Reset();
    }

// -----------------------------------------------------------------------------
// CSocketWriter::RunL
// -----------------------------------------------------------------------------
//
void CSocketWriter::RunL()
    {
    TInt status = iStatus.Int();
    if ( status == KErrNone )
        {
        if ( iWaiting )
            {
            DEBUG_PRINT( DEBUG_STRING(
                "Trying to write again..." ) );

            iWaiting = EFalse;
            IssueWrite();
            }
        else
            {
            iTransferBufferArray->Delete( 0 );
            if ( iTransferBufferArray->Count() > 0 )
                {
                IssueWrite();
                }
            else
                {
                iObserver->BufferUnderrunL();
                }
            }
        }
    else
        {
        DEBUG_PRINT( DEBUG_STRING(
            "CSocketWriter::RunL(), iStatus=%d" ), status );
        iObserver->WriterErrorL( status );
        //If Socket server is busy, wait for a while and try again
        if ( status == KErrServerBusy )
            {
            DEBUG_PRINT( DEBUG_STRING(
                "Socket server busy. Waiting for a while..." ) );
            iWaiting = ETrue;
            iTimer.After( iStatus, KServerBusyWaiting );
            SetActive();
            }
        }
    }

// -----------------------------------------------------------------------------
// CSocketWriter::DoCancel
// -----------------------------------------------------------------------------
//
void CSocketWriter::DoCancel()
    {
    iTimer.Cancel();
    iSocket.CancelWrite();
    }

// -----------------------------------------------------------------------------
// CSocketWriter::RunError
// -----------------------------------------------------------------------------
//
TInt CSocketWriter::RunError( TInt aError )
    {
    iObserver->ObserverLeaved( aError );
    return KErrNone;
    }

