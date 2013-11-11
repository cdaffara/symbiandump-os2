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
* Description:  Statemachine for configuring multicast address to drivers
*
*/


#ifndef CORE_OPERATION_CONFIGURE_MULTICAST_GROUP_H
#define CORE_OPERATION_CONFIGURE_MULTICAST_GROUP_H

#include "core_operation_base.h"

/**
 * Statemachine for configuring multicast address to drivers.
 * (driver sw performs multicast address filtering)
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_configure_multicast_group_c ) : 
    public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_req_configure_multicast_group,
        core_state_MAX
        };

    core_operation_configure_multicast_group_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        bool_t join_group,
        const core_mac_address_s& multicast_addr );

    virtual ~core_operation_configure_multicast_group_c();

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

    /** whether adding or removing address */
    bool_t is_join_m;
    
    /** multicast mac address to add */
    const core_mac_address_s multicast_addr_m;
    };

#endif // CORE_OPERATION_CONFIGURE_MULTICAST_GROUP_H
