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
* Description:  Statemachine for establishing an adhoc network
*
*/


#include "core_sub_operation_adhoc.h"
#include "core_tx_rate_policies.h"
#include "core_frame_dot11.h"
#include "core_server.h"
#include "core_tools.h"
#include "am_debug.h"

/** The channel definition for automatic channel selection. */
const u32_t AUTOMATIC_CHANNEL = 0;

const u32_t CH_WEIGHT_1       = 4;
const u32_t CH_WEIGHT_2       = 2;
const u32_t CH_WEIGHT_3       = 1;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_adhoc_c::core_sub_operation_adhoc_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_ap_data_c** ap_data ) :
    core_operation_base_c( core_operation_unspecified, request_id, server, drivers, adaptation,
        core_base_flag_drivers_needed ),
    ptr_ap_data_m( ap_data ),
    channel_m( 0 )
    {
    DEBUG( "core_sub_operation_adhoc_c::core_sub_operation_adhoc_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_adhoc_c::~core_sub_operation_adhoc_c()
    {
    DEBUG( "core_sub_operation_adhoc_c::~core_sub_operation_adhoc_c()" );

    server_m->unregister_event_handler( this );
    server_m->unregister_frame_handler( this );
    ptr_ap_data_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_adhoc_c::next_state()
    {
    DEBUG( "core_sub_operation_adhoc_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            core_tools_c::fillz(
                &noise_per_channel_m[0],
                SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO * sizeof( u32_t ) );            

            core_iap_data_c* iap = &(server_m->get_connection_data()->iap_data());
            ASSERT( iap->adhoc_channel() <= SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_FCC ); // FCC limitations: no ch 12 or 13
            ASSERT( iap->ssid().length );
            ASSERT( iap->ssid().length <= MAX_SSID_LEN );
            ASSERT( iap->security_mode() == core_security_mode_allow_unsecure ||
                    iap->security_mode() == core_security_mode_wep );

            channel_m = iap->adhoc_channel();
            if ( channel_m != AUTOMATIC_CHANNEL )
                {
                return goto_state( core_state_req_set_tx_rate_policies );
                }

            /**
             * Do a broadcast scan to detect the channel with least traffic.
             */
            operation_state_m = core_state_scan_start;

            server_m->get_scan_list().remove_entries_by_age(
                server_m->get_device_settings().scan_list_expiration_time );

            server_m->register_event_handler( this );
            server_m->register_frame_handler( this );

            drivers_m->scan(
                request_id_m,
                core_scan_mode_passive,
                BROADCAST_SSID,
                server_m->get_device_settings().scan_rate,
                server_m->get_core_settings().all_valid_scan_channels(),
                server_m->get_device_settings().passive_scan_min_ch_time,
                server_m->get_device_settings().passive_scan_max_ch_time,
                false_t );

            break;    
            }
        case core_state_scan_start:
            {
            DEBUG( "core_sub_operation_adhoc_c::next_state() - scan request completed, waiting for scan completion" );

            break;
            }            
        case core_state_scan_complete:
            {
            server_m->unregister_frame_handler( this );

            DEBUG( "core_sub_operation_adhoc_c::next_state() - selecting channel with least noise" );

            /**
             * All channels are now checked and the noise marked.
             * Now, we have to found the smallest noise.
             */
            u32_t tmp_best_noise( 0xFFFFFFFF );
            u8_t tmp_best_channel( 0 );
            for ( u8_t idx = 0; idx < SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_FCC; ++idx ) // Only ch 1 - 11
                {
                DEBUG2( "core_sub_operation_adhoc_c::next_state() - NoiseTable[%u] = %u",
                    idx, noise_per_channel_m[idx] );
                if ( noise_per_channel_m[idx] < tmp_best_noise )
                    {
                    tmp_best_noise = noise_per_channel_m[idx];
                    tmp_best_channel = idx;
                    }
                }
            channel_m = tmp_best_channel + 1;
            DEBUG1( "core_sub_operation_adhoc_c::next_state() - channel %u selected",
                channel_m );

            return goto_state( core_state_req_set_tx_rate_policies );
            }
        case core_state_req_set_tx_rate_policies:
            {
            operation_state_m = core_state_req_start_adhoc;

            core_tx_rate_policies_s policies;
            core_tx_rate_policy_mappings_s mappings;

            if ( server_m->get_core_settings().is_bt_connection_established() )
                {
                DEBUG( "core_sub_operation_adhoc_c::next_state() - using 802.11bg BT TX rate policy" );

                policies.policy_count = 1;
                policies.policy[0] = TX_RATE_POLICY_BLUETOOTH_BG;
                mappings.policy_for_best_effort = 0;
                mappings.policy_for_background = 0;
                mappings.policy_for_video = 0;
                mappings.policy_for_voice = 0;                
                }
            else
                {
                DEBUG( "core_sub_operation_adhoc_c::next_state() - using 802.11bg default and voice TX rate policy" );

                policies.policy_count = 2;
                policies.policy[0] = TX_RATE_POLICY_BG;
                policies.policy[1] = TX_RATE_POLICY_VOICE_BG;
                mappings.policy_for_best_effort = 0;
                mappings.policy_for_background = 0;
                mappings.policy_for_video = 0;
                mappings.policy_for_voice = 1;           
                }

            /**
             * Fill in the default retry limits if not overridden by the policy.
             */
            for ( u8_t idx( 0 ); idx < policies.policy_count; ++idx )
                {
                if ( !policies.policy[idx].short_retry_limit )
                    {
                    policies.policy[idx].short_retry_limit =
                        server_m->get_device_settings().short_retry;
                    }
                if ( ! policies.policy[idx].long_retry_limit )
                    {
                    policies.policy[idx].long_retry_limit =
                        server_m->get_device_settings().long_retry;
                    }
                }

            DEBUG( "core_sub_operation_adhoc_c::next_state() - setting TX rate policies" );

            drivers_m->set_tx_rate_policies(
                request_id_m,
                policies,
                mappings );

            break;
            }            
        case core_state_req_start_adhoc:
            {
            DEBUG( "core_sub_operation_adhoc_c::next_state() - TX rate policies set" );

            operation_state_m = core_state_adhoc_started;

            core_encryption_mode_e encryption_mode = core_encryption_mode_disabled;            
            if( server_m->get_connection_data()->iap_data().security_mode() ==
                core_security_mode_wep )
                {
                encryption_mode = core_encryption_mode_wep;
                }

            DEBUG1( "core_sub_operation_adhoc_c::next_state() - starting the adhoc on channel %u",
                channel_m );

            server_m->register_frame_handler( this );

            drivers_m->start_ibss(
                request_id_m,
                server_m->get_connection_data()->iap_data().ssid(),
                server_m->get_device_settings().beacon,
                channel_m,
                encryption_mode );

            break;    
            }
        case core_state_adhoc_started:
            {
            /**
             * If our own beacon has already been received, proceed to the
             * next state.
             */            
            if( *ptr_ap_data_m )
                {
                return goto_state( core_state_adhoc_complete );
                }
            
            return goto_state( core_state_adhoc_frame );
            }
        case core_state_adhoc_frame:
            {
            DEBUG( "core_sub_operation_adhoc_c::next_state() - waiting for beacon frame" );

            break;
            }
        case core_state_adhoc_complete:
            {
            server_m->unregister_frame_handler( this );

            DEBUG( "core_sub_operation_adhoc_c::next_state() - all done" );           

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
core_error_e core_sub_operation_adhoc_c::cancel()
    {
    DEBUG( "core_sub_operation_adhoc_c::cancel() " );    
    
    switch ( operation_state_m )
        {              
        case core_state_adhoc_started:
            {
            /** The connection attempt failed, we are no longer connected. */
            return next_state();
            }
        default:
            {
            return failure_reason_m;
            }
        }    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_sub_operation_adhoc_c::user_cancel(
    bool_t do_graceful_cancel )
    {
    DEBUG( "core_sub_operation_adhoc_c::user_cancel()" );

    if ( !do_graceful_cancel )
        {
        /**
         * If we are in a middle of a scan, we have to schedule our own
         * event.
         */
        if ( operation_state_m == core_state_scan_start &&
             server_m->event_handler() == this &&
             server_m->frame_handler() == this )         
            {
            asynch_default_user_cancel();
    
            return;
            }

        /**
         * If we are waiting for a beacon, we have to schedule our own
         * event.
         */
        if ( operation_state_m == core_state_adhoc_frame )
            {
            asynch_default_user_cancel();
    
            return;            
            }

        /**
         * Everything else is handled by the default implementation.
         */
        core_operation_base_c::user_cancel( do_graceful_cancel );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
bool_t core_sub_operation_adhoc_c::receive_frame(
    const core_frame_dot11_c* frame,
    u8_t rcpi )
    {
    DEBUG( "core_sub_operation_adhoc_c::receive_frame()" );
    
    if ( frame->type() != core_frame_dot11_c::core_dot11_type_beacon &&
         frame->type() != core_frame_dot11_c::core_dot11_type_probe_resp )
        {
        DEBUG( "core_sub_operation_adhoc_c::receive_frame() - not a beacon or a probe" );        
        return false_t;
        }

    core_ap_data_c* ap_data = core_ap_data_c::instance(
        server_m->get_wpx_adaptation_instance(),
        frame,
        rcpi,
        true_t );
    if ( ap_data )
        {
        server_m->get_scan_list().update_entry( *ap_data );

        if ( operation_state_m == core_state_scan_start )
            {
            u8_t channel = ap_data->channel();
            DEBUG1( "core_sub_operation_adhoc_c::next_state() - AP in channel %u",
                channel );
        
            if ( channel && channel <= SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO )
                {
                /**
                 * Add the ch table as follows:
                 * channel of the AP ==> increase by CH_WEIGHT_1
                 * channels +-1 ==> increase by CH_WEIGHT_2
                 * channels +-2 ==> increase by CH_WEIGHT_3
                 *
                 * Note that the legal channels are 1 - 13 BUT the
                 * array is 0 - 12! Beware edges!
                 */
                noise_per_channel_m[channel - 1] += CH_WEIGHT_1;
                if ( channel > 1 )
                    {
                    noise_per_channel_m[channel - 2] += CH_WEIGHT_2;
                    }
                if ( channel > 2)
                    {
                    noise_per_channel_m[channel - 3] += CH_WEIGHT_3;
                    }
                if ( channel < SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO -2 )
                    {
                    noise_per_channel_m[channel] += CH_WEIGHT_2;
                    }
                if ( channel < SCAN_BAND_2DOT4GHZ_MAX_CHANNEL_EURO -3 )
                    {
                    noise_per_channel_m[channel + 1] += CH_WEIGHT_3;
                    }
                }           
            }
        else if ( operation_state_m == core_state_adhoc_started ||
            operation_state_m == core_state_adhoc_frame )
            {
            if ( frame->type() == core_frame_dot11_c::core_dot11_type_beacon )
                {
                DEBUG( "core_sub_operation_adhoc_c::next_state() - beacon received" );

                ASSERT( !ap_data->is_infra() );
                ASSERT( !*ptr_ap_data_m );

                *ptr_ap_data_m = ap_data;

                /**
                 * If we are just waiting for the beacon frame, we have to schedule
                 * our own timer to proceed.
                 */
                if( operation_state_m == core_state_adhoc_frame )
                    {
                    asynch_goto( core_state_adhoc_complete, CORE_TIMER_IMMEDIATELY );
                    }

                return true_t;
                }
            }

        delete ap_data;
        ap_data = NULL;
        }    

    return true_t;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_sub_operation_adhoc_c::notify(
    core_am_indication_e indication )
    {
    if ( operation_state_m == core_state_scan_start &&
         indication == core_am_indication_wlan_scan_complete )
        {
        server_m->unregister_event_handler( this );

        DEBUG( "core_sub_operation_adhoc_c::notify() - scan complete" );

        asynch_goto( core_state_scan_complete, CORE_TIMER_IMMEDIATELY );

        return true_t;
        }

    return false_t;
    }
