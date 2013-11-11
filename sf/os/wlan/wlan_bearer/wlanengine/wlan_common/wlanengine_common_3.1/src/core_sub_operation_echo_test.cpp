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
* Description:  Statemachine for running an echo test.
*
*/


#include "core_sub_operation_echo_test.h"
#include "core_operation_update_power_mode.h"
#include "core_frame_echo_test.h"
#include "core_tools.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_echo_test_c::core_sub_operation_echo_test_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    const core_ap_data_c& ap_data,
    u8_t max_retry_count,
    u32_t timeout,
    bool_t test_mode ) :
    core_operation_base_c( core_operation_unspecified, request_id, server, drivers, adaptation,
        core_base_flag_connection_needed ),
    current_ap_m( ap_data ),
    max_retry_count_m( max_retry_count ),
    retry_count_m( 0 ),
    timeout_m( timeout ),
    token_m( 0 ),
    return_status_m( core_error_ok ),
    is_unicast_mode_m( test_mode )
    {
    DEBUG( "core_sub_operation_echo_test_c::core_sub_operation_echo_test_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_echo_test_c::~core_sub_operation_echo_test_c()
    {
    DEBUG( "core_sub_operation_echo_test_c::~core_sub_operation_echo_test_c()" );

    server_m->unregister_frame_handler( this );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_echo_test_c::next_state()
    {
    DEBUG( "core_sub_operation_echo_test_c::next_state()" );
    
    core_frame_echo_test_c* frame = NULL;
    
    switch ( operation_state_m )
        {
        case core_state_init:
            {
            if ( is_unicast_mode_m )
                {
                frame = core_frame_echo_test_c::instance(
                    server_m->own_mac_addr(),  
                    server_m->own_mac_addr(), 
                    ++token_m );
                }
            else
                {
                frame = core_frame_echo_test_c::instance(
                    BROADCAST_MAC_ADDR, 
                    server_m->own_mac_addr(),
                    ++token_m );
                }
            if ( !frame )
                {
                DEBUG( "core_sub_operation_echo_test_c::next_state() - unable to generate a test frame" );

                return core_error_no_memory;
                }
            
            if ( is_unicast_mode_m )
                {
                DEBUG( "core_sub_operation_echo_test_c::next_state() - sending a unicast echo test frame:" );
                DEBUG_BUFFER( frame->data_length(), frame->data() );

                operation_state_m = core_state_echo_frame_unicast_timeout;

                server_m->register_frame_handler( this );

                server_m->send_data_frame(
                     current_ap_m,
                     core_frame_type_test,
                     frame->data_length(),
                     frame->data(),
                     core_access_class_best_effort,
                     server_m->own_mac_addr() );  
                }
            else 
                {
                DEBUG( "core_sub_operation_echo_test_c::next_state() - sending a broadcast echo test frame:" );
                DEBUG_BUFFER( frame->data_length(), frame->data() );

                operation_state_m = core_state_echo_frame_broadcast_timeout;

                server_m->register_frame_handler( this );

                server_m->send_data_frame(
                    current_ap_m,
                    core_frame_type_test,
                    frame->data_length(),
                    frame->data(),
                    core_access_class_best_effort,
                    BROADCAST_MAC_ADDR );
                }

            delete frame;
            frame = NULL;

            /**
             * Schedule a timeout if no response is received.
             */
            server_m->schedule_operation_timer(
                timeout_m );

            break;
            }
        case core_state_echo_frame_broadcast_timeout:
            {
            /**
             * Timeout, no response to broadcast frame was received.
             */

            DEBUG( "core_sub_operation_echo_test_c::next_state() - timeout while waiting for a broadcast frame" );
            
            server_m->unregister_frame_handler( this );
            ++retry_count_m;
            if ( retry_count_m < max_retry_count_m )
                {
                DEBUG1( "core_sub_operation_echo_test_c::next_state() - %u broadcast retries still left, retrying",
                    max_retry_count_m - retry_count_m );
                
                return goto_state( core_state_init );
                }            
            else
                {
                DEBUG( "core_sub_operation_echo_test_c::next_state() - no broadcast retries left" );
                }

            return core_error_timeout;
            }
        case core_state_echo_frame_unicast_timeout:
             {
             /**
              * Timeout, no response to unicast frame was received.
              */

             DEBUG( "core_sub_operation_echo_test_c::next_state() - timeout while waiting for a unicast frame" );
             
             server_m->unregister_frame_handler( this );
             ++retry_count_m;
             if ( retry_count_m < max_retry_count_m )
                 {
                 DEBUG1( "core_sub_operation_echo_test_c::next_state() - %u unicast retries still left, retrying",
                     max_retry_count_m - retry_count_m );
                 
                 return goto_state( core_state_init );
                 }            
             else
                 {
                 DEBUG( "core_sub_operation_echo_test_c::next_state() - no unicast retries left" );
                 }

             return core_error_timeout;
             }
        case core_state_echo_frame_unicast:
            {
            /**
             * A unicast frame received successfully.
             */            
            DEBUG( "core_sub_operation_echo_test_c::next_state() unicast - echo test success" );

            return core_error_ok;
            }
        case core_state_echo_frame_broadcast:
            {
            /**
             * A broadcast frame received successfully.
             */            
            DEBUG( "core_sub_operation_echo_test_c::next_state() broadcast - echo test success" );

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
bool_t core_sub_operation_echo_test_c::receive_test_frame(
    const core_frame_echo_test_c* frame,
    u8_t /* rcpi */ )
    {
    DEBUG( "core_sub_operation_echo_test_c::receive_test_frame()" );

    if ( frame->token() != token_m )
        {
        DEBUG2( "core_sub_operation_echo_test_c::receive_test_frame() - token doesn't match, received %u, expected %u",
            frame->token(), token_m );
        }

    if ( operation_state_m == core_state_echo_frame_unicast_timeout &&
         frame->destination() == server_m->own_mac_addr() &&
         frame->source() == server_m->own_mac_addr() )
        {
        DEBUG( "core_sub_operation_echo_test_c::receive_test_frame() - unicast echo test frame received" );

        server_m->cancel_operation_timer();

        asynch_goto( core_state_echo_frame_unicast, CORE_TIMER_IMMEDIATELY );
        }
    else if ( operation_state_m == core_state_echo_frame_broadcast_timeout &&
         frame->destination() == BROADCAST_MAC_ADDR &&
         frame->source() == server_m->own_mac_addr() )
        {
        DEBUG( "core_sub_operation_echo_test_c::receive_test_frame() - broadcast echo test frame received" );

        server_m->cancel_operation_timer();

        asynch_goto( core_state_echo_frame_broadcast, CORE_TIMER_IMMEDIATELY );
        }
    else
        {
        DEBUG( "core_sub_operation_echo_test_c::receive_test_frame() - unknown echo test frame" );        
        DEBUG( "core_sub_operation_echo_test_c::receive_test_frame() - destination:" );
        DEBUG_MAC( frame->destination().addr );
        DEBUG( "core_sub_operation_echo_test_c::receive_test_frame() - source:" );
        DEBUG_MAC( frame->source().addr );
        DEBUG1( "core_sub_operation_echo_test_c::receive_test_frame() - token: %u",
            frame->token() );
        }

    return true_t;
    }
