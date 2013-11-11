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
* Description:  UDP protocol implementation for IP proxy
*
*/



// INCLUDE FILES
#include "CProtocolUDP.h"
#include "CExprUDPMsg.h"
#include "MSocketWriter.h"
#include "MProtocolObserverUDP.h"
#include "CommRouterDefinitions.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CProtocolUDP::CProtocolUDP
// -----------------------------------------------------------------------------
//
CProtocolUDP::CProtocolUDP( MProtocolObserverUDP* aObserver )
    : iObserver( aObserver )
    {
    __ASSERT_DEBUG( iObserver, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CProtocolUDP::ConstructL
// -----------------------------------------------------------------------------
//
void CProtocolUDP::ConstructL()
    {
    // Add all the expressions for UDP
    iExpressionArray = new (ELeave) CArrayPtrFlat<MBPExpression> ( 3 );

    CExprUDPMsg* UDPMsg = CExprUDPMsg::NewL( this );
    iExpressionArray->AppendL( UDPMsg );
    }

// -----------------------------------------------------------------------------
// CProtocolUDP::NewL
// -----------------------------------------------------------------------------
//
CProtocolUDP* CProtocolUDP::NewL( MProtocolObserverUDP* aObserver )
    {
    CProtocolUDP* self = CProtocolUDP::NewLC( aObserver );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CProtocolUDP::NewLC
// -----------------------------------------------------------------------------
//
CProtocolUDP* CProtocolUDP::NewLC( MProtocolObserverUDP* aObserver )
    {
    CProtocolUDP* self = new( ELeave ) CProtocolUDP( aObserver );
    CleanupStack::PushL( self );

    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CProtocolUDP::~CProtocolUDP
// -----------------------------------------------------------------------------
//
CProtocolUDP::~CProtocolUDP()
    {
    if ( iExpressionArray )
        {
        iExpressionArray->ResetAndDestroy();
        delete iExpressionArray;
        }
    }

// -----------------------------------------------------------------------------
// CProtocolUDP::FrameStarted
// -----------------------------------------------------------------------------
//
void CProtocolUDP::FrameStarted()
    {
    iObserver->FrameStarted();
    }

// -----------------------------------------------------------------------------
// CProtocolUDP::FrameParsedL
// -----------------------------------------------------------------------------
//
void CProtocolUDP::FrameParsedL( TUint aPort, const TDesC8& aData )
    {
    iObserver->UDPFrameParsedL( aPort, aData );
    }

// -----------------------------------------------------------------------------
// CProtocolUDP::ProtocolErrorL
// -----------------------------------------------------------------------------
//
void CProtocolUDP::ProtocolErrorL(
    TInt aErrorCode, const TDesC8& aReceivedData )
    {
    iObserver->ProtocolErrorL( aErrorCode, aReceivedData );
    }


// -----------------------------------------------------------------------------
// CProtocolUDP::WriteFrameL
// -----------------------------------------------------------------------------
//
void CProtocolUDP::WriteFrameL( MSocketWriter& aSocketWriter,
            TUint aPeerPort, TUint aOriginalPort, const TDesC8& aData ) const
    {
    DEBUG_PRINT( DEBUG_STRING(
        "CProtocolUDP::WriteFrameL(), peer port=%d, original port=%d" ),
        aPeerPort, aOriginalPort );

    TInt bufSize = 0;
    bufSize += KUDPPrefix().Length();
    bufSize += KHexDecimalLength;
    bufSize += KPortSuffix().Length();
    bufSize += KHexDecimalLength;
    bufSize += KLengthSuffix().Length();
    bufSize += aData.Length();
    bufSize += KDataSuffix().Length();
    bufSize += KMessageSuffix().Length();
    
    TBuf8<KHexDecimalLength> hexbuf;
    HBufC8* buf = HBufC8::NewLC( bufSize );
    TPtr8 ptr( buf->Des() );
    
    // Append UDP prefix
    ptr.Append( KUDPPrefix );

    // Append peer port
    hexbuf.Format( KHexFormat, aPeerPort );
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
// CProtocolUDP::HandleReceivedDataL
// -----------------------------------------------------------------------------
//
TBool CProtocolUDP::HandleReceivedDataL( TDes8& aData, TInt& aStartPos,
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
//  End of File
