/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for running an echo test in power save.
*
*/


#include "core_operation_power_save_test.h"
#include "core_operation_update_power_mode.h"
#include "core_sub_operation_echo_test.h"
#include "core_tools.h"
#include "core_server.h"
#include "am_debug.h"

/** The maximum amount of times a particular echo frame is re-sent. */
const u8_t CORE_MAX_POWER_SAVE_TEST_RETRY_COUNT = 3;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_power_save_test_c::core_operation_power_save_test_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation ) :
    core_operation_base_c( core_operation_power_save_test, request_id, server, drivers, adaptation,
        core_base_flag_connection_needed | core_base_flag_only_one_instance ),
    current_ap_m( NULL ),
    return_status_m( core_error_ok ),
    is_power_save_m( true_t ),
    is_unicast_test_on_m( true_t )
    {
    DEBUG( "core_operation_power_save_test_c::core_operation_power_save_test_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_power_save_test_c::~core_operation_power_save_test_c()
    {
    DEBUG( "core_operation_power_save_test_c::~core_operation_power_save_test_c()" );

    current_ap_m = NULL;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_power_save_test_c::next_state()
    {
    DEBUG( "core_operation_power_save_test_c::next_state()" );
    
    switch ( operation_state_m )
        {
        case core_state_init:
            {
            DEBUG( "core_operation_power_save_test_c::next_state() core_state_init" );
            if( !server_m->get_core_settings().is_driver_loaded() ||
                 !server_m->get_connection_data() ||
                 !server_m->get_connection_data()->current_ap_data() )
                {
                DEBUG( "core_operation_power_save_test_c::next_state() - not connected, no reason to run the power save test" );

                return core_error_not_supported;
                }

            current_ap_m = server_m->get_connection_data()->current_ap_data();
            core_mac_address_s bssid(
                current_ap_m->bssid() );

            /**
             * If this AP has already been tested, do not test it again.
             */
            bool_t is_success( true_t );
            if( server_m->get_connection_data()->is_ap_power_save_test_run(
                bssid,
                is_success ) )
                {
                DEBUG1( "core_operation_power_save_test_c::next_state() - AP already tested (verdict %u)",
                    is_success );

                return core_error_ok;
                }

            /**
             * If the feature isn't enabled, pretend the test succeeded.
             */
            if( !server_m->get_core_settings().is_feature_enabled(
                core_feature_power_save_test ) )
                {
                return goto_state( core_state_reset_power_mode );
                }

            /**
             * Set the phone to wake up on every DTIM.
             */
            core_operation_base_c* operation = new core_operation_update_power_mode_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                CORE_POWER_SAVE_MODE_EVERY_DTIM );

            return run_sub_operation( operation, core_state_echo_test );
            }
        case core_state_set_power_mode_none:
            {
            DEBUG( "core_operation_power_save_test_c::next_state(): core_state_set_power_mode_none" );
            is_power_save_m = false_t;
            return_status_m = core_error_ok;

            /**
             * Disable power save.
             */
            core_operation_base_c* operation = new core_operation_update_power_mode_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                CORE_POWER_SAVE_MODE_NONE );

            return run_sub_operation( operation, core_state_echo_test );            
            }
        case core_state_set_power_mode_on:
            {
            is_power_save_m = true_t;
            return_status_m = core_error_ok;

            /**
             * Set the phone to wake up on every DTIM.
             */
            core_operation_base_c* operation = new core_operation_update_power_mode_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                CORE_POWER_SAVE_MODE_EVERY_DTIM );
            
            DEBUG( "core_operation_power_save_test_c::next_state(): core_state_set_power_mode_on" );
            is_unicast_test_on_m = false_t;

            return run_sub_operation( operation, core_state_echo_test );            
            }
        case core_state_echo_test:
            {
            /**
             * The phone is now in correct power mode and the echo test can be started.
             */

            /**
             * The response timeout is based on the AP's beacon interval and DTIM period.
             */
            u32_t timeout = ( current_ap_m->beacon_interval() * current_ap_m->dtim_period() + 20 ) *
                MILLISECONDS_FROM_MICROSECONDS;
            DEBUG3( "core_operation_power_save_test_c::next_state() - AP beacon interval is %u ms, DTIM period is %u, timeout is %u us",
                current_ap_m->beacon_interval(),
                current_ap_m->dtim_period(),
                timeout );
 
            
            core_operation_base_c* operation = new core_sub_operation_echo_test_c(
                    request_id_m,
                    server_m,
                    drivers_m,
                    adaptation_m,
                    *current_ap_m,
                    CORE_MAX_POWER_SAVE_TEST_RETRY_COUNT,
                    timeout,
                    is_unicast_test_on_m );

            return run_sub_operation( operation, core_state_echo_test_complete );
            }
        case core_state_echo_test_complete:
            {
            DEBUG( "core_operation_power_save_test_c::next_state(): core_state_echo_test_complete" );
            /**
             * Echo test has been completed.
             */

            /**
             * Echo test can fail because the AP just refuses to echo unicast or
             * broadcast frames. Re-run the test without power save.
             */
            if( is_power_save_m &&
                return_status_m == core_error_timeout )
                {
                return goto_state( core_state_set_power_mode_none ); //first & third
                }
            
            if( is_unicast_test_on_m &&
                return_status_m == core_error_timeout )
                {
                return goto_state( core_state_set_power_mode_on ); //second
                }
                
            return goto_state( core_state_reset_power_mode ); //fourth (or any if success)
            }
        case core_state_reset_power_mode:
            {
            core_mac_address_s bssid(
                current_ap_m->bssid() );

            if( is_power_save_m &&
                return_status_m == core_error_ok )
                {
                /**
                 * Echo test w/ power save was completed successfully. 
                 */
                DEBUG( "core_operation_power_save_test_c::next_state() - power save test verdict: PASS" );

                server_m->get_connection_data()->add_ap_power_save_test_verdict(
                    bssid,
                    true_t );
                }
            else if( return_status_m == core_error_ok )
                {
                /**
                 * Echo test w/ power save failed but succeeded without power save.
                 */
                DEBUG( "core_operation_power_save_test_c::next_state() - power save test verdict: FAIL" );

                server_m->get_connection_data()->add_ap_power_save_test_verdict(
                    bssid,
                    false_t );                

                }
            else if( return_status_m == core_error_timeout )
                {
                /**
                 * Echo test failed both without and with power save.
                 */
                DEBUG( "core_operation_power_save_test_c::next_state() - power save test verdict: INCONCLUSIVE" );

                server_m->get_connection_data()->add_ap_power_save_test_verdict(
                    bssid,
                    true_t );                
                }
            else
                {
                DEBUG1( "core_operation_power_save_test_c::next_state() - power save test verdict: UNKNOWN (failure %u)",
                    return_status_m );
                }

            DEBUG( "core_operation_power_save_test_c::next_state() - scheduling a power save update" );

            core_operation_base_c* operation = new core_operation_update_power_mode_c(
                REQUEST_ID_CORE_INTERNAL,
                server_m,
                drivers_m,
                adaptation_m );

            server_m->queue_int_operation( operation );

            return return_status_m;
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
core_error_e core_operation_power_save_test_c::cancel()
    {
    DEBUG( "core_operation_power_save_test_c::cancel() " );    

    switch ( operation_state_m )
        {
        case core_state_echo_test_complete:
            {
            // Store the return status of the echo test operation.
            return_status_m = failure_reason_m;

            return next_state();
            }
        default:
            {
            return failure_reason_m;
            }
        }
    }
