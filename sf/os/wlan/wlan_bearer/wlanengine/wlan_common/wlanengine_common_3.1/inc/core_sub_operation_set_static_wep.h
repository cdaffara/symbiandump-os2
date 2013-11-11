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
* Description:  Statemachine for settings static WEP keys
*
*/


#ifndef CORE_SUB_OPERATION_SET_STATIC_WEP_H
#define CORE_SUB_OPERATION_SET_STATIC_WEP_H

#include "core_operation_base.h"
#include "core_ap_data.h"

class core_server_c;
class abs_core_driverif_c;

/**
 * Statemachine for settings static WEP keys
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_sub_operation_set_static_wep_c ) : public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_req_set_key1,
        core_state_req_set_key2,
        core_state_req_set_key3,
        core_state_req_set_key4,
        core_state_MAX
        };

    /**
     * Constructor.
     */
    core_sub_operation_set_static_wep_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation );

    /**
     * Destructor.
     */
    virtual ~core_sub_operation_set_static_wep_c();

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
   
    };

#endif // CORE_SUB_OPERATION_SET_STATIC_WEP_H
