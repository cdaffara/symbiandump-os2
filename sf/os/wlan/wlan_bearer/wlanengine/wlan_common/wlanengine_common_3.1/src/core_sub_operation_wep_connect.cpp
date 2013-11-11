/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State machine for connecting to a WEP network.
*
*/

/*
* %version: 6 %
*/

#include "core_sub_operation_wep_connect.h"
#include "core_sub_operation_connect.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_wep_connect_c::core_sub_operation_wep_connect_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    bool_t& is_connected,
    core_management_status_e& connect_status,
    const core_ssid_s& ssid,
    core_ap_data_c& ap_data,
    core_authentication_mode_e auth_mode,
    core_encryption_mode_e encryption_level,
    core_type_list_c<core_frame_dot11_ie_c>& assoc_ie_list,
    core_frame_assoc_resp_c** assoc_resp ) :
    core_operation_base_c( core_operation_unspecified, request_id, server, drivers, adaptation,
        core_base_flag_drivers_needed ),
    is_connected_m( is_connected ),
    connect_status_m( connect_status ),
    ssid_m( ssid ),
    ap_data_m( ap_data ),
    initial_auth_mode_m( auth_mode ),
    auth_mode_m( auth_mode ),
    encryption_m( encryption_level ),
    assoc_ie_list_m( assoc_ie_list ),
    assoc_resp_m( assoc_resp )
    {
    DEBUG( "core_sub_operation_wep_connect_c::core_sub_operation_wep_connect_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_wep_connect_c::~core_sub_operation_wep_connect_c()
    {
    DEBUG( "core_sub_operation_wep_connect_c::~core_sub_operation_wep_connect_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_wep_connect_c::next_state()
    {
    DEBUG( "core_sub_operation_wep_connect_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            ASSERT( auth_mode_m == core_authentication_mode_open ||
                auth_mode_m == core_authentication_mode_shared );

            if ( auth_mode_m == core_authentication_mode_open )
                {
                DEBUG( "core_sub_operation_wep_connect_c::next_state() - attempting association with core_authentication_mode_open" );
                }
            else
                {
                DEBUG( "core_sub_operation_wep_connect_c::next_state() - attempting association with core_authentication_mode_shared" );
                }

            core_operation_base_c* operation = new core_sub_operation_connect_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                is_connected_m,
                connect_status_m,
                ssid_m,
                ap_data_m,
                auth_mode_m,
                server_m->get_wpx_adaptation_instance().encryption_mode(
                    ap_data_m, encryption_m ),
                core_cipher_key_type_none,
                assoc_ie_list_m,
                assoc_resp_m,
                true_t,    // ignored, pairwise key not used
                false_t,
                NULL );

            return run_sub_operation( operation, core_state_connect );
            }
        case core_state_connect:
            {
            DEBUG( "core_sub_operation_wep_connect_c::next_state() - association success" );

            return core_error_ok;
            }
        case core_state_connect_failed:
            {
            DEBUG( "core_sub_operation_wep_connect_c::next_state() - association failed" );

            DEBUG1( "core_sub_operation_wep_connect_c::next_state() - failure_reason_m is %u",
                failure_reason_m );
            DEBUG1( "core_sub_operation_wep_connect_c::next_state() - management_status_m is %u",
                connect_status_m );

            /**
             * We are only interested in management status codes that indicate
             * that the AP doesn't support the selected authentication mode.
             */
            if ( connect_status_m == core_management_status_auth_algo_not_supported &&
                 auth_mode_m == initial_auth_mode_m )
                {
                if ( auth_mode_m == core_authentication_mode_open )
                    {
                    DEBUG( "core_sub_operation_wep_connect_c::next_state() - core_authentication_mode_open failed, trying with core_authentication_mode_shared" );
                    auth_mode_m = core_authentication_mode_shared;
                    }
                else
                    {
                    DEBUG( "core_sub_operation_wep_connect_c::next_state() - core_authentication_mode_shared failed, trying with core_authentication_mode_open" );
                    auth_mode_m = core_authentication_mode_open;
                    }

                /**
                 * Update the used authentication mode to IAP data so that
                 * processing in parent operations is done correctly.
                 */
                server_m->get_connection_data()->iap_data().set_authentication_mode( auth_mode_m );

                return goto_state( core_state_init );
                }

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
core_error_e core_sub_operation_wep_connect_c::cancel()
    {
    switch ( operation_state_m )
        {
        case core_state_connect:
            {
            return goto_state( core_state_connect_failed );
            }
        default:
            {
            return failure_reason_m;
            }
        }
    }
