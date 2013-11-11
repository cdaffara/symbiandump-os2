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
* Description:  Class for storing roaming metrics.
*
*/

/*
* %version: 6 %
*/

#include "core_roam_metrics.h"
#include "core_am_tools.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_roam_metrics_c::core_roam_metrics_c() :
    roam_ts_userdata_disabled_m( 0 ),
    roam_ts_userdata_enabled_m( 0 ),
    roam_ts_connect_start_m( 0 ),
    roam_ts_connect_completed_m( 0 ),
    roam_success_count_m( 0 )
    {
    clear_metrics(); 
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_roam_metrics_c::~core_roam_metrics_c()
    {    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_roam_metrics_c::clear_metrics()
    {
    roam_ts_userdata_disabled_m = 0;
    roam_ts_userdata_enabled_m = 0;
    roam_ts_connect_start_m = 0;
    roam_ts_connect_completed_m = 0;
    roam_success_count_m = 0;

    core_tools_c::fillz(
        &roam_attempt_count_m[0],
        sizeof ( roam_attempt_count_m ) );
    core_tools_c::fillz(
        &roam_attempt_failed_count_m[0],
        sizeof ( roam_attempt_failed_count_m ) );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u64_t core_roam_metrics_c::roam_ts_userdata_disabled() const
    {
    return roam_ts_userdata_disabled_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
void core_roam_metrics_c::set_roam_ts_userdata_disabled()
    {
    roam_ts_userdata_disabled_m = core_am_tools_c::timestamp();  
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u64_t core_roam_metrics_c::roam_ts_userdata_enabled() const
    {
    return roam_ts_userdata_enabled_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
void core_roam_metrics_c::set_roam_ts_userdata_enabled()
    {
    roam_ts_userdata_enabled_m = core_am_tools_c::timestamp();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u64_t core_roam_metrics_c::roam_ts_connect_started() const
    {
    return roam_ts_connect_start_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
void core_roam_metrics_c::set_roam_ts_connect_started()
    {
    roam_ts_connect_start_m = core_am_tools_c::timestamp();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u64_t core_roam_metrics_c::roam_ts_connect_completed() const
    {
    return roam_ts_connect_completed_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void core_roam_metrics_c::set_roam_ts_connect_completed()
    {
    roam_ts_connect_completed_m = core_am_tools_c::timestamp();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_roam_metrics_c::roam_total_delay() const
    {
    return static_cast<u64_t>( roam_ts_userdata_enabled_m - roam_ts_userdata_disabled_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_roam_metrics_c::roam_connect_delay() const
    {
    return static_cast<u64_t>( roam_ts_connect_completed_m - roam_ts_connect_start_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_roam_metrics_c::roam_success_count() const
    {
    return roam_success_count_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_roam_metrics_c::inc_roam_success_count()
    {
    ++roam_success_count_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_roam_metrics_c::roam_attempt_count(
    core_roam_reason_e reason ) const
    {
    return roam_attempt_count_m[reason];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_roam_metrics_c::inc_roam_attempt_count(
    core_roam_reason_e reason )
    {
    ++roam_attempt_count_m[reason];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_roam_metrics_c::roam_attempt_failed_count(
    core_roam_failed_reason_e reason ) const
    {
    return roam_attempt_failed_count_m[reason];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//       
void core_roam_metrics_c::inc_roam_attempt_failed_count(
    core_roam_failed_reason_e reason )
    {
    ++roam_attempt_failed_count_m[reason];
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_roam_metrics_c::trace_current_roam_metrics() const
    {
#ifdef _DEBUG
    DEBUG( "core_roam_metrics_c::next_state() - current roam metrics:" );
    DEBUG1( "core_roam_metrics_c::next_state() - last roam total delay in ms:                  %u",
    roam_total_delay() / MILLISECONDS_FROM_MICROSECONDS );
    DEBUG1( "core_roam_metrics_c::next_state() - last roam connect delay in ms:                %u",
        roam_connect_delay() / MILLISECONDS_FROM_MICROSECONDS );
    DEBUG1( "core_roam_metrics_c::next_state() - roam success count:                           %u",
        roam_success_count() );

    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts due to initial connect:         %u",
        roam_attempt_count( core_roam_reason_initial_connect ) );
    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts due to BSS lost:                %u",
        roam_attempt_count( core_roam_reason_bss_lost ) );
    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts due to media disconnect:        %u",
        roam_attempt_count( core_roam_reason_media_disconnect ) );
    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts due to signal strength:         %u",
        roam_attempt_count( core_roam_reason_signal_strength ) );
    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts due to signal prediction:       %u",
        roam_attempt_count( core_roam_reason_signal_loss_prediction ) );
    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts due to failed reauth:           %u",
        roam_attempt_count( core_roam_reason_failed_reauthentication ) );
    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts due to directed roam request:   %u",
        roam_attempt_count( core_roam_reason_directed_roam ) );
    
    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts failed due to timeout:          %u",
        roam_attempt_failed_count( core_roam_failed_reason_timeout ) );
    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts failed due to no suitable ap:   %u",
        roam_attempt_failed_count( core_roam_failed_reason_no_suitable_ap ) );
    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts failed due to AP's status code: %u",
        roam_attempt_failed_count( core_roam_failed_reason_ap_status_code ) );
    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts failed due to EAPOL failure:    %u",
        roam_attempt_failed_count( core_roam_failed_reason_eapol_failure ) );
    DEBUG1( "core_roam_metrics_c::next_state() - roam attempts failed due to other failure:    %u",
        roam_attempt_failed_count( core_roam_failed_reason_other_failure ) );
#endif // _DEBUG    
    }
