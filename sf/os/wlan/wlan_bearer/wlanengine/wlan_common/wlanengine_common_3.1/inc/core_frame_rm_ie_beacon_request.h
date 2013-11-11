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
* Description:  Class for parsing RM Beacon Request IEs.
*
*/


#ifndef CORE_FRAME_RM_IE_BEACON_REQUEST_H
#define CORE_FRAME_RM_IE_BEACON_REQUEST_H

#include "core_frame_rm_ie.h"
#include "core_frame_rm_ie_beacon_request_ie.h"
#include "core_frame_rm_ie_beacon_request_detail_ie.h"
#include "core_types.h"

/**
 * Class for parsing RM Beacon Request Element IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_rm_ie_beacon_request_c ) : public core_frame_rm_ie_c
    {

public:
    enum core_frame_rm_ie_beacon_request_element_id_e
        {
        core_frame_rm_ie_beacon_request_element_id_beacon_reporting_information = 1,
        core_frame_rm_ie_beacon_request_element_id_beacon_reporting_detail      = 2
        };
    
    enum core_beacon_request_scan_mode_e
        {
        core_beacon_request_scan_mode_passive   = 0,
        core_beacon_request_scan_mode_active    = 1,
        core_beacon_request_scan_mode_beacon    = 2
        };
    
    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.2
     * @param ie Reference to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_rm_ie_beacon_request_c* instance(
        const core_frame_dot11_ie_c& ie );

    /** 
     * Destructor.
     */
    virtual ~core_frame_rm_ie_beacon_request_c();

    /**
     * Return the Regulatory Class field.
     * @return The Regulatory Class field.
     */
    u8_t regulatory_class() const;

    /**
     * Return the Channel Number field.
     * @return Channel Number field.
     */
    u8_t channel_number() const;

    /**
     * Return the Randomization Interval field.
     * @return Randomization Interval field.
     */
    u16_t randomization_interval() const;

    /**
     * Return the Measurement Duration field.
     * @return Measurement Duration field.
     */
    u16_t measurement_duration() const;

    /**
     * Return the Measurement Mode field.
     * @return Measurement Mode field.
     */
    u8_t measurement_mode() const;
    
    /**
     * Return the BSSID field.
     * @return BSSID field.
     */
    core_mac_address_s bssid() const;
    
    /**
     * Return the first IE of the Beacon Request.
     *
     * @since S60 v5.2
     * @return Pointer to the first IE, NULL if no IEs.
     */
    core_frame_dot11_ie_c* first_ie();

    /**
     * Return the next IE of the Beacon Request.
     *
     * @since S60 v5.2
     * @return Pointer to the next IE, NULL if no more IEs.
     */
    core_frame_dot11_ie_c* next_ie();
    
private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     * @param max_data_length Maximum length of the IE data.
     */
    core_frame_rm_ie_beacon_request_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );
    
    /**
     * Return the offset of the first IE.
     * @return The offset of the first IE. 0 if the frame doesn't support IEs.
     */
    u16_t first_ie_offset() const;
    
    /**
     * Return the length of the payload data.
     *
     * @since S60 v5.2
     * @return The length of the payload data.
     */
    u16_t payload_data_length() const;
    
    /**
     * Return a pointer to the payload data.
     *
     * @since S60 v5.2
     * @return A pointer to the payload data.
     */
    const u8_t* payload_data() const;

private: // data

    /** Pointer to the start of the current IE data. Not owned by this pointer. */
    const u8_t* current_ie_m;

    /** The maximum length of the current IE. */
    u16_t current_ie_max_length_m;    
    
    };

#endif // CORE_FRAME_RM_IE_BEACON_REQUEST_H
