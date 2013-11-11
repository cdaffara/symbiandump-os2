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
* Description:  Statemachine for doing scans when roaming
*
*/

/*
* %version: 22 %
*/

#include "core_sub_operation_roam_scan.h"
#include "core_frame_dot11.h"
#include "core_server.h"
#include "core_tools_parser.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_roam_scan_c::core_sub_operation_roam_scan_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    const core_ssid_s& scan_ssid,
    const core_scan_channels_s& scan_channels,
    bool_t is_connected,
    bool_t is_first_match_selected ) :    
    core_operation_base_c( core_operation_scan, request_id, server, drivers, adaptation,
        core_base_flag_drivers_needed ),
    scan_ssid_m( scan_ssid ),
    scan_channels_m( scan_channels ),
    is_connected_m( is_connected ),
    is_first_match_selected( is_first_match_selected )
    {
    DEBUG( "core_sub_operation_roam_scan_c::core_sub_operation_roam_scan_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_roam_scan_c::~core_sub_operation_roam_scan_c()
    {
    DEBUG( "core_sub_operation_roam_scan_c::~core_sub_operation_roam_scan_c()" );

    server_m->unregister_event_handler( this );
    server_m->unregister_frame_handler( this );    
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_roam_scan_c::next_state()
    {
    DEBUG( "core_sub_operation_roam_scan_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {                        
            operation_state_m = core_state_scan_start;

            if ( scan_ssid_m.length )
                {
                DEBUG1S( "core_sub_operation_roam_scan_c::next_state() - requesting a direct scan with SSID ",
                    scan_ssid_m.length, &scan_ssid_m.ssid[0] );
                }
            else
                {
                DEBUG( "core_sub_operation_roam_scan_c::next_state() - requesting a broadcast scan" );
                }

            if ( is_connected_m )
                {
                DEBUG( "core_sub_operation_roam_scan_c::next_state() - requesting a split-scan" );
                }
            else
                {
                DEBUG( "core_sub_operation_roam_scan_c::next_state() - requesting a regular scan" );
                }

            DEBUG2( "core_sub_operation_roam_scan_c::next_state() - requesting scan on channels 0x%02X%02X",
                scan_channels_m.channels2dot4ghz[1],
                scan_channels_m.channels2dot4ghz[0] );

            server_m->get_scan_list().remove_entries_by_age(
                server_m->get_device_settings().scan_list_expiration_time );

            server_m->register_event_handler( this );
            server_m->register_frame_handler( this );

            drivers_m->scan(
                request_id_m,
                core_scan_mode_active,
                scan_ssid_m,
                server_m->get_device_settings().scan_rate,
                server_m->get_core_settings().valid_scan_channels( scan_channels_m ),
                server_m->get_device_settings().active_scan_min_ch_time,
                server_m->get_device_settings().active_scan_max_ch_time,
                is_connected_m );            

            break;
            }
        case core_state_scan_start:
            {
            operation_state_m = core_state_scan_started;

            DEBUG( "core_sub_operation_roam_scan_c::next_state() - scan start completed, waiting for scan completion" );

            break;
            }
        case core_state_scan_stop:
            {
            operation_state_m = core_state_scan_stopped;

            DEBUG( "core_sub_operation_roam_scan_c::next_state() - scan stop completed, waiting for scan completion" );

            break;
            }
        case core_state_scan_stopped:
            {
            DEBUG( "core_sub_operation_roam_scan_c::next_state() - invalid state transition in core_state_scan_stopped" );
            ASSERT( false_t );
            }
        case core_state_scan_complete:
            {
            server_m->unregister_frame_handler( this );

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
void core_sub_operation_roam_scan_c::user_cancel(
    bool_t do_graceful_cancel )
    {
    DEBUG( "core_sub_operation_roam_scan_c::user_cancel()" );

    if ( !do_graceful_cancel )
        {
        /**
         * If we are in a middle of a scan, we have to schedule our own
         * event.
         */
        if ( operation_state_m == core_state_scan_started )
            {
            asynch_default_user_cancel();
    
            return;
            }
    
        /**
         * Everything else is handled by the default implementation.
         */
        core_operation_base_c::user_cancel( do_graceful_cancel );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
bool_t core_sub_operation_roam_scan_c::receive_frame(
    const core_frame_dot11_c* frame,
    u8_t rcpi )
    {
    DEBUG( "core_sub_operation_roam_scan_c::receive_frame()" );
    
    if ( frame->type() != core_frame_dot11_c::core_dot11_type_beacon &&
         frame->type() != core_frame_dot11_c::core_dot11_type_probe_resp )
        {
        DEBUG( "core_sub_operation_roam_scan_c::receive_frame() - not a beacon or a probe" );        
        return false_t;
        }

    core_ap_data_c* ap_data = core_ap_data_c::instance(
        server_m->get_wpx_adaptation_instance(),
        frame,
        rcpi,
        false_t );
    if ( ap_data )
        {        
        const core_ssid_s ssid(
            ap_data->ssid() );

#ifdef _DEBUG
        DEBUG1S( "core_sub_operation_roam_scan_c::receive_frame() - SSID: ",
            ssid.length, &ssid.ssid[0] );

        const core_mac_address_s bssid(
            ap_data->bssid() );
        DEBUG6( "core_sub_operation_roam_scan_c::receive_frame() - BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
            bssid.addr[0], bssid.addr[1], bssid.addr[2], 
            bssid.addr[3], bssid.addr[4], bssid.addr[5] ); 
#endif // _DEBUG

        server_m->get_scan_list().update_entry( *ap_data );

        /**
         * The current implementation assumes that scan is only stopped
         * in an emergency situation so there's no need to check for
         * admission control parameters.
         */
        if ( operation_state_m == core_state_scan_started &&
             is_first_match_selected &&
             scan_ssid_m == ssid &&
             core_tools_parser_c::is_ap_suitable(
                 server_m->get_wpx_adaptation_instance(),
                 *ap_data,
                 server_m->get_connection_data()->iap_data(),
                 server_m->get_core_settings(),
                 *server_m->get_connection_data(),
                 server_m->get_device_settings().scan_stop_rcpi_threshold,
                 MEDIUM_TIME_NOT_DEFINED,
                 server_m->is_cm_active(),
                 ZERO_MAC_ADDR ) == core_connect_ok )
            {
            DEBUG( "core_sub_operation_roam_scan_c::receive_frame() - AP is suitable, requesting scan to be stopped" );

            operation_state_m = core_state_scan_stop;

            drivers_m->stop_scan(
                request_id_m );
            }
        else if ( operation_state_m == core_state_scan_start )
            {
            DEBUG( "core_sub_operation_roam_scan_c::receive_frame() - scan frame received while starting scan" );        
            }
        else if ( operation_state_m == core_state_scan_stop )
            {
            DEBUG( "core_sub_operation_roam_scan_c::receive_frame() - scan frame received while stopping scan" );
            }
        else if ( operation_state_m == core_state_scan_stopped )
            {
            DEBUG( "core_sub_operation_roam_scan_c::receive_frame() - scan frame after scan was stopped" );
            }

        delete ap_data;
        ap_data = NULL;
        }

    return true_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_sub_operation_roam_scan_c::notify(
    core_am_indication_e indication )
    {
    if ( indication == core_am_indication_wlan_scan_complete )
        {
        server_m->unregister_event_handler( this );
        
        if ( operation_state_m == core_state_scan_start )
            {
            DEBUG( "core_sub_operation_roam_scan_c::notify() - scan complete received while starting scan" );

            /**
             * No need to do anything here, state will be changed when the request completes.
             */
            operation_state_m = core_state_scan_complete;            
            }
        else if ( operation_state_m == core_state_scan_started )
            {            
            DEBUG( "core_sub_operation_roam_scan_c::notify() - scan complete" );

            asynch_goto( core_state_scan_complete, CORE_TIMER_IMMEDIATELY );
            }
        else if ( operation_state_m == core_state_scan_stop )
            {
            DEBUG( "core_sub_operation_roam_scan_c::notify() - scan complete received while stopping scan" );

            /**
             * No need to do anything here, state will be changed when the request completes.
             */
            operation_state_m = core_state_scan_complete;
            }
        else if ( operation_state_m == core_state_scan_stopped )
            {
            DEBUG( "core_sub_operation_roam_scan_c::notify() - scan complete after scan stop completed" );

            asynch_goto( core_state_scan_complete, CORE_TIMER_IMMEDIATELY );           
            }        
        else
            {
            DEBUG( "core_sub_operation_roam_scan_c::notify() - scan complete in unknown state" );
            ASSERT( false_t );
            }

        return true_t;
        }

    return false_t;
    }
