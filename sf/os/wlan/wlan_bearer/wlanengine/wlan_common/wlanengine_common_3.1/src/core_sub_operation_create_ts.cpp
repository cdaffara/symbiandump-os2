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
* Description:  Statemachine for creating a traffic stream.
*
*/

/*
* %version: 13 %
*/

#include "core_sub_operation_create_ts.h"
#include "core_server.h"
#include "core_frame_wmm_ie.h"
#include "core_frame_wmm_ie_tspec.h"
#include "core_frame_action_wmm.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "am_debug.h"

/** The amount of microseconds to wait for a response to our request. */
const u32_t CORE_AP_RESP_WAITING_TIME = 1000000;

/** The maximum amount of times AP can reject our request due to invalid parameters. */
const u8_t CORE_MAX_INVALID_PARAMETERS_COUNT = 3;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_create_ts_c::core_sub_operation_create_ts_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    const core_ap_data_c& ap_data,
    u8_t tid,
    u8_t user_priority,
    core_traffic_stream_params_s& tspec,
    core_traffic_stream_status_e& stream_status ) :
    core_operation_base_c( core_operation_unspecified, request_id, server, drivers, adaptation,
        core_base_flag_none ),
    current_ap_m( ap_data ),
    tid_m( tid ),
    user_priority_m( user_priority ),
    tspec_m( tspec ),
    stream_status_m( stream_status ),
    invalid_parameters_count_m( 0 )
    {
    DEBUG( "core_sub_operation_create_ts_c::core_sub_operation_create_ts_c()" );

    stream_status_m = core_traffic_stream_status_inactive_other;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_create_ts_c::~core_sub_operation_create_ts_c()
    {
    DEBUG( "core_sub_operation_create_ts_c::~core_sub_operation_create_ts_c()" );

    server_m->unregister_frame_handler( this );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_create_ts_c::next_state()
    {
    DEBUG( "core_sub_operation_create_ts_c::next_state()" );
    
    switch ( operation_state_m )
        {
        case core_state_init:
            {
            bool_t is_uapsd( true_t );
            if ( !current_ap_m.is_uapsd_supported() ||
                 !server_m->get_core_settings().is_uapsd_enable_for_access_class(
                    core_tools_c::convert_user_priority_to_ac( user_priority_m ) ) )
                {
                is_uapsd = false_t;
                }
            DEBUG1( "core_sub_operation_create_ts_c::next_state() - U-APSD %u", is_uapsd );

            core_frame_wmm_ie_tspec_c* tspec_ie = core_frame_wmm_ie_tspec_c::instance(
                tid_m,
                user_priority_m,
                is_uapsd,
                tspec_m.is_periodic_traffic,
                tspec_m.direction,
                tspec_m.nominal_msdu_size,
                tspec_m.maximum_msdu_size,
                tspec_m.minimum_service_interval,
                tspec_m.maximum_service_interval,
                tspec_m.inactivity_interval,
                tspec_m.suspension_interval,
                tspec_m.service_start_time,
                tspec_m.minimum_data_rate,
                tspec_m.mean_data_rate,
                tspec_m.peak_data_rate,
                tspec_m.maximum_burst_size,
                tspec_m.delay_bound,
                tspec_m.minimum_phy_rate,
                tspec_m.surplus_bandwidth_allowance,
                tspec_m.medium_time );
            if ( !tspec_ie )
                {
                DEBUG( "core_sub_operation_create_ts_c::next_state() - unable to allocate a WMM TSPEC IE" );
                return core_error_no_memory;
                }

            DEBUG( "core_sub_operation_create_ts_c::next_state() - TSPEC IE:" );
            DEBUG_BUFFER( tspec_ie->data_length(), tspec_ie->data() );

            const core_mac_address_s dest(
                current_ap_m.bssid() );
            const core_mac_address_s src(
                server_m->own_mac_addr() );

            core_frame_action_wmm_c* frame = core_frame_action_wmm_c::instance(
                0,                  // Duration
                dest,               // Destination
                src,                // Source
                dest,               // BSSID
                0,                  // Sequence Control
                core_frame_action_wmm_c::core_dot11_action_wmm_type_addts_req, // Action Type
                255,                // Dialog Token
                core_frame_action_wmm_c::core_dot11_action_wmm_status_admission_accepted, // Status
                tspec_ie );

            delete tspec_ie;
            tspec_ie = NULL;        

            if ( !frame )
                {
                DEBUG( "core_sub_operation_create_ts_c::next_state() - unable to allocate an action frame" );
                return core_error_no_memory;
                }

            /**
             * Handle the WPX-specific features in traffic stream creation.
             */
            server_m->get_wpx_adaptation_instance().handle_ts_create_request(
                current_ap_m,
                frame,
                tid_m,
                tspec_m );

            DEBUG( "core_sub_operation_create_ts_c::next_state() - ADDTS REQ:" );
            DEBUG_BUFFER( frame->data_length(), frame->data() );

            operation_state_m = core_state_request_sent;            

            server_m->register_frame_handler( this );
            
            server_m->send_management_frame(
                core_frame_type_dot11,
                frame->data_length(),
                frame->data(),
                dest );

            delete frame;
            frame = NULL;

            /**
             * Schedule a timeout if no response is received.
             */
            server_m->schedule_operation_timer(
                CORE_AP_RESP_WAITING_TIME ); 

            break;
            }
        case core_state_request_sent:
            {
            server_m->unregister_frame_handler( this );

            DEBUG( "core_sub_operation_create_ts_c::next_state() - timer has expired, no response from AP received" );

            return core_error_general;
            }
        case core_state_invalid_parameters:
            {
            ++invalid_parameters_count_m;

            if( invalid_parameters_count_m >= CORE_MAX_INVALID_PARAMETERS_COUNT )
                {
                server_m->unregister_frame_handler( this );

                DEBUG( "core_sub_operation_create_ts_c::next_state() - invalid parameters counter exceeded, giving up" );

                return core_error_general;
                }

            DEBUG( "core_sub_operation_create_ts_c::next_state() - AP has downgraded our parameters, retrying" );

            return goto_state( core_state_init );
            }
        case core_state_failure:
            {
            server_m->unregister_frame_handler( this );

            DEBUG( "core_sub_operation_create_ts_c::next_state() - AP has rejected our traffic stream request" );

            return core_error_general;
            }
        case core_state_success:
            {
            server_m->unregister_frame_handler( this );

            DEBUG( "core_sub_operation_create_ts_c::next_state() - traffic stream created successfully" );

            DEBUG_RATES( "core_sub_operation_create_ts_c::next_state() - nominal PHY rate: ",
                tspec_m.nominal_phy_rate );
            DEBUG_RATES( "core_sub_operation_create_ts_c::next_state() - override tx rates: ",
                tspec_m.override_rates );

            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
bool_t core_sub_operation_create_ts_c::receive_frame(
    const core_frame_dot11_c* frame,
    u8_t /* rcpi */ )
    {
    DEBUG( "core_sub_operation_create_ts_c::receive_frame()" );

    if ( frame->type() != core_frame_dot11_c::core_dot11_type_action )
        {
        DEBUG( "core_sub_operation_create_ts_c::receive_frame() - not an action frame" );
        return false_t;
        }

    core_frame_action_c* action = core_frame_action_c::instance(
        *frame );
    if ( action )
        {
        DEBUG( "core_sub_operation_create_ts_c::receive_frame() - 802.11 action frame found" );
        DEBUG1( "core_sub_operation_create_ts_c::receive_frame() - 802.11 action category: 0x%02X",
            action->category() );
        if ( action->category() == core_frame_action_c::core_dot11_action_category_wmm_qos )
            {
            core_frame_action_wmm_c* wmm_action = core_frame_action_wmm_c::instance( *action );
            if ( wmm_action )
                {
                DEBUG( "core_sub_operation_create_ts_c::receive_frame() - 802.11 WMM action frame found" );
                DEBUG1( "core_sub_operation_create_ts_c::receive_frame() - 802.11 WMM action type: 0x%02X",
                    wmm_action->action_type() );
                DEBUG1( "core_sub_operation_create_ts_c::receive_frame() - 802.11 WMM action dialog token: 0x%02X",
                    wmm_action->dialog_token() );
                DEBUG1( "core_sub_operation_create_ts_c::receive_frame() - 802.11 WMM action status: 0x%02X",
                    wmm_action->status() );

                /**
                 * Get traffic stream parameters from the response.
                 */
                bool_t ret = core_tools_parser_c::get_wmm_traffic_stream_params(
                    *wmm_action,
                    tid_m,
                    tspec_m );
                if ( !ret )
                    {
                    DEBUG1( "core_sub_operation_create_ts_c::receive_frame() - TSPEC with TID %u not found, ignoring frame",
                        tid_m );

                    delete wmm_action;
                    wmm_action = NULL;

                    delete action;
                    action = NULL;

                    return true_t;
                    }

                server_m->get_wpx_adaptation_instance().get_wpx_traffic_stream_params(
                    wmm_action,
                    tid_m,
                    tspec_m );

                server_m->cancel_operation_timer();

                if ( wmm_action->status() == core_frame_action_wmm_c::core_dot11_action_wmm_status_admission_accepted )
                    {
                    stream_status_m = core_traffic_stream_status_active;

                    server_m->get_wpx_adaptation_instance().handle_ts_create_success(
                        wmm_action,
                        tid_m,
                        user_priority_m );

                    asynch_goto( core_state_success, CORE_TIMER_IMMEDIATELY );
                    }
                else if ( wmm_action->status() == core_frame_action_wmm_c::core_dot11_action_wmm_status_invalid_parameters )
                    {
                    stream_status_m = core_traffic_stream_status_inactive_invalid_parameters;

                    asynch_goto( core_state_invalid_parameters, CORE_TIMER_IMMEDIATELY );
                    }
                else
                    {
                    stream_status_m = server_m->get_wpx_adaptation_instance().get_wpx_traffic_stream_status(
                        wmm_action->status() );

                    asynch_goto( core_state_failure, CORE_TIMER_IMMEDIATELY );
                    }

                delete wmm_action;
                }
            }

        delete action;
        action = NULL;
        }
        
    return true_t;
    }

