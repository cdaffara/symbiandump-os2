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


// ======== MEMBER FUNCTIONS ========

#include "core_server_factory.h"
#include "core_server.h"
#include "am_debug.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
abs_core_server_c* core_server_factory_c::instance(
    abs_core_server_callback_c& callback,
    abs_core_driverif_c& drivers,
    core_device_settings_s& settings,
    core_mac_address_s& mac_address,
    u32_t features )
    {
    DEBUG( "core_server_factory_c::instance() " );

    core_server_c* server = new core_server_c(
        callback, drivers, settings, mac_address, features );
    if ( !server )
        {
        DEBUG( "core_server_factory_c::instance() - unable to create core_server_c" );
        return NULL;
        }

    core_error_e ret = server->init();
    if ( ret != core_error_ok )
        {
        DEBUG1( "core_server_factory_c::instance() - init() failed with %u",
            ret );
        delete server;
        server = NULL;

        return NULL;
        }

    return server;
    }
