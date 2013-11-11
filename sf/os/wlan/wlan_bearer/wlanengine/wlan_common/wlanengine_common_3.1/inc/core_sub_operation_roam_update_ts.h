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
* Description:  Statemachine for updating traffic stream statuses after roam.
*
*/

/*
* %version: 3 %
*/

#ifndef CORE_SUB_OPERATION_ROAM_UPDATE_TS_H
#define CORE_SUB_OPERATION_ROAM_UPDATE_TS_H

#include "core_operation_base.h"
#include "core_virtual_traffic_stream_list.h"
#include "core_virtual_traffic_stream_list_iter.h"
#include "core_traffic_stream_list.h"
#include "core_traffic_stream_list_iter.h"
#include "core_ap_data.h"

/**
 * Statemachine for updating traffic stream statuses after roam.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_sub_operation_roam_update_ts_c ) : public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_recreate_next,
        core_state_recreate_success,
        core_state_recreate_fail,
        core_state_recreate_virtual_next,
        core_state_recreate_virtual_success,
        core_state_recreate_virtual_fail,
        core_state_set_params_next,
        core_state_set_params_success,
        core_state_MAX
        };

    core_sub_operation_roam_update_ts_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        core_ap_data_c& ap_data );

    virtual ~core_sub_operation_roam_update_ts_c();

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
     * Set the status of all virtual traffic streams with a matching TID
     * and send a notification to clients.
     *
     * @param tid TID to match against.
     * @param stream_status Stream status to set.
     */
    void set_virtual_traffic_stream_inactive_by_tid(
        u8_t tid,
        core_traffic_stream_status_e stream_status );

    /**
     * Set the status of all virtual traffic streams with a matching ID
     * and send a notification to clients.
     *
     * @param id ID to match against.
     * @param stream_status Stream status to set.
     */
    void set_virtual_traffic_stream_inactive_by_id(
        u32_t id,
        core_traffic_stream_status_e stream_status );    

    /**
     * Set the status of all virtual traffic streams with a matching ID
     * and send a notification to clients.
     *
     * @param id ID to match against.
     * @param tid Current TID of the virtual traffic stream.
     */
    void set_virtual_traffic_stream_active_by_id(
        u32_t id,
        u8_t tid );

private: // data

    /** The AP currently tried. */
    core_ap_data_c& current_ap_m;

    /** List of virtual traffic streams to be recreated. */
    core_virtual_traffic_stream_list_c virtual_stream_list_m;

    /** Iterator for virtual traffic streams. */
    core_virtual_traffic_stream_list_iter_c virtual_stream_iter_m;

    /** List of traffic stream to be recreated. */
    core_traffic_stream_list_c stream_list_m;

    /** Iterator for traffic streams. */
    core_traffic_stream_list_iter_c stream_iter_m;

    /** TID of the traffic stream to be recreated. */
    u8_t tid_m;

    /** The User Priority of the traffic stream. */    
    u8_t user_priority_m;

    /** Parameters of the traffic stream to be recreated. */
    core_traffic_stream_params_s params_m;

    /** The current status of the traffic stream. */
    core_traffic_stream_status_e stream_status_m;

    };

#endif // CORE_SUB_OPERATION_ROAM_UPDATE_TS_H
