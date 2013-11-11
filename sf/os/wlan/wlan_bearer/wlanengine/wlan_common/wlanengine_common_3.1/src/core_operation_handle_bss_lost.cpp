/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for handling BSS lost notification from drivers.
*
*/

/*
* %version: 34 %
*/

#include "core_operation_handle_bss_lost.h"
#include "core_operation_roam.h"
#include "core_operation_release.h"
#include "core_sub_operation_roam_scan.h"
#include "core_server.h"
#include "core_tools.h"
#include "core_am_tools.h"
#include "core_scan_list.h"
#include "am_debug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_handle_bss_lost_c::core_operation_handle_bss_lost_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_bss_lost_reason_e reason,
    u32_t reason_data ) :
    core_operation_base_c( core_operation_handle_bss_lost, request_id, server, drivers, adaptation,
        core_base_flag_drivers_needed | core_base_flag_connection_needed | core_base_flag_roam_operation ),
    reason_m( reason ),
    reason_data_m( reason_data ),
    failure_count_m( 0 ),
    is_connected_m( false_t ),
    scan_channels_m(),
    is_scan_channels_inverted_m( false_t ),
    release_reason_m( core_release_reason_max_roam_attempts_exceeded ),
    scan_interval_count_m( 0 ),
    scan_interval_m( 0 ),
    is_first_scan_m( true_t )
    {
    DEBUG( "core_operation_handle_bss_lost_c::core_operation_handle_bss_lost_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_handle_bss_lost_c::~core_operation_handle_bss_lost_c()
    {
    DEBUG( "core_operation_handle_bss_lost_c::~core_operation_handle_bss_lost_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_handle_bss_lost_c::next_state()
    {
    DEBUG( "core_operation_handle_bss_lost_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            core_mac_address_s bssid =
                server_m->get_connection_data()->current_ap_data()->bssid();

            /**
             * Update the reason for roaming.
             *
             * If the operation has been started because of a failed RCPI roam,
             * the original roam reason must not be changed.
             */
            if ( reason_m != core_bss_lost_reason_failed_rcpi_roam )
                {
                server_m->get_connection_data()->set_last_roam_failed_reason(
                    core_roam_failed_reason_none );

                switch ( reason_m )
                    {
                    case core_bss_lost_reason_media_disconnect:
                        server_m->get_connection_data()->set_last_roam_reason(
                            core_roam_reason_media_disconnect );
                        break;
                    case core_bss_lost_reason_failed_reauthentication:
                        server_m->get_connection_data()->set_last_roam_reason(
                            core_roam_reason_failed_reauthentication );
                        break;
                    case core_bss_lost_reason_bss_lost:
                        /** Falls through on purpose. */
                    default:
                        server_m->get_connection_data()->set_last_roam_reason(
                            core_roam_reason_bss_lost );                   
                        break;
                    }
                }

            /**
             * If the AP has deauthenticated us soon after a successful association
             * and EAPOL authentication, increase the deauthentication count. 
             * 
             * This check is done to detect APs that do MAC filtering after a
             * successful association. 
             */
            if ( reason_m == core_bss_lost_reason_media_disconnect &&
                 !server_m->get_connection_data()->is_eapol_authenticating() )
                {
                u64_t time = static_cast<u64_t>(
                    core_am_tools_c::timestamp() -
                    server_m->get_core_settings().roam_metrics().roam_ts_userdata_enabled() );

                DEBUG1( "core_operation_handle_bss_lost_c::next_state() - deauthenticated after being connected for %u second(s)",
                    static_cast<u32_t>( time / SECONDS_FROM_MICROSECONDS ) );

                if ( server_m->get_device_settings().max_ap_deauthentication_count &&
                     server_m->get_device_settings().ap_deauthentication_timeout &&
                     time <= server_m->get_device_settings().ap_deauthentication_timeout )
                    {
                    u8_t count( server_m->get_connection_data()->ap_deauthentication_count( bssid ) );
                    DEBUG1( "core_operation_handle_bss_lost_c::next_state() - AP deauthentication count was %u earlier",
                        count );

                    if ( count >= server_m->get_device_settings().max_ap_deauthentication_count - 1 )
                        {
                        DEBUG1( "core_operation_handle_bss_lost_c::next_state() - deauthentication count (%u) exceeded, blacklisting the AP",
                            server_m->get_device_settings().max_ap_deauthentication_count );
                        server_m->get_connection_data()->add_mac_to_temporary_blacklist(
                            bssid, core_ap_blacklist_reason_max_deauthentication_count );
                        }
                    else
                        {
                        DEBUG( "core_operation_handle_bss_lost_c::next_state() - increasing AP deauthentication count" );
                        server_m->get_connection_data()->increase_ap_deauthentication_count( bssid );
                        }
                    }
                }

            /**
             * Check the channels that were previously reported to be active.
             */
            server_m->get_scan_list().get_channels_by_ssid(
                scan_channels_m,
                server_m->get_connection_data()->ssid() );

            scan_channels_m.add(
                server_m->get_connection_data()->current_ap_data()->band(),
                server_m->get_connection_data()->current_ap_data()->channel() );

            scan_channels_m.merge(
                server_m->get_connection_data()->adjacent_ap_channels() );

            scan_channels_m.merge(
                server_m->get_connection_data()->current_ap_data()->ap_channel_report() );

            server_m->cancel_roam_timer();

            DEBUG( "core_operation_handle_bss_lost_c::next_state() - removing current AP entries from scan list" );
            server_m->get_scan_list().remove_entries_by_bssid(
                bssid );

            /**
             * If the connection is lost when EAPOL is doing (re-)authentication,
             * EAPOL must be notified.
             */
            if ( ( server_m->get_connection_data()->is_eapol_authenticating() ||
                   reason_m == core_bss_lost_reason_failed_reauthentication ) &&
                 ( server_m->get_connection_data()->iap_data().is_eap_used() ||
                   server_m->get_connection_data()->iap_data().is_wapi_used() ) )
                {
                network_id_c network_id(
                    &bssid.addr[0],
                    MAC_ADDR_LEN,
                    &server_m->own_mac_addr().addr[0],
                    MAC_ADDR_LEN,
                    server_m->get_eapol_instance().ethernet_type() );

                DEBUG( "core_operation_handle_bss_lost_c::next_state() - marking is_eapol_authenticating as false" );
                server_m->get_connection_data()->set_eapol_authenticating(
                    false_t );

                DEBUG6( "core_operation_handle_bss_lost_c::next_state() - EAPOL disassociation from BSSID %02X:%02X:%02X:%02X:%02X:%02X",
                    bssid.addr[0], bssid.addr[1], bssid.addr[2],
                    bssid.addr[3], bssid.addr[4], bssid.addr[5] );
                DEBUG( "core_operation_handle_bss_lost_c::next_state() - marking is_eapol_disconnecting as true" );
                server_m->get_connection_data()->set_eapol_disconnecting(
                    true );
                server_m->get_connection_data()->set_eapol_auth_failure(
                    core_error_eapol_failure );

                server_m->get_eapol_instance().disassociation( &network_id );
                operation_state_m = core_state_eapol_disassociated;

                return core_error_request_pending;
                }

            return goto_state( core_state_eapol_disassociated );
            }            
        case core_state_eapol_disassociated:
            {            
            operation_state_m = core_state_set_tx_level;            

            server_m->get_core_settings().roam_metrics().set_roam_ts_userdata_disabled();

            drivers_m->disable_user_data(
                request_id_m );

            break;
            }
        case core_state_set_tx_level:
            {
            operation_state_m = core_state_set_tx_level_success;

            DEBUG1( "core_operation_handle_bss_lost_c::next_state() - setting tx power (%u)",
                server_m->get_device_settings().tx_power_level );

            drivers_m->set_tx_power_level(
                request_id_m,
                server_m->get_device_settings().tx_power_level );

            break;
            }
        case core_state_set_tx_level_success:
            {
            u32_t tx_level = server_m->get_device_settings().tx_power_level;
            if ( server_m->get_connection_data()->last_tx_level() != tx_level )
                {
                DEBUG( "core_operation_handle_bss_lost_c::next_state() - TX level has changed, notifying change" );

                adaptation_m->notify(
                    core_notification_tx_power_level_changed,
                    sizeof( tx_level ),
                    reinterpret_cast<u8_t*>(&tx_level) );

                server_m->get_connection_data()->set_last_tx_level( tx_level );
                }

            return goto_state( core_state_scan_start );
            }
        case core_state_scan_start:
            {
            operation_state_m = core_state_scan_complete;

            server_m->get_scan_list().set_tag(
                core_scan_list_tag_roam_scan );

            core_operation_base_c* operation = new core_sub_operation_roam_scan_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                server_m->get_connection_data()->ssid(),
                scan_channels_m.channels(),
                false_t,
                true_t );                

            return run_sub_operation( operation );            
            }
        case core_state_scan_complete:
            {
            operation_state_m = core_state_connect_success;

            DEBUG( "core_operation_handle_bss_lost_c::next_state() - trying to connect" );

            core_operation_base_c* operation = new core_operation_roam_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                is_connected_m,
                core_scan_list_tag_roam_scan,
                RCPI_VALUE_NONE,
                MEDIUM_TIME_NOT_DEFINED,
                server_m->get_connection_data()->ssid(),
                BROADCAST_MAC_ADDR );

            return run_sub_operation( operation );
            }                        
        case core_state_connect_success:
            {
            operation_state_m = core_state_connect_set_rcpi_trigger;

            DEBUG( "core_operation_handle_bss_lost_c::next_state() - roamed successfully" );

            DEBUG( "core_operation_handle_bss_lost_c::next_state() - reseting RCPI roam check interval" );
            server_m->get_connection_data()->reset_rcpi_roam_interval();

            DEBUG1( "core_operation_handle_bss_lost_c::next_state() - internal RCPI threshold level is %u",
                server_m->get_device_settings().rcpi_trigger );
            DEBUG1( "core_operation_handle_bss_lost_c::next_state() - external RCPI threshold level is %u",
                server_m->get_core_settings().rcp_decline_boundary() );

            u8_t trigger_level = server_m->get_core_settings().rcp_decline_boundary();
            if ( trigger_level < server_m->get_device_settings().rcpi_trigger )
                {
                trigger_level = server_m->get_device_settings().rcpi_trigger;
                }
            DEBUG1( "core_operation_handle_bss_lost_c::next_state() - arming RCPI roam trigger (%u)",
                trigger_level );

            drivers_m->set_rcpi_trigger_level(
                request_id_m,
                trigger_level );

            break;
            }
        case core_state_connect_set_rcpi_trigger:
            {
            return core_error_ok;
            }
        case core_state_connect_failure:
            {
            if( failure_reason_m == core_error_not_found )
                {
                if( is_first_scan_m )
                    {
                    /**
                     * This scan is part of the scans done in the beginning
                     * using partial channel masks.
                     */

                    if( !is_scan_channels_inverted_m )
                        {
                        /**
                         * If no suitable APs were found on the first scan, we scan
                         * immediately the remaining channels.
                         */
                        DEBUG( "core_operation_handle_bss_lost_c::next_state() - inverting channel mask" );
    
                        is_scan_channels_inverted_m = true_t;                    
                        scan_channels_m.invert_channels();
    
                        core_scan_channels_s filtered_channels(
                            server_m->get_core_settings().valid_scan_channels( scan_channels_m.channels() ) );
    
                        if( !filtered_channels.channels2dot4ghz[0] &&
                            !filtered_channels.channels2dot4ghz[1] )
                            {
                            DEBUG( "core_operation_handle_bss_lost_c::next_state() - inverted channel mask is zero, using full channel mask" );
    
                            scan_channels_m.set(
                                SCAN_CHANNELS_2DOT4GHZ_ETSI );
                            }
                        }
                    else
                        {
                        /**
                         * We start scanning using the normal algorithm and
                         * a full channel mask.
                         */
                        is_first_scan_m = false_t;
                        scan_interval_m =
                            server_m->get_device_settings().bss_lost_roam_min_interval;
                        scan_channels_m.set(
                            SCAN_CHANNELS_2DOT4GHZ_ETSI );
                        }
                    }
                else
                    {
                    scan_channels_m.set(
                        SCAN_CHANNELS_2DOT4GHZ_ETSI );

                    ++failure_count_m;
                    ++scan_interval_count_m;
                    if( scan_interval_m < server_m->get_device_settings().bss_lost_roam_max_interval &&
                        scan_interval_count_m >= server_m->get_device_settings().bss_lost_roam_attempts_per_interval )
                        {
                        DEBUG1( "core_operation_handle_bss_lost_c::next_state() - maximum interval attempts (%u) exceeded, increasing interval",
                            server_m->get_device_settings().bss_lost_roam_attempts_per_interval );
                        scan_interval_count_m = 0;
                        scan_interval_m *= server_m->get_device_settings().bss_lost_roam_next_interval_factor;
                        scan_interval_m += server_m->get_device_settings().bss_lost_roam_next_interval_addition;                        

                        if( scan_interval_m > server_m->get_device_settings().bss_lost_roam_max_interval )
                            {
                            scan_interval_m = server_m->get_device_settings().bss_lost_roam_max_interval;

                            DEBUG1( "core_operation_handle_bss_lost_c::next_state() - interval value set to maximum (%u)",
                                scan_interval_m );                            
                            }
                        }
                    }
                }
            else
                {
                /**
                 * The roaming failed because of some other reason than not
                 * finding a suitable AP.
                 */
                is_first_scan_m = false_t;
                failure_count_m = 0;
                scan_interval_count_m = 0;
                scan_interval_m =
                    server_m->get_device_settings().bss_lost_roam_min_interval;

                scan_channels_m.set(
                    SCAN_CHANNELS_2DOT4GHZ_ETSI );
                }

            DEBUG1( "core_operation_handle_bss_lost_c::next_state() - roaming has failed %u time(s)",
                failure_count_m );
            DEBUG1( "core_operation_handle_bss_lost_c::next_state() - failure_reason_m is %u",
                failure_reason_m );
            DEBUG1( "core_operation_handle_bss_lost_c::next_state() - connect_status is %u",
                server_m->get_connection_data()->connect_status() );

            /**
             * If EAPOL has notified about a total failure or we have tried too many times,
             * close down the connection. Otherwise, try again.
             */
            if ( failure_reason_m == core_error_eapol_total_failure ||
                 failure_reason_m == core_error_eapol_canceled_by_user ||
                 failure_count_m >= server_m->get_device_settings().bss_lost_roam_max_tries_to_find_nw )
                {
                return goto_state( core_state_connect_total_failure );
                }

            DEBUG1( "core_operation_handle_bss_lost_c::next_state() - next scan in %u microsecond(s)",
                scan_interval_m );

            return asynch_goto(
                core_state_scan_start,
                scan_interval_m );
            }
        case core_state_connect_total_failure:
            {
            operation_state_m = core_state_connect_disconnect;

            DEBUG1( "core_operation_handle_bss_lost_c::next_state() - gave up roaming after %u tries",
                failure_count_m );

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
            DEBUG( "core_operation_handle_bss_lost_c::next_state() - connection closed successfully" );

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
void core_operation_handle_bss_lost_c::user_cancel(
    bool_t /* do_graceful_cancel */ )
    {
    DEBUG( "core_operation_handle_bss_lost_c::user_cancel()" );

    release_reason_m = core_release_reason_external_request;

    operation_state_m = core_state_connect_total_failure;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_handle_bss_lost_c::cancel()
    {
    DEBUG( "core_operation_handle_bss_lost_c::cancel() " );    

    switch ( operation_state_m )
        {              
        case core_state_connect_success:
            {
            return goto_state( core_state_connect_failure );
            }
        default:
            {
            return failure_reason_m;
            }
        }
    }
