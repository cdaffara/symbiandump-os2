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
* Description:  Statemachine for handling traffic stream deletion from network side.
*
*/

/*
* %version: 10 %
*/

#include "core_operation_handle_delete_ts.h"
#include "core_operation_update_tx_rate_policies.h"
#include "core_tools.h"
#include "core_server.h"
#include "core_traffic_stream_list_iter.h"
#include "core_virtual_traffic_stream_list_iter.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_handle_delete_ts_c::core_operation_handle_delete_ts_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    u8_t tid ) :
    core_operation_base_c( core_operation_handle_delete_ts, request_id, server, drivers, adaptation, 
        core_base_flag_none ),
    tid_m( tid )
    {
    DEBUG( "core_operation_handle_delete_ts_c::core_operation_handle_delete_ts_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_handle_delete_ts_c::~core_operation_handle_delete_ts_c()
    {
    DEBUG( "core_operation_handle_delete_ts_c::~core_operation_handle_delete_ts_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_handle_delete_ts_c::next_state()
    {
    DEBUG( "core_operation_handle_delete_ts_c::next_state()" );

    switch ( operation_state_m )
        {              
        case core_state_init:
            {
            operation_state_m = core_base_state_parameters_set;

            if ( !server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_handle_delete_ts_c::next_state() - not connected, nothing to do" );

                return core_error_general;
                }

            DEBUG1( "core_operation_handle_delete_ts_c::next_state() - network has requested deletion of TID %u",
                tid_m );

            core_traffic_stream_list_c& ts_list(
                server_m->get_connection_data()->traffic_stream_list() );
            core_virtual_traffic_stream_list_c& virtual_ts_list(
                server_m->get_connection_data()->virtual_traffic_stream_list() );
            core_traffic_stream_list_iter_c ts_iter( ts_list );

            virtual_ts_list.print_contents();
            ts_list.print_contents();

            core_traffic_stream_c* stream = NULL;
            core_traffic_stream_c* iter = ts_iter.first();
            while( iter )
                {
                if ( iter->tid() == tid_m )
                    {
                    stream = iter;

                    iter = NULL;
                    }
                else
                    {
                    iter = ts_iter.next();
                    }
                }

            if ( !stream )
                {
                DEBUG1( "core_operation_handle_delete_ts_c::next_state() - no traffic stream with TID %u found",
                    tid_m );

                return core_error_not_found;
                }

            u8_t user_priority = stream->user_priority();
            core_traffic_stream_direction_e direction = stream->direction();

            /**
             * Send a status update to all affected virtual traffic streams. 
             */
            set_virtual_traffic_stream_inactive_by_tid(
                tid_m,
                core_traffic_stream_status_inactive_deleted_by_ap );

            /**
             * Delete the actual traffic stream.
             */
            server_m->get_wpx_adaptation_instance().handle_ts_delete(
                tid_m, user_priority );           
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
                DEBUG( "core_operation_handle_delete_ts_c::next_state() - traffic no longer admitted on this AC, notifying clients" );

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
            DEBUG( "core_operation_handle_delete_ts_c::next_state() - tx queue parameters reset" );

            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }

void core_operation_handle_delete_ts_c::set_virtual_traffic_stream_inactive_by_tid(
    u8_t tid,
    core_traffic_stream_status_e stream_status )
    {
    DEBUG( "core_operation_handle_delete_ts_c::set_virtual_traffic_stream_inactive_by_tid()" );

    core_virtual_traffic_stream_list_c& virtual_ts_list(
        server_m->get_connection_data()->virtual_traffic_stream_list() );
    core_virtual_traffic_stream_list_iter_c virtual_ts_iter( virtual_ts_list );

    core_virtual_traffic_stream_c* virtual_iter = virtual_ts_iter.first();
    while( virtual_iter )
        {
        if( virtual_iter->tid() == tid )
            {
            u32_t id( virtual_iter->id() );

            /**
             * The virtual traffic stream is no longer mapped to any actual
             * traffic stream.
             */
            virtual_iter->set_tid(
                TRAFFIC_STREAM_TID_NONE );
            virtual_iter->set_status(
                stream_status );

            DEBUG1( "core_operation_handle_delete_ts_c::set_virtual_traffic_stream_inactive_by_tid() - virtual traffic stream with ID %u is now inactive",
                id );

            u8_t buf[5];                    
            core_tools_c::copy(
                &buf[0],
                reinterpret_cast<u8_t*>( &id ),
                sizeof( u32_t ) );
            buf[4] = static_cast<u8_t>( stream_status );

            adaptation_m->notify(
                core_notification_ts_status_changed,
                sizeof( buf ),
                buf );                    
            }

        virtual_iter = virtual_ts_iter.next();
        }
    }
