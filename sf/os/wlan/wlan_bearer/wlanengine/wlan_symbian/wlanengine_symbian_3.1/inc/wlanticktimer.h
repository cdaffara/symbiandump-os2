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
* %version: 1 %
*/

#ifndef WLANTICKTIMER_H
#define WLANTICKTIMER_H

#include <e32base.h>

/**
 *  CWlanTickTimer class.
 *
 *  Class providing timer services. The underlaying implementation relies
 *  on RTimer. Therefore this module has the same limitations as RTimer.
 * 
 *  @lib wlmserversrv.dll
 *  @since S60 v5.2
 */
 
NONSHARABLE_CLASS( CWlanTickTimer ) : public CActive
    {
    
public: // CWlanTickTimer public methods

    /**
     * Static constructor.
     * @param aTimerExpiredCB   Callback for indicating timer expiry.
     * @param aTimerCancelledCB Callback for indicating timer cancellation.
     * @param aTimerErrorCB     Callback for indicating timer error.
     */
    static CWlanTickTimer* NewL(
        const TCallBack& aTimerExpiredCB,
        const TCallBack& aTimerCancelledCB,
        const TCallBack& aTimerErrorCB );

    /**
     * Destructor.
     */
    ~CWlanTickTimer();

    /**
     * Requests an event after the specified interval.
     * @param The time interval in system ticks.
     */
    void After(
        TUint aTicks );
    
private: // CWlanTickTimer private methods

    /**
     * C++ constructor.
     * @param aTimerExpiredCB   Callback for indicating timer expiry.
     * @param aTimerCancelledCB Callback for indicating timer cancellation.
     * @param aTimerErrorCB     Callback for indicating timer error.
     */
    CWlanTickTimer(
        const TCallBack& aTimerExpiredCB,
        const TCallBack& aTimerCancelledCB,
        const TCallBack& aTimerErrorCB );
    
    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

private: // From CActive

    /**
     * From CActive.
     * Called by the active object framework when the request has been completed.
     */
    void RunL();

    /**
     * From CActive.
     * Called by the framework if RunL leaves.
     *
     * @param aError The error code RunL leaved with.
     * @return KErrNone if leave was handled, one of the system-wide error codes otherwise.
     */
    TInt RunError(
        TInt aError );

    /**
     * From CActive.
     * Called by the framework when Cancel() has been called.
     */
    void DoCancel();

private: // CWlanTickTimer private data

    /**
     * Handle to RTimer.
     */
    RTimer iTimer;

    /**
     * TimerExpiredCB.
     * Registered callback method that is called, when the timer
     * expires. This is set when CWlanTickTimer instance is created.
     */   
    TCallBack iTimerExpiredCB;
    
    /**
     * TimerCancelledCB.
     * Registered callback method that is called, when the timer
     * is cancelled by an external event (i.e. cancellation is
     * not requested by the client). This is set when CWlanTickTimer
     * instance is created.
     */
    TCallBack iTimerCancelledCB;
    
    /**
     * TimerErrorCB.
     * Registered callback method that is called, when there is
     * a timer error. This is set when CWlanTickTimer instance is created.
     */
    TCallBack iTimerErrorCB;
    };

#endif // WLANTICKTIMER_H
