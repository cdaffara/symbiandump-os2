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
* Description:  Class parsing 802.11k Beacon Request IEs.
*
*/


#ifndef CORE_FRAME_RM_IE_BEACON_REQUEST_IE_H
#define CORE_FRAME_RM_IE_BEACON_REQUEST_IE_H

#include "core_types.h"
#include "core_frame_dot11_ie.h"

/**
 * Class parsing 802.11k Beacon Request IEs.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_rm_ie_beacon_request_ie_c ) : public core_frame_dot11_ie_c
    {

public:
    
    
    enum core_frame_rm_ie_bri_reporting_condition_e
        {
        core_frame_rm_ie_bri_reporting_condition_default = 0
        };

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.2
     * @param ie Reference to the IE data.
     * @return Pointer to the created parser instance.
     */
    static core_frame_rm_ie_beacon_request_ie_c* instance(
            const core_frame_dot11_ie_c& ie );

    /**
     * Return the Reporting Condition field of the IE.
     *
     * @since S60 v5.2
     * @return The Reporting Condition field of the IE.
     */
    u8_t reporting_condition() const;
    
    /**
     * Return the Threshold/Offset Reference Value of the IE.
     *
     * @since S60 v5.2
     * @return The Threshold/Offset Reference Value field of the IE.
     */
    u8_t threshold_reference_value() const;
    
private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.
     * @param max_data_length Maximum length of the IE data.
     */
    core_frame_rm_ie_beacon_request_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    };

#endif // CORE_FRAME_RM_IE_BEACON_REQUEST_IE_H
