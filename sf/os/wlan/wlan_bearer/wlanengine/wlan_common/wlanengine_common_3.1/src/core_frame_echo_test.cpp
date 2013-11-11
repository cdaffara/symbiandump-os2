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
* Description:  Class for parsing and generating echo test frames.
*
*/


#include "core_frame_echo_test.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

const u16_t CORE_FRAME_ECHO_TEST_LENGTH = 16;
const u16_t CORE_FRAME_ECHO_TOKEN_INDEX = 14;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_echo_test_c* core_frame_echo_test_c::instance(
    const core_frame_ethernet_c& frame )
    {
    DEBUG( "core_frame_echo_test_c::instance()" );

    if ( frame.data_length() < CORE_FRAME_ECHO_TEST_LENGTH )
        {
        DEBUG( "core_frame_echo_test_c::instance() - not a valid echo test frame, frame is too short" );

        return NULL;
        }

    core_frame_echo_test_c* instance = new core_frame_echo_test_c(
        frame.data_length(),
        frame.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_echo_test_c::instance() - unable to create an instance" );
        
        return NULL;
        }

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_echo_test_c* core_frame_echo_test_c::instance(
    const core_mac_address_s& destination,
    const core_mac_address_s& source,
    u16_t token )
    {
    const u8_t max_data_length = CORE_FRAME_ECHO_TEST_LENGTH;

    u8_t* buffer = new u8_t[max_data_length];
    if ( !buffer )
        {
        DEBUG( "core_frame_echo_test_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_echo_test_c* instance =
        new core_frame_echo_test_c( 0, buffer, max_data_length );
    if ( !instance )
        {
        DEBUG( "core_frame_echo_test_c::instance() - unable to create an instance" );
        delete[] buffer;
        buffer = NULL;

        return NULL;
        }

    instance->generate(
        destination,
        source,
        token );

    return instance;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_echo_test_c::~core_frame_echo_test_c()
    {
    DEBUG( "core_frame_echo_test_c::~core_frame_echo_test_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_echo_test_c::token() const
    {
    return core_tools_c::get_u16_big_endian( data_m, CORE_FRAME_ECHO_TOKEN_INDEX );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_echo_test_c::generate(
    const core_mac_address_s& destination,
    const core_mac_address_s& source,
    u16_t token )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    // Ethernet header
    core_frame_ethernet_c::generate(    
        destination,
        source,    
        core_frame_ethernet_c::core_ethernet_type_test );

    // Token
    u16_t temp16 = core_tools_c::convert_host_to_big_endian(
        token  );
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>( &temp16 ),
        sizeof( temp16 ) );
    data_length_m += sizeof( temp16 );    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_echo_test_c::core_frame_echo_test_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_ethernet_c( data_length, data, max_data_length )
    {
    DEBUG( "core_frame_echo_test_c::core_frame_echo_test()" );
    }
