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
* Description:  IP-Proxy TCP protocol expression for opening listening phone
*                side TCP socket.
*
*/



// INCLUDE FILES
#include "CExprTCPLstn.h"
#include "CommRouterDefinitions.h"
#include "MExpressionObserverTCP.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExprTCPLstn::CExprTCPLstn
// -----------------------------------------------------------------------------
//
CExprTCPLstn::CExprTCPLstn( MExpressionObserverTCP* aObserver )
    : iObserver( aObserver )
    {
    __ASSERT_DEBUG( iObserver, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CExprTCPLstn::NewL
// -----------------------------------------------------------------------------
//
CExprTCPLstn* CExprTCPLstn::NewL( MExpressionObserverTCP* aObserver )
    {
    CExprTCPLstn* self = CExprTCPLstn::NewLC( aObserver );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CExprTCPLstn::NewLC
// -----------------------------------------------------------------------------
//
CExprTCPLstn* CExprTCPLstn::NewLC( MExpressionObserverTCP* aObserver )
    {
    CExprTCPLstn* self = new( ELeave ) CExprTCPLstn( aObserver );
    CleanupStack::PushL( self );

    return self;
    }

// Destructor
CExprTCPLstn::~CExprTCPLstn()
    {
    }

// -----------------------------------------------------------------------------
// CExprTCPLstn::HandleReceivedDataL()
// -----------------------------------------------------------------------------
//
TBool CExprTCPLstn::HandleRecievedMsgL( TDes8& aData, TInt& aStartPos, TInt& aLength )
    {
    // Check if the prefix matches
    aStartPos = aData.Find( KTCPLstnPrefix );

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
// CExprTCPLstn::TryParsingL
// -----------------------------------------------------------------------------
//
TInt CExprTCPLstn::TryParsingL( TDes8& aData, TInt& aLength )
    {
    __ASSERT_ALWAYS( aData.Left( KTCPLstnPrefix().Length() ) == KTCPLstnPrefix,
        User::Panic( _L("Protocol"), 1 ) );

    // TCP_LSTN:0123
    TInt frameOverhead =
        KTCPLstnPrefix().Length() +
        KHexDecimalLength;

    if ( aData.Length() >= frameOverhead )
        {
        TPtrC8 portPtr(
            aData.Mid( KTCPLstnPrefix().Length(), KHexDecimalLength ) );

        TLex8 portLexer( portPtr );
        TUint port;
        if ( portLexer.Val( port, EHex ) != KErrNone )
            {
            return KErrCorrupt;
            }

        // send parsed results
        iObserver->OpenListeningTCPConnectionL( port );

        aLength = frameOverhead;

        return KErrNone;
        }
    return KErrNone;
    }

//  End of File
