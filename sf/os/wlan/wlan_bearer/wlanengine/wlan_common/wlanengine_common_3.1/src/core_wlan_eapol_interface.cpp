/*
* Copyright (c) 2001-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface to EAPOL authentication protocols.
*
*/


// INCLUDE FILES

#include "core_wlan_eapol_interface.h"
#include "abs_core_server_callback.h"
#include "core_tools.h"

// LOCAL CONSTANTS

const uint_t CORE_EAPOL_TRAILER_LENGTH = 0;
const uint_t CORE_EAPOL_HEADER_OFFSET  = 0;
const uint_t CORE_EAPOL_MTU_VALUE = 1500;

// ================= MEMBER FUNCTIONS =======================

//--------------------------------------------------

core_wlan_eapol_interface_c::core_wlan_eapol_interface_c(
    abs_core_server_callback_c& wlm_callback ) :
    wlm_callback_m( wlm_callback ),
    function( ),
    completed_check_pmksa_cache_list( ),
    stored_frame_m( NULL ),
    eapol_mode_m( core_eapol_operating_mode_wfa ),
    authentication_type_m( wlan_eapol_if_eapol_key_authentication_type_none )
    {
    DEBUG( "core_wlan_eapol_interface_c::core_wlan_eapol_interface_c()" );
    }

//--------------------------------------------------

core_wlan_eapol_interface_c::~core_wlan_eapol_interface_c()
    {
    DEBUG( "core_wlan_eapol_interface_c::~core_wlan_eapol_interface_c()" );

    delete stored_frame_m;
    }

//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::convert_if_status_to_core_error( const u32_t if_status )
    {
    DEBUG1( "core_wlan_eapol_interface_c::convert_if_status_to_core_error( %i )", if_status );
    
    switch ( if_status )
        {
        case wlan_eapol_if_error_ok:
            return core_error_ok;
        case wlan_eapol_if_error_pending_request:
            return core_error_request_pending;
        case wlan_eapol_if_error_allocation_error:
            return core_error_no_memory;
        case wlan_eapol_if_error_illegal_parameter:
            return core_error_illegal_argument;
        case wlan_eapol_if_error_process_general_error:
            return core_error_general;
        default:
            DEBUG( "core_wlan_eapol_interface_c::convert_if_status_to_core_error - Error: unknown status" );
            return core_error_general;
        }
    }

//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::load_eapol( 
    core_eapol_operating_mode_e mode,
    abs_wlan_eapol_callback_c* const eapol_handler )
    {
    DEBUG( "core_wlan_eapol_interface_c::load_eapol()" );

    eapol_mode_m = mode;
    return core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.load_eapol( mode, eapol_handler ) );
    }

//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::shutdown()
    {
    DEBUG( "core_wlan_eapol_interface_c::shutdown()" );
    return core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.shutdown() );
    }

//--------------------------------------------------

core_error_e core_wlan_eapol_interface_c::configure()
    {
    DEBUG( "core_wlan_eapol_interface_c::configure()" );
    return core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.configure(
            CORE_EAPOL_HEADER_OFFSET,
            CORE_EAPOL_MTU_VALUE,
            CORE_EAPOL_TRAILER_LENGTH ) );
    }

//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::check_pmksa_cache(
    core_type_list_c<network_id_c> & network_id_list,
    const wlan_eapol_if_eapol_key_authentication_type_e selected_eapol_key_authentication_type,
    const wlan_eapol_if_rsna_cipher_e pairwise_key_cipher_suite,
    const wlan_eapol_if_rsna_cipher_e group_key_cipher_suite )
    {
    DEBUG( "core_wlan_eapol_interface_c::check_pmksa_cache()" );
    core_error_e error = function.check_pmksa_cache(
        network_id_list,
        selected_eapol_key_authentication_type, 
        pairwise_key_cipher_suite,
        group_key_cipher_suite );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::check_pmksa_cache() - Encoding function returns %i", error );
        return error;
        }
    return core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    }

