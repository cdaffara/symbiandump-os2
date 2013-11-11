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
* Description:  Statemachine for handling traffic stream deletion from network side.
*
*/

/*
* %version: 4 %
*/

#ifndef CORE_OPERATION_HANDLE_DELETE_TS_H
#define CORE_OPERATION_HANDLE_DELETE_TS_H

#include "core_operation_base.h"

/**
 * Statemachine for handling traffic stream deletion from network side.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_operation_handle_delete_ts_c ) : public core_operation_base_c
    {

public:

    /**
     * The possible states of the operation.
     */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_base_state_parameters_set,
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_operation_handle_delete_ts_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        u8_t tid );

    virtual ~core_operation_handle_delete_ts_c();

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
     * Set the status of all virtual traffic streams with a matching TID
     * and send a notification to clients.
     *
     * @param tid TID to match against.
     * @param stream_status Stream status to set.
     */
    void set_virtual_traffic_stream_inactive_by_tid(
        u8_t tid,
        core_traffic_stream_status_e stream_status );

private: // data

    /**
     * The TID of the traffic stream.
     */
    u8_t tid_m;

    };

#endif // CORE_OPERATION_HANDLE_DELETE_TS_H
