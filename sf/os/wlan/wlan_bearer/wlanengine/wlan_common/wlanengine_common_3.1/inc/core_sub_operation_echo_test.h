/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for running an echo test.
*
*/


#ifndef CORE_SUB_OPERATION_ECHO_TEST_H
#define CORE_SUB_OPERATION_ECHO_TEST_H

#include "core_operation_base.h"
#include "abs_core_frame_handler.h"
#include "core_ap_data.h"

/**
 * Statemachine for running an echo test.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_sub_operation_echo_test_c ) :
    public core_operation_base_c,
    public abs_core_frame_handler_c
    {

public:

    /**
     * The possible states of the operation.
     */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_echo_frame_broadcast_timeout,
        core_state_echo_frame_unicast_timeout,
        core_state_echo_frame_broadcast,
        core_state_echo_frame_unicast,
        core_state_MAX
        };

    /**
     * Constructor.
     *
     * @since S60 v3.2
     * @param request_id An unique identification for the request.
     * @param server Handle to core server.
     * @param drivers Handle to low-level management functionality.
     * @param adaptation Handle upper-level management functionality.
     * @param ap_data The access point being tested.
     * @param max_retry_count Maximum times an echo test frame can be re-sent.
     * @param timeout The amount of microseconds to wait for response.
     * @param test_mode Indicates the current test mode which is unicast or broadcast. 
     */
    core_sub_operation_echo_test_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        const core_ap_data_c& ap_data,
        u8_t max_retry_count,
        u32_t timeout,
        bool_t test_mode );

    /**
     * Destructor.
     */
    virtual ~core_sub_operation_echo_test_c();

protected:

    /**
     * From core_operation_base_c This method is called when a pending request has been completed
     * and no sub-operations are pending.
     *
     * @since S60 v3.2
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e next_state();

    /**
     * From abs_core_frame_handler_c Called by the core server when an echo test frame has been received.
     *
     * @since S60 v3.2
     * @param frame Pointer to an echo test frame parser.
     * @param rcpi RCPI value of the frame.
     * @return true_t if the frame was handled, false_t otherwise.
     */
    bool_t receive_test_frame(
        const core_frame_echo_test_c* frame,
        u8_t rcpi );

private: // data

    /**
     * The access point being tested.
     */
    const core_ap_data_c& current_ap_m;

    /**
     * Maximum times an echo test frame can be re-sent.
     */
    const u8_t max_retry_count_m;

    /**
     * Number of times the echo test frame has been sent.
     */
    u8_t retry_count_m;

    /**
     * The amount of microseconds to wait for response.
     */
    u32_t timeout_m;

    /**
     * The token used in the sent echo test frame.
     */
    u16_t token_m;

    /** 
     * Status of the operation.
     *
     * core_error_timeout if no response was received from the AP.
     * core_error_ok if response was successfully received,
     * an error otherwise.
     */
    core_error_e return_status_m;
    
    /**
     * Illustrates the state if unicast test mode is on.
     */
    bool_t is_unicast_mode_m;

    };

#endif // CORE_SUB_OPERATION_ECHO_TEST_H