//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::start_authentication(
    u8_t * ssid, const u32_t ssid_length, 
    const wlan_eapol_if_eapol_key_authentication_type_e selected_eapol_key_authentication_type,
    u8_t * wpa_psk, const u32_t wpa_psk_length, 
    const bool_t wpa_override_enabled,
    const network_id_c * network_id )
    {
    DEBUG( "core_wlan_eapol_interface_c::start_authentication()" );

    authentication_type_m = selected_eapol_key_authentication_type;

    core_error_e error = function.start_authentication(
        ssid, ssid_length,
        selected_eapol_key_authentication_type,
        wpa_psk, wpa_psk_length,
        wpa_override_enabled,
        network_id );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::start_authentication() - Encoding function returns %i", error );
        return error;
        }
    return core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    }

//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::complete_association(
    const wlan_eapol_if_eapol_wlan_authentication_state_e association_result,
    const network_id_c * network_id,
    u8_t * received_wpa_ie, const u32_t received_wpa_ie_length,
    u8_t * sent_wpa_ie, const u32_t sent_wpa_ie_length,
    const wlan_eapol_if_rsna_cipher_e pairwise_key_cipher_suite,
    const wlan_eapol_if_rsna_cipher_e group_key_cipher_suite )
    {
    DEBUG( "core_wlan_eapol_interface_c::complete_association()" );
    core_error_e error = function.complete_association(
        association_result,
        network_id,
        received_wpa_ie, received_wpa_ie_length,
        sent_wpa_ie, sent_wpa_ie_length,
        pairwise_key_cipher_suite,
        group_key_cipher_suite );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::complete_association() - Encoding function returns %i", error );
        return error;
        }
    return core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    }

//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::disassociation(
    const network_id_c * network_id ) ///< source includes remote address, destination includes local address.
    {
    DEBUG( "core_wlan_eapol_interface_c::disassociation()" );
    core_error_e error = function.disassociation(
        network_id );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::disassociation() - Encoding function returns %i", error );
        return error;
        }
    core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    return error;
    }


//--------------------------------------------------


//
core_error_e core_wlan_eapol_interface_c::start_preauthentication(
    const network_id_c * network_id )
    {
    DEBUG( "core_wlan_eapol_interface_c::start_preauthentication()" );
    core_error_e error = function.start_preauthentication(
        network_id );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::start_preauthentication() - Encoding function returns %i", error );
        return error;
        }
    core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    return error;
    }


//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::start_reassociation(
    const network_id_c * old_network_id,
    const network_id_c * new_network_id,
    const wlan_eapol_if_eapol_key_authentication_type_e selected_eapol_key_authentication_type )
    {
    DEBUG( "core_wlan_eapol_interface_c::start_reassociation()" );

    authentication_type_m = selected_eapol_key_authentication_type;

    core_error_e error = function.start_reassociation(
        old_network_id,
        new_network_id, 
        selected_eapol_key_authentication_type );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::start_reassociation() - Encoding function returns %i", error );
        return error;
        }
    core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    return error;
    }

//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::complete_reassociation(
    const wlan_eapol_if_eapol_wlan_authentication_state_e association_result,
    const network_id_c * network_id,
    u8_t * received_wpa_ie, const u32_t received_wpa_ie_length,
    u8_t * sent_wpa_ie, const u32_t sent_wpa_ie_length,
    const wlan_eapol_if_rsna_cipher_e pairwise_key_cipher_suite,
    const wlan_eapol_if_rsna_cipher_e group_key_cipher_suite )
    {
    DEBUG( "core_wlan_eapol_interface_c::complete_reassociation()" );
    core_error_e error = function.complete_reassociation(
        association_result,
        network_id,
        received_wpa_ie, received_wpa_ie_length,
        sent_wpa_ie, sent_wpa_ie_length,
        pairwise_key_cipher_suite,
        group_key_cipher_suite );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::complete_reassociation() - Encoding function returns %i", error );
        return error;
        }
    core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    return error;
    }


