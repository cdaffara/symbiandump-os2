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
* Description:  IPProxy TCP protocol expression for opening phone side TCP
*                connection.
*
*/



// INCLUDE FILES
#include "CExprTCPOpen.h"
#include "CommRouterDefinitions.h"
#include "MExpressionObserverTCP.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExprTCPOpen::CExprTCPOpen
// -----------------------------------------------------------------------------
//
CExprTCPOpen::CExprTCPOpen( MExpressionObserverTCP* aObserver )
    : iObserver( aObserver )
    {
    __ASSERT_DEBUG( iObserver, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CExprTCPOpen::NewL
// -----------------------------------------------------------------------------
//
CExprTCPOpen* CExprTCPOpen::NewL( MExpressionObserverTCP* aObserver )
    {
    CExprTCPOpen* self = CExprTCPOpen::NewLC( aObserver );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CExprTCPOpen::NewLC
// -----------------------------------------------------------------------------
//
CExprTCPOpen* CExprTCPOpen::NewLC( MExpressionObserverTCP* aObserver )
    {
    CExprTCPOpen* self = new( ELeave ) CExprTCPOpen( aObserver );
    CleanupStack::PushL( self );

    return self;
    }


// Destructor
CExprTCPOpen::~CExprTCPOpen()
    {
    }


// -----------------------------------------------------------------------------
// CExprTCPOpen::HandleReceivedDataL()
// -----------------------------------------------------------------------------
//
TBool CExprTCPOpen::HandleRecievedMsgL( TDes8& aData, TInt& aStartPos, TInt& aLength )
    {
    // Check if the prefix matches
    aStartPos = aData.Find( KTCPOpenPrefix );

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
// CExprTCPOpen::TryParsingL
// -----------------------------------------------------------------------------
//
TInt CExprTCPOpen::TryParsingL( TDes8& aData, TInt& aLength )
    {
    __ASSERT_ALWAYS( aData.Left( KTCPOpenPrefix().Length() ) == KTCPOpenPrefix,
        User::Panic( _L("Protocol"), 1 ) );

    // TCP_OPEN:0123
    TInt frameOverhead =
        KTCPOpenPrefix().Length() +
        KHexDecimalLength;

    if ( aData.Length() >= frameOverhead )
        {
        TPtrC8 portPtr(
            aData.Mid( KTCPOpenPrefix().Length(), KHexDecimalLength ) );

        TLex8 portLexer( portPtr );
        TUint port;
        if ( portLexer.Val( port, EHex ) != KErrNone )
            {
            return KErrCorrupt;
            }

        // send parsed results
        iObserver->OpenLocalTCPConnectionL( port );

        aLength = frameOverhead;

        return KErrNone;
        }
    return KErrNone;
    }

//  End of File
