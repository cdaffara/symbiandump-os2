/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  State machine for a directed roam request.
*
*/

/*
* %version: 3 %
*/

#include "core_operation_directed_roam.h"
#include "core_operation_roam.h"
#include "core_operation_handle_bss_lost.h"
#include "core_sub_operation_roam_scan.h"
#include "core_server.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_directed_roam_c::core_operation_directed_roam_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    const core_mac_address_s& bssid ) :
    core_operation_base_c( core_operation_directed_roam, request_id, server, drivers, adaptation,
        core_base_flag_drivers_needed | core_base_flag_connection_needed | core_base_flag_roam_operation ),
    bssid_m( bssid ),
    current_rcpi_m( 0 ),
    is_connected_m( true_t ) 
    {
    DEBUG( "core_operation_directed_roam_c::core_operation_directed_roam_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_directed_roam_c::~core_operation_directed_roam_c()
    {
    DEBUG( "core_operation_directed_roam_c::~core_operation_directed_roam_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_directed_roam_c::next_state()
    {
    DEBUG( "core_operation_directed_roam_c::next_state()" );
    
    switch ( operation_state_m )
        {
        case core_state_init:
            {
            if( !server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_directed_roam_c::next_state() - not connected, nothing to do" );

                return core_error_general;
                }

            operation_state_m = core_state_scan_start;

            core_mac_address_s current_bssid =
                server_m->get_connection_data()->current_ap_data()->bssid();

            DEBUG6( "core_operation_directed_roam_c::next_state() - current BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
                current_bssid.addr[0], current_bssid.addr[1], current_bssid.addr[2],
                current_bssid.addr[3], current_bssid.addr[4], current_bssid.addr[5] );
            DEBUG6( "core_operation_directed_roam_c::next_state() - requested BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
                bssid_m.addr[0], bssid_m.addr[1], bssid_m.addr[2],
                bssid_m.addr[3], bssid_m.addr[4], bssid_m.addr[5] );

            DEBUG( "core_operation_directed_roam_c::next_state() - requesting RCPI" );

            drivers_m->get_current_rcpi(
                request_id_m,
                current_rcpi_m );

            break;
            }
        case core_state_scan_start:
            {
            DEBUG1( "core_operation_directed_roam_c::next_state() - current RCPI is %u",
                current_rcpi_m );

            DEBUG( "core_operation_directed_roam_c::next_state() - starting a direct scan on all channels" );           

            server_m->get_scan_list().set_tag(
                core_scan_list_tag_roam_scan );

            core_operation_base_c* operation = new core_sub_operation_roam_scan_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                server_m->get_connection_data()->ssid(),
                server_m->get_core_settings().all_valid_scan_channels(),
                true_t,
                false_t );

            return run_sub_operation( operation, core_state_scan_complete );
            }
        case core_state_scan_complete:
            {
            DEBUG( "core_operation_directed_roam_c::next_state() - attempting to roam" );

            server_m->get_connection_data()->set_last_roam_reason(
                core_roam_reason_directed_roam );               
            server_m->get_connection_data()->set_last_roam_failed_reason(
                core_roam_failed_reason_none );

            core_operation_base_c* operation = NULL;
            if( bssid_m == BROADCAST_MAC_ADDR )
                {
                /**
                 * Directed roam request with no BSSID specified.
                 * 
                 * Any suitable AP that is better than the current AP will do. 
                 */
                medium_time_s admitted_medium_time(
                    server_m->get_connection_data()->traffic_stream_list().admitted_medium_time() );

                operation = new core_operation_roam_c(
                    request_id_m,
                    server_m,
                    drivers_m,
                    adaptation_m,
                    is_connected_m,
                    core_scan_list_tag_roam_scan,
                    current_rcpi_m + server_m->get_device_settings().rcpi_difference,
                    admitted_medium_time,
                    server_m->get_connection_data()->ssid(),
                    BROADCAST_MAC_ADDR );
                }
            else
                {
                /**
                 * Directed roam request to a certain BSSID.
                 */
                operation = new core_operation_roam_c(
                    request_id_m,
                    server_m,
                    drivers_m,
                    adaptation_m,
                    is_connected_m,
                    core_scan_list_tag_roam_scan,
                    server_m->get_device_settings().rcpi_trigger,
                    MEDIUM_TIME_NOT_DEFINED,
                    server_m->get_connection_data()->ssid(),
                    bssid_m );
                }

            return run_sub_operation( operation, core_state_connect_success );
            }                                   
        case core_state_connect_success:
            {
            operation_state_m = core_state_rcpi_trigger;

            DEBUG( "core_operation_directed_roam_c::next_state() - roamed successfully" );

            server_m->cancel_roam_timer();

            DEBUG( "core_operation_directed_roam_c::next_state() - reseting RCPI roam check interval" );
            server_m->get_connection_data()->reset_rcpi_roam_interval();

            DEBUG1( "core_operation_directed_roam_c::next_state() - internal RCPI threshold level is %u",
                server_m->get_device_settings().rcpi_trigger );
            DEBUG1( "core_operation_directed_roam_c::next_state() - external RCPI threshold level is %u",
                server_m->get_core_settings().rcp_decline_boundary() );

            u8_t trigger_level = server_m->get_core_settings().rcp_decline_boundary();
            if ( trigger_level < server_m->get_device_settings().rcpi_trigger )
                {
                trigger_level = server_m->get_device_settings().rcpi_trigger;
                }
            DEBUG1( "core_operation_directed_roam_c::next_state() - arming RCPI roam trigger (%u)",
                trigger_level );

            drivers_m->set_rcpi_trigger_level(
                request_id_m,
                trigger_level );

            break;
            }
        case core_state_connect_failure:
            {
            DEBUG( "core_operation_directed_roam_c::next_state() - roaming failed" );

            if( !is_connected_m )
                {
                DEBUG( "core_operation_directed_roam_c::next_state() - no longer connected, scheduling bss_lost operation" );

                core_operation_base_c* operation = new core_operation_handle_bss_lost_c(
                    REQUEST_ID_CORE_INTERNAL,
                    server_m,
                    drivers_m,
                    adaptation_m,
                    core_operation_handle_bss_lost_c::core_bss_lost_reason_failed_directed_roam );

                server_m->queue_int_operation( operation );
                }

            return core_error_general;
            }
        case core_state_rcpi_trigger:
            {
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
core_error_e core_operation_directed_roam_c::cancel()
    {
    DEBUG( "core_operation_directed_roam_c::cancel() " );    

    switch ( operation_state_m )
        {
        case core_state_connect_success:
            {            
            return goto_state( core_state_connect_failure );
            }
        default:
            {
            return failure_reason_m;
            }
        }    
    }