//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::start_wpx_fast_roam_reassociation(
    const network_id_c * old_network_id,
    const network_id_c * new_network_id,
    u8_t * reassociation_request_ie,
    const u32_t reassociation_request_ie_length,
    const u8_t* received_wpa_ie,
    u32_t received_wpa_ie_length,
    const u8_t* sent_wpa_ie,
    u32_t sent_wpa_ie_length )
    {
    DEBUG( "core_wlan_eapol_interface_c::start_wpx_fast_roam_reassociation()" );

    authentication_type_m = wlan_eapol_if_eapol_key_authentication_type_wpx_fast_roam;

    core_error_e error = function.start_wpx_fast_roam_reassociation(
        old_network_id,
        new_network_id,
        reassociation_request_ie,
        reassociation_request_ie_length,
        received_wpa_ie,
        received_wpa_ie_length,
        sent_wpa_ie,
        sent_wpa_ie_length );

    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::start_wpx_fast_roam_reassociation() - Encoding function returns %i", error );
        return error;
        }
    core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    return error;
    }

//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::complete_wpx_fast_roam_reassociation(
    const wlan_eapol_if_eapol_wlan_authentication_state_e reassociation_result,
    const network_id_c * network_id,
    u8_t * received_reassociation_ie, const u32_t received_reassociation_ie_length )
    {
    DEBUG( "core_wlan_eapol_interface_c::complete_wpx_fast_roam_reassociation()" );
    core_error_e error = function.complete_wpx_fast_roam_reassociation(
        reassociation_result,
        network_id,
        received_reassociation_ie, received_reassociation_ie_length );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::complete_wpx_fast_roam_reassociation() - Encoding function returns %i", error );
        return error;
        }
    core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    return error;
    }


//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::process_frame(
    const core_frame_ethernet_c& frame )
    {
    DEBUG( "core_wlan_eapol_interface_c::process_frame()" );

    core_mac_address_s destination =
        frame.destination();
    core_mac_address_s source =
        frame.source();

    network_id_c network_id(
        const_cast<u8_t*>( &source.addr[0] ),
        MAC_ADDR_LEN,
        &destination.addr[0],
        MAC_ADDR_LEN,
        frame.type() );

    core_error_e error = function.packet_process(
        &network_id,
        frame.data(), frame.data_length() );

    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::process_frame() - Encoding function returns %i", error );
        return error;
        }
    core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    return error;
    }

//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::tkip_mic_failure(
    const network_id_c * network_id,
    const bool_t is_fatal_failure, 
    const wlan_eapol_if_eapol_tkip_mic_failure_type_e tkip_mic_failure_type )
    {
    DEBUG( "core_wlan_eapol_interface_c::tkip_mic_failure()" );
    
    core_error_e error = function.tkip_mic_failure(
        network_id,
        is_fatal_failure, 
        tkip_mic_failure_type );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::tkip_mic_failure() - Encoding function returns %i", error );
        return error;
        }
    core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    return error;
    }

//--------------------------------------------------

//
core_error_e core_wlan_eapol_interface_c::eap_acknowledge(
    const network_id_c * network_id )
    {
    DEBUG( "core_wlan_eapol_interface_c::eap_acknowledge()" );
    core_error_e error = function.eap_acknowledge(
        network_id );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::eap_acknowledge() - Encoding function returns %i", error );
        return error;
        }
    core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    return error;
    }

//--------------------------------------------------

core_error_e core_wlan_eapol_interface_c::update_header_offset(
    const u32_t header_offset,
    const u32_t MTU,
    const u32_t trailer_length )
    {
    DEBUG( "core_wlan_eapol_interface_c::update_header_offset()" );
    core_error_e error = function.update_header_offset(
        header_offset,
        MTU, 
        trailer_length );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::update_header_offset() - Encoding function returns %i", error );
        return error;
        }
    core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    return error;
    }

//--------------------------------------------------				

core_error_e core_wlan_eapol_interface_c::update_wlan_database_reference_values(
    u8_t * database_reference_value, const u32_t database_reference_value_length )
    {
    DEBUG( "core_wlan_eapol_interface_c::update_wlan_database_reference_values()" );
    core_error_e error = function.update_wlan_database_reference_values(
        database_reference_value, database_reference_value_length );
    
    if ( error != core_error_ok )
        {
        DEBUG1( "core_wlan_eapol_interface_c::update_wlan_database_reference_values() - Encoding function returns %i", error );
        return error;
        }
    core_wlan_eapol_interface_c::convert_if_status_to_core_error( 
        wlm_callback_m.process_data( function.get_data(), function.size()) );
    return error;
    }

