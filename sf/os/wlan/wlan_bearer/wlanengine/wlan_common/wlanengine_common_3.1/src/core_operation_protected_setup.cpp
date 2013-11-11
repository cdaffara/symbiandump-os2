/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for Wi-Fi Protected setup
*
*/


#include "core_operation_protected_setup.h"
#include "core_operation_scan.h"
#include "core_sub_operation_wpa_connect.h"
#include "core_operation_release.h"
#include "core_server.h"
#include "core_timer_factory.h"
#include "core_callback.h"
#include "core_frame_wsc_ie.h"
#include "core_ap_data.h"
#include "core_tools.h"
#include "core_tools_parser.h"

#include "am_debug.h"

// This is for cheat to enable testing even when there are no selected registrars.
//#define CHEAT_FOR_TESTING
#define SCANLIST_PARSING_AND_PRINTING

/** 
 * Delay between scans when looking for a selected registrar (in microseconds).
 * The default is two seconds.
 */
const u32_t CORE_OPERATION_PROTECTED_SETUP_DELAY_FOR_NEXT_SCAN = 2 * SECONDS_FROM_MICROSECONDS;

/**
 * Timeout for finding a selected registrar (in microseconds). The default value is 120 seconds.
 */
const u32_t CORE_OPERATION_PROTECTED_SETUP_WALKTIME = 120 * SECONDS_FROM_MICROSECONDS;

/**
 * Defines how often Protected Setup is attempted against an AP even if the selected
 * registrar bit is not set. The default is every third scan.
 */
const u32_t CORE_OPERATION_PROTECTED_SETUP_ROUNDTRIP_INTERVAL = 3;

/**
 * Defines how many scans are required before CORE_OPERATION_PROTECTED_SETUP_ROUNDTRIP_INTERVAL
 * is taken into use.
 */
const u32_t CORE_OPERATION_PROTECTED_SETUP_ROUNDTRIP_START = 13;

/**
 * Delay before Protected Setup is attempted against an AP (in microseconds). Some APs are not
 * ready immediately even if the selected registrat bit is set. The default delay is 5 seconds.
 */
