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
* Description:  IPProxy TCP protocol expression for a regular TCP message
*                connection.
*
*/



// INCLUDE FILES
#include "CExprTCPMsg.h"
#include "CommRouterDefinitions.h"
#include "MProtocolObserverTCP.h"
#include "MExpressionObserver.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExprTCPMsg::CExprTCPMsg
// -----------------------------------------------------------------------------
//
CExprTCPMsg::CExprTCPMsg( MExpressionObserver* aObserver )
    : iObserver( aObserver )
    {
    __ASSERT_DEBUG( iObserver, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CExprTCPMsg::NewL
// -----------------------------------------------------------------------------
//
CExprTCPMsg* CExprTCPMsg::NewL( MExpressionObserver* aObserver )
    {
    CExprTCPMsg* self = CExprTCPMsg::NewLC( aObserver );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CExprTCPMsg::NewLC
// -----------------------------------------------------------------------------
//
CExprTCPMsg* CExprTCPMsg::NewLC( MExpressionObserver* aObserver )
    {
    CExprTCPMsg* self = new( ELeave ) CExprTCPMsg( aObserver );
    CleanupStack::PushL( self );

    return self;
    }


// Destructor
CExprTCPMsg::~CExprTCPMsg()
    {
    }


// -----------------------------------------------------------------------------
// CExprTCPMsg::HandleReceivedDataL()
// -----------------------------------------------------------------------------
//
TBool CExprTCPMsg::HandleRecievedMsgL( TDes8& aData, TInt& aStartPos, TInt& aLength )
    {
    // Check if the prefix matches
    aStartPos = aData.Find( KTCPPrefix );

    if ( aStartPos != KErrNotFound  )
        {
        // Found a matching prefix
        // Let the observer know
        iObserver->FrameStarted();

        TPtr8 dataToParse( aData.MidTPtr( aStartPos ) );

        TInt err = TryParsingL( dataToParse, aLength );

        if ( err != KErrNone )
            {
            // corrupted data in the frame
            iObserver->ProtocolErrorL( err, aData );
            // delete the corrupted data
            aData.SetLength( 0 );
            }

        return ETrue;
        }
    else
        {
        return EFalse;
        }

    }

// -----------------------------------------------------------------------------
// CExprTCPMsg::TryParsingL
// -----------------------------------------------------------------------------
//
TInt CExprTCPMsg::TryParsingL( TDes8& aData, TInt& aLength )
    {
    __ASSERT_ALWAYS( aData.Left( KTCPPrefix().Length() ) == KTCPPrefix,
        User::Panic( _L("Protocol"), 1 ) );

    // TCP:0123,000e,[Some test data]
    TInt frameOverhead =
        KTCPPrefix().Length() +
        KHexDecimalLength +
        KPortSuffix().Length() +
        KHexDecimalLength +
        KLengthSuffix().Length() +
        KDataSuffix().Length() +
        KMessageSuffix().Length();

    if ( aData.Length() >= frameOverhead )
        {
        TPtrC8 portPtr(
            aData.Mid( KTCPPrefix().Length(), KHexDecimalLength ) );

        TLex8 portLexer( portPtr );
        TUint port;
        if ( portLexer.Val( port, EHex ) != KErrNone )
            {
            return KErrCorrupt;
            }
        DEBUG_PRINT( DEBUG_STRING( "CExprTCPMsg::TryParsingL, port = %d" ), port );

        //Check port suffix
        if ( aData.Mid( KTCPPrefix().Length() +
            KHexDecimalLength, KPortSuffix().Length() ) != KPortSuffix )
            {
            return KErrCorrupt;
            }

        TPtrC8 lengthPtr( aData.Mid( KTCPPrefix().Length() +
            KHexDecimalLength + KPortSuffix().Length(), KHexDecimalLength ) );
        TLex8 lengthLexer( lengthPtr );
        TUint length;
        if ( lengthLexer.Val( length, EHex ) != KErrNone )
            {
            return KErrCorrupt;
            }
        DEBUG_PRINT( DEBUG_STRING( "CExprTCPMsg::TryParsingL, length = %d" ), length );

        //Check length suffix
        if ( aData.Mid(
            KTCPPrefix().Length() +
            KHexDecimalLength +
            KPortSuffix().Length() +
            KHexDecimalLength, KLengthSuffix().Length() ) != KLengthSuffix )
            {
            return KErrCorrupt;
            }

        if ( aData.Length() >= TInt( frameOverhead + length ) )
            {
            TInt messagePos = KTCPPrefix().Length() +
                KHexDecimalLength +
                KPortSuffix().Length() +
                KHexDecimalLength +
                KLengthSuffix().Length();

            TPtrC8 message( aData.Mid( messagePos, length ) );
            if ( aData.Mid( messagePos + length,
                KDataSuffix().Length() ) != KDataSuffix )
                {
                return KErrCorrupt;
                }
            DEBUG_PRINT( DEBUG_STRING( "CExprTCPMsg::TryParsingL, message OK" ) );

            if ( aData.Mid( messagePos + length + KDataSuffix().Length(),
                KMessageSuffix().Length() ) != KMessageSuffix )
                {
                return KErrCorrupt;
                }

            // send parsed results
            iObserver->FrameParsedL( port, message );
            // set the length of the handled message
            aLength = frameOverhead + length;

            return KErrNone;
            }

        }
    return KErrNone;
    }

//  End of File
