/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for deleting a traffic stream.
*
*/

/*
* %version: 9 %
*/

#include "core_operation_delete_ts.h"
#include "core_traffic_stream.h"
#include "core_frame_wmm_ie_tspec.h"
#include "core_frame_action_wmm.h"
#include "core_server.h"
#include "core_tools.h"
#include "core_traffic_stream_list_iter.h"
#include "core_virtual_traffic_stream_list_iter.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_delete_ts_c::core_operation_delete_ts_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    u32_t stream_id ) :
    core_operation_base_c( core_operation_delete_ts, request_id, server, drivers, adaptation,
        core_base_flag_none ),
    id_m( stream_id )
    {
    DEBUG( "core_operation_delete_ts_c::core_operation_delete_ts_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_delete_ts_c::~core_operation_delete_ts_c()
    {
    DEBUG( "core_operation_delete_ts_c::~core_operation_delete_ts_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_delete_ts_c::next_state()
    {
    DEBUG( "core_operation_delete_ts_c::next_state()" );

    switch ( operation_state_m )
        {              
        case core_state_init:
            {
            operation_state_m = core_base_state_parameters_set;
            
            if( !server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_delete_ts_c::next_state() - not connected, nothing to do" );

                return core_error_general;
                }

            core_traffic_stream_list_c& ts_list(
                server_m->get_connection_data()->traffic_stream_list() );
            core_virtual_traffic_stream_list_c& virtual_ts_list(
                server_m->get_connection_data()->virtual_traffic_stream_list() );
            core_traffic_stream_list_iter_c ts_iter( ts_list );
            core_virtual_traffic_stream_list_iter_c virtual_ts_iter( virtual_ts_list );

            DEBUG( "core_operation_delete_ts_c::next_state() - current traffic streams:" );
            virtual_ts_list.print_contents();
            ts_list.print_contents();
            
            /**
             * Find the virtual traffic stream to be deleted.
             */
            core_virtual_traffic_stream_c* virtual_stream = NULL;
            core_virtual_traffic_stream_c* virtual_iter = virtual_ts_iter.first();
            while( virtual_iter )
                {
                if( virtual_iter->id() == id_m )
                    {
                    virtual_stream = virtual_iter;
                    virtual_iter = NULL;
                    }
                else
                    {
                    virtual_iter = virtual_ts_iter.next();
                    }
                }

            if( !virtual_stream )
                {
                DEBUG1( "core_operation_delete_ts_c::next_state() - no virtual traffic stream with ID %u found",
                    id_m );

                return core_error_not_found;
                }

            u8_t tid = virtual_stream->tid();
            
            DEBUG3( "core_operation_delete_ts_c::next_state() - virtual traffic stream with ID %u found, TID: %u, UP: %u",
                id_m, tid, virtual_stream->user_priority() );

            /**
             * Delete the virtual traffic stream.
             */
            virtual_ts_iter.remove();
            virtual_stream = NULL;
            
            /**
             * Locate the actual traffic stream based on the TID.
             */
            core_traffic_stream_c* stream = NULL;            
            core_traffic_stream_c* iter = ts_iter.first();
            while( iter )
                {
                if( iter->tid() == tid )
                    {
                    stream = iter;

                    iter = NULL;
                    }
                else
                    {
                    iter = ts_iter.next();
                    }
                }

            if( !stream )
                {
                DEBUG1( "core_operation_delete_ts_c::next_state() - no traffic stream with TID %u found",
                    id_m );

                /**
                 * This is not an error since the virtual traffic streams can
                 * exist eventhough the AP doesn't require admission control.
                 */

                return core_error_ok;
                }

            stream->dec_reference_count();
            if( stream->reference_count() )
                {
                DEBUG1( "core_operation_delete_ts_c::next_state() - traffic stream reference count is %u, not deleting",
                    stream->reference_count() );
                
                /**
                 * If there are still other virtual traffic streams referencing
                 * this traffic stream, do not delete it. 
                 */

                return core_error_ok;
                }

            u8_t user_priority = stream->user_priority();
            core_traffic_stream_direction_e direction = stream->direction();
            bool_t is_uapsd( true_t );
            if( !server_m->get_connection_data()->current_ap_data()->is_uapsd_supported() ||
                !server_m->get_core_settings().is_uapsd_enable_for_access_class(
                    core_tools_c::convert_user_priority_to_ac( user_priority ) ) )
                {
                is_uapsd = false_t;
                }
            DEBUG1( "core_operation_delete_ts_c::next_state() - U-APSD %u",
                is_uapsd );

            core_frame_wmm_ie_tspec_c* tspec_ie = core_frame_wmm_ie_tspec_c::instance(
                stream->tid(),
                user_priority,
                is_uapsd,
                stream->is_periodic_traffic(),
                stream->direction(),
                stream->nominal_msdu_size(),
                stream->maximum_msdu_size(),
                stream->minimum_service_interval(),
                stream->maximum_service_interval(),
                stream->inactivity_interval(),
                stream->suspension_interval(),
                stream->service_start_time(),
                stream->minimum_data_rate(),
                stream->mean_data_rate(),
                stream->peak_data_rate(),
                stream->maximum_burst_size(),
                stream->delay_bound(),
                stream->minimum_phy_rate(),
                stream->surplus_bandwidth_allowance(),
                stream->medium_time() );
            if( !tspec_ie )
                {
                DEBUG( "core_operation_delete_ts_c::next_state() - unable to allocate a WMM TSPEC IE" );
                return core_error_no_memory;
                }

            DEBUG( "core_operation_delete_ts_c::next_state() - TSPEC IE:" );
            DEBUG_BUFFER( tspec_ie->data_length(), tspec_ie->data() );

            const core_mac_address_s dest(
                server_m->get_connection_data()->current_ap_data()->bssid() );
            const core_mac_address_s src(
                server_m->own_mac_addr() );

            core_frame_action_wmm_c* frame = core_frame_action_wmm_c::instance(
                0,                  // Duration
                dest,               // Destination
                src,                // Source
                dest,               // BSSID
                0,                  // Sequence Control
                core_frame_action_wmm_c::core_dot11_action_wmm_type_delts, // Action Type
                0,                  // Dialog Token
                core_frame_action_wmm_c::core_dot11_action_wmm_status_admission_accepted, // Status Code
                tspec_ie );

            delete tspec_ie;
            tspec_ie = NULL;

            if( !frame )
                {
                DEBUG( "core_operation_delete_ts_c::next_state() - unable to allocate an action frame" );
                return core_error_no_memory;
                }

            DEBUG( "core_operation_delete_ts_c::next_state() - DELTS REQ:" );
            DEBUG_BUFFER( frame->data_length(), frame->data() );

            server_m->send_management_frame(
                core_frame_type_dot11,
                frame->data_length(),
                frame->data(),
                dest );

            delete frame;
            frame = NULL;

            DEBUG( "core_operation_delete_ts_c::next_state() - DELTS request sent" );

            server_m->get_wpx_adaptation_instance().handle_ts_delete(
                tid, user_priority );

            /**
             * If there no more manual virtual traffic streams in this AC,
             * switch the traffic mode back to automatic.
             */
            if( virtual_ts_list.traffic_mode_by_ac( stream->access_class() ) == core_access_class_traffic_mode_automatic )
                {
                server_m->get_connection_data()->set_ac_traffic_mode(
                    stream->access_class(),
                    core_access_class_traffic_mode_automatic );

                DEBUG( "core_operation_delete_ts_c::next_state() - traffic mode set to automatic for this AC, notifying clients" );

                u8_t buf[5];
                buf[0] = static_cast<u8_t>( 
                    stream->access_class() );
                buf[1] = static_cast<u8_t>(
                    core_access_class_traffic_mode_automatic );
                adaptation_m->notify(
                    core_notification_ac_traffic_mode_changed,
                    sizeof( buf ),
                    buf );                
                }

            /**
             * Delete the actual traffic stream.
             */
            ts_iter.remove();
            stream = NULL;

            if( direction == core_traffic_stream_direction_uplink ||
                direction == core_traffic_stream_direction_bidirectional )
                {
                server_m->get_connection_data()->set_ac_traffic_status(
                    core_tools_c::convert_user_priority_to_ac( user_priority ),
                    core_access_class_traffic_status_not_admitted );

                /**
                 * Send an indication to notify clients that this access class
                 * is no longer admitted.
                 * 
                 * We don't have to the check the ACM bits since the traffic
                 * stream wouldn't exist if the AP didn't require admission
                 * control.  
                 */
                DEBUG( "core_operation_delete_ts_c::next_state() - traffic no longer admitted on this AC, notifying clients" );

                u8_t buf[5];
                buf[0] = static_cast<u8_t>( 
                    core_tools_c::convert_user_priority_to_ac( user_priority ) );
                buf[1] = static_cast<u8_t>(
                    core_access_class_traffic_status_not_admitted );
                adaptation_m->notify(
                    core_notification_ac_traffic_status_changed,
                    sizeof( buf ),
                    buf );

                /**
                 * Reset TX queue parameters back to default values.
                 */
                DEBUG( "core_operation_delete_ts_c::next_state() - resetting tx queue parameters" ); 

                drivers_m->set_tx_queue_parameters(
                    request_id_m,
                    core_tools_c::convert_user_priority_to_ac( user_priority ),
                    0,
                    server_m->get_device_settings().max_tx_msdu_life_time );
                }
            else
                {
                /**
                 * Since TX queue parameters apply only to uplink, there is nothing
                 * more to do on downlink streams.
                 */
                return core_error_ok; 
                }

            break;
            }
        case core_base_state_parameters_set:
            {
            DEBUG( "core_operation_delete_ts_c::next_state() - tx queue parameters reset" );
            
            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }
