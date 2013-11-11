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
* Description:  Class for parsing 802.11 frames
*
*/


#ifndef CORE_FRAME_DOT11_H
#define CORE_FRAME_DOT11_H

#include "core_types.h"

class core_frame_dot11_ie_c;

/**
 * Class for parsing 802.11 frames.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_frame_dot11_c )
    {

public:

    /**
     * Bit masks for 802.11 Frame Control fields.
     */
    enum core_dot11_frame_control_e
        {
        core_dot11_frame_control_version_mask          = 0x0003,
        core_dot11_frame_control_type_mask             = 0x000C,
        core_dot11_frame_control_subtype_mask          = 0x00F0,
        core_dot11_frame_control_type_subtype_mask     = 0x00FC,        
        core_dot11_frame_control_to_ds_mask            = 0x0100,
        core_dot11_frame_control_from_ds_mask          = 0x0200,
        core_dot11_frame_control_more_fragments_mask   = 0x0400,
        core_dot11_frame_control_retry_mask            = 0x0800,
        core_dot11_frame_control_power_mgmt_mask       = 0x1000,
        core_dot11_frame_control_more_data_mask        = 0x2000,
        core_dot11_frame_control_privacy_mask          = 0x4000,
        core_dot11_frame_control_order_mask            = 0x8000
        };

    /**
     * Enumerations for 802.11 Frame Control Type field.
     */
    enum core_dot11_frame_control_type_e
        {
        core_dot11_frame_control_type_management    = 0x0000,
        core_dot11_frame_control_type_control       = 0x0004,
        core_dot11_frame_control_type_data          = 0x0008
        };

    /**
     * Bit masks for 802.11 Frame Control Subtype field.
     */
    enum core_dot11_frame_control_subtype_e
        {
        core_dot11_frame_control_subtype_cf_ack_mask    = 0x0010,
        core_dot11_frame_control_subtype_cf_poll_mask   = 0x0020,
        core_dot11_frame_control_subtype_no_body_mask   = 0x0040,
        core_dot11_frame_control_subtype_qos_mask       = 0x0080
        };
    
    /**
     * Enumerations for 802.11 Type/Subtype combinations.
     */
    enum core_dot11_type_e
        {        
        core_dot11_type_association_req    = 0x0000,
        core_dot11_type_association_resp   = 0x0010,
        core_dot11_type_reassociation_req  = 0x0020,
        core_dot11_type_reassociation_resp = 0x0030,
        core_dot11_type_probe_req          = 0x0040,
        core_dot11_type_probe_resp         = 0x0050,
        core_dot11_type_beacon             = 0x0080,
        core_dot11_type_action             = 0x00D0,
        core_dot11_type_data               = 0x0008,
        core_dot11_type_qos_data           = 0x0088
        };

    /**
     * Definitions for Capability bitmask in beacons/probe responses.
     */
    enum core_dot11_capability_e
        {        
        core_dot11_capability_ess                 = 0x0001,
        core_dot11_capability_ibss                = 0x0002,
        core_dot11_capability_cf_pollable         = 0x0004,
        core_dot11_capability_cf_poll_request     = 0x0008,
        core_dot11_capability_privacy             = 0x0010,
        core_dot11_capability_preamble            = 0x0020,
        core_dot11_capability_pbcc                = 0x0040,
        core_dot11_capability_channel_agility     = 0x0080,
        core_dot11_capability_spectrum_mgmt       = 0x0100,
        core_dot11_capability_qos                 = 0x0200,
        core_dot11_capability_slot_time           = 0x0400,
        core_dot11_capability_apsd                = 0x0800,
        core_dot11_capability_radio_measurement   = 0x1000,
        core_dot11_capability_dsss_ofdm           = 0x2000,
        core_dot11_capability_delayed_block_ack   = 0x4000,
        core_dot11_capability_immediate_block_ack = 0x8000
        };

    /**
     * Factory for creating a parser for the given frame.
     *
     * @since S60 v3.1
     * @param data_length Length of the frame.
     * @param data Pointer to the frame data.
     * @param is_copied Whether the object should take a copy of the frame data.
     * @return Pointer to the created parser instance.
     */
    static core_frame_dot11_c* instance(
        u16_t data_length,
        const u8_t* data,
        bool_t is_copied );

    /**
     * Destructor.
     */
    virtual ~core_frame_dot11_c();

    /**
     * Return the frame control field of the frame.
     * 
     * @since S60 v3.1
     * @return The frame control field of the frame.
     */
    u16_t frame_control() const;
    
    /**
     * Return the type of the frame.
     * 
     * @since S60 v3.1
     * @return type of the frame
     */
    core_frame_dot11_c::core_dot11_type_e type() const;
    
    /** 
     * Return the duration field of the frame.
     *
     * @since S60 v3.1
     * @return The duration field of the frame.
     */
    u16_t duration() const;

    /**
     * Return the destination MAC field of the frame.
     *
     * @since S60 v3.1
     * @return The destination MAC field of the frame.
     */
    core_mac_address_s destination() const;

    /** 
     * Return the source MAC field of the frame.
     *
     * @since S60 v3.1
     * @return The source MAC field of the frame.
     */
    core_mac_address_s source() const;

    /**
     * Return the BSSID field of the frame.
     *
     * @since S60 v3.1
     * @return The BSSID field of the frame.
     */
    core_mac_address_s bssid() const;

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

    /**
     * Return the first IE of the frame.
     *
     * @since S60 v3.1
     * @return Pointer to the first IE, NULL if no IEs.
     */
     core_frame_dot11_ie_c* first_ie();

    /**
     * Return the next IE of the frame.
     *
     * @since S60 v3.1
     * @return Pointer to the next IE, NULL if no more IEs.
     */
     core_frame_dot11_ie_c* next_ie();

    /**
     * Append an IE to frame.
     *
     * @since S60 v3.1     
     * @param ie IE to add to the frame. 
     */
    void append_ie(
        const core_frame_dot11_ie_c* ie );
    
    /**
     * Set Destination MAC Address of the frame.
     *
     * @since S60 v5.2     
     * @param da Destination Address to set. 
     */
    void set_destination( const core_mac_address_s& da );
    
    /**
     * Set Source MAC Address of the frame.
     *
     * @since S60 v5.2     
     * @param sa Source Address to set. 
     */
    void set_source( const core_mac_address_s& sa );

    /**
     * Return the offset of the first fixed element.
     * 
     * @since S60 v5.2     
     * @return The offset of the first fixed element.
     */
    u16_t first_fixed_element_offset() const;

protected:

    /**
     * Return the offset of the first IE.
     * @return The offset of the first IE. 0 if the frame doesn't support IEs.
     */
    virtual u16_t first_ie_offset() const;

    /**
     * Return the MAC address field from the given index.
     *
     * @param index Index of the MAC address field.
     * @return The MAC address field from the given index.
     */
    core_mac_address_s mac_address(
        u16_t index ) const;    

    /**
     * Generate a 802.11 header.
     *
     * @param type The Type and Subtype field of the header.
     * @param duration The Duration field of the frame.
     * @param destination The destination MAC field of the frame.
     * @param source The source MAC field of the frame.
     * @param bssid The BSSID field of the frame.
     * @param sequence_control The Sequence Control field of the frame.
     */
    void generate(
        u8_t type,
        u16_t duration,
        const core_mac_address_s& destination,
        const core_mac_address_s& source,
        const core_mac_address_s& bssid,
        u16_t sequence_control );

    /**
     * Constructor.
     *
     * @param data_length Length of the frame.
     * @param data Pointer to the frame data.
     * @param max_data_length The maximum size of the frame buffer.
     */
    core_frame_dot11_c(
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

private: // data

    /** Whether this frame includes QoS Control field. */
    bool_t is_qos_m;

    /** Whether this frame includes HT Control field. */
    bool_t is_ht_m;

    /** Pointer to the start of the current IE data. Not owned by this pointer. */
    const u8_t* current_ie_m;

    /** The maximum length of the current IE. */
    u16_t current_ie_max_length_m;

    };

#endif // CORE_FRAME_DOT11_H
