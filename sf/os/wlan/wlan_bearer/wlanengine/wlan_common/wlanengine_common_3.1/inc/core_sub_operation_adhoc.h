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
* Description:  Statemachine for establishing an adhoc network
*
*/


#ifndef CORE_SUB_OPERATION_ADHOC_H
#define CORE_SUB_OPERATION_ADHOC_H

#include "core_operation_base.h"
#include "core_ap_data.h"
#include "abs_core_frame_handler.h"
#include "abs_core_event_handler.h"

class core_server_c;
class abs_core_driverif_c;

/**
 * Statemachine for establishing an adhoc network
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_sub_operation_adhoc_c ) :
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
        core_state_req_set_tx_rate_policies,
        core_state_req_start_adhoc,
        core_state_adhoc_started,
        core_state_adhoc_frame,
        core_state_adhoc_complete,
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_sub_operation_adhoc_c(
        u32_t request_id,        
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        core_ap_data_c** ap_data );

    /**
     * Destructor.
     */
    virtual ~core_sub_operation_adhoc_c();

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

    core_ap_data_c** ptr_ap_data_m;

    /** The channel to create the network to. */    
    u32_t channel_m;

    /** Structure for the channel noise information. */
    u32_t noise_per_channel_m[SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO];

    };

#endif // CORE_SUB_OPERATION_ADHOC_H
