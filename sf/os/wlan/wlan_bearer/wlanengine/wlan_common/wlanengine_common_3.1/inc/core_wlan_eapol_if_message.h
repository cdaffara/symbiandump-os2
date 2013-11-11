/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for parsing and generating messages for Engine-EAPOL interface.
*
*/


#ifndef CORE_WLAN_EAPOL_IF_MESSAGE_H
#define CORE_WLAN_EAPOL_IF_MESSAGE_H

#include "am_platform_libraries.h"
#include "core_type_list.h"
#include "core_types.h"
#include "core_tools.h"

// Compile time assert. Used to make sure that generated datatypes are correct size.
#define COMPILE_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]


/**
 * Class for parsing and generating WLAN EAPOL Plugin interface messages.
 *
 * @since S60 v3.2
 */


enum wlan_eapol_if_message_type_e
    {
    wlan_eapol_if_message_type_none                                 = 0,
    wlan_eapol_if_message_type_array                                = 1,
    wlan_eapol_if_message_type_boolean                              = 2,
    wlan_eapol_if_message_type_eap_protocol_layer                   = 3,
    wlan_eapol_if_message_type_eap_state_notification               = 4,
    wlan_eapol_if_message_type_eap_status                           = 5,
    wlan_eapol_if_message_type_eap_type                             = 6,
    wlan_eapol_if_message_type_eapol_key_802_11_authentication_mode = 7,
    wlan_eapol_if_message_type_eapol_key_authentication_type        = 8,
    wlan_eapol_if_message_type_eapol_key_type                       = 9,
    wlan_eapol_if_message_type_eapol_tkip_mic_failure_type          = 10,
    wlan_eapol_if_message_type_eapol_wlan_authentication_state      = 11,
    wlan_eapol_if_message_type_error                                = 12,
    wlan_eapol_if_message_type_function                             = 13,
    wlan_eapol_if_message_type_network_id                           = 14,
    wlan_eapol_if_message_type_network_key                          = 15,
    wlan_eapol_if_message_type_protected_setup_credential           = 16,
    wlan_eapol_if_message_type_RSNA_cipher                          = 17,
    wlan_eapol_if_message_type_session_key                          = 18,
    wlan_eapol_if_message_type_u8_t                                 = 19,
    wlan_eapol_if_message_type_u16_t                                = 20,
    wlan_eapol_if_message_type_u32_t                                = 21,
    wlan_eapol_if_message_type_u64_t                                = 22,
    wlan_eapol_if_message_type_variable_data                        = 23,
    };


enum wlan_eapol_if_message_type_function_e
    {
    wlan_eapol_if_message_type_function_none                                        = 0,
    wlan_eapol_if_message_type_function_check_pmksa_cache                           = 1,
    wlan_eapol_if_message_type_function_start_authentication                        = 2,
    wlan_eapol_if_message_type_function_complete_association                        = 3,
    wlan_eapol_if_message_type_function_disassociation                              = 4,
    wlan_eapol_if_message_type_function_start_preauthentication                     = 5,
    wlan_eapol_if_message_type_function_start_reassociation                         = 6,
    wlan_eapol_if_message_type_function_complete_reassociation                      = 7,
    wlan_eapol_if_message_type_function_start_wpx_fast_roam_reassociation           = 8,
    wlan_eapol_if_message_type_function_complete_wpx_fast_roam_reassociation        = 9,
    wlan_eapol_if_message_type_function_packet_process                              = 10,
    wlan_eapol_if_message_type_function_tkip_mic_failure                            = 11,
    wlan_eapol_if_message_type_function_eap_acknowledge                             = 12,
    wlan_eapol_if_message_type_function_update_header_offset                        = 13,
    wlan_eapol_if_message_type_function_complete_check_pmksa_cache                  = 14,
    wlan_eapol_if_message_type_function_packet_send                                 = 15,
    wlan_eapol_if_message_type_function_associate                                   = 16,
    wlan_eapol_if_message_type_function_disassociate                                = 17,
    wlan_eapol_if_message_type_function_packet_data_session_key                     = 18,
    wlan_eapol_if_message_type_function_state_notification                          = 19,
    wlan_eapol_if_message_type_function_reassociate                                 = 20,
    wlan_eapol_if_message_type_function_update_wlan_database_reference_values       = 21,
    wlan_eapol_if_message_type_function_complete_start_wpx_fast_roam_reassociation  = 22,
    wlan_eapol_if_message_type_function_new_protected_setup_credentials             = 23,
    wlan_eapol_if_message_type_function_complete_disassociation                     = 37
    };



