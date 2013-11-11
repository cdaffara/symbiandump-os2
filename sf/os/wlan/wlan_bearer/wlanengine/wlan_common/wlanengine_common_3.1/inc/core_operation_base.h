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
* Description:  Base class for wlan engine operations
*
*/


#ifndef CORE_OPERATION_BASE_H
#define CORE_OPERATION_BASE_H

#include "core_types.h"

class core_server_c;
class abs_core_driverif_c;
class abs_core_server_callback_c;

/**
 * Base class for wlan engine operations
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_base_c )
    {

public:

    /**
     * Defines the possible operation states used in the state machine
     * of the base class.
     */     
    enum core_base_state_e
        {
        core_base_state_init = 0,
        core_base_state_load_drivers,
        core_base_state_user_cancel,
        core_base_state_next,
        core_base_state_MAX
        };

    /**
     * Defines the possible feature flags an operation can have.
     */
    enum core_base_flag_e
        {
        /**
         * No special features.
         */
        core_base_flag_none = 0,
        /**
         * Operation needs drivers. The drivers will be automatically
         * loaded if not yet loaded.
         */
        core_base_flag_drivers_needed = 1,
        /**
         * Operation needs a onnection to a WLAN AP. This flag is used
         * to clear the operation queue from connection related operations
         * after a connection has been closed.
         */
        core_base_flag_connection_needed = 2,
        /**
         * Only one instance of the operation is allowed in the operation
         * queue.
         */
        core_base_flag_only_one_instance = 4,
        /**
         * The operation is a roam operation.
         */
        core_base_flag_roam_operation = 8
        };

    core_operation_base_c(
        core_operation_type_e type,
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        u32_t feature_flags );

    virtual ~core_operation_base_c();

    /**
     * starts the operation
     *
     * @since S60 v3.1
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e start_operation();

    /**
     * callback to previously made asynchronous request
     *
     * @since S60 v3.1
     * @param request_status Status of the completed request.
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e continue_operation(
        core_error_e request_status );

    /**
     * cancels the operation
     *
     * @since S60 v3.1
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e cancel_operation();

    /**
     * User cancels the operation
     *
     * @since S60 v3.2
     * @param do_graceful_cancel Whether cancel should be graceful or forced.
     */
    void user_cancel_operation( 
        bool_t do_graceful_cancel );

    /**
     * returns the request_id related to this operation
     *
     * @since S60 v3.1
     * @return enum specifying operation type
     */
    u32_t request_id() const;

    /**
     * returns the operation type
     *
     * @since S60 v3.1
     * @return enum specifying operation type
     */
    core_operation_type_e operation_type() const;

    /**
     * whether operation is executing or not ("in queue")
     * 
     * @since S60 v3.1
     * @return true or false
     */
    bool_t is_executing() const;

    /**
     * Check whether the operation has the given feature flags.
     * 
     * @since S60 v3.1
     * @param u32_t The feature flags to check for.
     * @return Whether the operation has the given feature flags.
     */
    bool_t is_flags(
        u32_t feature_flags ) const;

protected:

    /**
     * This method is called when a pending request has been completed
     * and sub-operations are pending.
     *
     * @since S60 v3.1
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    virtual core_error_e next_state() = 0;

    /**
     * This method is called when the operation needs to be canceled.     
     *
     * @since S60 v3.1
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    virtual core_error_e cancel();

    /**
     * This method is called when user want the operation to be canceled.
     *
     * @since S60 v3.2
     * @param do_graceful_cancel Whether cancel should be graceful or forced.
     */
    virtual void user_cancel(
        bool_t do_graceful_cancel );

    /**
     * Change to another state.
     *
     * @since S60 v3.1
     * @param state The new state.
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation     
     */
    core_error_e goto_state(
        u32_t state );

    /** 
     * Start a sub-operation and advance to the next state if
     * operation completes successfully.
     *
     * If the sub-operation fails, will initiate canceling.
     * 
     * @since S60 v3.1
     * @param sub_operation The sub-operation to start.
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e run_sub_operation(
        core_operation_base_c* sub_operation );

    /** 
     * Start a sub-operation and advance to the next state if
     * operation completes successfully.
     *
     * If the sub-operation fails, will initiate canceling.
     * 
     * @since S60 v3.1
     * @param sub_operation The sub-operation to start.
     * @param state The new state.
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e run_sub_operation(
        core_operation_base_c* sub_operation,
        u32_t state );

    /**
     * Change to another state asynchronously.
     *
     * @since S60 v3.1
     * @param state The new state.
     * @param delay Delay before changing state in microseconds.
     * @return Returns always core_error_request_pending.
     */     
    core_error_e asynch_goto(
        u32_t state,
        u32_t delay = CORE_TIMER_IMMEDIATELY );

    /**
     * The asynchronous version of the default user cancel handler.
     *
     * This method is meant for situations where the cancel cannot
     * proceed because the operation is waiting for an event.
     *
     * @since S60 v3.2
     */
    void asynch_default_user_cancel();

protected: // data

    /**
     * Request_id is used for identifying the initial caller.
     */
    u32_t request_id_m;

    /**
     * Pointer to the core_server instance.
     * Not owned by this pointer.
     */
    core_server_c* server_m;

    /**
     * Pointer to low-level management functionality.
     * Not owned by this pointer.
     */
    abs_core_driverif_c* drivers_m;

    /**
     * Pointer to the upper-level management functionality.
     * Not owned by this pointer.
     */
    abs_core_server_callback_c* adaptation_m;

    /**
     * Has this operation been started or not.
     */
    bool_t is_executing_m;

    /**
     * Whether this operation is currently being canceled.
     */
    bool_t is_canceling_m;

    /**
     * Internal state of the operation.
     */
    u32_t operation_state_m;

    /**
     * The pending sub-operation.
     */
    core_operation_base_c* sub_operation_m;

    /**
     * Operation failure reason.
     */
    core_error_e failure_reason_m;

private: // data    

    /**
     * specifies the type of the operation (e.g. connect, scan, ...)
     */
    core_operation_type_e operation_type_m;

    /**
     * Defines the features of the operation.
     */
    u32_t feature_flags_m;

    };

#endif // CORE_OPERATION_BASE_H
