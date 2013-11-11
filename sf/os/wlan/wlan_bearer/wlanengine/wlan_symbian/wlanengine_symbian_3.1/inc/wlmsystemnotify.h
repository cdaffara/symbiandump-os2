/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition for the system data notification service callback interface.
*
*/


#ifndef WLMSYSTEMNOTIFY_H
#define WLMSYSTEMNOTIFY_H

#include "wlmclientserver.h"
#include "wlanerrorcodes.h"

enum TWlmIconStatus
    {
    EWlmIconStatusNotAvailable,
    EWlmIconStatusAvailable,
    EWlmIconStatusConnected,
    EWlmIconStatusConnectedSecure
    };

/**
 * WLAN on/off states are used as status codes for status
 * requests and therefore they are mapped directly to
 * corresponding error codes.
 */
enum TWlanOnOffState
    {
    EWlanOff = KErrWlanOff,
    EWlanForceOff = KErrWlanForceOff,
    EWlanOn = 0
    };

/** 
* Callback interface for system data notification services.
*
* These virtual methods should be implemented by the client if it
* needs any notifications.
* @lib
* @since Series 60 3.0
*/
class MWlmSystemNotify
    {
    public:

        /**
        * System Mode has changed.
        */
        virtual void SystemModeChanged(
            TWlanSystemMode /* aOldMode */,
            TWlanSystemMode /* aNewMode */ ) = 0;

        /**
        * Emergency call has started, WLAN is forced to be enabled.
        */
        virtual void EmergencyCallEstablished() = 0;
        
        /** 
        * Emergency call has ended, WLAN is not forced to be enabled anymore.
        */
        virtual void EmergencyCallCompleted( TWlanSystemMode /* aNewMode */ ) = 0;
                       
        /**
        * BT connection has been established.
        */
        virtual void BtConnectionEstablished() = 0;
        
        /**
        * BT connection has been disconnected.        
        */
        virtual void BtConnectionDisabled() = 0;
        
        /**
         * WLAN has been set ON.
         */
        virtual void WlanOn() = 0;
        
        /**
         * WLAN has been set OFF.
         */
        virtual void WlanOff() = 0;
    };

#endif // WLMSYSTEMNOTIFY_H
