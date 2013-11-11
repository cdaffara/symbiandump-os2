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
* Description:  This abstract class defines WLAN scan provider interface.
*
*/

/*
* %version: 1 %
*/


#ifndef WLANTIMERSERVICEINTERFACE_H
#define WLANTIMERSERVICEINTERFACE_H

#include <e32def.h>



/**
 * @brief Interface definition for WLAN Timer service callback.
 *
 * @since S60 v5.2
 */
class MWlanTimerServiceCallback
    {
    public:
    
        /**
         * Timeout has occurred.
         */
        virtual void OnTimeout() = 0;

    };

class MWlanTimerServices
    {
    
public:
    /**
     * Start the timer
     *
     * @since S60 v5.2
     * @param aRequestId reference to request id
     * @param aAt reference to time when timeout should occur
     * @param aCb reference to callback object
     * @return error code
     */
    virtual TInt StartTimer( TUint& aRequestId, TTime& aAt, MWlanTimerServiceCallback& aCb ) = 0;

    /**
     * Stop the timer
     *
     * @since S60 v5.2
     * @param aRequestId Id of timer to stop
     */
    virtual void StopTimer( const TUint& aRequestId ) = 0;
    
    /**
     * Handles timer timeout.
     *
     * @since S60 v5.2
     */
    virtual void HandleTimeout() = 0;

    };

#endif // WLANTIMERSERVICEINTERFACE_H
