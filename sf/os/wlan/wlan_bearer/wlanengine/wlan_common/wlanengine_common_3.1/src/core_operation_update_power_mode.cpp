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
* Description:  Statemachine for updating power mode
*
*/


#include "core_operation_update_power_mode.h"
#include "core_server.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_update_power_mode_c::core_operation_update_power_mode_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation ) :
    core_operation_base_c( core_operation_update_power_mode, request_id, server, drivers, adaptation,
        core_base_flag_connection_needed | core_base_flag_only_one_instance ),
    preferred_mode_m(
        server_m->get_core_settings().preferred_power_save_mode() ),
    power_mode_m( CORE_POWER_MODE_CAM )
    {
    DEBUG( "core_operation_update_power_mode_c::core_operation_update_power_mode_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_update_power_mode_c::core_operation_update_power_mode_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    const core_power_save_mode_s& mode ) :
    core_operation_base_c( core_operation_update_power_mode, request_id, server, drivers, adaptation,
        core_base_flag_connection_needed | core_base_flag_only_one_instance ),
    preferred_mode_m( mode ),
    power_mode_m( CORE_POWER_MODE_CAM )
    {
    DEBUG( "core_operation_update_power_mode_c::core_operation_update_power_mode_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_update_power_mode_c::~core_operation_update_power_mode_c()
    {
    DEBUG( "core_operation_update_power_mode_c::~core_operation_update_power_mode_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_update_power_mode_c::next_state()
    {
    DEBUG( "core_operation_update_power_mode_c::next_state()" );
    
    switch ( operation_state_m )
        {
        case core_state_init:
            {
            operation_state_m = core_state_req_set_power_mode;

            if ( !server_m->get_core_settings().is_driver_loaded() ||
                 !server_m->get_connection_data() )
                {
                DEBUG( "core_operation_update_power_mode_c::next_state() - not connected, no reason to set power save mode" );

                return core_error_ok;
                }

            /**
             * Determine the correct mode to use.
             */
            power_mode_m = determine_power_mode();

            const core_power_mode_s& current_mode(
                server_m->get_core_settings().power_mode() );

            if( power_mode_m.mode == current_mode.mode &&
                power_mode_m.wakeup_mode_light == current_mode.wakeup_mode_light &&
                power_mode_m.wakeup_interval_light == current_mode.wakeup_interval_light &&
                power_mode_m.wakeup_mode_deep == current_mode.wakeup_mode_deep &&
                power_mode_m.wakeup_interval_deep == current_mode.wakeup_interval_deep )                
                {
                DEBUG1( "core_operation_update_power_mode_c::next_state() - power save mode (%u) already set",
                    power_mode_m.mode );

                return core_error_ok;
                }

#ifdef _DEBUG
            if( power_mode_m.mode == core_power_mode_cam )
                {
                DEBUG( "core_operation_update_power_mode_c::next_state() - setting power mode to core_power_mode_cam" );
                }
            else
                {
                DEBUG4( "core_operation_update_power_mode_c::next_state() - setting power mode to core_power_mode_ps (light %u, %u) (deep %u, %u)",
                    power_mode_m.wakeup_mode_light, power_mode_m.wakeup_interval_light,
                    power_mode_m.wakeup_mode_deep, power_mode_m.wakeup_interval_deep );
                }
#endif // _DEBUG

            drivers_m->set_power_mode(
                request_id_m,
                power_mode_m );

            break;
            }
        case core_state_req_set_power_mode:
            {
            DEBUG1( "core_operation_update_power_mode_c::next_state() - power save mode (%u) successfully set",
                power_mode_m.mode );

            server_m->get_core_settings().set_power_mode( power_mode_m );

            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_power_mode_s core_operation_update_power_mode_c::determine_power_mode() const
    {
    DEBUG( "core_operation_update_power_mode_c::determine_power_mode()" );

    if( !server_m->get_device_settings().power_save_enabled )
        {
        DEBUG( "core_operation_update_power_mode_c::determine_power_mode() - CORE_POWER_MODE_CAM, disabled in settings" );
        
        return CORE_POWER_MODE_CAM;
        }

    if ( server_m->get_connection_data() &&
         server_m->get_connection_data()->iap_data().operating_mode() == core_operating_mode_ibss )
        {
        DEBUG( "core_operation_update_power_mode_c::determine_power_mode() - CORE_POWER_MODE_CAM, IBSS network" );

        return CORE_POWER_MODE_CAM;
        }

    if( preferred_mode_m.mode == core_power_save_mode_none )
        {
        DEBUG( "core_operation_update_power_mode_c::determine_power_mode() - CORE_POWER_MODE_CAM, preferred mode" );

        return CORE_POWER_MODE_CAM;        
        }

    if( preferred_mode_m.mode == core_power_save_mode_beacon )
        {
        DEBUG( "core_operation_update_power_mode_c::determine_power_mode() - CORE_POWER_MODE_PS_BEACON, preferred mode" );

        core_power_mode_s mode( CORE_POWER_MODE_PS_BEACON );
        mode.wakeup_interval_light = preferred_mode_m.wakeup_interval;
        mode.wakeup_interval_deep = preferred_mode_m.wakeup_interval;

        return mode;
        }

    if( preferred_mode_m.mode == core_power_save_mode_dtim )
        {
        DEBUG( "core_operation_update_power_mode_c::determine_power_mode() - CORE_POWER_MODE_PS_DTIM, preferred mode" );

        core_power_mode_s mode( CORE_POWER_MODE_PS_DTIM );
        mode.wakeup_interval_light = preferred_mode_m.wakeup_interval;
        mode.wakeup_interval_deep = preferred_mode_m.wakeup_interval;

        return mode;
        }

    u8_t wakeup_interval( 1 );
    bool_t is_power_save_test_success( true_t );
    if( server_m->get_connection_data() &&
        server_m->get_connection_data()->current_ap_data() )
        {                
        u32_t dtim_skip_interval(
            server_m->get_connection_data()->current_ap_data()->dtim_period() *
            server_m->get_connection_data()->current_ap_data()->beacon_interval() *
            MILLISECONDS_FROM_MICROSECONDS );

        /**
         * DTIM period can be zero if we haven't received a beacon from
         * the AP yet.
         */        
        if( dtim_skip_interval )
            {
            DEBUG1( "core_operation_update_power_mode_c::determine_power_mode() - max_dtim_skip_interval: %u",
                server_m->get_device_settings().max_dtim_skip_interval );
            DEBUG1( "core_operation_update_power_mode_c::determine_power_mode() - dtim_skip_interval: %u",
                dtim_skip_interval );

            while( dtim_skip_interval * ( wakeup_interval + 1 ) <= server_m->get_device_settings().max_dtim_skip_interval )
                {
                ++wakeup_interval;
                }
            }

        core_mac_address_s bssid(
            server_m->get_connection_data()->current_ap_data()->bssid() );
        server_m->get_connection_data()->is_ap_power_save_test_run(
            bssid, is_power_save_test_success );
        }

    if( preferred_mode_m.mode == core_power_save_mode_dtim_skipping )
        {
        DEBUG( "core_operation_update_power_mode_c::determine_power_mode() - CORE_POWER_MODE_PS_DTIM, preferred mode" );

        core_power_mode_s mode( CORE_POWER_MODE_PS_DTIM );
        mode.wakeup_interval_light = wakeup_interval;
        mode.wakeup_interval_deep = wakeup_interval;

        return mode;
        }

    if( !is_power_save_test_success )
        {
        DEBUG( "core_operation_update_power_mode_c::determine_power_mode() - CORE_POWER_MODE_CAM, AP power save test failure" );

        return CORE_POWER_MODE_CAM;
        }

    DEBUG( "core_operation_update_power_mode_c::determine_power_mode() - CORE_POWER_MODE_PS" );

    core_power_mode_s mode( CORE_POWER_MODE_PS );
    mode.wakeup_interval_deep = wakeup_interval;

    return mode;
    }
