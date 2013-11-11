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
* Description:  Statemachine for handling a frame received from drivers
*
*/


#ifndef CORE_OPERATION_HANDLE_FRAME_H
#define CORE_OPERATION_HANDLE_FRAME_H

#include "core_operation_base.h"
#include "core_frame_dot11.h"

/**
 * Statemachine for handling a frame received from drivers
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_operation_handle_frame_c ) : public core_operation_base_c
    {

public:

    enum core_state_e
        {
        core_state_init = core_base_state_next,
        core_state_done,
        core_state_MAX
        };

    core_operation_handle_frame_c(
        u32_t request_id,
        core_server_c* server,
        abs_core_driverif_c* drivers,
        abs_core_server_callback_c* adaptation,
        core_frame_type_e frame_type,
        u16_t data_length,
        const u8_t* data );

    virtual ~core_operation_handle_frame_c();

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

    // Prohibit copy constructor
    core_operation_handle_frame_c(
        const core_operation_handle_frame_c& );
    // Prohibit assigment operator
    core_operation_handle_frame_c& operator=(
        const core_operation_handle_frame_c& );

private: // data

    /** The type of the frame. */
    core_frame_type_e type_m;

    /** The frame length. */
    const u16_t data_length_m;

    /** Pointer to the frame data */
    u8_t* data_m;

    /** Pointer to the 802.11 frame */
    core_frame_dot11_c* frame_m;
    
    };

#endif // CORE_OPERATION_HANDLE_FRAME_H
