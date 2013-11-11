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
* Description:  Class for parsing and generating 802.11 action frames.
*
*/


#include "core_frame_action.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

const u16_t CORE_FRAME_ACTION_MIN_LENGTH = 25;
const u16_t CORE_FRAME_ACTION_CATEGORY_INDEX = 24;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_action_c* core_frame_action_c::instance(
    const core_frame_dot11_c& frame )
    {
    DEBUG( "core_frame_action_c::instance()" );

    if ( frame.data_length() < CORE_FRAME_ACTION_MIN_LENGTH )
        {
        DEBUG( "core_frame_action_c::instance() - not a valid 802.11 frame, frame is too short" );
        
        return NULL;
        }

    core_frame_action_c* instance = new core_frame_action_c(
        frame.data_length(),
        frame.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_action_c::instance() - unable to create an instance" );
        
        return NULL;
        }

    if ( instance->type() != core_frame_dot11_c::core_dot11_type_action )
        {
        DEBUG( "core_frame_action_c::instance() - not a valid action frame" );
        delete instance;

        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_action_c::~core_frame_action_c()
    {
    DEBUG( "core_frame_action_c::~core_frame_action_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_action_c::category() const
    {
    return data_m[CORE_FRAME_ACTION_CATEGORY_INDEX];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_action_c::set_category( u8_t cat )
    {
    data_m[CORE_FRAME_ACTION_CATEGORY_INDEX] = cat;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_action_c::generate(
    u16_t duration,
    const core_mac_address_s& destination,
    const core_mac_address_s& source,
    const core_mac_address_s& bssid,
    u16_t sequence_control,
    u8_t category )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    core_frame_dot11_c::generate(    
        core_frame_dot11_c::core_dot11_type_action,
        duration,
        destination,
        source,
        bssid,
        sequence_control );

    // Category
    data_m[data_length_m++] = category;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_action_c::core_frame_action_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_c( data_length, data, max_data_length )
    {
    DEBUG( "core_frame_action_c::core_frame_action_c()" );
    }
