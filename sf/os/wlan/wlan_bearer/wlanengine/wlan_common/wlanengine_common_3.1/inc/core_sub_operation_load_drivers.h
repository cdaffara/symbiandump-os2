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
* Description:  Statemachine for loading the drivers
*
*/


#ifndef CORE_SUB_OPERATION_LOAD_DRIVERS_H
#define CORE_SUB_OPERATION_LOAD_DRIVERS_H

#include "core_operation_base.h"

class core_server_c;
class abs_core_driverif_c;

/**
 * Statemachine for loading the drivers
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_sub_operation_load_drivers_c ) : public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_req_region,
        core_state_req_load_drivers,
        core_state_req_set_tx_power,        
        core_state_req_set_rxtx_parameters,
        core_state_req_set_uapsd_settings,
        core_state_req_set_power_save_settings,
        core_state_req_set_power_mode_mgmt_settings,
        core_state_req_set_block_ack_usage,
        core_state_req_set_wpx_settings,
        core_state_cancel_req_unload_drivers,
        core_state_done,
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_sub_operation_load_drivers_c(
        u32_t request_id,
        core_server_c* server,        
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation );

    /**
     * Destructor.
     */
    virtual ~core_sub_operation_load_drivers_c();

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
     * This method is called when the operation is cancelled from the outside.
     *
     * @since S60 v3.2
     * @param do_graceful_cancel Whether cancel should be graceful or forced.
     */
    void user_cancel(
        bool_t do_graceful_cancel );

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
     * Current WLAN region.
     */
    core_wlan_region_e region_m;
    
    /**
     * Is MCC (mobile country code) currently known
     */
    bool_t mcc_known_m;

    };

#endif // CORE_SUB_OPERATION_LOAD_DRIVERS_H
