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


#include "core_frame_ethernet.h"
#include "core_tools.h"
#include "am_debug.h"

const u16_t CORE_ETHERNET_MIN_LENGTH = 14;
const u16_t CORE_ETHERNET_DESTINATION_INDEX = 0;
const u16_t CORE_ETHERNET_SOURCE_INDEX = 6;
const u16_t CORE_ETHERNET_TYPE_INDEX = 12;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_ethernet_c* core_frame_ethernet_c::instance(
    u16_t data_length,
    const u8_t* data,
    bool_t is_copied )
    {
    DEBUG( "core_frame_ethernet_c::instance()" );
    DEBUG1( "core_frame_ethernet_c::instance() - is_copied %u",
        is_copied );   
    
    if ( data_length < CORE_ETHERNET_MIN_LENGTH )
        {
        DEBUG( "core_frame_ethernet_c::instance() - not a valid Ethernet frame, frame is too short" );
        
        return NULL;
        }

    u8_t* buffer = const_cast<u8_t*>( data );
    u16_t buffer_length( 0 );

    if ( is_copied )
        {
        buffer_length = data_length;
        buffer = new u8_t[buffer_length];
        
        if ( !buffer )
            {
            DEBUG( "core_frame_ethernet_c::instance() - not able to allocate buffer for copying" );
        
            return NULL;            
            }

        core_tools_c::copy(
            buffer,
            data,
            buffer_length );
        }

    core_frame_ethernet_c* instance = new core_frame_ethernet_c(
        data_length,
        buffer,
        buffer_length );
    if ( !instance )
        {
        DEBUG( "core_frame_ethernet_c::instance() - unable to create an instance" );
        if ( is_copied )
            {
            delete[] buffer;
            }

        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
core_frame_ethernet_c* core_frame_ethernet_c::instance(
    u16_t max_data_length,
    const core_mac_address_s& destination,
    const core_mac_address_s& source,
    u16_t type )
    {
    DEBUG( "core_frame_ethernet_c::instance()" );

    u8_t* buffer = new u8_t[max_data_length];
    if ( !buffer )
        {
        DEBUG( "core_frame_ethernet_c::instance() - unable create the internal buffer" );
        }

    core_frame_ethernet_c* instance =
        new core_frame_ethernet_c( 0, buffer, max_data_length );
    if ( !instance )
        {
        DEBUG( "core_frame_ethernet_c::instance() - unable to create an instance" );
        delete[] buffer;

        return NULL;
        }

    instance->generate(
        destination,
        source,
        type );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//      
core_frame_ethernet_c::~core_frame_ethernet_c()
    {
    DEBUG( "core_frame_ethernet_c::~core_frame_ethernet_c" );

    /** 
     * If maximum frame length has been defined, we have allocated
     * the frame buffer ourselves.
     */    
    if ( max_data_length_m )
        {
        delete[] data_m;
        }    

    data_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_mac_address_s core_frame_ethernet_c::destination() const
    {
    return mac_address( CORE_ETHERNET_DESTINATION_INDEX );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_mac_address_s core_frame_ethernet_c::source() const
    {
    return mac_address( CORE_ETHERNET_SOURCE_INDEX );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_ethernet_c::type() const
    {
    return core_tools_c::get_u16_big_endian( data_m, CORE_ETHERNET_TYPE_INDEX );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_mac_address_s core_frame_ethernet_c::mac_address(
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
u16_t core_frame_ethernet_c::data_length() const
    {
    return data_length_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
const u8_t* core_frame_ethernet_c::data() const
    {
    return data_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_ethernet_c::payload_data_length() const
    {
    return data_length_m - CORE_ETHERNET_MIN_LENGTH;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_ethernet_c::payload_data() const
    {
    return data_m + CORE_ETHERNET_MIN_LENGTH;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_ethernet_c::core_frame_ethernet_c::generate(
    const core_mac_address_s& destination,
    const core_mac_address_s& source,
    u16_t type )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    // Destination MAC
    core_tools_c::copy(
        &data_m[data_length_m],
        &destination.addr[0],
        MAC_ADDR_LEN );
    data_length_m += MAC_ADDR_LEN;        

    // Source MAC
    core_tools_c::copy(
        &data_m[data_length_m],
        &source.addr[0],
        MAC_ADDR_LEN );
    data_length_m += MAC_ADDR_LEN;    

    // Ethernet Type
    u16_t temp16 = core_tools_c::convert_host_to_big_endian(
        type  );
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>( &temp16 ),
        sizeof( temp16 ) );
    data_length_m += sizeof( temp16 );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_ethernet_c::core_frame_ethernet_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    data_length_m( data_length ),
    data_m( NULL ),
    max_data_length_m( max_data_length )
    {
    DEBUG( "core_frame_ethernet_c::core_frame_ethernet_c" );

    /**
     * The reason the const is discarded is that the same pointer
     * is used when we have allocated the frame buffer ourselves.
     */
    data_m = const_cast<u8_t*>( data );
    }
