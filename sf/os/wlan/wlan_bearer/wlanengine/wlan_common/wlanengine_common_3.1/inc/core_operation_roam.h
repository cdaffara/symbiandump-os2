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
* Description:  State machine for roaming
*
*/

/*
* %version: 26 %
*/

#ifndef CORE_OPERATION_ROAM_H
#define CORE_OPERATION_ROAM_H

#include "core_operation_base.h"
#include "core_type_list.h"
#include "core_ap_data.h"
#include "core_frame_dot11_ie.h"

class core_frame_assoc_resp_c;

/**
 * State machine for roaming between APs.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_roam_c ) : public core_operation_base_c
    {

public:

    /**
     * Data structure for a roaming list entry.
     */
    struct roaming_list_entry_s
        {
        core_ap_data_c* ap_data;
        bool_t is_cached_sa_available;
        };

    /**
     * The possible states of the state machine.
     */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_req_disable_userdata_before_connect,
        core_state_req_connect,
        core_state_req_update_ts,
        core_state_req_enable_userdata,
        core_state_fail_connect,
        core_state_fail_set_tx_power,
        core_state_MAX
        };

    /**
     * Constructor
     *
     * @param request_id An unique identification for the request.
     * @param server Handle to core server.
     * @param drivers Handle to low-level management functionality.
     * @param adaptation Handle upper-level management functionality.
     * @param is_connected Whether the terminal is currently connected to a BSS.
     * @param tag Tag used for iterating scan results for a suitable AP.
     * @param min_required_rcpi Minimum RCPI value required for connection.
     * @param min_medium_time Minimum amount of free medium time required for connection. 
     * @param ssid SSID used for connection.
     * @param bssid BSSID used for connection, BROADCAST_MAC_ADDR if any.
     */
    core_operation_roam_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        bool_t& is_connected,
        u8_t tag,
        u8_t min_required_rcpi,
        const medium_time_s& min_medium_time,
        const core_ssid_s& ssid,
        const core_mac_address_s& bssid );

    /**
     * Destructor.
     */
    virtual ~core_operation_roam_c();

protected:

    /**
     * This method is called when a pending request has been completed
     * and so sub-operations are pending.
     *
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e next_state();
    
    /**
     * This method is called when the operation needs to be canceled.     
     *
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e cancel();

private:

    /**
     * Check whether a cached security association is available for the given AP.
     *
     * @param ap_data Information about the AP.
     * @return true_t if a security association exists, false_t otherwise.
     */
    bool_t is_security_association_available(
        const core_ap_data_c& ap_data );

    /**
     * Determine the connection status based on the status information.
     *
     * @param request_status The request completion status.
     * @param management_status The association response status from the AP.
     * @return The connection status based on the status information.
     */
    core_connect_status_e connect_status(
        core_error_e request_status,
        core_management_status_e management_status );

    /**
     * Decide whether the connection failure is fatal and the AP should
     * be blacklisted.
     *
     * @param request_status The request completion status.
     * @param management_status The association response status from the AP.
     * @param is_reassociation Whether the connection attempt was a reassociation.
     * @return core_ap_blacklist_reason_none if no blacklisting required,
     *         the reason otherwise.
     */
    core_ap_blacklist_reason_e is_fatal_failure(
        core_error_e request_status,
        core_management_status_e management_status,
        bool_t is_reassociation ) const;

    /**
     * Determine the connection status based on the EAP information.
     *
     * @param security_mode The used security mode.
     * @param eap_type EAP type used
     * @param eap_error EAP-specific error code.
     * @return The connection status based on the EAP information.
     */
    core_connect_status_e eap_connect_status(
        core_security_mode_e security_mode,
        u32_t eap_type,
        u32_t eap_error ) const;

    /**
     * Process the frame and add it to the roaming list if suitable for connecting.
     *
     * @param ap_data Information about the AP.
     * @return true_t if the AP was added to the roaming list, false_t otherwise.
     */
    bool_t process_frame(
        core_ap_data_c& ap_data );

    /**
     * Update roam metrics based on the connection failure reason.
     *
     * @param request_status The request completion status.
     * @param management_status The association response status from the AP.
     */
    void update_roam_failure_count(
        core_error_e request_status,
        core_management_status_e management_status );

    /**
     * Create and send neighbor report request.
     */
    void handle_neighbor_request();

private: // data

    /** Whether we are still connected to an AP. */
    bool_t& is_connected_m;

    /** The scan tag used for iterating suitable APs. */
    u8_t tag_m;

    /** The minimum RCPI value required. */
    u8_t min_required_rcpi_m;

    /** The minimum amount of free medium time required. */
    const medium_time_s min_medium_time_m;

    /** SSID to use for connection. */
    const core_ssid_s ssid_m;
    
    /** BSSID to use for connection. */
    const core_mac_address_s bssid_m;
    
    /** List of suitable APs. */
    core_type_list_c<roaming_list_entry_s> roaming_list_m;

    /** Status of connection attempt. */
    core_management_status_e management_status_m;

    /** Connect status for the current connection attempt. */
    core_connect_status_e connect_status_m;
    
    /** The AP currently tried. Not owned by this pointer. */
    core_ap_data_c* current_ap_m;

    /** BSSID currently connected to. */
    core_mac_address_s current_bssid_m;

    /** The list of IEs that will be added to the association/reassociation request */
    core_type_list_c<core_frame_dot11_ie_c> assoc_ie_list_m;

    /** Whether the connection attempt uses a cached security association. */
    bool_t is_cached_sa_used_m;

    /** The (re-)association response frame is stored here if received. */
    core_frame_assoc_resp_c* assoc_resp_m;

    };

#endif // CORE_OPERATION_ROAM_H
