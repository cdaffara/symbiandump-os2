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
* Description:  Class parsing 802.11 IEs.
*
*/


#include "genscaninfo.h"
#include "core_frame_dot11_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

const u8_t CORE_FRAME_DOT11_IE_LENGTH = 2;
const u8_t CORE_FRAME_DOT11_IE_MIN_LENGTH_FIELD = 0;

const u8_t CORE_FRAME_DOT11_IE_ELEMENT_INDEX = 0;
const u8_t CORE_FRAME_DOT11_IE_LENGTH_INDEX = 1;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_dot11_ie_c* core_frame_dot11_ie_c::instance(
    u16_t data_length,
    const u8_t* data,
    bool_t is_copied )
    {
    if ( data_length < CORE_FRAME_DOT11_IE_LENGTH )
        {
        DEBUG2( "core_frame_dot11_ie_c::instance() - IE data length is too short, actual length %u, minimum length %u",
            data_length, CORE_FRAME_DOT11_IE_LENGTH );

        return NULL;
        }

    u8_t* buffer = const_cast<u8_t*>( data );
    u16_t buffer_length( 0 );

    if ( is_copied )
        {
        DEBUG( "core_frame_dot11_ie_c::instance() - copying IE data" );
        
        buffer_length = data_length;
        buffer = new u8_t[buffer_length];
        
        if ( !buffer )
            {
            DEBUG( "core_frame_dot11_ie_c::instance() - not able to allocate buffer for copying" );
        
            return NULL;            
            }

        core_tools_c::copy(
            buffer,
            data,
            buffer_length );            
        }

    core_frame_dot11_ie_c* instance = new core_frame_dot11_ie_c(
        data_length,
        buffer,
        buffer_length );
    if ( !instance )
        {
        DEBUG( "core_frame_dot11_ie_c::instance() - unable to create an instance" );
        
        if ( is_copied )
            {
            delete[] buffer;
            }

        return NULL;
        }
    
    if ( instance->length() + CORE_FRAME_DOT11_IE_HEADER_LENGTH > data_length )
        {
        DEBUG( "core_frame_dot11_ie_c::instance() - length field is too big" );
        DEBUG2( "core_frame_dot11_ie_c::instance() - IE size is %u, IE length field is %u",
            data_length, instance->length() );
        delete instance;

        return NULL;
        }
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_dot11_ie_c::~core_frame_dot11_ie_c()
    {
    /** 
     * If maximum IE length has been defined, we have allocated
     * the frame buffer ourselves.
     */    
    if ( max_data_length_m )
        {
        delete[] data_m;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_dot11_ie_c::element_id() const
    {
    return data_m[CORE_FRAME_DOT11_IE_ELEMENT_INDEX];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
u8_t core_frame_dot11_ie_c::length() const
    {
    return data_m[CORE_FRAME_DOT11_IE_LENGTH_INDEX];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_dot11_ie_c::data_length() const
    {
    return length() + CORE_FRAME_DOT11_IE_HEADER_LENGTH;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const u8_t* core_frame_dot11_ie_c::data() const
    {
    return data_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_dot11_ie_c::generate(
    u8_t element_id )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    data_length_m = 0;

    // Element ID
    data_m[data_length_m++] = element_id;

    // Length
    data_m[data_length_m++] = CORE_FRAME_DOT11_IE_MIN_LENGTH_FIELD;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_dot11_ie_c::set_length(
    u8_t length )
    {
    data_m[CORE_FRAME_DOT11_IE_LENGTH_INDEX] = length - CORE_FRAME_DOT11_IE_HEADER_LENGTH;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_dot11_ie_c::core_frame_dot11_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    data_length_m( data_length ),
    data_m( NULL ),
    max_data_length_m( max_data_length )
    {
    /**
     * The reason the const is discarded is that the same pointer
     * is used when we have allocated the frame buffer ourselves.
     */    
    data_m = const_cast<u8_t*>( data );
    }
