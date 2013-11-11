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
* Description:  UDP packet sender
*
*/



// INCLUDE FILES
#include "CUDPSender.h"
#include "MUDPSenderObserver.h"

#include <badesca.h>

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"


const TInt KServerBusyWaiting = 200000;  //200 ms delay
const TInt KUDPSenderSlots = 50;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUDPSender::CUDPSender
// -----------------------------------------------------------------------------
//
CUDPSender::CUDPSender( MUDPSenderObserver* aObserver ) :
    CActive( EPriorityStandard ), iObserver( aObserver )
    {
    __ASSERT_DEBUG( iObserver, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CUDPSender::ConstructL
// -----------------------------------------------------------------------------
//
void CUDPSender::ConstructL()
    {
    User::LeaveIfError( iSocketServ.Connect( KUDPSenderSlots ) );

    // Create the socket
    User::LeaveIfError( iSocket.Open( iSocketServ, KAfInet, KSockDatagram,
        KProtocolInetUdp  ) );

    iRemoteAddr.SetAddress(KInetAddrLoop);
    
    iTransferBufferArray = new (ELeave) CDesC8ArraySeg( 10 );
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CUDPSender::NewL
// -----------------------------------------------------------------------------
//
CUDPSender* CUDPSender::NewL( MUDPSenderObserver* aObserver )
    {
    CUDPSender* self = CUDPSender::NewLC( aObserver );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CUDPSender::NewLC
// -----------------------------------------------------------------------------
//
CUDPSender* CUDPSender::NewLC( MUDPSenderObserver* aObserver )
    {
    CUDPSender* self = new( ELeave ) CUDPSender( aObserver );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }


// Destructor
CUDPSender::~CUDPSender()
    {
    Cancel();

    iSocket.Close();
    iSocketServ.Close();

    iTimer.Close();
    delete iTransferBufferArray;
    }

// -----------------------------------------------------------------------------
// CUDPSender::IssueWriteL
// -----------------------------------------------------------------------------
//
void CUDPSender::IssueWriteL( TUint aPort, const TDesC8& aData )
    {
    iRemoteAddr.SetPort(aPort);

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
// CUDPSender::IssueWrite
// -----------------------------------------------------------------------------
void CUDPSender::IssueWrite()
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CUDPSender::IssueWrite" ) );
    iWriteBuffer = (*iTransferBufferArray)[ 0 ];

    DEBUG_PRINT( DEBUG_STRING(
        "CSocketWriter::IssueWrite()" ) );

    iSocket.SendTo( iWriteBuffer, iRemoteAddr, 0, iStatus );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CUDPSender::ContinueAfterError
// -----------------------------------------------------------------------------
void CUDPSender::ContinueAfterError()
    {
    if ( iTransferBufferArray->Count() > 0 )
        {
        IssueWrite();
        }
    }

// -----------------------------------------------------------------------------
// CUDPSender::ResetTransferBuffer
// -----------------------------------------------------------------------------
void CUDPSender::ResetTransferBuffer()
    {
    iTransferBufferArray->Reset();
    }

// -----------------------------------------------------------------------------
// CUDPSender::RunL
// -----------------------------------------------------------------------------
//
void CUDPSender::RunL()
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
            }
        }
    else
        {
        DEBUG_PRINT( DEBUG_STRING(
            "CUDPSender::RunL(), iStatus=%d" ), status );
        iObserver->UDPSenderErrorL( status );
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
// CUDPSender::DoCancel
// -----------------------------------------------------------------------------
//
void CUDPSender::DoCancel()
    {
    iTimer.Cancel();
    iSocket.CancelWrite();
    }

// -----------------------------------------------------------------------------
// CUDPSender::RunError
// -----------------------------------------------------------------------------
//
TInt CUDPSender::RunError( TInt aError )
    {
    iObserver->UDPSenderLeavedL( aError );
    return KErrNone;
    }

