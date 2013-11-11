/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   WlanTimer implementation
*
*/

/*
* %version: 10 %
*/

#include "osa_includeme.h"

#include <wlantimerclient.h>

#include "osatimer.h"
#include "osa.h"

// mask value to check is timer periodic or not
const TUint KPeriodicTimerMask( 1 << 1 );
// mask value to check if timer is running
const TUint KTimerRunningMask( 1 << 2 );


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
WlanTimer::WlanTimer( WlanOsa& aOsa, void* aDfcQueue )
  : iTimerDfc( WlanTimer::TimerTrigger, this, 0 ),
    iOsa( aOsa ),
    iNTimer( ),
    iTimerClient( NULL ),
    iTimeoutInMicroSeconds( 0 ),
    iCtx( 0 ),
    iFlags( 0 ),
    iRefCnt( 1 )
    {
    TraceDump(INFO_LEVEL | TIMER_LEVEL, ("[WLAN] WlanTimer() ctor +"));

    MWlanOsa::Assert( 
        reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__,
        aDfcQueue != NULL );

    iTimerDfc.SetDfcQ( reinterpret_cast<TDfcQue*>(aDfcQueue) );
    Validate();

    TraceDump(INFO_LEVEL | TIMER_LEVEL, ("[WLAN] WlanTimer() ctor -"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
WlanTimer::~WlanTimer()
    {
    TraceDump(INFO_LEVEL | TIMER_LEVEL, ("[WLAN] WlanTimer() dtor +"));

    if ( iRefCnt != 0 )
        {
        TraceDump(CRIT_LEVEL, (("[WLAN] error: Timer destroyed while "
                                "active - delete used directly?")));
        MWlanOsa::Assert(
            reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
        }

    TraceDump(INFO_LEVEL | TIMER_LEVEL, ("[WLAN] WlanTimer() dtor -"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void WlanTimer::TimerTrigger( TAny* aPtr )
    {
    WlanTimer* timer = reinterpret_cast< WlanTimer* >( aPtr );
    timer->TimerTrigger();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void WlanTimer::TimerTrigger()
    {
    WlanOsa &osa = iOsa;

    TraceDump(INFO_LEVEL | TIMER_LEVEL, ("[WLAN] WlanTimer::TimerTrigger +"));

    // get hold of the global mutex
    iOsa.MutexAcquire();

    if (iFlags & KTimerRunningMask)
        {
        if ( iFlags & KPeriodicTimerMask )
            {
            // a periodic timer was requested - re-arm the timer for the next
            // run now.
            const TInt ticks(
                NKern::TimerTicks( iTimeoutInMicroSeconds / 1000 ));
            TraceDump(TIMER_LEVEL, (("[WLAN] timer ticks: %d"), ticks));

            // on arming the timer, add a reference to it - the reference
            // shall be cleared upon the completion callback execution.
            RefAdd();
            const TInt ret (iNTimer.Again( ticks ));

            if ( ret != KErrNone )
                {
                TraceDump(CRIT_LEVEL, (("[WLAN] error: ret %d"), ret));
                MWlanOsa::Assert(
                    reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
                RefDel();
                }
            }
        else
            {
            // clear the timer running mask
            iFlags &= (~KTimerRunningMask);
            }

        TraceDump(TIMER_LEVEL,
            (("[WLAN] current system time: %d"), Kern::SystemTime()));
        TraceDump(TIMER_LEVEL,
            (("[WLAN] timer client address: 0x%08x"), iTimerClient));
        TraceDump(TIMER_LEVEL,
            (("[WLAN] call timer callback with context: %d"), iCtx));

        // execute the timer client call back function
        iTimerClient->OnTimeout( iCtx );
        }

    // a reference was added for this timer instance upon arming of the
    // timer - here, we must remove that reference - and possibly delete
    // the object instance if we were last to hold reference to it.
    // NOTE! here the reference count may become zero (if the
    // TimerDestroy OSA method was called for the instance prior to this
    // handler executing) and hence the instance may be deleted upon this
    // call. do not touch instance data after this call.
    RefDel();

    // release the global mutex - do not use the "iOsa" as the instance
    // may now have been deleted, instead use a local reference.
    osa.MutexRelease();

    TraceDump(INFO_LEVEL | TIMER_LEVEL, ("[WLAN] WlanTimer::TimerTrigger -"));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void WlanTimer::Enqueue(
    MWlanTimerClient& aTimerClient,
    TInt aCtx,
    TInt aTimeoutInMicroSeconds,
    TBool aPeriodic )
    {

    if ( iFlags & KTimerRunningMask )
        {
        // ignore the call if the timer is already running
        TraceDump(INFO_LEVEL | TIMER_LEVEL,
            (("[WLAN] WlanTimer::Enqueue: + addr: 0x%08x: "
              "Timer already enqueued, no effect."), this));
        }
    else
        {
        TraceDump(INFO_LEVEL | TIMER_LEVEL,
            (("[WLAN] WlanTimer::Enqueue: + addr: 0x%08x"), this));
        TraceDump(TIMER_LEVEL,
            (("[WLAN] timeout in microseconds: %d"), aTimeoutInMicroSeconds));
        TraceDump(TIMER_LEVEL, (("[WLAN] periodic: %d"), aPeriodic));

        // store timer parameters
        iFlags = KTimerRunningMask;
        iTimerClient = &aTimerClient;
        iCtx = aCtx;

        if ( aPeriodic )
            {
            iFlags |= KPeriodicTimerMask;
            }

        const TInt KMinTimeout = 1000;

        // the timeout minimum is 1000 us
        if (aTimeoutInMicroSeconds < KMinTimeout)
            {
              iTimeoutInMicroSeconds = KMinTimeout;
            }
        else
            {
            // adding 999 here will allow the us timeout to be rounded
            // up to the next full ms, guaranteeing at least the timeout
            // requested, no less.
            iTimeoutInMicroSeconds = aTimeoutInMicroSeconds + 999;
            }

        // arm the timer (using ms instead of us for the unit)
        const TInt ticks( NKern::TimerTicks( iTimeoutInMicroSeconds / 1000 ));

        TraceDump(TIMER_LEVEL, (("[WLAN] timeout in ticks: %d"), ticks));
        TraceDump(TIMER_LEVEL,
            (("[WLAN] current system time: %d"), Kern::SystemTime()));

        // add reference to the timer instance - this reference will be
        // removed once the timeout event has been handled
        RefAdd();
        const TInt ret( iNTimer.OneShot( ticks, iTimerDfc ));

        if ( ret != KErrNone )
            {
            TraceDump(CRIT_LEVEL, (("[WLAN] error: ret %d"), ret));
            MWlanOsa::Assert(
                reinterpret_cast<const TInt8*>(WLAN_FILE), __LINE__ );
            RefDel();
            }
        }

    TraceDump(INFO_LEVEL | TIMER_LEVEL,
        (("[WLAN] WlanTimer::Enqueue: - addr: 0x%08x: "), this));
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void WlanTimer::Dequeue()
    {
    TraceDump(INFO_LEVEL | TIMER_LEVEL,
        (("[WLAN] WlanTimer::Dequeue: + addr: 0x%08x"), this));

    if ( iFlags & KTimerRunningMask )
        {
        iFlags = 0;

        if ( iNTimer.Cancel())
            {
            // if cancellation of the timer succeeds, remove a reference from
            // the timer instance - as this will not be done in the timeout
            // handler now. NOTE! that here the reference count should never
            // reach zero (instance exists + the timer is running == 2)
            RefDel();

            }
        // if the cancellation of the timer failed above, the DFC is most
        // likely already executing - attempting to cancel it here would
        // be wasting of clock cycles.
        //
        // the most common scenario is that the timer cancel succeeds. for
        // the rare cases of a failure, we just let the DFC execute, and
        // do nothing (the flags are cleared.)
        }
    else
        {
        TraceDump(INFO_LEVEL | TIMER_LEVEL,
             (("[WLAN] WlanTimer::Dequeue: Timer is not running.")));
        }

    TraceDump(INFO_LEVEL | TIMER_LEVEL,
        (("[WLAN] WlanTimer::Dequeue: - addr: 0x%08x"), this));
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void WlanTimer::RefDel(void)
    {
    // decrement the reference counter
    iRefCnt --;

    if (iRefCnt == 0)
        {
        // referees have reached zero, the object is abandoned, and
        // shall be deleted
        WlanTimer *timer = this;
        delete timer;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void WlanTimer::RefAdd(void)
    {
    // increment the reference counter
    iRefCnt ++;
    }
