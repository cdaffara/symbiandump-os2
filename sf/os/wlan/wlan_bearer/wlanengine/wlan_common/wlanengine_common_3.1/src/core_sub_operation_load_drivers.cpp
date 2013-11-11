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
* Description:  Statemachine for loading the drivers
*
*/


#include "core_sub_operation_load_drivers.h"
#include "core_operation_update_rxtx_parameters.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_load_drivers_c::core_sub_operation_load_drivers_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation ) :
    core_operation_base_c( core_operation_unspecified, request_id, server, drivers, adaptation,
        core_base_flag_none ),
    region_m( core_wlan_region_etsi ),
    mcc_known_m( false_t )
    {
    DEBUG( "core_sub_operation_load_drivers_c::core_sub_operation_load_drivers_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_load_drivers_c::~core_sub_operation_load_drivers_c()
    {
    DEBUG( "core_sub_operation_load_drivers_c::~core_sub_operation_load_drivers_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_load_drivers_c::next_state()
    {
    DEBUG( "core_sub_operation_load_drivers_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            if ( !server_m->get_core_settings().is_wlan_enabled() )
                {
                DEBUG( "core_sub_operation_load_drivers_c::next_state() - WLAN is disabled, drivers cannot be loaded" );
                return core_error_wlan_disabled;
                }

            if ( server_m->get_core_settings().is_driver_loaded() )
                {
                DEBUG( "core_sub_operation_load_drivers_c::next_state() - drivers already loaded" );
                return core_error_ok;
                }

            DEBUG( "core_sub_operation_load_drivers_c::next_state() - requesting region" );

            operation_state_m = core_state_req_region;

            adaptation_m->get_regulatory_domain(
                request_id_m,
                region_m,
                mcc_known_m );

            break;
            }
        case core_state_req_region:
            {            
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - current region %u",
                region_m );
            server_m->get_core_settings().set_regional_domain(
                region_m );
            server_m->get_core_settings().set_mcc_known( mcc_known_m );

            DEBUG( "core_sub_operation_load_drivers_c::next_state() - loading drivers" );

            operation_state_m = core_state_req_load_drivers;

            drivers_m->load_drivers(
                request_id_m,
                server_m->get_device_settings().rts,
                server_m->get_device_settings().max_tx_msdu_life_time,
                server_m->get_device_settings().qos_null_frame_entry_timeout,
                server_m->get_device_settings().qos_null_frame_entry_tx_count,
                server_m->get_device_settings().qos_null_frame_interval,
                server_m->get_device_settings().qos_null_frame_exit_timeout,
                server_m->get_device_settings().keep_alive_interval,
                server_m->get_device_settings().sp_rcpi_target,
                server_m->get_device_settings().sp_time_target,
                server_m->get_device_settings().sp_min_indication_interval,
                server_m->get_core_settings().enabled_features() );

            break;
            }
        case core_state_req_load_drivers:
            {
            server_m->get_core_settings().set_driver_state( true_t );

            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - setting tx power (%u)",
                server_m->get_device_settings().tx_power_level );

            operation_state_m = core_state_req_set_tx_power;

            drivers_m->set_tx_power_level(
                request_id_m,
                server_m->get_device_settings().tx_power_level );

            break;
            }
        case core_state_req_set_tx_power:
            {
            DEBUG( "core_sub_operation_load_drivers_c::next_state() - setting power mode to CORE_POWER_MODE_CAM" );

            operation_state_m = core_state_req_set_rxtx_parameters;

            server_m->get_core_settings().set_power_mode(
                CORE_POWER_MODE_CAM );

            drivers_m->set_power_mode(
                request_id_m,
                CORE_POWER_MODE_CAM );

            break;
            }
        case core_state_req_set_rxtx_parameters:
            {
            DEBUG( "core_sub_operation_load_drivers_c::next_state() - setting rxtx parameters" );

            operation_state_m = core_state_req_set_uapsd_settings;

            core_operation_base_c* operation = new core_operation_update_rxtx_parameters_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m );

            return run_sub_operation( operation );
            }            
        case core_state_req_set_uapsd_settings:
            {
            DEBUG( "core_sub_operation_load_drivers_c::next_state() - setting U-APSD settings" );

            operation_state_m = core_state_req_set_power_save_settings;

            const core_uapsd_settings_s& settings(
                server_m->get_core_settings().uapsd_settings() );

            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - max_service_period: %u",
                settings.max_service_period );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - uapsd_enabled_for_voice: %u",
                settings.uapsd_enabled_for_voice );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - uapsd_enabled_for_video: %u",
                settings.uapsd_enabled_for_video );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - uapsd_enabled_for_best_effort: %u",
                settings.uapsd_enabled_for_best_effort );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - uapsd_enabled_for_background: %u",
                settings.uapsd_enabled_for_background );

            drivers_m->set_uapsd_settings(
                request_id_m,
                settings );

            break;
            }
        case core_state_req_set_power_save_settings:
            {
            DEBUG( "core_sub_operation_load_drivers_c::next_state() - setting power save settings" );

            operation_state_m = core_state_req_set_power_mode_mgmt_settings;

            const core_power_save_settings_s& settings(
                server_m->get_core_settings().power_save_settings() );

            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - stay_in_uapsd_power_save_for_voice: %u",
                settings.stay_in_uapsd_power_save_for_voice );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - stay_in_uapsd_power_save_for_video: %u",
                settings.stay_in_uapsd_power_save_for_video );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - stay_in_uapsd_power_save_for_best_effort: %u",
                settings.stay_in_uapsd_power_save_for_best_effort );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - stay_in_uapsd_power_save_for_background: %u",
                settings.stay_in_uapsd_power_save_for_background );

            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - stay_in_legacy_power_save_for_voice: %u",
                settings.stay_in_legacy_power_save_for_voice );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - stay_in_legacy_power_save_for_video: %u",
                settings.stay_in_legacy_power_save_for_video );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - stay_in_legacy_power_save_for_best_effort: %u",
                settings.stay_in_legacy_power_save_for_best_effort );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - stay_in_legacy_power_save_for_background: %u",
                settings.stay_in_legacy_power_save_for_background );

            drivers_m->set_power_save_settings(
                request_id_m,
                settings );

            break;
            }
        case core_state_req_set_power_mode_mgmt_settings:
            {
            DEBUG( "core_sub_operation_load_drivers_c::next_state() - setting power mode mgmt settings" );

            operation_state_m = core_state_req_set_block_ack_usage;

            core_power_mode_mgmt_parameters_s settings;
            settings.active_to_light_timeout =
                server_m->get_device_settings().ps_active_to_light_timeout;
            settings.active_to_light_threshold =
                server_m->get_device_settings().ps_active_to_light_threshold;
            settings.light_to_active_timeout =
                server_m->get_device_settings().ps_light_to_active_timeout;
            settings.light_to_active_threshold =
                server_m->get_device_settings().ps_light_to_active_threshold;
            settings.light_to_deep_timeout =
                server_m->get_device_settings().ps_light_to_deep_timeout;
            settings.light_to_deep_threshold =
                server_m->get_device_settings().ps_light_to_deep_threshold;
            settings.uapsd_rx_frame_length_threshold =
                server_m->get_device_settings().ps_uapsd_rx_frame_length;

            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - active_to_light_timeout: %u",
                settings.active_to_light_timeout );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - active_to_light_threshold: %u",
                settings.active_to_light_threshold );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - light_to_active_timeout: %u",
                settings.light_to_active_timeout );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - light_to_active_threshold: %u",
                settings.light_to_active_threshold );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - light_to_deep_timeout: %u",
                settings.light_to_deep_timeout );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - light_to_deep_threshold: %u",
                settings.light_to_deep_threshold );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - uapsd_rx_frame_length_threshold: %u",
                settings.uapsd_rx_frame_length_threshold );

            drivers_m->set_power_mode_mgmt_parameters(
                request_id_m,
                settings );

            break;
            }
        case core_state_req_set_block_ack_usage:
            {
            DEBUG( "core_sub_operation_load_drivers_c::next_state() - setting block ACK usage" );

            operation_state_m = core_state_req_set_wpx_settings;

            const core_block_ack_usage_s& usage(
                server_m->get_core_settings().block_ack_usage() );

            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - tx_usage: 0x%02X",
                usage.tx_usage );
            DEBUG1( "core_sub_operation_load_drivers_c::next_state() - rx_usage: 0x%02X",
                usage.rx_usage );

            drivers_m->set_block_ack_usage(
                request_id_m,
                usage );

            break;
            }
        case core_state_req_set_wpx_settings:
            {
            core_operation_base_c* operation =
                server_m->get_wpx_adaptation_instance().get_wpx_load_drivers_operation();
            if( !operation )
                {
                return goto_state( core_state_done );
                }

            DEBUG( "core_sub_operation_load_drivers_c::next_state() - setting WPX specific settings" );

            return run_sub_operation( operation, core_state_done );
            }
        case core_state_done:
            {
            DEBUG( "core_sub_operation_load_drivers_c::next_state() - all operations done" );

            return core_error_ok;  
            }
        case core_state_cancel_req_unload_drivers:
            {
            server_m->get_core_settings().set_driver_state( false_t );

            DEBUG( "core_sub_operation_load_drivers_c::next_state() - unloading done " );

            return failure_reason_m;
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
void core_sub_operation_load_drivers_c::user_cancel(
    bool_t /* do_graceful_cancel */ )
    {
    DEBUG( "core_sub_operation_load_drivers_c::user_cancel()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_load_drivers_c::cancel()
    {
    DEBUG( "core_sub_operation_load_drivers_c::cancel() " );

    switch ( operation_state_m )
        {
        case core_state_init:
            /** Falls through on purpose. */
        case core_state_req_region:
            /** Falls through on purpose. */
        case core_state_req_load_drivers:
            {
            /* Also in this case we need to make unload because it can happen that loading returns 
             * failure but there is still need to unload */
            
            DEBUG( "core_sub_operation_load_drivers_c::cancel() - unloading drivers, load failed" );
            
            operation_state_m = core_state_cancel_req_unload_drivers;
            
            drivers_m->unload_drivers(
                request_id_m );

            break;
            }
        case core_state_cancel_req_unload_drivers:
            {
            DEBUG( "core_sub_operation_load_drivers_c::cancel() - unloading failed, ignoring " );
            
            return failure_reason_m;
            }
        default:
            {            
            ASSERT( server_m->get_core_settings().is_driver_loaded() );
            DEBUG( "core_sub_operation_load_drivers_c::cancel() - unloading drivers " );
            
            operation_state_m = core_state_cancel_req_unload_drivers;
            
            drivers_m->unload_drivers(
                request_id_m );

            break;
            }
        }

    return core_error_request_pending;
    }
