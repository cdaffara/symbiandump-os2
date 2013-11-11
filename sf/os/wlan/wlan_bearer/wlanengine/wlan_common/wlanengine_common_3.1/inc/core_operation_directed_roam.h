/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  State machine for a directed roam request.
*
*/

/*
* %version: 1 %
*/

#ifndef CORE_OPERATION_DIRECTED_ROAM_H
#define CORE_OPERATION_DIRECTED_ROAM_H

#include "core_operation_base.h"

/**
 * State machine for a directed roam request.
 */
NONSHARABLE_CLASS( core_operation_directed_roam_c ) : public core_operation_base_c
    {

public:

    /**
     * The possible states of the state machine.
     */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_scan_start,
        core_state_scan_complete,
        core_state_connect_success,
        core_state_connect_failure,
        core_state_rcpi_trigger,
        core_state_MAX
        };

    /**
     * Constructor.
     * 
     * @param request_id An unique identification for the request.
     * @param server Handle to core server.
     * @param drivers Handle to low-level management functionality.
     * @param adaptation Handle upper-level management functionality.
     * @param bssid BSSID to roam to, BROADCAST_MAC_ADDR if any.
     */
    core_operation_directed_roam_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        const core_mac_address_s& bssid );

    /**
     * Destructor.
     */
    virtual ~core_operation_directed_roam_c();

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

private: // data

    /**
     * BSSID to roam to, BROADCAST_MAC_ADDR if any.
     */
    core_mac_address_s bssid_m;

    /**
     * Current RCPI value.
     */
    u32_t current_rcpi_m;

    /**
     * Whether we are still connected to the AP after the roam attempt.
     */
    bool_t is_connected_m;

    };

#endif // CORE_OPERATION_DIRECTED_ROAM_H
