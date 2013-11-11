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


#include "core_frame_action_rm.h"
#include "core_frame_rm_ie.h"
#include "core_tools.h"
#include "am_debug.h"

const u16_t CORE_FRAME_ACTION_RM_MIN_LENGTH = 28;
const u16_t CORE_FRAME_ACTION_RM_LENGTH = 400;
const u16_t CORE_FRAME_ACTION_RM_TOKEN_INDEX = 26;
const u16_t CORE_FRAME_ACTION_RM_NBR_OF_REPETITIONS_INDEX = 27;
const u16_t CORE_FRAME_ACTION_RM_IE_INDEX = 5;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_action_rm_c* core_frame_action_rm_c::instance(
    const core_frame_action_c& frame,
    bool_t is_copied )
    {
    DEBUG( "core_frame_action_rm_c::instance()" );
    DEBUG1( "core_frame_action_rm_c::instance() - is_copied %u",
        is_copied );  

    if ( frame.data_length() < CORE_FRAME_ACTION_RM_MIN_LENGTH )
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
            DEBUG( "core_frame_action_rm_c::instance() - not able to allocate buffer for copying" );
        
            return NULL;            
            }

        core_tools_c::copy( buffer, frame.data(), buffer_length );
        }

    core_frame_action_rm_c* instance = new core_frame_action_rm_c(
        buffer_length,
        buffer,
        buffer_length );

    if ( !instance )
        {
        DEBUG( "core_frame_action_rm_c::instance() - unable to create an instance" );
        delete[] buffer;
        buffer = NULL;
        
        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_action_rm_c* core_frame_action_rm_c::instance(
    u16_t duration,
    const core_mac_address_s& destination,
    const core_mac_address_s& source,
    const core_mac_address_s& bssid,
    u16_t sequence_control,
    u8_t action_type,
    u8_t dialog_token,
    const core_frame_rm_ie_c* rm_ie )
    {
    const u16_t max_data_length = CORE_FRAME_ACTION_RM_LENGTH;

    u8_t* buffer = new u8_t[max_data_length];
    if ( !buffer )
        {
        DEBUG( "core_frame_action_rm_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_action_rm_c* instance =
        new core_frame_action_rm_c( 0, buffer, max_data_length );
    if ( !instance )
        {
        DEBUG( "core_frame_action_rm_c::instance() - unable to create an instance" );
        delete[] buffer;
        buffer = NULL;

        return NULL;
        }

    instance->generate(
        duration,
        destination,
        source,
        bssid,
        sequence_control,
        action_type,
        dialog_token,
        rm_ie );

    return instance;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_action_rm_c::~core_frame_action_rm_c()
    {
    DEBUG1( "core_frame_action_rm_c::~core_frame_action_rm_c() @ 0x%08X", this );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_action_rm_c::dialog_token() const
    {
    return data_m[CORE_FRAME_ACTION_RM_TOKEN_INDEX];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_action_rm_c::first_ie_offset() const
    {
    return CORE_FRAME_ACTION_RM_IE_INDEX;
    }    

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_action_rm_c::nbr_of_repetitions() const
    {
    u16_t repetitions( 0 );
    core_tools_c::copy( &repetitions, &data_m[CORE_FRAME_ACTION_RM_NBR_OF_REPETITIONS_INDEX], sizeof( repetitions ) );

    return repetitions;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_action_rm_c::generate(
    u16_t duration,
    const core_mac_address_s& destination,
    const core_mac_address_s& source,
    const core_mac_address_s& bssid,
    u16_t sequence_control,
    u8_t action_type,
    u8_t dialog_token,
    const core_frame_rm_ie_c* rm_ie )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );
    ASSERT( rm_ie );

    core_frame_action_c::generate(            
        duration,
        destination,
        source,
        bssid,
        sequence_control,
        core_frame_action_c::core_dot11_action_category_rm );
    
    // Action Type
    data_m[data_length_m++] = action_type;

    // Dialog Token
    data_m[data_length_m++] = dialog_token;

    // IE
    core_tools_c::copy(
        &data_m[data_length_m],
        rm_ie->data(),
        rm_ie->data_length() );
    data_length_m += rm_ie->data_length();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_action_rm_c::core_frame_action_rm_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_action_c( data_length, data, max_data_length )
    {
    DEBUG1( "core_frame_action_rm_c::core_frame_action_rm_c() @ 0x%08X", this );
    }
