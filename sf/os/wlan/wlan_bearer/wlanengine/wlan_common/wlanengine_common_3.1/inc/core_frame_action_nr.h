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
* Description:  Class for parsing and generating 802.11 action frames for Neighbor Reports.
*
*/


#ifndef CORE_FRAME_ACTION_NR_H
#define CORE_FRAME_ACTION_NR_H

#include "am_platform_libraries.h"
#include "core_types.h"
#include "core_frame_action.h"

class core_frame_nr_ie_c;

/**
 * Class for parsing and generating 802.11 action frames for Neighbor Reports.
 *
 * @lib wlmserversrv.lib
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_action_nr_c ) : public core_frame_action_c
    {

public:

    /**
     * Factory for creating a parser for the given frame.
     *
     * @since S60 v5.2
     * @param frame Frame to be parsed.
     * @param is_copied Frame is copied.
     * @return Pointer to the created parser instance.
     */
    static core_frame_action_nr_c* instance(
        const core_frame_action_c& frame,
        bool_t is_copied );

    /**
     * Generate a 802.11 action frame in RM category.
     *
     * @since S60 v5.2
     * @param duration The Duration field of the frame.
     * @param destination The destination MAC field of the frame.
     * @param source The source MAC field of the frame.
     * @param bssid The BSSID field of the frame.
     * @param sequence_control The Sequence Control field of the frame.
     * @param action_type Action Type field of the frame.
     * @param dialog_token The Dialog Token field of the frame.
     * @param nr_ie Reference to the NR information element of the frame.
     * @return Pointer to the created frame instance.
     */
    static core_frame_action_nr_c* instance(
        u16_t duration,
        const core_mac_address_s& destination,
        const core_mac_address_s& source,
        const core_mac_address_s& bssid,
        u16_t sequence_control,
        u8_t action_type,
        u8_t dialog_token,
        const core_frame_nr_ie_c* nr_ie );

    /**
     * Destructor.
     *     
     * @since S60 v5.2
     */
    virtual ~core_frame_action_nr_c();

    /**
     * Return the Dialog Token field of the header.
     * 
     * @since S60 v5.2
     */
    u8_t dialog_token() const;

private:

    /**
     * Generate a 802.11 neighbor report request frame.
     *  
     * @since S60 v5.2
     * @param duration The Duration field of the frame.
     * @param destination The destination MAC field of the frame.
     * @param source The source MAC field of the frame.
     * @param bssid The BSSID field of the frame.
     * @param sequence_control The Sequence Control field of the frame.
     * @param action_type Action Type field of the frame.
     * @param dialog_token The Dialog Token field of the frame.
     * @param nr_ie Reference to the NR information element of the frame.
     */
    void generate(
        u16_t duration,
        const core_mac_address_s& destination,
        const core_mac_address_s& source,
        const core_mac_address_s& bssid,
        u16_t sequence_control,
        u8_t action_type,
        u8_t dialog_token,
        const core_frame_nr_ie_c* nr_ie );

    /**
     * Constructor.
     *  
     * @since S60 v5.2
     * @param data_length Length of the frame.
     * @param data Pointer to the frame data.
     * @param max_data_length The maximum size of the frame buffer.    
     */
    core_frame_action_nr_c(
        u16_t data_length,
        const u8_t* data,
        u16_t max_data_length );

protected:
    
    /**
     * Return the offset of the first IE.
     * @return The offset of the first IE. 0 if the frame doesn't support IEs.
     */
    virtual u16_t first_ie_offset() const;

    };

#endif // CORE_FRAME_ACTION_NR_H
