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
* Description:  Class for parsing RM Beacon Report IEs.
*
*/


#ifndef CORE_FRAME_RM_IE_BEACON_REPORT_IE_H
#define CORE_FRAME_RM_IE_BEACON_REPORT_IE_H

#include "core_frame_rm_ie.h"
#include "core_frame_rm_ie_beacon_report_frame_body_ie.h"
#include "core_types.h"
#include "core_ap_data.h"

/**
 * Class for parsing Beacon Report Element IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_rm_ie_beacon_report_ie_c ) : public core_frame_dot11_ie_c
    {

public:

    /**
     * Factory for creating a Beacon Report IE
     * instance with given parameters.
     *
     * @since S60 v5.2
     * @param regulatory_class Value for Regulatory Class field of IE.
     * @param measurement_duration Value for Measurement Duration field of IE.
     * @param reported_frame_information Value for Reported Frame Information field of IE.
     * @param antenna_id Value for Antenna ID field of IE.
     * @param ap_data Reference to core_ap_data_c from which to generate the information.
     */
    static core_frame_rm_ie_beacon_report_ie_c* instance(
        u8_t regulatory_class,
        u16_t measurement_duration,
        u8_t reported_frame_information,
        u8_t antenna_id,
        core_ap_data_c& ap_data );

    /**
     * Factory for creating a Beacon Report IE
     * instance with given parameters.
     *
     * @since S60 v5.2
     * @param regulatory_class Value for Regulatory Class field of IE.
     * @param measurement_duration Value for Measurement Duration field of IE.
     * @param reported_frame_information Value for Reported Frame Information field of IE.
     * @param antenna_id Value for Antenna ID field of IE.
     * @param ap_data Reference to core_ap_data_c from which to generate the information.
     * @param frame_body_ie Pointer to frame body ie.
     */
    static core_frame_rm_ie_beacon_report_ie_c* instance(
        u8_t regulatory_class,
        u16_t measurement_duration,
        u8_t reported_frame_information,
        u8_t antenna_id,
        core_ap_data_c& ap_data,
        core_frame_rm_ie_beacon_report_frame_body_ie_c* frame_body_ie );
    
    /** 
     * Destructor.
     */
    virtual ~core_frame_rm_ie_beacon_report_ie_c();
    
    /**
     * Generate an IE header.
     *
     * @param regulatory_class Value for Regulatory Class field of IE.
     * @param actual_measurement_start_time Value for Actual Measurement Start Time field of IE.
     * @param measurement_duration Value for Measurement Duration field of IE.
     * @param reported_frame_information Value for Reported Frame Information field of IE.
     * @param rsni Value for RSNI field of IE.
     * @param antenna_id Value for Antenna ID field of IE.
     * @param parent_tsf Value for Parent TSF field of IE.
     * @param ap_data Reference to core_ap_data_c from which to generate the information.
     */
    void generate(
            u8_t regulatory_class,
            core_tsf_value_s& actual_measurement_start_time,
            u16_t measurement_duration,
            u8_t reported_frame_information,
            u8_t rsni,
            u8_t antenna_id,
            core_tsf_value_s& parent_tsf,
            core_ap_data_c& ap_data );

    /**
     * Generate an IE header.
     *
     * @param regulatory_class Value for Regulatory Class field of IE.
     * @param actual_measurement_start_time Value for Actual Measurement Start Time field of IE.
     * @param measurement_duration Value for Measurement Duration field of IE.
     * @param reported_frame_information Value for Reported Frame Information field of IE.
     * @param rsni Value for RSNI field of IE.
     * @param antenna_id Value for Antenna ID field of IE.
     * @param parent_tsf Value for Parent TSF field of IE.
     * @param ap_data Reference to core_ap_data_c from which to generate the information.
     * @param frame_body_ie Pointer to frame body ie.
     */
    void generate(
            u8_t regulatory_class,
            core_tsf_value_s& actual_measurement_start_time,
            u16_t measurement_duration,
            u8_t reported_frame_information,
            u8_t rsni,
            u8_t antenna_id,
            core_tsf_value_s& parent_tsf,
            core_ap_data_c& ap_data,
            core_frame_rm_ie_beacon_report_frame_body_ie_c* frame_body_ie );
    
    /**
     * Return the length of the IE.
     *
     * @since S60 v5.2
     * @return The length of the IE.
     */
    u8_t length() const;
    
private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     * @param max_data_length The maximum size of the frame buffer.    
     */
    core_frame_rm_ie_beacon_report_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

private: // data

    /** Pointer to the start of the current IE data. Not owned by this pointer. */
    const u8_t* current_ie_m;

    /** The maximum length of the current IE. */
    u16_t current_ie_max_length_m;    
    
    };

#endif // CORE_FRAME_RM_IE_BEACON_REPORT_IE_H


