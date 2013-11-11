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
* Description:  Statemachine for connecting to a network
*
*/


#ifndef CORE_OPERATION_CONNECT_H
#define CORE_OPERATION_CONNECT_H

#include "core_operation_base.h"
#include "core_type_list.h"
#include "core_ap_data.h"

class abs_core_timer_c;

/**
 * Statemachine for connecting to a network.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_connect_c ) : public core_operation_base_c
    {

public:

    /** The possible states of the state machine. */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_power_mode,
        core_state_secondary_ssid_scan_start,
        core_state_secondary_ssid_scan,
        core_state_scan_start,
        core_state_scan_complete,
        core_state_connect_failure,
        core_state_connect_secondary_ssid_failure,
        core_state_connect_total_failure,
        core_state_connect_disconnect,
        core_state_connect_success,
        core_state_set_rcpi_trigger,
        core_state_MAX
        };

    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param request_id to corresponding service request
     * @param server core server
     * @param drivers pointer to lower adaptation
     * @param adaptation pointer to upper adaptation
     * @param settings The settings used for establishing the connection.
     * @param ssid_list List of possible secondary SSIDs.
     * @param connect_status contains the error code of connection attempt
     *        on completion.
     */
    core_operation_connect_c(
        u32_t request_id,
        core_server_c* server,        
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        const core_iap_data_s& settings,
        core_type_list_c<core_ssid_entry_s>* ssid_list,
        core_connect_status_e& connect_status );

    /**
     * Destructor.
     */
    virtual ~core_operation_connect_c();

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
     * This method is called when user want the operation to be canceled.
     *
     * @since S60 v3.2
     * @param do_graceful_cancel Whether cancel should be graceful or forced.
     */
    void user_cancel(
        bool_t do_graceful_cancel );

private: // data

    /**
     * The settings used for establishing the connection.
     */
    const core_iap_data_s settings_m;

    /** Connect status that will be returned to adaptation */
    core_connect_status_e& connect_status_m;
    
    /**
     * The number of times roaming has failed.
     */
    u32_t failure_count_m;

    /**
     * Whether we are still connected to an AP.
     */
    bool_t is_connected_m;

    /**
     * SSID to connect to.
     */    
    core_ssid_s ssid_m;

    /**
     * List of secondary SSIDs. NULL if none. Not owned by this pointer.
     */
    core_type_list_c<core_ssid_entry_s>* ssid_list_m;

    /**
     * Reason the connection is released.
     */
    core_release_reason_e release_reason_m;

    };

#endif // CORE_OPERATION_CONNECT_H