static const u32_t WLAN_EAPOL_MESSAGE_IF_TYPE_OFFSET   = 0;
static const u32_t WLAN_EAPOL_MESSAGE_IF_LENGTH_OFFSET = 4;
static const u32_t WLAN_EAPOL_MESSAGE_IF_DATA_OFFSET   = 8;



class network_id_c
    {
public:
    
    /**
     * Constructor
     */
    network_id_c(
        u8_t * source, u32_t source_length,
        u8_t * destination, u32_t destination_length,
        u16_t packet_type ) 
        : source_ptr( source )
        , source_length( source_length )
        , destination_ptr( destination )
        , destination_length( destination_length )
        , packet_type( packet_type )
        , source_m( ZERO_MAC_ADDR )
        , destination_m( ZERO_MAC_ADDR )
        {
        }

    /** 
     * Destructor.
     */
    ~network_id_c()
        {
        source_ptr = NULL;
        destination_ptr = NULL;
        }

    const core_mac_address_s& source()
        {
        if( source_ptr )
            {
            core_tools_c::copy(
                &source_m.addr[0],
                source_ptr,
                MAC_ADDR_LEN );
            }

        return source_m;
        }

    const core_mac_address_s& destination()
        {
        if( destination_ptr )
            {
            core_tools_c::copy(
                &destination_m.addr[0],
                destination_ptr,
                MAC_ADDR_LEN );
            }

        return destination_m;
        }

public: // data

    u8_t * source_ptr;
    u32_t source_length;
    u8_t * destination_ptr;
    u32_t destination_length;
    u16_t packet_type;
    core_mac_address_s source_m;
    core_mac_address_s destination_m;
    };





class session_key_c
    {
public:
    
    /**
     * Constructor
     */
    session_key_c(
        u8_t * key, u32_t key_length,
        u8_t * sequence_number, u32_t sequence_number_length,
        wlan_eapol_if_eapol_key_type_e eapol_key_type,
        u32_t key_index,
        bool_t key_tx_bit )
        : key( key )
        , key_length( key_length )
        , sequence_number( sequence_number )
        , sequence_number_length( sequence_number_length )
        , eapol_key_type( eapol_key_type )
        , key_index( key_index )
        , key_tx_bit( key_tx_bit )
        { }
    
    /** 
     * Destructor.
     */
    ~session_key_c()
        {
        key = NULL;
        sequence_number = NULL;
        }

public: // data

    u8_t * key;
    u32_t key_length;
    
    u8_t * sequence_number;
    u32_t sequence_number_length;
    
    wlan_eapol_if_eapol_key_type_e eapol_key_type;
    u32_t key_index;
    bool_t key_tx_bit;
    };





class state_notification_c
    {
public:

    /**
     * Constructor
     */
    state_notification_c(
        const network_id_c * network_id,
        wlan_eapol_if_eap_protocol_layer_e protocol_layer,
        u32_t protocol,
        u32_t eap_type_vendor_id,
        u32_t eap_type_vendor_type,
        u32_t current_state,
        bool_t is_client,
        wlan_eapol_if_eap_status_e authentication_error )
        : network_id( *network_id )
        , protocol_layer( protocol_layer )
        , protocol( protocol )
        , eap_type_vendor_id( eap_type_vendor_id )
        , eap_type_vendor_type( eap_type_vendor_type )
        , current_state( current_state )
        , is_client( is_client )
        , authentication_error( authentication_error )
        { }
    
    /** 
     * Destructor.
     */
    ~state_notification_c()
        { }

public: // data

    network_id_c network_id;
    wlan_eapol_if_eap_protocol_layer_e protocol_layer;
    u32_t protocol;
    u32_t eap_type_vendor_id;
    u32_t eap_type_vendor_type;
    u32_t current_state;
    bool_t is_client;
    wlan_eapol_if_eap_status_e authentication_error;
    
    };


class network_key_c
    {
public:
    
    /**
     * Constructor
     */
    network_key_c(
        u8_t network_key_index,
        u8_t * network_key,
        u32_t network_key_length )
        : network_key_index( network_key_index )
        , network_key( network_key )
        , network_key_length( network_key_length )
        { }
    
    /** 
     * Destructor.
     */
    ~network_key_c()
        {
        network_key = NULL;
        }

public: // data

    u8_t network_key_index;
    u8_t * network_key;
    u32_t network_key_length;
    
    };


