/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State machine for connecting to a WEP network.
*
*/


#ifndef CORE_SUB_OPERATION_WEP_CONNECT_H
#define CORE_SUB_OPERATION_WEP_CONNECT_H

#include "core_operation_base.h"
#include "core_ap_data.h"
#include "core_type_list.h"

/**
 * State machine for connecting to a WEP AP.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_sub_operation_wep_connect_c ) : public core_operation_base_c
    {

public:

    /** The possible states of the state machine. */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_connect,
        core_state_connect_failed,
        core_state_MAX
        };

    /**
     * Constructor.
     *
     * @param request_id An unique identification for the request.
     * @param server Callback to the core server.
     * @param drivers Callback to the lower adaptation.
     * @param adaptation Callback to the upper adaptation.
     * @param is_connected Whether we are still connected to an AP.
     * @param connect_status Status of connection attempt.
     * @param ssid SSID to connect to.
     * @param ap_data AP to connect to.
     * @param auth_mode Authentication mode to use.
     * @param encryption_level Encryption to use.
     * @param assoc_ie_list The list of IEs to be added.
     * @param assoc_resp The (re-)association response frame is stored here.
     */
    core_sub_operation_wep_connect_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        bool_t& is_connected,
        core_management_status_e& connect_status,
        const core_ssid_s& ssid,
        core_ap_data_c& ap_data,
        core_authentication_mode_e auth_mode,
        core_encryption_mode_e encryption_level,
        core_type_list_c<core_frame_dot11_ie_c>& assoc_ie_list,
        core_frame_assoc_resp_c** assoc_resp );

    /**
     * Destructor.
     */
    virtual ~core_sub_operation_wep_connect_c();

protected:

// from base class core_operation_base_c

    /**
     * From core_operation_base_c.
     * This method is called when a pending request has been completed
     * and so sub-operations are pending.
     *
     * @since S60 v3.2
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e next_state();

    /**
     * From core_operation_base_c.
     * This method is called when the operation needs to be canceled.     
     *
     * @since S60 v3.2
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e cancel();

private: // data

    /** Whether we are still connected to an AP. */
    bool_t& is_connected_m;

    /** Status of connection attempt. */
    core_management_status_e& connect_status_m;

    /** SSID to connect to. */
    const core_ssid_s& ssid_m;

    /** AP to connect to. */
    core_ap_data_c& ap_data_m;

    /** Authentication mode to try initially. */    
    core_authentication_mode_e initial_auth_mode_m;

    /** Authentication mode to use. */
    core_authentication_mode_e auth_mode_m;

    /** Encryption to use. */
    core_encryption_mode_e encryption_m;

    /** The list of IEs that will be added to the association/resassociation request */
    core_type_list_c<core_frame_dot11_ie_c>& assoc_ie_list_m;

    /**
     * The (re-)association response frame is stored here if received.
     * Not owned by this pointer.
     */
    core_frame_assoc_resp_c** assoc_resp_m;

    };

#endif // CORE_SUB_OPERATION_WEP_CONNECT_H
