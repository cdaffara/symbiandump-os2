/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for creating a traffic stream.
*
*/

/*
* %version: 5 %
*/

#ifndef CORE_SUB_OPERATION_CREATE_TS_H
#define CORE_SUB_OPERATION_CREATE_TS_H

#include "core_operation_base.h"
#include "core_traffic_stream.h"
#include "core_ap_data.h"
#include "abs_core_frame_handler.h"

/**
 * Statemachine for creating a traffic stream.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_sub_operation_create_ts_c ) :
    public core_operation_base_c,
    public abs_core_frame_handler_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_request_sent,
        core_state_invalid_parameters,
        core_state_failure,
        core_state_success,
        core_state_MAX
        };

    core_sub_operation_create_ts_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        const core_ap_data_c& ap_data,
        u8_t tid,
        u8_t user_priority,
        core_traffic_stream_params_s& tspec,
        core_traffic_stream_status_e& stream_status );

    virtual ~core_sub_operation_create_ts_c();

    /**
     * From abs_core_frame_handler_c Called by the core server when a dot11 frame has been received.
     *
     * @since S60 v3.1
     * @param frame Pointer to a frame parser.
     * @param rcpi RCPI value of the frame.
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

private: // data

    /** The AP currently tried. */
    const core_ap_data_c& current_ap_m;

    /**
     * The TID of the traffic stream.
     */
    u8_t tid_m;

    /**
     * The User Priority of the traffic stream.
     */    
    u8_t user_priority_m;

    /** Traffic stream to be created. */
    core_traffic_stream_params_s& tspec_m;

    /** The current status of the traffic stream. */
    core_traffic_stream_status_e& stream_status_m; 

    /** How many times AP has rejected our request due to invalid parameters. */
    u8_t invalid_parameters_count_m;
    
    };

#endif // CORE_SUB_OPERATION_CREATE_TS_H