class protected_setup_credential_c
    {
public:
    
    /**
     * Constructor
     */
    protected_setup_credential_c(
        u8_t network_index,
        u8_t * ssid,
        u32_t ssid_length,
        u16_t authentication_type,
        u16_t encryption_type,
        u8_t * mac_address,
        u32_t mac_address_length )
        : network_index( network_index )
        , ssid( ssid )
        , ssid_length( ssid_length )
        , authentication_type( authentication_type )
        , encryption_type( encryption_type )
        , network_key_list( )
        , mac_address( mac_address )
        , mac_address_length( mac_address_length )
        { }
    
    /** 
     * Destructor.
     */
    ~protected_setup_credential_c()
        {
        ssid = NULL;
        mac_address = NULL;
        }

public: // data

    u8_t network_index;
    u8_t * ssid;
    u32_t ssid_length;
    u16_t authentication_type;
    u16_t encryption_type;
    core_type_list_c<network_key_c> network_key_list;
    u8_t * mac_address;
    u32_t mac_address_length;
    
    };








//NONSHARABLE_CLASS( core_wlan_eapol_if_parameter_c )

class core_wlan_eapol_if_parameter_c
    {
public:

    /**
     * Constructor
     */
    core_wlan_eapol_if_parameter_c();
    
    /**
     * Constructor
     *
     * @param data Pointer to the TLV encoded data.
     * @param data_length Length of the TLV encoded data.
     */
    core_wlan_eapol_if_parameter_c(
        u8_t *data, u32_t data_length );
    
    /** 
     * Destructor.
     */
    ~core_wlan_eapol_if_parameter_c();
    
    /**
     * Update content of parameter
     *
     * @param data Pointer to the TLV encoded data.
     * @param data_length Length of the TLV encoded data.
     */
    void update(
        u8_t *data, u32_t data_length );

    
    // ============================================================================
    // General methods to handle parameters
    
    u32_t size() const;

    wlan_eapol_if_message_type_e get_parameter_type() const;

    u32_t get_parameter_length() const;

    u8_t* get_data() const;
    

    // ============================================================================
    // Parameter specific methods
    
    // All 32 bit base types
    core_error_e set_parameter_data( 
        wlan_eapol_if_message_type_e type, 
        u32_t value );

    // All already generated structures, arrays and variable data
    core_error_e set_parameter_data( 
        wlan_eapol_if_message_type_e type, 
        const u8_t* data, u32_t length );

    // EAP type
    core_error_e set_parameter_data( 
        wlan_eapol_if_message_type_e type, 
        u32_t vendor_id, 
        u32_t vendor_type );

    core_error_e set_parameter_data_boolean( 
        wlan_eapol_if_message_type_e type, 
        bool_t boolean );

    core_error_e set_parameter_data_u16_t( 
        wlan_eapol_if_message_type_e type, 
        u16_t value );

    core_error_e set_parameter_data_u8_t( 
        wlan_eapol_if_message_type_e type, 
        u8_t value );

    core_error_e set_parameter_data_u64_t(
        u64_t data );

    // ============================================================================

    // All 32 bit values
    core_error_e get_parameter_data(
        u32_t * value ) const;
    
    // 
    core_error_e get_parameter_data(
        u64_t * value ) const;

    // 
    core_error_e get_parameter_data(
        bool_t * value ) const;

    core_error_e get_parameter_data(
        u16_t * value ) const;

    core_error_e get_parameter_data(
        u8_t * value ) const;

    core_error_e get_parameter_data(
        u8_t ** const data, u32_t * data_length ) const;

    core_error_e get_parameter_data(
        u32_t * vendor_id, 
        u32_t * vendor_type ) const;


// ============================================================================

private:

    core_error_e reserve_buffer(
        u32_t needed_buffer_length  );

    void add_header(
        wlan_eapol_if_message_type_e type, 
        u32_t length );

    u64_t get_parameter_u64_t() const;

    void add_parameter_u32_t(
        u32_t data );

    u32_t get_parameter_u32_t() const;

    void add_parameter_u16_t(
        u16_t data );

    u16_t get_parameter_u16_t() const;

    void add_parameter_u8_t(
        u8_t data );

    u8_t get_parameter_u8_t() const;

private: // Data

    u8_t * data_m;           // This is pointer to current buffer
    u32_t buffer_length_m;
    bool_t delete_buffer_m;
    
    };










