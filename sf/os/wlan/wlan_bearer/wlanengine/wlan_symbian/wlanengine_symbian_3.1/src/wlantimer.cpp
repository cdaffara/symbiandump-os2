/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* This class implements timer services.
*
*/

/*
* %version: 1 %
*/

#include "wlantimer.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CWlanTimer::CWlanTimer( 
    const TCallBack& aTimerExpiredCB,
    const TCallBack& aTimerCancelledCB,
    const TCallBack& aTimerErrorCB ) :
    CTimer( CActive::EPriorityStandard ),
    iTimerExpiredCB( aTimerExpiredCB ),
    iTimerCancelledCB( aTimerCancelledCB ),
    iTimerErrorCB( aTimerErrorCB )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CWlanTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Static constructor.
//
// @return  Pointer to CWlanTimer instance.
// ---------------------------------------------------------------------------
//
CWlanTimer* CWlanTimer::NewL(  
    const TCallBack& aTimerExpiredCB,
    const TCallBack& aTimerCancelledCB,
    const TCallBack& aTimerErrorCB )
    {
    CWlanTimer* self = new (ELeave) CWlanTimer(
        aTimerExpiredCB,
        aTimerCancelledCB,
        aTimerErrorCB );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CWlanTimer::~CWlanTimer()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// RunL.
// Handles timer events.
// ---------------------------------------------------------------------------
//
void CWlanTimer::RunL()
    {
    switch( iStatus.Int() )
        {
        case KErrNone: // Timer expired
            iTimerExpiredCB.CallBack();
            break;
        case KErrCancel: // Timer cancelled
            DEBUG( "CWlanTimer::RunL() - timer cancelled" );
            iTimerCancelledCB.CallBack();
            break;
        default: // Timer error code
            DEBUG1( "CWlanTimer::RunL() - timer error (status: %d)",
                iStatus.Int() );
            iTimerErrorCB.CallBack();
        }
    }

// ---------------------------------------------------------------------------
// Cancel timer.
// ---------------------------------------------------------------------------
//
void CWlanTimer::DoCancel()
    {
    CTimer::DoCancel();
    }
