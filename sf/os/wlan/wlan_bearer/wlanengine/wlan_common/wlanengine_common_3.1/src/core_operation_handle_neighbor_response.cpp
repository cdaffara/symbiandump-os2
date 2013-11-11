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
* Description:  Statemachine for handling received neighbor response.
*
*/


#include "core_operation_handle_neighbor_response.h"
#include "core_frame_dot11_ie.h"
#include "core_frame_mgmt_ie.h"
#include "core_frame_nr_ie.h"
#include "core_frame_action_nr.h"
#include "core_tools.h"
#include "core_server.h"
#include "am_debug.h"


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_handle_neighbor_response_c::core_operation_handle_neighbor_response_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_frame_action_nr_c* neighbor_response ) :
    core_operation_base_c( core_operation_handle_neighbor_response, request_id, server, drivers, adaptation, 
        core_base_flag_none ),
    neighbor_response_m( neighbor_response ),
    current_ie_m( NULL ),
    neighbor_resp_parser_m ( NULL )
    {
    DEBUG( "core_operation_handle_neighbor_response_c::core_operation_handle_neighbor_response_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_handle_neighbor_response_c::~core_operation_handle_neighbor_response_c()
    {
    DEBUG( "core_operation_handle_neighbor_response_c::~core_operation_handle_neighbor_response_c()" );

    ie_list_m.clear();
    
    if ( neighbor_response_m )
        {
        delete neighbor_response_m;
        neighbor_response_m = NULL;
        }

    if ( current_ie_m )
        {
        delete current_ie_m;
        current_ie_m = NULL;
        }
    
    if ( neighbor_resp_parser_m )
        {
        delete neighbor_resp_parser_m;
        neighbor_resp_parser_m = NULL;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_handle_neighbor_response_c::next_state()
    {
    DEBUG( "core_operation_handle_neighbor_response_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            DEBUG( "core_operation_handle_neighbor_response_c::next_state() - core_state_init" );
            
            if ( !server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_handle_neighbor_response_c::next_state() - not connected, nothing to do" );

                return core_error_general;
                }
                        
            DEBUG1( "core_operation_handle_neighbor_response_c::next_state() - Dialog token: 0x%02X",
                    neighbor_response_m->dialog_token() );
            
            // loop through IEs
            for ( core_frame_dot11_ie_c* ie = neighbor_response_m->first_ie(); ie; ie = neighbor_response_m->next_ie() )
                {
                DEBUG( "core_operation_handle_neighbor_response_c::next_state() - Found IE:" );
                DEBUG1( "core_operation_handle_neighbor_response_c::next_state() - ID: 0x%02X",
                    ie->element_id() );
                DEBUG1( "core_operation_handle_neighbor_response_c::next_state() - Length: 0x%02X",
                    ie->length() );
                
                ie_list_m.append( ie );
                }
            
            if ( ie_list_m.count() == 0 )
                {
                DEBUG( "core_operation_handle_neighbor_response_c::next_state() - Neighbor response didn't contain any IEs" );
                DEBUG( "core_operation_handle_neighbor_response_c::next_state() - Going to core_state_operation_finished." );
                
                return goto_state( core_state_operation_finished );
                }
            
            return goto_state( core_state_get_next_ie );
            }
        case core_state_get_next_ie:
            {
            DEBUG( "core_operation_handle_neighbor_response_c::next_state() - state core_state_get_next_ie" );
            
            if ( current_ie_m != NULL )
                {
                // deallocate IE
                delete current_ie_m;
                current_ie_m = NULL;
                }

            current_ie_m = ie_list_m.first();
            
            if ( current_ie_m == NULL )
                {
                DEBUG( "core_operation_handle_neighbor_response_c::next_state() - All IEs processed" );
                return goto_state( core_state_operation_finished );
                }
            
            // remove ie from list, it is now owned by current_ie_m
            ie_list_m.remove ( current_ie_m );
            
            return asynch_goto( core_state_process_current_ie );
            }
        case core_state_process_current_ie:
            {
            if ( current_ie_m->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_neighbor_report )
                {
                // parse IE
                parse( current_ie_m );
                
                // deallocate IE
                delete current_ie_m;
                current_ie_m = NULL;
                }

            DEBUG( "core_operation_handle_neighbor_response_c::next_state() - Going to core_state_get_next_ie." );
            
            return goto_state( core_state_get_next_ie );
            }
        case core_state_operation_finished:
            {
            DEBUG( "core_operation_handle_neighbor_response_c::next_state() - state core_state_operation_finished" );
            
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
void core_operation_handle_neighbor_response_c::parse(
        core_frame_dot11_ie_c* ie )
    {
    DEBUG( "core_operation_handle_neighbor_response_c::parse() - * Neighbor Report Response IE received *" );
    
    if ( neighbor_resp_parser_m != NULL )
        {
        delete neighbor_resp_parser_m;
        neighbor_resp_parser_m = NULL;
        }
    
    neighbor_resp_parser_m = core_frame_nr_ie_c::instance( *ie );
    
    if ( neighbor_resp_parser_m )
        {
        core_mac_address_s a = neighbor_resp_parser_m->bssid();
        DEBUG6( "core_operation_handle_neighbor_response_c::parse() - BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
                a.addr[0],a.addr[1],a.addr[2],a.addr[3],a.addr[4],a.addr[5] );
        DEBUG1( "core_operation_handle_neighbor_response_c::parse() - BSSID Information: 0x%08X",
                neighbor_resp_parser_m->bssid_info() );
        DEBUG1( "core_operation_handle_neighbor_response_c::parse() - Regulatory Class: 0x%02X",
                neighbor_resp_parser_m->regulatory_class() );
        DEBUG1( "core_operation_handle_neighbor_response_c::parse() - Channel Number: 0x%02X",
                neighbor_resp_parser_m->channel_number() );
        DEBUG1( "core_operation_handle_neighbor_response_c::parse() - PHY Type: 0x%02X",
                neighbor_resp_parser_m->phy_type() );

        core_scan_channels_c channels;

        if ( server_m->get_core_settings().is_valid_channel(
            SCAN_BAND_2DOT4GHZ,
            neighbor_resp_parser_m->channel_number() ) )
            {
            channels.add(
                SCAN_BAND_2DOT4GHZ,
                neighbor_resp_parser_m->channel_number() );
            }
        else
            {
            DEBUG( "core_operation_handle_neighbor_response_c::parse() - channel doesn't match the current regional domain, ignoring." );
            }
        
        DEBUG1( "core_operation_handle_neighbor_response_c::parse() - adding the current channel %u",
            server_m->get_connection_data()->current_ap_data()->channel() );
        channels.add(
            SCAN_BAND_2DOT4GHZ,
            server_m->get_connection_data()->current_ap_data()->channel() );
        server_m->get_connection_data()->merge_adjacent_ap_channels(
            channels );            
        }
    }
