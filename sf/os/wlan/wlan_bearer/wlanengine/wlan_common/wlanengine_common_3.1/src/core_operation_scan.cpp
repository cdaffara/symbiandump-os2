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
* ST-Ericsson
*
* Description:  Statemachine for scanning
*
*/

/*
* %version: 28 %
*/

#include "core_operation_scan.h"
#include "core_server.h"
#include "core_tools.h"
#include "core_frame_beacon.h"
#include "core_scan_list.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_scan_c::core_operation_scan_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_scan_mode_e scan_mode,
    const core_ssid_s& scan_ssid,
    const core_scan_channels_s& scan_channels,
    u8_t scan_max_age,       
    ScanList& scan_data,
    bool_t passive_scan_all_channels,
    bool_t is_current_ap_added ) :
    core_operation_base_c( core_operation_scan, request_id, server, drivers, adaptation,
        core_base_flag_drivers_needed ),    
    scan_mode_m( scan_mode ),
    scan_ssid_m( scan_ssid ),
    scan_channels_m( scan_channels ),
    scan_max_age_m( scan_max_age ),
    scan_data_m( scan_data ),
    passive_scan_all_channels_m( passive_scan_all_channels ),
    region_from_ap_m( core_wlan_region_fcc ),
    is_current_ap_added_m( is_current_ap_added ),
    current_rcpi_m( 0 )
    {
    DEBUG( "core_operation_scan_c::core_operation_scan_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_scan_c::~core_operation_scan_c()
    {      
    DEBUG( "core_operation_scan_c::~core_operation_scan_c()" );

    server_m->unregister_event_handler( this );
    server_m->unregister_frame_handler( this );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_scan_c::next_state()
    {
    DEBUG( "core_operation_scan_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {                        
            operation_state_m = core_state_scan_start;

            u32_t min_ch_time( server_m->get_device_settings().active_scan_min_ch_time );
            u32_t max_ch_time( server_m->get_device_settings().active_scan_max_ch_time );
            if ( scan_mode_m == core_scan_mode_active &&
                 !scan_ssid_m.length &&
                 !server_m->get_core_settings().is_connected() )
                {
                min_ch_time = server_m->get_device_settings().active_broadcast_scan_min_ch_time;
                max_ch_time = server_m->get_device_settings().active_broadcast_scan_max_ch_time;            
                }
            else if ( scan_mode_m == core_scan_mode_passive )
                {
                min_ch_time = server_m->get_device_settings().passive_scan_min_ch_time;
                max_ch_time = server_m->get_device_settings().passive_scan_max_ch_time;
                }

            if ( scan_ssid_m.length )
                {
                DEBUG1S( "core_operation_scan_c::next_state() - requesting a direct scan with SSID ",
                    scan_ssid_m.length, &scan_ssid_m.ssid[0] );
                }
            else
                {
                DEBUG( "core_operation_scan_c::next_state() - requesting a broadcast scan" );
                }

            bool_t is_split_scan( false_t );
            if ( server_m->get_core_settings().is_connected() )
                {
                is_split_scan = true_t;
                DEBUG( "core_operation_scan_c::next_state() - requesting a split-scan" );
                }
            else
                {
                DEBUG( "core_operation_scan_c::next_state() - requesting a regular scan" );
                }

            server_m->get_scan_list().remove_entries_by_age(
                server_m->get_device_settings().scan_list_expiration_time );

            server_m->get_scan_list().set_tag(
                core_scan_list_tag_scan );

            server_m->register_event_handler( this );
            server_m->register_frame_handler( this );
            
            drivers_m->scan(
                request_id_m,
                scan_mode_m,
                scan_ssid_m,
                server_m->get_device_settings().scan_rate,
                server_m->get_core_settings().valid_scan_channels( scan_channels_m ),
                min_ch_time,
                max_ch_time,
                is_split_scan );

            break;
            }
        case core_state_scan_start:
            {
            DEBUG( "core_operation_scan_c::next_state() - scan request completed, waiting for scan completion" );

            break;
            }
        case core_state_scan_complete:
            {          
            /* If country information is not known then channels 12 and 13 can be scanned in passive mode */
            if ( server_m->get_core_settings().mcc_known() || scan_ssid_m.length || !passive_scan_all_channels_m )
            	{
            	/* All possible scans are done. 
            	 * When MCC information is known, the allowed scan channels are known and handled already.
            	 * If SSID is given then direct scan would be required so no need to do passive broadcast scan.
            	 * If operation does not require passive scanning on channels 12 and 13, this is not done. 
            	 */
            	return goto_state( core_state_scan_complete_handle_result );
            	}
            
            operation_state_m = core_state_scan_start_unknown_region;
            
            u32_t min_ch_time( server_m->get_device_settings().passive_scan_min_ch_time );
            u32_t max_ch_time( server_m->get_device_settings().passive_scan_max_ch_time );

            bool_t is_split_scan( false_t );
            if ( server_m->get_core_settings().is_connected() )
                {
                is_split_scan = true_t;
                DEBUG( "core_operation_scan_c::next_state() - requesting a split-scan for channels 12 and 13" );
                }
            else
                {
                DEBUG( "core_operation_scan_c::next_state() - requesting a regular scan for channels 12 and 13" );
                }

            server_m->register_event_handler( this );
            server_m->register_frame_handler( this );

            drivers_m->scan(
                request_id_m,
                core_scan_mode_passive,
                scan_ssid_m,
                server_m->get_device_settings().scan_rate,
                server_m->get_core_settings().invalid_scan_channels( scan_channels_m ),
                min_ch_time,
                max_ch_time,
                is_split_scan );

            break;
            }
        case core_state_scan_start_unknown_region:
            {
            DEBUG( "core_operation_scan_c::next_state() - scan request for channels 12 and 13 completed, waiting for scan completion" );

            break;
            }
        case core_state_scan_complete_unknown_region:
        	{
            server_m->unregister_frame_handler( this );
            server_m->get_scan_list().print_contents(); // Additional print            

            operation_state_m = core_state_scan_complete_store_country_info;

            /* If WLAN region was not known before the scan, then check if country information is present
             * in the scan results. APs on channels 12 and 13 must be ignored if country information indicates this. 
             */
            core_scan_list_iterator_by_tag_c iter_country_beacon(
                server_m->get_scan_list(),
                core_scan_list_tag_scan );

            core_wlan_region_e found_region = core_wlan_region_undefined;
            bool_t inconsistent_info( false_t );
            for ( core_ap_data_c* ap_data = iter_country_beacon.first(); ap_data; ap_data = iter_country_beacon.next() )
                {
                core_country_string_s country_info = ap_data->country_info();
                core_wlan_region_e ap_region = core_wlan_region_undefined;
                if ( country_info.country[0] != 0 )                   
                    {
                    ap_region = core_tools_c::convert_country_to_region( country_info );
                    if ( found_region != core_wlan_region_undefined )
                      	{
                       	if ( ap_region != found_region )
                       		{
                      	    inconsistent_info = true_t; 
                       		}
                       	}
                    else
                       	{
                       	found_region = ap_region;
                       	}                        
                    }
                }
            if ( found_region != core_wlan_region_undefined )
               	{
               	if ( !inconsistent_info )
               		{	
               	    region_from_ap_m = found_region;
               		}
               	else
               		{
               		region_from_ap_m = core_wlan_region_etsi;
               		}
               	adaptation_m->store_ap_country_info( request_id_m, region_from_ap_m, inconsistent_info );
               	}
            else
               	{
               	DEBUG( "core_operation_scan_c::next_state() - country info not found, continue with handling scan results" );
               	return goto_state( core_state_scan_complete_handle_result );
               	}
       	    break;
       	    }
        	
        case core_state_scan_complete_store_country_info:
          	{
            operation_state_m = core_state_scan_complete_handle_result;
            	
            /* Set the new region information also to core settings */
            DEBUG1( "core_operation_scan_c::next_state() - current region %u",
                region_from_ap_m );
            server_m->get_core_settings().set_regional_domain(
                region_from_ap_m );
            server_m->get_core_settings().set_mcc_known( true_t );
            	
            return goto_state( core_state_scan_complete_handle_result );
            }

        case core_state_scan_complete_handle_result:   
          	{
            /* If region is FCC, then ignore and remove the APs from channels 12 and 13 */
            if ( server_m->get_core_settings().regional_domain() == core_wlan_region_fcc )
            	{
            	DEBUG( "core_operation_scan_c::next_state() - remove APs that were found on channels 12 and 13" );
            	remove_disallowed_aps();
            	}

            DEBUG( "core_operation_scan_c::next_state() - final scan list" );
            server_m->get_scan_list().print_contents(); // Additional print            

            /**
             * If we have an ongoing connection, we'll have to see whether
             * the current AP was found in the scan.
             */
            core_ssid_s current_ssid( BROADCAST_SSID );
            core_mac_address_s current_bssid( ZERO_MAC_ADDR );
            bool_t is_current_ap_in_list( true_t );
            if ( is_current_ap_added_m &&
                 server_m->get_connection_data() &&
                 server_m->get_connection_data()->current_ap_data() )
                {
                current_ssid =
                    server_m->get_connection_data()->ssid();
                current_bssid =
                    server_m->get_connection_data()->current_ap_data()->bssid();

                /**
                 * Current AP only needs to be added in a broadcast scan or a direct
                 * scan done with the same SSID.
                 */
                if ( !scan_ssid_m.length ||
                     scan_ssid_m == current_ssid )
                    {
                    is_current_ap_in_list = false_t;
                    }
                }

            if ( scan_ssid_m.length )
                {
                core_scan_list_iterator_by_tag_and_ssid_c iter(
                    server_m->get_scan_list(),
                    core_scan_list_tag_scan,
                    scan_ssid_m );
                for ( core_ap_data_c* ap_data = iter.first(); ap_data; ap_data = iter.next() )
                    {
                    const core_mac_address_s bssid = ap_data->bssid();

                    if ( !server_m->get_core_settings().is_mac_in_permanent_blacklist( bssid ) )
                        {
                        if ( !is_current_ap_in_list &&
                            current_bssid == bssid &&
                            current_ssid == ap_data->ssid() )
                            {
                            is_current_ap_in_list = true_t;
                            }

                        core_tools_c::add_beacon_to_scan_list(
                            scan_data_m,
                            *ap_data,
                            ap_data->rcpi() );
                        }
                    }
                }
            else if ( !scan_max_age_m )
                {
                core_scan_list_iterator_by_tag_c iter(
                    server_m->get_scan_list(),
                    core_scan_list_tag_scan );
                for ( core_ap_data_c* ap_data = iter.first(); ap_data; ap_data = iter.next() )
                    {
                    const core_mac_address_s bssid = ap_data->bssid();

                    if ( !server_m->get_core_settings().is_mac_in_permanent_blacklist( bssid ) )
                        {
                        if ( !is_current_ap_in_list &&
                             current_bssid == bssid &&
                             current_ssid == ap_data->ssid() )
                            {
                            is_current_ap_in_list = true_t;
                            }

                        core_tools_c::add_beacon_to_scan_list(
                            scan_data_m,
                            *ap_data,
                            ap_data->rcpi() );
                        }
                    }
                }
            else
                {
                core_scan_list_iterator_by_age_c iter(
                    server_m->get_scan_list(),
                    scan_max_age_m );
                for ( core_ap_data_c* ap_data = iter.first(); ap_data; ap_data = iter.next() )
                    {
                    const core_mac_address_s bssid = ap_data->bssid();

                    if ( !server_m->get_core_settings().is_mac_in_permanent_blacklist( bssid ) )
                        {
                        if ( !is_current_ap_in_list &&
                             current_bssid == bssid &&
                             current_ssid == ap_data->ssid() )
                            {
                            is_current_ap_in_list = true_t;
                            }

                        core_tools_c::add_beacon_to_scan_list(
                            scan_data_m,
                            *ap_data,
                            ap_data->rcpi() );
                        }
                    }
                }

            if ( is_current_ap_in_list )
                {
                return goto_state( core_state_scanning_done );
                }

            operation_state_m = core_state_rcpi_received;

            DEBUG( "core_operation_scan_c::next_state() - current AP was not found in scan" );
            DEBUG( "core_operation_scan_c::next_state() - requesting RCPI" );

            drivers_m->get_current_rcpi(
                request_id_m,
                current_rcpi_m );

            break;
            }
        case core_state_rcpi_received:
            {
            DEBUG1( "core_operation_scan_c::next_state() - current RCPI is %u",
                current_rcpi_m );
            DEBUG( "core_operation_scan_c::next_state() - appending current AP to the scan list" );

            core_tools_c::add_beacon_to_scan_list(
                scan_data_m,
                *server_m->get_connection_data()->current_ap_data(),
                current_rcpi_m );

            return goto_state( core_state_scanning_done );
            }
        case core_state_scanning_done:
            {
            DEBUG( "core_operation_scan_c::next_state() - scan complete" );
            DEBUG1( "core_operation_scan_c::next_state() - scan list contains %u AP(s)",
                scan_data_m.Count() );            
            DEBUG1( "core_operation_scan_c::next_state() - scan list size is %u bytes",
                scan_data_m.Size() );

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
void core_operation_scan_c::user_cancel(
    bool_t do_graceful_cancel )
    {
    DEBUG1( "core_operation_scan_c::user_cancel(do_graceful_cancel=%d)", do_graceful_cancel );

	/**
	 * If region is FCC and region information is not known, then ignore 
	 * and remove the APs from channels 12 and 13 
	 */
    if ( !server_m->get_core_settings().mcc_known() && 
          server_m->get_core_settings().regional_domain() == core_wlan_region_fcc )
    	{
    	DEBUG( "core_operation_scan_c::next_state() - remove APs that were found on channels 12 and 13" );
    	remove_disallowed_aps();            	
    	}
    
    if ( !do_graceful_cancel )
        {
        /**
         * If we are in a middle of a scan, we have to schedule our own
         * event.
         */
        if ( ( operation_state_m == core_state_scan_start ||
               operation_state_m == core_state_scan_start_unknown_region ) &&
             server_m->event_handler() == this &&
             server_m->frame_handler() == this )
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
bool_t core_operation_scan_c::receive_frame(
    const core_frame_dot11_c* frame,
    u8_t rcpi )
    {
    DEBUG( "core_operation_scan_c::receive_frame()" );
    
    if ( frame->type() != core_frame_dot11_c::core_dot11_type_beacon &&
         frame->type() != core_frame_dot11_c::core_dot11_type_probe_resp )
        {
        DEBUG( "core_operation_scan_c::receive_frame() - not a beacon or a probe" );        
        return false_t;
        }

    core_ap_data_c* ap_data = core_ap_data_c::instance(
        server_m->get_wpx_adaptation_instance(),
        frame,
        rcpi,
        false_t );
    if ( ap_data )
        {        
        const core_ssid_s ssid = ap_data->ssid();
        DEBUG1S( "core_operation_scan_c::receive_frame() - SSID: ",
            ssid.length, &ssid.ssid[0] );

        core_mac_address_s bssid(
            ap_data->bssid() );
        DEBUG6( "core_operation_scan_c::receive_frame() - BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
            bssid.addr[0], bssid.addr[1], bssid.addr[2], 
            bssid.addr[3], bssid.addr[4], bssid.addr[5] ); 
       
        server_m->get_scan_list().update_entry( *ap_data );

        delete ap_data;
        ap_data = NULL;            
        }

    return true_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_operation_scan_c::notify(
    core_am_indication_e indication )
    {
    if ( operation_state_m == core_state_scan_start &&
         indication == core_am_indication_wlan_scan_complete )
        {
        server_m->unregister_event_handler( this );

        DEBUG( "core_operation_scan_c::notify() - scan complete" );

        asynch_goto( core_state_scan_complete, CORE_TIMER_IMMEDIATELY );

        return true_t;
        }
    else if ( operation_state_m == core_state_scan_start_unknown_region && 
    		  indication == core_am_indication_wlan_scan_complete )
    	{
    	server_m->unregister_event_handler( this );

    	DEBUG( "core_operation_scan_c::notify() - scan complete for channels 12 and 13" );

        asynch_goto( core_state_scan_complete_unknown_region, CORE_TIMER_IMMEDIATELY );

        return true_t;
    	}

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_scan_c::remove_disallowed_aps()
    {
    core_type_list_c<core_mac_address_s> remove_ap_list;
                
    core_scan_list_iterator_by_tag_c iter_removed_aps(
        server_m->get_scan_list(),
        core_scan_list_tag_scan );

    core_ap_data_c* ap_data = iter_removed_aps.first();
    while (ap_data)
        {
        if ( !server_m->get_core_settings().is_valid_channel(
                SCAN_BAND_2DOT4GHZ,
                ap_data->channel() ) )
            {
            core_mac_address_s* ignored_ap = new core_mac_address_s;
            if ( ignored_ap )
                {
                *ignored_ap = ap_data->bssid();
                remove_ap_list.append( ignored_ap );
                }
            }
        ap_data = iter_removed_aps.next(); 
        }

    core_mac_address_s* ignored_bssid = remove_ap_list.first(); 
    while (ignored_bssid )
        {
        server_m->get_scan_list().remove_entries_by_bssid( *ignored_bssid );
        ignored_bssid = remove_ap_list.next();
        }
    remove_ap_list.clear();
    
    }
