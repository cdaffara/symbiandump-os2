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
* Description:  Statemachine for updating device settings
*
*/


#include "core_operation_update_device_settings.h"
#include "core_operation_update_power_mode.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_update_device_settings_c::core_operation_update_device_settings_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation ) :
    core_operation_base_c( core_operation_update_device_settings, request_id, server, drivers, adaptation,
        core_base_flag_none )
    {
    DEBUG( "core_operation_update_device_settings_c::core_operation_update_device_settings_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_update_device_settings_c::~core_operation_update_device_settings_c()
    {
    DEBUG( "core_operation_update_device_settings_c::~core_operation_update_device_settings_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_update_device_settings_c::next_state()
    {
    DEBUG( "core_operation_update_device_settings_c::next_state()" );

    switch( operation_state_m )
        {
        case core_state_init:
            {
            operation_state_m = core_state_req_set_power_mode;

            /**
             * Currently power save mode is the only device setting that will cause
             * an immediate operation.
             * 
             * However, we do not want to disturb DHCP negotiation.
             */
            if( !server_m->is_dhcp_timer_active() )
                {
                core_operation_base_c* operation = new core_operation_update_power_mode_c(
                    request_id_m,
                    server_m,
                    drivers_m,
                    adaptation_m );
    
                return run_sub_operation( operation );
                }

            return core_error_ok;
            }
        case core_state_req_set_power_mode:
            {
            DEBUG( "core_operation_update_device_settings_c::next_state() - device settings updated" );

            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }
