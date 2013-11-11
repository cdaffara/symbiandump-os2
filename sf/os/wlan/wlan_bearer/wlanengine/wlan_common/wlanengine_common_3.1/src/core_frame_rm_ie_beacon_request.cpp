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
* Description:  Class for parsing RM Beacon Request IEs.
*
*/


#include "core_frame_rm_ie_beacon_request.h"
#include "core_frame_dot11_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_MIN_LENGTH = 18;
const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_REGULATORY_CLASS_OFFSET = 5;
const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_CHANNEL_NUMBER_OFFSET = 6;
const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_RANDOMIZATION_INTERVAL_OFFSET = 7;
const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_MEASUREMENT_DURATION_OFFSET = 9;
const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_MEASUREMENT_MODE_OFFSET = 11;
const u8_t CORE_FRAME_RM_IE_BEACON_REQUEST_BSSID_OFFSET = 12;

const u16_t CORE_FRAME_RM_IE_BEACON_REQUEST_IE_INDEX = 18;

//#define WLAN_CORE_FRAME_RM_IE_BEACON_REQUEST_IE_DEBUG

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_request_c* core_frame_rm_ie_beacon_request_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    u16_t length = ie.data_length();
    if ( length < CORE_FRAME_RM_IE_BEACON_REQUEST_MIN_LENGTH )
        {
        DEBUG( "core_frame_rm_ie_beacon_request_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_rm_ie_beacon_request_c* instance = new core_frame_rm_ie_beacon_request_c(
        length,
        ie.data(),
        0 );
    
    if ( !instance )
        {
        DEBUG( "core_frame_rm_ie_beacon_request_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    DEBUG1( "core_frame_rm_ie_beacon_request_c::instance() - Regulatory Class: 0x%02X",
            instance->regulatory_class() );
    DEBUG1( "core_frame_rm_ie_beacon_request_c::instance() - Channel Number: 0x%02X",
            instance->channel_number() );
    DEBUG1( "core_frame_rm_ie_beacon_request_c::instance() - Randomization Interval: 0x%04X (TU)",
            instance->randomization_interval() );
    DEBUG1( "core_frame_rm_ie_beacon_request_c::instance() - Measurement Duration: 0x%04X (TU)",
            instance->measurement_duration() );
    DEBUG1( "core_frame_rm_ie_beacon_request_c::instance() - Measurement Mode: 0x%02X",
            instance->measurement_mode() );
    core_mac_address_s a = instance->bssid();
    DEBUG6( "core_frame_rm_ie_beacon_request_c::instance() - BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
            a.addr[0],a.addr[1],a.addr[2],a.addr[3],a.addr[4],a.addr[5] );
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_rm_ie_beacon_request_c::~core_frame_rm_ie_beacon_request_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_rm_ie_beacon_request_c::regulatory_class() const
    {
    return data_m[CORE_FRAME_RM_IE_BEACON_REQUEST_REGULATORY_CLASS_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_rm_ie_beacon_request_c::channel_number() const
    {
    return data_m[CORE_FRAME_RM_IE_BEACON_REQUEST_CHANNEL_NUMBER_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_rm_ie_beacon_request_c::randomization_interval() const
    {
    u16_t random_interval( 0 );
    core_tools_c::copy( &random_interval, &data_m[CORE_FRAME_RM_IE_BEACON_REQUEST_RANDOMIZATION_INTERVAL_OFFSET], sizeof( random_interval ) );

    return random_interval;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_rm_ie_beacon_request_c::measurement_duration() const
    {
    u16_t measurement_duration( 0 );
    core_tools_c::copy( &measurement_duration, &data_m[CORE_FRAME_RM_IE_BEACON_REQUEST_MEASUREMENT_DURATION_OFFSET], sizeof( measurement_duration ) );

    return measurement_duration;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_rm_ie_beacon_request_c::measurement_mode() const
    {
    return data_m[CORE_FRAME_RM_IE_BEACON_REQUEST_MEASUREMENT_MODE_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_mac_address_s core_frame_rm_ie_beacon_request_c::bssid() const
    {
    core_mac_address_s mac_addr( ZERO_MAC_ADDR );
    core_tools_c::copy( &mac_addr, &data_m[CORE_FRAME_RM_IE_BEACON_REQUEST_BSSID_OFFSET], sizeof( core_mac_address_s ) );
    return mac_addr;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_rm_ie_beacon_request_c::first_ie_offset() const
    {
    return CORE_FRAME_RM_IE_BEACON_REQUEST_IE_INDEX;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_rm_ie_beacon_request_c::payload_data_length() const
    {
    u16_t length( data_length_m - CORE_FRAME_RM_IE_BEACON_REQUEST_MIN_LENGTH );

    return length;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_rm_ie_beacon_request_c::payload_data() const
    {
    u8_t* data( data_m + CORE_FRAME_RM_IE_BEACON_REQUEST_MIN_LENGTH );

    return data;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
core_frame_dot11_ie_c* core_frame_rm_ie_beacon_request_c::first_ie()
    {
    u16_t offset = first_ie_offset();
    if( !offset )
        {
        DEBUG( "core_frame_rm_ie_beacon_request_c::instance() - the frame has no IEs" );
        return NULL;
        }

    current_ie_m = payload_data();
    current_ie_max_length_m = payload_data_length();

    return next_ie();
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_dot11_ie_c* core_frame_rm_ie_beacon_request_c::next_ie()
    {

#ifdef WLAN_CORE_FRAME_RM_IE_BEACON_REQUEST_IE_DEBUG
    DEBUG1( "core_frame_rm_ie_beacon_request_c::next_ie() - real frame length: %u",
        data_length_m );  
    DEBUG1( "core_frame_rm_ie_beacon_request_c::next_ie() - data_m is at %08X",
        data_m );
    DEBUG1( "core_frame_rm_ie_beacon_request_c::next_ie() - current_ie_m is at %08X",
        current_ie_m );
    DEBUG1( "core_frame_rm_ie_beacon_request_c::next_ie() - data end is at %08X",
        data_m + data_length_m );
    DEBUG1( "core_frame_rm_ie_beacon_request_c::next_ie() - maximum IE length is %u",
        current_ie_max_length_m );
#endif // WLAN_CORE_FRAME_RM_IE_BEACON_REQUEST_IE_DEBUG

    if( !current_ie_max_length_m )
        {
        return NULL;
        }

    core_frame_dot11_ie_c* instance =
    core_frame_dot11_ie_c::instance( current_ie_max_length_m, current_ie_m );
    if( !instance )
        {
        DEBUG( "core_frame_rm_ie_beacon_request_c::next_ie() - unable to create IE parser" );
        
        return NULL;
        }

    current_ie_m += instance->data_length();

#ifdef WLAN_CORE_FRAME_RM_IE_BEACON_REQUEST_IE_DEBUG
    DEBUG1( "core_frame_rm_ie_beacon_request_c::next_ie() - IE ID is %u",
        instance->element_id() );
    DEBUG1( "core_frame_rm_ie_beacon_request_c::next_ie() - IE length is %u",
        instance->length() );
    DEBUG1( "core_frame_rm_ie_beacon_request_c::next_ie() - IE data length is %u",
        instance->data_length() );  
#endif // WLAN_CORE_FRAME_RM_IE_BEACON_REQUEST_IE_DEBUG

    ASSERT( current_ie_m <= data_m + data_length_m );
    current_ie_max_length_m -= instance->data_length();

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_request_c::core_frame_rm_ie_beacon_request_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_rm_ie_c( data_length, data, max_data_length ),
    current_ie_m( NULL ),
    current_ie_max_length_m( 0 )
    {
    }
