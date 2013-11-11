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
* Description:  DFC context interface declaration
*
*/

/*
* %version: 4 %
*/

#ifndef M_WLANDFC_H
#define M_WLANDFC_H

#include <wlanosaplatform.h>

class MWlanDfcClient;

/**
 *  DFC context interface declaration
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class MWlanDfc 
    {

public:

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~MWlanDfc() {};

    /**
     * Enqueue a dfc for execution. 
     * Either succeeds or has no effect.
     * Calling this method for allready queued dfc has no effect
     *
     * @since S60 v3.2
     * @param aDfcClient client interface
     * @param aCtx context passed back in client callback
     */
     virtual void Enqueue( MWlanDfcClient& aDfcClient, TInt aCtx ) = 0;

    /**
     * Dequeue a queued dfc. 
     * Either succeeds or has no effect.
     * Calling this method for non queued dfc has no effect 
     *
     * @since S60 v3.2
     */
     virtual void Dequeue() = 0;

    };

#endif // M_WLANDFC_H
