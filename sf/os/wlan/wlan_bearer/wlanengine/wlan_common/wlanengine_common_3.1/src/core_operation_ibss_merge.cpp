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
* Description:  Statemachine for merging existing IBSS (ad hoc) network to another one
*
*/

/*
* %version: 16 %
*/

#include "core_operation_ibss_merge.h"
#include "core_operation_roam.h"
#include "core_tools_parser.h"
#include "core_ap_data.h"
#include "core_tools.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_ibss_merge_c::core_operation_ibss_merge_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_frame_dot11_c* frame ) :
    core_operation_base_c( core_operation_ibss_merge, request_id, server, drivers, adaptation, 
        core_base_flag_drivers_needed | core_base_flag_connection_needed ),
    frame_m( frame ),
    is_connected_m( true_t )
    {
    DEBUG( "core_operation_ibss_merge_c::core_operation_ibss_merge_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_ibss_merge_c::~core_operation_ibss_merge_c()
    {
    DEBUG( "core_operation_ibss_merge_c::~core_operation_ibss_merge_c()" );
    frame_m = NULL;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_ibss_merge_c::next_state()
    {
    DEBUG( "core_operation_ibss_merge_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            core_ap_data_c* ap_data = core_ap_data_c::instance(
                server_m->get_wpx_adaptation_instance(),
                frame_m,
                0,
                false_t );

            if ( !ap_data )
                {
                DEBUG( "core_operation_ibss_merge_c::next_state() - unable to instantiate core_ap_data_c" );
                return cancel();
                }

            server_m->get_scan_list().set_tag(
                core_scan_list_tag_ibss_merge );

            server_m->get_scan_list().update_entry( *ap_data );

            if ( core_tools_parser_c::is_ap_compatible_with_iap(
                server_m->get_wpx_adaptation_instance(),
                *ap_data,
                server_m->get_connection_data()->iap_data(),
                server_m->get_core_settings() ) != core_connect_ok )
                {
                delete ap_data;
                ap_data = NULL;
                
                DEBUG( "core_operation_ibss_merge_c::next_state() - IBSS merge not possible, network settings do not match the IAP" );
                return cancel();
                }

            if ( !server_m->get_core_settings().is_valid_channel(
                ap_data->band(),
                ap_data->channel() ) )
                {
                delete ap_data;
                ap_data = NULL;

                DEBUG( "core_operation_ibss_merge_c::next_state() - IBSS merge not possible, invalid channel" );
                return cancel();
                }
            
            delete ap_data;
            ap_data = NULL;

            operation_state_m = core_state_merge_done;

            DEBUG( "core_operation_ibss_merge_c::next_state() - trying an IBSS merge" );

            core_operation_base_c* operation = new core_operation_roam_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                is_connected_m,
                core_scan_list_tag_ibss_merge,
                RCPI_VALUE_NONE,
                MEDIUM_TIME_NOT_DEFINED,                
                server_m->get_connection_data()->ssid(),
                BROADCAST_MAC_ADDR );
 
            return run_sub_operation( operation );
            }
        case core_state_merge_done:
            {
            DEBUG( "core_operation_ibss_merge_c::next_state() - IBSS merge done successfully" );
            
            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }
