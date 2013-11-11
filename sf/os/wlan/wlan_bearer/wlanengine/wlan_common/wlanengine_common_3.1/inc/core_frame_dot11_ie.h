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
* Description:  Class parsing 802.11 IEs.
*
*/


#ifndef CORE_FRAME_DOT11_IE_H
#define CORE_FRAME_DOT11_IE_H

#include "core_types.h"

const u8_t CORE_FRAME_DOT11_IE_HEADER_LENGTH = 2;

/**
 * Class parsing 802.11 IEs.
 *
 * This class acts as a base class for the various IE parsers.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_dot11_ie_c )
    {

public:

    enum core_frame_dot11_ie_element_id_e
        {
        core_frame_dot11_ie_element_id_ssid                  = 0,
        core_frame_dot11_ie_element_id_supported_rates       = 1,
        core_frame_dot11_ie_element_id_fh_parameter_set      = 2,
        core_frame_dot11_ie_element_id_ds_parameter_set      = 3,
        core_frame_dot11_ie_element_id_cf_paremeter_set      = 4,
        core_frame_dot11_ie_element_id_tim                   = 5,
        core_frame_dot11_ie_element_id_ibss_parameter_set    = 6,
        core_frame_dot11_ie_element_id_country               = 7,
        core_frame_dot11_ie_element_id_hopping_pattern       = 8,
        core_frame_dot11_ie_element_id_hopping_pattern_table = 9,
        core_frame_dot11_ie_element_id_request               = 10,
        core_frame_dot11_ie_element_id_qbss_load             = 11,
        core_frame_dot11_ie_element_id_challenge_text        = 16,
        // Reserved for challenge text extension 17 - 31
        core_frame_dot11_ie_element_id_power_constraint      = 32,
        core_frame_dot11_ie_element_id_power_capability      = 33,
        core_frame_dot11_ie_element_id_measurement_req       = 38,
        core_frame_dot11_ie_element_id_measurement_resp      = 39,
        core_frame_dot11_ie_element_id_erp_information       = 42,
        core_frame_dot11_ie_element_id_rsn                   = 48,
        core_frame_dot11_ie_element_id_extended_rates        = 50,
        core_frame_dot11_ie_element_id_ap_channel_report     = 51,
        core_frame_dot11_ie_element_id_neighbor_report       = 52,
        core_frame_dot11_ie_element_id_admission_capacity    = 67,
        core_frame_dot11_ie_element_id_wapi                  = 68,
        core_frame_dot11_ie_element_id_rrm_capabilities      = 70,
        core_frame_dot11_ie_element_id_wpa                   = 221,
        core_frame_dot11_ie_element_id_wmm_parameter         = 221,
        core_frame_dot11_ie_element_id_wmm_tspec             = 221
        };

    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v3.1
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.
     * @param is_copied Whether the object should take a copy of the frame data. Default value is false_t.
     * @return Pointer to the created parser instance.
     */
    static core_frame_dot11_ie_c* instance(
        u16_t data_length,
        const u8_t* data,
        bool_t is_copied = false_t );

    /** 
     * Destructor.
     */
    virtual ~core_frame_dot11_ie_c();

    /**
     * Return the element ID field of the IE.
     * 
     * @since S60 v3.1
     * @return The element ID field of the IE.
     */
    u8_t element_id() const;
    
    /**
     * Return the length field of the IE.
     *
     * @since S60 v3.1
     * @return The length field of the IE.
     */
    u8_t length() const;
    
    /**
     * Return the data of the IE, including the IE headers.
     *
     * @since S60 v3.1
     * @return The total length of the IE.
     */    
    u16_t data_length() const;

    /**
     * Return a pointer to the IE data.
     *
     * @since S60 v3.1
     * @return A pointer to the IE data.
     */
    const u8_t* data() const;

protected:

    /**
     * Generate a IE header.
     *
     * @param element_id The element ID field of the header.
     */
    void generate(
        u8_t element_id );

    /**
     * Set the total length of the IE to the given value.
     *
     * @param length The total length of the IE.
     */
    void set_length(
        u8_t length );

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.
     * @param max_data_length
     */
    core_frame_dot11_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

protected:

    /** The length of the IE data. */
    u16_t data_length_m;

    /** Pointer to the IE data. */
    u8_t* data_m;

    /** The maximum IE length when generating an IE. */
    u16_t max_data_length_m;

    };

#endif // CORE_FRAME_DOT11_IE_H
