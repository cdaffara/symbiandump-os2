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
* Description:  Statemachine for getting the packet statistics.
*
*/


#ifndef CORE_OPERATION_GET_STATISTICS_H
#define CORE_OPERATION_GET_STATISTICS_H

#include "core_operation_base.h"

/**
 * Statemachine for getting the packet statistics.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_get_statistics_c ) : public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_req_packet_statistics,
        core_state_MAX
        };

    core_operation_get_statistics_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        core_packet_statistics_s& statistics );

    virtual ~core_operation_get_statistics_c();

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

    /** Packet statistics for the client are stored here. */
    core_packet_statistics_s& client_statistics_m;

    /** Packet statistics for the current connection. */
    core_packet_statistics_by_access_category_s current_statistics_m;

    };

#endif // CORE_OPERATION_GET_STATISTICS_H
