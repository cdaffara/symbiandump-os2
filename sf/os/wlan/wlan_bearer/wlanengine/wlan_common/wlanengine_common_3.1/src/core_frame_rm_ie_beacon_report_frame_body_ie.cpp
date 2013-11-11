/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class parsing 802.11k Beacon Report Frame Body IEs.
*
*/


#include "genscaninfo.h"
#include "core_frame_rm_ie_beacon_report_frame_body_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

const u8_t CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_MAX_LENGTH = 224;
const u8_t CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_HEADER_LENGTH = 2;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_report_frame_body_ie_c* core_frame_rm_ie_beacon_report_frame_body_ie_c::instance(
    core_ap_data_c& ap_data )
    {
    DEBUG( "core_frame_rm_ie_beacon_report_frame_body_ie_c::instance()" );

    u8_t reported_frame_body[CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_MAX_LENGTH] = { 0 };
    u16_t reported_frame_body_length = ( 0 );
    ap_data.get_reported_frame_body( &reported_frame_body_length, reported_frame_body );

    u8_t* buffer = new u8_t[reported_frame_body_length + CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_HEADER_LENGTH];
    if ( !buffer )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_frame_body_ie_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_rm_ie_beacon_report_frame_body_ie_c* instance =
        new core_frame_rm_ie_beacon_report_frame_body_ie_c( 0, buffer, reported_frame_body_length + CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_HEADER_LENGTH );
    if ( !instance )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_frame_body_ie_c::instance() - unable to create an instance" );
        delete[] buffer;
        
        return NULL;
        }

    instance->generate(
            ap_data );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_report_frame_body_ie_c* core_frame_rm_ie_beacon_report_frame_body_ie_c::instance(
    core_ap_data_c& ap_data,
    u8_t* ie_id_list,
    u8_t ie_id_list_length )
    {
    DEBUG( "core_frame_rm_ie_beacon_report_frame_body_ie_c::instance()" );

    u8_t reported_frame_body[CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_MAX_LENGTH] = { 0 };
    u16_t reported_frame_body_length = ( 0 );

    ap_data.request_ie( &reported_frame_body_length, reported_frame_body, ie_id_list, ie_id_list_length );

    u8_t* buffer = new u8_t[reported_frame_body_length + CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_HEADER_LENGTH];
    if ( !buffer )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_frame_body_ie_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_rm_ie_beacon_report_frame_body_ie_c* instance =
        new core_frame_rm_ie_beacon_report_frame_body_ie_c( 0, buffer, reported_frame_body_length + CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_HEADER_LENGTH );
    if ( !instance )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_frame_body_ie_c::instance() - unable to create an instance" );
        delete[] buffer;
        
        return NULL;
        }

    instance->generate(
            ap_data,
            ie_id_list,
            ie_id_list_length );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_rm_ie_beacon_report_frame_body_ie_c::generate(
    core_ap_data_c& ap_data )
    {
    DEBUG( "core_frame_rm_ie_beacon_report_frame_body_ie_c::generate()" );

    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );

    core_frame_dot11_ie_c::generate( core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_supported_rates );

    // Reported Frame Body
    u8_t reported_frame_body[CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_MAX_LENGTH] = { 0 };
    u16_t reported_frame_body_length = ( 0 );
    ap_data.get_reported_frame_body( &reported_frame_body_length, reported_frame_body );
    DEBUG1( "core_frame_rm_ie_beacon_report_frame_body_ie_c::generate() - Reported Frame Body Length: 0x%04X", reported_frame_body_length );
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<const u8_t*>( &reported_frame_body[0] ),
        reported_frame_body_length );
    data_length_m += reported_frame_body_length;

    set_length( data_length_m );

    // check generated data length doesn't exceed allocated buffer
    ASSERT( data_length_m <= max_data_length_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_rm_ie_beacon_report_frame_body_ie_c::generate(
    core_ap_data_c& ap_data,
    u8_t* ie_id_list,
    u8_t ie_id_list_length )
    {
    DEBUG( "core_frame_rm_ie_beacon_report_frame_body_ie_c::generate()" );

    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );

    core_frame_dot11_ie_c::generate( core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_supported_rates );

    // Reported Frame Body
    u8_t reported_frame_body[CORE_FRAME_RM_IE_BEACON_REPORT_FRAME_BODY_IE_MAX_LENGTH] = { 0 };
    u16_t reported_frame_body_length = ( 0 );
    ap_data.request_ie( &reported_frame_body_length, reported_frame_body, ie_id_list, ie_id_list_length );
    DEBUG1( "core_frame_rm_ie_beacon_report_frame_body_ie_c::generate() - Reported Frame Body Length: 0x%04X", reported_frame_body_length );
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<const u8_t*>( &reported_frame_body[0] ),
        reported_frame_body_length );
    data_length_m += reported_frame_body_length;

    set_length( data_length_m );

    // check generated data length doesn't exceed allocated buffer
    ASSERT( data_length_m <= max_data_length_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_rm_ie_beacon_report_frame_body_ie_c::~core_frame_rm_ie_beacon_report_frame_body_ie_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_report_frame_body_ie_c::core_frame_rm_ie_beacon_report_frame_body_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length ),
    current_ie_m( NULL ),
    current_ie_max_length_m( 0 )
    {
    }
