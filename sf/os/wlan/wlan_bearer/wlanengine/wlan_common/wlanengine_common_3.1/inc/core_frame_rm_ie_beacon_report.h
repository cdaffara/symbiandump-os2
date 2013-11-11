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


#ifndef CORE_FRAME_RM_IE_BEACON_REPORT_H
#define CORE_FRAME_RM_IE_BEACON_REPORT_H

#include "core_frame_rm_ie.h"
#include "core_frame_rm_ie_beacon_report_ie.h"
#include "core_types.h"
#include "core_ap_data.h"

/**
 * Class for parsing RM Beacon Report Element IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_rm_ie_beacon_report_c ) : public core_frame_rm_ie_c
    {

public:

    /**
     * Factory for creating a Beacon Report IE
     * instance with given sub-element.
     *
     * @since S60 v5.2
     * @param measurement_token Value for Measurement Token field of IE.
     * @param measurement_request_mode Value for Measurement Request Mode field of IE.
     * @param ie Pointer to sub-element.
     */
    static core_frame_rm_ie_beacon_report_c* instance(
        u8_t measurement_token,
        u8_t measurement_request_mode,
        core_frame_rm_ie_beacon_report_ie_c* ie );
    
    /**
     * Factory for creating a Beacon Report IE
     * instance with given sub-element.
     *
     * @since S60 v5.2
     * @param measurement_token Value for Measurement Token field of IE.
     * @param measurement_request_mode Value for Measurement Request Mode field of IE.
     * @param ie Pointer to sub-element.
     * @param frame_body_ie Pointer to frame body ie.
     */
    static core_frame_rm_ie_beacon_report_c* instance(
        u8_t measurement_token,
        u8_t measurement_request_mode,
        core_frame_rm_ie_beacon_report_ie_c* ie,
        core_frame_rm_ie_beacon_report_frame_body_ie_c* frame_body_ie );
    
    /** 
     * Destructor.
     */
    virtual ~core_frame_rm_ie_beacon_report_c();
    
    /**
     * Generate an IE header.
     *
     * @param measurement_token Value for Measurement Token field of IE.
     * @param measurement_request_mode Value for Measurement Request Mode field of IE.
     * @param ie Pointer to sub-element.
     */
    void generate(
            u8_t measurement_token,
            u8_t measurement_request_mode,
            core_frame_rm_ie_beacon_report_ie_c* ie );

    /**
     * Generate an IE header.
     *
     * @param measurement_token Value for Measurement Token field of IE.
     * @param measurement_request_mode Value for Measurement Request Mode field of IE.
     * @param ie Pointer to sub-element.
     * @param frame_body_ie Pointer to frame body ie.
     */
    void generate(
            u8_t measurement_token,
            u8_t measurement_request_mode,
            core_frame_rm_ie_beacon_report_ie_c* ie,
            core_frame_rm_ie_beacon_report_frame_body_ie_c* frame_body_ie );
    
private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     * @param max_data_length The maximum size of the frame buffer.    
     */
    core_frame_rm_ie_beacon_report_c(
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

#endif // CORE_FRAME_RM_IE_BEACON_REPORT_H

