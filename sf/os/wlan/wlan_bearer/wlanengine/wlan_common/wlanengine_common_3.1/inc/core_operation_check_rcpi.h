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
* Description:  Statemachine for requesting RCPI values for roaming.
*
*/


#ifndef CORE_OPERATION_CHECK_RCPI_H
#define CORE_OPERATION_CHECK_RCPI_H

#include "core_operation_base.h"

/**
 * Statemachine for requesting RCPI values for roaming.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_check_rcpi_c ) : public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_rcpi_received,
        core_state_scan_start,
        core_state_scan_complete,
        core_state_connect_success,
        core_state_connect_failure,
        core_state_rcpi_trigger,
        core_state_MAX
        };

    /** 
     * Defines the possible reasons for running the RCPI check operation.
     */
    enum core_rcpi_check_reason_e
        {
        /** The operation has been triggered by a polling timer. */
        core_rcpi_check_reason_timer,
        /** The operation has been triggered by an RCPI trigger indication. */
        core_rcpi_check_reason_rcpi_trigger,
        /** The operation has been triggered by a signal loss prediction indication. */
        core_rcpi_check_reason_signal_loss_prediction
        };

    core_operation_check_rcpi_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        core_rcpi_check_reason_e reason );

    virtual ~core_operation_check_rcpi_c();

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

private: // data

    /**
     * The reason the operation was started.
     */
    core_rcpi_check_reason_e reason_m;

    /**
     * Current RCPI value.
     */
    u32_t current_rcpi_m;

    /**
     * Whether we are still connected to the AP after the roam attempt.
     */
    bool_t is_connected_m;

    };

#endif // CORE_OPERATION_CHECK_RCPI_H
