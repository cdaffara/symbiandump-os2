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
* Description:  State machine for connecting to a WPA network
*
*/

/*
* %version: 62 %
*/

#include "core_sub_operation_wpa_connect.h"
#include "core_sub_operation_connect.h"
#include "core_server.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "core_ap_data.h"
#include "core_frame_rsn_ie.h"
#include "core_frame_wpa_ie.h"
#include "core_frame_wapi_ie.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_wpa_connect_c::core_sub_operation_wpa_connect_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    bool_t& is_connected,
    core_management_status_e& connect_status,
    const core_ssid_s& ssid,
    core_ap_data_c& ap_data,
    bool_t& is_cached_sa_used,
    core_type_list_c<core_frame_dot11_ie_c>& assoc_ie_list,
    core_frame_assoc_resp_c** assoc_resp ) :
    core_operation_base_c( core_operation_unspecified, request_id, server, drivers, adaptation,
        core_base_flag_drivers_needed ),
    abs_wlan_eapol_callback_interface_c(),
    is_connected_m( is_connected ),
    is_cached_sa_used_m( is_cached_sa_used ),
    connect_status_m( connect_status ),
    ssid_m( ssid ),
    ap_data_m( ap_data ),
    current_bssid_m( ZERO_MAC_ADDR ),
    pmkid_length_m( 0 ),
    sent_ie_m( NULL ),
    auth_algorithm_m( core_authentication_mode_open ),
    eapol_auth_type_m( wlan_eapol_if_eapol_key_authentication_type_none ),
    assoc_ie_list_m( assoc_ie_list ),
    assoc_resp_m( assoc_resp ),
    is_key_caching_used_m( false_t ),
    pairwise_key_m( CIPHER_KEY_NOT_DEFINED )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::core_sub_operation_wpa_connect_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_wpa_connect_c::~core_sub_operation_wpa_connect_c()
    {
    DEBUG( "core_sub_operation_wpa_connect_c::~core_sub_operation_wpa_connect_c()" );

    /**
     * Ownership of sent_ie_m has been transferred to assoc_ie_list_m,
     * no need delete it.
     */
    sent_ie_m = NULL;

    server_m->set_eapol_handler( NULL );
    server_m->unregister_event_handler( this );
    assoc_resp_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_wpa_connect_c::next_state()
    {
    DEBUG( "core_sub_operation_wpa_connect_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - marking is_eapol_authentication_started as true" );
            server_m->get_connection_data()->set_eapol_authentication_started(
                true_t );
            server_m->get_connection_data()->set_eapol_auth_bssid(
                ZERO_MAC_ADDR );
            server_m->get_connection_data()->set_eapol_auth_failure(
                core_error_ok );

            eapol_auth_type_m = core_tools_c::eap_authentication_type(
                server_m->get_connection_data()->iap_data(),
                ap_data_m );
            current_bssid_m = ap_data_m.bssid();

            server_m->set_eapol_handler( this );

            server_m->get_eapol_instance().clear_stored_frame();

            /**
             * If roaming to a WPA2-EAP AP without a cached PMKSA, attempt
             * to use proactive key caching if not previously done so.
             */
            if( !is_key_caching_used_m &&
                !is_cached_sa_used_m &&              
                server_m->get_connection_data()->current_ap_data() &&
                eapol_auth_type_m == wlan_eapol_if_eapol_key_authentication_type_rsna_eap )
                {
                DEBUG( "core_sub_operation_wpa_connect_c::next_state() - attempting to use proactive key caching" );

                is_cached_sa_used_m = true_t;
                is_key_caching_used_m = true_t;
                }
            else if( is_key_caching_used_m )
                {
                is_cached_sa_used_m = false_t;
                is_key_caching_used_m = false_t;                
                }

            // update_wlan_database_reference_values must be sent before start_authentication and start_reassociation.
            u32_t reference[2] = {
                3, // ELan
                server_m->get_connection_data()->iap_data().id() };
            core_error_e ret = server_m->get_eapol_instance().update_wlan_database_reference_values(
                reinterpret_cast<u8_t*>( reference ), 2*sizeof( u32_t ) );

            if( is_cached_sa_used_m )
                {
                if( eapol_auth_type_m == wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam )
                    {
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - starting a WPX fast-roam reassociation" );

                    core_error_e ret = server_m->get_wpx_adaptation_instance().handle_fast_roam_start_reassociation(
                        ap_data_m,
                        assoc_ie_list_m );
                    DEBUG1( "core_sub_operation_wpa_connect_c::next_state() - handle_fast_roam_start_reassociation returned with %u",
                        ret );
                    /*
                     *  State machine will move forward from here by:
                     * - complete_start_wpx_fast_roam_reassociation() 
                     *     -> core_state_do_connect
                     * - Error message (start_wpx_fast_roam_reassociation) 
                     *     -> core_state_init (with is_cached_sa_used_m = false_t)
                     */
                    }
                else
                    {
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - starting a reassociation" );
                    
                    operation_state_m = core_state_do_connect;
                    
                    core_mac_address_s previous_bssid( ZERO_MAC_ADDR );
                    if( server_m->get_connection_data()->current_ap_data() )
                        {
                        previous_bssid = server_m->get_connection_data()->current_ap_data()->bssid();
                        }

                    network_id_c previous_ap(
                        &previous_bssid.addr[0],
                        MAC_ADDR_LEN,
                        &server_m->own_mac_addr().addr[0],
                        MAC_ADDR_LEN,
                        server_m->get_eapol_instance().ethernet_type() );
    
                    network_id_c new_ap(
                        &current_bssid_m.addr[0],
                        MAC_ADDR_LEN,
                        &server_m->own_mac_addr().addr[0],
                        MAC_ADDR_LEN,
                        server_m->get_eapol_instance().ethernet_type() );
                    
                    ret = server_m->get_eapol_instance().start_reassociation(
                        &previous_ap, &new_ap, eapol_auth_type_m );
                    DEBUG1( "core_sub_operation_wpa_connect_c::next_state() - start_reassociation returned with %u",
                        ret );
                    /*
                     *  State machine will move forward from here by:
                     * - reassociate(), when reassociation is possible 
                     *     -> core_state_do_connect
                     * - Error message (start_reassociation), when start_reassociation is not possible 
                     *     -> core_state_start_authentication_needed
                     */
                    }
                }
            else
                {
                // Full authentication is needed.
                operation_state_m = core_state_start_authentication_needed;
                return next_state();
                }
            break;
            }
        case core_state_start_authentication_needed:
            {
            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - starting a full authentication" );

            operation_state_m = core_state_do_connect;

            network_id_c network_id(
                &current_bssid_m.addr[0],
                MAC_ADDR_LEN,
                &server_m->own_mac_addr().addr[0],
                MAC_ADDR_LEN,
                server_m->get_eapol_instance().ethernet_type() );

            core_error_e ret = server_m->get_eapol_instance().start_authentication(
                const_cast<u8_t*>( &ssid_m.ssid[0] ), ssid_m.length,
                eapol_auth_type_m,
                const_cast<u8_t*>( &server_m->get_connection_data()->iap_data().psk_key().key_data[0] ),
                server_m->get_connection_data()->iap_data().psk_key().key_length,
                server_m->get_connection_data()->iap_data().is_psk_overridden(),
                &network_id );
            DEBUG1( "core_sub_operation_wpa_connect_c::next_state() - start_authentication returned with %u",
                ret );
            
            break;
            }
        case core_state_do_connect:
            {
            core_key_management_e key_management( core_key_management_eap );
            core_encryption_mode_e encryption_mode( core_encryption_mode_wpa );
            bool_t is_pairwise_key_invalidated( true_t );
            
            switch ( eapol_auth_type_m )
                {
                case wlan_eapol_if_eapol_key_authentication_type_rsna_eap:
                    /** Falls through on purpose. */
                case wlan_eapol_if_eapol_key_authentication_type_wpa_eap:
                    {
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using key management core_key_management_eap" );
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using encryption mode core_encryption_mode_wpa" );
                    key_management = core_key_management_eap;
                    encryption_mode = core_encryption_mode_wpa;                    
                    break;
                    }
                case wlan_eapol_if_eapol_key_authentication_type_wfa_sc:
                    {
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using key management core_key_management_eap" );
                    key_management = core_key_management_eap;

                    if ( ap_data_m.is_privacy_enabled() )
                        {
                        DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using encryption mode core_encryption_mode_wpa" );
                        encryption_mode = core_encryption_mode_wpa;
                        }
                    else
                        {
                        DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using encryption mode core_encryption_mode_disabled" );
                        encryption_mode = core_encryption_mode_disabled;
                        }
                    break;
                    }
                case wlan_eapol_if_eapol_key_authentication_type_rsna_psk:
                    /** Falls through on purpose. */
                case wlan_eapol_if_eapol_key_authentication_type_wpa_psk:
                    {
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using key management core_key_management_preshared" );
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using encryption mode core_encryption_mode_wpa" );
                    key_management = core_key_management_preshared;
                    encryption_mode = core_encryption_mode_wpa;
                    break;
                    }
                case wlan_eapol_if_eapol_key_authentication_type_802_1x:
                    {
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using key management core_key_management_eap" );
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using encryption mode core_encryption_mode_802dot1x" );
                    key_management = core_key_management_eap;
                    encryption_mode = core_encryption_mode_802dot1x;
                    break;
                    }
                case wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam:
                    {
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using key management wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam" );
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using encryption mode core_encryption_mode_802dot1x" );
                    key_management = core_key_management_wpx_fast_roam;
                    encryption_mode = core_encryption_mode_802dot1x;
                    if( is_cached_sa_used_m )
                        {
                        is_pairwise_key_invalidated = false_t;
                        }
                    break;
                    }
                case wlan_eapol_if_eapol_key_authentication_type_wapi:
                    {
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using key management core_key_management_wapi_certificate" );
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using encryption mode core_security_mode_wapi" );
                    key_management = core_key_management_wapi_certificate;
                    encryption_mode = core_encryption_mode_wpi;
                    break;                    
                    }
                case wlan_eapol_if_eapol_key_authentication_type_wapi_psk:
                    {
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using key management core_key_management_wapi_psk" );
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using encryption mode core_security_mode_wapi" );
                    key_management = core_key_management_wapi_psk;
                    encryption_mode = core_encryption_mode_wpi;
                    break;                    
                    }
                case wlan_eapol_if_eapol_key_authentication_type_802_1x_unencrypted:
                    {
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using key management core_key_management_none" );
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - using encryption mode core_encryption_mode_disabled" );
                    key_management = core_key_management_none;
                    encryption_mode = core_encryption_mode_disabled;
                    break;                    
                    }
                default:
                    {
                    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - unknown authentication type" );
                    ASSERT( false_t );
                    }
                }
            
            operation_state_m = core_state_req_connect;

            if( ap_data_m.is_rsn_ie_present() )
                {
                DEBUG( "core_sub_operation_wpa_connect_c::next_state() - generating RSN IE" );

                sent_ie_m = core_frame_rsn_ie_c::instance(
                    server_m->get_wpx_adaptation_instance(),
                    ap_data_m.best_group_cipher(),
                    ap_data_m.best_pairwise_cipher(),
                    key_management,
                    pmkid_length_m,
                    &pmkid_data_m[0] );
                }
            else if( ap_data_m.is_wpa_ie_present() )
                {
                DEBUG( "core_sub_operation_wpa_connect_c::next_state() - generating WPA IE" );

                sent_ie_m = core_frame_wpa_ie_c::instance(
                    server_m->get_wpx_adaptation_instance(),
                    ap_data_m.best_group_cipher(),
                    ap_data_m.best_pairwise_cipher(),
                    key_management );
                }
            else if( ap_data_m.is_wapi_ie_present() )
                {
                DEBUG( "core_sub_operation_wpa_connect_c::next_state() - generating WAPI IE" );

                sent_ie_m = core_frame_wapi_ie_c::instance(
                    ap_data_m.best_group_cipher(),
                    ap_data_m.best_pairwise_cipher(),
                    key_management,
                    0,
                    pmkid_length_m,
                    &pmkid_data_m[0] );
                }
            
            if( sent_ie_m &&
                eapol_auth_type_m != wlan_eapol_if_eapol_key_authentication_type_wfa_sc )
                {
                assoc_ie_list_m.append(
                    sent_ie_m,
                    sent_ie_m->element_id() );
                }

            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - marking is_eapol_connecting as true" );
            server_m->get_connection_data()->set_eapol_connecting(
                true_t );

            core_operation_base_c* operation = new core_sub_operation_connect_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                is_connected_m,
                connect_status_m,
                ssid_m,
                ap_data_m,
                auth_algorithm_m,
                server_m->get_wpx_adaptation_instance().encryption_mode(
                    ap_data_m, encryption_mode ),
                core_tools_c::cipher_key_type(
                    ap_data_m.best_pairwise_cipher() ),
                assoc_ie_list_m,
                assoc_resp_m,
                is_pairwise_key_invalidated,
                true_t,
                pairwise_key_m.key_length ? &pairwise_key_m : NULL );

            return run_sub_operation( operation );
            }
        case core_state_req_connect:
            {
            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - connection success" );

            operation_state_m = core_state_req_state_notification;

            network_id_c network(
                &current_bssid_m.addr[0],
                MAC_ADDR_LEN,
                &server_m->own_mac_addr().addr[0],
                MAC_ADDR_LEN,
                server_m->get_eapol_instance().ethernet_type() );

            const core_frame_dot11_ie_c* ie = NULL;
            if( ap_data_m.is_rsn_ie_present() )
                {
                ie = ap_data_m.rsn_ie();
                }
            else if( ap_data_m.is_wpa_ie_present() )
                {
                ie = ap_data_m.wpa_ie();
                }
            else if( ap_data_m.is_wapi_ie_present() )
                {
                ie = ap_data_m.wapi_ie();
                }

            core_frame_dot11_ie_c * recv_ie = NULL;
            core_frame_dot11_ie_c * sent_ie = NULL;

            if ( sent_ie_m )
                {
                sent_ie = sent_ie_m;
                DEBUG( "core_sub_operation_wpa_connect_c::next_state() - sent IE: " );
                DEBUG_BUFFER( sent_ie_m->data_length(), sent_ie_m->data() );
                }
    
            if ( ie )
                {
                recv_ie = core_frame_dot11_ie_c::instance( ie->data_length(), ie->data() );

                DEBUG( "core_sub_operation_wpa_connect_c::next_state() - received IE: " );
                DEBUG_BUFFER( ie->data_length(), ie->data() );
                }

            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - marking is_eapol_connecting as false" );
            server_m->get_connection_data()->set_eapol_connecting(
                false_t );
            server_m->get_connection_data()->set_eapol_auth_bssid(
                current_bssid_m );

            /**
             * WPX fast-roam reassociation is handled differently, EAPOL indication will
             * move the state machine forward.
             */
            if ( is_cached_sa_used_m &&
                 eapol_auth_type_m == wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam )
                {
                server_m->get_wpx_adaptation_instance().handle_fast_roam_reassoc_resp(
                    assoc_resp_m ? *assoc_resp_m : NULL );

                delete recv_ie;
                recv_ie = NULL;
                delete ie;
                ie = NULL;
                return core_error_request_pending;
                }

            u8_t * temp_recv_ie( NULL );
            u32_t temp_recv_ie_length( 0 );
            u8_t * temp_sent_ie( NULL );
            u32_t temp_sent_ie_length( 0 );
            
            if ( recv_ie )
                {
                temp_recv_ie = const_cast<u8_t*>( recv_ie->data() );
                temp_recv_ie_length = recv_ie->data_length();
                }
            if ( sent_ie_m )
                {
                temp_sent_ie = const_cast<u8_t*>( sent_ie->data() );
                temp_sent_ie_length = sent_ie->data_length();
                }
                
            if ( is_cached_sa_used_m )                 
                {
                server_m->get_eapol_instance().complete_reassociation(
                    wlan_eapol_if_eapol_wlan_authentication_state_association_ok,
                    &network,
                    temp_recv_ie, temp_recv_ie_length,
                    temp_sent_ie, temp_sent_ie_length,
                    core_tools_c::eapol_cipher( ap_data_m.best_pairwise_cipher() ),
                    core_tools_c::eapol_cipher( ap_data_m.best_group_cipher() ) );
                }
            else
                {
                server_m->get_eapol_instance().complete_association(                
                    wlan_eapol_if_eapol_wlan_authentication_state_association_ok,
                    &network,
                    temp_recv_ie, temp_recv_ie_length,
                    temp_sent_ie, temp_sent_ie_length,
                    core_tools_c::eapol_cipher( ap_data_m.best_pairwise_cipher() ),
                    core_tools_c::eapol_cipher( ap_data_m.best_group_cipher() ) );
                }

            delete recv_ie;
            recv_ie = NULL;
            delete ie;
            ie = NULL;
            
            // Send EAPOL frame after timer. This will allow new EAPOL frames to come from queue.
            return asynch_goto( core_state_process_eapol_frame, CORE_TIMER_IMMEDIATELY );
            }
        case core_state_process_eapol_frame:
            {
            operation_state_m = core_state_req_state_notification;
            
            server_m->get_eapol_instance().process_stored_frame();
            server_m->register_event_handler( this );

            break;
            }
        case core_state_req_state_notification:
            {
            server_m->unregister_event_handler( this );

            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - authentication success" );

            return core_error_ok;
            }
        case core_state_req_connect_failed:
            {
            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - connection failed" );

            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - marking is_eapol_authentication_started as false" );
            server_m->get_connection_data()->set_eapol_authentication_started(
                false_t );
            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - marking is_eapol_connecting as false" );
            server_m->get_connection_data()->set_eapol_connecting(
                false_t );

            /** The connection attempt failed, we are no longer connected. */
            is_connected_m = false_t;

            /**
             * WPX fast-roam reassociation is handled differently, EAPOL indication will
             * move the state machine forward.
             * 
             * This completion should be before clearing eapol_handler. 
             * Otherwise we don't get error message.
             */
            if ( is_cached_sa_used_m &&
                 eapol_auth_type_m == wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam )
                {
                DEBUG( "core_sub_operation_wpa_connect_c::next_state() - calling EAPOL complete_fast_roam_reassociation (this_ap_failed)" );
                server_m->get_wpx_adaptation_instance().handle_fast_roam_reassoc_resp( NULL );

                return core_error_request_pending;
                }

            /**
             * We already know the connection has failed so don't process the failure
             * indication from EAPOL.
             */
            server_m->set_eapol_handler( NULL );

            wlan_eapol_if_eapol_wlan_authentication_state_e eapol_reason(
                wlan_eapol_if_eapol_wlan_authentication_state_this_ap_failed );

            if ( connect_status_m == core_management_status_auth_algo_not_supported )
                {
                eapol_reason = wlan_eapol_if_eapol_wlan_authentication_state_802_11_auth_algorithm_not_supported;
                }
            else if ( failure_reason_m == core_error_timeout )
                {
                eapol_reason = wlan_eapol_if_eapol_wlan_authentication_state_no_response;
                }

            network_id_c network(
                &current_bssid_m.addr[0],
                MAC_ADDR_LEN,
                &server_m->own_mac_addr().addr[0],
                MAC_ADDR_LEN,
                server_m->get_eapol_instance().ethernet_type() );

            if ( is_cached_sa_used_m )
                {
                DEBUG1( "core_sub_operation_wpa_connect_c::next_state() - calling EAPOL complete_reassociation with code %u",
                    eapol_reason );

                server_m->get_eapol_instance().complete_reassociation(
                    eapol_reason,
                    &network,
                    NULL, 0,
                    NULL, 0,
                    wlan_eapol_if_rsna_cipher_none,
                    wlan_eapol_if_rsna_cipher_none );
                }
            else
                {
                DEBUG1( "core_sub_operation_wpa_connect_c::next_state() - calling EAPOL complete_association with code %u",
                    eapol_reason );

                server_m->get_eapol_instance().complete_association(
                    eapol_reason,
                    &network,
                    NULL, 0,
                    NULL, 0,
                    wlan_eapol_if_rsna_cipher_none,
                    wlan_eapol_if_rsna_cipher_none );
                }

            return cancel();
            }
        case core_state_req_association_failed:
            {
            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - association, reassociation or WPX fast-roam reassociation failed" );
            
            server_m->set_eapol_handler( NULL );

            return cancel();
            }
        case core_state_bss_lost:
            {
            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - authentication failed due to BSS lost" );

            /** The connection attempt failed, we are no longer connected. */
            is_connected_m = false_t;
            
            return core_error_timeout;
            }
        case core_state_user_cancel:
            {
            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - pending request has been completed, proceeding with user cancel" );

            network_id_c network(
                &current_bssid_m.addr[0],
                MAC_ADDR_LEN,
                &server_m->own_mac_addr().addr[0],
                MAC_ADDR_LEN,
                server_m->get_eapol_instance().ethernet_type() );

            DEBUG6( "core_sub_operation_wpa_connect_c::next_state() - EAPOL disassociation from BSSID %02X:%02X:%02X:%02X:%02X:%02X",
                current_bssid_m.addr[0], current_bssid_m.addr[1], current_bssid_m.addr[2],
                current_bssid_m.addr[3], current_bssid_m.addr[4], current_bssid_m.addr[5] );
            DEBUG( "core_sub_operation_wpa_connect_c::next_state() - marking is_eapol_disconnecting as true" );
            server_m->get_connection_data()->set_eapol_disconnecting(
                true );

            server_m->get_eapol_instance().disassociation( &network );
            operation_state_m = core_state_user_cancel_disassociated;
            
            break;
            }
        case core_state_user_cancel_disassociated:
            {
            /** The connection attempt failed, we are no longer connected. */
            is_connected_m = false_t;            

            return core_error_cancel;
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
core_error_e core_sub_operation_wpa_connect_c::cancel()
    {
    DEBUG( "core_sub_operation_wpa_connect_c::cancel()" );

    switch( operation_state_m )
        {
        case core_state_req_connect:
            {
            return goto_state( core_state_req_connect_failed );
            }
        case core_state_req_state_notification:
            {
            if( server_m->get_connection_data()->is_eapol_require_immediate_reconnect() ||
                ( is_key_caching_used_m &&
                  failure_reason_m == core_error_eapol_auth_start_timeout ) )
                {
                DEBUG( "core_sub_operation_wpa_connect_c::cancel() - re-attempting authentication" );
                server_m->get_connection_data()->set_eapol_require_immediate_reconnect( false_t );
                assoc_ie_list_m.clear();

                return asynch_goto( core_state_init, 100000 ); // 100 ms delay before new connect
                }

            /** The connection attempt failed, we are no longer connected. */
            is_connected_m = false_t;            

            return failure_reason_m;
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
void core_sub_operation_wpa_connect_c::user_cancel(
    bool_t /* do_graceful_cancel */ )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::user_cancel()" );

    /**
     * Do not handle any EAPOL indications that might occur during our
     * transition to core_state_user_cancel state.
     */
    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - marking is_eapol_authenticating as false" );
    server_m->get_connection_data()->set_eapol_authenticating(
        false_t );
    DEBUG( "core_sub_operation_wpa_connect_c::next_state() - marking is_eapol_authentication_started as false" );
    server_m->get_connection_data()->set_eapol_authentication_started(
        false_t );
    
    /**
     * If we are waiting for an EAPOL indication, we'll have to schedule
     * our own timer to proceed.
     */
    if( operation_state_m == core_state_req_state_notification )
        {
        asynch_goto( core_state_user_cancel, CORE_TIMER_IMMEDIATELY );

        return;
        }

    /**
     * Otherwise we'll just wait for the pending request complete
     * before continuing.
     */    
    operation_state_m = core_state_user_cancel;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_wpa_connect_c::packet_send(
    network_id_c * send_network_id,
    u8_t * packet_data,
    u32_t packet_data_length,
    bool_t send_unencrypted )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::packet_send()" );    

    server_m->send_data_frame(
        ap_data_m,
        core_frame_type_ethernet,
        static_cast<u16_t>( packet_data_length ),
        packet_data,
        core_access_class_voice,
        send_network_id->destination(),
        send_unencrypted );

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_wpa_connect_c::associate(
    wlan_eapol_if_eapol_key_authentication_mode_e authentication_mode )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::associate()" );

    auth_algorithm_m = server_m->get_wpx_adaptation_instance().authentication_algorithm(
        eapol_auth_type_m,
        authentication_mode );

    next_state();

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_wpa_connect_c::disassociate(
    network_id_c * receive_network_id,
    const bool_t /* self_disassociation */ )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::disassociate()" );

    const core_mac_address_s bssid(
        receive_network_id->source() );
    DEBUG6( "core_sub_operation_wpa_connect_c::disassociate() - function BSSID is %02X:%02X:%02X:%02X:%02X:%02X",
        bssid.addr[0], bssid.addr[1], bssid.addr[2], 
        bssid.addr[3], bssid.addr[4], bssid.addr[5] );
    DEBUG6( "core_sub_operation_wpa_connect_c::disassociate() - EAPOL disassociation from BSSID %02X:%02X:%02X:%02X:%02X:%02X",
        current_bssid_m.addr[0], current_bssid_m.addr[1], current_bssid_m.addr[2],
        current_bssid_m.addr[3], current_bssid_m.addr[4], current_bssid_m.addr[5] );
    if( operation_state_m == core_state_req_state_notification &&
        bssid == current_bssid_m )
        {
        DEBUG( "core_sub_operation_wpa_connect_c::disassociate() - marking is_eapol_disconnecting as true" );
        server_m->get_connection_data()->set_eapol_disconnecting(
            true );

        server_m->get_eapol_instance().disassociation( receive_network_id );
        }
    
    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_wpa_connect_c::packet_data_session_key(
    network_id_c * send_network_id,
    session_key_c * key )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::packet_data_session_key()" );
    ASSERT ( key != NULL );
    ASSERT ( send_network_id != NULL );

    core_mac_address_s mac( BROADCAST_MAC_ADDR );

    if ( key->eapol_key_type == wlan_eapol_if_eapol_key_type_unicast )
        {
        mac = send_network_id->destination();
        }

    core_cipher_key_type_e type =
        core_tools_c::cipher_key_type(
            key->eapol_key_type,
            ap_data_m.best_pairwise_cipher(),
            ap_data_m.best_group_cipher() );

    /**
     * If a pairwise key is supplied before association, we cache it for
     * later use instead of immediately setting it.
     */
    if ( is_cached_sa_used_m &&
         eapol_auth_type_m == wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam &&
         key->eapol_key_type == wlan_eapol_if_eapol_key_type_unicast )
        {
        pairwise_key_m.key_length = static_cast<u16_t>( key->key_length );
        pairwise_key_m.key_index = static_cast<u8_t>( key->key_index );
        core_tools_c::copy(
            &pairwise_key_m.key_data[0],
            key->key,
            pairwise_key_m.key_length );
        }
    else
        {
        ASSERT( drivers_m );
        drivers_m->add_cipher_key(
            type,
            static_cast<u8_t>( key->key_index ),
            static_cast<u16_t>( key->key_length ),
            key->key,
            mac,
            true_t );
        }

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	    
void core_sub_operation_wpa_connect_c::state_notification(
    state_notification_c * /*state*/ )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::state_notification()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	    
core_error_e core_sub_operation_wpa_connect_c::reassociate(
    network_id_c * /* send_network_id */,
    const wlan_eapol_if_eapol_key_authentication_type_e /* authentication_type */,
    u8_t * PMKID,
    u32_t PMKID_length )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::reassociate()" );

    if( PMKID )
        {
        pmkid_length_m = PMKID_length;
        core_tools_c::copy(
            &pmkid_data_m[0],
            PMKID,
            pmkid_length_m );
        }

    DEBUG( "core_sub_operation_wpa_connect_c::reassociate() - using open authentication algorithm" );
    auth_algorithm_m = core_authentication_mode_open;

    next_state();

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_wpa_connect_c::complete_check_pmksa_cache(
    core_type_list_c<network_id_c> & /* network_id_list */ )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::complete_check_pmksa_cache()" );
    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_wpa_connect_c::complete_start_wpx_fast_roam_reassociation(
    network_id_c * /* receive_network_id */,
    u8_t * reassociation_request_ie,
    u32_t reassociation_request_ie_length )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::complete_start_wpx_fast_roam_reassociation()" );
    
    // Generate a WPX fast-roam IE and append it to assoc_ie_list.
    // Actually this could be done with core_wpx_frame_fast_roam_req_ie_c::instance() to make sure it's size is correct.
    core_frame_dot11_ie_c* ie = core_frame_dot11_ie_c::instance(
        reassociation_request_ie_length,
        reassociation_request_ie,
        true_t );
    if ( !ie )
        {
        DEBUG( "core_sub_operation_wpa_connect_c::complete_start_wpx_fast_roam_reassociation() - unable to generate a dot11 IE" );
        is_cached_sa_used_m = false_t;
        //asynch_goto( core_state_init, CORE_TIMER_IMMEDIATELY );
        return core_error_no_memory;
        }

    assoc_ie_list_m.append(
        ie,
        ie->element_id() );

    if( eapol_auth_type_m == wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam &&
        is_cached_sa_used_m )
        {
        server_m->get_connection_data()->set_eapol_authenticating(
            true_t ); 
        }

    asynch_goto( core_state_do_connect, CORE_TIMER_IMMEDIATELY );
    return core_error_ok;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_wpa_connect_c::new_protected_setup_credentials(
    core_type_list_c< protected_setup_credential_c > & /* credential_list */ )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::new_protected_setup_credentials()" );
    
    ASSERT( false_t );
    
    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_wpa_connect_c::complete_disassociation(
    network_id_c * /* receive_network_id */ )
    {
    DEBUG( "core_sub_operation_wpa_connect_c::complete_disassociation()" );

    ASSERT( false_t );

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_sub_operation_wpa_connect_c::handle_error(
    wlan_eapol_if_error_e errorcode,
    wlan_eapol_if_message_type_function_e function )
    {
    DEBUG3( "core_sub_operation_wpa_connect_c::handle_error() - Received error message: errorcode=%i, function=%i (operation_state_m=%i)", 
        errorcode, 
        function, 
        operation_state_m );
    
    if ( errorcode != wlan_eapol_if_error_ok 
        && errorcode != wlan_eapol_if_error_pending_request )
        {
        if ( function == wlan_eapol_if_message_type_function_start_reassociation )
            {
            // Full authentication is needed.
            is_cached_sa_used_m = false_t;
            asynch_goto( core_state_start_authentication_needed, CORE_TIMER_IMMEDIATELY );
            }
        else if ( function == wlan_eapol_if_message_type_function_start_wpx_fast_roam_reassociation )
            {
            is_cached_sa_used_m = false_t;

            asynch_goto( core_state_init, CORE_TIMER_IMMEDIATELY );
            }
        else if ( ( function == wlan_eapol_if_message_type_function_complete_association
               || function == wlan_eapol_if_message_type_function_complete_reassociation
               || function == wlan_eapol_if_message_type_function_complete_wpx_fast_roam_reassociation  ) &&
               eapol_auth_type_m == wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam )
            {
            DEBUG( "core_sub_operation_wpa_connect_c::handle_error() - (WPX fast-roam) (re-)association failed" );
            asynch_goto( core_state_req_association_failed, CORE_TIMER_IMMEDIATELY );
            }
        /*else if ( function == wlan_eapol_if_message_type_function_start_authentication )
            {
            }*/
        else
            {
            DEBUG( "core_sub_operation_wpa_connect_c::handle_error() - Error ignored." );
            }
        }
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_sub_operation_wpa_connect_c::notify(
    core_am_indication_e indication )
    {
    if ( operation_state_m == core_state_req_state_notification &&
         ( indication == core_am_indication_wlan_media_disconnect ||
           indication == core_am_indication_wlan_beacon_lost ||
           indication == core_am_indication_wlan_power_mode_failure ||
           indication == core_am_indication_wlan_tx_fail ) )
        {
        server_m->unregister_event_handler( this );

        if ( indication == core_am_indication_wlan_media_disconnect )
            {
            DEBUG( "core_sub_operation_wpa_connect_c::notify() - AP has disconnected us during authentication, notifying EAPOL" );
            }
        else
            {
            DEBUG( "core_sub_operation_wpa_connect_c::notify() - connection to the AP has been lost, notifying EAPOL" );
            }

        network_id_c network(
            &current_bssid_m.addr[0],
            MAC_ADDR_LEN,
            &server_m->own_mac_addr().addr[0],
            MAC_ADDR_LEN,
            server_m->get_eapol_instance().ethernet_type() );

        DEBUG6( "core_sub_operation_wpa_connect_c::notify() - EAPOL disassociation from BSSID %02X:%02X:%02X:%02X:%02X:%02X",
            current_bssid_m.addr[0], current_bssid_m.addr[1], current_bssid_m.addr[2],
            current_bssid_m.addr[3], current_bssid_m.addr[4], current_bssid_m.addr[5] );
        DEBUG( "core_sub_operation_wpa_connect_c::next_state() - marking is_eapol_disconnecting as true" );
        server_m->get_connection_data()->set_eapol_disconnecting(
            true );
        server_m->get_connection_data()->set_eapol_auth_failure(
            core_error_eapol_failure );

        server_m->get_eapol_instance().disassociation( &network );

        /**
         * EAPOL indication will move the state machine forward.
         */

        return true_t;
        }

    return false_t;
    }
