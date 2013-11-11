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
* Description:  IPProxy TCP protocol expression for closing phone side TCP
*                connection.
*
*/



// INCLUDE FILES
#include "CExprTCPClose.h"
#include "CommRouterDefinitions.h"
#include "MExpressionObserverTCP.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExprTCPClose::CExprTCPClose
// -----------------------------------------------------------------------------
//
CExprTCPClose::CExprTCPClose( MExpressionObserverTCP* aObserver )
    : iObserver( aObserver )
    {
    __ASSERT_DEBUG( iObserver, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CExprTCPClose::NewL
// -----------------------------------------------------------------------------
//
CExprTCPClose* CExprTCPClose::NewL( MExpressionObserverTCP* aObserver )
    {
    CExprTCPClose* self = CExprTCPClose::NewLC( aObserver );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CExprTCPClose::NewLC
// -----------------------------------------------------------------------------
//
CExprTCPClose* CExprTCPClose::NewLC( MExpressionObserverTCP* aObserver )
    {
    CExprTCPClose* self = new( ELeave ) CExprTCPClose( aObserver );
    CleanupStack::PushL( self );

    return self;
    }


// Destructor
CExprTCPClose::~CExprTCPClose()
    {
    }


// -----------------------------------------------------------------------------
// CExprTCPClose::HandleReceivedDataL()
// -----------------------------------------------------------------------------
//
TBool CExprTCPClose::HandleRecievedMsgL( TDes8& aData, TInt& aStartPos,
                                         TInt& aLength  )
    {
    // Check if the prefix matches
    aStartPos = aData.Find( KTCPClosePrefix );

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
// CExprTCPClose::TryParsing
// -----------------------------------------------------------------------------
//
TInt CExprTCPClose::TryParsingL( TDes8& aData, TInt& aLength )
    {
    __ASSERT_ALWAYS( aData.Left( KTCPClosePrefix().Length() ) == KTCPClosePrefix,
        User::Panic( _L("Protocol"), 1 ) );

    // TCP_CLOSE:0fff
    TInt frameOverhead =
        KTCPClosePrefix().Length() +
        KHexDecimalLength;

    if ( aData.Length() >= frameOverhead )
        {
        TPtrC8 portPtr(
            aData.Mid( KTCPClosePrefix().Length(), KHexDecimalLength ) );

        TLex8 portLexer( portPtr );
        TUint port;
        if ( portLexer.Val( port, EHex ) != KErrNone )
            {
            return KErrCorrupt;
            }

        // send parsed results
        iObserver->CloseTCPConnectionL( port );

        aLength = frameOverhead;

        return KErrNone;
        }
    return KErrNone;
    }

//  End of File
