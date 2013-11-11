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
* Description:  Class for parsing and generating 802.11 action frames in RM category.
*
*/


#include "core_frame_radio_measurement_action.h"
#include "core_frame_rm_ie.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

const u16_t CORE_FRAME_RADIO_MEASUREMENT_ACTION_MIN_LENGTH = 28;
const u16_t CORE_FRAME_RADIO_MEASUREMENT_ACTION_TYPE_INDEX = 25;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_radio_measurement_action_c* core_frame_radio_measurement_action_c::instance(
    const core_frame_action_c& frame,
    bool_t is_copied )
    {
    DEBUG( "core_frame_radio_measurement_action_c::instance()" );
    DEBUG1( "core_frame_radio_measurement_action_c::instance() - is_copied %u",
        is_copied );  

    if ( frame.data_length() < CORE_FRAME_RADIO_MEASUREMENT_ACTION_MIN_LENGTH )
        {
        DEBUG( "core_frame_action_rm_c::instance() - not a valid 802.11 frame, frame is too short" );
        
        return NULL;
        }

    u8_t* buffer = const_cast<u8_t*>( frame.data() );
    u16_t buffer_length( 0 );

    if( is_copied )
        {
        buffer_length = frame.data_length();
        buffer = new u8_t[buffer_length];
        
        if( !buffer )
            {
            DEBUG( "core_frame_radio_measurement_action_c::instance() - not able to allocate buffer for copying" );
        
            return NULL;            
            }

        core_tools_c::copy( buffer, frame.data(), buffer_length );
        }

    core_frame_radio_measurement_action_c* instance = new core_frame_radio_measurement_action_c(
        buffer_length,
        buffer,
        buffer_length );

    if ( !instance )
        {
        DEBUG( "core_frame_radio_measurement_action_c::instance() - unable to create an instance" );
        
        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_radio_measurement_action_c::~core_frame_radio_measurement_action_c()
    {
    DEBUG1( "core_frame_radio_measurement_action_c::~core_frame_radio_measurement_action_c() @ 0x%08X", this );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_radio_measurement_action_c::action_type() const
    {
    return data_m[CORE_FRAME_RADIO_MEASUREMENT_ACTION_TYPE_INDEX];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_radio_measurement_action_c::core_frame_radio_measurement_action_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_action_c( data_length, data, max_data_length )
    {
    }
