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
* Description:  Class for parsing RM IEs.
*
*/


#ifndef CORE_FRAME_RM_IE_H
#define CORE_FRAME_RM_IE_H

#include "core_frame_dot11_ie.h"
#include "core_types.h"

/**
 * Class for parsing RM Parameter Element IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_rm_ie_c ) : public core_frame_dot11_ie_c
    {

public:
    
    enum core_frame_rm_ie_action_type_e
        {
        core_frame_rm_ie_action_type_beacon_request = 5
        };
    
    /**
     * Factory for creating a parser instance for receiving an IE.
     *
     * @since S60 v5.2
     * @param ie Reference to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_rm_ie_c* instance(
        const core_frame_dot11_ie_c& ie );

    /**
     * Factory for creating a Measurement Report IE
     * instance with given parameters.
     *
     * @since S60 v5.2
     * @param measurement_token Value for Measurement Token field of IE.
     * @param measurement_request_mode Value for Measurement Request Mode field of IE.
     * @param measurement_type Value for Measurement Type field of IE.
     * @return A pointer to the created IE instance.
     */
    static core_frame_rm_ie_c* instance(
        u8_t measurement_token,
        u8_t measurement_request_mode,
        u8_t measurement_type );
    
    /** 
     * Destructor.
     */
    virtual ~core_frame_rm_ie_c();

    /**
     * Return the Measurement Token field.
     * @return The Measurement Token field.
     */
    u8_t measurement_token() const;

    /**
     * Return the Measurement Request Mode field.
     * @return Measurement Request Mode field.
     */
    u8_t measurement_request_mode() const;

    /**
     * Return the Measurement Type field.
     * @return Measurement Type field.
     */
    u8_t measurement_type() const;

protected:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     * @param max_data_length Maximum length of the IE data.
     */
    core_frame_rm_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    /**
     * Generate an IE header.
     *
     * @param measurement_token Value for Measurement Token field of IE.
     * @param measurement_request_mode Value for Measurement Request Mode field of IE.
     * @param measurement_type Value for Measurement Type field of IE.
     */
    void generate(
        u8_t measurement_token,
        u8_t measurement_request_mode,
        u8_t measurement_type );
    
    
    };

#endif // CORE_FRAME_RM_IE_H
