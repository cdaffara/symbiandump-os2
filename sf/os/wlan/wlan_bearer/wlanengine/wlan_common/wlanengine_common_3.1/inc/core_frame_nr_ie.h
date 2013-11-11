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
* Description:  Class for parsing Neighbor Report IEs.
*
*/


#ifndef CORE_FRAME_NR_IE_H
#define CORE_FRAME_NR_IE_H

#include "core_frame_dot11_ie.h"
#include "core_types.h"

/**
 * Class for parsing NR Parameter Element IE.
 *
 * @since S60 v5.2
 */
NONSHARABLE_CLASS( core_frame_nr_ie_c ) : public core_frame_dot11_ie_c
    {

public:
    
    /**
     * Factory for creating a parser instance.
     *
     * @since S60 v5.2
     * @param ie Pointer to the IE data.
     * @return A pointer to the created IE instance.
     */
    static core_frame_nr_ie_c* instance(
        const core_frame_dot11_ie_c& ie );

    /**
     * Factory for creating a Measurement Report IE
     * instance with given parameters.
     *
     * @since S60 v5.2
     * @param 
     */
    static core_frame_nr_ie_c* instance(
        const core_ssid_s& ssid );
    
    /** 
     * Destructor.
     */
    virtual ~core_frame_nr_ie_c();

    /**
     * Return the BSS Id field.
     * 
     * @since S60 v5.2
     * @return The BSS Id field.
     */
    core_mac_address_s bssid() const;

    /**
     * Return the BSS Id info field.
     * 
     * @since S60 v5.2
     * @return The BSS Id info field.
     */
    u32_t bssid_info() const;
    
    /**
     * Return the regulatory class field.
     * 
     * @since S60 v5.2
     * @return The regulatory class field.
     */
    u8_t regulatory_class() const;

    /**
     * Return the Channel Number field.
     * 
     * @since S60 v5.2
     * @return The Channel Number field.
     */
    u8_t channel_number() const;

    /**
     * Return the phy type field.
     * 
     * @since S60 v5.2
     * @return The phy type field.
     */
    u8_t phy_type() const;

protected:

    /**
     * Constructor
     *
     * @param data_length Length of the IE data.
     * @param data Pointer to the IE data.     
     * @param max_data_length Maximum length of the IE data.
     */
    core_frame_nr_ie_c(
       u16_t data_length,
       const u8_t* data,
       u16_t max_data_length );

    /**
     * Generate an IE header.
     *
     * @since S60 v5.2
     * @param ssid Reference to ssid.
     */
    void generate(
        const core_ssid_s& ssid );
    
    };

#endif // CORE_FRAME_NR_IE_H
