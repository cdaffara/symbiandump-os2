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
* Description:  Interface implemented by core server. Lower adaptation can use
*                these services.
*
*/


#ifndef ABS_CORE_DRIVERIF_CALLBACK_H
#define ABS_CORE_DRIVERIF_CALLBACK_H

#include "core_types.h"

/**
 * Abstract core server interface for low-level adaptation layer callbacks.
 *
 * This interface is implemented by the core server and it provides a callback
 * interface for the low-level adaptation layer.
 *
 * @since S60 v3.1
 * @see abs_core_driverif_c
 */
class abs_core_driverif_callback_c
    {

public:

    /**
     * Destructor.
     */
    virtual ~abs_core_driverif_callback_c() {};

    /**
     * Inform the core server about a new received frame.
     *
     * @since S60 v3.1
     * @param frame_type The type of the frame received.
     * @param frame_length The length of the frame.
     * @param frame_data Pointer to the frame data.     
     * @param frame_rcpi RCPI value of the frame.
     */
    virtual void receive_frame(
        core_frame_type_e frame_type,
        const u16_t frame_length,
        const u8_t* const frame_data,
        u8_t frame_rcpi ) = 0;

    /**
     * Notify the core server about an adaptation layer event.
     *
     * @since S60 v3.1
     * @param indication Adaptation layer event.
     */
    virtual void notify(
        core_am_indication_e indication ) = 0;

    /**
     * An asynchronous request from the core server has been completed.
     *
     * @since S60 v3.1
     * @param request_id Identification of the corresponding request.
     * @param status Completion status of the request.
     */
    virtual void request_complete(
        u32_t request_id,
        core_error_e status ) = 0;

    };

#endif // ABS_CORE_DRIVERIF_CALLBACK_H
