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
* Description:  Class for generating WMM TSPEC elements.
*
*/

/*
* %version: 7 %
*/

#include "core_frame_wmm_ie_tspec.h"
#include "core_tools.h"
#include "am_debug.h"

const u8_t CORE_FRAME_WMM_IE_TSPEC_LENGTH = 63;
const u8_t CORE_FRAME_WMM_IE_TSPEC_OUI_OFFSET = 2;
const u8_t CORE_FRAME_WMM_IE_TSPEC_OUI_LENGTH = 5;
const u8_t CORE_FRAME_WMM_IE_TSPEC_OUI[] = { 0x00, 0x50, 0xF2, 0x02, 0x02 };
const u8_t CORE_FRAME_WMM_IE_TSPEC_VERSION_OFFSET = 7;

const u8_t CORE_FRAME_WMM_IE_TSPEC_TS_INFO_1ST_BYTE_OFFSET = 8;
const u8_t CORE_FRAME_WMM_IE_TSPEC_TS_INFO_2ND_BYTE_OFFSET = 9;
const u8_t CORE_FRAME_WMM_IE_TSPEC_NOMINAL_MSDU_SIZE_OFFSET = 11;
const u8_t CORE_FRAME_WMM_IE_TSPEC_MAXIMUM_MSDU_SIZE_OFFSET = 13;
const u8_t CORE_FRAME_WMM_IE_TSPEC_MINIMUM_SERVICE_INTERVAL_OFFSET = 15;
const u8_t CORE_FRAME_WMM_IE_TSPEC_MAXIMUM_SERVICE_INTERVAL_OFFSET = 19;
const u8_t CORE_FRAME_WMM_IE_TSPEC_INACTIVITY_INTERVAL_OFFSET = 23;
const u8_t CORE_FRAME_WMM_IE_TSPEC_SUSPENSION_INTERVAL_OFFSET = 27;
const u8_t CORE_FRAME_WMM_IE_TSPEC_SERVICE_START_TIME = 31;
const u8_t CORE_FRAME_WMM_IE_TSPEC_MINIMUM_DATA_RATE_OFFSET = 35;
const u8_t CORE_FRAME_WMM_IE_TSPEC_MEAN_DATA_RATE_OFFSET = 39;
const u8_t CORE_FRAME_WMM_IE_TSPEC_PEAK_DATA_RATE_OFFSET = 43;
const u8_t CORE_FRAME_WMM_IE_TSPEC_MAXIMUM_BURST_SIZE_OFFSET = 47;
const u8_t CORE_FRAME_WMM_IE_TSPEC_DELAY_BOUND_OFFSET = 51;
const u8_t CORE_FRAME_WMM_IE_TSPEC_MINIMUM_PHY_RATE_OFFSET = 55;
const u8_t CORE_FRAME_WMM_IE_TSPEC_SURPLUS_BANDWIDTH_ALLOWANCE_OFFSET = 59;
const u8_t CORE_FRAME_WMM_IE_TSPEC_MEDIUM_TIME_OFFSET_OFFSET = 61;

const u8_t CORE_FRAME_WMM_IE_TSPEC_TID_MASK = 0x1E;
const u8_t CORE_FRAME_WMM_IE_TSPEC_TID_SHIFT = 1;
const u8_t CORE_FRAME_WMM_IE_TSPEC_PSB_MASK = 0x04;
const u8_t CORE_FRAME_WMM_IE_TSPEC_PERIODIC_MASK = 0x01;
const u8_t CORE_FRAME_WMM_IE_TSPEC_UP_MASK = 0x38;
const u8_t CORE_FRAME_WMM_IE_TSPEC_UP_SHIFT = 3;
const u8_t CORE_FRAME_WMM_IE_TSPEC_DIRECTION_MASK = 0x60; 
const u8_t CORE_FRAME_WMM_IE_TSPEC_DIRECTION_SHIFT = 5;

