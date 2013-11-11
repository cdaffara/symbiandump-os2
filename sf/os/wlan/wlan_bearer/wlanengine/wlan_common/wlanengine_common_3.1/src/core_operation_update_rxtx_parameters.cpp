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
* Description:  Statemachine for updating rxtx parameters
*
*/


#include "core_operation_update_rxtx_parameters.h"
#include "core_operation_update_tx_rate_policies.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

/** Beacon lost value to be used when BT connection is disabled. */
const u32_t CORE_BEACON_LOST_COUNT_NO_BT = 15;
/** Beacon lost value to be used when BT connection is active. */
const u32_t CORE_BEACON_LOST_COUNT_BT = 20;
/**
 * Number of consecutive packets that have to be lost before
 * core_am_indication_wlan_tx_fail is indicated.
 */
const u32_t CORE_FAILED_TX_PACKETS_COUNT = 4;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_update_rxtx_parameters_c::core_operation_update_rxtx_parameters_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation ) :
    core_operation_base_c( core_operation_update_rxtx_parameters, request_id, server, drivers, adaptation,
        core_base_flag_only_one_instance )
    {
    DEBUG( "core_operation_update_rxtx_parameters_c::core_operation_update_rxtx_parameters_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_update_rxtx_parameters_c::~core_operation_update_rxtx_parameters_c()
    {
    DEBUG( "core_operation_update_rxtx_parameters_c::~core_operation_update_rxtx_parameters_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_update_rxtx_parameters_c::next_state()
    {
    DEBUG( "core_operation_update_rxtx_parameters_c::next_state()" );
    
    switch ( operation_state_m )
        {
        case core_state_init:
            {                        
            operation_state_m = core_state_req_set_bss_lost_parameters;

            if( !server_m->get_core_settings().is_driver_loaded() )
                {
                DEBUG( "core_operation_update_rxtx_parameters_c::next_state() - driver not loaded, completing operation" );
                return core_error_ok;
                }

            core_bss_lost_parameters_s parameters =
                { CORE_BEACON_LOST_COUNT_NO_BT, CORE_FAILED_TX_PACKETS_COUNT };
            if ( server_m->get_core_settings().is_bt_connection_established() )
                {
                DEBUG( "core_operation_update_rxtx_parameters_c::next_state() - BT connection on, adjusting beacon lost count" );
                parameters.beacon_lost_count = CORE_BEACON_LOST_COUNT_BT; 
                }

            DEBUG( "core_operation_update_rxtx_parameters_c::next_state() - setting bss lost parameters" );
            DEBUG1( "core_operation_update_rxtx_parameters_c::next_state() - beacon_lost_count = %u",
                parameters.beacon_lost_count );
            DEBUG1( "core_operation_update_rxtx_parameters_c::next_state() - failed_tx_packet_count = %u",
                parameters.failed_tx_packet_count );

            drivers_m->set_bss_lost_parameters(
                request_id_m,
                parameters );

            break;
            }
        case core_state_req_set_bss_lost_parameters:
            {
            operation_state_m = core_state_req_set_tx_rate_policies;

            if ( server_m->get_core_settings().is_connected() &&
                 server_m->get_connection_data() &&
                 server_m->get_connection_data()->current_ap_data() )
                {
                core_operation_base_c* operation = new core_operation_update_tx_rate_policies_c(
                    request_id_m,
                    server_m,
                    drivers_m,
                    adaptation_m,
                    *server_m->get_connection_data()->current_ap_data() );

                return run_sub_operation( operation );
                }

            DEBUG( "core_operation_update_rxtx_parameters_c::next_state() - not connected, no need to set TX rate policies" );

            return goto_state( core_state_req_set_tx_rate_policies );
            }
        case core_state_req_set_tx_rate_policies:
            {
            DEBUG( "core_operation_update_rxtx_parameters_c::next_state() - all set" );

            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }
