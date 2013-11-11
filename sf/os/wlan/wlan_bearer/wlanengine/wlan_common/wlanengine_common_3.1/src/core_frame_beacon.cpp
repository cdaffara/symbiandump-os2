/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Parser utility for beacon and probe response frames
*
*/


#include "core_frame_beacon.h"
#include "core_frame_dot11_ie.h"
#include "core_tools.h"
#include "am_debug.h"

const u16_t CORE_BEACON_LENGTH = 36;
const u16_t CORE_BEACON_TIMESTAMP_INDEX = 0;
const u16_t CORE_BEACON_BEACON_INTERVAL_INDEX = 8;
const u16_t CORE_BEACON_CAPABILITY_INDEX = 10;
const u16_t CORE_BEACON_IE_INDEX = 12;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_beacon_c* core_frame_beacon_c::instance(
    const core_frame_dot11_c& frame,
    bool_t is_copied )
    {
    DEBUG( "core_frame_beacon_c::instance()" );

    if ( frame.data_length() < CORE_BEACON_LENGTH )
        {
        DEBUG( "core_frame_beacon_c::instance() - not a valid 802.11 frame, frame is too short" );
        
        return NULL;
        }

    u8_t* buffer = const_cast<u8_t*>( frame.data() );
    u16_t buffer_length( 0 );

    if ( is_copied )
        {
        buffer_length = frame.data_length();
        buffer = new u8_t[buffer_length];
        
        if ( !buffer )
            {
            DEBUG( "core_frame_beacon_c::instance() - not able to allocate buffer for copying" );
        
            return NULL;            
            }

        core_tools_c::copy(
            buffer,
            frame.data(),
            buffer_length );            
        }

    core_frame_beacon_c* instance = new core_frame_beacon_c(
        frame.data_length(),
        buffer,
        buffer_length );
    if ( !instance )
        {
        DEBUG( "core_frame_beacon_c::instance() - unable to create an instance" );
        
        if ( is_copied )
            {
            delete[] buffer;
            }
        
        return NULL;
        }

    if ( instance->type() != core_frame_dot11_c::core_dot11_type_beacon &&
         instance->type() != core_frame_dot11_c::core_dot11_type_probe_resp )
        {
        DEBUG( "core_frame_beacon_c::instance() - not a valid beacon or probe response frame" );
        delete instance;
        
        return NULL;
        }
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_beacon_c::~core_frame_beacon_c()
    {
    DEBUG( "core_frame_beacon_c::~core_frame_beacon_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_beacon_c::first_ie_offset() const
    {
    return CORE_BEACON_IE_INDEX;
    }    

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_tsf_value_s core_frame_beacon_c::timestamp() const
    {
    core_tsf_value_s timestamp;
    
    core_tools_c::copy(
        &timestamp.tsf[0],
        payload_data() + CORE_BEACON_TIMESTAMP_INDEX,
        TSF_VALUE_LEN );
        
    return timestamp;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_beacon_c::beacon_interval() const
    {
    return core_tools_c::get_u16( payload_data(),
        CORE_BEACON_BEACON_INTERVAL_INDEX );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_beacon_c::capability() const
    {
    return core_tools_c::get_u16( payload_data(),
        CORE_BEACON_CAPABILITY_INDEX );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_beacon_c::core_frame_beacon_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_c( data_length, data, max_data_length )
    {
    DEBUG( "core_frame_beacon_c::core_frame_beacon_c()" );
    }