const u32_t CORE_OPERATION_PROTECTED_SETUP_START_DELAY = 5 * SECONDS_FROM_MICROSECONDS;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_protected_setup_c::core_operation_protected_setup_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    const core_iap_data_s& iap_data,
    core_type_list_c<core_iap_data_s>& iap_data_list,
    core_protected_setup_status_e& protected_setup_status ) :
    core_operation_base_c( core_operation_protected_setup, request_id, server, drivers, adaptation, 
        core_base_flag_drivers_needed ),
    core_iap_data_m( iap_data ),
    iap_data_list_m( iap_data_list ),
    protected_setup_status_m( protected_setup_status ),
    scan_data_m( NULL ),
    walktime_timer_m( NULL ),
    selected_ap_data_m( NULL ),
    tag_m( core_scan_list_tag_scan ),
    all_valid_scan_channels_m( ),
    assoc_ie_list_m( false_t ),
    is_reassociation_m( false_t ),
    management_status_m( core_management_status_success ),
    is_connected_m( false_t ),
    wps_scan_count_m( 0 ),
    is_selected_registrar_found_m( false_t ),
    is_pushbutton_method_used_m( false_t )
    {
    DEBUG( "core_operation_protected_setup_c::core_operation_protected_setup_c()" );
    
    all_valid_scan_channels_m = server_m->get_core_settings().all_valid_scan_channels();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_protected_setup_c::~core_operation_protected_setup_c()
    {
    DEBUG( "core_operation_protected_setup_c::~core_operation_protected_setup_c()" );
    delete scan_data_m;
    scan_data_m = NULL;
    
    assoc_ie_list_m.clear();

    if ( walktime_timer_m )
        {
        walktime_timer_m->stop();
        core_timer_factory_c::destroy_timer( walktime_timer_m );
        walktime_timer_m = NULL;
        }
    selected_ap_data_m = NULL;

    if ( operation_state_m > core_state_init
        && server_m->get_core_settings().connection_state() != core_connection_state_notconnected )
        {
        DEBUG( "core_operation_protected_setup_c::~core_operation_protected_setup_c() - Error: Connection is not released!" );
        ASSERT( false_t );
        }
    server_m->set_protected_setup_handler( NULL );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_protected_setup_c::next_state()
    {
    DEBUG( "core_operation_protected_setup_c::next_state()" );
    switch( operation_state_m )
        {
        case core_state_init:
            {
            // This state is for initial actions.
            DEBUG( "- core_state_init" );
            DEBUG1S("User selected SSID: ", core_iap_data_m.ssid.length, core_iap_data_m.ssid.ssid);
            
            if ( core_iap_data_m.wpa_preshared_key.key_length == 0 )
                {
                DEBUG( "Using Pushbutton method" );
                is_pushbutton_method_used_m = true_t;
                }
            else
                {
                DEBUG( "Using Pincode method" );
                is_pushbutton_method_used_m = false_t;
                }

            scan_data_m = new ScanList;
            if( !scan_data_m )
                {
                DEBUG( "core_operation_protected_setup_c::next_state() - unable to create ScanList" );
                return core_error_no_memory;
                }
            
            core_callback_c* timer_callback = 
                new core_callback_c( &(core_operation_protected_setup_c::timer_expired), this );
            if( !timer_callback )
                {
                DEBUG( "core_operation_protected_setup_c::next_state() - unable to create callbacks" );
                return core_error_no_memory;
                }

            walktime_timer_m = core_timer_factory_c::create_timer( timer_callback );
            if( !walktime_timer_m )
                {
                DEBUG( "core_operation_protected_setup_c::next_state() - unable to create timer" );
                delete timer_callback;
                return core_error_no_memory;
                }


            if ( server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_protected_setup_c::next_state() - already connected, completing request" );

                return core_error_connection_already_active;
                }

            // Override the security mode just in case.
            core_iap_data_m.security_mode = core_security_mode_protected_setup;
            
            if ( !server_m->create_eapol_instance( core_eapol_operating_mode_wfa ) )
                {
                DEBUG( "core_operation_protected_setup_c::next_state() - unable to instantiate EAPOL" );
                return core_error_no_memory;
                }

            walktime_timer_m->start( CORE_OPERATION_PROTECTED_SETUP_WALKTIME );
            
            return goto_state( core_state_prepare_scanning );
            }
        // This state is for actions what should be done at start or after core_operation_release_c.
        case core_state_prepare_scanning:
            {
            DEBUG( "- core_state_prepare_scanning" );
            core_error_e ret = server_m->init_connection_data(
                    core_iap_data_m,
                    server_m->get_device_settings() );
            if ( ret != core_error_ok )
                {
                DEBUG1( "core_operation_protected_setup_c::next_state() - unable to initialize connection data (%d)", ret );
                return ret;
                }

            if ( !server_m->get_connection_data()->iap_data().is_eap_used() )
                {
                DEBUG( "core_operation_protected_setup_c::next_state() - is_eap_used() == false, it should be true." );
                return core_error_illegal_argument;
                }

            server_m->get_core_settings().set_connection_state( core_connection_state_searching );
            
            return goto_state( core_state_start_scanning );
            }
            
        case core_state_start_scanning:
            {
            DEBUG( "- core_state_start_scanning" );
            operation_state_m = core_state_scan;
            
            // Clear scanlist 
            ASSERT( scan_data_m );
            scan_data_m->ClearAll();
            
            // add tag to help scanlist iteration
            server_m->get_scan_list().set_tag( tag_m );
            selected_ap_data_m = NULL;
            
            // Broadcast scan is used to detect session overlap.
            // Otherwise this could be direct scan.
            core_operation_base_c* operation = new core_operation_scan_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                core_scan_mode_active,
                BROADCAST_SSID,
                all_valid_scan_channels_m,
                0,
                *scan_data_m,
                false_t,
                false_t );
                
            return run_sub_operation( operation );
            }
            
        case core_state_scan:
            {
            DEBUG( "- core_state_scan" );
            operation_state_m = core_state_MAX; // This is just to spot leak in following switch.
            ++wps_scan_count_m;
            
#ifdef SCANLIST_PARSING_AND_PRINTING

            core_scan_list_iterator_by_tag_c parsing_iter(
                server_m->get_scan_list(),
                tag_m);
            for ( core_ap_data_c* ap_data = parsing_iter.first(); ap_data; ap_data = parsing_iter.next() )
                {
                u8_t buffer[70];
                core_tools_c::fillz(buffer, sizeof(buffer) );
                
                for (u32_t i=0; i<ap_data->ssid().length; ++i)
                    {
                    buffer[2*i]   = ap_data->ssid().ssid[i];
                    buffer[2*i+1] = 0;
                    }
                
                DEBUG8("RADAR: SSID:%s BSSID:%02x.%02x.%02x.%02x.%02x.%02x RCPI:%u",
                    buffer,
                    //ap_data->ssid().ssid,
                    ap_data->bssid().addr[0],
                    ap_data->bssid().addr[1],
                    ap_data->bssid().addr[2],
                    ap_data->bssid().addr[3],
                    ap_data->bssid().addr[4],
                    ap_data->bssid().addr[5],
                    ap_data->rcpi()
                    );
                }
                
#endif // SCANLIST_PARSING_AND_PRINTING

            // Decide whether ap_data should be copied even when selected_registrar is not set.
            bool_t copy_ap_data( false_t );
            if( wps_scan_count_m == 1 ||
                ( wps_scan_count_m >= CORE_OPERATION_PROTECTED_SETUP_ROUNDTRIP_START &&
                  ( wps_scan_count_m % CORE_OPERATION_PROTECTED_SETUP_ROUNDTRIP_INTERVAL == 1 ) ) )
                {
                copy_ap_data = true_t;
                }
            DEBUG2( "core_operation_protected_setup_c::next_state() - wps_scan_count_m=%d, copy_ap_data=%d", wps_scan_count_m, copy_ap_data );
            
            abs_core_scan_list_iterator_c * iter;
            
            if ( is_pushbutton_method_used_m )
                {
                // Pushbutton: Check all APs from scanlist.
                iter = new core_scan_list_iterator_by_tag_c(
                        server_m->get_scan_list(),
                        tag_m);
                }
            else
                {
                // Pincode: Check only APs with given SSID from scanlist.
                iter = new core_scan_list_iterator_by_tag_and_ssid_c(
                        server_m->get_scan_list(),
                        tag_m,
                        core_iap_data_m.ssid);
                }

            u32_t selected_registrar_count( check_selected_registrars( iter, copy_ap_data ) );
            delete iter;
            iter = NULL;
            switch ( selected_registrar_count )
                {
                case 0: // No selected registrars yet. We should wait until some registrar appears
                    {
                    // Second round-trip should not start before selected_registrar is found.
                    if ( !selected_ap_data_m )
                        {
                        // Wait some time before new scan
                        return asynch_goto( core_state_start_scanning, CORE_OPERATION_PROTECTED_SETUP_DELAY_FOR_NEXT_SCAN );
                        }
                    }
                    /* 
                     * This will fall through in first round-trip if selected AP was found.
                     * It means that selected_registrar flag is not required until second round-trip.
                     */
                    //lint -fallthrough
                case 1: // Successfull case: one and only one selected registrar
                    {
                    // AP not found. It is coding error.
                    ASSERT( selected_ap_data_m );
                    
                    // Check that selected registrar is in SSID where it should be...
                    if ( core_iap_data_m.ssid != selected_ap_data_m->ssid() )
                        {
                        // If it is not, then wait some time before new scan
                        DEBUG( "core_operation_protected_setup_c::next_state() - selected registrar is not in wanted AP, ignoring" );
                        return asynch_goto( core_state_start_scanning, CORE_OPERATION_PROTECTED_SETUP_DELAY_FOR_NEXT_SCAN );
                        }
                    if ( selected_registrar_count > 0 )
                        {
                        // selected_registrar == true was found in correct AP.
                        is_selected_registrar_found_m = true_t;
                        }

                    return asynch_goto( core_state_connect_and_setup, CORE_OPERATION_PROTECTED_SETUP_START_DELAY );
                    }
                default: // More than one selected registrar: It means "multiple PBC sessions detected"
                    {
                    // But if we are using pincode method, then we just try connection with first AP.
                    // No need to check SSID, because it is already filtered.
                    if ( !is_pushbutton_method_used_m )
                        {
                        DEBUG1( "core_operation_protected_setup_c::next_state() - %i selected registrars found for the same SSID, trying pincode for first AP.", selected_registrar_count );
                        is_selected_registrar_found_m = true_t;
                        return asynch_goto( core_state_connect_and_setup, CORE_OPERATION_PROTECTED_SETUP_START_DELAY );
                        }
                    
                    protected_setup_status_m = core_protected_setup_status_multiple_PBC_sessions_detected;
                    walktime_timer_m->stop();

                    server_m->get_core_settings().set_connection_state( core_connection_state_notconnected );

                    return core_error_ok;
                    }
                }
            }

        case core_state_connect_and_setup:
            {
            DEBUG( "- core_state_connect_and_setup" );
            
            if ( is_selected_registrar_found_m )
                {
                /**
                 * Walk time timer can be stopped because user has pressed 
                 * Push-button or PIN-code from both phone and registrar.
                 * Do not stop timer, if selected registrar is not found.
                 */
                ASSERT( walktime_timer_m );
                walktime_timer_m->stop();
                }
            
            core_iap_data_c core_iap_data( core_iap_data_m );

            operation_state_m = core_state_setup_completed;
            
            // Add WSC IE to association request
            core_frame_wsc_ie_c* wsc_ie = core_frame_wsc_ie_c::instance(
                CORE_FRAME_WSC_IE_USED_VERSION, 
                CORE_FRAME_WSC_IE_REQUEST_TYPE_ENROLLEE );
            
            if ( wsc_ie == NULL )
                {
                server_m->get_core_settings().set_connection_state( core_connection_state_notconnected );
                return core_error_no_memory;
                }
            assoc_ie_list_m.append(
                wsc_ie,
                wsc_ie->element_id() );
            
            ASSERT( selected_ap_data_m != NULL );
            
            server_m->set_protected_setup_handler( this );
            
            server_m->get_core_settings().set_connection_state( core_connection_state_secureinfra );

            // WPA connect initiates connection and start authentication (in this case starts protected setup)
            core_operation_base_c* operation = new core_sub_operation_wpa_connect_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                is_connected_m,
                management_status_m,
                core_iap_data_m.ssid,
                *selected_ap_data_m,
                is_reassociation_m,
                assoc_ie_list_m,
                NULL );

            return run_sub_operation( operation );
            }

        case core_state_setup_completed:
            {
            DEBUG( "- core_state_setup_completed" );

            protected_setup_status_m = core_protected_setup_status_ok;
            ASSERT( walktime_timer_m );
            walktime_timer_m->stop();

            DEBUG( "Wi-Fi Protected Setup done, starting disconnect." );

            operation_state_m = core_state_disconnect;
            
            core_operation_base_c* operation = new core_operation_release_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                core_release_reason_other );

            return run_sub_operation( operation );
            }
            
        case core_state_disconnect:
            {
            DEBUG( "- core_state_disconnect" );
            server_m->get_core_settings().set_connection_state( core_connection_state_notconnected );
            return core_error_ok;
            }
            
        case core_state_user_cancel:
            {
            DEBUG( "- core_state_user_cancel" );
            
            operation_state_m = core_state_disconnect_on_cancel;
            
            // 
            ASSERT( walktime_timer_m );
            walktime_timer_m->stop();
            
            // We should cancel this timer, because asynch_goto could be ongoing.
            server_m->cancel_operation_timer();
            
            // This is not necessary? operation release will check this also...
            if( server_m->get_core_settings().connection_state() 
                == core_connection_state_secureinfra )
                {
                core_operation_base_c* operation = new core_operation_release_c(
                    request_id_m,
                    server_m,
                    drivers_m,
                    adaptation_m,
                    core_release_reason_external_request );

                return run_sub_operation( operation );
                }
            return next_state( );
            }
            
        case core_state_disconnect_on_cancel:
            {
            DEBUG( "- core_state_disconnect_on_cancel" );

            server_m->get_core_settings().set_connection_state( core_connection_state_notconnected );

            return core_error_cancel;
            }

        case core_state_setup_failed:
            {
            DEBUG( "- core_state_setup_failed" );

            operation_state_m = core_state_disconnect_on_error;
            ASSERT( walktime_timer_m );
            walktime_timer_m->stop();

            // Get EAPOL errorcode before releasing connection.
            wlan_eapol_if_eap_status_e eapol_status = static_cast<wlan_eapol_if_eap_status_e>( server_m->get_connection_data()->last_eap_error() );
            protected_setup_status_m = core_tools_c::convert_eapol_error_to_protected_setup_status( eapol_status );
            
            DEBUG2( "Protected Setup has failed, starting disconnect. (EAPOL status %i, protected setup status %i)",
                    eapol_status,
                    protected_setup_status_m );
            server_m->get_core_settings().set_connection_state( core_connection_state_secureinfra );

            core_operation_base_c* operation = new core_operation_release_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                core_release_reason_other );

            return run_sub_operation( operation );
            }

        case core_state_disconnect_on_error:
            {
            DEBUG( "- core_state_disconnect_on_error" );

            server_m->get_core_settings().set_connection_state( core_connection_state_notconnected );

            DEBUG1( "core_operation_protected_setup_c::next_state() - failure_reason_m is %u",
                failure_reason_m );
            DEBUG1( "core_operation_protected_setup_c::next_state() - management_status_m is %u",
                management_status_m );

            core_error_e ret = protected_setup_status(
                failure_reason_m,
                management_status_m,
                protected_setup_status_m );
            DEBUG1( "core_operation_protected_setup_c::next_state() - returned error code is %u",
                ret );
            DEBUG1( "core_operation_protected_setup_c::next_state() - returned status is %u",
                protected_setup_status_m );

            return ret;
            }

        case core_state_disconnect_before_second_round_trip:
            {
            DEBUG( "- core_state_disconnect_before_second_round_trip" );

            operation_state_m = core_state_prepare_scanning;

            // Get EAPOL errorcode before releasing connection.
            wlan_eapol_if_eap_status_e eapol_status = static_cast<wlan_eapol_if_eap_status_e>( server_m->get_connection_data()->last_eap_error() );
            protected_setup_status_m = core_tools_c::convert_eapol_error_to_protected_setup_status( eapol_status );
            
            DEBUG2( "Protected Setup has failed (first round-trip), starting disconnect. (EAPOL status %i, protected setup status %i)",
                    eapol_status,
                    protected_setup_status_m );
            server_m->get_core_settings().set_connection_state( core_connection_state_secureinfra );

            core_operation_base_c* operation = new core_operation_release_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                core_release_reason_other );

            return run_sub_operation( operation );
            }
        
        case core_state_walktime_expiration:
            {
            DEBUG( "- core_state_walktime_expiration" );

            server_m->get_core_settings().set_connection_state( core_connection_state_notconnected );

            // indicate unsuccessfully completed protected setup
            protected_setup_status_m = core_protected_setup_status_walktime_expired;

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
core_error_e core_operation_protected_setup_c::cancel()
    {
    DEBUG( "core_operation_protected_setup_c::cancel()" );

    if ( operation_state_m == core_state_setup_completed )
        {
        DEBUG( "core_operation_protected_setup_c::cancel() - Protected Setup has failed " );
        if ( !is_selected_registrar_found_m )
            {
            DEBUG( "core_operation_protected_setup_c::cancel() - Goto core_state_disconnect_before_second_round_trip");
            return asynch_goto( core_state_disconnect_before_second_round_trip, CORE_TIMER_IMMEDIATELY );
            }
        return goto_state( core_state_setup_failed );
        }

    server_m->get_core_settings().set_connection_state( core_connection_state_notconnected );

    return failure_reason_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Handle cancel from user.
// 
void core_operation_protected_setup_c::user_cancel(
    bool_t /* do_graceful_cancel */ )
    {
    DEBUG( "core_operation_protected_setup_c::user_cancel()" );

    // This will move execution to user cancel state, when we get execution time next time.
    operation_state_m = core_state_user_cancel;
    
    return;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_operation_protected_setup_c::check_selected_registrars(
        abs_core_scan_list_iterator_c * iter,
        bool_t copy_ap_data_always )
    {
    DEBUG( "core_operation_protected_setup_c::check_selected_registrars()" );
    u32_t registrar_count = 0;
    
    // Search through scan_list to count all selected registrars
    for ( core_ap_data_c* ap_data = iter->first(); ap_data; ap_data = iter->next() )
        {
        const core_frame_wsc_ie_c* wsc_ie = ap_data->wsc_ie();
        if ( wsc_ie != NULL )
            {
            // Do we need to check something else? AP setup locked?
            if (wsc_ie->selected_registrar() == true_t)
                {
                ++registrar_count;
                selected_ap_data_m = ap_data;
                DEBUG1S( "core_operation_protected_setup_c::check_selected_registrars() - selected_registrar=true, SSID: ", 
                    ap_data->ssid().length, 
                    ap_data->ssid().ssid );
                }
            else
                {
                DEBUG( "core_operation_protected_setup_c::check_selected_registrars() - selected_registrar=false" );
                
                // Copy ap_data in first round-trip. Do not overwrite if selected_registrar is already found.
                if ( copy_ap_data_always && 
                     registrar_count == 0 &&
                     core_iap_data_m.ssid == ap_data->ssid() )
                    {
                    DEBUG( "core_operation_protected_setup_c::check_selected_registrars() - SSID found. Copying ap_data.");
                    selected_ap_data_m = ap_data;
                    }
                }
            
            delete wsc_ie;
            wsc_ie = NULL;
            }

#ifdef CHEAT_FOR_TESTING
        if ( core_tools_c::compare(
            core_iap_data_m.ssid.ssid,
            core_iap_data_m.ssid.length,
            ap_data->ssid().ssid,
            ap_data->ssid().length ) == 0)
            {
            DEBUG( "copy ap_data for testing...");
            selected_ap_data_m = ap_data;
            }
#endif // CHEAT_FOR_TESTING
        }
    DEBUG1( "core_operation_protected_setup_c::check_selected_registrars() - registrar_count %i", registrar_count);
    return registrar_count;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_protected_setup_c::timer_expired( void* this_ptr )
    {
    DEBUG( "core_operation_protected_setup_c::timer_expired()" );

    core_operation_protected_setup_c* self =
        static_cast<core_operation_protected_setup_c*>( this_ptr );
    
    ASSERT( self );
    
    // If scan is ongoing, do not use operation_timer.
    if ( self->operation_state_m == core_state_scan )
        {
        DEBUG( "core_operation_protected_setup_c::timer_expired() - goto core_state_walktime_expiration after scan_complete" );
        self->operation_state_m = core_state_walktime_expiration;
        }
    else if ( self->operation_state_m == core_state_setup_completed )
        {
        DEBUG( "core_operation_protected_setup_c::timer_expired() - Ignoring walktime timer. Let EAPOL continue." );
        /* 
         * This will prevent newer ending round-trips, if walktime timer expires 
         * while EAPOL is running protected setup and if it fails.
         */
        self->is_selected_registrar_found_m = true_t;
        }
    else
        {
        DEBUG( "core_operation_protected_setup_c::timer_expired() - goto core_state_walktime_expiration immediately" );
        
        // Must stop operation timer before asynch_goto.
        self->server_m->cancel_operation_timer();
        self->asynch_goto( core_state_walktime_expiration, CORE_TIMER_IMMEDIATELY );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_protected_setup_c::handle_protected_setup_network(
    const core_iap_data_s& iap_data )
    {
    DEBUG( "core_operation_protected_setup_c::handle_protected_setup_network()" );  

    core_iap_data_s* instance = new core_iap_data_s(
        iap_data );
    if ( instance )
        {
        iap_data_list_m.append( instance );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_protected_setup_c::protected_setup_status(
    core_error_e request_status,
    core_management_status_e management_status,
    core_protected_setup_status_e& protected_setup_status ) const
    {
    
    /**
     * If management status is defined, the AP has refused our authentication
     * or association.
     */
    if ( management_status != core_management_status_success )
        {
        switch ( management_status )
            {
            case core_management_status_auth_algo_not_supported:
                /** Falls through on purpose. */
            case core_management_status_auth_frame_out_of_sequence:
                /** Falls through on purpose. */
            case core_management_status_auth_challenge_failure:
                /** Falls through on purpose. */
            case core_management_status_auth_timeout:
                {
                protected_setup_status = core_protected_setup_status_network_auth_failure;
                break;
                }
            default:
                {
                protected_setup_status = core_protected_setup_status_network_assoc_failure;
                break;
                }
            }

        return core_error_ok;
        }

    /**
     * If EAPOL has indicated an error, determine the error code from
     * the saved notifications.
     */
    if ( request_status == core_error_eapol_total_failure ||
         request_status == core_error_eapol_failure )
        {
        // In this case, protected_setup_status is already set from EAPOL errorcode (no need to set it here).
        return core_error_ok;
        }
    else if ( request_status == core_error_timeout )
    	{
    	protected_setup_status = core_protected_setup_status_network_assoc_failure;
    	return core_error_ok;
    	}
    else if ( request_status != core_error_ok )
    	{
    	protected_setup_status = core_protected_setup_status_network_auth_failure;
    	return core_error_ok;
    	}
    
    /**
     * An error occured somewhere else, has nothing to do with Protected Setup.
     */
    protected_setup_status = core_protected_setup_status_undefined;

    return request_status;
    }
