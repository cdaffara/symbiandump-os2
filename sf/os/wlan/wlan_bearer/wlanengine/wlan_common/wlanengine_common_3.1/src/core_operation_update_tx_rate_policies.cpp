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
* Description:  Statemachine for updating the currently active TX rate policies.
*
*/

/*
* %version: 3 %
*/

#include "core_operation_update_tx_rate_policies.h"
#include "core_tx_rate_policies.h"
#include "core_frame_dot11_ie.h"
#include "core_server.h"
#include "core_tools_parser.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_update_tx_rate_policies_c::core_operation_update_tx_rate_policies_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_ap_data_c& ap_data ) :
    core_operation_base_c( core_operation_scan, request_id, server, drivers, adaptation,
        core_base_flag_only_one_instance ),
    current_ap_m( &ap_data )
    {
    DEBUG( "core_operation_update_tx_rate_policies_c::core_operation_update_tx_rate_policies_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_update_tx_rate_policies_c::core_operation_update_tx_rate_policies_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation ) :
    core_operation_base_c( core_operation_scan, request_id, server, drivers, adaptation,
        core_base_flag_only_one_instance ),
    current_ap_m( NULL )
    {
    DEBUG( "core_operation_update_tx_rate_policies_c::core_operation_update_tx_rate_policies_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_update_tx_rate_policies_c::~core_operation_update_tx_rate_policies_c()
    {
    DEBUG( "core_operation_update_tx_rate_policies_c::~core_operation_update_tx_rate_policies_c()" );

    current_ap_m = NULL;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_update_tx_rate_policies_c::next_state()
    {
    DEBUG( "core_operation_update_tx_rate_policies_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            operation_state_m = core_state_success;

            if( !server_m->get_core_settings().is_driver_loaded() )
                {
                DEBUG( "core_operation_update_tx_rate_policies_c::next_state() - driver not loaded, completing operation" );
                return core_error_ok;
                }

            if ( !current_ap_m )
                {
                if ( !server_m->get_connection_data() ||
                     !server_m->get_connection_data()->current_ap_data() )
                    {
                    DEBUG( "core_operation_update_tx_rate_policies_c::next_state() - no current AP defined, completing operation" );
                    return core_error_ok;                    
                    }

                current_ap_m = server_m->get_connection_data()->current_ap_data();
                }

            u32_t basic_rates( 0 );
            u32_t supported_rates( 0 );
            for( core_frame_dot11_ie_c* ie = current_ap_m->frame()->first_ie(); ie; ie = current_ap_m->frame()->next_ie() )
                {
                if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_supported_rates ||
                     ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_extended_rates )
                    {
                    u32_t temp_basic_rates( 0 );
                    u32_t temp_supported_rates( 0 );

                    core_tools_parser_c::parse_rates(
                        ie->data() + CORE_FRAME_DOT11_IE_HEADER_LENGTH,
                        ie->length(),
                        temp_basic_rates,
                        temp_supported_rates );

                    basic_rates |= temp_basic_rates;
                    supported_rates |= temp_supported_rates;
                    }

                delete ie;
                }

            DEBUG_RATES( "core_operation_update_tx_rate_policies_c::next_state() - AP basic rates: ",
                basic_rates );
            DEBUG_RATES( "core_operation_update_tx_rate_policies_c::next_state() - AP supported rates: ",
                supported_rates );

            core_ap_type_e current_ap_type(
                core_ap_type_802p11bg );

            /**
             * AP is 802.11b only if only 802.11b rates are advertised as supported rates.
             */
            if ( !( supported_rates & ~CORE_TX_RATES_802P11B ) )
                {
                current_ap_type = core_ap_type_802p11b_only;
                }
            /**
             * AP is 802.11g only if any of the 802.11g rates is a basic rate.
             */
            else if ( basic_rates & CORE_TX_RATES_802P11G )
                {
                current_ap_type = core_ap_type_802p11g_only;
                }

            /**
             * It's possible AP has given us TX rates to be used.
             */
            u32_t override_voice_rates( 0 );
            core_traffic_stream_list_c& ts_list(
                server_m->get_connection_data()->traffic_stream_list() );
            core_traffic_stream_c* iter = ts_list.first();
            while( iter && !override_voice_rates )
                {
                if ( core_tools_c::convert_user_priority_to_ac( iter->user_priority() ) == core_access_class_voice &&
                     iter->status() == core_traffic_stream_status_active &&
                     iter->override_rates() )
                    {
                    override_voice_rates = iter->override_rates();
                    DEBUG_RATES( "core_operation_update_tx_rate_policies_c::next_state() - AP override rates for Voice: ",
                        override_voice_rates );
                    }

                iter = ts_list.next();
                }

            core_tx_rate_policies_s policies;
            core_tx_rate_policy_mappings_s mappings;

            if ( server_m->get_core_settings().is_bt_connection_established() )
                {
                policies.policy_count = 1;
                mappings.policy_for_best_effort = 0;
                mappings.policy_for_background = 0;
                mappings.policy_for_video = 0;
                mappings.policy_for_voice = 0;          

                if ( current_ap_type == core_ap_type_802p11b_only )
                    {
                    DEBUG( "core_operation_update_tx_rate_policies_c::next_state() - using 802.11b-only BT TX rate policy" );

                    policies.policy[0] = TX_RATE_POLICY_BLUETOOTH_B;
                    }
                else if ( current_ap_type == core_ap_type_802p11g_only )
                    {
                    DEBUG( "core_operation_update_tx_rate_policies_c::next_state() - using 802.11g-only BT TX rate policy" );

                    policies.policy[0] = TX_RATE_POLICY_BLUETOOTH_G;
                    }
                else
                    {
                    DEBUG( "core_operation_update_tx_rate_policies_c::next_state() - using 802.11bg BT TX rate policy" );

                    policies.policy[0] = TX_RATE_POLICY_BLUETOOTH_BG;
                    }
                }
            else
                {
                policies.policy_count = 2;
                mappings.policy_for_best_effort = 0;
                mappings.policy_for_background = 0;
                mappings.policy_for_video = 0;
                mappings.policy_for_voice = 1;

                if ( current_ap_type == core_ap_type_802p11b_only )
                    {
                    DEBUG( "core_operation_update_tx_rate_policies_c::next_state() - using 802.11b-only default and voice TX rate policy" );

                    policies.policy[0] = TX_RATE_POLICY_B;
                    policies.policy[1] = TX_RATE_POLICY_VOICE_B;
                    }
                else if ( current_ap_type == core_ap_type_802p11g_only )
                    {
                    DEBUG( "core_operation_update_tx_rate_policies_c::next_state() - using 802.11g-only default and voice TX rate policy" );

                    policies.policy[0] = TX_RATE_POLICY_G;
                    policies.policy[1] = TX_RATE_POLICY_VOICE_G;
                    }
                else
                    {
                    DEBUG( "core_operation_update_tx_rate_policies_c::next_state() - using 802.11bg default and voice TX rate policy" );

                    policies.policy[0] = TX_RATE_POLICY_BG;
                    policies.policy[1] = TX_RATE_POLICY_VOICE_BG;
                    }                    

                if ( override_voice_rates )
                    {
                    DEBUG( "core_operation_update_tx_rate_policies_c::next_state() - generating a new voice TX rate policy based on override rates" );
                    DEBUG_RATES( "core_operation_update_tx_rate_policies_c::next_state() - initial rate: ",
                        core_tools_c::highest_tx_rate( override_voice_rates ) );

                    core_tx_rate_policy_s voice_policy;
                    core_tools_c::fillz(
                        &voice_policy,
                        sizeof( voice_policy ) );
                    voice_policy =
                        core_tools_c::convert_tx_rates_to_tx_policy( override_voice_rates );
                    voice_policy.initial_tx_rate =
                        core_tools_c::highest_tx_rate( override_voice_rates );
                    policies.policy[1] = voice_policy;
                    }
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

            DEBUG( "core_operation_update_rxtx_parameters_c::next_state() - setting TX rate policies" );

            drivers_m->set_tx_rate_policies(
                request_id_m,
                policies,
                mappings );

            break;
            }
        case core_state_success:
            {
            DEBUG( "core_operation_update_tx_rate_policies_c::next_state() - TX rate policies set" );

            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }
