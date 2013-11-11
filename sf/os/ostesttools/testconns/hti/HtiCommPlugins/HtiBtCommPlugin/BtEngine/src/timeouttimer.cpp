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
* Description:  Timer.
*
*/


// INCLUDE FILES
#include "timeouttimer.h"
#include "timeoutnotifier.h"
#include "HtiBtEngineLogging.h"

// -----------------------------------------------------------------------------
CTimeOutTimer* CTimeOutTimer::NewL( const TInt aPriority,
                                    MTimeOutNotifier& aTimeOutNotify )
    {
    CTimeOutTimer* self = new ( ELeave ) CTimeOutTimer( aPriority,
                                                        aTimeOutNotify );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
CTimeOutTimer::CTimeOutTimer( const TInt aPriority,
                              MTimeOutNotifier& aObserver )
: CTimer( aPriority ), iObserver( aObserver )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
void CTimeOutTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
CTimeOutTimer::~CTimeOutTimer()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
void CTimeOutTimer::RunL()
    {
    // Timer request has completed, so notify the timer's owner
    if ( iStatus == KErrNone )
        {
        iObserver.TimerExpired();
        }
    else
        {
        LOGFMT_E("CTimeOutTimer: bad completion code: %d", iStatus.Int())
        }
    }

// End of File
