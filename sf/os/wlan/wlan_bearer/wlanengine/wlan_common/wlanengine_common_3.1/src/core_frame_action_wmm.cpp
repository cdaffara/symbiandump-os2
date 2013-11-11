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
* Description:  Class for parsing and generating 802.11 action frames in WMM category.
*
*/


#include "core_frame_action_wmm.h"
#include "core_frame_wmm_ie_tspec.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

const u16_t CORE_FRAME_ACTION_WMM_MIN_LENGTH = 28;
const u16_t CORE_FRAME_ACTION_WMM_LENGTH = 200;
const u16_t CORE_FRAME_ACTION_WMM_TYPE_INDEX = 25;
const u16_t CORE_FRAME_ACTION_WMM_TOKEN_INDEX = 26;
const u16_t CORE_FRAME_ACTION_WMM_STATUS_INDEX = 27;
const u16_t CORE_FRAME_ACTION_WMM_IE_INDEX = 4;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_action_wmm_c* core_frame_action_wmm_c::instance(
    const core_frame_action_c& frame )
    {
    DEBUG( "core_frame_action_wmm_c::instance()" );

    if ( frame.data_length() < CORE_FRAME_ACTION_WMM_MIN_LENGTH )
        {
        DEBUG( "core_frame_action_wmm_c::instance() - not a valid 802.11 frame, frame is too short" );
        
        return NULL;
        }

    core_frame_action_wmm_c* instance = new core_frame_action_wmm_c(
        frame.data_length(),
        frame.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_action_wmm_c::instance() - unable to create an instance" );
        
        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_action_wmm_c* core_frame_action_wmm_c::instance(
    u16_t duration,
    const core_mac_address_s& destination,
    const core_mac_address_s& source,
    const core_mac_address_s& bssid,
    u16_t sequence_control,
    u8_t action_type,
    u8_t dialog_token,
    u8_t status,
    const core_frame_wmm_ie_tspec_c* tspec )
    {
    const u8_t max_data_length = CORE_FRAME_ACTION_WMM_LENGTH;

    u8_t* buffer = new u8_t[max_data_length];
    if ( !buffer )
        {
        DEBUG( "core_frame_action_wmm_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_action_wmm_c* instance =
        new core_frame_action_wmm_c( 0, buffer, max_data_length );
    if ( !instance )
        {
        DEBUG( "core_frame_action_wmm_c::instance() - unable to create an instance" );
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
        status,
        tspec );

    return instance;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_action_wmm_c::~core_frame_action_wmm_c()
    {
    DEBUG( "core_frame_action_wmm_c::~core_frame_action_wmm_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_action_wmm_c::action_type() const
    {
    return data_m[CORE_FRAME_ACTION_WMM_TYPE_INDEX];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_action_wmm_c::dialog_token() const
    {
    return data_m[CORE_FRAME_ACTION_WMM_TOKEN_INDEX];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_action_wmm_c::status() const
    {
    return data_m[CORE_FRAME_ACTION_WMM_STATUS_INDEX];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_action_wmm_c::first_ie_offset() const
    {
    DEBUG( "core_frame_action_wmm_c::first_ie_offset()" );
    
    return CORE_FRAME_ACTION_WMM_IE_INDEX;
    }    


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_action_wmm_c::generate(
    u16_t duration,
    const core_mac_address_s& destination,
    const core_mac_address_s& source,
    const core_mac_address_s& bssid,
    u16_t sequence_control,
    u8_t action_type,
    u8_t dialog_token,
    u8_t status,
    const core_frame_wmm_ie_tspec_c* tspec )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    core_frame_action_c::generate(            
        duration,
        destination,
        source,
        bssid,
        sequence_control,
        core_frame_action_c::core_dot11_action_category_wmm_qos );

    // Action Type
    data_m[data_length_m++] = action_type;

    // Dialog Token
    data_m[data_length_m++] = dialog_token;

    // Status
    data_m[data_length_m++] = status;

    // TSPEC
    core_tools_c::copy(
        &data_m[data_length_m],
        tspec->data(),
        tspec->data_length() );
    data_length_m += tspec->data_length();    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_action_wmm_c::core_frame_action_wmm_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_action_c( data_length, data, max_data_length )
    {
    DEBUG( "core_frame_action_wmm_c::core_frame_action_wmm_c()" );
    }