//--------------------------------------------------                

core_error_e core_wlan_eapol_interface_c::update_completed_check_pmksa_cache_list(
    core_type_list_c<network_id_c> & network_id_list )
    {
    DEBUG1( "core_wlan_eapol_interface_c::update_completed_check_pmksa_cache_list() - network_id_list.count() = %i", network_id_list.count() );
    
    completed_check_pmksa_cache_list.clear();
    
    network_id_list.first();
    while ( network_id_list.current() )
        {
        if ( network_id_list.current()->source_ptr )
            {
            DEBUG( "core_wlan_eapol_interface_c::update_completed_check_pmksa_cache_list() - Adding mac address to completed_check_pmksa_cache_list" );
            core_mac_address_s * mac = new core_mac_address_s(
                network_id_list.current()->source() );
            if ( !mac )
                {
                DEBUG( "core_wlan_eapol_interface_c::update_completed_check_pmksa_cache_list() - new core_mac_address_s failed" );
                return core_error_no_memory;
                }

            core_error_e error = completed_check_pmksa_cache_list.append( mac );
            if ( error != core_error_ok )
                {
                delete mac;
                mac = NULL;
                DEBUG1( "core_wlan_eapol_interface_c::update_completed_check_pmksa_cache_list() - completed_check_pmksa_cache_list.append( mac ) failed with code %i", error );
                return error;
                }
            }
        else
            {
            DEBUG( "core_wlan_eapol_interface_c::update_completed_check_pmksa_cache_list() - source == NULL" );
            }
    
        network_id_list.next();
        }
    
    DEBUG1( "core_wlan_eapol_interface_c::update_completed_check_pmksa_cache_list() - completed_check_pmksa_cache_list.count() %i", completed_check_pmksa_cache_list.count() );
    return core_error_ok;
    }

//--------------------------------------------------                

core_type_list_c<core_mac_address_s> & core_wlan_eapol_interface_c::get_completed_check_pmksa_cache_list()
    {
    return completed_check_pmksa_cache_list;
    }

//--------------------------------------------------                

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_wlan_eapol_interface_c::store_frame(
    const core_frame_ethernet_c& frame )
    {
    DEBUG( "core_wlan_eapol_interface_c::store_frame()" );
    
    if ( stored_frame_m )
        {
        DEBUG( "core_wlan_eapol_interface_c::store_frame() - deleting previously stored frame" );

        delete stored_frame_m;
        stored_frame_m = NULL;
        }

    stored_frame_m = core_frame_ethernet_c::instance(
        frame.data_length(), frame.data(), true_t );
    if ( !stored_frame_m )
        {
        DEBUG( "core_wlan_eapol_interface_c::store_frame() - unable to copy the frame" );

        return core_error_no_memory;
        }

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_wlan_eapol_interface_c::clear_stored_frame()
    {
    delete stored_frame_m;
    stored_frame_m = NULL;   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_wlan_eapol_interface_c::process_stored_frame()
    {
    if ( !stored_frame_m )
        {
        DEBUG( "core_wlan_eapol_interface_c::store_frame() - no stored frame" );
        
        return core_error_ok;
        }

    core_error_e ret = process_frame( *stored_frame_m );
    clear_stored_frame();

    return ret;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_eapol_operating_mode_e core_wlan_eapol_interface_c::operating_mode() const
    {
    return eapol_mode_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
wlan_eapol_ethernet_type_e core_wlan_eapol_interface_c::ethernet_type() const
    {
    if( eapol_mode_m == core_eapol_operating_mode_wapi )
        {
        return wlan_eapol_ethernet_type_wapi;
        }

    return wlan_eapol_ethernet_type_pae;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
wlan_eapol_if_eapol_key_authentication_type_e core_wlan_eapol_interface_c::authentication_type() const
    {
    return authentication_type_m;
    }

// End of file
