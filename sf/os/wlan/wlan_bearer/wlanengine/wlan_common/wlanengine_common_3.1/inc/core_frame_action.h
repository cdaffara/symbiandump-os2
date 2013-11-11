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
* Description:  Class for parsing and generating 802.11 action frames.
*
*/


#ifndef CORE_FRAME_ACTION_H
#define CORE_FRAME_ACTION_H

#include "am_platform_libraries.h"
#include "core_types.h"
#include "core_frame_dot11.h"

/**
 * Class for parsing and generating 802.11 action frames.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_action_c ) : public core_frame_dot11_c
    {

public:

    /**
     * Defines the possible Category values
     */
    enum core_dot11_action_category_e
        {
        core_dot11_action_category_spectrum_mgmt = 0,
        core_dot11_action_category_qos           = 1,
        core_dot11_action_category_rm            = 5,
        core_dot11_action_category_wmm_qos       = 17
        };

    /**
     * Factory for creating a parser for the given frame.
     *
     * @since S60 v3.1
     * @param frame Frame to be parsed.
     * @return Pointer to the created parser instance.
     */
    static core_frame_action_c* instance(
        const core_frame_dot11_c& frame );

    /**
     * Destructor.
     */
    virtual ~core_frame_action_c();

    /**
     * Return the Category field of the header.
     */
    u8_t category() const;
    
    /**
     * Set the Category field of the header.
     * 
     * @since S60 v5.2
     * @param cat New value for the Category field of the header.
     */
    void set_category( u8_t cat );

protected:

    /**
     * Generate a 802.11 action frame.
     *
     * @param duration The Duration field of the frame.
     * @param destination The destination MAC field of the frame.
     * @param source The source MAC field of the frame.
     * @param bssid The BSSID field of the frame.
     * @param sequence_control The Sequence Control field of the frame.
     * @param Category field of the frame.
     */
    void generate(
        u16_t duration,
        const core_mac_address_s& destination,
        const core_mac_address_s& source,
        const core_mac_address_s& bssid,
        u16_t sequence_control,
        u8_t category );

    /**
     * Constructor.
     *
     * @param data_length Length of the frame.
     * @param data Pointer to the frame data.
     * @param max_data_length The maximum size of the frame buffer.    
     */
    core_frame_action_c(
        u16_t data_length,
        const u8_t* data,
        u16_t max_data_length );

    };

#endif // CORE_FRAME_ACTION_H
