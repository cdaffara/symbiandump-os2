/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   WlanTimer declaration
*
*/

/*
* %version: 5 %
*/

#ifndef WLANTIMER_H
#define WLANTIMER_H

#include <wlantimer.h>
#include "wlanobject.h"

class WlanOsa;

/**
 *  concrete timer context object
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class WlanTimer : public MWlanTimer, public WlanObject
    {
    friend class WlanOsa;

public:

    /**
     * Destructor.
     *
     * @since S60 v3.2
     */
    virtual ~WlanTimer();

    /**
     * Constructor.
     *
     * @since S60 v3.2
     * @param aOsa wlanosa object
     * @param aDfcQueue Pointer to the DFC queue to use
     */
    explicit WlanTimer( WlanOsa& aOsa, void* aDfcQueue );

protected: // from base class MWlanTimer

    /**
     * From MWlanTimer.
     * Enqueue a timeout for execution.
     * Either succeeds or has no effect.
     * Calling this method for allready queued timeout has no effect
     *
     * @since S60 v3.2
     * @param aTimerClient timer client
     * @param aCtx context for timer client callback
     * @param aTimeoutInMicroSeconds timeout in microseconds
     * @param aPeriodic periodic timer or not
     */
     virtual void Enqueue(
         MWlanTimerClient& aTimerClient,
         TInt aCtx,
         TInt aTimeoutInMicroSeconds,
         TBool aPeriodic );

    /**
     * From MWlanTimer.
     * Dequeue a queued timeout.
     * Either succeeds or has no effect.
     * Calling this method for non queued timeout has no effect
     *
     * @since S60 v3.2
     */
     virtual void Dequeue();

private:

    /** Prohibit copy constructor. */
    WlanTimer( const WlanTimer& );
    
    /** Prohibit assigment operator. */
    WlanTimer& operator= ( const WlanTimer& );

    /**
     * timer execution entry
     *
     * @since S60 v3.2
     */
    static void TimerTrigger( TAny* aPtr );

    /**
     * timer execution entry
     */
    void TimerTrigger();

    /**
     * Decrements the reference counter of this object and when all references
     * are cleared the object is deleted.
     */
    void RefDel();

    /**
     * Incrementes the reference counter.
     */
    void RefAdd();

private: // data

    /** 
     * DFC for timer callbacks.
     */
    TDfc iTimerDfc;

    /**
     * osa object reference
     */
    WlanOsa& iOsa;

    /**
     * the one and only timer
     */
    NTimer iNTimer;

    /**
     * timer client callback interface
     * Not Own.
     */
    MWlanTimerClient* iTimerClient;

    /**
     * timeout value
     */
    TInt iTimeoutInMicroSeconds;

    /**
     * callback context
     */
    TInt iCtx;

    /**
     * internal state flags
     */
    TUint iFlags;

    /**
     * reference counter for cleanup
     */
    TUint iRefCnt;

    };

#endif // WLANTIMER_H
