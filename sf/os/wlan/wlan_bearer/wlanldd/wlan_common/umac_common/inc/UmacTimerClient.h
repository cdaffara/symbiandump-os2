/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   timer client callback interface
*
*/

/*
* %version: 6 %
*/

#ifndef WLANTIMERCLIENT_H
#define WLANTIMERCLIENT_H

/**
 *  timer client callback interface
 *
 *
 *  @lib wlanumac.lib
 *  @since S60 v3.1
 */
class MWlanTimerClient 
    {

public: // New functions

    /**
     * timer callback
     *
     * @since S60 v3.1
     */
    virtual void OnTimeout( TWlanTimer aTimer ) = 0;

    };

#endif      // WLANTIMERCLIENT_H
            
