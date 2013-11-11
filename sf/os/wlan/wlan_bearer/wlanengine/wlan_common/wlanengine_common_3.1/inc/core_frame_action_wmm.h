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
* Description:  Class for parsing and generating 802.11 action frames in WMM category.
*
*/


#ifndef CORE_FRAME_ACTION_WMM_H
#define CORE_FRAME_ACTION_WMM_H

#include "am_platform_libraries.h"
#include "core_types.h"
#include "core_frame_action.h"

class core_frame_wmm_ie_tspec_c;

/**
 * Class for parsing and generating 802.11 action frames in WMM category.
 *
 * @lib wlmserversrv.lib
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_action_wmm_c ) : public core_frame_action_c
    {

public:

    /**
     * Defines the possible Type values.
     */
    enum core_dot11_action_wmm_type_e
        {
        core_dot11_action_wmm_type_addts_req  = 0,
        core_dot11_action_wmm_type_addts_resp = 1,
        core_dot11_action_wmm_type_delts      = 2
        };

    /**
     * Defines the possible Status Code values.
     */
    enum core_dot11_action_wmm_status_e
        {
        core_dot11_action_wmm_status_admission_accepted = 0,
        core_dot11_action_wmm_status_invalid_parameters = 1,
        core_dot11_action_wmm_status_refused            = 3
        /** 2, 4 - 255 reserved */        
        };    

    /**
     * Factory for creating a parser for the given frame.
     *
     * @since S60 v3.2
     * @param frame Frame to be parsed.
     * @return Pointer to the created parser instance.
     */
    static core_frame_action_wmm_c* instance(
        const core_frame_action_c& frame );

    /**
     * Generate a 802.11 action frame in WWMM category.
     *
     * @since S60 v3.2
     * @param duration The Duration field of the frame.
     * @param destination The destination MAC field of the frame.
     * @param source The source MAC field of the frame.
     * @param bssid The BSSID field of the frame.
     * @param sequence_control The Sequence Control field of the frame.
     * @param action_type Action Type field of the frame.
     * @param dialog_token The Dialog Token field of the frame.
     * @param status The Status field of the frame.
     * @param tspec The TSPEC element of the frame.
     * @return Pointer to the created frame instance.
     */
    static core_frame_action_wmm_c* instance(
        u16_t duration,
        const core_mac_address_s& destination,
        const core_mac_address_s& source,
        const core_mac_address_s& bssid,
        u16_t sequence_control,
        u8_t action_type,
        u8_t dialog_token,
        u8_t status,
        const core_frame_wmm_ie_tspec_c* tspec );

    /**
     * Destructor.
     */
    virtual ~core_frame_action_wmm_c();

    /**
     * Return the Action Type field of the header.
     */
    u8_t action_type() const;

    /**
     * Return the Dialog Token field of the header.
     */
    u8_t dialog_token() const;

    /**
     * Return the Status field of the header.
     */
    u8_t status() const;

private:

    /**
     * Return the offset of the first IE.
     * @return The offset of the first IE. 0 if the frame doesn't support IEs.
     */
    u16_t first_ie_offset() const;

    /**
     * Generate a 802.11 ADDTS request frame.
     *
     * @param duration The Duration field of the frame.
     * @param destination The destination MAC field of the frame.
     * @param source The source MAC field of the frame.
     * @param bssid The BSSID field of the frame.
     * @param sequence_control The Sequence Control field of the frame.
     * @param action_type Action Type field of the frame.
     * @param dialog_token The Dialog Token field of the frame.
     * @param status The Status field of the frame.
     * @param tspec The TSPEC element of the frame.
     */
    void generate(
        u16_t duration,
        const core_mac_address_s& destination,
        const core_mac_address_s& source,
        const core_mac_address_s& bssid,
        u16_t sequence_control,
        u8_t action_type,
        u8_t dialog_token,
        u8_t status,
        const core_frame_wmm_ie_tspec_c* tspec );

    /**
     * Constructor.
     *
     * @param data_length Length of the frame.
     * @param data Pointer to the frame data.
     * @param max_data_length The maximum size of the frame buffer.    
     */
    core_frame_action_wmm_c(
        u16_t data_length,
        const u8_t* data,
        u16_t max_data_length );

    };

#endif // CORE_FRAME_ACTION_WMM_H
