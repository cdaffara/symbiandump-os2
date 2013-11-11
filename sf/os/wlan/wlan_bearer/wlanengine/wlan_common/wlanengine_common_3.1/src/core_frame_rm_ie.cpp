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
* Description:  Class for parsing RM Measurement Request/Report IEs.
*
*/


#include "core_frame_rm_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

const u8_t CORE_FRAME_RM_IE_MIN_LENGTH = 5;
const u8_t CORE_FRAME_RM_IE_MEASUREMENT_TOKEN_OFFSET = 2;
const u8_t CORE_FRAME_RM_IE_MEASUREMENT_REQUEST_MODE_OFFSET = 3;
const u8_t CORE_FRAME_RM_IE_MEASUREMENT_TYPE_OFFSET = 4;


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_c* core_frame_rm_ie_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    u16_t length = ie.data_length();
    if ( length < CORE_FRAME_RM_IE_MIN_LENGTH )
        {
        DEBUG( "core_frame_rm_ie_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    core_frame_rm_ie_c* instance = new core_frame_rm_ie_c(
        length,
        ie.data(),
        0 );
    
    if ( !instance )
        {
        DEBUG( "core_frame_rm_ie_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    DEBUG1( "core_frame_rm_ie_c::instance() - Measurement Token: 0x%02X",
            instance->measurement_token() );
    DEBUG1( "core_frame_rm_ie_c::instance() - Measurement Request Mode: 0x%02X",
            instance->measurement_request_mode() );
    DEBUG1( "core_frame_rm_ie_c::instance() - Measurement Type: 0x%02X",
            instance->measurement_type() );
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_frame_rm_ie_c* core_frame_rm_ie_c::instance(
    u8_t measurement_token,
    u8_t measurement_request_mode,
    u8_t measurement_type )
    {
    const u8_t max_length = CORE_FRAME_RM_IE_MIN_LENGTH;
    
    u8_t* buffer = new u8_t[max_length];
    if ( !buffer )
        {
        DEBUG( "core_frame_rm_ie_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_rm_ie_c* instance =
        new core_frame_rm_ie_c( 0, buffer, max_length );
    
    if ( !instance )
        {
        DEBUG( "core_frame_rm_ie_c::instance() - unable to create an instance" );
        delete [] buffer;

        return NULL;
        }

    instance->generate(
            measurement_token,
            measurement_request_mode,
            measurement_type );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_rm_ie_c::generate(
    u8_t measurement_token,
    u8_t measurement_request_mode,
    u8_t measurement_type )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );

    core_frame_dot11_ie_c::generate( core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_measurement_resp );
    
    // Measurement Token
    data_m[data_length_m++] = measurement_token;
    DEBUG1( "core_frame_rm_ie_c::generate() - Measurement Token: 0x%02X", data_m[data_length_m - 1] );
    
    // Measurement Request Mode
    data_m[data_length_m++] = measurement_request_mode;
    DEBUG1( "core_frame_rm_ie_c::generate() - Measurement Request Mode: 0x%02X", data_m[data_length_m - 1] );
    
    // Measurement Type
    data_m[data_length_m++] = measurement_type;
    DEBUG1( "core_frame_rm_ie_c::generate() - Measurement Type: 0x%02X", data_m[data_length_m - 1] );

    set_length( data_length_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_rm_ie_c::~core_frame_rm_ie_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_rm_ie_c::measurement_token() const
    {
    return data_m[CORE_FRAME_RM_IE_MEASUREMENT_TOKEN_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_rm_ie_c::measurement_request_mode() const
    {
    return data_m[CORE_FRAME_RM_IE_MEASUREMENT_REQUEST_MODE_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_rm_ie_c::measurement_type() const
    {
    return data_m[CORE_FRAME_RM_IE_MEASUREMENT_TYPE_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_c::core_frame_rm_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }
