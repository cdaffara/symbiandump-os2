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
* Description:  HTTPProxy protocol implementation
*
*/



// INCLUDE FILES
#include "CProtocolTCP.h"
#include "CExprTCPMsg.h"
#include "CExprTCPOpen.h"
#include "CExprTCPLstn.h"
#include "CExprTCPClose.h"
#include "CExprTCPCloseAll.h"
#include "MSocketWriter.h"
#include "MProtocolObserverTCP.h"
#include "CommRouterDefinitions.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CProtocolTCP::CProtocolTCP
// -----------------------------------------------------------------------------
//
CProtocolTCP::CProtocolTCP( MProtocolObserverTCP* aObserver )
    : iObserver( aObserver )
    {
    __ASSERT_DEBUG( iObserver, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::ConstructL
// -----------------------------------------------------------------------------
//
void CProtocolTCP::ConstructL()
    {
    // Add all the expressions for TCP
    iExpressionArray = new (ELeave) CArrayPtrFlat<MBPExpression> ( 5 );

    CExprTCPMsg* tcpMsg = CExprTCPMsg::NewL( this );
    iExpressionArray->AppendL( tcpMsg );

    CExprTCPOpen* tcpOpen = CExprTCPOpen::NewL( this );
    iExpressionArray->AppendL( tcpOpen );

    CExprTCPLstn* tcpLstn = CExprTCPLstn::NewL( this );
    iExpressionArray->AppendL( tcpLstn );

    CExprTCPClose* tcpClose = CExprTCPClose::NewL( this );
    iExpressionArray->AppendL( tcpClose );

    CExprTCPCloseAll* tcpCloseAll = CExprTCPCloseAll::NewL( this );
    iExpressionArray->AppendL( tcpCloseAll );
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::NewL
// -----------------------------------------------------------------------------
//
CProtocolTCP* CProtocolTCP::NewL( MProtocolObserverTCP* aObserver )
    {
    CProtocolTCP* self = CProtocolTCP::NewLC( aObserver );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::NewLC
// -----------------------------------------------------------------------------
//
CProtocolTCP* CProtocolTCP::NewLC( MProtocolObserverTCP* aObserver )
    {
    CProtocolTCP* self = new( ELeave ) CProtocolTCP( aObserver );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CProtocolTCP::~CProtocolTCP
// -----------------------------------------------------------------------------
//
CProtocolTCP::~CProtocolTCP()
    {
    if ( iExpressionArray )
        {
        iExpressionArray->ResetAndDestroy();
        delete iExpressionArray;
        }
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::FrameStarted
// -----------------------------------------------------------------------------
//
void CProtocolTCP::FrameStarted()
    {
    iObserver->FrameStarted();
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::FrameParsedL
// -----------------------------------------------------------------------------
//
void CProtocolTCP::FrameParsedL( TUint aPort, const TDesC8& aData )
    {
    iObserver->TCPFrameParsedL( aPort, aData );
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::ProtocolErrorL
// -----------------------------------------------------------------------------
//
void CProtocolTCP::ProtocolErrorL(
    TInt aErrorCode, const TDesC8& aReceivedData )
    {
    iObserver->ProtocolErrorL( aErrorCode, aReceivedData );
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::OpenLocalTCPConnectionL
// -----------------------------------------------------------------------------
//
void CProtocolTCP::OpenLocalTCPConnectionL( TUint aPort )
    {
    iObserver->OpenLocalTCPConnectionL( aPort );
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::OpenListeningTCPConnectionL
// -----------------------------------------------------------------------------
//
void CProtocolTCP::OpenListeningTCPConnectionL( TUint aPort )
    {
    iObserver->OpenListeningTCPConnectionL( aPort );
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::CloseTCPConnection
// -----------------------------------------------------------------------------
//
void CProtocolTCP::CloseTCPConnectionL( TUint aPort )
    {
    iObserver->CloseTCPConnectionL( aPort );
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::ProtocolErrorL
// -----------------------------------------------------------------------------
//
void CProtocolTCP::CloseAllTCPConnections()
    {
    iObserver->CloseAllTCPConnections();
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::WriteFrameL
// -----------------------------------------------------------------------------
//
void CProtocolTCP::WriteFrameL( MSocketWriter& aSocketWriter,
            TUint aPeerPort, TUint aOriginalPort, const TDesC8& aData ) const
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CProtocolTCP::WriteFrameL(), peer port=%d, original port=%d" ),
        aPeerPort, aOriginalPort );

    TInt bufSize = 0;
    bufSize += KTCPPrefix().Length();
    bufSize += KHexDecimalLength;
    bufSize += KPortSuffix().Length();
    bufSize += KHexDecimalLength;
    bufSize += KPortSuffix().Length();
    bufSize += KHexDecimalLength;
    bufSize += KLengthSuffix().Length();
    bufSize += aData.Length();
    bufSize += KDataSuffix().Length();
    bufSize += KMessageSuffix().Length(); // ? is this really needed

    TBuf8<KHexDecimalLength> hexbuf;
    HBufC8* buf = HBufC8::NewLC( bufSize );
    TPtr8 ptr( buf->Des() );

    // Append TCP prefix
    ptr.Append( KTCPPrefix );

    // Append peer port
    hexbuf.Format( KHexFormat, aPeerPort );
    ptr.Append( hexbuf );
    ptr.Append( KPortSuffix );

    // Append host port
    hexbuf.Format( KHexFormat, aOriginalPort );
    ptr.Append( hexbuf );
    ptr.Append( KPortSuffix );

    // Append data length
    hexbuf.Format( KHexFormat, aData.Length() );
    ptr.Append( hexbuf );
    ptr.Append( KLengthSuffix );

    // Append data
    ptr.Append( aData );
    ptr.Append( KDataSuffix );
    ptr.Append( KMessageSuffix );

    // Write to socket
    aSocketWriter.WriteL( *buf );

    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::HandleReceivedDataL
// -----------------------------------------------------------------------------
//
TBool CProtocolTCP::HandleReceivedDataL( TDes8& aData, TInt& aStartPos,
                                         TInt& aLength )
    {
    TBool msgHandled = EFalse;
    TInt protCount = iExpressionArray->Count();

    for ( TInt i = 0; i < protCount; i++ )
        {
        msgHandled = iExpressionArray->At( i )->HandleRecievedMsgL( aData,
                                                                    aStartPos,
                                                                    aLength);
        if ( msgHandled )
            {
            break;
            }
        }

    return msgHandled;
    }

// -----------------------------------------------------------------------------
// CProtocolTCP::SendCloseTCPConnection
// -----------------------------------------------------------------------------
//
void CProtocolTCP::SendCloseTCPConnection( MSocketWriter& aSocketWriter,
        TUint aPort )
    {
    DEBUG_PRINT(
        DEBUG_STRING( "CProtocolTCP::SendCloseTCPConnection, port %d" ),
        aPort );

    aSocketWriter.WriteL( KTCPClosePrefix );
    TBuf8<KHexDecimalLength> portBuf;
    portBuf.Format( KHexFormat, aPort );
    aSocketWriter.WriteL( portBuf );
    }

//  End of File
