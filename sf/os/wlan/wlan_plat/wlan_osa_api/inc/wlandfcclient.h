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
* Description:  dfc client callback interface
*
*/

/*
* %version: 4 %
*/

#ifndef WLANDFCCLIENT_H
#define WLANDFCCLIENT_H

#include <wlanosaplatform.h>

/**
 *  dfc client callback interface
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class MWlanDfcClient 
    {

public:

	/**
	 * Destructor.
	 *
	 * @since S60 v3.2
	 */
    virtual ~MWlanDfcClient() {};

    /**
     * dfc callback
     *
     * @since S60 v3.2
     * @param aCtx callback context
     */
    virtual void OnDfc( TInt aCtx ) = 0;

    };


#endif // WLANDFCCLIENT_H
