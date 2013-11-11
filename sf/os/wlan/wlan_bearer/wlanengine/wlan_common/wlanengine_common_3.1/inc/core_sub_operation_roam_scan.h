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
* Description:  Statemachine for doing scans when roaming
*
*/


#ifndef CORE_SUB_OPERATION_ROAM_SCAN_H
#define CORE_SUB_OPERATION_ROAM_SCAN_H

#include "core_operation_base.h"
#include "abs_core_frame_handler.h"
#include "abs_core_event_handler.h"

NONSHARABLE_CLASS( core_sub_operation_roam_scan_c ) :
    public core_operation_base_c,
    public abs_core_frame_handler_c,
    public abs_core_event_handler_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_scan_start,
        core_state_scan_started,
        core_state_scan_stop,
        core_state_scan_stopped,
        core_state_scan_complete,
        core_state_MAX
        };

    /**
     * Constructor
     *
     * @since S60 v3.1
     * @param request_id to corresponding service request
     * @param server core server
     * @param drivers pointer to lower adaptation
     * @param adaptation pointer to upper adaptation
     * @param scan_ssid SSID to be scanned.
     * @param scan_channels List of channels to scan.
     * @param is_connected Whether the terminal is currently connected to a BSS.
     * @param is_first_match_selected Whether scan should be stopped at the first match.
     */
    core_sub_operation_roam_scan_c(
        u32_t request_id,
        core_server_c* server,        
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        const core_ssid_s& scan_ssid,
        const core_scan_channels_s& scan_channels,
        bool_t is_connected,
        bool_t is_first_match_selected );

    /**
     * Destructor.
     */
    virtual ~core_sub_operation_roam_scan_c();

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

    /**
     * From abs_core_event_handler_c Called by the core server when an indication has been received.
     *
     * @since S60 v3.1
     * @param indication Adaptation layer event.
     * @return true_t if the indication was handled, false_t if not handled.
     */
    bool_t notify(
        core_am_indication_e indication );

private: // data

    /**
     * The SSID to be scanned.
     */
    const core_ssid_s scan_ssid_m;

    /**
     * The channels to be scanned.
     */
    const core_scan_channels_s scan_channels_m; 

    /**
     * Whether the terminal is currently connected to a BSS.
     */     
    bool_t is_connected_m;

    /**
     * Whether scan should be stopped at the first match.
     */        
    bool_t is_first_match_selected;

    };

#endif // CORE_SUB_OPERATION_ROAM_SCAN_H
