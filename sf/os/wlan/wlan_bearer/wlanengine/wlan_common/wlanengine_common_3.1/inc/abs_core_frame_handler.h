/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for operations registered as frame handlers
*
*/


#ifndef ABS_CORE_FRAME_HANDLER_H
#define ABS_CORE_FRAME_HANDLER_H

#include "am_platform_libraries.h"
#include "core_types.h"

class core_frame_dot11_c;
class core_frame_ethernet_c;
class core_frame_echo_test_c;

/**
 * Abstract interface for operations wanting to handle frames themselves.
 *
 * @since S60 v3.1
 */
class abs_core_frame_handler_c
    {
public:

    /**
     * Destructor.
     */
    virtual ~abs_core_frame_handler_c() {};

    /**
     * Called by the core server when a dot11 frame has been received.
     *
     * @since S60 v3.1
     * @param frame Pointer to a dot11 frame parser.
     * @param rcpi RCPI value of the frame.
     * @return true_t if the frame was handled, false_t otherwise.
     */
    virtual bool_t receive_frame(
        const core_frame_dot11_c* /* frame */,
        u8_t /* rcpi */ ) { return false_t; }

    /**
     * Called by the core server when an echo test frame has been received.
     *
     * @since S60 v3.2
     * @param frame Pointer to an echo test frame parser.
     * @param rcpi RCPI value of the frame.
     * @return true_t if the frame was handled, false_t otherwise.
     */
    virtual bool_t receive_test_frame(
        const core_frame_echo_test_c* /* frame */,
        u8_t /* rcpi */ ) { return false_t; }
    };

#endif // ABS_CORE_FRAME_HANDLER_H
