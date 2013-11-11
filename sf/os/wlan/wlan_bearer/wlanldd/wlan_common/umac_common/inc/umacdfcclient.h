/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   dfc client callback interface
*
*/

/*
* %version: 3 %
*/

#ifndef WLANDFCCLIENT_H
#define WLANDFCCLIENT_H

/**
 *  dfc client callback interface
 *
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class MWlanDfcClient 
    {

public:

    /**
     * dfc callback
     *
     * @since S60 v3.1
     * @param aCtx callback context
     */
    virtual void OnDfc( TAny* aCtx ) = 0;

    };


#endif // WLANDFCCLIENT_H
