/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  timer context interface declaration
*
*/

/*
* %version: 4 %
*/

#ifndef M_WLANTIMER_H
#define M_WLANTIMER_H

#include <wlanosaplatform.h>

class MWlanTimerClient;

/**
 *  timer context interface declaration
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class MWlanTimer 
    {

public:

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
     virtual ~MWlanTimer() {};

    /**
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
         TBool aPeriodic ) = 0;

    /**
     * Dequeue a queued timeout. 
     * Either succeeds or has no effect.
     * Calling this method for non queued timeout has no effect
     *
     * @since S60 v3.2
     */
     virtual void Dequeue() = 0;
    };

#endif // M_WLANTIMER_H
