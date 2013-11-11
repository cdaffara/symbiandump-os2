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




#ifndef CORE_WLAN_EAPOL_INTERFACE_H
#define CORE_WLAN_EAPOL_INTERFACE_H

// INCLUDES
#include "am_platform_libraries.h"
#include "core_wlan_eapol_if_message.h"
#include "abs_core_server_callback.h"
#include "core_frame_ethernet.h"

// CLASS DECLARATION
class core_wlan_eapol_interface_c
{
public:

    core_wlan_eapol_interface_c(
        abs_core_server_callback_c& wlm_callback );
    
    ~core_wlan_eapol_interface_c();
	
    static core_error_e convert_if_status_to_core_error( const u32_t if_status );

    core_error_e load_eapol(
        core_eapol_operating_mode_e mode,
        abs_wlan_eapol_callback_c* const eapol_handler );
    
    core_error_e shutdown();
    
    core_error_e configure();


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
        const network_id_c * network_id );

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

    /**
     * Send the given frame to EAPOL for processing.
     *
     * @param frame Frame to be sent for processing
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e process_frame(
        const core_frame_ethernet_c& frame );

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

    core_error_e update_wlan_database_reference_values(
        u8_t * database_reference_value, const u32_t database_reference_value_length );

    core_error_e update_completed_check_pmksa_cache_list(
        core_type_list_c<network_id_c> & network_id_list );

    core_type_list_c<core_mac_address_s> & get_completed_check_pmksa_cache_list();

    /**
     * Store a frame for later use.
     *
     * @param frame Ethernet frame to be stored.
     * @note This method will take a copy of the frame.
     */
    core_error_e store_frame(
        const core_frame_ethernet_c& frame );

    /**
     * Delete the stored frame.
     */
    void clear_stored_frame();

    /**
     * Process the stored frame.
     *
     * If a frame has been stored using store_frame(), this method
     * will send the frame to EAPOL for processing.     
     * @return core_error_ok if success, an error code otherwise.
     */
    core_error_e process_stored_frame();

    /**
     * Return current EAPOL operating mode.
     * 
     * @return Current EAPOL operating mode.
     */ 
    core_eapol_operating_mode_e operating_mode() const;

    /**
     * Return current EAPOL ethernet type.
     * 
     * @return Current EAPOL ethernet type.
     */
    wlan_eapol_ethernet_type_e ethernet_type() const;

    /**
     * Return current EAPOL authentication type.
     * 
     * @return Current EAPOL authentication type.
     */
    wlan_eapol_if_eapol_key_authentication_type_e authentication_type() const;
    
private: // Data

    abs_core_server_callback_c& wlm_callback_m;
    core_wlan_eapol_if_function_c function;
    
    core_type_list_c< core_mac_address_s > completed_check_pmksa_cache_list;

    /** Frame stored for later use. */
    core_frame_ethernet_c* stored_frame_m;

    /** Current EAPOL operating mode. */
    core_eapol_operating_mode_e eapol_mode_m;

    /** Currently used EAPOL authentication type. */
    wlan_eapol_if_eapol_key_authentication_type_e authentication_type_m;

	//--------------------------------------------------
}; // class core_wlan_eapol_interface_c

#endif //#ifndef CORE_WLAN_EAPOL_INTERFACE_H

//--------------------------------------------------

// End of file
