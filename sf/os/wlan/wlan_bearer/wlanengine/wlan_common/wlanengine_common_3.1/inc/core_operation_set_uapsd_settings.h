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
* Description:  Statemachine for setting U-APSD settings.
*
*/


#ifndef CORE_OPERATION_SET_UAPSD_SETTINGS_H
#define CORE_OPERATION_SET_UAPSD_SETTINGS_H

#include "core_operation_base.h"

/**
 * Statemachine for setting U-APSD settings.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_set_uapsd_settings_c ) : public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_set_uapsd_settings,
        core_state_MAX
        };

    core_operation_set_uapsd_settings_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        const core_uapsd_settings_s& settings );

    virtual ~core_operation_set_uapsd_settings_c();

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

    /** U-APSD settings are stored here. */
    const core_uapsd_settings_s uapsd_settings_m;

    };

#endif // CORE_OPERATION_SET_UAPSD_SETTINGS_H
