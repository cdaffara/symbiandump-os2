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
* Description:  Statemachine for roaming
*
*/

/*
* %version: 78 %
*/

#include "core_operation_roam.h"
#include "core_server.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "core_sub_operation_connect.h"
#include "core_sub_operation_adhoc.h"
#include "core_sub_operation_wep_connect.h"
#include "core_sub_operation_wpa_connect.h"
#include "core_sub_operation_roam_update_ts.h"
#include "core_operation_update_power_mode.h"
#include "core_frame_assoc_resp.h"
#include "core_frame_dot11.h"
#include "core_frame_qbss_load_ie.h"
#include "core_scan_list.h"
#include "core_frame_nr_ie.h"
#include "core_frame_action_nr.h"
#include "core_frame_action_rm.h"
#include "core_frame_radio_measurement_action.h"
#include "am_debug.h"

const u32_t CORE_ROAMING_LIST_BONUS_PMKSA = 20;
const u32_t CORE_ROAMING_LIST_BONUS_WPX_FAST_ROAM = 20;

/** The maximum amount association/authentication failures when WAPI is used. */
const u32_t CORE_MAX_WAPI_AP_ASSOCIATION_FAILURE_COUNT = 10; 

/** Definitions for RRM Enabled Capabilities bitmask */
const u64_t RRM_CAPABILITY_BIT_MASK_NEIGHBOR_REPORT = 0x0000000002;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_roam_c::core_operation_roam_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    bool_t& is_connected,
    u8_t tag,        
    u8_t min_required_rcpi,
    const medium_time_s& min_medium_time,        
    const core_ssid_s& ssid,
    const core_mac_address_s& bssid ) :
    core_operation_base_c( core_operation_roam, request_id, server, drivers, adaptation, 
        core_base_flag_drivers_needed ),
    is_connected_m( is_connected ),
    tag_m( tag ),
    min_required_rcpi_m( min_required_rcpi ),
    min_medium_time_m( min_medium_time ),
    ssid_m( ssid ),
    bssid_m( bssid ),    
    roaming_list_m( ),
    management_status_m( core_management_status_success ),
    connect_status_m( core_connect_network_not_found ),
    current_ap_m( NULL ),
    current_bssid_m( ZERO_MAC_ADDR ),
    assoc_ie_list_m( false_t ),
    is_cached_sa_used_m( false_t ),
    assoc_resp_m( NULL )
    {
    DEBUG( "core_operation_roam_c::core_operation_roam_c()" );    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_roam_c::~core_operation_roam_c()
    {
    DEBUG( "core_operation_roam_c::~core_operation_roam_c()" );

    for ( roaming_list_entry_s* iter = roaming_list_m.first(); iter; iter = roaming_list_m.next() )
        {
        delete iter->ap_data;
        }

    roaming_list_m.clear();
    assoc_ie_list_m.clear();
    delete assoc_resp_m;
    current_ap_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_roam_c::next_state()
    {
    DEBUG( "core_operation_roam_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            if( server_m->get_connection_data()->last_roam_reason() != core_roam_reason_initial_connect )
                {
                server_m->get_core_settings().roam_metrics().inc_roam_attempt_count(
                    server_m->get_connection_data()->last_roam_reason() );
                }

            if( is_connected_m )
                {
                current_bssid_m = server_m->get_connection_data()->current_ap_data()->bssid();

                DEBUG6( "core_operation_roam_c::next_state() - current BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
                    current_bssid_m.addr[0], current_bssid_m.addr[1], current_bssid_m.addr[2],
                    current_bssid_m.addr[3], current_bssid_m.addr[4], current_bssid_m.addr[5] );
                }

            DEBUG6( "core_operation_roam_c::next_state() - requested BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
                bssid_m.addr[0], bssid_m.addr[1], bssid_m.addr[2],
                bssid_m.addr[3], bssid_m.addr[4], bssid_m.addr[5] );           
            DEBUG1( "core_operation_roam_c::next_state() - minimum required RCPI value: %u",
                min_required_rcpi_m );

            /**
             * Set all the traffic streams as not active and reset TSPEC parameters to the
             * default values.
             */
            core_traffic_stream_list_c& ts_list(
                server_m->get_connection_data()->traffic_stream_list() );
            for( core_traffic_stream_c* iter = ts_list.first(); iter; iter = ts_list.next() )
                {
                iter->set_status( core_traffic_stream_status_undefined );
                iter->reset_to_default_values();
                }

            if ( !is_connected_m )
                {
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
                }

            server_m->get_scan_list().print_contents();

            DEBUG1( "core_operation_roam_c::next_state() - tag entry for iterator is 0x%02X",
                tag_m );
            core_scan_list_iterator_by_tag_and_ssid_and_bssid_c iter(
                server_m->get_scan_list(),
                tag_m,
                ssid_m,
                bssid_m );
            for ( core_ap_data_c* ap_data = iter.first(); ap_data; ap_data = iter.next() )
                {
                if ( process_frame( *ap_data ) )
                    {
                    DEBUG( "core_operation_roam_c::next_state() - AP added to roaming list" );
                    }                
                }               

#ifdef _DEBUG
            DEBUG( "core_operation_roam_c::next_state() - roaming list start" );

            roaming_list_entry_s* entry_iter = roaming_list_m.first();
            TUint idx( 1 );

            while ( entry_iter )
                {
                const core_mac_address_s bssid = entry_iter->ap_data->bssid();

                DEBUG8( "core_operation_roam_c::next_state() - entry #%02u %02X:%02X:%02X:%02X:%02X:%02X (RCPI %u)",
                    idx, bssid.addr[0], bssid.addr[1], bssid.addr[2], 
                    bssid.addr[3], bssid.addr[4], bssid.addr[5], entry_iter->ap_data->rcpi() );

                entry_iter = roaming_list_m.next();
                ++idx;
                }

            DEBUG( "core_operation_roam_c::next_state() - roaming list end" );
#endif // _DEBUG            

            if ( roaming_list_m.first() )
                {
                current_ap_m = roaming_list_m.first()->ap_data;
                }

            /**
             * We can proceed with the connection if there are entries in
             * the roaming list OR we are going to start our own IBSS network.
             */
            if( current_ap_m ||
                ( !is_connected_m &&
                  server_m->get_connection_data()->iap_data().operating_mode() == core_operating_mode_ibss ) )
                {
                operation_state_m = core_state_req_disable_userdata_before_connect;
                
                if ( !is_connected_m )
                    {
                    return next_state();
                    }

                DEBUG( "core_operation_roam_c::next_state() - disabling user data before connect" );

                server_m->get_core_settings().roam_metrics().set_roam_ts_userdata_disabled();

                drivers_m->disable_user_data(
                    request_id_m );
                }
            else
                {
                DEBUG( "core_operation_roam_c::next_state() - no suitable AP found" );
                failure_reason_m = core_error_not_found;
                if ( server_m->get_connection_data()->connect_status() == core_connect_undefined )
                    {
                    server_m->get_connection_data()->set_connect_status( connect_status_m );
                    }

                server_m->get_core_settings().roam_metrics().inc_roam_attempt_failed_count(
                    core_roam_failed_reason_no_suitable_ap );
                if ( server_m->get_connection_data()->last_roam_failed_reason() == core_roam_failed_reason_none )
                    {
                    server_m->get_connection_data()->set_last_roam_failed_reason( core_roam_failed_reason_no_suitable_ap );
                    }

                /**
                 * Set all traffic stream statuses and parameters back to the values
                 * they were before the roam attempt.
                 */
                core_traffic_stream_list_c& ts_list(
                    server_m->get_connection_data()->traffic_stream_list() );

                for( core_traffic_stream_c* iter = ts_list.first(); iter; iter = ts_list.next() )
                    {
                    iter->set_status( core_traffic_stream_status_active );
                    iter->reset_to_previous_values();
                    }

                return cancel();
                }            

            break;                
            }
        case core_state_req_disable_userdata_before_connect:
            {
            operation_state_m = core_state_req_connect;

#ifdef _DEBUG
            if( current_ap_m )
                {
                const core_mac_address_s current_bssid =
                    current_ap_m->bssid();

                DEBUG6( "core_operation_roam_c::next_state() - selecting BSSID %02X:%02X:%02X:%02X:%02X:%02X",
                    current_bssid.addr[0], current_bssid.addr[1], current_bssid.addr[2],
                    current_bssid.addr[3], current_bssid.addr[4], current_bssid.addr[5] );                
                }
            else
                {
                DEBUG("core_operation_roam_c::next_state() - establishing adhoc network");
                }
#endif // _DEBUG

            core_security_mode_e mode(
                server_m->get_connection_data()->iap_data().security_mode() );
            bool_t is_ibss_mode(
                server_m->get_connection_data()->iap_data().operating_mode() == core_operating_mode_ibss );

            /**
             * Establish Adhoc network
             */
            if( !current_ap_m &&
                is_ibss_mode )
                {
                core_operation_base_c* operation = new core_sub_operation_adhoc_c(
                    request_id_m,
                    server_m,
                    drivers_m,
                    adaptation_m,
                    &current_ap_m ); // conveying a pointer to pointer

                return run_sub_operation( operation );
                }

            /**
             * IAP with no security.
             */
            if ( mode == core_security_mode_allow_unsecure )                     
                {                                        
                core_operation_base_c* operation = new core_sub_operation_connect_c(
                    request_id_m,
                    server_m,
                    drivers_m,
                    adaptation_m,
                    is_connected_m,
                    management_status_m,
                    ssid_m,
                    *current_ap_m,
                    core_authentication_mode_open,
                    core_encryption_mode_disabled,
                    core_cipher_key_type_none,
                    assoc_ie_list_m,
                    is_ibss_mode ? NULL : &assoc_resp_m,
                    true_t,    // ignored, pairwise key not used
                    true_t,    // ignored, group key not used
                    NULL );

                return run_sub_operation( operation );
                }

            /**
             * IAP with static WEP.
             */
            if ( mode == core_security_mode_wep )
                {
                core_operation_base_c* operation = new core_sub_operation_wep_connect_c(
                    request_id_m,
                    server_m,
                    drivers_m,
                    adaptation_m,
                    is_connected_m,
                    management_status_m,
                    ssid_m,
                    *current_ap_m,
                    server_m->get_connection_data()->iap_data().authentication_mode(),
                    core_encryption_mode_wep,
                    assoc_ie_list_m,
                    is_ibss_mode ? NULL : &assoc_resp_m );
                        
                return run_sub_operation( operation );
                }

            /**
             * IAP with EAP.
             */
            is_cached_sa_used_m = roaming_list_m.first()->is_cached_sa_available;

            core_operation_base_c* operation = new core_sub_operation_wpa_connect_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                is_connected_m,
                management_status_m,
                ssid_m,
                *current_ap_m,
                is_cached_sa_used_m,
                assoc_ie_list_m,
                is_ibss_mode ? NULL : &assoc_resp_m );

            return run_sub_operation( operation );
            }
        case core_state_req_connect:
            {
            operation_state_m = core_state_req_update_ts;
            
            DEBUG( "core_operation_roam_c::next_state() - connection success" );
    
            /**
             * Send information about the current AP to the subscribed clients.
             *
             * This has to be done before updating traffic stream statuses so
             * that NIF won't try to recreate rejected streams.
             */
            core_ap_information_s info = core_tools_parser_c::get_ap_info(
                server_m->get_connection_data()->iap_data(),
                *current_ap_m );
            DEBUG( "core_operation_roam_c::next_state() - notifying access point information:" );
            DEBUG1( "core_operation_roam_c::next_state() - is_ac_required_for_voice = %u",
                info.is_ac_required_for_voice );
            DEBUG1( "core_operation_roam_c::next_state() - is_ac_required_for_video = %u",
                info.is_ac_required_for_video );
            DEBUG1( "core_operation_roam_c::next_state() - is_ac_required_for_best_effort = %u",
                info.is_ac_required_for_best_effort );
            DEBUG1( "core_operation_roam_c::next_state() - is_ac_required_for_background = %u",
                info.is_ac_required_for_background );
            DEBUG1( "core_operation_roam_c::next_state() - is_wpx = %u",
                info.is_wpx );
            adaptation_m->notify(
                core_notification_ap_info_changed,
                sizeof ( info ),
                reinterpret_cast<u8_t*>( &info ) );

            if ( server_m->get_connection_data()->current_ap_data() &&
                 server_m->get_connection_data()->current_ap_data()->bssid() != current_ap_m->bssid() )
                {
                server_m->get_connection_data()->set_previous_ap_data(
                    *server_m->get_connection_data()->current_ap_data() );

                /**
                 * EAPOL must notified about the disassociation.
                 */
                if ( server_m->get_connection_data()->iap_data().is_eap_used() ||
                     server_m->get_connection_data()->iap_data().is_wapi_used() )
                    {
                    core_mac_address_s bssid =
                        server_m->get_connection_data()->previous_ap_data()->bssid();

                    network_id_c network(
                        &bssid.addr[0],
                        MAC_ADDR_LEN,
                        &server_m->own_mac_addr().addr[0],
                        MAC_ADDR_LEN,
                        server_m->get_eapol_instance().ethernet_type() );

                    DEBUG6( "core_operation_roam_c::next_state() - EAPOL disassociation from BSSID %02X:%02X:%02X:%02X:%02X:%02X",
                        bssid.addr[0], bssid.addr[1], bssid.addr[2],
                        bssid.addr[3], bssid.addr[4], bssid.addr[5] );

                    /**
                     * is_eapol_disconnecting is not updated here because this disassociation
                     * is for the previous AP and thus we don't really care when this
                     * request gets completed.
                     */

                    server_m->get_eapol_instance().disassociation( &network );
                    }
                }

            server_m->get_connection_data()->set_current_ap_data(
                *current_ap_m );

            core_operation_base_c* operation = new core_sub_operation_roam_update_ts_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                *current_ap_m );

            return run_sub_operation( operation );           
            }        
        case core_state_req_update_ts:
            {
            operation_state_m = core_state_req_enable_userdata;

            DEBUG( "core_operation_roam_c::next_state() - allowing user data" );
            
            drivers_m->enable_user_data(
                request_id_m );

            break;
            }
        case core_state_req_enable_userdata:
            {                        
            DEBUG( "core_operation_roam_c::next_state() - connection done" );

            server_m->get_connection_data()->set_ssid( ssid_m );
            server_m->get_core_settings().roam_metrics().set_roam_ts_userdata_enabled();
            server_m->get_core_settings().roam_metrics().inc_roam_success_count();

            server_m->get_connection_data()->clear_ap_association_failure_count(
                current_ap_m->bssid() );
            
            server_m->get_connection_data()->clear_all_authentication_failure_counts();

            server_m->get_connection_data()->remove_temporary_blacklist_entries(
                core_ap_blacklist_reason_eapol_failure |
                core_ap_blacklist_reason_max_association_failure_count );

            if ( current_ap_m->bssid() != server_m->get_connection_data()->last_bssid() )
                {
                DEBUG( "core_operation_roam_c::next_state() - BSSID changed, notifying change" );

                const core_mac_address_s bssid = current_ap_m->bssid();

                adaptation_m->notify(
                    core_notification_bssid_changed,
                    sizeof ( bssid ),
                    &bssid.addr[0] );
                    
                server_m->get_connection_data()->set_last_bssid(
                    bssid );
                }

            if ( !is_connected_m )
                {
                core_connection_state_e state = server_m->get_connection_data()->connection_state();
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
                }

            if ( server_m->get_connection_data()->last_rcp_class() != core_rcp_normal )
                {
                DEBUG( "core_operation_roam_c::next_state() - sending a notification about the normal signal level" );

                u8_t buf[5];
                buf[0] = static_cast<u8_t>( core_rcp_normal );
                buf[1] = current_ap_m->rcpi();

                adaptation_m->notify(
                    core_notification_rcp_changed,
                    sizeof( buf ),
                    buf );
                server_m->get_connection_data()->set_last_rcp_class( core_rcp_normal );
                }

            if ( current_ap_m->is_wpx() )
                {
                server_m->get_wpx_adaptation_instance().handle_wpx_roam_success(
                    *current_ap_m );
                }

            if( current_ap_m->rrm_capabilities() & RRM_CAPABILITY_BIT_MASK_NEIGHBOR_REPORT )
                {
                handle_neighbor_request();
                }
            
            /**
             * If roaming to another AP, schedule a power save update.
             *
             * This is not done on first connect because we do not want to disturb
             * DHCP negotiation.
             */
            if ( server_m->get_connection_data()->iap_data().operating_mode() == core_operating_mode_infrastructure &&
                 server_m->get_connection_data()->previous_ap_data() )
                {
                DEBUG( "core_operation_roam_c::next_state() - scheduling a power save update" );

                core_operation_base_c* operation = new core_operation_update_power_mode_c(
                    REQUEST_ID_CORE_INTERNAL,
                    server_m,
                    drivers_m,
                    adaptation_m );

                server_m->queue_int_operation( operation );
                }

            return core_error_ok;
            }
        case core_state_fail_connect:
            {
            DEBUG( "core_operation_roam_c::next_state() - connection failed" );

            DEBUG1( "core_operation_roam_c::next_state() - failure_reason_m is %u",
                failure_reason_m );
            DEBUG1( "core_operation_roam_c::next_state() - management_status_m is %u",
                management_status_m );
            DEBUG1( "core_operation_roam_c::next_state() - is_cached_sa_used_m is %u",
                is_cached_sa_used_m );

            core_connect_status_e status = connect_status(
                failure_reason_m,
                management_status_m );

            DEBUG1( "core_operation_roam_c::next_state() - connect_status is %u",
                status );

            server_m->get_connection_data()->set_connect_status( status );

            const core_mac_address_s bssid =
                current_ap_m->bssid();

            DEBUG( "core_operation_roam_c::next_state() - removing current AP entries from scan list" );
            server_m->get_scan_list().remove_entries_by_bssid(
                bssid );

            update_roam_failure_count(
                failure_reason_m,
                management_status_m );

            core_ap_blacklist_reason_e reason =
                is_fatal_failure(
                    failure_reason_m,
                    management_status_m,
                    is_cached_sa_used_m );
            if( reason != core_ap_blacklist_reason_none )
                {
                if ( reason == core_ap_blacklist_reason_eapol_failure )
                    {     
                    u8_t count_af( server_m->get_connection_data()->ap_authentication_failure_count( bssid ) );
                
                    u32_t max_ap_authentication_failure_count(
                     server_m->get_device_settings().max_ap_authentication_failure_count );
                
                    DEBUG1( "core_operation_roam_c::next_state() - this AP has failed %u time(s) in authentication earlier",
                        count_af );
                
                    if ( count_af >=  max_ap_authentication_failure_count )
                        {
                        DEBUG1( "core_operation_roam_c::next_state() - authentication failure count (%u), blacklisting the AP", count_af );
                        server_m->get_connection_data()->add_mac_to_temporary_blacklist(
                            bssid, reason );
                        }
                    else
                        {
                        DEBUG( "core_operation_roam_c::next_state() - increasing authentication failure count" );
                        server_m->get_connection_data()->increase_ap_authentication_failure_count( bssid );
                        }
                    }
                else
                    {
                    DEBUG( "core_operation_roam_c::next_state() - fatal failure, blacklisting the AP");
                    server_m->get_connection_data()->add_mac_to_temporary_blacklist(
                        bssid, reason );
                    }
                }
            else
                {
                u8_t count( server_m->get_connection_data()->ap_association_failure_count( bssid ) );
                DEBUG1( "core_operation_roam_c::next_state() - this AP has failed %u time(s) earlier",
                    count );
                
                u32_t max_ap_association_failure_count(
                    server_m->get_device_settings().max_ap_association_failure_count );
                if( server_m->get_connection_data()->iap_data().is_wapi_used() )
                    {
                    max_ap_association_failure_count = CORE_MAX_WAPI_AP_ASSOCIATION_FAILURE_COUNT;
                    }
                
                if( count >= max_ap_association_failure_count - 1 )
                    {
                    DEBUG1( "core_operation_roam_c::next_state() - failure count (%u) exceeded, blacklisting the AP",
                        max_ap_association_failure_count );
                    server_m->get_connection_data()->add_mac_to_temporary_blacklist(
                        bssid, core_ap_blacklist_reason_max_association_failure_count );
                    }
                else
                    {
                    DEBUG( "core_operation_roam_c::next_state() - increasing AP failure count" );
                    server_m->get_connection_data()->increase_ap_association_failure_count( bssid );
                    }
                }

            DEBUG1( "core_operation_roam_c::next_state() - setting tx power (%u)",
                server_m->get_device_settings().tx_power_level );

            operation_state_m = core_state_fail_set_tx_power;
            
            drivers_m->set_tx_power_level(
                request_id_m,
                server_m->get_device_settings().tx_power_level );

            break;
            }
        case core_state_fail_set_tx_power:
            {
            u32_t tx_level = server_m->get_device_settings().tx_power_level;
            if ( server_m->get_connection_data()->last_tx_level() != tx_level )
                {                
                DEBUG( "core_operation_roam_c::next_state() - TX level has changed, notifying change" );

                adaptation_m->notify(
                    core_notification_tx_power_level_changed,
                    sizeof( tx_level ),
                    reinterpret_cast<u8_t*>(&tx_level) );

                server_m->get_connection_data()->set_last_tx_level( tx_level );
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
core_error_e core_operation_roam_c::cancel()
    {
    DEBUG( "core_operation_roam_c::cancel() " );    
    
    switch ( operation_state_m )
        {
        case core_state_req_connect:
            {
            operation_state_m = core_state_fail_connect;

            return next_state();        
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
bool_t core_operation_roam_c::is_security_association_available(
    const core_ap_data_c& ap_data )
    {
    DEBUG( "core_operation_roam_c::is_security_association_available() " );
    
    core_type_list_c<network_id_c> network_id_list;

    /*const*/ core_mac_address_s bssid( ap_data.bssid() );
    network_id_c * network_id = new network_id_c(
        &bssid.addr[0],
        MAC_ADDR_LEN,
        &server_m->own_mac_addr().addr[0],
        MAC_ADDR_LEN,
        server_m->get_eapol_instance().ethernet_type() );
    if ( !network_id )
        {
        DEBUG( "core_operation_roam_c::is_security_association_available() - unable to create network_id_c" );
        return false_t;
        }
    
    network_id_list.append( network_id );

    wlan_eapol_if_eapol_key_authentication_type_e auth_type = core_tools_c::eap_authentication_type(
        server_m->get_connection_data()->iap_data(),
        ap_data );
    
    core_error_e ret = server_m->get_eapol_instance().check_pmksa_cache(
        network_id_list,
        auth_type,
        core_tools_c::eapol_cipher( ap_data.best_pairwise_cipher() ),
        core_tools_c::eapol_cipher( ap_data.best_group_cipher() ) );
    if ( ret != core_error_ok )
        {
        DEBUG1( "core_operation_roam_c::is_security_association_available() - check_pmksa_cache returned %i", ret );
        if ( ret == core_error_request_pending )
            {
            DEBUG( "core_operation_roam_c::is_security_association_available() - check_pmksa_cache returns core_error_request_pending! Implement complete_check_pmksa_cache for asynchronous operation! " );
            }
        
        return false_t;
        }
    
    // Ok, because ret == core_error_ok, we know that complete_check_pmksa_cache is already called from EAPOL.
    // If ret == core_error_request_pending, we should wait until complete_check_pmksa_cache.
    core_type_list_c< core_mac_address_s > & pmksa_list = server_m->get_eapol_instance().get_completed_check_pmksa_cache_list();
    
    DEBUG1( "core_operation_roam_c::is_security_association_available() - pmksa_list.count() = %i", pmksa_list.count() );

    if ( !pmksa_list.count() )
        {
        pmksa_list.clear();
        return false_t;
        }

    core_mac_address_s* list_obj = pmksa_list.first();
    if ( !list_obj )
        {
        pmksa_list.clear();
        return false_t;
        }

    if ( *list_obj != bssid )
        {
        pmksa_list.clear();
        return false_t;
        }

    pmksa_list.clear();

    return true_t;        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_connect_status_e core_operation_roam_c::connect_status(
    core_error_e request_status,
    core_management_status_e management_status )
    {
    DEBUG( "core_operation_roam_c::connect_status()" );

    core_iap_data_c& iap_data( server_m->get_connection_data()->iap_data() );

    if ( request_status == core_error_eapol_auth_start_timeout )
        {
        DEBUG( "core_operation_roam_c::connect_status() - EAPOL authentication timeout before authentication was started" );
        return core_connect_eapol_auth_start_timeout;
        }

    if ( request_status == core_error_unsupported_config )
        {
        DEBUG( "core_operation_roam_c::connect_status() - AP has an unsupported configuration" );
        return core_connect_ap_unsupported_configuration;
        }

    switch( iap_data.security_mode() )
        {
        case core_security_mode_wep:
            {
            if ( management_status == core_management_status_auth_algo_not_supported )
                {
                if ( iap_data.authentication_mode() == core_authentication_mode_open )
                    {
                    DEBUG( "core_operation_roam_c::connect_status() - open authentication unsupported (WEP)" );
                    return core_connect_wep_open_authentication_unsupported;
                    }
                else
                    {
                    DEBUG( "core_operation_roam_c::connect_status() - shared authentication unsupported (WEP)" );
                    return core_connect_wep_shared_authentication_unsupported;
                    }
                }
            else if ( iap_data.authentication_mode() == core_authentication_mode_shared &&
                      ( management_status == core_management_status_auth_challenge_failure ||
                        management_status == core_management_status_auth_frame_out_of_sequence ||
                        management_status == core_management_status_unspecified_failure ||
                        request_status == core_error_timeout ) )
                {
                DEBUG( "core_operation_roam_c::connect_status() - shared authentication failed (WEP)" );
                return core_connect_wep_shared_authentication_failed;
                }
            else if ( management_status == core_management_status_unsupported_capabilities &&
                      !current_ap_m->is_privacy_enabled() )
                {
                DEBUG( "core_operation_roam_c::connect_status() - association using privacy failed (WEP)" );
                return core_connect_iap_wep_but_ap_has_no_privacy;
                }
                
            break;
            }
        case core_security_mode_allow_unsecure:
            {
            break;
            }   
        case core_security_mode_802dot1x_unencrypted:
            /** Falls through on purpose. */
        case core_security_mode_802dot1x:
            {
            if ( request_status == core_error_eapol_total_failure ||
                 request_status == core_error_eapol_failure )
                {
                DEBUG( "core_operation_roam_c::connect_status() - 802.1x EAP failure" );
                return eap_connect_status(
                    iap_data.security_mode(),
                    server_m->get_connection_data()->last_failed_eap_type(),
                    server_m->get_connection_data()->last_eap_error() );
                }
            else if ( management_status == core_management_status_auth_algo_not_supported )
                {
                DEBUG( "core_operation_roam_c::connect_status() - authentication algorithm not supported (802.1x)" );
                return core_connect_802_1x_authentication_algorithm_not_supported;
                }

            break;
            }
        case core_security_mode_wpa:
            /** Falls through on purpose. */
        case core_security_mode_wpa2only:            
            {
            if ( request_status == core_error_eapol_total_failure ||
                 request_status == core_error_eapol_failure )
                {
                if ( iap_data.is_psk_used() )
                    {
                    DEBUG( "core_operation_roam_c::connect_status() - WPA-PSK failure" );
                    return core_connect_wpa_psk_failure;
                    }
                else
                    {
                    DEBUG( "core_operation_roam_c::connect_status() - WPA EAP failure" );
                    return eap_connect_status(
                        iap_data.security_mode(),
                        server_m->get_connection_data()->last_failed_eap_type(),
                        server_m->get_connection_data()->last_eap_error() );
                    }
                }

            break;        
            }
        case core_security_mode_wapi:            
            {
            if ( request_status == core_error_eapol_total_failure ||
                 request_status == core_error_eapol_failure )
                {
                if ( iap_data.is_psk_used() )
                    {
                    DEBUG( "core_operation_roam_c::connect_status() - WAPI-PSK failure" );
                    return core_connect_wapi_psk_failure;
                    }
                else
                    {
                    DEBUG( "core_operation_roam_c::connect_status() - WAPI certificate failure" );
                    return core_connect_wapi_certificate_failure;
                   }
                }

            break;        
            }

        default:
            {
            ASSERT( false_t );
            }
        }

    if ( management_status == core_management_status_assoc_denied_full_ap )
        {
        DEBUG( "core_operation_roam_c::connect_status() - AP full" );
        return core_connect_ap_full;
        }

    if ( management_status == core_management_status_unsupported_capabilities ||
         management_status == core_management_status_assoc_unsup_basic_rates ||
         management_status == core_management_status_assoc_unsup_ht_features )
        {
        DEBUG( "core_operation_roam_c::connect_status() - AP has an unsupported configuration" );
        return core_connect_ap_unsupported_configuration;
        }

    return core_connect_undefined;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ap_blacklist_reason_e core_operation_roam_c::is_fatal_failure(
    core_error_e request_status,
    core_management_status_e management_status,
    bool_t is_cached_sa_used ) const
    {
    DEBUG( "core_operation_roam_c::is_fatal_failure()" );
    
    /**
     * EAPOL failures in 802.1x/WPA mode are all fatal but blacklisting
     * is only done with core_error_eapol_failure because others cause
     * the connection to be shutdown immediately.
     */
    if( request_status == core_error_eapol_failure &&
        server_m->get_connection_data()->iap_data().is_eap_used() )
        {
        DEBUG( "core_operation_roam_c::is_fatal_failure() - fatal, EAP failure" );
        return core_ap_blacklist_reason_eapol_failure;
        }

    /**
     * Unsupported configuration is always fatal.
     */
    if( request_status == core_error_unsupported_config )
        {
        DEBUG( "core_operation_roam_c::is_fatal_failure() - fatal, unsupported configuration" );
        return core_ap_blacklist_reason_association_status;
        }

    /**
     * Don't consider a failed (re-)association attempt as fatal since it's
     * possible AP just doesn't have our authentication cached any more.
     */
    if ( is_cached_sa_used )
        {
        DEBUG( "core_operation_roam_c::is_fatal_failure() - not fatal, a reassociation attempt" );
        return core_ap_blacklist_reason_none;
        }

    /**
     * Non-fatal WPX management status codes should not cause blacklisting.
     */
    if ( server_m->get_wpx_adaptation_instance().is_wpx_management_status( management_status ) &&
         !server_m->get_wpx_adaptation_instance().is_fatal_wpx_management_status( management_status ) )
        {
        DEBUG( "core_operation_roam_c::is_fatal_failure() - not fatal, WPX-specific management status" );
        return core_ap_blacklist_reason_none;
        }

    if ( management_status != core_management_status_success &&
         management_status != core_management_status_assoc_denied_full_ap )
        {
        DEBUG1( "core_operation_roam_c::is_fatal_failure() - fatal, management status %u",
            management_status );
        return core_ap_blacklist_reason_association_status;
        }

    DEBUG( "core_operation_roam_c::is_fatal_failure() - not fatal" );
    return core_ap_blacklist_reason_none;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_connect_status_e core_operation_roam_c::eap_connect_status(
    core_security_mode_e security_mode,
    u32_t eap_type,
    u32_t eap_error ) const
    {
    DEBUG( "core_operation_roam_c::eap_connect_status()" );
    DEBUG1( "core_operation_roam_c::eap_connect_status() - eap_type %u",
        eap_type );
    DEBUG1( "core_operation_roam_c::eap_connect_status() - eap_error %u",
        eap_error );
 
    core_connect_status_e status( core_connect_wpa_eap_failure );
    if ( security_mode == core_security_mode_802dot1x ||
         security_mode == core_security_mode_802dot1x_unencrypted )
        {
        status = core_connect_802_1x_failure;
        }
 
    /** 
     * Handle EAP type specific errors.
     */
    switch ( eap_type )
        {
        case wlan_eapol_if_eap_type_tls:
            {
            switch ( eap_error )
                {
                case wlan_eapol_if_eap_status_certificate_expired:
                case wlan_eapol_if_eap_status_certificate_revoked:
                    status = core_connect_eap_tls_server_certificate_expired;
                    break;
                case wlan_eapol_if_eap_status_unsupported_certificate:
                    status = core_connect_eap_tls_server_certificate_unknown;
                    break;
                case wlan_eapol_if_eap_status_user_certificate_unknown:
                    status = core_connect_eap_tls_user_certificate_unknown;
                    break;
                case wlan_eapol_if_eap_status_illegal_cipher_suite:
                    status = core_connect_eap_tls_illegal_cipher_suite;
                    break;
                case wlan_eapol_if_eap_status_bad_certificate:
                    status = core_connect_eap_tls_user_rejected;
                    break;
                default:
                    status = core_connect_eap_tls_failure;
                }             
            break;
            }
        case wlan_eapol_if_eap_type_leap:
            {
            status = core_connect_eap_leap_failure;
            break;
            }
        case wlan_eapol_if_eap_type_gsmsim:
            {
            switch ( eap_error )
                {
                case wlan_eapol_if_eap_status_identity_query_failed:
                    status = core_connect_eap_sim_identity_query_failed;
                    break;
                case wlan_eapol_if_eap_status_user_has_not_subscribed_to_the_requested_service:
                    status = core_connect_eap_sim_user_has_not_subscribed_to_the_requested_service;
                    break;
                case wlan_eapol_if_eap_status_users_calls_are_barred:
                    status = core_connect_eap_sim_users_calls_are_barred;
                    break;
                default:
                    status = core_connect_eap_sim_failure;
                }            
            break;
            }
        case wlan_eapol_if_eap_type_ttls:
            {
            switch ( eap_error )
                {
                case wlan_eapol_if_eap_status_certificate_expired:
                case wlan_eapol_if_eap_status_certificate_revoked:
                    status = core_connect_eap_ttls_server_certificate_expired;
                    break;
                case wlan_eapol_if_eap_status_unsupported_certificate:
                    status = core_connect_eap_ttls_server_certificate_unknown;
                    break;
                case wlan_eapol_if_eap_status_user_certificate_unknown:
                    status = core_connect_eap_ttls_user_certificate_unknown;
                    break;
                case wlan_eapol_if_eap_status_illegal_cipher_suite:
                    status = core_connect_eap_ttls_illegal_cipher_suite;
                    break;
                case wlan_eapol_if_eap_status_bad_certificate:
                    status = core_connect_eap_ttls_user_rejected;
                    break;
                default:
                    status = core_connect_eap_ttls_failure;
                }             
            break;
            }
        case wlan_eapol_if_eap_type_aka:
            {
            switch ( eap_error )
                {
                case wlan_eapol_if_eap_status_identity_query_failed:
                    status = core_connect_eap_sim_identity_query_failed;
                    break;
                case wlan_eapol_if_eap_status_user_has_not_subscribed_to_the_requested_service:
                    status = core_connect_eap_sim_user_has_not_subscribed_to_the_requested_service;
                    break;
                case wlan_eapol_if_eap_status_users_calls_are_barred:
                    status = core_connect_eap_sim_users_calls_are_barred;
                    break;
                default:
                    status = core_connect_eap_aka_failure;
                }
            break;
            }
        case wlan_eapol_if_eap_type_peap:
            {
            switch ( eap_error )
                {
                case wlan_eapol_if_eap_status_certificate_expired:
                case wlan_eapol_if_eap_status_certificate_revoked:
                    status = core_connect_eap_peap_server_certificate_expired;
                    break;
                case wlan_eapol_if_eap_status_unsupported_certificate:
                    status = core_connect_eap_peap_server_certificate_unknown;
                    break;
                case wlan_eapol_if_eap_status_user_certificate_unknown:
                    status = core_connect_eap_peap_user_certificate_unknown;
                    break;
                case wlan_eapol_if_eap_status_illegal_cipher_suite:
                    status = core_connect_eap_peap_illegal_cipher_suite;
                    break;
                case wlan_eapol_if_eap_status_bad_certificate:
                    status = core_connect_eap_peap_user_rejected;
                    break;
                default:
                    status = core_connect_eap_peap_failure;
                }             
            break;
            }
        case wlan_eapol_if_eap_type_mschapv2:
            {
            switch ( eap_error )
                {
                case wlan_eapol_if_eap_status_password_expired:
                    status = core_connect_eap_mschapv2_password_expired;
                    break;
                case wlan_eapol_if_eap_status_no_dialin_permission:
                    status = core_connect_eap_mschapv2_no_dialin_permission;
                    break;
                case wlan_eapol_if_eap_status_account_disabled:
                    status = core_connect_eap_mschapv2_account_disabled;
                    break;
                case wlan_eapol_if_eap_status_restricted_logon_hours:
                    status = core_connect_eap_mschapv2_restricted_logon_hours;
                    break;                    
                default:
                    status = core_connect_eap_mschapv2_failure;
                }
            break;
            }
        case wlan_eapol_if_eap_type_fast:
            {
            switch ( eap_error )
                {
                case wlan_eapol_if_eap_status_tunnel_compromise_error:
                    status = core_connect_eap_fast_tunnel_compromise_error;
                    break;
                case wlan_eapol_if_eap_status_unexpected_tlv_exhanged:
                    status = core_connect_eap_fast_unexpected_tlv_exhanged;
                    break;
                case wlan_eapol_if_eap_status_no_pac_nor_certs_to_authenticate_with_provision_disabled:
                    status = core_connect_eap_fast_no_pac_nor_certs_to_authenticate_with_provision_disabled;
                    break;
                case wlan_eapol_if_eap_status_no_matching_pac_for_aid:
                    status = core_connect_eap_fast_no_matching_pac_for_aid;
                    break;
                case wlan_eapol_if_eap_status_pac_store_corrupted:
                    status = core_connect_eap_fast_pac_store_corrupted;
                    break;
                case wlan_eapol_if_eap_status_authentication_failure: /* flow through on purpose */
                default:
                    status = core_connect_eap_fast_authentication_failed;
                }
            break;
            }
        default:
            break;
        }     

    return status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
bool_t core_operation_roam_c::process_frame(
    core_ap_data_c& ap_data )
    {
    connect_status_m = core_tools_parser_c::is_ap_suitable(
        server_m->get_wpx_adaptation_instance(),
        ap_data,
        server_m->get_connection_data()->iap_data(),
        server_m->get_core_settings(),
        *server_m->get_connection_data(),
        min_required_rcpi_m,
        min_medium_time_m,
        server_m->is_cm_active(),
        current_bssid_m );

    if( connect_status_m == core_connect_ok )
        {
        roaming_list_entry_s* entry = new roaming_list_entry_s;
        if( !entry )
            {
            DEBUG( "core_operation_roam_c::process_frame() - unable to roaming_list_entry_s instance()" );

            return false_t;                        
            }

        entry->ap_data = NULL;
        entry->is_cached_sa_available = false_t;

        entry->ap_data = core_ap_data_c::instance( ap_data );
        if( !entry->ap_data )
            {
            DEBUG( "core_operation_roam_c::process_frame() - unable to create core_ap_data_c instance()" );

            delete entry;
            entry = NULL;
            
            return false_t;
            }

        u32_t entry_score( entry->ap_data->rcpi() );

        /**
         * If this is a roaming situation, APs supporting WPX fast-roam or with
         * cached PMKSAs are preferred over others.
         */
        if( server_m->get_connection_data()->current_ap_data() &&
            server_m->get_connection_data()->iap_data().is_802dot1x_used() &&
            entry->ap_data->is_wpx_fast_roam_available() &&
            is_security_association_available( *server_m->get_connection_data()->current_ap_data() ) )
            {
            DEBUG( "core_operation_roam_c::process_frame() - WPX fast-roam supported" );

            entry->is_cached_sa_available = true_t;
            entry_score += CORE_ROAMING_LIST_BONUS_WPX_FAST_ROAM;
            }
        else if( server_m->get_connection_data()->iap_data().is_eap_used() &&
                 is_security_association_available( *entry->ap_data ) )
            {
            DEBUG( "core_operation_roam_c::process_frame() - a cached PMKSA found" );
                
            entry->is_cached_sa_available = true_t;
            entry_score += CORE_ROAMING_LIST_BONUS_PMKSA;
            }
            
        // These conditions could be added to previous conditions...
        if( server_m->get_connection_data()->current_ap_data() ) // previous connection exists
            {
            if( !server_m->get_connection_data()->current_ap_data()->is_wpx_fast_roam_available() &&  // previous connection was not WPX
                entry->ap_data->is_wpx_fast_roam_available() ) // AND new connection is WPX
                {
                DEBUG( "core_operation_roam_c::process_frame() - reassociation is not possible, previous connection was not WPX, but new connection is WPX" );
                entry->is_cached_sa_available = false_t;
                }
            }
        else // previous connection does not exists
            {
            if( entry->ap_data->is_wpx_fast_roam_available() ) // new connection has WPX
                {
                DEBUG( "core_operation_roam_c::process_frame() - reassociation is not possible, there is no previous connection and new is WPX" );
                entry->is_cached_sa_available = false_t;
                }
            }
                
        /**
         * If AP advertises QBSS Load IE, check the amount of connected
         * stations and adjust the score accordingly.
         */
        const core_frame_qbss_load_ie_c* qbss_load_ie = entry->ap_data->qbss_load_ie();
        if( qbss_load_ie )
            {
            DEBUG1( "core_ap_data_c::process_frame() - QBSS Load IE detected, %u station(s) associated",
                qbss_load_ie->station_count() );

            delete qbss_load_ie;
            }

        roaming_list_m.append(
            entry,
            entry_score );
                
        return true_t;            
        }

    DEBUG1( "core_operation_roam_c::process_frame() - AP is unsuitable, reason %u",
        connect_status_m );

    return false_t;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_roam_c::update_roam_failure_count(
    core_error_e request_status,
    core_management_status_e management_status )
    {
    core_roam_failed_reason_e failure_reason( core_roam_failed_reason_none );

    if ( request_status == core_error_eapol_failure ||
         request_status == core_error_eapol_total_failure )
        {
        failure_reason = core_roam_failed_reason_eapol_failure;
        }
    else if ( request_status == core_error_timeout )
        {
        failure_reason = core_roam_failed_reason_timeout;
        }
    else if ( management_status != core_management_status_success )    
        {
        failure_reason = core_roam_failed_reason_ap_status_code;
        }
    else
        {
        failure_reason = core_roam_failed_reason_other_failure;
        }

    server_m->get_core_settings().roam_metrics().inc_roam_attempt_failed_count(
        failure_reason );
    if ( server_m->get_connection_data()->last_roam_failed_reason() == core_roam_failed_reason_none ||
         server_m->get_connection_data()->last_roam_failed_reason() == core_roam_failed_reason_no_suitable_ap )
        {
        server_m->get_connection_data()->set_last_roam_failed_reason( failure_reason );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_roam_c::handle_neighbor_request()
    {
    /* neighbor report request */
    DEBUG( "core_operation_roam_c::handle_neighbor_request() - Neighbor report bit is on" );
    
    core_frame_action_c* request = NULL;

    // create IE
    core_frame_nr_ie_c* nr_request = core_frame_nr_ie_c::instance( ssid_m );
    if ( nr_request )
        {
        // create frame, frame ownership will belong to caller of this method
        request = core_frame_action_nr_c::instance(
                                            0,                          // Duration
                                            current_ap_m->bssid(),      // Destination 
                                            server_m->own_mac_addr(),   // Source
                                            current_ap_m->bssid(),      // BSSID
                                            0,                          // Sequence Control
                                            core_frame_radio_measurement_action_c::core_dot11_action_rm_type_neighbor_req, // Action Type
                                            0,                          // Dialog Token
                                            nr_request );

        if ( !request )
            {
            DEBUG( "core_operation_roam_c::handle_neighbor_request() - Unable to instantiate core_frame_action_nr_c" );
            }
        
        delete nr_request;
        nr_request = NULL;
        }
    else
        {
        DEBUG( "core_operation_roam_c::handle_neighbor_request() - Unable to instantiate core_frame_nr_ie_c" );
        }

    if ( request )
        {
        DEBUG1( "core_operation_roam_c::handle_neighbor_request() - sending frame (%u bytes): ",
            request->data_length() );
        DEBUG_BUFFER(
            request->data_length(),
            request->data() );
        
        server_m->send_management_frame(
            core_frame_type_dot11,
            request->data_length(),
            request->data(),
            current_ap_m->bssid() );

        delete request;
        request = NULL;
        }
    }
