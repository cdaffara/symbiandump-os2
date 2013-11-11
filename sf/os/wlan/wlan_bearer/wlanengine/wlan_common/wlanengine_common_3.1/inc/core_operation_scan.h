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
* Description:  Statemachine for scanning
*
*/


#ifndef CORE_OPERATION_SCAN_H
#define CORE_OPERATION_SCAN_H

#include "core_operation_base.h"
#include "abs_core_frame_handler.h"
#include "core_type_list.h"
#include "genscanlist.h"
#include "abs_core_event_handler.h"

class core_server_c;

/**
 * Statemachine for loading the drivers
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_scan_c ) :
    public core_operation_base_c,
    public abs_core_frame_handler_c,
    public abs_core_event_handler_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_scan_start,
        core_state_scan_complete,
        core_state_scan_start_unknown_region,
        core_state_scan_complete_unknown_region,
        core_state_scan_complete_store_country_info,
        core_state_scan_complete_handle_result,
        core_state_rcpi_received,
        core_state_scanning_done,
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_operation_scan_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        core_scan_mode_e scan_mode,
        const core_ssid_s& scan_ssid,
        const core_scan_channels_s& scan_channels,
        u8_t scan_max_age,
        ScanList& scan_data,
        bool_t passive_scan_all_channels,
        bool_t is_current_ap_added );

    /**
     * Destructor.
     */
    virtual ~core_operation_scan_c();

    /**
     * From abs_core_frame_handler_c Called by the core server when a dot11 frame has been received.
     *
     * @since S60 v3.1
     * @param frame Pointer to a dot11 frame parser.
     * @param rcpi RCPI value of the frame.
     * @return true_t if the frame was handled, false_t otherwise.
     */
    bool_t receive_frame(
        const core_frame_dot11_c* frame,
        u8_t rcpi );

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
     * This method is called when the operation is cancelled from the outside.
     *
     * @since S60 v3.2
     * @param do_graceful_cancel Whether cancel should be graceful or forced.
     */
    void user_cancel(
        bool_t do_graceful_cancel );

    /**
     * From abs_core_event_handler_c Called by the core server when an indication has been received.
     *
     * @since S60 v3.1
     * @param indication Adaptation layer event.
     * @return true_t if the indication was handled, false_t if not handled.
     */
    bool_t notify(
        core_am_indication_e indication );

private:

    /**
     * If region information is not known before the scan, also the channels 12 and 13 can be
     * scanned in passive mode. After the scan if region is still not known or region
     * limits the allowed channels, this method removes the those APs from scan list that
     * are not on valid channels.
     *
     * @since S60 v5.0
     */
     void remove_disallowed_aps();

private: // data

    /**
     * The scanning mode.
     */
    const core_scan_mode_e scan_mode_m;

    /**
     * The SSID to be scanned.
     */
    const core_ssid_s& scan_ssid_m;

    /**
     * The channels to be scanned.
     */
    const core_scan_channels_s& scan_channels_m;

    /**
     * The maximum age of returned scan results.
     */
    const u8_t scan_max_age_m;

    /**
     * Handle to the ScanList instance.
     */
    ScanList& scan_data_m;

    /**
     * Whether all channels should be scanned in passive mode if region information (MCC) is not known.
     */
    const bool_t passive_scan_all_channels_m;

    /**
     * Region information that is selected based on the APs country information.
     */
    core_wlan_region_e region_from_ap_m;

    /**
     * Whether the current AP needs be added during an ongoing connection if not otherwise
     * found in the scan.
     */
    const bool_t is_current_ap_added_m;

    /**
     * Current RCPI value.
     */
    u32_t current_rcpi_m;

    };

#endif // CORE_OPERATION_SCAN_H
