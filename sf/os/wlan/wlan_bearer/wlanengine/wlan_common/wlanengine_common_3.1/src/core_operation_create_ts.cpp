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
* Description:  Statemachine for creating a traffic stream.
*
*/

/*
* %version: 6 %
*/

#include "core_operation_create_ts.h"
#include "core_operation_delete_ts.h"
#include "core_traffic_stream.h"
#include "core_sub_operation_create_ts.h"
#include "core_frame_wmm_ie.h"
#include "core_server.h"
#include "core_tools.h"
#include "core_traffic_stream_list_iter.h"
#include "core_virtual_traffic_stream_list_iter.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_create_ts_c::core_operation_create_ts_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    u8_t tid,
    u8_t user_priority,
    bool_t is_automatic_stream,
    const core_traffic_stream_params_s& params,
    u32_t& stream_id,
    core_traffic_stream_status_e& stream_status ) :
    core_operation_base_c( core_operation_create_ts, request_id, server, drivers, adaptation,
        core_base_flag_none ),
    requested_tid_m( tid ),
    tid_m( tid ),
    user_priority_m( user_priority ),
    access_class_m( core_access_class_best_effort ),
    is_automatic_stream_m( is_automatic_stream ),
    requested_params_m( params ),
    params_m( params ),
    stream_id_m( stream_id ),
    stream_status_m( stream_status ),
    stream_m( NULL )
    {
    DEBUG( "core_operation_create_ts_c::core_operation_create_ts_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_create_ts_c::~core_operation_create_ts_c()
    {
    DEBUG( "core_operation_create_ts_c::~core_operation_create_ts_c()" );

    delete stream_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_create_ts_c::next_state()
    {
    DEBUG( "core_operation_create_ts_c::next_state()" );

    switch ( operation_state_m )
        {              
        case core_state_init:
            {
            if( !server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_create_ts_c::next_state() - not connected, nothing to do" );

                return core_error_general;
                }
            if( !is_valid_params() )
                {
                DEBUG( "core_operation_create_ts_c::next_state() - invalid traffic stream parameters" );

                return core_error_illegal_argument;                
                }
            access_class_m = core_tools_c::convert_user_priority_to_ac( user_priority_m );

            core_traffic_stream_list_c& ts_list(
                server_m->get_connection_data()->traffic_stream_list() );
            core_virtual_traffic_stream_list_c& virtual_ts_list(
                server_m->get_connection_data()->virtual_traffic_stream_list() );
            core_traffic_stream_list_iter_c ts_iter( ts_list );
            core_virtual_traffic_stream_list_iter_c virtual_ts_iter( virtual_ts_list );

            DEBUG( "core_operation_create_ts_c::next_state() - current traffic streams:" );
            virtual_ts_list.print_contents();
            ts_list.print_contents();

            if( tid_m != TRAFFIC_STREAM_TID_NONE )
                {
                /**
                 * If TID has been specified for the virtual traffic stream, check
                 * whether it clashes with an existing traffic stream.
                 */
                if( ts_list.is_traffic_stream_for_tid( tid_m ) )
                    {
                    DEBUG1( "core_operation_create_ts_c::next_state() - stream already exists for TID %u",
                        tid_m );

                    return core_error_already_exists;
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
                tid_m = virtual_ts_list.next_tid();
                if( tid_m == MAX_TRAFFIC_STREAM_TID )
                    {
                    tid_m = ts_list.next_tid();
                    if( tid_m == MAX_TRAFFIC_STREAM_TID )
                        {                    
                        DEBUG( "core_operation_create_ts_c::next_state() - no free TIDs" );

                        return core_error_general;
                        }
                    }
                }

#ifdef _DEBUG
            DEBUG2( "core_operation_create_ts_c::next_state() - Traffic Stream TID is %u, UP is %u",
                tid_m, user_priority_m );

            DEBUG( "core_operation_create_ts_c::next_state() - trying to create a traffic stream with values:" );
            if( params_m.is_periodic_traffic )
                {
                DEBUG( "core_operation_create_ts_c::next_state() - Traffic pattern:              periodic" );
                }
            else
                {
                DEBUG( "core_operation_create_ts_c::next_state() - Traffic pattern:              aperiodic" );
                }            
            if( params_m.direction == core_traffic_stream_direction_uplink )
                {
                DEBUG( "core_operation_create_ts_c::next_state() - Direction:                    uplink" );
                }
            else if( params_m.direction == core_traffic_stream_direction_downlink )
                {
                DEBUG( "core_operation_create_ts_c::next_state() - Direction:                    downlink" );
                }
            else
                {
                DEBUG( "core_operation_create_ts_c::next_state() - Direction:                    bi-directional" );
                }                
            DEBUG2( "core_operation_create_ts_c::next_state() - Nominal MSDU Size:            %u (0x%04X)",
                params_m.nominal_msdu_size, params_m.nominal_msdu_size );                
            DEBUG2( "core_operation_create_ts_c::next_state() - Maximum MSDU Size:            %u (0x%04X)",
                params_m.maximum_msdu_size, params_m.maximum_msdu_size );
            DEBUG2( "core_operation_create_ts_c::next_state() - Minimum Service Interval:     %u (0x%08X)",
                params_m.minimum_service_interval, params_m.minimum_service_interval );
            DEBUG2( "core_operation_create_ts_c::next_state() - Maximum Service Interval:     %u (0x%08X)",
                params_m.maximum_service_interval, params_m.maximum_service_interval );
            DEBUG2( "core_operation_create_ts_c::next_state() - Inactivity Interval:          %u (0x%08X)",
                params_m.inactivity_interval, params_m.inactivity_interval );
            DEBUG2( "core_operation_create_ts_c::next_state() - Suspension Interval:          %u (0x%08X)",
                params_m.suspension_interval, params_m.suspension_interval );
            DEBUG2( "core_operation_create_ts_c::next_state() - Service Start Time:           %u (0x%08X)",
                params_m.service_start_time, params_m.service_start_time );
            DEBUG2( "core_operation_create_ts_c::next_state() - Minimum Data Rate:            %u (0x%08X)",
                params_m.minimum_data_rate, params_m.minimum_data_rate );        
            DEBUG2( "core_operation_create_ts_c::next_state() - Mean Data Rate:               %u (0x%08X)",
                params_m.mean_data_rate, params_m.mean_data_rate );
            DEBUG2( "core_operation_create_ts_c::next_state() - Peak Data Rate:               %u (0x%08X)",
                params_m.peak_data_rate, params_m.peak_data_rate );
            DEBUG2( "core_operation_create_ts_c::next_state() - Maximum Burst Size:           %u (0x%08X)",
                params_m.maximum_burst_size, params_m.maximum_burst_size );
            DEBUG2( "core_operation_create_ts_c::next_state() - Delay Bound:                  %u (0x%08X)",
                params_m.delay_bound, params_m.delay_bound );
            DEBUG2( "core_operation_create_ts_c::next_state() - Minimum PHY Rate:             %u (0x%08X)",
                params_m.minimum_phy_rate, params_m.minimum_phy_rate );
            DEBUG2( "core_operation_create_ts_c::next_state() - Nominal PHY Rate:             %u (0x%04X)",
                params_m.nominal_phy_rate, params_m.nominal_phy_rate );
            DEBUG2( "core_operation_create_ts_c::next_state() - Surplus Bandwidth Allowance:  %u (0x%04X)",
                params_m.surplus_bandwidth_allowance, params_m.surplus_bandwidth_allowance );
#endif // _DEBUG

            stream_m = new core_traffic_stream_c(
                tid_m,
                user_priority_m );
            if( !stream_m )
                {
                DEBUG( "core_operation_create_ts_c::next_state() - unable to allocate a traffic stream instance" );

                return core_error_no_memory;
                }

            /**
             * If the Max Tx MSDU Lifetime is not overridden, use the default value.             
             */
            if( !params_m.override_max_tx_msdu_lifetime )
                {
                params_m.override_max_tx_msdu_lifetime = server_m->get_device_settings().max_tx_msdu_life_time;
                }

            stream_m->set_default_traffic_values( params_m );            
            stream_m->reset_to_default_values();            

            /**
             * If this is a manual traffic stream, all automatic streams
             * need to be deleted before continuing.
             */
            if( !is_automatic_stream_m )
                {
                server_m->get_connection_data()->set_ac_traffic_mode(
                    access_class_m,
                    core_access_class_traffic_mode_manual );

                /**
                 * Send an indication to notify clients that traffic mode
                 * is now set to manual.
                 */
                u8_t buf[5];
                buf[0] = static_cast<u8_t>( 
                    access_class_m );
                buf[1] = static_cast<u8_t>(
                    core_access_class_traffic_mode_manual );
                adaptation_m->notify(
                    core_notification_ac_traffic_mode_changed,
                    sizeof( buf ),
                    buf );

                return goto_state( core_state_delete_streams );
                }

            /**
             * Otherwise proceed with stream creation.
             */            
            return goto_state( core_state_create_stream );
            }
        case core_state_delete_streams:
            {
            core_virtual_traffic_stream_list_iter_c virtual_ts_iter(
                server_m->get_connection_data()->virtual_traffic_stream_list() );
            for( core_virtual_traffic_stream_c* iter = virtual_ts_iter.first(); iter; iter = virtual_ts_iter.next() )
                {
                if( iter->access_class() == access_class_m &&
                    iter->is_automatic_stream() )
                    {
                    core_operation_base_c* operation = new core_operation_delete_ts_c(
                        request_id_m,
                        server_m,
                        drivers_m,
                        adaptation_m,
                        iter->id() );

                    return run_sub_operation( operation, core_state_delete_streams );                    
                    }
                }

            return asynch_goto( core_state_create_stream );
            }            
        case core_state_create_stream:
            {
            core_ap_data_c* current_ap = server_m->get_connection_data()->current_ap_data();
            if( !current_ap->is_wmm_ie_present() ||
                !current_ap->is_admission_control_required( access_class_m ) )
                {
                DEBUG1( "core_operation_create_ts_c::next_state() - AP doesn't require admission control on AC %u",
                    access_class_m );
                
                /**
                 * An actual traffic stream is not needed but the virtual traffic
                 * stream is still created.
                 */                
                server_m->get_connection_data()->virtual_traffic_stream_list().add_traffic_stream(
                    requested_tid_m,
                    TRAFFIC_STREAM_TID_NONE,
                    user_priority_m,
                    is_automatic_stream_m,
                    requested_params_m,
                    stream_id_m,
                    core_traffic_stream_status_inactive_not_required );

                return core_error_ok;
                }

            /**
             * Try to create the actual traffic stream.
             */
            core_operation_base_c* operation = new core_sub_operation_create_ts_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                *server_m->get_connection_data()->current_ap_data(),
                tid_m,
                user_priority_m,
                params_m,
                stream_status_m );

            return run_sub_operation( operation, core_state_create_stream_success );
            }
        case core_state_create_stream_failed:
            {
            DEBUG( "core_operation_create_ts_c::next_state() - traffic stream creation failed" );

            /**
             * Eventhough the actual traffic stream couldn't be created, the virtual traffic
             * stream is still created.
             */            
            server_m->get_connection_data()->virtual_traffic_stream_list().add_traffic_stream(
                requested_tid_m,
                TRAFFIC_STREAM_TID_NONE,
                user_priority_m,
                is_automatic_stream_m,
                requested_params_m,
                stream_id_m,
                stream_status_m );

            return core_error_ok;
            }
        case core_state_create_stream_success:
            {
            operation_state_m = core_state_parameters_set;

            DEBUG( "core_operation_create_ts_c::next_state() - traffic stream created successfully" );

            server_m->get_connection_data()->virtual_traffic_stream_list().add_traffic_stream(
                requested_tid_m,
                tid_m,
                user_priority_m,
                is_automatic_stream_m,
                requested_params_m,
                stream_id_m,
                core_traffic_stream_status_active );

            /**
             * Store the new traffic stream parameters.
             */
            stream_m->set_status( core_traffic_stream_status_active );
            stream_m->set_traffic_values( params_m );            
            server_m->get_connection_data()->traffic_stream_list().update_traffic_stream(
                *stream_m );

            if( params_m.direction == core_traffic_stream_direction_uplink ||
                params_m.direction == core_traffic_stream_direction_bidirectional )
                {
                server_m->get_connection_data()->set_ac_traffic_status(
                    access_class_m,
                    core_access_class_traffic_status_admitted );

                /**
                 * Send an indication to notify clients that this access class
                 * is now admitted. 
                 */
                u8_t buf[5];
                buf[0] = static_cast<u8_t>( 
                    access_class_m );
                buf[1] = static_cast<u8_t>(
                    core_access_class_traffic_status_admitted );
                adaptation_m->notify(
                    core_notification_ac_traffic_status_changed,
                    sizeof( buf ),
                    buf );

                /**
                 * Set the parameters to the drivers based on the created
                 * traffic stream.
                 */
                DEBUG( "core_operation_create_ts_c::next_state() - setting tx queue parameters" );
    
                drivers_m->set_tx_queue_parameters(
                    request_id_m,
                    stream_m->access_class(),
                    stream_m->medium_time(),
                    stream_m->max_tx_msdu_lifetime() );
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
        case core_state_parameters_set:
            {
            DEBUG( "core_operation_create_ts_c::next_state() - tx queue parameters set" );

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
core_error_e core_operation_create_ts_c::cancel()
    {
    DEBUG( "core_operation_create_ts_c::cancel() " );    

    switch( operation_state_m )
        {              
        case core_state_create_stream_success:
            {
            return goto_state( core_state_create_stream_failed );
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
bool_t core_operation_create_ts_c::is_valid_params()
    {
    if( tid_m >= MAX_TRAFFIC_STREAM_TID && tid_m != TRAFFIC_STREAM_TID_NONE )
        {
        DEBUG1( "core_operation_create_ts_c::is_valid_params() - invalid TID value: %u",
            tid_m );

        return false_t;
        }
    if( user_priority_m >= MAX_QOS_USER_PRIORITY )
        {
        DEBUG1( "core_operation_create_ts_c::is_valid_params() - invalid user priority value: %u",
            user_priority_m );        

        return false_t;
        }

    return true_t;
    }

