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
* Description:  Class for storing traffic stream parameters.
*
*/

/*
* %version: 7 %
*/

#include "core_traffic_stream.h"
#include "core_tools.h"
#include "am_debug.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_c::core_traffic_stream_c(
    u8_t tid,
    u8_t user_priority ) :
    tid_m( tid ),
    user_priority_m( user_priority ),
    status_m( core_traffic_stream_status_undefined ),
    reference_count_m( 1 )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_c::~core_traffic_stream_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_traffic_stream_c::tid() const
    {
    return tid_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
u8_t core_traffic_stream_c::user_priority() const
    {
    return user_priority_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_access_class_e core_traffic_stream_c::access_class() const
    {
    return core_tools_c::convert_user_priority_to_ac( user_priority_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_traffic_stream_c::set_default_traffic_values(
    const core_traffic_stream_params_s& params )
    {
    default_params_m = params;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_traffic_stream_c::set_traffic_values(
    const core_traffic_stream_params_s& params )
    {
    previous_params_m = params_m;
    params_m = params;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_traffic_stream_c::reset_to_default_values()
    {
    previous_params_m = params_m;
    params_m = default_params_m;  
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_traffic_stream_c::reset_to_previous_values()
    {
    params_m = previous_params_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_status_e core_traffic_stream_c::status() const
    {
    return status_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_traffic_stream_c::set_status(
    core_traffic_stream_status_e status )
    {
    status_m = status;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_traffic_stream_c::reference_count()
    {
    return reference_count_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_traffic_stream_c::inc_reference_count()
    {
    ++reference_count_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_traffic_stream_c::dec_reference_count()
    {
    ASSERT( reference_count_m );
    --reference_count_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_traffic_stream_c::is_periodic_traffic() const
    {
    return params_m.is_periodic_traffic;  
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_direction_e core_traffic_stream_c::direction() const
    {
    return params_m.direction;  
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_traffic_stream_c::nominal_msdu_size() const
    {
    return params_m.nominal_msdu_size;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
u16_t core_traffic_stream_c::maximum_msdu_size() const
    {
    return params_m.maximum_msdu_size;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::minimum_service_interval() const
    {
    return params_m.minimum_service_interval;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::maximum_service_interval() const
    {
    return params_m.maximum_service_interval;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::inactivity_interval() const
    {
    return params_m.inactivity_interval;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::suspension_interval() const
    {
    return params_m.suspension_interval;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::service_start_time() const
    {
    return params_m.service_start_time;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::minimum_data_rate() const
    {
    return params_m.minimum_data_rate;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::mean_data_rate() const
    {
    return params_m.mean_data_rate;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
u32_t core_traffic_stream_c::peak_data_rate() const
    {
    return params_m.peak_data_rate;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::maximum_burst_size() const
    {
    return params_m.maximum_burst_size; 
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::delay_bound() const
    {
    return params_m.delay_bound;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::minimum_phy_rate() const
    {
    return params_m.minimum_phy_rate;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_traffic_stream_c::surplus_bandwidth_allowance() const
    {
    return params_m.surplus_bandwidth_allowance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_traffic_stream_c::medium_time() const
    {
    return params_m.medium_time;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_tx_rate_e core_traffic_stream_c::nominal_phy_rate() const
    {
    return params_m.nominal_phy_rate;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::override_rates() const
    {
    return params_m.override_rates;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_c::max_tx_msdu_lifetime() const
    {
    return params_m.override_max_tx_msdu_lifetime;   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_c& core_traffic_stream_c::operator=(
    const core_traffic_stream_c& src )
    {
    // Check first assignment to itself
    if( &src == this)
        {
        DEBUG( "core_traffic_stream_c::operator=() - assignment to this" );
        return *this;
        }

    tid_m = src.tid_m;
    user_priority_m = src.user_priority_m;
    default_params_m = src.default_params_m;
    params_m = src.params_m;
    previous_params_m = src.previous_params_m;
    status_m = src.status_m;
    reference_count_m = src.reference_count_m;

    return *this;
    }
