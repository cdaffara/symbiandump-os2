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
* Description:  Class for parsing Ethernet frames
*
*/


#ifndef CORE_FRAME_ETHERNET_H
#define CORE_FRAME_ETHERNET_H

#include "core_types.h"

/**
 * Class for parsing Ethernet frames.
 *
 * @since S60 v3.2
 */
NONSHARABLE_CLASS( core_frame_ethernet_c )
    {

public:

    /**
     * Possible values for Ethernet Type field.
     */
    enum core_ethernet_type_e
        {
        core_ethernet_type_none = 0x0000,
        core_ethernet_type_ip   = 0x0800,
        core_ethernet_type_arp  = 0x0806,
        core_ethernet_type_test = 0x8013
        };

    /**
     * Factory for creating a parser for the given frame.
     *
     * @since S60 v3.2
     * @param data_length Length of the frame.
     * @param data Pointer to the frame data.
     * @param is_copied Whether the object should take a copy of the frame data.
     * @return Pointer to the created parser instance.
     */
    static core_frame_ethernet_c* instance(
        u16_t data_length,
        const u8_t* data,
        bool_t is_copied );

    /**
     * Factory for creating an Ethernet frame.
     *
     * @since S60 v3.2
     * @param max_data_length The maximum size of the frame.
     * @param destination The destination MAC field of the frame.
     * @param source The source MAC field of the frame.
     * @param type The Ethernet Type field of the frame.
     * @return Pointer to the created frame instance.
     */    
    static core_frame_ethernet_c* instance(
        u16_t max_data_length,
        const core_mac_address_s& destination,
        const core_mac_address_s& source,
        u16_t type );

    /**
     * Destructor.
     */
    virtual ~core_frame_ethernet_c();

    /**
     * Return the destination MAC field of the frame.
     *
     * @since S60 v3.2
     * @return The destination MAC field of the frame.
     */
    core_mac_address_s destination() const;

    /** 
     * Return the source MAC field of the frame.
     *
     * @since S60 v3.2
     * @return The source MAC field of the frame.
     */
    core_mac_address_s source() const;

    /**
     * Return the Ethernet Type field of the frame.
     *
     * @since S60 v3.2
     * @return The Ethernet Type field of the frame.
     */
    u16_t type() const;

    /**
     * Return the current length of the frame.
     *
     * @since S60 v3.1
     * @return The current length of the frame.
     */
    u16_t data_length() const;

    /**
     * Return a pointer to the frame data.
     *
     * @since S60 v3.1
     * @return A pointer to the frame data.
     */    
    const u8_t* data() const;

    /**
     * Return the length of the payload data.
     *
     * @since S60 v3.1
     * @return The length of the payload data.
     */
    u16_t payload_data_length() const;

    /**
     * Return a pointer to the payload data.
     *
     * @since S60 v3.1
     * @return A pointer to the payload data.
     */
    const u8_t* payload_data() const;

protected:

    /**
     * Return the MAC address field from the given index.
     *
     * @param index Index of the MAC address field.
     * @return The MAC address field from the given index.
     */
    core_mac_address_s mac_address(
        u16_t index ) const;    

    /**
     * Generate an Ethernet header.
     *
     * @param destination The destination MAC field of the frame.
     * @param source The source MAC field of the frame.
     * @param type The Ethernet Type field of the frame.
     */
    void generate(
        const core_mac_address_s& destination,
        const core_mac_address_s& source,
        u16_t type );

    /**
     * Constructor.
     *
     * @param data_length Length of the frame.
     * @param data Pointer to the frame data.
     * @param max_data_length The maximum size of the frame buffer.
     */
    core_frame_ethernet_c(
        u16_t data_length,
        const u8_t* data,
        u16_t max_data_length );

protected: // data

    /** The length of the frame. */
    u16_t data_length_m;     

    /** Pointer to the frame data. */
    u8_t* data_m;

    /** The maximum frame length when generating a frame or copying it. */
    const u16_t max_data_length_m;

    };

#endif // CORE_FRAME_ETHERNET_H
