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
* Description:  Reads from socket.
*
*/


// INCLUDE FILES
#include "BtSerialEngine.pan"
#include "socketsreader.h"
#include "SocketObserver.h"

// -----------------------------------------------------------------------------
CSocketsReader* CSocketsReader::NewL( MSocketObserver& aEngineNotifier,
                                      RSocket& aSocket )
    {
    CSocketsReader* self = new ( ELeave ) CSocketsReader( aEngineNotifier,
                                                          aSocket );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
CSocketsReader::CSocketsReader( MSocketObserver& aObserver,
                                RSocket& aSocket )
: CActive( EPriorityStandard ),
  iSocket( aSocket ),
  iObserver( aObserver )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
void CSocketsReader::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
CSocketsReader::~CSocketsReader()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
void CSocketsReader::DoCancel()
    {
    // Cancel asychronous read request
    iSocket.CancelRead();
    }

// -----------------------------------------------------------------------------
void CSocketsReader::RunL()
    {
    switch ( iStatus.Int() )
        {
        case KErrNone:
            iObserver.NewData(iBuffer);
//            IssueRead(); // Immediately start another read
            break;
        case KErrDisconnected:
            iObserver.ReportError( MSocketObserver::EDisconnected,
                                   iStatus.Int() );
            break;
        default:
            iObserver.ReportError( MSocketObserver::EGeneralReadError,
                                   iStatus.Int() );
            break;
        }
    }

// -----------------------------------------------------------------------------
void CSocketsReader::IssueRead()
    {
    // Initiate a new read from socket into iBuffer
    __ASSERT_ALWAYS( !IsActive(), Panic(EBTSerialEngineReadSocketBadState) );
    iSocket.RecvOneOrMore( iBuffer, 0, iStatus, iLen );
    SetActive();
    }

// -----------------------------------------------------------------------------
void CSocketsReader::ReadAsync()
    {
    // Initiate a new read from socket into iBuffer
    if (!IsActive())
        {
        IssueRead();
        }
    }

// End of File
