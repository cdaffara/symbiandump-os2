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
* Description:  Writes to socket.
*
*/


// INCLUDE FILES
#include "socketswriter.h"
#include "timeouttimer.h"
#include "BtSerialEngine.pan"
#include "SocketObserver.h"
#include "HtiBtEngineLogging.h"

// STATIC MEMBER INITIALISATIONS
const TInt KTimeOut = 10 * 1000 * 1000; // 10 seconds time-out

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
CSocketsWriter* CSocketsWriter::NewL( MSocketObserver& aEngineNotifier,
                                      RSocket& aSocket )
    {
    CSocketsWriter* self = new ( ELeave ) CSocketsWriter( aEngineNotifier,
                                                          aSocket );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
CSocketsWriter::CSocketsWriter( MSocketObserver& aEngineNotifier,
                                RSocket& aSocket )
: CActive( EPriorityStandard ),
  iSocket( aSocket ),
  iObserver( aEngineNotifier )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
void CSocketsWriter::ConstructL()
    {
    CActiveScheduler::Add( this );
    iTimeOut = KTimeOut;
    iTimer = CTimeOutTimer::NewL( CActive::EPriorityUserInput, *this );
    iWriteStatus = EIdle;
    }

// -----------------------------------------------------------------------------
CSocketsWriter::~CSocketsWriter()
    {
    CancelSending();
    delete iTimer;
    }

// -----------------------------------------------------------------------------
void CSocketsWriter::DoCancel()
    {
    // Cancel asychronous write request
    iSocket.CancelWrite();
    iTimer->Cancel();
    iWriteStatus = EIdle;
    }

// -----------------------------------------------------------------------------
void CSocketsWriter::RunL()
    {
    iTimer->Cancel();

    // Active object request complete handler
    if ( iStatus == KErrNone )
        {
        switch ( iWriteStatus )
            {
            // Character has been written to socket
            case ESending:
                SendNextPacket();
                break;
            default:
                LOGFMT_E( "CSocketsWriter: Bad write status: %d", iWriteStatus )
                Panic( EBTSerialEngineWriteSocketBadState );
                break;
            };
        }
    else
        {
        iWriteStatus = EIdle;
        iWriteBuffer.Zero();
        iTransferBuffer.Zero();
        iObserver.ReportError( MSocketObserver::EGeneralWriteError,
                                     iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
void CSocketsWriter::TimerExpired()
    {
    Cancel();
    iWriteStatus = EIdle;
    iObserver.ReportError( MSocketObserver::ETimeOutOnWrite,
                                 KErrTimedOut );
    }

// -----------------------------------------------------------------------------
void CSocketsWriter::SendL(const TDesC8& aData)
    {
    if ( aData.Length() > FreeSpaceInSendBuffer() )
        User::Leave( KErrOverflow );

    iTransferBuffer.Append( aData );
    SendNextPacket();
    }

// -----------------------------------------------------------------------------
TInt CSocketsWriter::FreeSpaceInSendBuffer()
    {
    return iTransferBuffer.MaxSize() - iTransferBuffer.Size();
    }

// -----------------------------------------------------------------------------
TInt CSocketsWriter::SendBufferMaxSize()
    {
    return iTransferBuffer.MaxLength();
    }

// -----------------------------------------------------------------------------
void CSocketsWriter::SendNextPacket()
    {
    if ( IsActive() )
        {
        return; // already sending
        }
    if ( iTransferBuffer.Length() > 0 )
        {
        // Move data from transfer buffer to actual write buffer
        iWriteBuffer.Copy(iTransferBuffer.Left(iWriteBuffer.MaxLength()));
        iTransferBuffer.Delete(0, iWriteBuffer.MaxLength());
        iSocket.Write( iWriteBuffer, iStatus ); // Initiate actual write

        iWriteStatus = ESending;
        iTimer->After( iTimeOut );
        SetActive();
        }
    else
        {
        iWriteStatus = EIdle;
        iObserver.AllBufferedDataSent();
        }
    }

// -----------------------------------------------------------------------------
void CSocketsWriter::CancelSending()
    {
    Cancel();
    iWriteBuffer.Zero();
    iTransferBuffer.Zero();
    }
// End of File
