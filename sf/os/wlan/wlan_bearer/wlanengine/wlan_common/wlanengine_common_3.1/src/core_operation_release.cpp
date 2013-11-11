/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for connection release
*
*/

/*
* %version: 18 %
*/

#include "core_operation_release.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_release_c::core_operation_release_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_release_reason_e reason ) :
    core_operation_base_c( core_operation_release, request_id, server, drivers, adaptation, 
        core_base_flag_none ),
    reason_m( reason ) 
    {
    DEBUG( "core_operation_release_c::core_operation_release_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_release_c::~core_operation_release_c()
    {
    DEBUG( "core_operation_release_c::~core_operation_release_c()" );
    
    if ( server_m->get_connection_data() )
        {
        DEBUG( "core_operation_release_c::~core_operation_release_c() - marking is_disconnecting as false" );
        server_m->get_connection_data()->set_disconnecting(
                false_t );
        }
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_release_c::next_state()
    {
    DEBUG( "core_operation_release_c::next_state()" );
    switch( operation_state_m )
        {
        case core_state_init:
            {                        
            DEBUG( "core_operation_release_c::next_state() - core_state_init" );
            // ensure state is correct
            if( server_m->get_core_settings().connection_state() 
                == core_connection_state_notconnected )
                {
                return core_error_not_supported;
                }

            DEBUG( "core_operation_release_c::next_state() - marking is_disconnecting as true" );
            server_m->get_connection_data()->set_disconnecting(
                true_t );

            server_m->cancel_roam_timer();
            server_m->cancel_dhcp_timer();
            server_m->get_wpx_adaptation_instance().handle_wpx_connection_stop();

#ifdef _DEBUG
            switch ( reason_m )
                {
                case core_release_reason_external_request:
                    DEBUG( "core_operation_release_c::next_state() - release due to core_release_reason_external_request" );
                    break;
                case core_release_reason_max_roam_attempts_exceeded:
                    DEBUG( "core_operation_release_c::next_state() - release due to core_release_reason_max_roam_attempts_exceeded" );

                    switch ( server_m->get_connection_data()->last_roam_reason() )
                        {
                        case core_roam_reason_initial_connect:
                            DEBUG( "core_operation_release_c::next_state() - roam was initiated due to core_roam_reason_initial_connect" );
                            break;
                        case core_roam_reason_bss_lost:
                            DEBUG( "core_operation_release_c::next_state() - roam was initiated due to core_roam_reason_bss_lost" );
                            break;
                        case core_roam_reason_media_disconnect:
                            DEBUG( "core_operation_release_c::next_state() - roam was initiated due to core_roam_reason_media_disconnect" );
                            break;
                        case core_roam_reason_signal_strength:
                            DEBUG( "core_operation_release_c::next_state() - roam was initiated due to core_roam_reason_signal_strength" );
                            break;
                        case core_roam_reason_signal_loss_prediction:
                            DEBUG( "core_operation_release_c::next_state() - roam was initiated due to core_roam_reason_signal_loss_prediction" );
                            break;
                        case core_roam_reason_failed_reauthentication:
                            DEBUG( "core_operation_release_c::next_state() - roam was initiated due to core_roam_reason_failed_reauthentication" );
                            break;
                        default:
                            DEBUG( "core_operation_release_c::next_state() - roam was initiated due to an undefined reason" );
                            ASSERT( false_t );
                        }
                    
                    switch ( server_m->get_connection_data()->last_roam_failed_reason() )
                        {
                        case core_roam_failed_reason_timeout:
                            DEBUG( "core_operation_release_c::next_state() - roam failed due to core_roam_failed_reason_timeout" );
                            break;
                        case core_roam_failed_reason_no_suitable_ap:
                            DEBUG( "core_operation_release_c::next_state() - roam failed due to core_roam_failed_reason_no_suitable_ap" );
                            break;
                        case core_roam_failed_reason_ap_status_code:
                            DEBUG( "core_operation_release_c::next_state() - roam failed due to core_roam_failed_reason_ap_status_code" );
                            break;
                        case core_roam_failed_reason_eapol_failure:
                            DEBUG( "core_operation_release_c::next_state() - roam failed due to core_roam_failed_reason_eapol_failure" );
                            break;
                        case core_roam_failed_reason_other_failure:
                            DEBUG( "core_operation_release_c::next_state() - roam failed due to core_roam_failed_reason_other_failure" );
                            break;
                        default:
                            DEBUG( "core_operation_release_c::next_state() - roam failed due to an undefined reason" );
                            ASSERT( false_t );
                        }
                    break;
                case core_release_reason_hw_failure:
                    DEBUG( "core_operation_release_c::next_state() - release due to core_release_reason_hw_failure" );
                    break;
                case core_release_reason_tkip_mic_failure:
                    DEBUG( "core_operation_release_c::next_state() - release due to core_release_reason_tkip_mic_failure" );
                    break;
                case core_release_reason_other:
                    DEBUG( "core_operation_release_c::next_state() - release due to core_release_reason_other" );
                    break;
                default:
                    DEBUG( "core_operation_release_c::next_state() - release due to an undefined reason" );
                    ASSERT( false_t );
                }

            server_m->get_core_settings().roam_metrics().trace_current_roam_metrics();
#endif // _DEBUG

            /**
             * EAPOL must notified about the disassociation.
             */
            if ( ( server_m->get_connection_data()->iap_data().is_eap_used() ||
                   server_m->get_connection_data()->iap_data().is_wapi_used() ) &&
                 server_m->get_connection_data()->current_ap_data() )
                {
                /*const*/ core_mac_address_s bssid =
                    server_m->get_connection_data()->current_ap_data()->bssid();
                
                network_id_c network(
                    &bssid.addr[0],
                    MAC_ADDR_LEN,
                    &server_m->own_mac_addr().addr[0],
                    MAC_ADDR_LEN,
                    server_m->get_eapol_instance().ethernet_type() );

                DEBUG6( "core_operation_release_c::next_state() - EAPOL disassociation from BSSID %02X:%02X:%02X:%02X:%02X:%02X",
                    bssid.addr[0], bssid.addr[1], bssid.addr[2],
                    bssid.addr[3], bssid.addr[4], bssid.addr[5] );
                DEBUG( "core_operation_release_c::next_state() - marking is_eapol_disconnecting as true" );
                server_m->get_connection_data()->set_eapol_disconnecting(
                    true );

                server_m->get_eapol_instance().disassociation( &network );
                operation_state_m = core_state_eapol_disassociated;

                return core_error_request_pending;
                }

            return goto_state( core_state_eapol_disassociated );
            }
        case core_state_eapol_disassociated:
            {
            DEBUG( "core_operation_release_c::next_state() - core_state_eapol_disassociated" );
            // disable user data
            drivers_m->disable_user_data( request_id_m );
            operation_state_m = core_state_disable_user_data;
            break;
            }
        case core_state_disable_user_data:
            {
            DEBUG( "core_operation_release_c::next_state() - core_state_disable_user_data" );
            drivers_m->set_tx_power_level(
                request_id_m,
                server_m->get_device_settings().tx_power_level );
            operation_state_m = core_state_tx_power_level;            
            break;
            }
        case core_state_tx_power_level:
            {
            DEBUG( "core_operation_release_c::next_state() - core_state_tx_power_level" );

            u32_t tx_level = server_m->get_device_settings().tx_power_level;

            if ( server_m->get_connection_data()->last_tx_level() != tx_level )
                {
                DEBUG( "core_operation_release_c::next_state() - TX level has changed, notifying change" );

                adaptation_m->notify(
                    core_notification_tx_power_level_changed,
                    sizeof( tx_level ),
                    reinterpret_cast<u8_t*>( &tx_level ) );

                server_m->get_connection_data()->set_last_tx_level( tx_level );                    
                }

            drivers_m->disconnect( request_id_m );
            operation_state_m = core_state_disconnect;
            break;
            }
        case core_state_disconnect:
            {
            DEBUG( "core_operation_release_c::next_state() - core_state_disconnect" );

            // notify adaptation
            core_connection_state_e state = core_connection_state_notconnected;
            server_m->get_core_settings().set_connection_state( state );

            if ( server_m->get_connection_data()->last_connection_state() != state )
                {
                u8_t buf[5];
                buf[0] = static_cast<u8_t>( state );
                buf[1] = static_cast<u8_t>( reason_m );
                buf[2] = 0;
                buf[3] = 0;
                if ( reason_m == core_release_reason_max_roam_attempts_exceeded )
                    {
                    buf[2] = static_cast<u8_t>( 
                        server_m->get_connection_data()->last_roam_reason() );
                    buf[3] = static_cast<u8_t>(
                        server_m->get_connection_data()->last_roam_failed_reason() );
                    }

                adaptation_m->notify(
                    core_notification_connection_state_changed,
                    sizeof( buf ),
                    buf );
                    
                server_m->get_connection_data()->set_last_connection_state(
                    state );
                }            

            // destroy connection data
            server_m->clear_connection_data();              

            // Cancel any operations that depend on an active connection.
            server_m->cancel_operations_with_flags(
                core_operation_base_c::core_base_flag_connection_needed );

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
void core_operation_release_c::user_cancel(
    bool_t do_graceful_cancel )
    {
    DEBUG( "core_operation_release_c::user_cancel()" );

    if ( !do_graceful_cancel )
        {
        core_operation_base_c::user_cancel( do_graceful_cancel );
        return;
        }

    /**
     * On graceful cancel this operation must be executed completely
     * to guarantee disconnect. Therefore user_cancel() is ignored.
     */
    }
