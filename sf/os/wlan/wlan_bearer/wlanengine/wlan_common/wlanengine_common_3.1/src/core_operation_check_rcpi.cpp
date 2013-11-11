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
* Description:  Statemachine for requesting RCPI values for roaming.
*
*/

/*
* %version: 23 %
*/

#include "core_operation_check_rcpi.h"
#include "core_operation_roam.h"
#include "core_operation_handle_bss_lost.h"
#include "core_sub_operation_roam_scan.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_check_rcpi_c::core_operation_check_rcpi_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_rcpi_check_reason_e reason ) :
    core_operation_base_c( core_operation_check_rcpi, request_id, server, drivers, adaptation,
        core_base_flag_drivers_needed | core_base_flag_connection_needed | core_base_flag_roam_operation ),
    reason_m(  reason ),
    current_rcpi_m( 0 ),
    is_connected_m( true_t ) 
    {
    DEBUG( "core_operation_check_rcpi_c::core_operation_check_rcpi_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_check_rcpi_c::~core_operation_check_rcpi_c()
    {
    DEBUG( "core_operation_check_rcpi_c::~core_operation_check_rcpi_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_check_rcpi_c::next_state()
    {
    DEBUG( "core_operation_check_rcpi_c::next_state()" );
    
    switch ( operation_state_m )
        {
        case core_state_init:
            {
#ifdef _DEBUG
            switch ( reason_m )
                {
                case core_rcpi_check_reason_timer:
                    DEBUG( "core_operation_check_rcpi_c::next_state() - core_rcpi_check_reason_timer" );
                    break;
                case core_rcpi_check_reason_rcpi_trigger:
                    DEBUG( "core_operation_check_rcpi_c::next_state() - core_rcpi_check_reason_rcpi_trigger" );
                    break;
                case core_rcpi_check_reason_signal_loss_prediction:
                    DEBUG( "core_operation_check_rcpi_c::next_state() - core_rcpi_check_reason_signal_loss_prediction" );
                    break;
                default:
                    ASSERT( false_t );
                }
#endif // _DEBUG

            operation_state_m = core_state_rcpi_received;

            /**
             * Cancel the roaming timer just in case.
             */
            server_m->cancel_roam_timer();

            DEBUG( "core_operation_check_rcpi_c::next_state() - requesting RCPI" );

            drivers_m->get_current_rcpi(
                request_id_m,
                current_rcpi_m );

            break;
            }
        case core_state_rcpi_received:
            {
            operation_state_m = core_state_rcpi_trigger;

            DEBUG1( "core_operation_check_rcpi_c::next_state() - current RCPI is %u",
                current_rcpi_m );
            DEBUG1( "core_operation_check_rcpi_c::next_state() - internal RCPI threshold level is %u",
                server_m->get_device_settings().rcpi_trigger );
            DEBUG1( "core_operation_check_rcpi_c::next_state() - external RCPI threshold level (down) is %u",
                server_m->get_core_settings().rcp_decline_boundary() );
            DEBUG1( "core_operation_check_rcpi_c::next_state() - external RCPI threshold level (up) is %u",
                server_m->get_core_settings().rcp_improve_boundary() );

            u8_t trigger_level_down = server_m->get_core_settings().rcp_decline_boundary();
            if ( trigger_level_down < server_m->get_device_settings().rcpi_trigger )
                {
                trigger_level_down = server_m->get_device_settings().rcpi_trigger;
                }
            u8_t trigger_level_up = server_m->get_core_settings().rcp_improve_boundary();
            if ( trigger_level_up < server_m->get_device_settings().rcpi_trigger )
                {
                trigger_level_up = server_m->get_device_settings().rcpi_trigger;
                }

            /**
             * If signal level drops below roaming threshold, attempt roaming.
             */
            if ( current_rcpi_m < server_m->get_device_settings().rcpi_trigger &&
                 server_m->get_connection_data()->iap_data().is_roaming_allowed() )
                {
                return goto_state( core_state_scan_start );
                }

            /**
             * If moving from weak level to normal, notify client.
             */
            if ( current_rcpi_m > server_m->get_core_settings().rcp_improve_boundary() &&
                 server_m->get_connection_data()->last_rcp_class() != core_rcp_normal )
                {
                DEBUG( "core_operation_check_rcpi_c::next_state() - sending a notification about the normal signal level" );

                u8_t buf[5];
                buf[0] = static_cast<u8_t>( core_rcp_normal );
                buf[1] = static_cast<u8_t>( current_rcpi_m );

                adaptation_m->notify(
                    core_notification_rcp_changed,
                    sizeof( buf ),
                    buf );
                server_m->get_connection_data()->set_last_rcp_class( core_rcp_normal );

                DEBUG( "core_operation_check_rcpi_c::next_state() - reseting RCPI roam check interval" );
                server_m->get_connection_data()->reset_rcpi_roam_interval();
                }

            /** 
             * Send an indication to adaptation to indicate that this
             * roam attempt has been completed.
             */  
            adaptation_m->notify(                
                core_notification_rcpi_roam_attempt_completed,
                0,
                NULL );           

            /**
             * If the signal level is above the trigger level, we only have to check
             * whether we should re-arm the trigger.
             */
            if ( current_rcpi_m > trigger_level_up )
                {
                DEBUG1( "core_operation_check_rcpi_c::next_state() - signal level is above trigger_level_up (%u)",
                    trigger_level_up );

                /**
                 * If the operation was started because of a trigger event, schedule a timer.
                 * This is done to prevent constant triggering when the signal level is
                 * hovering right on the trigger level.
                 */
                if ( reason_m == core_rcpi_check_reason_rcpi_trigger ||
                     reason_m == core_rcpi_check_reason_signal_loss_prediction )
                    {
                    DEBUG1( "core_operation_check_rcpi_c::next_state() - was triggered, scheduling the roaming timer to %u",
                        server_m->get_device_settings().roam_timer );

                    server_m->schedule_roam_timer(
                        server_m->get_device_settings().roam_timer );

                    return core_error_ok;
                    }

                /**
                 * At least one timer-based poll has been done, trigger can be re-armed.
                 */
                DEBUG1( "core_operation_check_rcpi_c::next_state() - was not triggered, arming RCPI roam trigger (%u)",
                    trigger_level_down );

                drivers_m->set_rcpi_trigger_level(
                    request_id_m,
                    trigger_level_down );

                DEBUG( "core_operation_check_rcpi_c::next_state() - reseting RCPI roam check interval" );
                server_m->get_connection_data()->reset_rcpi_roam_interval();

                return core_error_request_pending;
                }

            /**
             * If we have come this far, it means the current signal level is between the
             * external and the internal threshold level. Schedule a timer and if the operation
             * was started because of a timer event, re-arm the trigger as well.
             */
            DEBUG2( "core_operation_check_rcpi_c::next_state() - between trigger_level_down (%u) and trigger_level_up (%u)",
                trigger_level_down, trigger_level_up );
            DEBUG1( "core_operation_check_rcpi_c::next_state() - scheduling the roaming timer to %u",
                server_m->get_device_settings().roam_timer );

            server_m->schedule_roam_timer(
                server_m->get_device_settings().roam_timer );            

            if ( reason_m == core_rcpi_check_reason_timer )
                {
                DEBUG1( "core_operation_check_rcpi_c::next_state() - was not triggered, arming RCPI roam trigger (%u)",
                    trigger_level_down );                

                drivers_m->set_rcpi_trigger_level(
                    request_id_m,
                    trigger_level_down );

                return core_error_request_pending;
                }

            return core_error_ok;
            }
        case core_state_rcpi_trigger:
            {
            DEBUG( "core_operation_check_rcpi_c::next_state() - trigger re-armed" ); 

            return core_error_ok;
            }
        case core_state_scan_start:
            {
            operation_state_m = core_state_scan_complete;

            DEBUG( "core_operation_check_rcpi_c::next_state() - starting a direct scan on all channels" );           

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

            return run_sub_operation( operation );            
            }
        case core_state_scan_complete:
            {
            operation_state_m = core_state_connect_success;

            DEBUG( "core_operation_check_rcpi_c::next_state() - attempting to roam" );

            if ( reason_m == core_rcpi_check_reason_signal_loss_prediction )
                {
                server_m->get_connection_data()->set_last_roam_reason(
                    core_roam_reason_signal_loss_prediction );
                }
            else
                {
                server_m->get_connection_data()->set_last_roam_reason(
                    core_roam_reason_signal_strength );               
                }
            server_m->get_connection_data()->set_last_roam_failed_reason(
                core_roam_failed_reason_none );

            medium_time_s admitted_medium_time(
                server_m->get_connection_data()->traffic_stream_list().admitted_medium_time() );            

            core_operation_base_c* operation = new core_operation_roam_c(
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

            return run_sub_operation( operation );
            }                                   
        case core_state_connect_success:
            {
            operation_state_m = core_state_rcpi_trigger;

            DEBUG( "core_operation_check_rcpi_c::next_state() - roamed successfully" );
                        
            /** 
             * Send an indication to adaptation to indicate that this
             * roam attempt has been completed.
             */  
            adaptation_m->notify(                
                core_notification_rcpi_roam_attempt_completed,
                0,
                NULL );           

            DEBUG( "core_operation_check_rcpi_c::next_state() - reseting RCPI roam check interval" );
            server_m->get_connection_data()->reset_rcpi_roam_interval();

            DEBUG1( "core_operation_check_rcpi_c::next_state() - internal RCPI threshold level is %u",
                server_m->get_device_settings().rcpi_trigger );
            DEBUG1( "core_operation_check_rcpi_c::next_state() - external RCPI threshold level is %u",
                server_m->get_core_settings().rcp_decline_boundary() );

            u8_t trigger_level = server_m->get_core_settings().rcp_decline_boundary();
            if ( trigger_level < server_m->get_device_settings().rcpi_trigger )
                {
                trigger_level = server_m->get_device_settings().rcpi_trigger;
                }
            DEBUG1( "core_operation_check_rcpi_c::next_state() - arming RCPI roam trigger (%u)",
                trigger_level );

            drivers_m->set_rcpi_trigger_level(
                request_id_m,
                trigger_level );

            break;
            }
        case core_state_connect_failure:
            {
            operation_state_m = core_state_rcpi_trigger;
                      
            DEBUG( "core_operation_check_rcpi_c::next_state() - roaming failed" );
            
            DEBUG1( "core_operation_check_rcpi_c::next_state() - internal RCPI threshold level is %u",
                server_m->get_device_settings().rcpi_trigger );
            DEBUG1( "core_operation_check_rcpi_c::next_state() - external RCPI threshold level is %u",
                server_m->get_core_settings().rcp_decline_boundary() );
            
            DEBUG1( "core_operation_check_rcpi_c::next_state() - current RCPI is %u",
                current_rcpi_m );

            u8_t weak_trigger_level = server_m->get_core_settings().rcp_decline_boundary();
            if ( weak_trigger_level > server_m->get_device_settings().rcpi_trigger )
                {
                weak_trigger_level = server_m->get_device_settings().rcpi_trigger;
                }
            weak_trigger_level = weak_trigger_level - 20;
            
            DEBUG1( "core_operation_check_rcpi_c::next_state() - weak_trigger_level (%u)",
                               weak_trigger_level );
            /**
             * If we haven't notified the clients about a weak signal earlier,
             * do it now.
             */
            if ( current_rcpi_m < weak_trigger_level &&
                server_m->get_connection_data()->last_rcp_class() != core_rcp_weak )
                {
                DEBUG( "core_operation_check_rcpi_c::next_state() - sending a notification about the weak signal level" );

                u8_t buf[5];
                buf[0] = static_cast<u8_t>( core_rcp_weak );
                buf[1] = static_cast<u8_t>( current_rcpi_m );

                adaptation_m->notify(
                    core_notification_rcp_changed,
                    sizeof( buf ),
                    buf );
                server_m->get_connection_data()->set_last_rcp_class( core_rcp_weak ); 
                
                }

            if ( !is_connected_m )
                {
                DEBUG( "core_operation_check_rcpi_c::next_state() - no longer connected, scheduling bss_lost operation" );

                core_operation_base_c* operation = new core_operation_handle_bss_lost_c(
                    REQUEST_ID_CORE_INTERNAL,
                    server_m,
                    drivers_m,
                    adaptation_m,
                    core_operation_handle_bss_lost_c::core_bss_lost_reason_failed_rcpi_roam );

                server_m->queue_int_operation( operation );
                }
            else
                {
                /** 
                 * Send an indication to adaptation to indicate that this
                 * roam attempt has been completed.
                 */  
                adaptation_m->notify(                
                    core_notification_rcpi_roam_attempt_completed,
                    0,
                    NULL );
                
                core_rcpi_roam_interval_s interval(
                    server_m->get_connection_data()->rcpi_roam_interval() );
                ++interval.count;

                if ( interval.count > server_m->get_device_settings().rcpi_roam_attempts_per_interval )
                    {
                    DEBUG1( "core_operation_check_rcpi_c::next_state() - maximum interval attempts (%u) exceeded, increasing interval",
                        server_m->get_device_settings().rcpi_roam_attempts_per_interval );
                    interval.count = 1;
                    interval.interval *= server_m->get_device_settings().rcpi_roam_next_interval_factor;
                    interval.interval += server_m->get_device_settings().rcpi_roam_next_interval_addition;

                    if ( interval.interval > server_m->get_device_settings().rcpi_roam_max_interval )
                        {
                        interval.interval = server_m->get_device_settings().rcpi_roam_max_interval;
                        
                        DEBUG1( "core_operation_check_rcpi_c::next_state() - interval value set to maximum (%u)",
                            interval.interval );
                        }
                    else
                        {
                        DEBUG1( "core_operation_check_rcpi_c::next_state() - interval value set to %u",
                            interval.interval );
                        }
                    }

                server_m->get_connection_data()->set_core_rcpi_roam_interval(
                    interval );

                DEBUG1( "core_operation_check_rcpi_c::next_state() - arming the roaming timer to %u",
                    interval.interval );

                server_m->schedule_roam_timer(
                    interval.interval );
                
                if ( server_m->get_connection_data()->last_rcp_class() != core_rcp_weak )
                    {
                    DEBUG( "core_operation_check_rcpi_c::next_state() - set_rcpi_trigger_level( weak_trigger_level )" );
            
                    DEBUG1( "core_operation_check_rcpi_c::next_state() - arming new RCPI roam trigger (%u)",
                        weak_trigger_level );
               
                    drivers_m->set_rcpi_trigger_level(   
                         request_id_m,
                         weak_trigger_level );
               
                    return core_error_request_pending;
                    }
                }
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
core_error_e core_operation_check_rcpi_c::cancel()
    {
    DEBUG( "core_operation_check_rcpi_c::cancel() " );    
    
    switch ( operation_state_m )
        {              
        case core_state_connect_success:
            {            
            return goto_state( core_state_connect_failure );
            }
        default:
            {
            /** 
             * Send an indication to adaptation to indicate that this
             * roam attempt has been completed.
             */  
            adaptation_m->notify(                
                core_notification_rcpi_roam_attempt_completed,
                0,
                NULL );

            return failure_reason_m;
            }
        }    
    }
