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
* Description:  Class for parsing RM Beacon Report IEs
*
*/


#include "core_frame_rm_ie_beacon_report.h"
#include "core_frame_rm_ie_beacon_report_ie.h"
#include "core_frame_dot11_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "core_types.h"
#include "am_debug.h"

const u8_t CORE_FRAME_RM_IE_BEACON_REPORT_MIN_LENGTH = 5;

//#define WLAN_CORE_FRAME_RM_IE_BEACON_REPORT_IE_DEBUG

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_report_c* core_frame_rm_ie_beacon_report_c::instance(
    u8_t measurement_token,
    u8_t measurement_request_mode,
    core_frame_rm_ie_beacon_report_ie_c* ie )
    {
    DEBUG( "core_frame_rm_ie_beacon_report_c::instance()" );
    
    const u16_t required_buffer_length = ie->length() + CORE_FRAME_RM_IE_BEACON_REPORT_MIN_LENGTH;
    
    u8_t* buffer = new u8_t[ required_buffer_length ];
    if ( !buffer )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_c::instance() - unable create the internal buffer" );
        return NULL;
        }
    
    core_frame_rm_ie_beacon_report_c* instance =
        new core_frame_rm_ie_beacon_report_c( 0, buffer, required_buffer_length );
    if ( !instance )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_c::instance() - unable to create an instance" );
        delete[] buffer;
        
        return NULL;
        }

    instance->generate(
            measurement_token,
            measurement_request_mode,
            ie );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_report_c* core_frame_rm_ie_beacon_report_c::instance(
    u8_t measurement_token,
    u8_t measurement_request_mode,
    core_frame_rm_ie_beacon_report_ie_c* ie,
    core_frame_rm_ie_beacon_report_frame_body_ie_c* frame_body_ie )
    {
    DEBUG( "core_frame_rm_ie_beacon_report_c::instance()" );
    
    const u16_t required_buffer_length = ie->length() + frame_body_ie->data_length() + CORE_FRAME_RM_IE_BEACON_REPORT_MIN_LENGTH;
    
    u8_t* buffer = new u8_t[ required_buffer_length ];
    if ( !buffer )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_c::instance() - unable create the internal buffer" );
        return NULL;
        }
    
    core_frame_rm_ie_beacon_report_c* instance =
        new core_frame_rm_ie_beacon_report_c( 0, buffer, required_buffer_length );
    if ( !instance )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_c::instance() - unable to create an instance" );
        delete[] buffer;
        
        return NULL;
        }

    instance->generate(
            measurement_token,
            measurement_request_mode,
            ie,
            frame_body_ie );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_rm_ie_beacon_report_c::generate(
    u8_t measurement_token,
    u8_t measurement_request_mode,
    core_frame_rm_ie_beacon_report_ie_c* ie )
    {
    DEBUG( "core_frame_rm_ie_beacon_report_c::generate()" );
    
#ifdef WLAN_CORE_FRAME_RM_IE_BEACON_REPORT_IE_DEBUG
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - max frame length: %u",
        max_data_length_m );  
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - real frame length: %u",
        data_length_m );  
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - data_m is at 0x%08X",
        data_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - current_ie_m is at 0x%08X",
        current_ie_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - data end is at 0x%08X",
        data_m + data_length_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - maximum IE length is %u",
        current_ie_max_length_m );
#endif // WLAN_CORE_FRAME_RM_IE_BEACON_REPORT_IE_DEBUG
    
    core_frame_rm_ie_c::generate(
        measurement_token,
        measurement_request_mode,
        core_frame_rm_ie_c::core_frame_rm_ie_action_type_beacon_request );

    ASSERT( max_data_length_m );

    if ( data_length_m + ie->length() <= max_data_length_m )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_c::generate() - enough size for IE, appending IE" ); 
        core_tools_c::copy(
            &data_m[data_length_m],
            ie->data(),
            ie->length() );
        data_length_m += ie->length();
        set_length( data_length_m );
        }
    else
        {
        DEBUG2( "core_frame_rm_ie_beacon_report_c::generate() - unable to append, IE size %u vs. available size %u",
            ie->length(), max_data_length_m - data_length_m );
        }
    
