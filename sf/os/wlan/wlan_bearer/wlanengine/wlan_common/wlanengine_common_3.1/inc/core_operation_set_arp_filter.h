/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State machine for updating arp filter
*
*/


#ifndef CORE_OPERATION_SET_ARP_FILTER_H
#define CORE_OPERATION_SET_ARP_FILTER_H

#include "core_operation_base.h"

/**
 * Statemachine for updating the device ARP filter.
 *
 * @lib wlmserversrv.lib
 * @since S60 v5.0
 */
NONSHARABLE_CLASS( core_operation_set_arp_filter_c ) : public core_operation_base_c
    {

public:

    /** The possible states of the operation. */
    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_set_arp_filter,
        core_state_MAX
        };

    /**
     * Constructor.
     * 
     * When the operation is instantiated, the ARP filter
     * is passed as an argument.
     */
    core_operation_set_arp_filter_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        const core_arp_filter_s& filter );

    /**
     * Destructor.
     */
    virtual ~core_operation_set_arp_filter_c();

protected:

    /**
     * This method is called when a pending request has been completed
     * and so sub-operations are pending.
     *
     * @since S60 v5.0
     * @return status of the operation:
     *     core_error_request_pending if the operation is not finished,
     *     otherwise the status code of the finished operation
     */
    core_error_e next_state();

private: // data

    /** ARP filter is stored here. */
    const core_arp_filter_s arp_filter_m;
    
    };

#endif // CORE_OPERATION_SET_ARP_FILTER_H
