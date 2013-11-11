/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for parsing and generating 802.11 action frames in RM category.
*
*/


#ifndef CORE_FRAME_RADIO_MEASUREMENT_ACTION_H
#define CORE_FRAME_RADIO_MEASUREMENT_ACTION_H

#include "am_platform_libraries.h"
#include "core_types.h"
#include "core_frame_action.h"

/**
 * Class for parsing and generating 802.11 action frames in RM category.
 *
 * @lib wlmserversrv.lib
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_radio_measurement_action_c ) : public core_frame_action_c
    {

public:

    /**
     * Defines the possible Type values.
     */
    enum core_dot11_action_rm_type_e
        {
        core_dot11_action_rm_type_meas_req      = 0,
        core_dot11_action_rm_type_meas_resp     = 1,
        core_dot11_action_rm_type_neighbor_req  = 4,
        core_dot11_action_rm_type_neighbor_resp = 5
        };

    /**
     * Factory for creating a parser for the given frame.
     *
     * @since S60 v5.2
     * @param frame Frame to be parsed.
     * @param is_copied Frame is copied.
     * @return Pointer to the created parser instance.
     */
    static core_frame_radio_measurement_action_c* instance(
        const core_frame_action_c& frame,
        bool_t is_copied );

    /**
     * Destructor.
     *     
     * @since S60 v5.2
     */
    virtual ~core_frame_radio_measurement_action_c();

    /**
     * Return the Action Type field of the header.
     * 
     * @since S60 v5.2
     */
    u8_t action_type() const;

private:

    /**
     * Constructor.
     *  
     * @since S60 v5.2
     * @param data_length Length of the frame.
     * @param data Pointer to the frame data.
     * @param max_data_length The maximum size of the frame buffer.    
     */
    core_frame_radio_measurement_action_c(
        u16_t data_length,
        const u8_t* data,
        u16_t max_data_length );

    };

#endif // CORE_FRAME_RADIO_MEASUREMENT_ACTION_H
