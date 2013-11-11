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
* Description:  IPProxy UDP protocol expression for a regular UDP message
*                connection.
*
*/



// INCLUDE FILES
#include "CExprUDPMsg.h"
#include "CommRouterDefinitions.h"
#include "MExpressionObserver.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExprUDPMsg::CExprUDPMsg
// -----------------------------------------------------------------------------
//
CExprUDPMsg::CExprUDPMsg( MExpressionObserver* aObserver )
    : iObserver( aObserver )
    {
    __ASSERT_DEBUG( iObserver, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CExprUDPMsg::NewL
// -----------------------------------------------------------------------------
//
CExprUDPMsg* CExprUDPMsg::NewL( MExpressionObserver* aObserver )
    {
    CExprUDPMsg* self = CExprUDPMsg::NewLC( aObserver );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CExprUDPMsg::NewLC
// -----------------------------------------------------------------------------
//
CExprUDPMsg* CExprUDPMsg::NewLC( MExpressionObserver* aObserver )
    {
    CExprUDPMsg* self = new( ELeave ) CExprUDPMsg( aObserver );
    CleanupStack::PushL( self );

    return self;
    }


// Destructor
CExprUDPMsg::~CExprUDPMsg()
    {
    }


// -----------------------------------------------------------------------------
// CExprUDPMsg::HandleReceivedDataL()
// -----------------------------------------------------------------------------
//
TBool CExprUDPMsg::HandleRecievedMsgL( TDes8& aData, TInt& aStartPos, TInt& aLength )
    {
    // Check if the prefix matches
    aStartPos = aData.Find( KUDPPrefix );

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
// CExprUDPMsg::TryParsingL
// -----------------------------------------------------------------------------
//
TInt CExprUDPMsg::TryParsingL( TDes8& aData, TInt& aLength )
    {
    __ASSERT_ALWAYS( aData.Left( KUDPPrefix().Length() ) == KUDPPrefix,
        User::Panic( _L("Protocol"), 1 ) );

    // UDP:0123,000e,[Some test data]
    TInt frameOverhead =
        KUDPPrefix().Length() +
        KHexDecimalLength +
        KPortSuffix().Length() +
        KHexDecimalLength +
        KLengthSuffix().Length() +
        KDataSuffix().Length() +
        KMessageSuffix().Length();

    if ( aData.Length() >= frameOverhead )
        {
        TPtrC8 portPtr(
            aData.Mid( KUDPPrefix().Length(), KHexDecimalLength ) );

        TLex8 portLexer( portPtr );
        TUint port;
        if ( portLexer.Val( port, EHex ) != KErrNone )
            {
            return KErrCorrupt;
            }
        DEBUG_PRINT( DEBUG_STRING( "CExprUDPMsg::TryParsingL, port = %d" ), port );

        //Check port suffix
        if ( aData.Mid( KUDPPrefix().Length() +
            KHexDecimalLength, KPortSuffix().Length() ) != KPortSuffix )
            {
            return KErrCorrupt;
            }

        TPtrC8 lengthPtr( aData.Mid( KUDPPrefix().Length() +
            KHexDecimalLength + KPortSuffix().Length(), KHexDecimalLength ) );
        TLex8 lengthLexer( lengthPtr );
        TUint length;
        if ( lengthLexer.Val( length, EHex ) != KErrNone )
            {
            return KErrCorrupt;
            }
        DEBUG_PRINT( DEBUG_STRING( "CExprUDPMsg::TryParsingL, length = %d" ), length );

        //Check length suffix
        if ( aData.Mid(
            KUDPPrefix().Length() +
            KHexDecimalLength +
            KPortSuffix().Length() +
            KHexDecimalLength, KLengthSuffix().Length() ) != KLengthSuffix )
            {
            return KErrCorrupt;
            }

        DEBUG_PRINT( DEBUG_STRING( "CExprUDPMsg::TryParsingL, parsing data" ), length );

        if ( aData.Length() >= TInt( frameOverhead + length ) )
            {
            TInt messagePos = KUDPPrefix().Length() +
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
            DEBUG_PRINT( DEBUG_STRING( "CExprUDPMsg::TryParsingL, message OK" ) );

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