class core_wlan_eapol_if_function_c
    {
public:

    /**
     * Constructor
     */
    core_wlan_eapol_if_function_c();

    /**
     * Constructor
     *
     * @param data Pointer to the IE data.
     * @param data_length Length of the IE data.
     */
    core_wlan_eapol_if_function_c(
        u8_t * data,
        const u32_t length );
    
    /** 
     * Destructor.
     */
    ~core_wlan_eapol_if_function_c();
    
    
    wlan_eapol_if_message_type_e get_type();

    wlan_eapol_if_message_type_function_e get_function();
    
    core_error_e append(
        const core_wlan_eapol_if_parameter_c * const param );

    u32_t size() const;

    u8_t* get_data();

    void clear();
    
    // ============================================================================
    
    // Iterators
    void first();
    
    void next();

    const core_wlan_eapol_if_parameter_c* current();
    
    bool_t is_done() const;

    // ============================================================================
    // Methods to generate and parse structured datatypes
    
    core_error_e reserve_buffer(
        u32_t needed_buffer_length );

    // Higher level methods to handle structured parameters and functions
    
    core_error_e generate_network_id(
        const network_id_c * const network_id );
    
    core_error_e parse_network_id(
        network_id_c * network_id );

    core_error_e generate_network_key(
        network_key_c * network_key );
    
    core_error_e parse_network_key(
        network_key_c * network_key );

    core_error_e generate_protected_setup_credential(
        protected_setup_credential_c * credential );
    
    core_error_e parse_protected_setup_credential(
        protected_setup_credential_c * credential );

    core_error_e generate_session_key(
        session_key_c * session_key );

    core_error_e parse_session_key(
        session_key_c * session_key );

    core_error_e generate_eap_state_notification(
        state_notification_c * state_notification );

    core_error_e parse_eap_state_notification(
        state_notification_c * state_notification );


    // ============================================================================
    // Methods to generate and parse functions

    core_error_e check_pmksa_cache(
        core_type_list_c<network_id_c> & network_id_list,
        const wlan_eapol_if_eapol_key_authentication_type_e selected_eapol_key_authentication_type,
        const wlan_eapol_if_rsna_cipher_e pairwise_key_cipher_suite,
        const wlan_eapol_if_rsna_cipher_e group_key_cipher_suite );

    core_error_e start_authentication(
        u8_t * ssid, const u32_t ssid_length, 
        const wlan_eapol_if_eapol_key_authentication_type_e selected_eapol_key_authentication_type,
        u8_t * wpa_psk, const u32_t wpa_psk_length, 
        const bool_t wpa_override_enabled,
        const network_id_c * receive_network_id );

    core_error_e complete_association(
        const wlan_eapol_if_eapol_wlan_authentication_state_e association_result,
        const network_id_c * network_id,
        u8_t * received_wpa_ie, const u32_t received_wpa_ie_length,
        u8_t * sent_wpa_ie, const u32_t sent_wpa_ie_length,
        const wlan_eapol_if_rsna_cipher_e pairwise_key_cipher_suite,
        const wlan_eapol_if_rsna_cipher_e group_key_cipher_suite );
    
    core_error_e disassociation(
        const network_id_c * network_id );

    core_error_e start_preauthentication(
        const network_id_c * network_id );

    core_error_e start_reassociation(
        const network_id_c * old_network_id,
        const network_id_c * new_network_id,
        const wlan_eapol_if_eapol_key_authentication_type_e selected_eapol_key_authentication_type );
        
    core_error_e complete_reassociation(
        const wlan_eapol_if_eapol_wlan_authentication_state_e association_result,
        const network_id_c * network_id,
        u8_t * received_wpa_ie, const u32_t received_wpa_ie_length,
        u8_t * sent_wpa_ie, const u32_t sent_wpa_ie_length,
        const wlan_eapol_if_rsna_cipher_e pairwise_key_cipher_suite,
        const wlan_eapol_if_rsna_cipher_e group_key_cipher_suite );
    
    core_error_e start_wpx_fast_roam_reassociation(
        const network_id_c * old_network_id,
        const network_id_c * new_network_id,
        u8_t * reassociation_request_ie,
        const u32_t reassociation_request_ie_length,
        const u8_t* received_wpa_ie,
        u32_t received_wpa_ie_length,
        const u8_t* sent_wpa_ie,
        u32_t sent_wpa_ie_length );

    core_error_e complete_wpx_fast_roam_reassociation(
        const wlan_eapol_if_eapol_wlan_authentication_state_e reassociation_result,
        const network_id_c * network_id,
        u8_t * received_reassociation_ie, const u32_t received_reassociation_ie_length );

    core_error_e packet_process(
        const network_id_c * network_id,
        const u8_t * packet_data, const u32_t packet_data_length );

    core_error_e tkip_mic_failure(
        const network_id_c * network_id,
        const bool_t is_fatal_failure, 
        const wlan_eapol_if_eapol_tkip_mic_failure_type_e tkip_mic_failure_type );

    core_error_e eap_acknowledge(
        const network_id_c * network_id );

    core_error_e update_header_offset(
        const u32_t header_offset,
        const u32_t MTU,
        const u32_t trailer_length );


    
    core_error_e generate_complete_check_pmksa_cache(
        core_type_list_c<network_id_c> & network_id_list );
    
    core_error_e parse_complete_check_pmksa_cache(
        core_type_list_c<network_id_c> & network_id_list );
    
    core_error_e generate_packet_send(
        network_id_c * send_network_id,
        u8_t * packet_data, const u32_t packet_data_length );

    core_error_e parse_packet_send(
        network_id_c * send_network_id,
        u8_t ** packet_data, u32_t * packet_data_length );

    core_error_e generate_associate(
        const wlan_eapol_if_eapol_key_authentication_mode_e authentication_mode );

    core_error_e parse_associate(
        wlan_eapol_if_eapol_key_authentication_mode_e * authentication_mode );
    
    core_error_e generate_disassociate(
        network_id_c * receive_network_id,
        const bool_t self_disassociation );

    core_error_e parse_disassociate(
        network_id_c * receive_network_id,
        bool_t * self_disassociation );

    core_error_e generate_packet_data_session_key(
        network_id_c * send_network_id,
        session_key_c * session_key );

    core_error_e parse_packet_data_session_key(
        network_id_c * send_network_id,
        session_key_c * session_key );

    core_error_e generate_state_notification(
        state_notification_c * state_notification );

    core_error_e parse_state_notification(
        state_notification_c * state_notification );

    core_error_e generate_reassociate(
        network_id_c * send_network_id,
        wlan_eapol_if_eapol_key_authentication_type_e authentication_type,
        u8_t * pmkid,
        u32_t pmkid_length );

    core_error_e parse_reassociate(
        network_id_c * send_network_id,
        wlan_eapol_if_eapol_key_authentication_type_e * authentication_type,
        u8_t ** pmkid,
        u32_t * pmkid_length );

    core_error_e update_wlan_database_reference_values(
        u8_t * database_reference_value, const u32_t database_reference_value_length );

    core_error_e generate_complete_start_wpx_fast_roam_reassociation(
        network_id_c * receive_network_id,
        u8_t * reassociation_request_ie,
        u32_t reassociation_request_ie_length );

    core_error_e parse_complete_start_wpx_fast_roam_reassociation(
        network_id_c * receive_network_id,
        u8_t ** reassociation_request_ie,
        u32_t * reassociation_request_ie_length );

    core_error_e generate_error(
        wlan_eapol_if_error_e errorcode,
        wlan_eapol_if_message_type_function_e function );

    core_error_e parse_error(
        wlan_eapol_if_error_e * errorcode,
        wlan_eapol_if_message_type_function_e * function );

    core_error_e generate_new_protected_setup_credentials(
        core_type_list_c< protected_setup_credential_c > & credential_list );

    core_error_e parse_new_protected_setup_credentials(
        core_type_list_c< protected_setup_credential_c > & credential_list );

    core_error_e parse_complete_disassociation(
        network_id_c * receive_network_id );

    void debug_print();
    
    void debug_print_type_string(
        wlan_eapol_if_message_type_e type, 
        u32_t length );

    void debug_print_function_string(
        wlan_eapol_if_message_type_function_e function );

private: // Data

    u8_t * data_m;           // This is pointer to current buffer
    u32_t data_length_m;
    u32_t buffer_length_m;
    bool_t delete_buffer_m;

    u32_t iter_m;
    core_wlan_eapol_if_parameter_c current_parameter_m;
    
    };


#endif // CORE_WLAN_EAPOL_IF_MESSAGE_H
