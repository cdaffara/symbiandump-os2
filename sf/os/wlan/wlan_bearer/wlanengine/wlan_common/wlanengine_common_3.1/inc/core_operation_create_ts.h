/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State machine for creating a virtual traffic stream.
*
*/

/*
* %version: 3 %
*/

#ifndef CORE_OPERATION_CREATE_TS_H
#define CORE_OPERATION_CREATE_TS_H

#include "core_operation_base.h"
#include "abs_core_frame_handler.h"
#include "core_virtual_traffic_stream.h"

class core_traffic_stream_c;

/**
 * State machine for creating a virtual traffic stream.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_create_ts_c ) : public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_delete_streams,
        core_state_create_stream,
        core_state_create_stream_success,
        core_state_create_stream_failed,
        core_state_parameters_set,
        core_state_MAX
        };

    core_operation_create_ts_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        u8_t tid,
        u8_t user_priority,
        bool_t is_automatic_stream,
        const core_traffic_stream_params_s& params,
        u32_t& stream_id,
        core_traffic_stream_status_e& stream_status );

    virtual ~core_operation_create_ts_c();

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

private:
    
    /**
     * Validate the given traffic stream parameters.
     * 
     * @return true_t if the parameters are valid, false_t otherwise.
     */
    bool_t is_valid_params();

private: // data

    /**
     * Requested TID of the virtual traffic stream.
     */
    u8_t requested_tid_m;

    /**
     * Selected TID of the virtual traffic stream.
     */
    u8_t tid_m;

    /**
     * User priority of the virtual traffic stream.
     */
    u8_t user_priority_m;

    /**
     * Access class of the virtual traffic stream.
     */
    core_access_class_e access_class_m;
    
    /**
     * Whether the virtual traffic stream has been created automatically.
     */
    bool_t is_automatic_stream_m;

    /**
     * Requested parameters of the virtual traffic stream.
     */
    core_traffic_stream_params_s requested_params_m;
    
    /**
     * Parameters of the virtual traffic stream.
     */
    core_traffic_stream_params_s params_m;

    /**
     * ID of the virtual traffic stream.
     */
    u32_t& stream_id_m;

    /**
     * The current status of the virtual traffic stream.
     */
    core_traffic_stream_status_e& stream_status_m;

    /**
     * The traffic stream being created.
     */
    core_traffic_stream_c* stream_m;

    };

#endif // CORE_OPERATION_CREATE_TS_H
