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
* Description:  Statemachine for updating power mode
*
*/


#ifndef CORE_OPERATION_UPDATE_POWER_MODE_H
#define CORE_OPERATION_UPDATE_POWER_MODE_H

#include "core_operation_base.h"

/**
 * Statemachine for updating the device IP address.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_update_power_mode_c ) : public core_operation_base_c
    {

public:

    /** The possible states of the operation. */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_req_set_power_mode,
        core_state_MAX
        };

    /**
     * Constructor.
     * 
     * When the operation is instantiated through this constructor, the power save mode
     * is determined from core_settings_c parameters.
     */
    core_operation_update_power_mode_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation );

    /**
     * Constructor.
     * 
     * When the operation is instantiated through this constructor, the power save mode
     * is passed as an argument.
     */
    core_operation_update_power_mode_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        const core_power_save_mode_s& mode );

    /**
     * Destructor.
     */
    virtual ~core_operation_update_power_mode_c();

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

private:

    /**
     * Determine the power mode to be used.
     *
     * @since S60 v3.2
     * @return The power save mode to be used.
     */
    core_power_mode_s determine_power_mode() const;    

private: // data

    /** The power save mode to be used. */
    core_power_save_mode_s preferred_mode_m;

    /** The power mode to be set. */
    core_power_mode_s power_mode_m;

    };

#endif // CORE_OPERATION_UPDATE_POWER_MODE_H
