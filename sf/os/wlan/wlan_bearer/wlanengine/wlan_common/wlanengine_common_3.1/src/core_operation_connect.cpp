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
* %version: 41 %
*/

#include "genscaninfo.h"
#include "core_operation_connect.h"
#include "core_sub_operation_set_static_wep.h"
#include "core_sub_operation_roam_scan.h"
#include "core_operation_roam.h"
#include "core_operation_release.h"
#include "core_operation_update_power_mode.h"
#include "core_operation_power_save_test.h"
#include "abs_core_timer.h"
#include "core_timer_factory.h"
#include "core_server.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

/** The time to wait for DHCP completion after connect */
const u32_t UPDATE_IP_ADDR_TIMEOUT = 7*1000000; // 7 seconds

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_connect_c::core_operation_connect_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    const core_iap_data_s& settings,
    core_type_list_c<core_ssid_entry_s>* ssid_list,
    core_connect_status_e& connect_status ) :
    core_operation_base_c( core_operation_connect, request_id, server, drivers, adaptation, 
        core_base_flag_drivers_needed | core_base_flag_roam_operation ),
    settings_m( settings ),
    connect_status_m( connect_status ),
    failure_count_m( 0 ),
    is_connected_m( false_t ),
    ssid_m( settings.ssid ),
    ssid_list_m( ssid_list ),
    release_reason_m( core_release_reason_max_roam_attempts_exceeded )
    {
    DEBUG( "core_operation_connect_c::core_operation_connect_c()" );

    connect_status_m = core_connect_undefined;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_connect_c::~core_operation_connect_c()
    {
    DEBUG( "core_operation_connect_c::~core_operation_connect_c()" );
    }
   
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_connect_c::next_state()
    {
    DEBUG( "core_operation_connect_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            if ( server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_connect_c::next_state() - already connected, completing request" );

                return core_error_connection_already_active;
                }            

            core_error_e ret = server_m->init_connection_data(
                settings_m,
                server_m->get_device_settings() );
            if( ret != core_error_ok )
                {
                DEBUG1( "core_operation_connect_c::next_state() - unable to initialize connection data (%d)", ret );

                return ret;
                }

            if( !server_m->get_connection_data()->iap_data().is_valid() )
                {
                DEBUG( "core_operation_connect_c::next_state() - invalid connection settings" );

                server_m->clear_connection_data();

                return core_error_illegal_argument;
                }

            if( server_m->get_connection_data()->iap_data().is_eap_used() &&
                !server_m->create_eapol_instance( core_eapol_operating_mode_wfa ) )
                {
                DEBUG( "core_operation_connect_c::next_state() - unable to instantiate EAPOL (WFA)" );

                server_m->clear_connection_data();

                return core_error_no_memory;
                }
            else if( server_m->get_connection_data()->iap_data().is_wapi_used() &&
                     !server_m->create_eapol_instance( core_eapol_operating_mode_wapi ) )
                {
                DEBUG( "core_operation_connect_c::next_state() - unable to instantiate EAPOL (WAPI)" );

                server_m->clear_connection_data();

                return core_error_no_memory;
                }

            server_m->get_core_settings().clear_connection_statistics();
            server_m->get_core_settings().roam_metrics().set_roam_ts_userdata_disabled();

            server_m->get_connection_data()->set_last_roam_reason(
                core_roam_reason_initial_connect );
            server_m->get_core_settings().roam_metrics().inc_roam_attempt_count(
                core_roam_reason_initial_connect );

            DEBUG( "core_operation_connect_c::next_state() - reseting RCPI roam check interval" );
            server_m->get_connection_data()->reset_rcpi_roam_interval();

            /**
             * Indicate core_connection_state_searching state to adaptation.
             */
            core_connection_state_e state = core_connection_state_searching;
            server_m->get_core_settings().set_connection_state( state );

            if ( server_m->get_connection_data()->last_connection_state() != state )
                {
                u8_t buf[5];
                buf[0] = static_cast<u8_t>( state );
                adaptation_m->notify(
                    core_notification_connection_state_changed,
                    1,
                    buf );

                server_m->get_connection_data()->set_last_connection_state(
                    state );
                }

            /**
             * Disable the power save mode before first connect.
             */
            core_operation_base_c* operation = new core_operation_update_power_mode_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                CORE_POWER_SAVE_MODE_NONE );

            return run_sub_operation( operation, core_state_power_mode );
            }
        case core_state_power_mode:
            {
            if ( ssid_list_m &&
                 ssid_list_m->count() )
                {
                DEBUG1( "core_operation_connect_c::next_state() - IAP has %u secondary SSID(s) defined",
                    ssid_list_m->count() );

                operation_state_m = core_state_secondary_ssid_scan_start;
                }
            else
                {
                operation_state_m = core_state_scan_start;
                }

            if ( server_m->get_connection_data()->iap_data().security_mode() == core_security_mode_wep )
                {
                core_operation_base_c* operation = new core_sub_operation_set_static_wep_c(
                    request_id_m,
                    server_m,
                    drivers_m,
                    adaptation_m );

                return run_sub_operation( operation );
                }

            return next_state();
            }
        case core_state_secondary_ssid_scan_start:
            {
            DEBUG( "core_operation_connect_c::next_state() - doing a broadcast scan to detect available secondary SSIDs" );

            ASSERT( ssid_list_m );
            ASSERT( ssid_list_m->count() );
            (void)ssid_list_m->first();

            server_m->get_scan_list().set_tag(
                core_scan_list_tag_scan );

            core_operation_base_c* operation = new core_sub_operation_roam_scan_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                BROADCAST_SSID,
                server_m->get_core_settings().all_valid_scan_channels(),
                false_t,
                false_t );

            return run_sub_operation( operation, core_state_secondary_ssid_scan );
            }
        case core_state_secondary_ssid_scan:
            {
            ASSERT( ssid_list_m );
            ASSERT( ssid_list_m->count() );

            core_ssid_entry_s* entry =
                ssid_list_m->current();
            ASSERT( entry );

            DEBUG( "core_operation_connect_c::next_state() - searching broadcast scan results for the secondary SSID:" );
            DEBUG1S( "core_operation_connect_c::next_state() - ssid: ",
                entry->ssid.length, &entry->ssid.ssid[0] );
            DEBUG1S( "core_operation_connect_c::next_state() - used_ssid: ",
                entry->used_ssid.length, &entry->used_ssid.ssid[0] );

            core_scan_list_iterator_by_tag_and_ssid_c iter(
                server_m->get_scan_list(),
                core_scan_list_tag_scan,
                entry->ssid );

            if ( iter.first() != NULL )
                {
                DEBUG( "core_operation_connect_c::next_state() - secondary SSID match found" );
                ssid_m = entry->used_ssid;

                return goto_state( core_state_scan_start );
                }

            DEBUG( "core_operation_connect_c::next_state() - secondary SSID match not found" );

            return goto_state( core_state_connect_secondary_ssid_failure );
            }
        case core_state_scan_start:
            {
            operation_state_m = core_state_scan_complete;

            DEBUG( "core_operation_connect_c::next_state() - starting a direct scan on all channels" );

            server_m->get_scan_list().set_tag(
                core_scan_list_tag_roam_scan );

            core_operation_base_c* operation = new core_sub_operation_roam_scan_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                ssid_m,
                server_m->get_core_settings().all_valid_scan_channels(),
                false_t,
                false_t );

            return run_sub_operation( operation );
            }
        case core_state_scan_complete:
            {
            operation_state_m = core_state_connect_success;

            DEBUG1S( "core_operation_connect_c::next_state() - trying to connect to SSID ",
                ssid_m.length, &ssid_m.ssid[0] );

            core_operation_base_c* operation = new core_operation_roam_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                is_connected_m,
                core_scan_list_tag_roam_scan,
                RCPI_VALUE_NONE,
                MEDIUM_TIME_NOT_DEFINED,                                
                ssid_m,
                BROADCAST_MAC_ADDR );

            return run_sub_operation( operation );
            }            
        case core_state_connect_failure:
            {
            if ( failure_reason_m == core_error_not_found )
                {
                failure_count_m++;

                if ( ssid_list_m &&
                     ssid_list_m->count() )
                    {
                    return goto_state( core_state_connect_secondary_ssid_failure );
                    }
                }
            else
                {
                failure_count_m = 0;
                }

            DEBUG1( "core_operation_connect_c::next_state() - connecting has failed %u time(s)",
                failure_count_m );
            DEBUG1( "core_operation_connect_c::next_state() - failure_reason_m is %u",
                failure_reason_m );
            DEBUG1( "core_operation_connect_c::next_state() - connect_status is %u",
                server_m->get_connection_data()->connect_status() );

            /**
             * If EAPOL has notified about a total failure or we have tried too many times,
             * close down the connection. Otherwise, try again.
             */
            if ( failure_reason_m == core_error_eapol_total_failure ||
                 failure_reason_m == core_error_eapol_canceled_by_user ||
                 failure_count_m >= server_m->get_device_settings().max_tries_to_find_nw )
                {
                return goto_state( core_state_connect_total_failure );
                }

            return asynch_goto(
                core_state_scan_start,
                server_m->get_device_settings().delay_between_find_nw );
            }
        case core_state_connect_secondary_ssid_failure:
            {
            DEBUG( "core_operation_connect_c::next_state() - trying the next secondary SSID entry" );

            core_ssid_entry_s* entry =
                ssid_list_m->next();
            if ( entry )
                {
                return asynch_goto( core_state_secondary_ssid_scan );
                }

            DEBUG( "core_operation_connect_c::next_state() - no more secondary SSID entries to try, closing connection" );
            if ( server_m->get_connection_data()->connect_status() == core_connect_undefined )
                {
                server_m->get_connection_data()->set_connect_status( core_connect_network_not_found );
                }

            return goto_state( core_state_connect_total_failure );
            }
        case core_state_connect_total_failure:
            {
            operation_state_m = core_state_connect_disconnect;

            DEBUG1( "core_operation_connect_c::next_state() - gave up connecting after %u tries",
                failure_count_m );

            /**
             * Complete adaptation with the same status as was set to connection data.
             */
            connect_status_m = server_m->get_connection_data()->connect_status();

            core_operation_base_c* operation = new core_operation_release_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                release_reason_m );

            return run_sub_operation( operation );
            }
        case core_state_connect_disconnect:
            {
            DEBUG( "core_operation_connect_c::next_state() - connection closed successfully" );            
            return core_error_ok;
            }
        case core_state_connect_success:
            {
            operation_state_m = core_state_set_rcpi_trigger;

            DEBUG( "core_operation_connect_c::next_state() - connected successfully" );

            /**
             * If no roaming between APs is allowed, add the current AP to the
             * whitelist to prevent roaming in BSS lost situations.
             */
            if ( !server_m->get_connection_data()->iap_data().is_roaming_allowed() &&
                 server_m->get_connection_data()->iap_data().operating_mode() == core_operating_mode_infrastructure )
                {
                const core_mac_address_s current_bssid =
                    server_m->get_connection_data()->current_ap_data()->bssid();

                server_m->get_connection_data()->iap_data().add_mac_to_iap_whitelist(
                    current_bssid );
                }

            /**
             * If power save is enabled in infrastructure mode, schedule a power save test.
             */
            if ( server_m->get_connection_data()->iap_data().operating_mode() == core_operating_mode_infrastructure &&
                 server_m->get_device_settings().power_save_enabled )
                {
                if ( server_m->get_connection_data()->iap_data().is_dynamic_ip_addr() )
                    {
                    DEBUG( "core_operation_connect_c::next_state() - scheduling a power save test after DHCP timeout" );
                    server_m->schedule_dhcp_timer( UPDATE_IP_ADDR_TIMEOUT );
                    }
                else
                    {
                    DEBUG( "core_operation_connect_c::next_state() - scheduling a power save test" );

                    core_operation_base_c* operation = new core_operation_power_save_test_c(
                        REQUEST_ID_CORE_INTERNAL,
                        server_m,
                        drivers_m,
                        adaptation_m );

                    server_m->queue_int_operation( operation );
                    }
                }

            /**
             * RCPI trigger is only used in infrastructure mode.
             */
            if ( server_m->get_connection_data()->iap_data().operating_mode() !=
                core_operating_mode_infrastructure )
                {
                return next_state();
                }

            DEBUG1( "core_operation_connect_c::next_state() - internal RCPI threshold level is %u",
                server_m->get_device_settings().rcpi_trigger );
            DEBUG1( "core_operation_connect_c::next_state() - external RCPI threshold level is %u",
                server_m->get_core_settings().rcp_decline_boundary() );

            u8_t trigger_level = server_m->get_core_settings().rcp_decline_boundary();
            if ( trigger_level < server_m->get_device_settings().rcpi_trigger )
                {
                trigger_level = server_m->get_device_settings().rcpi_trigger;
                }
            DEBUG1( "core_operation_connect_c::next_state() - arming RCPI roam trigger (%u)",
                trigger_level );

            drivers_m->set_rcpi_trigger_level(
                request_id_m,
                trigger_level );

            break;
            }
        case core_state_set_rcpi_trigger:
            {
            // Connection is OK, complete connection data
            server_m->get_connection_data()->set_connect_status( core_connect_ok );
            // complete adaptation status
            connect_status_m = core_connect_ok;
            // complete operation

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
void core_operation_connect_c::user_cancel(
    bool_t /* do_graceful_cancel */ )
    {
    DEBUG( "core_operation_connect_c::user_cancel()" );

    release_reason_m = core_release_reason_external_request;

    operation_state_m = core_state_connect_total_failure;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_connect_c::cancel()
    {
    DEBUG( "core_operation_connect_c::cancel() " );    

    switch( operation_state_m )
        {
        case core_state_connect_success:
            {
            return goto_state( core_state_connect_failure );
            }
        default:
            {
            // complete adaptation with the same status as was just set to connection data
            connect_status_m = server_m->get_connection_data()->connect_status();
            // connect operation returns OK if possible
            return core_error_ok;
            }
        }
    }
