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

#ifndef C_WLANTIMER_H
#define C_WLANTIMER_H

#include <e32base.h>

/**
 *  CWlanTimer class.
 *
 *  Class providing timer services. The underlaying implementation relies
 *  on CTimer. Therefore this module has the same limitations as CTimer.
 * 
 *  @lib wlmserversrv.dll
 *  @since S60 v5.2
 */
 
NONSHARABLE_CLASS( CWlanTimer ) : public CTimer
    {
    
public: // CWlanTimer public methods

    /**
     * Static constructor.
     * @param aTimerExpiredCB   Callback for indicating timer expiry.
     * @param aTimerCancelledCB Callback for indicating timer cancellation.
     * @param aTimerErrorCB     Callback for indicating timer error.
     */
    static CWlanTimer* NewL(
        const TCallBack& aTimerExpiredCB,
        const TCallBack& aTimerCancelledCB,
        const TCallBack& aTimerErrorCB );
        
    /**
     * Destructor.
     */
    ~CWlanTimer();

private: // CWlanTimer private methods

    /**
     * C++ constructor.
     * @param aTimerExpiredCB   Callback for indicating timer expiry.
     * @param aTimerCancelledCB Callback for indicating timer cancellation.
     * @param aTimerErrorCB     Callback for indicating timer error.
     */
    CWlanTimer(
        const TCallBack& aTimerExpiredCB,
        const TCallBack& aTimerCancelledCB,
        const TCallBack& aTimerErrorCB );
    
    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

private: // From CTimer

    /**
     * Handles timer events.
     */
    void RunL();
    
    /**
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

private: // CWlanTimer private data

    /**
     * TimerExpiredCB.
     * Registered callback method that is called, when the timer
     * expires. This is set when CWlanTimer instance is created.
     */   
    TCallBack iTimerExpiredCB;
    
    /**
     * TimerCancelledCB.
     * Registered callback method that is called, when the timer
     * is cancelled by an external event (i.e. cancellation is
     * not requested by the client). This is set when CWlanTimer
     * instance is created.
     */
    TCallBack iTimerCancelledCB;
    
    /**
     * TimerErrorCB.
     * Registered callback method that is called, when there is
     * a timer error. This is set when CWlanTimer instance is created.
     */
    TCallBack iTimerErrorCB;
    };

#endif // C_WLANTIMER_H