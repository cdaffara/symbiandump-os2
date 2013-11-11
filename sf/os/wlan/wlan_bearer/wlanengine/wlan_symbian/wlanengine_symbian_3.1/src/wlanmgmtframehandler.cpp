/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Active object to send and receive management packets (EAP packets).
*
*/

/*
* %version: 11 %
*/

// INCLUDE FILES
#include <e32std.h>
#include "wlanmgmtframehandler.h"
#include "am_debug.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlanMgmtFrameHandler::CWlanMgmtFrameHandler(
    RWlanLogicalChannel& aServiceProvider,
    MWlanMgmtFrameCallback& aClient ) :
    CActive( CActive::EPriorityHigh ),
    iServiceProvider( aServiceProvider ),
    iClient( aClient ),
    iDataBlock( NULL )
    {
    DEBUG( "CWlanMgmtFrameHandler::CWlanMgmtFrameHandler()" );
    }

// Static constructor.
CWlanMgmtFrameHandler* CWlanMgmtFrameHandler::NewL(
    RWlanLogicalChannel& aServiceProvider,
    MWlanMgmtFrameCallback& aClient)
    {
    DEBUG( "CWlanMgmtFrameHandler::NewL()" );
    CWlanMgmtFrameHandler* self =
        new (ELeave) CWlanMgmtFrameHandler( aServiceProvider, aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Symbian 2nd phase constructor can leave.
void CWlanMgmtFrameHandler::ConstructL()
    {
    DEBUG( "CWlanMgmtFrameHandler::ConstructL()" );

    CActiveScheduler::Add( this );
    }

// Destructor
CWlanMgmtFrameHandler::~CWlanMgmtFrameHandler()
    {
    DEBUG( "CWlanMgmtFrameHandler::~CWlanMgmtFrameHandler()" );

    Cancel();
    TInt ret = KErrNone;
    if( iServiceProvider.Handle() != KNullHandle )
        {
        ret = iServiceProvider.ReleaseBuffers();
        }    
    if( ret != KErrNone )
        {
        DEBUG1( "ReleaseBuffers failed, reason=%d", ret );
        }
    iDataBlock = NULL;
    }

// ---------------------------------------------------------
// CWlanMgmtFrameHandler::DoCancel
// Release synchronise call. Is called by CActive::Cancel()
// ---------------------------------------------------------
//
void CWlanMgmtFrameHandler::DoCancel()
    {
    DEBUG( "CWlanMgmtFrameHandler::DoCancel()" );
    iServiceProvider.CancelRxRequests();
    }

// ---------------------------------------------------------
// CWlanMgmtFrameHandler::IssueRequest
// Request a synchronous call for management packets.
// ---------------------------------------------------------
//
void CWlanMgmtFrameHandler::IssueRequest()
    {
    DEBUG( "CWlanMgmtFrameHandler::IssueRequest()" );
    iServiceProvider.RequestFrame( iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CWlanMgmtFrameHandler::Start
// ---------------------------------------------------------
//
TInt CWlanMgmtFrameHandler::Start()
    {
    DEBUG( "CWlanMgmtFrameHandler::Start()" );
    TInt ret = iServiceProvider.InitialiseBuffers( iDataBlock );
    if( ret != KErrNone || iDataBlock == NULL )
        {
        return ret;
        }
    IssueRequest();
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanMgmtFrameHandler::Stop
// ---------------------------------------------------------
//
void CWlanMgmtFrameHandler::Stop()
    {
    DEBUG( "CWlanMgmtFrameHandler::Stop()" );
    Cancel();
    TInt ret = iServiceProvider.ReleaseBuffers();
    if( ret != KErrNone )
        {
        DEBUG1( "ERROR: ReleaseBuffers failed with %d", ret );
        }
    }    

// ---------------------------------------------------------
// CWlanMgmtFrameHandler::RunL
// Synchronise call has finished. Send data to client.
// ---------------------------------------------------------
//
void CWlanMgmtFrameHandler::RunL()
    {
    DEBUG( "CWlanMgmtFrameHandler::RunL()" );

    // Send all received packets to the client.
    if ( iDataBlock == NULL )
        {
        DEBUG( "CWlanMgmtFrameHandler::RunL() iDataBlock=NULL" );
        }
    else
        {
        DEBUG( "CWlanMgmtFrameHandler::RunL() iDataBlock is not NULL" );
        TDataBuffer* buffer = NULL;
        TDataBuffer* frameToFree = NULL;
        while( ( buffer = iServiceProvider.GetRxFrame( frameToFree ) ) != NULL )
            {
            DEBUG1( "EAPOL packet received, length=%d", 
                    buffer->GetLength() );

            iClient.OnFrameReceive(
                buffer->FrameType(),
                buffer->GetLength(),
                buffer->GetBuffer(),
                buffer->Rcpi() );
            
            frameToFree = buffer;
            }
        }
    // Wait for next packets.
    IssueRequest();
    }

// ---------------------------------------------------------
// CWlanMgmtFrameHandler::SendFrame
// Send a management data frame.
// ---------------------------------------------------------
//
TInt CWlanMgmtFrameHandler::SendFrame(
    const TDataBuffer::TFrameType aFrameType,
    const TUint aLength,
    const TUint8* const aData,
    TUint8 aUserPriority,
    const TMacAddress& aDestination,
    TBool aSendUnencrypted )
    {
    DEBUG( "CWlanMgmtFrameHandler::SendFrame()" );

    // Check is iDataBlock ok.
    __ASSERT_ALWAYS( 
        iDataBlock != NULL, 
        User::Panic( 
            _L( "CWlanMgmtFrameHandler::SendFrame() - iDataBlock = NULL" ),
            KErrBadHandle));

    // Make sure that TX buffer is empty.
    iDataBlock->ClearTxDataBuffer();
    iDataBlock->AppendTxDataBuffer(
        aData,
        aLength,
        aFrameType,
        aUserPriority,
        aSendUnencrypted,
        &aDestination );

    // Make the call synchronous.
    TRequestStatus status;
    DEBUG7( "CWlanMgmtFrameHandler::SendFrame() - sending packet to %02X:%02X:%02X:%02X:%02X:%02X with priority %u",
        aDestination.iMacAddress[0], aDestination.iMacAddress[1], aDestination.iMacAddress[2],
        aDestination.iMacAddress[3], aDestination.iMacAddress[4], aDestination.iMacAddress[5],
        aUserPriority );
    iServiceProvider.WriteFrame( status );
    User::WaitForRequest( status );

    DEBUG1( "CWlanMgmtFrameHandler::SendFrame() - packet sent with status = %d",
        status.Int() );

    return status.Int();
    }
