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
* Description:  Statemachine for connection release
*
*/


#ifndef CORE_OPERATION_RELEASE_H
#define CORE_OPERATION_RELEASE_H

#include "core_operation_base.h"

class core_server_c;

/**
 * Statemachine for releasing the connection
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_release_c ) : public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_eapol_disassociated,
        core_state_disable_user_data,
        core_state_tx_power_level,
        core_state_disconnect,
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_operation_release_c(
        u32_t request_id,
        core_server_c* server,        
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        core_release_reason_e reason );

    /**
     * Destructor.
     */
    virtual ~core_operation_release_c();

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
     * This method is called when user want the operation to be canceled.
     *
     * @since S60 v3.2
     * @param do_graceful_cancel Whether cancel should be graceful or forced.
     */
    void user_cancel(
        bool_t do_graceful_cancel );

private: // data

    /**
     * The reason the operation was started.
     */
    core_release_reason_e reason_m;

    };

#endif // CORE_OPERATION_RELEASE_H
