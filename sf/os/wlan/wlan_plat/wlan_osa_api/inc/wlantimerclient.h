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
* Description:  timer client callback interface
*
*/

/*
* %version: 4 %
*/

#ifndef WLANTIMERCLIENT_H
#define WLANTIMERCLIENT_H

#include <wlanosaplatform.h>

/**
 *  timer client callback interface
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class MWlanTimerClient 
    {

public:

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~MWlanTimerClient() {};

    /**
     * timer callback
     *
     * @since S60 v3.2
     * @param aCtx callback context
     */
    virtual void OnTimeout( TInt aCtx ) = 0;

    };


#endif // WLANTIMERCLIENT_H