#ifdef WLAN_CORE_FRAME_RM_IE_BEACON_REPORT_IE_DEBUG
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - max frame length: %u",
        max_data_length_m );  
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - real frame length: %u",
        max_data_length_m );  
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - data_m is at %08X",
        data_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - current_ie_m is at %08X",
        current_ie_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - data end is at %08X",
        data_m + data_length_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - maximum IE length is %u",
        current_ie_max_length_m );
#endif // WLAN_CORE_FRAME_RM_IE_BEACON_REPORT_IE_DEBUG
    
    // check generated data length doesn't exceed allocated buffer
    ASSERT( data_length_m <= max_data_length_m );

    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_rm_ie_beacon_report_c::generate(
    u8_t measurement_token,
    u8_t measurement_request_mode,
    core_frame_rm_ie_beacon_report_ie_c* ie,
    core_frame_rm_ie_beacon_report_frame_body_ie_c* frame_body_ie )
    {
    DEBUG( "core_frame_rm_ie_beacon_report_c::generate()" );
    
#ifdef WLAN_CORE_FRAME_RM_IE_BEACON_REPORT_IE_DEBUG
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - max frame length: %u",
        max_data_length_m );  
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - real frame length: %u",
        data_length_m );  
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - data_m is at 0x%08X",
        data_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - current_ie_m is at 0x%08X",
        current_ie_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - data end is at 0x%08X",
        data_m + data_length_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - maximum IE length is %u",
        current_ie_max_length_m );
#endif // WLAN_CORE_FRAME_RM_IE_BEACON_REPORT_IE_DEBUG
    
    core_frame_rm_ie_c::generate(
        measurement_token,
        measurement_request_mode,
        core_frame_rm_ie_c::core_frame_rm_ie_action_type_beacon_request );

    ASSERT( max_data_length_m );

    if ( data_length_m + ie->length() <= max_data_length_m )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_c::generate() - enough size for IE, appending IE" ); 
        core_tools_c::copy(
            &data_m[data_length_m],
            ie->data(),
            ie->length() );
        data_length_m += ie->length();
        set_length( data_length_m );
        }
    else
        {
        DEBUG2( "core_frame_rm_ie_beacon_report_c::generate() - unable to append, IE size %u vs. available size %u",
            ie->length(), max_data_length_m - data_length_m );
        }

    if ( data_length_m + frame_body_ie->data_length() <= max_data_length_m )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_c::generate() - enough size for REPORTED FRAME BODY IE, appending IE" ); 
        core_tools_c::copy(
            &data_m[data_length_m],
            frame_body_ie->data(),
            frame_body_ie->data_length() );
        data_length_m += frame_body_ie->data_length();
        set_length( data_length_m );
        }
    else
        {
        DEBUG2( "core_frame_rm_ie_beacon_report_c::generate() - unable to append, REPORTED FRAME BODY IE size %u vs. available size %u",
            frame_body_ie->data_length(), max_data_length_m - data_length_m );
        }
    
#ifdef WLAN_CORE_FRAME_RM_IE_BEACON_REPORT_IE_DEBUG
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - max frame length: %u",
        max_data_length_m );  
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - real frame length: %u",
        max_data_length_m );  
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - data_m is at %08X",
        data_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - current_ie_m is at %08X",
        current_ie_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - data end is at %08X",
        data_m + data_length_m );
    DEBUG1( "core_frame_rm_ie_beacon_report_c::generate() - maximum IE length is %u",
        current_ie_max_length_m );
#endif // WLAN_CORE_FRAME_RM_IE_BEACON_REPORT_IE_DEBUG
    
    // check generated data length doesn't exceed allocated buffer
    ASSERT( data_length_m <= max_data_length_m );

    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_rm_ie_beacon_report_c::~core_frame_rm_ie_beacon_report_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_report_c::core_frame_rm_ie_beacon_report_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_rm_ie_c( data_length, data, max_data_length ),
    current_ie_m( NULL ),
    current_ie_max_length_m( 0 )
    {
    }
