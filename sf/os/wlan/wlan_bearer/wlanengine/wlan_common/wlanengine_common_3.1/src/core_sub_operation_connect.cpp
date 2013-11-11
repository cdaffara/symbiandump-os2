/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for connecting to a network
*
*/

/*
* %version: 29 %
*/

#include "core_sub_operation_connect.h"
#include "core_operation_update_tx_rate_policies.h"
#include "core_frame_beacon.h"
#include "core_frame_dot11_ie.h"
#include "core_frame_assoc_resp.h"
#include "core_server.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_connect_c::core_sub_operation_connect_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    bool_t& is_connected,
    core_management_status_e& connect_status,
    const core_ssid_s& ssid,
    core_ap_data_c& ap_data,
    u16_t auth_algorithm,
    core_encryption_mode_e encryption_level,
    core_cipher_key_type_e pairwise_key_type,
    core_type_list_c<core_frame_dot11_ie_c>& assoc_ie_list,
    core_frame_assoc_resp_c** assoc_resp,
    bool_t is_pairwise_key_invalidated,
    bool_t is_group_key_invalidated,
    const core_cipher_key_s* pairwise_key ) :
    core_operation_base_c( core_operation_unspecified, request_id, server, drivers, adaptation,
        core_base_flag_drivers_needed ),
    is_connected_m( is_connected ),
    connect_status_m( connect_status ),
    ssid_m( ssid ),
    ap_data_m( ap_data ),
    auth_algorithm_m( auth_algorithm ),
    encryption_m( encryption_level ),
    pairwise_key_type_m( pairwise_key_type ),
    tx_level_m( 0 ),
    assoc_ie_list_m( assoc_ie_list ),
    assoc_ie_data_m( NULL ),
    assoc_resp_m( assoc_resp ),
    is_pairwise_key_invalidated_m( is_pairwise_key_invalidated ),
    is_group_key_invalidated_m( is_group_key_invalidated ),
    pairwise_key_m( pairwise_key )
    {
    DEBUG( "core_sub_operation_connect_c::core_sub_operation_connect_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_connect_c::~core_sub_operation_connect_c()
    {    
    DEBUG( "core_sub_operation_connect_c::~core_sub_operation_connect_c()" );

    server_m->unregister_frame_handler( this );
    delete[] assoc_ie_data_m;   
    assoc_resp_m = NULL;
    pairwise_key_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_connect_c::next_state()
    {
    DEBUG( "core_sub_operation_connect_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            operation_state_m = core_state_req_set_tx_level;

            tx_level_m = server_m->get_device_settings().tx_power_level;

            u32_t max_tx_power_level = ap_data_m.max_tx_power_level();
            if ( max_tx_power_level < tx_level_m )
                {
                tx_level_m = max_tx_power_level;
                DEBUG1( "core_sub_operation_connect_c::next_state() - setting maximum tx level to %u",
                    tx_level_m );

                drivers_m->set_tx_power_level(
                    request_id_m,
                    tx_level_m );                    
                }
            else
                {
                DEBUG( "core_sub_operation_connect_c::next_state() - no reason to set tx level" );

                return next_state();
                }

            break;
            }
        case core_state_req_set_tx_level:
            {
            operation_state_m = core_state_req_set_tx_rate_policies;

            if ( server_m->get_connection_data()->last_tx_level() != tx_level_m )
                {
                DEBUG( "core_sub_operation_connect_c::next_state() - TX level has changed, notifying change" );

                adaptation_m->notify(
                    core_notification_tx_power_level_changed,
                    sizeof( tx_level_m ),
                    reinterpret_cast<u8_t*>(&tx_level_m) );

                server_m->get_connection_data()->set_last_tx_level( tx_level_m );
                }

            core_operation_base_c* operation = new core_operation_update_tx_rate_policies_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                ap_data_m );

            return run_sub_operation( operation );
            }
        case core_state_req_set_tx_rate_policies:
            {
            operation_state_m = core_state_connect;

            DEBUG6( "core_sub_operation_connect_c::next_state() - trying to associate to %02X:%02X:%02X:%02X:%02X:%02X",
                ap_data_m.bssid().addr[0], ap_data_m.bssid().addr[1], ap_data_m.bssid().addr[2],
                ap_data_m.bssid().addr[3], ap_data_m.bssid().addr[4], ap_data_m.bssid().addr[5] );

            server_m->get_wpx_adaptation_instance().get_association_request_wpx_ie(
                ap_data_m,
                assoc_ie_list_m );

            if( ap_data_m.is_radio_measurement_supported() == true_t &&
                server_m->get_core_settings().is_feature_enabled( core_feature_802dot11k ) )
                {
                u8_t max_capability = ap_data_m.max_tx_power_level();

                /**
                 * Power capability IE.
                 */        
                core_frame_radio_mgmt_ie_c* capability_ie = core_frame_radio_mgmt_ie_c::instance(
                    CORE_FRAME_RADIO_MGMT_IE_LENGTH,
                    CORE_FRAME_RADIO_MGMT_IE_MIN_POWER_CAPABILITY,
                    max_capability );
                if ( capability_ie )
                    {
                    assoc_ie_list_m.append(
                        capability_ie,
                        capability_ie->element_id() );            
                    }

                /**
                 * RRM enabled capabilities IE.
                 */        
                const u64_t rrm_capability( RRM_CAPABILITY_BIT_MASK );

                core_frame_rrm_mgmt_ie_c* rrm_capability_ie = core_frame_rrm_mgmt_ie_c::instance(
                    reinterpret_cast<const u8_t*>(&rrm_capability) );
                if ( rrm_capability_ie )
                    {
                    assoc_ie_list_m.append(
                        rrm_capability_ie,
                        rrm_capability_ie->element_id() );            
                    }
                }

            /**
             * Copy all the IEs to a buffer.
             */
            u32_t assoc_ie_data_length( 0 );
            u32_t assoc_ie_data_index( 0 );
            
            core_frame_dot11_ie_c* ie = assoc_ie_list_m.first();
            while( ie )
                {
                assoc_ie_data_length += ie->data_length();

                ie = assoc_ie_list_m.next();
                }

            if ( assoc_ie_data_length )
                {
                assoc_ie_data_m = new u8_t[assoc_ie_data_length];
                ie = assoc_ie_list_m.first();
                while( ie )
                    {
                    DEBUG2( "core_sub_operation_connect_c::next_state() - adding IE ID %u (0x%02X)",
                        ie->element_id(), ie->element_id() );
                    DEBUG_BUFFER(
                        ie->data_length(),
                        ie->data() );

                    core_tools_c::copy(
                        &assoc_ie_data_m[assoc_ie_data_index],
                        ie->data(),
                        ie->data_length() );

                    assoc_ie_data_index += ie->data_length();
                    ie = assoc_ie_list_m.next();
                    }
                }
            else
                {
                DEBUG( "core_sub_operation_connect_c::next_state() - no IEs to add to the (re-)association request" );
                }

            server_m->get_core_settings().roam_metrics().set_roam_ts_connect_started();

            server_m->register_frame_handler( this );

            drivers_m->connect(                
                request_id_m,
                connect_status_m,
                ssid_m,
                ap_data_m.bssid(),
                auth_algorithm_m,
                encryption_m,
                pairwise_key_type_m,
                ap_data_m.is_infra(),
                assoc_ie_data_length,
                assoc_ie_data_m,
                ap_data_m.frame()->payload_data_length(),
                ap_data_m.frame()->payload_data(),
                is_pairwise_key_invalidated_m,
                is_group_key_invalidated_m,
                ap_data_m.is_radio_measurement_supported(),
                pairwise_key_m );

            break;    
            }
        case core_state_connect:
            {
            operation_state_m = core_state_connect_frame;

            /**
             * If (re-)association response frame is not needed or it has
             * already been received, proceed to the next state.
             */            
            if( !assoc_resp_m ||
                *assoc_resp_m )
                {
                return goto_state( core_state_connect_complete );
                }

            return goto_state( core_state_connect_frame );
            }
        case core_state_connect_frame:
            {
            DEBUG( "core_sub_operation_connect_c::next_state() - waiting for (re-)association response frame" );

            break;
            }
        case core_state_connect_complete:
            {
            server_m->unregister_frame_handler( this );
            server_m->get_core_settings().roam_metrics().set_roam_ts_connect_completed();
            server_m->get_wpx_adaptation_instance().handle_association_response(
                ap_data_m,
                assoc_resp_m ? *assoc_resp_m : NULL );

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
void core_sub_operation_connect_c::user_cancel(
    bool_t do_graceful_cancel )
    {
    DEBUG( "core_sub_operation_connect_c::user_cancel()" );

    if ( !do_graceful_cancel )
        {
        /**
         * If we are waiting for a beacon, we have to schedule our own
         * event.
         */
        if ( operation_state_m == core_state_connect_frame )
            {
            asynch_default_user_cancel();

            return;            
            }
        }

    /**
     * Everything else is handled by the default implementation.
     */
    core_operation_base_c::user_cancel( do_graceful_cancel );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_connect_c::cancel()
    {
    DEBUG( "core_sub_operation_connect_c::cancel() " );    
    
    switch ( operation_state_m )
        {              
        case core_state_connect:
            {
            /** The connection attempt failed, we are no longer connected. */
            is_connected_m = false_t;

            return goto_state( core_state_connect_complete );
            }
        default:
            {
            return failure_reason_m;
            }
        }    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
bool_t core_sub_operation_connect_c::receive_frame(
    const core_frame_dot11_c* frame,
    u8_t /* rcpi */ )
    {
    DEBUG( "core_sub_operation_connect_c::receive_frame()" );
    
    if ( frame->type() != core_frame_dot11_c::core_dot11_type_association_resp &&
         frame->type() != core_frame_dot11_c::core_dot11_type_reassociation_resp )
        {
        DEBUG( "core_sub_operation_connect_c::receive_frame() - not a (re-)association response" );
        return false_t;
        }

    if ( !assoc_resp_m )
        {
        DEBUG( "core_sub_operation_connect_c::receive_frame() - parent operation not interested in (re-)association response" );
        return true_t;
        }
    else if ( *assoc_resp_m )
        {
        DEBUG( "core_sub_operation_connect_c::receive_frame() - (re-)association response already received" );
        return true_t;       
        }    

    DEBUG( "core_sub_operation_connect_c::receive_frame() - (re-)association response received" );
    *assoc_resp_m = core_frame_assoc_resp_c::instance( *frame, true_t );

    /**
     * If we are just waiting for the response frame, we have to schedule
     * our own timer to proceed.
     */
    if( operation_state_m == core_state_connect_frame )
        {
        asynch_goto( core_state_connect_complete, CORE_TIMER_IMMEDIATELY );
        }

    return true_t;
    }
