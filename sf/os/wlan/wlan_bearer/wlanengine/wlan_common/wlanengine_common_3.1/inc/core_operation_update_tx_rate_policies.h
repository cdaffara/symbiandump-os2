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


#ifndef CORE_OPERATION_UPDATE_TX_RATE_POLICIES_H
#define CORE_OPERATION_UPDATE_TX_RATE_POLICIES_H

#include "core_operation_base.h"
#include "core_ap_data.h"

/**
 * Statemachine for updating the currently active TX rate policies.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_operation_update_tx_rate_policies_c ) :
    public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_success,
        core_state_MAX
        };

    enum core_ap_type_e
        {
        core_ap_type_802p11b_only,
        core_ap_type_802p11g_only,
        core_ap_type_802p11bg
        };

    /**
     * Constructor.
     */
    core_operation_update_tx_rate_policies_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        core_ap_data_c& ap_data );

    /**
     * Constructor.
     */
    core_operation_update_tx_rate_policies_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation );

    /**
     * Destructor.
     */
    virtual ~core_operation_update_tx_rate_policies_c();

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

    /** The AP to be parsed for supported rates. Not owned by this pointer. */
    core_ap_data_c* current_ap_m;

    };

#endif // CORE_OPERATION_UPDATE_TX_RATE_POLICIES_H
