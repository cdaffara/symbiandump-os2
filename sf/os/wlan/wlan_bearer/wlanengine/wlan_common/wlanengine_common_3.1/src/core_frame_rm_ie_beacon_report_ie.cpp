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
* Description:  Class parsing 802.11k Beacon Report IEs.
*
*/


#include "genscaninfo.h"
#include "core_frame_rm_ie_beacon_report_ie.h"
#include "core_frame_rm_ie_beacon_report_frame_body_ie.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

const u8_t CORE_FRAME_RM_IE_BEACON_REPORT_IE_MIN_LENGTH = 26;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
u8_t core_frame_rm_ie_beacon_report_ie_c::length() const
    {
    return CORE_FRAME_RM_IE_BEACON_REPORT_IE_MIN_LENGTH;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_report_ie_c* core_frame_rm_ie_beacon_report_ie_c::instance(
        u8_t regulatory_class,
        u16_t measurement_duration,
        u8_t reported_frame_information,
        u8_t antenna_id,
        core_ap_data_c& ap_data )
    {
    DEBUG( "core_frame_rm_ie_beacon_report_ie_c::instance()" );

    core_tsf_value_s dummy_tsf = {0};
    u8_t dummy_rsni = 0xFF;
    
    u8_t* buffer = new u8_t[CORE_FRAME_RM_IE_BEACON_REPORT_IE_MIN_LENGTH];
    if ( !buffer )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_ie_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_rm_ie_beacon_report_ie_c* instance =
        new core_frame_rm_ie_beacon_report_ie_c( 0, buffer, CORE_FRAME_RM_IE_BEACON_REPORT_IE_MIN_LENGTH );
    if ( !instance )
        {
        DEBUG( "core_frame_rm_ie_beacon_report_ie_c::instance() - unable to create an instance" );
        delete[] buffer;
        
        return NULL;
        }

    instance->generate(
            regulatory_class,
            dummy_tsf,
            measurement_duration,
            reported_frame_information,
            dummy_rsni,
            antenna_id,
            dummy_tsf,
            ap_data );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_rm_ie_beacon_report_ie_c::generate(
    u8_t regulatory_class,
    core_tsf_value_s& actual_measurement_start_time,
    u16_t measurement_duration,
    u8_t reported_frame_information,
    u8_t rsni,
    u8_t antenna_id,
    core_tsf_value_s& parent_tsf,
    core_ap_data_c& ap_data )
    {
    DEBUG( "core_frame_rm_ie_beacon_report_ie_c::generate()" );

    // Regulatory Class
    data_m[data_length_m++] = regulatory_class;
    DEBUG1( "core_frame_rm_ie_beacon_report_ie_c::generate() - Regulatory Class: 0x%02X", data_m[data_length_m - 1] );

    // Channel Number
    data_m[data_length_m++] = ap_data.channel();
    DEBUG1( "core_frame_rm_ie_beacon_report_ie_c::generate() - Channel Number: 0x%02X", data_m[data_length_m - 1] );

    // Actual Measurement Start Time
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<const u8_t*>( &actual_measurement_start_time.tsf[0] ),
        TSF_VALUE_LEN );
    data_length_m += TSF_VALUE_LEN;
    DEBUG1( "core_frame_rm_ie_beacon_report_ie_c::generate() - Actual Measurement Start Time: 0x%016X", data_m[data_length_m - 1] );

    // Measurement Duration
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<const u8_t*>( &measurement_duration ),
        sizeof( measurement_duration ) );
    data_length_m += sizeof( measurement_duration );
    DEBUG1( "core_frame_rm_ie_beacon_report_ie_c::generate() - Measurement Duration: 0x%04X", data_m[data_length_m - 1] );

    // Reported Frame Information
    data_m[data_length_m++] = reported_frame_information;
    DEBUG1( "core_frame_rm_ie_beacon_report_ie_c::generate() - Reported Frame Information: 0x%02X", data_m[data_length_m - 1] );

    // RCPI
    data_m[data_length_m++] = ap_data.rcpi();
    DEBUG1( "core_frame_rm_ie_beacon_report_ie_c::generate() - RCPI: 0x%02X", data_m[data_length_m - 1] );

    // RSNI
    data_m[data_length_m++] = rsni;
    DEBUG1( "core_frame_rm_ie_beacon_report_ie_c::generate() - RSNI: 0x%02X", data_m[data_length_m - 1] );

    // BSSID
    const core_mac_address_s bssid = ap_data.bssid();
    DEBUG6( "core_frame_rm_ie_beacon_report_ie_c::generate() - BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
        bssid.addr[0], bssid.addr[1], bssid.addr[2], 
        bssid.addr[3], bssid.addr[4], bssid.addr[5] );    
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<const u8_t*>( &bssid.addr[0] ),
        MAC_ADDR_LEN ); 
    data_length_m += MAC_ADDR_LEN;

    // Antenna ID
    data_m[data_length_m++] = antenna_id;
    DEBUG1( "core_frame_rm_ie_beacon_report_ie_c::generate() - Antenna ID: 0x%02X", data_m[data_length_m - 1] );

    // Parent TSF
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<const u8_t*>( &parent_tsf.tsf[0] ),
        TSF_VALUE_LEN / 2 );

    data_length_m += TSF_VALUE_LEN / 2;
    DEBUG1( "core_frame_rm_ie_beacon_report_ie_c::generate() - Parent TSF: 0x%08X", data_m[data_length_m - 1] );
    
    // check generated data length doesn't exceed allocated buffer
    ASSERT( data_length_m <= max_data_length_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_rm_ie_beacon_report_ie_c::~core_frame_rm_ie_beacon_report_ie_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_rm_ie_beacon_report_ie_c::core_frame_rm_ie_beacon_report_ie_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length ),
    current_ie_m( NULL ),
    current_ie_max_length_m( 0 )
    {
    }
