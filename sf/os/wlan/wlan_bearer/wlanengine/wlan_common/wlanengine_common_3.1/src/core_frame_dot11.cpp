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


#include "core_frame_dot11.h"
#include "core_frame_dot11_ie.h"
#include "core_tools.h"
#include "am_debug.h"

const u16_t CORE_DOT11_LENGTH = 24;
const u16_t CORE_DOT11_QOS_CONTROL_LENGTH = 2;
const u16_t CORE_DOT11_HT_CONTROL_LENGTH = 4;
const u16_t CORE_DOT11_FRAME_CONTROL_INDEX = 0;
const u16_t CORE_DOT11_DURATION_INDEX = 2;
const u16_t CORE_DOT11_ADDRESS1_INDEX = 4;
const u16_t CORE_DOT11_ADDRESS2_INDEX = 10;
const u16_t CORE_DOT11_ADDRESS3_INDEX = 16;
const u16_t CORE_DOT11_NO_IE_OFFSET = 0;
const u16_t CORE_DOT11_FRAME_CONTROL_DEFAULT = 0x0000;

/** Defining this enables IE iterator related traces. */
//#define WLAN_CORE_DEEP_DEBUG 1

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_dot11_c* core_frame_dot11_c::instance(
    u16_t data_length,
    const u8_t* data,
    bool_t is_copied )
    {
    DEBUG( "core_frame_dot11_c::instance()" );
    DEBUG1( "core_frame_dot11_c::instance() - is_copied %u",
        is_copied );   
    
    if( data_length < CORE_DOT11_LENGTH )
        {
        DEBUG( "core_frame_dot11_c::instance() - not a valid 802.11 frame, frame is too short" );
        
        return NULL;
        }

    u8_t* buffer = const_cast<u8_t*>( data );
    u16_t buffer_length( 0 );

    if( is_copied )
        {
        buffer_length = data_length;
        buffer = new u8_t[buffer_length];
        
        if( !buffer )
            {
            DEBUG( "core_frame_dot11_c::instance() - not able to allocate buffer for copying" );
        
            return NULL;            
            }

        core_tools_c::copy(
            buffer,
            data,
            buffer_length );
        }

    core_frame_dot11_c* instance = new core_frame_dot11_c(
        data_length,
        buffer,
        buffer_length );
    if( !instance )
        {
        DEBUG( "core_frame_dot11_c::instance() - unable to create an instance" );
        if( is_copied )
            {
            delete[] buffer;
            }

        return NULL;
        }

    u16_t required_length( CORE_DOT11_LENGTH );
    if( instance->is_qos_m )
        {
        required_length += CORE_DOT11_QOS_CONTROL_LENGTH;

        DEBUG( "core_frame_dot11_c::instance() - this frame includes QoS Control field" );
        }

    if( instance->is_ht_m )
        {
        required_length += CORE_DOT11_HT_CONTROL_LENGTH;

        DEBUG( "core_frame_dot11_c::instance() - this frame includes HT Control field" );
        }

    if( data_length < required_length )
        {
        DEBUG( "core_frame_dot11_c::instance() - not a valid 802.11 frame, frame is too short" );
        delete instance;

        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//      
core_frame_dot11_c::~core_frame_dot11_c()
    {
    DEBUG( "core_frame_dot11_c::~core_frame_dot11_c" );

    /** 
     * If maximum frame length has been defined, we have allocated
     * the frame buffer ourselves.
     */
    if( max_data_length_m )
        {
        delete[] data_m;
        }
    data_m = NULL;
    current_ie_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_dot11_c::frame_control() const
    {
    return core_tools_c::get_u16( data_m, CORE_DOT11_FRAME_CONTROL_INDEX );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_dot11_c::core_dot11_type_e core_frame_dot11_c::type() const
    {
    return static_cast<core_frame_dot11_c::core_dot11_type_e>(
        frame_control() & core_dot11_frame_control_type_subtype_mask );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
u16_t core_frame_dot11_c::duration() const
    {
    return core_tools_c::get_u16( data_m, CORE_DOT11_DURATION_INDEX );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_mac_address_s core_frame_dot11_c::destination() const
    {
    u16_t index( 0 );

    if( frame_control() & core_dot11_frame_control_to_ds_mask )
        {
        if( frame_control() & core_dot11_frame_control_from_ds_mask )
            {
            index = CORE_DOT11_ADDRESS3_INDEX;
            }
        else
            {
            index = CORE_DOT11_ADDRESS3_INDEX;
            }
        }
    else
        {
        if( frame_control() & core_dot11_frame_control_from_ds_mask )
            {
            index = CORE_DOT11_ADDRESS1_INDEX;
            }
        else
            {
            index = CORE_DOT11_ADDRESS1_INDEX;
            }
        }
        
    return mac_address( index );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_mac_address_s core_frame_dot11_c::source() const
    {
    u16_t index( 0 );

    if( frame_control() & core_dot11_frame_control_to_ds_mask )
        {
        if( frame_control() & core_dot11_frame_control_from_ds_mask )
            {
            return ZERO_MAC_ADDR;
            }
        else
            {
            index = CORE_DOT11_ADDRESS2_INDEX;
            }
        }
    else
        {
        if( frame_control() & core_dot11_frame_control_from_ds_mask )
            {
            index = CORE_DOT11_ADDRESS3_INDEX;
            }
        else
            {
            index = CORE_DOT11_ADDRESS2_INDEX;
            }
        }
        
    return mac_address( index );      
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_dot11_c::set_destination( const core_mac_address_s& da )
    {
    core_mac_address_s destination = da;
    core_tools_c::copy(
        &data_m[CORE_DOT11_ADDRESS2_INDEX],
        &destination,
        MAC_ADDR_LEN );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_dot11_c::set_source( const core_mac_address_s& sa )
    {
    core_mac_address_s source = sa;
    core_tools_c::copy(
        &data_m[CORE_DOT11_ADDRESS1_INDEX],
        &source,
        MAC_ADDR_LEN );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_mac_address_s core_frame_dot11_c::bssid() const
    {
    u16_t index( 0 );

    if( frame_control() & core_dot11_frame_control_to_ds_mask )
        {
        if( frame_control() & core_dot11_frame_control_from_ds_mask )
            {
            return ZERO_MAC_ADDR;
            }
        else
            {
            index = CORE_DOT11_ADDRESS1_INDEX;
            }
        }
    else
        {
        if( frame_control() & core_dot11_frame_control_from_ds_mask )
            {
            index = CORE_DOT11_ADDRESS2_INDEX;
            }
        else
            {
            index = CORE_DOT11_ADDRESS3_INDEX;
            }
        }

    return mac_address( index );       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_mac_address_s core_frame_dot11_c::mac_address(
    u16_t index ) const
    {
    core_mac_address_s mac( ZERO_MAC_ADDR );
    
    core_tools_c::copy(
        &mac.addr[0],
        &data_m[index],
        MAC_ADDR_LEN );

    return mac;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_dot11_c::data_length() const
    {
    return data_length_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
const u8_t* core_frame_dot11_c::data() const
    {
    return data_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_dot11_c::payload_data_length() const
    {
    u16_t length( data_length_m - CORE_DOT11_LENGTH );
    if( is_qos_m )
        {
        length -= CORE_DOT11_QOS_CONTROL_LENGTH;
        }
    if( is_ht_m )
        {
        length -= CORE_DOT11_HT_CONTROL_LENGTH;
        }

    return length;       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_dot11_c::payload_data() const
    {
    u8_t* data( data_m + CORE_DOT11_LENGTH );
    if( is_qos_m )
        {
        data += CORE_DOT11_QOS_CONTROL_LENGTH;
        }
    if( is_ht_m )
        {
        data += CORE_DOT11_HT_CONTROL_LENGTH;
        }

    return data;       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
core_frame_dot11_ie_c* core_frame_dot11_c::first_ie()
    {
    u16_t offset = first_ie_offset();
    if( !offset )
        {
        DEBUG( "core_frame_dot11_c::instance() - the frame has no IEs" );
        return NULL;
        }

    current_ie_m = payload_data() + offset;
    current_ie_max_length_m = payload_data_length() - offset;

    return next_ie();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_dot11_ie_c* core_frame_dot11_c::next_ie()
    {
#ifdef WLAN_CORE_DEEP_DEBUG
    DEBUG1( "core_frame_dot11_c::next_ie() - real frame length: %u",
        data_length_m );  
    DEBUG1( "core_frame_dot11_c::next_ie() - data_m is at %08X",
        data_m );
    DEBUG1( "core_frame_dot11_c::next_ie() - current_ie_m is at %08X",
        current_ie_m );
    DEBUG1( "core_frame_dot11_c::next_ie() - data end is at %08X",
        data_m + data_length_m );
    DEBUG1( "core_frame_dot11_c::next_ie() - maximum IE length is %u",
        current_ie_max_length_m );
#endif // WLAN_CORE_DEEP_DEBUG

    if( !current_ie_max_length_m )
        {
        return NULL;
        }

    core_frame_dot11_ie_c* instance =
        core_frame_dot11_ie_c::instance( current_ie_max_length_m, current_ie_m );
    if( !instance )
        {
        DEBUG( "core_frame_dot11_c::instance() - unable to create IE parser" );
        
        return NULL;
        }

    current_ie_m += instance->data_length();

#ifdef WLAN_CORE_DEEP_DEBUG
    DEBUG1( "core_frame_dot11_c::next_ie() - IE ID is %u",
        instance->element_id() );
    DEBUG1( "core_frame_dot11_c::next_ie() - IE length is %u",
        instance->length() );
    DEBUG1( "core_frame_dot11_c::next_ie() - IE data length is %u",
        instance->data_length() );  
#endif // WLAN_CORE_DEEP_DEBUG

    ASSERT( current_ie_m <= data_m + data_length_m );
    current_ie_max_length_m -= instance->data_length();

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_dot11_c::append_ie(
    const core_frame_dot11_ie_c* ie )
    {
    if( !ie )
        {
        return;
        }

#ifdef WLAN_CORE_DEEP_DEBUG  
    DEBUG1( "core_frame_dot11_c::append_ie() - real frame length: %u",
        data_length_m );
    DEBUG1( "core_frame_dot11_c::append_ie() - maximum frame length: %u",
        max_data_length_m );
    DEBUG1( "core_frame_dot11_c::append_ie() - ie length: %u",
        ie->data_length() );
#endif // WLAN_CORE_DEEP_DEBUG

    if( data_length_m + ie->data_length() <= max_data_length_m )
        {
        core_tools_c::copy(
            &data_m[data_length_m],
            ie->data(),
            ie->data_length() );
        data_length_m += ie->data_length();
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_dot11_c::first_ie_offset() const
    {
    /**
     * By default the frame doesn't have any IEs.
     */   
    return CORE_DOT11_NO_IE_OFFSET;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
u16_t core_frame_dot11_c::first_fixed_element_offset() const
    {
    u16_t offset = CORE_DOT11_LENGTH;

    if( is_qos_m )
        {
        offset += CORE_DOT11_QOS_CONTROL_LENGTH;
        }
    if( is_ht_m )
        {
        offset += CORE_DOT11_HT_CONTROL_LENGTH;
        }

    return offset;       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_dot11_c::generate(
    u8_t type,
    u16_t duration,
    const core_mac_address_s& destination,
    const core_mac_address_s& source,
    const core_mac_address_s& bssid,
    u16_t sequence_control )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    // Frame Control
    u16_t temp16( CORE_DOT11_FRAME_CONTROL_DEFAULT );
    temp16 |= type;

    // Data frames sent by a station must have the ToDS bit set.
    if( ( type & core_dot11_frame_control_type_mask ) == core_dot11_frame_control_type_data )
        {
        temp16 |= core_dot11_frame_control_to_ds_mask;
        }

    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp16),
        sizeof( temp16 ) );
    data_length_m += sizeof( temp16 );    

    // Duration
    temp16 = duration;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp16),
        sizeof( temp16 ) );
    data_length_m += sizeof( temp16 );    
    
    // BSSID
    core_tools_c::copy(
        &data_m[data_length_m],
        &bssid.addr[0],
        MAC_ADDR_LEN );
    data_length_m += MAC_ADDR_LEN;    
    
    // Source MAC
    core_tools_c::copy(
        &data_m[data_length_m],
        &source.addr[0],
        MAC_ADDR_LEN );
    data_length_m += MAC_ADDR_LEN;    

    // Destination MAC
    core_tools_c::copy(
        &data_m[data_length_m],
        &destination.addr[0],
        MAC_ADDR_LEN );
    data_length_m += MAC_ADDR_LEN;        

    // Sequence Control
    temp16 = sequence_control;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp16),
        sizeof( temp16 ) );
    data_length_m += sizeof( temp16 );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_dot11_c::core_frame_dot11_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    data_length_m( data_length ),
    data_m( NULL ),
    max_data_length_m( max_data_length ),
    is_qos_m( false_t ),
    is_ht_m( false_t ),
    current_ie_m( NULL ),
    current_ie_max_length_m( 0 )
    {
    DEBUG( "core_frame_dot11_c::core_frame_dot11_c" );

    /**
     * The reason the const is discarded is that the same pointer
     * is used when we have allocated the frame buffer ourselves.
     */
    data_m = const_cast<u8_t*>( data );

    core_dot11_frame_control_type_e type_field =
        static_cast<core_frame_dot11_c::core_dot11_frame_control_type_e>(
            frame_control() & core_dot11_frame_control_type_mask );

    if( type_field == core_dot11_frame_control_type_data &&
        frame_control() & core_dot11_frame_control_subtype_qos_mask )
        {
        is_qos_m = true_t;
        }

    if( ( type_field == core_dot11_frame_control_type_management ||
          is_qos_m ) &&
        frame_control() & core_dot11_frame_control_order_mask )
        {
        is_ht_m = true_t;
        }
    }
