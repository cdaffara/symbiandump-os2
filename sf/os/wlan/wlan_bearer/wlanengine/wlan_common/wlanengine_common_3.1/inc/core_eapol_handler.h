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


#ifndef CORE_EAPOL_HANDLER_H
#define CORE_EAPOL_HANDLER_H

#include "abs_wlan_eapol_callback_interface.h"
#include "abs_wlan_eapol_callback.h"
#include "am_platform_libraries.h"
#include "core_types.h"

class core_server_c;
class abs_core_driverif_c;
class abs_core_server_callback_c;
class abs_core_protected_setup_handler_c;
class network_id_c;

/**
 * This class acts as handler to EAPOL callbacks.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_eapol_handler_c ) :
    public abs_wlan_eapol_callback_interface_c,
    public abs_wlan_eapol_callback_c
    {

public:

    /**
     * constructor
     *
     * @since S60 v3.1
     * @param server pointer to core_server
     * @param drivers pointer to lower adaptation
     * @param adaptation pointer to upper adaptation
     */
    core_eapol_handler_c(
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation );

    /**
     * destructor
     */
    virtual ~core_eapol_handler_c();

    /**
     * eapol_handler
     * @since S60 v3.1
     * @return handler for EAPOL callbacks.
     */
    abs_wlan_eapol_callback_interface_c* eapol_handler();

    /**
     * Function to convert core_error_e to interface status
     * @since S60 v3.2
     * @param core_error
     * @return interface status
     */
    static u32_t convert_core_error_to_if_status(
        core_error_e core_error );

    /**
     * Direct the callbacks from EAPOL to the given handler.
     *
     * @since S60 v3.1
     * @param New handler of EAPOL callbacks.
     */   
    void set_eapol_handler(
        abs_wlan_eapol_callback_interface_c* handler );

    /**
     * Direct the Protected Setup events to the given handler.
     *
     * @since S60 v3.2
     * @param New handler of Protected Setup events.
     */  
    void set_protected_setup_handler(
        abs_core_protected_setup_handler_c* handler );

// from base class abs_wlan_eapol_callback_interface_c  

    /**
     * packet_send sends a frame given by EAPOL
     *
     * @since S60 v3.1
     * @param send_network_id
     * @param packet_data contains the data of the frame
     * @param packet_data_length contains the length of the data
     * @param send_unencrypted Whether the frame must be sent unencrypted.
     * @return status value
     */
    core_error_e packet_send(
        network_id_c * send_network_id,
        u8_t * packet_data,
        u32_t packet_data_length,
        bool_t send_unencrypted );

    /**
     * associate
     *
     * @since S60 v3.1
     * @param authentication_mode
     * @return status value
     */
    core_error_e associate(
        wlan_eapol_if_eapol_key_authentication_mode_e authentication_mode );
		
    /**
     * disassociate
     *
     * @since S60 v3.1
     * @param receive_network_id
     * @param self_disassociation
     * @return status value
     */
    core_error_e disassociate(
        network_id_c * receive_network_id,
	    const bool_t self_disassociation );
		  
    /**
     * packet_data_session_key
     *
     * @since S60 v3.1
     * @param send_network_id
     * @param key
     * @return status value
     */        
    core_error_e packet_data_session_key(
        network_id_c * send_network_id,
        session_key_c * key );

    /**
     * state_notification
     *
     * @since S60 v3.1
     * @param state
     */        
    void state_notification(
        state_notification_c * state );

    /**
     * reassociate
     *
     * @since S60 v3.1
     * @param send_network_id
     * @param authentication_type
     * @param PMKID
     * @param PMKID_length
     * @return status value
     */
    virtual core_error_e reassociate(
        network_id_c * send_network_id,
        const wlan_eapol_if_eapol_key_authentication_type_e authentication_type,
        u8_t * PMKID,
        u32_t PMKID_length );

    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    virtual core_error_e complete_check_pmksa_cache(
        core_type_list_c<network_id_c> & network_id_list );

    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    virtual core_error_e complete_start_wpx_fast_roam_reassociation(
        network_id_c * receive_network_id,
        u8_t * reassociation_request_ie,
        u32_t reassociation_request_ie_length );

    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    virtual core_error_e new_protected_setup_credentials(
        core_type_list_c< protected_setup_credential_c > & credential_list );

    /**
     * From abs_wlan_eapol_callback_interface_c 
     */
    virtual core_error_e complete_disassociation(
        network_id_c * receive_network_id );

    /**
     * From abs_wlan_eapol_callback_interface_c
     */
    virtual void handle_error(
        wlan_eapol_if_error_e errorcode,
        wlan_eapol_if_message_type_function_e function );


// from base class abs_wlan_eapol_callback_c

    /**
     * Callback interface to partner.
     *
     * @since S60 v3.2
     * @param data Pointer to the data to be sent.
     * @param length Length of the data to be sent.
     * @return Return value is specified in interface specification.
     */
    u32_t send_data(
        const void * const data, 
        const u32_t length );

private:

    /**
     * Handle a authentication state notification.
     *
     * @param state Current EAPOL authentication state.
     * @param bssid The BSSID the notification is about.
     */
    void handle_wlan_authentication_state(
        u32_t state,
        const core_mac_address_s& bssid );        

    /**
     * Convert an EAPOL state notification to a core error code.
     *
     * @param state State notification to be converted.
     * @return Core error code.
     */
    core_error_e eapol_wlan_authentication_state_to_error(
        u32_t state ) const;

    /**
     * Check whether the given EAPOL state notification is an error.
     */
    bool_t is_eapol_wlan_authentication_state_failure(
        u32_t state ) const;

    /**
     * Check whether the given EAPOL state notification is a success.
     */
    bool_t is_eapol_wlan_authentication_state_success(
        u32_t state ) const;


private: // data

    /** Handle to the core server. */
    core_server_c* server_m;

    /** Handle to adaptation interface. */
    abs_core_driverif_c* drivers_m;
    
    /** Handle to WLM. */
    abs_core_server_callback_c* adaptation_m;
    
    /** The actual handler of EAPOL callbacks. */
    abs_wlan_eapol_callback_interface_c* handler_m;
    
    /** Handler for protected setup indications. */
    abs_core_protected_setup_handler_c* protected_setup_handler_m;

    };

#endif // CORE_EAPOL_HANDLER_H
