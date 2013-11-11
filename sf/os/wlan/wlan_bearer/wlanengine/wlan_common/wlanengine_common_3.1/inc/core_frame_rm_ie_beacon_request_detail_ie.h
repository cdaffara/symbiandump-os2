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
* Description:  Class parsing 802.11k Beacon Request Detail IEs.
*
*/


#ifndef CORE_FRAME_RM_IE_BEACON_REQUEST_DETAIL_IE_H
#define CORE_FRAME_RM_IE_BEACON_REQUEST_DETAIL_IE_H

#include "core_types.h"
#include "core_frame_dot11_ie.h"

/**
 * Class parsing 802.11k Beacon Request Detail IEs.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_rm_ie_beacon_request_detail_ie_c ) : public core_frame_dot11_ie_c
    {

public:
    
    
    enum core_frame_rm_ie_brd_reporting_detail_e
        {
        core_frame_rm_ie_brd_reporting_detail_no_fields_or_elements         = 0,
        core_frame_rm_ie_brd_reporting_detail_fields_and_requested_elements = 1,
        core_frame_rm_ie_brd_reporting_detail_default                       = 2
        };

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.2
     * @param ie Pointer to the IE data.
     * @return Pointer to the created parser instance.
     */
    static core_frame_rm_ie_beacon_request_detail_ie_c* instance(
            const core_frame_dot11_ie_c& ie );

    /**
     * Return the Reporting Detail field of the IE.
     *
     * @since S60 v5.2
     * @return The Reporting Detail field of the IE.
     */
    u8_t reporting_detail() const;
    
private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.
     * @param max_data_length Maximum length of the IE data.
     */
    core_frame_rm_ie_beacon_request_detail_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

#endif // CORE_FRAME_RM_IE_BEACON_REQUEST_DETAIL_IE_H
