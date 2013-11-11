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
* Description:  Class for parsing RM Beacon Report Frame Body IEs.
*
*/


#ifndef CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_H
#define CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_H

#include "core_frame_rm_ie.h"
#include "core_types.h"
#include "core_ap_data.h"

/**
 * Class for parsing Beacon Report Element IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_rm_ie_beacon_report_frame_body_ie_c ) : public core_frame_dot11_ie_c
    {

public:

    enum core_frame_rm_ie_beacon_report_frame_body_ie_element_id_e
        {
        core_frame_rm_ie_beacon_report_frame_body_ie_element_id_reported_frame_body = 1
        };

    /**
     * Factory for creating a Beacon Report Frame Body IE
     * instance with given parameters.
     *
     * @since S60 v5.2
     * @param ap_data Reference to core_ap_data_c from which to generate the information.
     */
    static core_frame_rm_ie_beacon_report_frame_body_ie_c* instance(
        core_ap_data_c& ap_data );

    /**
     * Factory for creating a Beacon Report Frame Body IE
     * instance with given parameters.
     *
     * @since S60 v5.2
     * @param ap_data Reference to core_ap_data_c from which to generate the information.
     * @param ie_id_list Information element id list.
     * @param ie_id_list_length Information element id list length.
     */
    static core_frame_rm_ie_beacon_report_frame_body_ie_c* instance(
        core_ap_data_c& ap_data,
        u8_t* ie_id_list,
        u8_t ie_id_list_length );

    /** 
     * Destructor.
     */
    virtual ~core_frame_rm_ie_beacon_report_frame_body_ie_c();
    
    /**
     * Generate an IE header.
     *
     * @param ap_data Reference to core_ap_data_c from which to generate the information.
     */
    void generate( core_ap_data_c& ap_data );
    
    /**
     * Generate an IE header.
     *
     * @param ap_data Reference to core_ap_data_c from which to generate the information.
     * @param ie_id_list Information element id list.
     * @param ie_id_list_length Information element id list length.
     */
    void generate( 
        core_ap_data_c& ap_data,
        u8_t* ie_id_list,
        u8_t ie_id_list_length );
    
private:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     * @param max_data_length The maximum size of the frame buffer.    
     */
    core_frame_rm_ie_beacon_report_frame_body_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

private: // data

    /** Pointer to the start of the current IE data. Not owned by this pointer. */
    const u8_t* current_ie_m;

    /** The maximum length of the current IE. */
    u16_t current_ie_max_length_m;    
    
    };

#endif // CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_H


