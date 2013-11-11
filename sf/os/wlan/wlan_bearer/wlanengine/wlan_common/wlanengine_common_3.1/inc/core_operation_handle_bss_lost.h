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
* Description:  Statemachine for handling BSS lost notification from drivers.
*
*/

/*
* %version: 12 %
*/

#ifndef CORE_OPERATION_HANDLE_BSS_LOST_H
#define CORE_OPERATION_HANDLE_BSS_LOST_H

#include "core_operation_base.h"
#include "core_scan_channels.h"

class abs_core_timer_c;

/**
 * Statemachine for handling BSS lost notification from drivers.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_handle_bss_lost_c ) : public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_eapol_disassociated,
        core_state_set_tx_level,
        core_state_set_tx_level_success,
        core_state_scan_start,
        core_state_scan_complete,
        core_state_connect_failure,
        core_state_connect_total_failure,
        core_state_connect_disconnect,
        core_state_connect_success,
        core_state_connect_set_rcpi_trigger,
        core_state_MAX
        };

    /** 
     * Defines the possible reasons for running the BSS lost operation.
     */
    enum core_bss_lost_reason_e
        {
        /** The operation has been triggered by a BSS lost indication. */
        core_bss_lost_reason_bss_lost,
        /** The operation has been triggered by a media disconnect indication. */
        core_bss_lost_reason_media_disconnect,
        /** The operation has been triggered by a failed RCPI roaming attempt. */
        core_bss_lost_reason_failed_rcpi_roam,
        /** The operation has been triggered by a failed reauthentication. */
        core_bss_lost_reason_failed_reauthentication,
        /** The operation has been triggered by a failed directed roam attempt. */
        core_bss_lost_reason_failed_directed_roam
        };

    core_operation_handle_bss_lost_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        core_bss_lost_reason_e reason,
        u32_t reason_data = 0 );

    virtual ~core_operation_handle_bss_lost_c();

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
     * The reason the operation was started.
     */
    core_bss_lost_reason_e reason_m;

    /**
     * Optional data related to the reason_m.
     */
    u32_t reason_data_m;

    /**
     * The number of times roaming has failed due to no suitable AP found.
     */
    u32_t failure_count_m;

    /**
     * Whether we are still connected to an AP.
     */
    bool_t is_connected_m;    

    /**
     * The channels used for scanning when roaming.
     */
    core_scan_channels_c scan_channels_m;

    /**
     * Whether the scan channels were already inverted.
     */
    bool_t is_scan_channels_inverted_m;

    /**
     * Reason the connection is released.
     */
    core_release_reason_e release_reason_m;    

    /**
     * Number of times the current scan interval has been tried.
     */
    u32_t scan_interval_count_m;

    /**
     * The current scan interval value.
     */
    u32_t scan_interval_m;

    /**
     * Whether this scan is part of the scans done with
     * partial channel masks.
     */
    bool_t is_first_scan_m;
    
    };

#endif // CORE_OPERATION_HANDLE_BSS_LOST_H
