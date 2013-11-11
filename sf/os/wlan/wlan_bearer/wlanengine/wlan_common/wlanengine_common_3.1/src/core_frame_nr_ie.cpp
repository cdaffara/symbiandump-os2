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


#include "core_frame_nr_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

const u8_t CORE_FRAME_NR_IE_PARAMETER_REQ_MIN_LENGTH = 34;
const u8_t CORE_FRAME_NR_IE_PARAMETER_RESP_MIN_LENGTH = 15;
const u8_t CORE_FRAME_NR_IE_PARAMETER_BSSID_OFFSET = 2;
const u8_t CORE_FRAME_NR_IE_PARAMETER_BSSID_INFO_OFFSET = 8;
const u8_t CORE_FRAME_NR_IE_PARAMETER_REGULATORY_CLASS_OFFSET = 12;
const u8_t CORE_FRAME_NR_IE_PARAMETER_CHANNEL_NUMBER_OFFSET = 13;
const u8_t CORE_FRAME_NR_IE_PARAMETER_PHY_TYPE_OFFSET = 14;


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_nr_ie_c* core_frame_nr_ie_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() < CORE_FRAME_NR_IE_PARAMETER_RESP_MIN_LENGTH )
        {
        DEBUG( "core_frame_nr_ie_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_nr_ie_c* instance = new core_frame_nr_ie_c(
        ie.data_length(),
        ie.data(),
        0 );
    
    if ( !instance )
        {
        DEBUG( "core_frame_nr_ie_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_frame_nr_ie_c* core_frame_nr_ie_c::instance(
    const core_ssid_s& ssid )
    {
    const u8_t max_length = CORE_FRAME_NR_IE_PARAMETER_REQ_MIN_LENGTH;
    
    u8_t* buffer = new u8_t[max_length];
    if ( !buffer )
        {
        DEBUG( "core_frame_nr_ie_c::instance() - unable to create the internal buffer" );
        return NULL;
        }

    core_frame_nr_ie_c* instance =
        new core_frame_nr_ie_c( 0, buffer, max_length );
    
    if ( !instance )
        {
        DEBUG( "core_frame_nr_ie_c::instance() - unable to create an instance" );
        delete [] buffer;

        return NULL;
        }

    instance->generate( ssid );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_nr_ie_c::generate(
    const core_ssid_s& ssid )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );

    core_frame_dot11_ie_c::generate( core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_ssid );

    // SSID
    core_tools_c::fillz(
        &data_m[data_length_m],
        MAX_SSID_LEN );
    core_tools_c::copy(
        &data_m[data_length_m],
        &ssid.ssid[0],
        ssid.length );
    data_length_m += MAX_SSID_LEN;
    
    set_length( data_length_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_nr_ie_c::~core_frame_nr_ie_c()
    {
    DEBUG1( "core_frame_nr_ie_c::~core_frame_nr_ie_c() @ 0x%08X", this );
    DEBUG1( "core_frame_nr_ie_c::~core_frame_nr_ie_c() - data @ 0x%08X", data_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_mac_address_s core_frame_nr_ie_c::bssid() const
    {
    core_mac_address_s mac_addr( ZERO_MAC_ADDR );
    core_tools_c::copy( &mac_addr, &data_m[CORE_FRAME_NR_IE_PARAMETER_BSSID_OFFSET], sizeof( core_mac_address_s ) );
    return mac_addr;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_nr_ie_c::bssid_info() const
    {
    return core_tools_c::get_u32(
        data_m,
        CORE_FRAME_NR_IE_PARAMETER_BSSID_INFO_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_nr_ie_c::regulatory_class() const
    {
    return data_m[CORE_FRAME_NR_IE_PARAMETER_REGULATORY_CLASS_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_nr_ie_c::channel_number() const
    {
    return data_m[CORE_FRAME_NR_IE_PARAMETER_CHANNEL_NUMBER_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_nr_ie_c::phy_type() const
    {
    return data_m[CORE_FRAME_NR_IE_PARAMETER_PHY_TYPE_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_nr_ie_c::core_frame_nr_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }
