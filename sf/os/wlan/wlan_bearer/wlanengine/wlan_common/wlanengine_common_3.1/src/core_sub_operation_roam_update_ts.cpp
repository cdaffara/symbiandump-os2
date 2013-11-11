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
* Description:  Statemachine for updating traffic stream statuses after roam.
*
*/

/*
* %version: 13 %
*/

#include "core_sub_operation_roam_update_ts.h"
#include "core_sub_operation_create_ts.h"
#include "core_frame_wmm_ie.h"
#include "core_server.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_roam_update_ts_c::core_sub_operation_roam_update_ts_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_ap_data_c& ap_data ) :
    core_operation_base_c( core_operation_unspecified, request_id, server, drivers, adaptation,
        core_base_flag_none ),
    current_ap_m( ap_data ),
    virtual_stream_list_m(),
    virtual_stream_iter_m( virtual_stream_list_m ),
    stream_list_m(),
    stream_iter_m( stream_list_m ),
    tid_m( 0 ),
    user_priority_m( 0 )
    {
    DEBUG( "core_sub_operation_roam_update_ts_c::core_sub_operation_roam_update_ts_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_roam_update_ts_c::~core_sub_operation_roam_update_ts_c()
    {
    DEBUG( "core_sub_operation_roam_update_ts_c::~core_sub_operation_roam_update_ts_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_roam_update_ts_c::next_state()
    {
    DEBUG( "core_sub_operation_roam_update_ts_c::next_state()" );
    
    switch( operation_state_m )
        {
        case core_state_init:
            {
            if( !server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_sub_operation_roam_update_ts_c::next_state() - not connected, nothing to do" );

                return core_error_general;
                }

            bool_t is_ac_required[MAX_QOS_ACCESS_CLASS] =
                { 
                current_ap_m.is_admission_control_required(
                    core_access_class_best_effort ),
                current_ap_m.is_admission_control_required(
                    core_access_class_background ),
                current_ap_m.is_admission_control_required(
                    core_access_class_video ),
                current_ap_m.is_admission_control_required(
                    core_access_class_voice )
                };

            /**
             * If admission control is mandatory for an access class, send an indication
             * to notify clients that this access class is no longer admitted.
             * 
             * If admission control is not mandatory, send an indication that
             * this access class is admitted.
             */
            for( u8_t idx( 0 ); idx < MAX_QOS_ACCESS_CLASS; ++idx )
                {
                if( is_ac_required[idx] )
                    {
                    server_m->get_connection_data()->set_ac_traffic_status(
                        static_cast<core_access_class_e>( idx ),
                        core_access_class_traffic_status_not_admitted );

                    DEBUG1( "core_operation_handle_delete_ts_c::next_state() - traffic no longer admitted on AC %u, notifying clients",
                        idx );

                    u8_t buf[5];
                    buf[0] = idx; 
                    buf[1] = static_cast<u8_t>(
                        core_access_class_traffic_status_not_admitted );
                    adaptation_m->notify(
                        core_notification_ac_traffic_status_changed,
                        sizeof( buf ),
                        buf );                
                    }
                else
                    {
                    server_m->get_connection_data()->set_ac_traffic_status(
                        static_cast<core_access_class_e>( idx ),
                        core_access_class_traffic_status_admitted );

                    DEBUG1( "core_operation_handle_delete_ts_c::next_state() - traffic admitted on AC %u, notifying clients",
                        idx );

                    u8_t buf[5];
                    buf[0] = idx; 
                    buf[1] = static_cast<u8_t>(
                        core_access_class_traffic_status_admitted );
                    adaptation_m->notify(
                        core_notification_ac_traffic_status_changed,
                        sizeof( buf ),
                        buf );                    
                    }
                }

            core_traffic_stream_list_c& ts_list(
                server_m->get_connection_data()->traffic_stream_list() );
            core_virtual_traffic_stream_list_c& virtual_ts_list(
                server_m->get_connection_data()->virtual_traffic_stream_list() );
            core_traffic_stream_list_iter_c ts_iter( ts_list );
            core_virtual_traffic_stream_list_iter_c virtual_ts_iter( virtual_ts_list );
            
            DEBUG( "core_sub_operation_roam_update_ts_c::next_state() - traffic streams before processing:" );
            virtual_ts_list.print_contents();
            ts_list.print_contents();

            /**
             * Create a list of virtual traffic streams that are currently
             * inactive and need to be created, i.e. admission control is
             * mandatory for the access class.
             */
            core_virtual_traffic_stream_c* virtual_iter = virtual_ts_iter.first();
            while( virtual_iter )
                {
                if( virtual_iter->status() != core_traffic_stream_status_active )
                    {
                    if( is_ac_required[virtual_iter->access_class()] )
                        {
                        DEBUG1( "core_sub_operation_roam_update_ts_c::next_state() - virtual traffic stream with ID %u needs to created",
                            virtual_iter->id() );

                        virtual_stream_list_m.add_traffic_stream(
                            *virtual_iter );
                        }
                    else
                        {
                        /**
                         * Send a status update to all affected virtual traffic streams. 
                         */
                        set_virtual_traffic_stream_inactive_by_id(
                            virtual_iter->id(),
                            core_traffic_stream_status_inactive_not_required );
                        }
                    }

                virtual_iter = virtual_ts_iter.next();
                }

            /**
             * Delete all traffic streams that are no longer needed or that have
             * been marked as rejected. Create a list of traffic streams that 
             * need to be re-created.
             * 
             * There is no need to reset the TX queue parameters since
             * UMAC will reset them automatically after an association.
             */
            core_traffic_stream_c* iter = ts_iter.first();
            while( iter )
                {
                if( !is_ac_required[iter->access_class()] )
                    {
                    DEBUG2( "core_sub_operation_roam_update_ts_c::next_state() - traffic stream (TID %u, user priority %u) is no longer needed, deleting",
                        iter->tid(), iter->user_priority() );                        

                    /**
                     * Send a status update to all affected virtual traffic streams. 
                     */
                    set_virtual_traffic_stream_inactive_by_tid(
                        iter->tid(),
                        core_traffic_stream_status_inactive_not_required );
                    
                    /**
                     * Delete the actual traffic stream.
                     */
                    server_m->get_wpx_adaptation_instance().handle_ts_delete(
                        iter->tid(), iter->user_priority() );                   
                    ts_iter.remove();
                    iter = NULL;
                    }
                else if( iter->status() == core_traffic_stream_status_undefined )
                    {
                    DEBUG2( "core_sub_operation_roam_update_ts_c::next_state() - traffic stream (TID %u, user priority %u) needs to be re-created",
                        iter->tid(), iter->user_priority() );

                    stream_list_m.update_traffic_stream( *iter );
                    }
                else if( iter->status() != core_traffic_stream_status_active )
                    {
                    DEBUG2( "core_sub_operation_roam_update_ts_c::next_state() - traffic stream (TID %u, user priority %u) is marked as rejected, deleting",
                        iter->tid(), iter->user_priority() );

                    /**
                     * Send a status update to all affected virtual traffic streams. 
                     */
                    set_virtual_traffic_stream_inactive_by_tid(
                        iter->tid(),
                        iter->status() );                        

                    /**
                     * Delete the actual traffic stream.
                     */
                    server_m->get_wpx_adaptation_instance().handle_ts_delete(
                        iter->tid(), iter->user_priority() );                   
                    ts_iter.remove();
                    iter = NULL;
                    }

                iter = ts_iter.next();
                }

            (void)stream_iter_m.first();

            return goto_state( core_state_recreate_next );
            }
        case core_state_recreate_next:
            {
            core_traffic_stream_c* iter = stream_iter_m.current();
            if( !iter )
                {
                /**
                 * All previously active traffic streams have now been re-created,
                 * process the virtual traffic streams that need to be created next.
                 */                
                (void)virtual_stream_iter_m.first();

                return goto_state( core_state_recreate_virtual_next );
                }

            tid_m = iter->tid();
            user_priority_m = iter->user_priority();

            DEBUG2( "core_sub_operation_roam_update_ts_c::next_state() - trying to recreate traffic stream (TID %u, user priority %u)",
                tid_m, user_priority_m );

            params_m.is_periodic_traffic = iter->is_periodic_traffic();
            params_m.direction = iter->direction();
            params_m.nominal_msdu_size = iter->nominal_msdu_size();
            params_m.maximum_msdu_size = iter->maximum_msdu_size();
            params_m.minimum_service_interval = iter->minimum_service_interval();
            params_m.maximum_service_interval = iter->maximum_service_interval();
            params_m.inactivity_interval = iter->inactivity_interval();
            params_m.suspension_interval = iter->suspension_interval();
            params_m.service_start_time = iter->service_start_time();
            params_m.minimum_data_rate = iter->minimum_data_rate();
            params_m.mean_data_rate = iter->mean_data_rate();
            params_m.peak_data_rate = iter->peak_data_rate();
            params_m.maximum_burst_size = iter->maximum_burst_size();
            params_m.delay_bound = iter->delay_bound();
            params_m.minimum_phy_rate = iter->minimum_phy_rate();
            params_m.surplus_bandwidth_allowance = iter->surplus_bandwidth_allowance();
            params_m.medium_time = iter->medium_time();
            params_m.nominal_phy_rate = iter->nominal_phy_rate();
            params_m.override_rates = iter->override_rates();
            params_m.override_max_tx_msdu_lifetime = iter->max_tx_msdu_lifetime();

            core_operation_base_c* operation = new core_sub_operation_create_ts_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                current_ap_m,
                tid_m,
                user_priority_m,
                params_m,
                stream_status_m );

            return run_sub_operation( operation, core_state_recreate_success );
            }
        case core_state_recreate_success:
            {
            DEBUG( "core_sub_operation_roam_update_ts_c::next_state() - traffic stream created successfully" );

            /**
             * Store the new traffic stream parameters.
             */
            core_traffic_stream_c* iter = stream_iter_m.current();
            iter->set_status( core_traffic_stream_status_active );
            iter->set_traffic_values( params_m );
            server_m->get_connection_data()->traffic_stream_list().update_traffic_stream( *iter );

            /**
             * Move to the next entry.
             */
            stream_iter_m.remove();
            iter = NULL;
            (void)stream_iter_m.first();

            return goto_state( core_state_recreate_next );
            }
        case core_state_recreate_fail:
            {
            DEBUG( "core_sub_operation_roam_update_ts_c::next_state() - traffic stream creation failed" );

            /**
             * Send a status update to all affected virtual traffic streams. 
             */
            set_virtual_traffic_stream_inactive_by_tid(
                tid_m,
                core_traffic_stream_status_inactive_other );

            /**
             * Delete the actual traffic stream.
             */
            server_m->get_wpx_adaptation_instance().handle_ts_delete(
                tid_m, user_priority_m );
            server_m->get_connection_data()->traffic_stream_list().remove_traffic_stream_by_tid( tid_m );

            /**
             * Move to the next entry.
             */
            stream_iter_m.remove();
            (void)stream_iter_m.first();

            return goto_state( core_state_recreate_next );
            }
        case core_state_recreate_virtual_next:
            {
            core_virtual_traffic_stream_c* virtual_iter = virtual_stream_iter_m.current();
            if( !virtual_iter )
                {
                DEBUG( "core_sub_operation_roam_update_ts_c::next_state() - traffic streams after processing:" );
                server_m->get_connection_data()->virtual_traffic_stream_list().print_contents();
                server_m->get_connection_data()->traffic_stream_list().print_contents();
                (void)server_m->get_connection_data()->traffic_stream_list().first();

                DEBUG( "core_sub_operation_roam_update_ts_c::next_state() - setting TX queue parameters" );
                return goto_state( core_state_set_params_next );
                }

            tid_m = virtual_iter->requested_tid();
            user_priority_m = virtual_iter->user_priority();
            params_m = virtual_iter->params();
            stream_status_m = core_traffic_stream_status_inactive_other;
            if( tid_m != TRAFFIC_STREAM_TID_NONE )
                {
                /**
                 * If TID has been specified for the virtual traffic stream, check
                 * whether it clashes with an existing traffic stream.
                 */
                if( server_m->get_connection_data()->traffic_stream_list().is_traffic_stream_for_tid( tid_m ) )
                    {
                    DEBUG1( "core_operation_create_ts_c::next_state() - stream already exists for TID %u",
                        tid_m );
                    
                    return goto_state( core_state_recreate_virtual_fail );
                    }
                }
            else
                {
                /**
                 * If TID has been left unspecified, select the next free
                 * TID.
                 *
                 * First try to select the next free TID from virtual traffic
                 * stream list. In the unlikely scenario where all TIDs are
                 * in use, try the traffic stream list next.
                 */
                tid_m = server_m->get_connection_data()->virtual_traffic_stream_list().next_tid();
                if( tid_m == MAX_TRAFFIC_STREAM_TID )
                    {
                    tid_m = server_m->get_connection_data()->traffic_stream_list().next_tid();
                    if( tid_m == MAX_TRAFFIC_STREAM_TID )
                        {                    
                        DEBUG1( "core_operation_create_ts_c::next_state() - no free TIDs for stream ID %u",
                            virtual_iter->id() );

                        return goto_state( core_state_recreate_virtual_fail );
                        }
                    }
                }

            /**
             * Traffic stream to be created. 
             */
            core_traffic_stream_c iter(
                tid_m,
                user_priority_m );
            iter.set_default_traffic_values( params_m );            
            iter.reset_to_default_values();
            stream_list_m.update_traffic_stream(
                iter );
            (void)stream_iter_m.first();
            
            DEBUG2( "core_sub_operation_roam_update_ts_c::next_state() - trying to create traffic stream (TID %u, user priority %u)",
                tid_m, user_priority_m );

            core_operation_base_c* operation = new core_sub_operation_create_ts_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                current_ap_m,
                tid_m,
                user_priority_m,
                params_m,
                stream_status_m );

            return run_sub_operation( operation, core_state_recreate_virtual_success );
            }
        case core_state_recreate_virtual_success:
            {
            DEBUG( "core_sub_operation_roam_update_ts_c::next_state() - virtual traffic stream created successfully" );

            /**
             * Store the new traffic stream parameters.
             */
            core_virtual_traffic_stream_c* virtual_iter = virtual_stream_iter_m.current();            
            core_traffic_stream_c* iter = stream_iter_m.current();
            iter->set_status( core_traffic_stream_status_active );
            iter->set_traffic_values( params_m );
            server_m->get_connection_data()->traffic_stream_list().update_traffic_stream( *iter );

            /**
             * Send a status update to all affected virtual traffic streams. 
             */
            set_virtual_traffic_stream_active_by_id(
                virtual_iter->id(),
                tid_m );

            /**
             * Move to the next entry.
             */
            stream_iter_m.remove();
            iter = NULL;
            virtual_stream_iter_m.remove();
            virtual_iter = NULL;
            (void)virtual_stream_iter_m.first();

            return goto_state( core_state_recreate_virtual_next );
            }
        case core_state_recreate_virtual_fail:
            {
            DEBUG( "core_sub_operation_roam_update_ts_c::next_state() - virtual traffic stream creation failed" );

            core_virtual_traffic_stream_c* iter = virtual_stream_iter_m.current();

            /**
             * Send a status update to all affected virtual traffic streams. 
             */
            set_virtual_traffic_stream_inactive_by_id(
                iter->id(),
                stream_status_m );

            /**
             * Move to the next entry.
             */
            stream_iter_m.remove();
            virtual_stream_iter_m.remove();
            (void)virtual_stream_iter_m.first();

            return goto_state( core_state_recreate_virtual_next );
            }
        case core_state_set_params_next:
            {
            operation_state_m = core_state_set_params_success;

            core_traffic_stream_c* iter = server_m->get_connection_data()->traffic_stream_list().current();
            if( !iter )
                {
                DEBUG( "core_sub_operation_roam_update_ts_c::next_state() - all TX queue parameters set" );

                return core_error_ok;
                }

            ASSERT( iter->status() == core_traffic_stream_status_active );

            if( iter->direction() == core_traffic_stream_direction_uplink ||
                iter->direction() == core_traffic_stream_direction_bidirectional )
                {
                server_m->get_connection_data()->set_ac_traffic_status(
                    iter->access_class(),
                    core_access_class_traffic_status_admitted );

                /**
                 * Send an indication to notify clients that this access class
                 * is now admitted. 
                 */
                u8_t buf[5];
                buf[0] = static_cast<u8_t>( 
                    iter->access_class() );
                buf[1] = static_cast<u8_t>(
                    core_access_class_traffic_status_admitted );
                adaptation_m->notify(
                    core_notification_ac_traffic_status_changed,
                    sizeof( buf ),
                    buf );

                /**
                 * Set TX queue parameters to the drivers based on the created
                 * traffic stream.
                 */
                DEBUG3( "core_sub_operation_roam_update_ts_c::next_state() - setting queue_id %u; medium_time %u, max_tx_msdu_lifetime %u",
                    iter->access_class(),
                    iter->medium_time(),
                    iter->max_tx_msdu_lifetime() );

                drivers_m->set_tx_queue_parameters(
                    request_id_m,
                    iter->access_class(),
                    iter->medium_time(),
                    iter->max_tx_msdu_lifetime() );
                }
            else
                {
                /**
                 * Since TX queue parameters apply only to uplink, there is nothing
                 * more to do on downlink streams. Move to the next entry.
                 */
                (void)server_m->get_connection_data()->traffic_stream_list().next();

                return goto_state( core_state_set_params_next );                
                }

            break;
            }
        case core_state_set_params_success:
            {
            /**
             * Move to the next entry.
             */
            (void)server_m->get_connection_data()->traffic_stream_list().next();

            return goto_state( core_state_set_params_next );
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
core_error_e core_sub_operation_roam_update_ts_c::cancel()
    {
    DEBUG( "core_sub_operation_roam_update_ts_c::cancel()" );
    
    switch ( operation_state_m )
        {
        case core_state_recreate_success:
            {
            /**
             * Unable to create a traffic stream.
             */
            return goto_state( core_state_recreate_fail );
            }
        case core_state_recreate_virtual_success:
            {
            /**
             * Unable to create a virtual traffic stream.
             */
            return goto_state( core_state_recreate_virtual_fail );
            }            
        case core_state_set_params_success:
            {
            /**
             * Ignore errors when setting TX queue parameters.
             */            
            return goto_state( core_state_set_params_success );
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
void core_sub_operation_roam_update_ts_c::set_virtual_traffic_stream_inactive_by_tid(
    u8_t tid,
    core_traffic_stream_status_e stream_status )
    {
    DEBUG( "core_sub_operation_roam_update_ts_c::set_virtual_traffic_stream_inactive_by_tid()" );
    
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

            DEBUG1( "core_sub_operation_roam_update_ts_c::set_virtual_traffic_stream_inactive_by_tid() - virtual traffic stream with ID %u is inactive",
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

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_sub_operation_roam_update_ts_c::set_virtual_traffic_stream_inactive_by_id(
    u32_t id,
    core_traffic_stream_status_e stream_status )
    {
    DEBUG( "core_sub_operation_roam_update_ts_c::set_virtual_traffic_stream_inactive_by_id()" );
    
    core_virtual_traffic_stream_list_c& virtual_ts_list(
        server_m->get_connection_data()->virtual_traffic_stream_list() );
    core_virtual_traffic_stream_list_iter_c virtual_ts_iter( virtual_ts_list );

    core_virtual_traffic_stream_c* virtual_iter = virtual_ts_iter.first();
    while( virtual_iter )
        {
        if( virtual_iter->id() == id )
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

            DEBUG1( "core_sub_operation_roam_update_ts_c::set_virtual_traffic_stream_inactive_by_tid() - virtual traffic stream with ID %u is inactive",
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

            return;
            }

        virtual_iter = virtual_ts_iter.next();
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_sub_operation_roam_update_ts_c::set_virtual_traffic_stream_active_by_id(
    u32_t id,
    u8_t tid )
    {
    DEBUG( "core_sub_operation_roam_update_ts_c::set_virtual_traffic_stream_active_by_id()" );
    
    core_virtual_traffic_stream_list_c& virtual_ts_list(
        server_m->get_connection_data()->virtual_traffic_stream_list() );
    core_virtual_traffic_stream_list_iter_c virtual_ts_iter( virtual_ts_list );

    core_virtual_traffic_stream_c* virtual_iter = virtual_ts_iter.first();
    while( virtual_iter )
        {
        if( virtual_iter->id() == id )
            {
            u32_t id( virtual_iter->id() );

            /**
             * The virtual traffic stream is no longer mapped to any actual
             * traffic stream.
             */
            virtual_iter->set_tid(
                tid );
            virtual_iter->set_status(
                core_traffic_stream_status_active );

            DEBUG1( "core_sub_operation_roam_update_ts_c::set_virtual_traffic_stream_active_by_id() - virtual traffic stream with ID %u is active",
                id );

            u8_t buf[5];                    
            core_tools_c::copy(
                &buf[0],
                reinterpret_cast<u8_t*>( &id ),
                sizeof( u32_t ) );
            buf[4] = static_cast<u8_t>( core_traffic_stream_status_active );

            adaptation_m->notify(
                core_notification_ts_status_changed,
                sizeof( buf ),
                buf ); 

            return;
            }

        virtual_iter = virtual_ts_iter.next();
        }
    }
