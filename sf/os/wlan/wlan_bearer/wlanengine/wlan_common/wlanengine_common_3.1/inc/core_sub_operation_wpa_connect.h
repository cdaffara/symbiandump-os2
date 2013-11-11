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
* Description:  Statemachine for connecting to a WPA network
*
*/

/*
* %version: 29 %
*/

#ifndef CORE_SUB_OPERATION_WPA_CONNECT_H
#define CORE_SUB_OPERATION_WPA_CONNECT_H

#include "core_operation_base.h"
#include "core_ap_data.h"
#include "abs_core_event_handler.h"
#include "core_type_list.h"
#include "abs_wlan_eapol_callback_interface.h"

/** The maximum length of a PMKID. */
const u32_t CORE_EAPOL_PMKID_MAX_LENGTH  = 256;

class core_server_c;
class abs_core_driverif_c;
class core_frame_dot11_ie_c;
class core_frame_assoc_resp_c;

/**
 * Statemachine for connecting to a WPA network
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_sub_operation_wpa_connect_c ) :
    public core_operation_base_c,
    public abs_wlan_eapol_callback_interface_c,
    public abs_core_event_handler_c
    {

public:

    /**
     * The possible states of the state machine.
     */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_start_authentication_needed,
        core_state_do_connect,
        core_state_req_connect,
        core_state_req_connect_failed,
        core_state_req_association_failed,
        core_state_process_eapol_frame,
        core_state_req_state_notification,
        core_state_bss_lost,
        core_state_user_cancel,
        core_state_user_cancel_disassociated,
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_sub_operation_wpa_connect_c(
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
        core_frame_assoc_resp_c** assoc_resp );

    /**
     * Destructor.
     */
    virtual ~core_sub_operation_wpa_connect_c();

protected:

    /**
     * This method is called when a pending request has been completed
     * and so sub-operations are pending.
     *
     * @since S60 v3.1
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e next_state();

    /**
     * This method is called when the operation needs to be canceled.     
     *
     * @since S60 v3.1
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e cancel();

    /**
     * This method is called when the operation is cancelled from the outside.
     *
     * @since S60 v3.2
     * @param do_graceful_cancel Whether cancel should be graceful or forced.
     */
    void user_cancel(
        bool_t do_graceful_cancel );

// from base class abs_wlan_eapol_callback_interface_c  

    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    core_error_e packet_send(
        network_id_c * send_network_id,
        u8_t * packet_data,
        u32_t packet_data_length,
        bool_t send_unencrypted );

    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    core_error_e associate(
        wlan_eapol_if_eapol_key_authentication_mode_e authentication_mode );
		
    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    core_error_e disassociate(
        network_id_c * receive_network_id,
	    const bool_t self_disassociation );
		  
    /**
     * From abs_wlan_eapol_callback_interface_c
     */        
    core_error_e packet_data_session_key(
        network_id_c * send_network_id,
        session_key_c * key );

    /**
     * From abs_wlan_eapol_callback_interface_c
     */        
    void state_notification(
        state_notification_c * state );

    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    core_error_e reassociate(
        network_id_c * send_network_id,
        const wlan_eapol_if_eapol_key_authentication_type_e authentication_type,
        u8_t * PMKID,
        u32_t PMKID_length );

    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    core_error_e complete_check_pmksa_cache(
        core_type_list_c<network_id_c> & network_id_list );

    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    core_error_e complete_start_wpx_fast_roam_reassociation(
        network_id_c * receive_network_id,
        u8_t * reassociation_request_ie,
        u32_t reassociation_request_ie_length );

    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    core_error_e new_protected_setup_credentials(
        core_type_list_c< protected_setup_credential_c > & credential_list );

    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    core_error_e complete_disassociation(
        network_id_c * receive_network_id );
    
    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    void handle_error(
        wlan_eapol_if_error_e errorcode,
        wlan_eapol_if_message_type_function_e function );

// from base class abs_core_event_handler_c  

    bool_t notify(
        core_am_indication_e indication );

private: // data

    /**
     * Whether we are still connected to an AP.
     */
    bool_t& is_connected_m;
   
    /**
     * Whether the connection attempt uses a cached security association.
     */
    bool_t& is_cached_sa_used_m;
   
    /**
     * Status of connection attempt.
     */
    core_management_status_e& connect_status_m;

    /**
     * SSID to connect to.
     */
    const core_ssid_s& ssid_m;
    
    /**
     * AP to connect to.
     */
    core_ap_data_c& ap_data_m;

    /**
     * BSSID currently tried.
     */
    core_mac_address_s current_bssid_m;

    /**
     * Length of PMKID data.
     */
    u16_t pmkid_length_m;

    /**
     * PMKID data.
     */
    u8_t pmkid_data_m[CORE_EAPOL_PMKID_MAX_LENGTH];

    /**
     * Sent WPA/RSN/WAPI IE data.
     */
    core_frame_dot11_ie_c* sent_ie_m;

    /**
     * Authentication algorithm used for the connection.
     */
    u16_t auth_algorithm_m;
    
    /**
     * Authentication type for EAPOL.
     */
    wlan_eapol_if_eapol_key_authentication_type_e eapol_auth_type_m;

    /**
     * List of IEs that will be added to the (re-)association request.
     */
    core_type_list_c<core_frame_dot11_ie_c>& assoc_ie_list_m;

    /**
     * The (re-)association response frame is stored here if received.
     * Not owned by this class.
     */
    core_frame_assoc_resp_c** assoc_resp_m;

    /**
     * Whether the connection attempt uses a proactive key caching.
     */
    bool_t is_key_caching_used_m;

    /**
     * Pairwise key to set before association. CIPHER_KEY_NOT_DEFINED if none.
     */
    core_cipher_key_s pairwise_key_m;

    };

#endif // CORE_SUB_OPERATION_WPA_CONNECT_H
