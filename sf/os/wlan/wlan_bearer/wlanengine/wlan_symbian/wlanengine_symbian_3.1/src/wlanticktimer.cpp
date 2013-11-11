/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* This class implements a system tick based timer service.
*
*/

/*
* %version: 2 %
*/

#include "wlanticktimer.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanTickTimer::CWlanTickTimer( 
    const TCallBack& aTimerExpiredCB,
    const TCallBack& aTimerCancelledCB,
    const TCallBack& aTimerErrorCB ) :
    CActive( CActive::EPriorityStandard ), 
    iTimerExpiredCB( aTimerExpiredCB ),
    iTimerCancelledCB( aTimerCancelledCB ),
    iTimerErrorCB( aTimerErrorCB )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanTickTimer::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanTickTimer* CWlanTickTimer::NewL(  
    const TCallBack& aTimerExpiredCB,
    const TCallBack& aTimerCancelledCB,
    const TCallBack& aTimerErrorCB )
    {
    CWlanTickTimer* self = new (ELeave) CWlanTickTimer(
        aTimerExpiredCB,
        aTimerCancelledCB,
        aTimerErrorCB );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanTickTimer::~CWlanTickTimer()
    {
    Cancel();
    iTimer.Close();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanTickTimer::After(
    TUint aTicks )
    {
    DEBUG1( "CWlanTickTimer::After() - aTicks: %u",
        aTicks );
    
    iTimer.AfterTicks(
        iStatus,
        aTicks );
    SetActive();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanTickTimer::RunL()
    {
    DEBUG1( "CWlanTickTimer::RunL() - iStatus: %d",
        iStatus.Int() );

    switch( iStatus.Int() )
        {
        case KErrNone: // Timer expired
            iTimerExpiredCB.CallBack();
            break;
        case KErrCancel: // Timer cancelled
            iTimerCancelledCB.CallBack();
            break;
        default: // Timer error code
            iTimerErrorCB.CallBack();
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanTickTimer::RunError(
    TInt /* aError */ )
    {
    DEBUG( "CWlanTickTimer::RunError()" );

    return 0;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanTickTimer::DoCancel()
    {
    DEBUG( "CWlanTickTimer::DoCancel()" );

    iTimer.Cancel();
    }