const u8_t CORE_FRAME_WMM_IE_TSPEC_VERSION = 1;
const u8_t CORE_FRAME_WMM_IE_TSPEC_TS_INFO_1ST_BYTE = 0x80;
const u8_t CORE_FRAME_WMM_IE_TSPEC_TS_INFO_2ND_BYTE = 0x00;
const u8_t CORE_FRAME_WMM_IE_TSPEC_TS_INFO_3RD_BYTE = 0x00;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_wmm_ie_tspec_c* core_frame_wmm_ie_tspec_c::instance(
    const core_frame_dot11_ie_c& ie )
    {
    if ( ie.data_length() != CORE_FRAME_WMM_IE_TSPEC_LENGTH )
        {
        DEBUG( "core_frame_wmm_ie_tspec_c::instance() - not a valid IE, invalid length" );

        return NULL;
        }

    if ( core_tools_c::compare(
        &ie.data()[CORE_FRAME_WMM_IE_TSPEC_OUI_OFFSET],
        CORE_FRAME_WMM_IE_TSPEC_OUI_LENGTH,
        &CORE_FRAME_WMM_IE_TSPEC_OUI[0],
        CORE_FRAME_WMM_IE_TSPEC_OUI_LENGTH ) )
        {
        DEBUG( "core_frame_wpa_ie_c::instance() - not a valid IE, WMM OUI missing" );

        return NULL;       
        }

    core_frame_wmm_ie_tspec_c* instance = new core_frame_wmm_ie_tspec_c(
        ie.data_length(),
        ie.data(),
        0 );
    if ( !instance )
        {
        DEBUG( "core_frame_wmm_ie_tspec_c::instance() - unable to create an instance" );

        return NULL;
        }
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_wmm_ie_tspec_c* core_frame_wmm_ie_tspec_c::instance(
    u8_t tid,
    u8_t user_priority,
    bool_t is_uapsd_enabled,
    bool_t is_periodic_traffic,
    core_traffic_stream_direction_e direction,    
	u16_t nominal_msdu_size,
	u16_t maximum_msdu_size,
	u32_t minimum_service_interval,
	u32_t maximum_service_interval,
	u32_t inactivity_interval,
    u32_t suspension_interval,
    u32_t service_start_time,
    u32_t minimum_data_rate,
    u32_t mean_data_rate,
    u32_t peak_data_rate,
    u32_t maximum_burst_size,
    u32_t delay_bound,
    u32_t minimum_phy_rate,
    u16_t surplus_bandwidth_allowance,
    u16_t medium_time )
    {
    const u8_t max_length = CORE_FRAME_WMM_IE_TSPEC_LENGTH;
    
    u8_t* buffer = new u8_t[max_length];
    if ( !buffer )
        {
        DEBUG( "core_frame_wmm_ie_tspec_c::instance() - unable create the internal buffer" );
        return NULL;
        }

    core_frame_wmm_ie_tspec_c* instance =
        new core_frame_wmm_ie_tspec_c( 0, buffer, max_length );
    if ( !instance )
        {
        DEBUG( "core_frame_wmm_ie_tspec_c::instance() - unable to create an instance" );
        delete[] buffer;

        return NULL;
        }

    instance->generate(
        tid,
        user_priority,
        is_uapsd_enabled,
        is_periodic_traffic,
        direction,
	    nominal_msdu_size,
	    maximum_msdu_size,
	    minimum_service_interval,
	    maximum_service_interval,
	    inactivity_interval,
        suspension_interval,
        service_start_time,
        minimum_data_rate,
        mean_data_rate,
        peak_data_rate,
        maximum_burst_size,
        delay_bound,
        minimum_phy_rate,
        surplus_bandwidth_allowance,
        medium_time );

    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
core_frame_wmm_ie_tspec_c::~core_frame_wmm_ie_tspec_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wmm_ie_tspec_c::version() const
    {
    return data_m[CORE_FRAME_WMM_IE_TSPEC_VERSION_OFFSET];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wmm_ie_tspec_c::tid() const
    {
    u8_t tid_shifted =
        data_m[CORE_FRAME_WMM_IE_TSPEC_TS_INFO_1ST_BYTE_OFFSET] & CORE_FRAME_WMM_IE_TSPEC_TID_MASK;

    return ( tid_shifted >> CORE_FRAME_WMM_IE_TSPEC_TID_SHIFT );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_frame_wmm_ie_tspec_c::user_priority() const
    {
    u8_t up_shifted =
        data_m[CORE_FRAME_WMM_IE_TSPEC_TS_INFO_2ND_BYTE_OFFSET] & CORE_FRAME_WMM_IE_TSPEC_UP_MASK;

    return ( up_shifted >> CORE_FRAME_WMM_IE_TSPEC_UP_SHIFT );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_frame_wmm_ie_tspec_c::is_uapsd_enabled() const
    {
    return data_m[CORE_FRAME_WMM_IE_TSPEC_TS_INFO_2ND_BYTE_OFFSET] & CORE_FRAME_WMM_IE_TSPEC_PSB_MASK;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_frame_wmm_ie_tspec_c::is_periodic_traffic() const
    {
    return data_m[CORE_FRAME_WMM_IE_TSPEC_TS_INFO_1ST_BYTE_OFFSET] & CORE_FRAME_WMM_IE_TSPEC_PERIODIC_MASK;        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_direction_e core_frame_wmm_ie_tspec_c::direction() const
    {
    u8_t direction_shifted =
        data_m[CORE_FRAME_WMM_IE_TSPEC_TS_INFO_1ST_BYTE_OFFSET] & CORE_FRAME_WMM_IE_TSPEC_DIRECTION_MASK;

    return static_cast<core_traffic_stream_direction_e>(
        direction_shifted >> CORE_FRAME_WMM_IE_TSPEC_DIRECTION_SHIFT );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_wmm_ie_tspec_c::nominal_msdu_size() const
    {
    return core_tools_c::get_u16(
        data_m, CORE_FRAME_WMM_IE_TSPEC_NOMINAL_MSDU_SIZE_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_wmm_ie_tspec_c::maximum_msdu_size() const
    {
    return core_tools_c::get_u16(
        data_m, CORE_FRAME_WMM_IE_TSPEC_MAXIMUM_MSDU_SIZE_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_wmm_ie_tspec_c::minimum_service_interval() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_MINIMUM_SERVICE_INTERVAL_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_wmm_ie_tspec_c::maximum_service_interval() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_MAXIMUM_SERVICE_INTERVAL_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_wmm_ie_tspec_c::inactivity_interval() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_INACTIVITY_INTERVAL_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_wmm_ie_tspec_c::suspension_interval() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_SUSPENSION_INTERVAL_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_wmm_ie_tspec_c::service_start_time() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_SERVICE_START_TIME );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_wmm_ie_tspec_c::minimum_data_rate() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_MINIMUM_DATA_RATE_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_wmm_ie_tspec_c::mean_data_rate() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_MEAN_DATA_RATE_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_wmm_ie_tspec_c::peak_data_rate() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_PEAK_DATA_RATE_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_wmm_ie_tspec_c::maximum_burst_size() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_MAXIMUM_BURST_SIZE_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_wmm_ie_tspec_c::delay_bound() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_DELAY_BOUND_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_frame_wmm_ie_tspec_c::minimum_phy_rate() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_MINIMUM_PHY_RATE_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_wmm_ie_tspec_c::surplus_bandwidth_allowance() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_SURPLUS_BANDWIDTH_ALLOWANCE_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_frame_wmm_ie_tspec_c::medium_time() const
    {
    return core_tools_c::get_u32(
        data_m, CORE_FRAME_WMM_IE_TSPEC_MEDIUM_TIME_OFFSET_OFFSET );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_frame_wmm_ie_tspec_c::generate(
    u8_t tid,
    u8_t user_priority,
    bool_t is_uapsd_enabled,
    bool_t is_periodic_traffic,
    core_traffic_stream_direction_e direction,
    u16_t nominal_msdu_size,
    u16_t maximum_msdu_size,
    u32_t minimum_service_interval,
    u32_t maximum_service_interval,
    u32_t inactivity_interval,
    u32_t suspension_interval,
    u32_t service_start_time,
    u32_t minimum_data_rate,
    u32_t mean_data_rate,
    u32_t peak_data_rate,
    u32_t maximum_burst_size,
    u32_t delay_bound,
    u32_t minimum_phy_rate,
    u16_t surplus_bandwidth_allowance,
    u16_t medium_time )
    {
    ASSERT( !data_length_m );
    ASSERT( max_data_length_m );   

    core_frame_dot11_ie_c::generate(
        core_frame_dot11_ie_element_id_wmm_tspec );

    // OUI
    core_tools_c::copy(
        &data_m[data_length_m],
        &CORE_FRAME_WMM_IE_TSPEC_OUI[0],
        CORE_FRAME_WMM_IE_TSPEC_OUI_LENGTH );
    data_length_m += CORE_FRAME_WMM_IE_TSPEC_OUI_LENGTH;  

    // Version
    data_m[data_length_m++] = CORE_FRAME_WMM_IE_TSPEC_VERSION;

    // TS Info
    u8_t temp8 = CORE_FRAME_WMM_IE_TSPEC_TS_INFO_1ST_BYTE;
    temp8 |= ( tid << CORE_FRAME_WMM_IE_TSPEC_TID_SHIFT );
    temp8 |= ( direction << CORE_FRAME_WMM_IE_TSPEC_DIRECTION_SHIFT);
    if ( is_periodic_traffic )
        {
        temp8 |= CORE_FRAME_WMM_IE_TSPEC_PERIODIC_MASK;
        }
    data_m[data_length_m++] = temp8;

    temp8 = CORE_FRAME_WMM_IE_TSPEC_TS_INFO_2ND_BYTE;
    temp8 |= ( user_priority << CORE_FRAME_WMM_IE_TSPEC_UP_SHIFT );
    if ( is_uapsd_enabled )
        {
        temp8 |= CORE_FRAME_WMM_IE_TSPEC_PSB_MASK;
        }
    data_m[data_length_m++] = temp8;
    data_m[data_length_m++] = CORE_FRAME_WMM_IE_TSPEC_TS_INFO_3RD_BYTE;
    
    // Nominal MSDU Size
    u16_t temp16 = nominal_msdu_size;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp16),
        sizeof( temp16 ) );
    data_length_m += sizeof( temp16 );    
    
    // Maximum MSDU Size
    temp16 = maximum_msdu_size;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp16),
        sizeof( temp16 ) );
    data_length_m += sizeof( temp16 );
    
    // Minimum Service Interval
    u32_t temp32 = minimum_service_interval;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp32),
        sizeof( temp32 ) );
    data_length_m += sizeof( temp32 );    
    
    // Maximum Service Interval
    temp32 = maximum_service_interval;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp32),
        sizeof( temp32 ) );
    data_length_m += sizeof( temp32 );
    
    // Inactivity Interval
    temp32 = inactivity_interval;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp32),
        sizeof( temp32 ) );
    data_length_m += sizeof( temp32 );
    
    // Suspension Interval
    temp32 = suspension_interval;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp32),
        sizeof( temp32 ) );
    data_length_m += sizeof( temp32 );
    
    // Service Start Time
    temp32 = service_start_time;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp32),
        sizeof( temp32 ) );
    data_length_m += sizeof( temp32 );
    
    // Minimum Data Rate
    temp32 = minimum_data_rate;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp32),
        sizeof( temp32 ) );
    data_length_m += sizeof( temp32 );
    
    // Mean Data Rate
    temp32 = mean_data_rate;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp32),
        sizeof( temp32 ) );
    data_length_m += sizeof( temp32 );
    
    // Peak Data Rate
    temp32 = peak_data_rate;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp32),
        sizeof( temp32 ) );
    data_length_m += sizeof( temp32 );
    
    // Maximum Burst Size
    temp32 = maximum_burst_size;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp32),
        sizeof( temp32 ) );
    data_length_m += sizeof( temp32 );

    // Delay Bound
    temp32 = delay_bound;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp32),
        sizeof( temp32 ) );
    data_length_m += sizeof( temp32 );
    
    // Minimum PHY Rate
    temp32 = minimum_phy_rate;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp32),
        sizeof( temp32 ) );
    data_length_m += sizeof( temp32 );
    
    // Surplus Bandwidth Allowance
    temp16 = surplus_bandwidth_allowance;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp16),
        sizeof( temp16 ) );
    data_length_m += sizeof( temp16 );    

    // Medium Time
    temp16 = medium_time;
    core_tools_c::copy(
        &data_m[data_length_m],
        reinterpret_cast<u8_t*>(&temp16),
        sizeof( temp16 ) );
    data_length_m += sizeof( temp16 );

    set_length( data_length_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_frame_wmm_ie_tspec_c::core_frame_wmm_ie_tspec_c(
    u16_t data_length,
    const u8_t* data,
    u16_t max_data_length ) :
    core_frame_dot11_ie_c( data_length, data, max_data_length )
    {
    }
