/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Static factory class for instantiating a class 
*                implementing abs_core_server_c interface
*
*/


#ifndef CORE_SERVER_FACTORY_H
#define CORE_SERVER_FACTORY_H

#include "abs_core_server.h"
#include "abs_core_server_callback.h"
#include "abs_core_driverif.h"

/**
 * Factory for instantiating a class implementing abs_core_server_c interface.
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
class core_server_factory_c
    {

public:

    /**
     * Factory method used for creating a class implementing
     * abs_core_server_c interface.
     *
     * @since S60 v3.1
     * @param callback Reference to the class implementing abs_core_server_callback_c.
     * @param drivers Reference to the class implementing abs_core_driverif_c.
     * @param settings Initial settings for the WLAN service.
     * @param mac_address MAC address of the device.
     * @param features Bitmask of enabled features.
     */
    static abs_core_server_c* instance(
        abs_core_server_callback_c& callback,
        abs_core_driverif_c& drivers,
        core_device_settings_s& settings,
        core_mac_address_s& mac_address,
        u32_t features = core_feature_none );

private:

    /**
     * Private constructor to prevent instantiation.
     */
    core_server_factory_c();

    };

#endif // CORE_SERVER_FACTORY_H
