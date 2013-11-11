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
* Description:  Statemachine for running an echo test in power save.
*
*/


#ifndef CORE_OPERATION_POWER_SAVE_TEST_H
#define CORE_OPERATION_POWER_SAVE_TEST_H

#include "core_operation_base.h"
#include "abs_core_frame_handler.h"
#include "core_ap_data.h"

/**
 * Statemachine for running an echo test in power save.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_power_save_test_c ) : public core_operation_base_c
    {

public:

    /** The possible states of the operation. */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_set_power_mode_none, 
        core_state_set_power_mode_on, 
        core_state_reset_power_mode,
        core_state_echo_test,
        core_state_echo_test_complete,
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_operation_power_save_test_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation );

    /**
     * Destructor.
     */
    virtual ~core_operation_power_save_test_c();

protected:

    /**
     * This method is called when a pending request has been completed
     * and so sub-operations are pending.
     *
     * @since S60 v3.2
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e next_state();

    /**
     * This method is called when the operation needs to be canceled.     
     *
     * @since S60 v3.2
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e cancel();

private: // data

    /**
     * The AP currently being tested. Not owned by this pointer.
     */
    const core_ap_data_c* current_ap_m;

    /** 
     * Status of the operation.
     *
     * core_error_timeout if no response was received from the AP.
     * core_error_ok if response was successfully received,
     * an error otherwise.
     */
    core_error_e return_status_m;

    /**
     * Whether power save enabled during the echo test.
     */
    bool_t is_power_save_m;
    
    /**
      * Whether the Unicast test enabled.
      */
    bool_t is_unicast_test_on_m;

    };

#endif // CORE_OPERATION_POWER_SAVE_TEST_H
