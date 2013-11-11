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


#ifndef ABS_WLAN_EAPOL_CALLBACK_INTERFACE_H
#define ABS_WLAN_EAPOL_CALLBACK_INTERFACE_H

#include "core_type_list.h"
#include "core_wlan_eapol_if_message.h"


/// The abs_wlan_eapol_callback_interface_c class defines the interface the EAPOL
/// will use with the partner class.
class abs_wlan_eapol_callback_interface_c
{
public:
	//--------------------------------------------------

	// Destructor
	virtual ~abs_wlan_eapol_callback_interface_c()
	{
	}

	// Constructor
	abs_wlan_eapol_callback_interface_c()
	{
	}

    /** 
     * EAPOL could send packets to partner class with this function.
     */
	virtual core_error_e packet_send(
        network_id_c * send_network_id,
        u8_t * packet_data,
        u32_t packet_data_length,
        bool_t send_unencrypted ) = 0;

	/**
	 * This function call tells lower layer to associate with the selected 802.11 authentication mode.
	 */
	virtual core_error_e associate(
        wlan_eapol_if_eapol_key_authentication_mode_e authentication_mode ) = 0;

	/**
	 * Lower layer must return value of self_disassociation when it calls disassociation().
	 * This tells the cause of disassociation.
	 */
	virtual core_error_e disassociate(
        network_id_c * receive_network_id,
		const bool_t self_disassociation) = 0;

	/**
	 * The packet_data_session_key() function passes one traffic encryption key to 
	 * the lower layers. Ultimately the key can end up to the WLAN hardware.
	 */
	virtual core_error_e packet_data_session_key(
        network_id_c * send_network_id,
        session_key_c * key
		) = 0;

	/**
	 * This is notification of internal state transition.
	 * This is used for notifications, debugging and protocol testing.
	 * The primal notifications are wlan_eapol_if_eapol_wlan_authentication_state_authentication_successfull
	 * and wlan_eapol_if_eap_state_authentication_terminated_unsuccessfully.
	 * These two notifications are sent from EAP-protocol layer (wlan_eapol_if_eap_protocol_layer_eap).
	 */
	virtual void state_notification(
		state_notification_c * state ) = 0;

	/**
	 * This function call tells lower layer to re-associate with the selected network ID,
	 * authentication type and PMKID.
	 */
	virtual core_error_e reassociate(
        network_id_c * send_network_id,
        const wlan_eapol_if_eapol_key_authentication_type_e authentication_type,
        u8_t * PMKID,
        u32_t PMKID_length ) = 0;

    /**
     * The complete_check_pmksa_cache() function completes check_pmksa_cache query.
     */
    virtual core_error_e complete_check_pmksa_cache(
        core_type_list_c<network_id_c> & network_id_list ) = 0;

    /**
     * The complete_start_wpx_fast_roam_reassociation() function completes start_wpx_fast_roam_reassociation function.
     */
    virtual core_error_e complete_start_wpx_fast_roam_reassociation(
        network_id_c * receive_network_id,
        u8_t * reassociation_request_ie,
        u32_t reassociation_request_ie_length ) = 0;

    /**
     * The new_protected_setup_credentials() function gives credentials got from 
     * Protected Setup negotiation.
     */
    virtual core_error_e new_protected_setup_credentials(
        core_type_list_c< protected_setup_credential_c > & credential_list ) = 0;

    /**
     * The complete_disassociation() function completes disassociation function.
     */
    virtual core_error_e complete_disassociation(
        network_id_c * receive_network_id ) = 0;

    /**
     * The handle_error() function tells about error from EAPOL side.
     */
    virtual void handle_error(
        wlan_eapol_if_error_e errorcode,
        wlan_eapol_if_message_type_function_e function ) = 0;

	//--------------------------------------------------
}; // class abs_wlan_eapol_callback_interface_c

#endif //#ifndef ABS_WLAN_EAPOL_CALLBACK_INTERFACE_H

//--------------------------------------------------


// End.
