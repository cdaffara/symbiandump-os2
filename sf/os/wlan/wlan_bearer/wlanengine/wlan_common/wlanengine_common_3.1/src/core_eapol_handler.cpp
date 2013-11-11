/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  handler to EAPOL callbacks.
*
*/


#include "core_eapol_handler.h"
#include "core_server.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "abs_core_protected_setup_handler.h"
#include "am_debug.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_eapol_handler_c::core_eapol_handler_c(
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation ) :
    abs_wlan_eapol_callback_interface_c(),
    server_m( server ),
    drivers_m( drivers ),
    adaptation_m( adaptation ),
    handler_m( NULL ),
    protected_setup_handler_m( NULL )
    {
    DEBUG( "core_eapol_handler_c::core_eapol_handler_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_eapol_handler_c::~core_eapol_handler_c()
    {
    DEBUG( "core_eapol_handler_c::~core_eapol_handler_c()" );
    server_m = NULL;
    drivers_m = NULL;
    adaptation_m = NULL;
    handler_m = NULL;
    protected_setup_handler_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
abs_wlan_eapol_callback_interface_c* core_eapol_handler_c::eapol_handler()
    {
    return handler_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_eapol_handler_c::set_eapol_handler(
    abs_wlan_eapol_callback_interface_c* handler )
    {
    handler_m = handler;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_eapol_handler_c::set_protected_setup_handler(
    abs_core_protected_setup_handler_c* handler )
    {
    protected_setup_handler_m = handler;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_eapol_handler_c::packet_send(
    network_id_c * send_network_id,
    u8_t * packet_data,
    u32_t packet_data_length,
    bool_t send_unencrypted )
    {
    DEBUG( "core_eapol_handler_c::packet_send()" );

    ASSERT ( server_m != NULL );
    if ( !server_m->get_core_settings().is_connected() )
        {
        DEBUG( "core_eapol_handler_c::packet_send() - not connected or attempting connection, ignoring" );

        return core_error_ok;
        }

    if ( handler_m )
        {
        return handler_m->packet_send(
            send_network_id,
            packet_data,
            packet_data_length,
            send_unencrypted );
        }

    if ( !server_m->get_connection_data()->current_ap_data() )
        {
        /**
         * EAPOL might try to send packets after a failed connection attempt,
         * filter them out. This check is only valid when no handler is registered.
         */
        DEBUG( "core_eapol_handler_c::packet_send() - not connected or attempting connection, ignoring" );

        return core_error_ok;
        }

    server_m->send_data_frame(
        *server_m->get_connection_data()->current_ap_data(),
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
core_error_e core_eapol_handler_c::associate(
    wlan_eapol_if_eapol_key_authentication_mode_e authentication_mode )
    {
    DEBUG( "core_eapol_handler_c::associate()" );

    ASSERT ( handler_m );
    if ( handler_m )
        {
        return handler_m->associate(
                authentication_mode );
        }
    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_eapol_handler_c::disassociate(
    network_id_c * receive_network_id,
    const bool_t self_disassociation )
    {
    DEBUG( "core_eapol_handler_c::disassociate()" );
    
    ASSERT ( handler_m );
    if ( handler_m )
        {
        return handler_m->disassociate(
                receive_network_id,
                self_disassociation );
        }
    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	    
core_error_e core_eapol_handler_c::packet_data_session_key(
    network_id_c * send_network_id,
    session_key_c * key )
    {
    DEBUG( "core_eapol_handler_c::packet_data_session_key()" );
    ASSERT( key != NULL);
    ASSERT( send_network_id != NULL);

    if ( !handler_m )
        {
        ASSERT ( server_m != NULL );
        core_cipher_key_type_e type =
            core_tools_c::cipher_key_type(
                key->eapol_key_type,
                server_m->get_connection_data()->current_ap_data()->best_pairwise_cipher(),
                server_m->get_connection_data()->current_ap_data()->best_group_cipher() );

        core_mac_address_s mac( BROADCAST_MAC_ADDR );
        if ( key->eapol_key_type == wlan_eapol_if_eapol_key_type_unicast )
            {
            mac = send_network_id->destination();
            }

        ASSERT ( drivers_m != NULL );
        drivers_m->add_cipher_key(
            type,
            static_cast<u8_t>( key->key_index ),
            static_cast<u16_t>( key->key_length ),
            key->key,
            mac,
            true_t );

        return core_error_ok;
        }

    return handler_m->packet_data_session_key(
        send_network_id,
        key );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//	    
void core_eapol_handler_c::state_notification(
    state_notification_c * state )
    {
    DEBUG( "core_eapol_handler_c::state_notification()" );

    const u32_t eap_type_vendor_id_ietf = 0;
    const u32_t eap_type_vendor_id_WFA = 0x00372A;

    ASSERT ( server_m != NULL );
    if ( !server_m->get_core_settings().is_connected() )
        {
        DEBUG( "core_eapol_handler_c::state_notification() - not connected or attempting connection, ignoring" );
        
        return;
        }

    ASSERT ( state != NULL );

    if ( state->protocol_layer == wlan_eapol_if_eap_protocol_layer_wlan_authentication ||
         state->protocol_layer == wlan_eapol_if_eap_protocol_layer_wapi )
        {
        if ( state->current_state == wlan_eapol_if_eapol_wlan_authentication_state_authentication_cancelled )
        	{
        	if ( server_m->get_connection_data()->last_failed_eap_type() == EAP_TYPE_NONE )
	            {
	            // We know that vendor_type values (in ietf and WFA) are not overlapping 
	            // and thus there is no need for storing vendor_id's.
	            if ( state->eap_type_vendor_id == eap_type_vendor_id_ietf 
	                    || state->eap_type_vendor_id == eap_type_vendor_id_WFA )
	                {
	                server_m->get_connection_data()->set_last_failed_eap_type(
	                    state->eap_type_vendor_type );
	                server_m->get_connection_data()->set_last_eap_error(
	                    state->authentication_error );
	                }
	            else
	                {
	                DEBUG1( "core_eapol_handler_c::state_notification() - Unknown vendor_id %i in EAP-type. Type not stored.",
	                    state->eap_type_vendor_id );
	                }
	            }
        	}

        handle_wlan_authentication_state(
            state->current_state,
            state->network_id.destination() );
        }
    else
    	{
    	if ( state->protocol_layer == wlan_eapol_if_eap_protocol_layer_general &&
        state->current_state == wlan_eapol_if_eap_state_general_authentication_error )
	        {
	        DEBUG( "core_eapol_handler_c::state_notification() - EAP-specific authentication error:" );
	        }
        else if ( state->protocol_layer == wlan_eapol_if_eap_protocol_layer_eap &&
                state->current_state == wlan_eapol_if_eap_state_authentication_terminated_unsuccessfully )
            {
            DEBUG( "core_eapol_handler_c::state_notification() - EAP-layer error:" );
            }

    	if (  ( state->protocol_layer == wlan_eapol_if_eap_protocol_layer_general &&
    	        state->current_state == wlan_eapol_if_eap_state_general_authentication_error ) ||
		        ( state->protocol_layer == wlan_eapol_if_eap_protocol_layer_eap &&
		          state->current_state == wlan_eapol_if_eap_state_authentication_terminated_unsuccessfully ) )
    		{
		        DEBUG2( "core_eapol_handler_c::state_notification() - type: vendor_id: %u, vendor_type: %u",
			            state->eap_type_vendor_id,
			            state->eap_type_vendor_type );
			        DEBUG1( "core_eapol_handler_c::state_notification() - error: %u",
			            state->authentication_error );
			        DEBUG1( "core_eapol_handler_c::state_notification() - previous type: %u",
			            server_m->get_connection_data()->last_failed_eap_type() );
			        DEBUG1( "core_eapol_handler_c::state_notification() - previous error: %u",
			            server_m->get_connection_data()->last_eap_error() );
			        
	        if ( server_m->get_connection_data()->last_failed_eap_type() == EAP_TYPE_NONE )
	            {
	            // We know that vendor_type values (in ietf and WFA) are not overlapping 
	            // and thus there is no need for storing vendor_id's.
	            if ( state->eap_type_vendor_id == eap_type_vendor_id_ietf 
	                    || state->eap_type_vendor_id == eap_type_vendor_id_WFA )
	                {
	                server_m->get_connection_data()->set_last_failed_eap_type(
	                    state->eap_type_vendor_type );
	                server_m->get_connection_data()->set_last_eap_error(
	                    state->authentication_error );
	                }
	            else
	                {
	                DEBUG1( "core_eapol_handler_c::state_notification() - Unknown vendor_id %i in EAP-type. Type not stored.",
	                    state->eap_type_vendor_id );
	                }
	            }
    		}
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_eapol_handler_c::reassociate(
    network_id_c * send_network_id,
    const wlan_eapol_if_eapol_key_authentication_type_e authentication_type,
    u8_t * PMKID,
    u32_t PMKID_length )
    {
    DEBUG( "core_eapol_handler_c::reassociate()" );
    
    ASSERT ( handler_m );
    if ( handler_m )
        {
        return handler_m->reassociate(
                send_network_id,
                authentication_type,
                PMKID,
                PMKID_length );
        }
    return core_error_ok;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_eapol_handler_c::convert_core_error_to_if_status(
    core_error_e core_error )
    {
    DEBUG1( "core_eapol_handler_c::convert_core_error_to_if_status( %i )", core_error );
    switch ( core_error )
        {
        case core_error_ok:
            return wlan_eapol_if_error_ok;
        case core_error_request_pending:
            return wlan_eapol_if_error_pending_request;
        case core_error_no_memory:
            return wlan_eapol_if_error_allocation_error;
        case core_error_illegal_argument:
            return wlan_eapol_if_error_illegal_parameter;
        case core_error_general:
            return wlan_eapol_if_error_process_general_error;
        default:
            DEBUG( "core_eapol_handler_c::convert_core_error_to_if_status - Error: No special conversion to given status" );
            return wlan_eapol_if_error_process_general_error;
        }
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_eapol_handler_c::send_data(
    const void * const data, 
    const u32_t length )
    {
    DEBUG2( "core_eapol_handler_c::send_data(data=0x%08X, length=%i)", data, length );
    
    if ( !data || length == 0 )
        {
        DEBUG( "core_eapol_handler_c::send_data() - Parameters not valid. Ignoring." );

        core_wlan_eapol_if_function_c function;
        if ( function.generate_error( wlan_eapol_if_error_illegal_parameter, wlan_eapol_if_message_type_function_none )
            == core_error_ok )
            {
            adaptation_m->process_data( function.get_data(), function.size() );
            }
        return wlan_eapol_if_error_illegal_parameter;
        }
    
    core_wlan_eapol_if_function_c function( static_cast<u8_t *>( const_cast<void *>( data ) ), length );
    
    function.debug_print();
    
    core_error_e error( core_error_ok );

    // 1. Get function
    // 2. Parse it's parameters
    // 3. Call actual function
    
    // Check error case here.
    wlan_eapol_if_message_type_e type = function.get_type();
    if ( type == wlan_eapol_if_message_type_error )
        {
        DEBUG( "core_eapol_handler_c::send_data() - Error received." );
        
        wlan_eapol_if_error_e parsed_errorcode;
        wlan_eapol_if_message_type_function_e parsed_function;
        
        error = function.parse_error( &parsed_errorcode, &parsed_function );
        if ( error != core_error_ok )
            {
            DEBUG1( "core_eapol_handler_c::send_data() - Could not parse error message (parsing error=%i)", error );
            // Do not send Error message when error message parsing fails...
            }
        else
            {
            handle_error( parsed_errorcode, parsed_function );
            }
        return wlan_eapol_if_error_ok;
        }
    else if ( type != wlan_eapol_if_message_type_function )
        {
        DEBUG( "core_eapol_handler_c::send_data() - Type parameter is not valid. Ignoring." );
        
        if ( function.generate_error( wlan_eapol_if_error_illegal_parameter, wlan_eapol_if_message_type_function_none )
            == core_error_ok )
            {
            adaptation_m->process_data( function.get_data(), function.size() );
            }
        return wlan_eapol_if_error_illegal_parameter;
        }
    
    wlan_eapol_if_message_type_function_e func = function.get_function();
    
    switch ( func )
        {
        // These functions are used only from WLAN Engine to EAPOL. So, no need to parse and handle...
        case wlan_eapol_if_message_type_function_check_pmksa_cache:
        case wlan_eapol_if_message_type_function_start_authentication:
        case wlan_eapol_if_message_type_function_complete_association:
        case wlan_eapol_if_message_type_function_disassociation:
        case wlan_eapol_if_message_type_function_start_preauthentication:
        case wlan_eapol_if_message_type_function_start_reassociation:
        case wlan_eapol_if_message_type_function_complete_reassociation:
        case wlan_eapol_if_message_type_function_start_wpx_fast_roam_reassociation:
        case wlan_eapol_if_message_type_function_complete_wpx_fast_roam_reassociation:
        case wlan_eapol_if_message_type_function_packet_process:
        case wlan_eapol_if_message_type_function_tkip_mic_failure:
        case wlan_eapol_if_message_type_function_eap_acknowledge:
        case wlan_eapol_if_message_type_function_update_header_offset:
        case wlan_eapol_if_message_type_function_update_wlan_database_reference_values:
            DEBUG1( "core_eapol_handler_c::send_data() - Error: Message %i coming to from EAPOL to Engine (wrong direction...)", func );
            return wlan_eapol_if_error_illegal_parameter;

        // These functions are handled in WLAN Engine.
        case wlan_eapol_if_message_type_function_complete_check_pmksa_cache:
            {
            core_type_list_c<network_id_c> network_id_list;
            
            error = function.parse_complete_check_pmksa_cache(
                network_id_list );
            if ( error == core_error_ok )
                {
                error = complete_check_pmksa_cache( network_id_list );
                }
            break;
            }
        case wlan_eapol_if_message_type_function_packet_send:
            {
            network_id_c network_id( NULL, 0, NULL, 0, 0 );
            u8_t * packet( NULL );
            u32_t packet_length( 0 );

            wlan_eapol_if_eapol_key_authentication_type_e type(
                server_m->get_eapol_instance().authentication_type() );
            bool_t send_unecrypted( false_t );
            if( type == wlan_eapol_if_eapol_key_authentication_type_wapi ||
                type == wlan_eapol_if_eapol_key_authentication_type_wapi_psk )
                {
                send_unecrypted = true_t;
                }

            error = function.parse_packet_send(
                &network_id,
                &packet, &packet_length );
            if ( error == core_error_ok )
                {               
                error = packet_send( &network_id, packet, packet_length, send_unecrypted );
                }
            break;
            }
        case wlan_eapol_if_message_type_function_associate:
            {
            wlan_eapol_if_eapol_key_authentication_mode_e auth_mode( wlan_eapol_if_eapol_key_authentication_mode_none );
            
            error = function.parse_associate(
                &auth_mode );
            if ( error == core_error_ok )
                {
                error = associate( auth_mode );
                }
            break;
            }
        case wlan_eapol_if_message_type_function_disassociate:
            {
            network_id_c network_id( NULL, 0, NULL, 0, 0 );
            bool_t self_disassociation( false_t );
            
            error = function.parse_disassociate(
                &network_id,
                &self_disassociation );
            if ( error == core_error_ok )
                {
                error = disassociate( &network_id, self_disassociation );
                }
            break;
            }
        case wlan_eapol_if_message_type_function_packet_data_session_key:
            {
            network_id_c network_id( NULL, 0, NULL, 0, 0 );
            session_key_c session_key( NULL, 0, NULL, 0, wlan_eapol_if_eapol_key_type_broadcast, 0, false_t );
            
            error = function.parse_packet_data_session_key(
                &network_id,
                &session_key );
            if ( error == core_error_ok )
                {
                error = packet_data_session_key( &network_id, &session_key );
                }
            break;
            }
        case wlan_eapol_if_message_type_function_state_notification:
            {
            network_id_c network_id( NULL, 0, NULL, 0, 0 );
            state_notification_c state_notif( &network_id, wlan_eapol_if_eap_protocol_layer_none, 0, 0, 0, 0, false_t, wlan_eapol_if_eap_status_none );
            
            error = function.parse_state_notification(
                &state_notif );
            if ( error == core_error_ok )
                {
                state_notification( &state_notif );
                }
            break;
            }
        case wlan_eapol_if_message_type_function_reassociate:
            {
            network_id_c network_id( NULL, 0, NULL, 0, 0 );
            wlan_eapol_if_eapol_key_authentication_type_e authentication_type( wlan_eapol_if_eapol_key_authentication_type_none );
            u8_t * pmkid( NULL );
            u32_t pmkid_length( 0 );
            
            error = function.parse_reassociate(
                &network_id,
                &authentication_type,
                &pmkid,
                &pmkid_length );
            if ( error == core_error_ok )
                {
                error = reassociate(
                    &network_id,
                    authentication_type,
                    pmkid,
                    pmkid_length );
                }
            break;
            }
        case wlan_eapol_if_message_type_function_complete_start_wpx_fast_roam_reassociation:
            {
            network_id_c network_id( NULL, 0, NULL, 0, 0 );
            u8_t * reassociation_request_ie( NULL );
            u32_t reassociation_request_ie_length( 0 );
            
            error = function.parse_complete_start_wpx_fast_roam_reassociation(
                &network_id,
                &reassociation_request_ie,
                &reassociation_request_ie_length );
            if ( error == core_error_ok )
                {
                error = complete_start_wpx_fast_roam_reassociation(
                    &network_id,
                    reassociation_request_ie,
                    reassociation_request_ie_length );
                }
            break;
            }
        case wlan_eapol_if_message_type_function_new_protected_setup_credentials:
            {
            core_type_list_c< protected_setup_credential_c > credential_list;
            
            error = function.parse_new_protected_setup_credentials(
                credential_list );
            if ( error == core_error_ok )
                {
                error = new_protected_setup_credentials(
                    credential_list );
                }
            break;
            }
        case wlan_eapol_if_message_type_function_complete_disassociation:
            {
            network_id_c network_id( NULL, 0, NULL, 0, 0 );
            
            error = function.parse_complete_disassociation(
                &network_id );
            if ( error == core_error_ok )
                {
                error = complete_disassociation(
                    &network_id );
                }
            break;

            }
        case wlan_eapol_if_message_type_function_none:
        default:
            DEBUG1( "core_eapol_handler_c::send_data() - Error: unknown function %i", func );
        }

    // Check if error message should be sent
    if ( error != core_error_ok 
        && error != core_error_request_pending )
        {
        // send error message
        core_error_e ret_val = function.generate_error(
            static_cast<wlan_eapol_if_error_e>( core_eapol_handler_c::convert_core_error_to_if_status( error ) ),
            func );
        if ( ret_val == core_error_ok )
            {
            adaptation_m->process_data( function.get_data(), function.size() );
            }
        }
    return core_eapol_handler_c::convert_core_error_to_if_status( error );
    }



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_eapol_handler_c::complete_check_pmksa_cache(
    core_type_list_c<network_id_c> & network_id_list )
    {
    DEBUG( "core_eapol_handler_c::complete_check_pmksa_cache()" );
    
    return server_m->get_eapol_instance().update_completed_check_pmksa_cache_list( network_id_list );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_eapol_handler_c::complete_start_wpx_fast_roam_reassociation(
    network_id_c * receive_network_id,
    u8_t * reassociation_request_ie,
    u32_t reassociation_request_ie_length )
    {
    DEBUG( "core_eapol_handler_c::complete_start_wpx_fast_roam_reassociation()" );
    
    if ( handler_m )
        {
        return handler_m->complete_start_wpx_fast_roam_reassociation(
            receive_network_id,
            reassociation_request_ie,
            reassociation_request_ie_length );
        }
    return core_error_ok;
    }


enum protected_setup_authentication_type_e
{
    protected_setup_authentication_type_None    = 0x0000,
    protected_setup_authentication_type_Open    = 0x0001,
    protected_setup_authentication_type_WPAPSK  = 0x0002,
    protected_setup_authentication_type_Shared  = 0x0004,
    protected_setup_authentication_type_WPA     = 0x0008,
    protected_setup_authentication_type_WPA2    = 0x0010,
    protected_setup_authentication_type_WPA2PSK = 0x0020,
};

enum protected_setup_encryption_type_e
{
    protected_setup_encryption_type_None    = 0x0001,
    protected_setup_encryption_type_WEP     = 0x0002,
    protected_setup_encryption_type_TKIP    = 0x0004,
    protected_setup_encryption_type_AES     = 0x0008,
};

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_eapol_handler_c::new_protected_setup_credentials(
    core_type_list_c< protected_setup_credential_c > & credential_list )
    {
    DEBUG( "core_eapol_handler_c::new_protected_setup_credentials()" );

    ASSERT( protected_setup_handler_m );

    for ( protected_setup_credential_c* credential = credential_list.first(); credential; credential = credential_list.next() )
        {
        core_iap_data_s iap_data;
        core_tools_c::fillz(
            &iap_data,
            sizeof ( iap_data ) );

        iap_data.id = 0;
        iap_data.op_mode = core_operating_mode_infrastructure;
        iap_data.is_hidden = false_t;
        iap_data.authentication_mode = core_authentication_mode_open;
        iap_data.wpa_preshared_key_in_use = false_t;
        iap_data.is_wpa_overriden = false_t;  
        iap_data.used_adhoc_channel = 0;
        iap_data.is_dynamic_ip_addr = true_t;
        iap_data.is_roaming_allowed = true_t;

        // SSID
        if ( credential->ssid_length > MAX_SSID_LEN )
            {
            DEBUG1( "core_eapol_handler_c::new_protected_setup_credentials() - Error: Too long SSID (%d bytes). Ignoring credentials.", 
                    credential->ssid_length );
            }
        else
            {
            iap_data.ssid.length =
                credential->ssid_length;
            core_tools_c::copy(
                &iap_data.ssid.ssid[0],
                credential->ssid,
                iap_data.ssid.length );

            // Security mode
            switch ( credential->authentication_type )
                {
                case protected_setup_authentication_type_WPAPSK:
                    iap_data.security_mode = core_security_mode_wpa;
                    iap_data.wpa_preshared_key_in_use = true_t;
                    break;
                case protected_setup_authentication_type_WPA2PSK:
                    iap_data.security_mode = core_security_mode_wpa2only;
                    iap_data.wpa_preshared_key_in_use = true_t;
                    break;
                case protected_setup_authentication_type_WPA:
                    iap_data.security_mode = core_security_mode_wpa;
                    break;
                case protected_setup_authentication_type_WPA2:
                    iap_data.security_mode = core_security_mode_wpa2only;
                    break;
                case protected_setup_authentication_type_Shared:
                    iap_data.security_mode = core_security_mode_wep;
                    iap_data.authentication_mode = core_authentication_mode_shared;
                    break;
                case protected_setup_authentication_type_Open:
                    if ( credential->encryption_type == protected_setup_encryption_type_WEP )
                        {
                        iap_data.security_mode = core_security_mode_wep;
                        }
                    else
                        {
                        iap_data.security_mode = core_security_mode_allow_unsecure;
                        }
                    break;
                case protected_setup_authentication_type_None:
                /** Falls through on purpose. */
                default:                
                    iap_data.security_mode = core_security_mode_allow_unsecure;
                    break;
                }

            bool_t ignore_current_credential( false_t );
            // Encryption keys
            switch ( iap_data.security_mode )
                {
                case core_security_mode_wpa:
                    /** Falls through on purpose. */
                case core_security_mode_wpa2only:
                    if ( iap_data.wpa_preshared_key_in_use &&
                         credential->network_key_list.first() )
                        {
                        network_key_c* key = credential->network_key_list.current();
                        if ( key->network_key_length > MAX_WPA_PSK_LENGTH )
                            {
                            DEBUG1( "core_eapol_handler_c::new_protected_setup_credentials() - Error: Too long WPA Passphrase (%d bytes). Ignoring credentials.", 
                                    key->network_key_length );
                            ignore_current_credential = true_t;
                            }
                        else
                            {
                            iap_data.wpa_preshared_key.key_length = key->network_key_length;
                            core_tools_c::copy(
                                &iap_data.wpa_preshared_key.key_data[0],
                                key->network_key,
                                iap_data.wpa_preshared_key.key_length );
                            }
                        }
                    break;
                case core_security_mode_wep:
                    {
                    for ( network_key_c* key = credential->network_key_list.first(); key; key = credential->network_key_list.next())
                        {
                        core_wep_key_s wep_key;
                        if ( key->network_key_length > MAX_WEP_KEY_LENGTH)
                            {
                            // WEP key is ignored if its key_length is too long.
                            DEBUG2( "core_eapol_handler_c::new_protected_setup_credentials() - Error: Too long WEP key for key index %d (%d bytes). Ignoring WEP key.", 
                                    key->network_key_index,
                                    key->network_key_length );
                            }
                        else
                            {
                            wep_key.key_index = key->network_key_index;
                            wep_key.key_length = key->network_key_length;
                            core_tools_c::copy(
                                &wep_key.key_data[0],
                                key->network_key,
                                wep_key.key_length );

                            if ( wep_key.key_index == 1 )
                                {
                                iap_data.wep_key1 = wep_key;
                                }
                            else if ( wep_key.key_index == 2 )
                                {
                                iap_data.wep_key2 = wep_key;
                                }
                            else if ( wep_key.key_index == 3 )
                                {
                                iap_data.wep_key3 = wep_key;
                                }
                            else if ( wep_key.key_index == 4 )
                                {
                                iap_data.wep_key4 = wep_key;
                                }
                            else
                                {
                                // WEP key is ignored if its key_index is not valid.
                                DEBUG1( "core_eapol_handler_c::new_protected_setup_credentials() - Error: WEP key index is not valid (it was %d). Ignoring WEP key.", 
                                        wep_key.key_index );
                                }
                            }
                        }
    
                    /** Use the first defined key as the default key. */
                    if ( iap_data.wep_key1.key_length )
                        {
                        iap_data.default_wep_key = 0;
                        }
                    else if ( iap_data.wep_key2.key_length )
                        {
                        iap_data.default_wep_key = 1;
                        }
                    else if ( iap_data.wep_key3.key_length )
                        {
                        iap_data.default_wep_key = 2;
                        }
                    else if ( iap_data.wep_key4.key_length )
                        {
                        iap_data.default_wep_key = 3;
                        }
                    else
                        {
                        // Credentials are ignored if there was no valid keys.
                        DEBUG( "core_eapol_handler_c::new_protected_setup_credentials() - Error: No valid WEP keys defined. Could not set default wep key. Ignoring credentials." );
                        ignore_current_credential = true_t;
                        }

                    break;
                    }
                default:
                    break;
                }
    
            if ( !ignore_current_credential )
                {
                protected_setup_handler_m->handle_protected_setup_network( iap_data );
                }
            }
        }
    
    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_eapol_handler_c::complete_disassociation(
    network_id_c * receive_network_id )
    {
    DEBUG( "core_eapol_handler_c::complete_disassociation()" );

    if( !server_m->get_connection_data() ||
        !server_m->get_connection_data()->is_eapol_disconnecting() )
        {
        DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - complete_disassociation received while not disconnecting, ignoring" );

        return core_error_general;
        }

    const core_mac_address_s cur_bssid(
        server_m->get_connection_data()->eapol_auth_bssid() );
    const core_mac_address_s bssid(
        receive_network_id->source() );
    DEBUG6( "core_eapol_handler_c::complete_disassociation() - function BSSID is %02X:%02X:%02X:%02X:%02X:%02X",
        bssid.addr[0], bssid.addr[1], bssid.addr[2], 
        bssid.addr[3], bssid.addr[4], bssid.addr[5] );
    DEBUG6( "core_eapol_handler_c::complete_disassociation() - current BSSID is %02X:%02X:%02X:%02X:%02X:%02X",
        cur_bssid.addr[0], cur_bssid.addr[1], cur_bssid.addr[2], 
        cur_bssid.addr[3], cur_bssid.addr[4], cur_bssid.addr[5] );
    DEBUG1( "core_eapol_handler_c::complete_disassociation() - EAPOL authentication failure status is %u",
        server_m->get_connection_data()->eapol_auth_failure() );

    bool_t is_eapol_authentication_started(
        server_m->get_connection_data()->is_eapol_authentication_started() );
    DEBUG( "core_eapol_handler_c::complete_disassociation() - marking is_eapol_authenticating as false" );
    server_m->get_connection_data()->set_eapol_authenticating(
        false_t );
    DEBUG( "core_eapol_handler_c::complete_disassociation() - marking is_eapol_authentication_started as false" );
    server_m->get_connection_data()->set_eapol_authentication_started(
        false_t );
    DEBUG( "core_eapol_handler_c::complete_disassociation() - marking is_eapol_disconnecting as false" );
    server_m->get_connection_data()->set_eapol_disconnecting(
        false_t );

    /**
     * We only care about the pending status notification in case the authentication
     * has been started by us, otherwise we'll just ignore it. 
     */
    if ( is_eapol_authentication_started )
        {
        DEBUG( "core_eapol_handler_c::complete_disassociation() - completing request" );
        server_m->request_complete(
            REQUEST_ID_CORE_INTERNAL,
            server_m->get_connection_data()->eapol_auth_failure() );
        }
    else
        {
        DEBUG( "core_eapol_handler_c::complete_disassociation() - completing request (authentication not started)" );
        server_m->request_complete(
            REQUEST_ID_CORE_INTERNAL,
            core_error_ok );
        }

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_eapol_handler_c::handle_error(
    wlan_eapol_if_error_e errorcode,
    wlan_eapol_if_message_type_function_e function )
    {
    DEBUG2( "core_eapol_handler_c::handle_error() - Received error message: errorcode=%i, function=%i", errorcode, function );
    
    if ( handler_m )
        {
        handler_m->handle_error(
            errorcode,
            function );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_eapol_handler_c::handle_wlan_authentication_state(
    u32_t state,
    const core_mac_address_s& bssid )
    {
#ifdef _DEBUG
    switch ( state )
        {
        case wlan_eapol_if_eapol_wlan_authentication_state_no_response:
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - wlan_eapol_if_eapol_wlan_authentication_state_no_response" );
            break;    
        case wlan_eapol_if_eapol_wlan_authentication_state_this_ap_failed:
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - wlan_eapol_if_eapol_wlan_authentication_state_this_ap_failed" );
            break;
        case wlan_eapol_if_eapol_wlan_authentication_state_failed_completely:
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - wlan_eapol_if_eapol_wlan_authentication_state_failed_completely" );
            break;
        case wlan_eapol_if_eapol_wlan_authentication_state_authentication_cancelled:
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - wlan_eapol_if_eapol_wlan_authentication_state_authentication_cancelled" );
            break;
        case wlan_eapol_if_eapol_wlan_authentication_state_authentication_successfull:
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - wlan_eapol_if_eapol_wlan_authentication_state_authentication_successfull" );
            break;
        case wlan_eapol_if_eapol_wlan_authentication_state_association_ok:
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - wlan_eapol_if_eapol_wlan_authentication_state_association_ok" );
            break;
        case wlan_eapol_if_eapol_wlan_authentication_state_802_11_auth_algorithm_not_supported:
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - wlan_eapol_if_eapol_wlan_authentication_state_802_11_auth_algorithm_not_supported" );
            break;
        case wlan_eapol_if_eapol_wlan_authentication_state_eap_authentication_running:
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - wlan_eapol_if_eapol_wlan_authentication_state_eap_authentication_running" );
            break;
        case wlan_eapol_if_eapol_wlan_authentication_state_4_way_handshake_running:
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - wlan_eapol_if_eapol_wlan_authentication_state_4_way_handshake_running" );
            break;
        case wlan_eapol_if_eapol_wlan_authentication_state_wapi_authentication_running:
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - wlan_eapol_if_eapol_wlan_authentication_state_wapi_authentication_running" );
            break;            
        }
#endif // _DEBUG

    const core_mac_address_s cur_bssid(
        server_m->get_connection_data()->eapol_auth_bssid() );

    DEBUG6( "core_eapol_handler_c::handle_wlan_authentication_state() - notification BSSID is %02X:%02X:%02X:%02X:%02X:%02X",
        bssid.addr[0], bssid.addr[1], bssid.addr[2], 
        bssid.addr[3], bssid.addr[4], bssid.addr[5] );
    DEBUG6( "core_eapol_handler_c::handle_wlan_authentication_state() - current BSSID is %02X:%02X:%02X:%02X:%02X:%02X",
        cur_bssid.addr[0], cur_bssid.addr[1], cur_bssid.addr[2], 
        cur_bssid.addr[3], cur_bssid.addr[4], cur_bssid.addr[5] );

    if ( state == wlan_eapol_if_eapol_wlan_authentication_state_eap_authentication_running ||
         state == wlan_eapol_if_eapol_wlan_authentication_state_4_way_handshake_running ||
         state == wlan_eapol_if_eapol_wlan_authentication_state_wapi_authentication_running )
        {
        DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - marking is_eapol_authenticating as true" );

        /**
         * EAPOL has started authenticating, reset the previous
         * failures.
         */
        ASSERT ( server_m != NULL );

        server_m->get_connection_data()->set_eapol_authenticating(
            true_t ); 
        server_m->get_connection_data()->set_last_failed_eap_type(
            EAP_TYPE_NONE );
        server_m->get_connection_data()->set_last_eap_error(
            EAP_ERROR_NONE );

        return;
        }

    // EAP-FAST will fail after PAC provisioning. With this state EAPOL requires wlan engine to reconnect again.
    if ( state == wlan_eapol_if_eapol_wlan_authentication_state_immediate_reconnect )
        {
        DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - immediate reconnect required" );
        
        server_m->get_connection_data()->set_eapol_require_immediate_reconnect( true_t );
        
        return;
        }
    
    if ( !is_eapol_wlan_authentication_state_success( state ) &&
         !is_eapol_wlan_authentication_state_failure( state ) )
        {
        DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - neither a failure nor a success, ignoring notification" );

        return;
        }
    else if ( !server_m->get_connection_data()->is_eapol_authenticating() &&
              !server_m->get_connection_data()->is_eapol_authentication_started() )
        {
        DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - failure or success received while not authenticating, ignoring notification" );

        return;
        }
    else if ( bssid != ZERO_MAC_ADDR &&
              bssid != cur_bssid )
        {
        DEBUG( "core_ap_data_c::instance() - notification doesn't match the current BSSID, ignoring notification" );

        return;
        }

    server_m->get_connection_data()->set_eapol_auth_failure(
        eapol_wlan_authentication_state_to_error( state )  );

    if ( server_m->get_connection_data()->is_eapol_disconnecting() )
        {
        DEBUG( "core_ap_data_c::instance() - disassociation pending, request cannot be completed yet" );

        return;
        }

    bool_t is_authentication_started( server_m->get_connection_data()->is_eapol_authenticating() );

    DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - marking is_eapol_authenticating as false" );
    server_m->get_connection_data()->set_eapol_authenticating(
        false_t );
    DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - marking is_eapol_authentication_started as false" );
    server_m->get_connection_data()->set_eapol_authentication_started(
        false_t );

    if ( handler_m )
        {
        DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - eapol handler registered" );

        // Check whether authentication ended without getting even first message from AP.
        if ( is_authentication_started
             || state == wlan_eapol_if_eapol_wlan_authentication_state_failed_completely
             || state == wlan_eapol_if_eapol_wlan_authentication_state_authentication_cancelled 
             || state == wlan_eapol_if_eapol_wlan_authentication_state_authentication_successfull )
            {
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - completing request" );
            server_m->request_complete(
                    REQUEST_ID_CORE_INTERNAL,
                    eapol_wlan_authentication_state_to_error( state ) );
            }
        else
            {
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - completing request (authentication not yet started)" );
            server_m->request_complete(
                    REQUEST_ID_CORE_INTERNAL,
                    core_error_eapol_auth_start_timeout );
            }
        }
    else
        {
        DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - no eapol handler registered" );

        /**
         * Since we have no operation to complete, it means this indication is
         * about a reauthentication.
         */
        if ( is_eapol_wlan_authentication_state_failure( state ) )
            {
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - reauthentication has failed, scheduling a roam" );
            server_m->schedule_roam(
                core_operation_handle_bss_lost_c::core_bss_lost_reason_failed_reauthentication );
            }
        else
            {
            DEBUG( "core_eapol_handler_c::handle_wlan_authentication_state() - reauthentication has succeeded" );
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_eapol_handler_c::eapol_wlan_authentication_state_to_error(
    u32_t state ) const
    {
    switch ( state )
        {
        case wlan_eapol_if_eapol_wlan_authentication_state_no_response:
            /** Falls through on purpose. */
        case wlan_eapol_if_eapol_wlan_authentication_state_this_ap_failed:
            return core_error_eapol_failure;
        case wlan_eapol_if_eapol_wlan_authentication_state_failed_completely:
        	/** Falls through on purpose. */
        case wlan_eapol_if_eapol_wlan_authentication_state_authentication_cancelled:
            return core_error_eapol_total_failure;
        case wlan_eapol_if_eapol_wlan_authentication_state_authentication_successfull:
            /** Falls through on purpose. */
        default:
            return core_error_ok;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_eapol_handler_c::is_eapol_wlan_authentication_state_failure(
    u32_t state ) const
    {
    if ( state == wlan_eapol_if_eapol_wlan_authentication_state_no_response ||
         state == wlan_eapol_if_eapol_wlan_authentication_state_this_ap_failed ||
         state == wlan_eapol_if_eapol_wlan_authentication_state_failed_completely ||
         state == wlan_eapol_if_eapol_wlan_authentication_state_authentication_cancelled )
        {
        return true_t;
        }
        
    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_eapol_handler_c::is_eapol_wlan_authentication_state_success(
    u32_t state ) const
    {
    if ( state == wlan_eapol_if_eapol_wlan_authentication_state_authentication_successfull )
        {
        return true_t;
        }
        
    return false_t;
    }
