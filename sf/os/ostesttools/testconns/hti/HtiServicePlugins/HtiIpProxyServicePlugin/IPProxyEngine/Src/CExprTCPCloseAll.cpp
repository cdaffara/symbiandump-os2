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
* Description:  IP-Proxy TCP protocol expression for closing all phone side
*                sockets.
*
*/



// INCLUDE FILES
#include "CExprTCPCloseAll.h"
#include "CommRouterDefinitions.h"
#include "MExpressionObserverTCP.h"

#define DEBUG_FILENAME "IPProxyEngine.log"
#include "DebugPrint.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExprTCPCloseAll::CExprTCPCloseAll
// -----------------------------------------------------------------------------
//
CExprTCPCloseAll::CExprTCPCloseAll( MExpressionObserverTCP* aObserver )
    : iObserver( aObserver )
    {
    __ASSERT_DEBUG( iObserver, User::Invariant() );
    }

// -----------------------------------------------------------------------------
// CExprTCPCloseAll::NewL
// -----------------------------------------------------------------------------
//
CExprTCPCloseAll* CExprTCPCloseAll::NewL( MExpressionObserverTCP* aObserver )
    {
    CExprTCPCloseAll* self = CExprTCPCloseAll::NewLC( aObserver );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CExprTCPCloseAll::NewLC
// -----------------------------------------------------------------------------
//
CExprTCPCloseAll* CExprTCPCloseAll::NewLC( MExpressionObserverTCP* aObserver )
    {
    CExprTCPCloseAll* self = new( ELeave ) CExprTCPCloseAll( aObserver );
    CleanupStack::PushL( self );

    return self;
    }


// Destructor
CExprTCPCloseAll::~CExprTCPCloseAll()
    {
    }


// -----------------------------------------------------------------------------
// CExprTCPCloseAll::HandleReceivedDataL()
// -----------------------------------------------------------------------------
//
TBool CExprTCPCloseAll::HandleRecievedMsgL( TDes8& aData, TInt& aStartPos,
                                         TInt& aLength  )
    {
    // Check if the prefix matches
    aStartPos = aData.Find( KTCPCloseAllPrefix );

    if ( aStartPos != KErrNotFound  )
        {
        // Found a matching prefix
        // Let the observer know
        iObserver->FrameStarted();

        TPtr8 dataToParse( aData.MidTPtr( aStartPos ) );

        TInt err = TryParsing( dataToParse, aLength );

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
// CExprTCPOpen::TryParsing
// -----------------------------------------------------------------------------
//
TInt CExprTCPCloseAll::TryParsing( TDes8& aData, TInt& aLength )
    {
    __ASSERT_ALWAYS( aData.Left( KTCPCloseAllPrefix().Length() ) == KTCPCloseAllPrefix,
        User::Panic( _L("Protocol"), 1 ) );

    // TCP_CLOSEALL:
    TInt frameOverhead = KTCPCloseAllPrefix().Length();

    if ( aData.Length() >= frameOverhead )
        {
        // send parsed results
        iObserver->CloseAllTCPConnections();

        aLength = frameOverhead;
        }
    return KErrNone;
    }

//  End of File
