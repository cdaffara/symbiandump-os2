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
* Description:  Statemachine for handling received measurement request.
*
*/


#include "core_operation_handle_measurement_request.h"
#include "core_frame_dot11_ie.h"
#include "core_frame_mgmt_ie.h"
#include "core_frame_rm_ie.h"
#include "core_frame_rm_ie_beacon_report.h"
#include "core_frame_rm_ie_beacon_report_ie.h"
#include "core_frame_rm_ie_beacon_report_frame_body_ie.h"
#include "core_frame_rm_ie_beacon_request.h"
#include "core_frame_rm_ie_beacon_request_ie.h"
#include "core_frame_rm_ie_beacon_request_detail_ie.h"
#include "core_frame_action_rm.h"
#include "core_tools.h"
#include "core_server.h"
#include "am_debug.h"
#include "core_am_tools.h"

const u16_t PARSED_NONE                 = 0;
const u16_t MEASUREMENT_REQUEST_PARSED  = 1;
const u16_t BEACON_REQUEST_PARSED       = 2;
const u16_t SSID_IE_PARSED              = 4;
const u16_t BRI_IE_PARSED               = 8;
const u16_t BRD_IE_PARSED               = 16;
const u16_t AP_CH_REP_IE_PARSED         = 32;
const u16_t REQUEST_IE_PARSED           = 64;
const u16_t SSID_IE_PARSING_FAILED      = 128;
const u16_t BRI_IE_PARSING_FAILED       = 256;
const u16_t BRD_IE_PARSING_FAILED       = 512;
const u16_t AP_CH_REP_IE_PARSING_FAILED = 1024;
const u16_t REQUEST_IE_PARSING_FAILED   = 2048;
const u16_t CATEGORY_ERROR_IND          = 128;

const u8_t BEACON_TABLE_REPORTED_FRAME_INFORMATION      = 0xFF;
const u8_t ANTENNA_ID                                   = 0x00;
const u8_t BEACON_TABLE_REGULATORY_CLASS                = 0xFF;
const u16_t BEACON_TABLE_MEASUREMENT_DURATION           = 0x0000;
const u8_t MEASUREMENT_REPORT_MODE_OK                   = 0x00;
const u8_t MEASUREMENT_REPORT_MODE_INCAPABLE            = 0x02;
const u8_t MEASUREMENT_REPORT_MODE_REFUSED              = 0x04;
const u8_t DURATION_MANDATORY                           = 0x10;

const u8_t SCAN_ALL_CHANNELS_IN_REGULATORY_CLASS        = 0x00;
const u8_t SCAN_ALL_CHANNELS_IN_AP_CH_REPORT            = 0xFF;

const u8_t REQUEST_IE_MAX_LENGTH = 237;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_handle_measurement_request_c::core_operation_handle_measurement_request_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_frame_action_rm_c* measurement_request ) :
    core_operation_base_c( core_operation_handle_measurement_request, request_id, server, drivers, adaptation, 
        core_base_flag_none ),
    measurement_request_m( measurement_request ),
    parsed_mask_m( PARSED_NONE ),
    current_ie_m( NULL ),
    report_m ( NULL ),
    meas_req_parser_m ( NULL ),
    beacon_req_parser_m ( NULL ),
    ssid_parser_m ( NULL ),
    bri_parser_m ( NULL ),
    brd_parser_m ( NULL ),
    ap_ch_rep_parser_m ( NULL ),
    request_ie_parser_m ( NULL ),
    actual_measurement_duration_m ( 0 )
    {
    DEBUG( "core_operation_handle_measurement_request_c::core_operation_handle_measurement_request_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_handle_measurement_request_c::~core_operation_handle_measurement_request_c()
    {
    DEBUG( "core_operation_handle_measurement_request_c::~core_operation_handle_measurement_request_c()" );

    server_m->unregister_event_handler( this );
    server_m->unregister_frame_handler( this );
    
    ie_list_m.clear();
        
    if ( measurement_request_m )
        {
        delete measurement_request_m;
        measurement_request_m = NULL;
        }

    if ( current_ie_m )
        {
        delete current_ie_m;
        current_ie_m = NULL;
        }
    
    if ( beacon_req_parser_m )
        {
        delete beacon_req_parser_m;
        beacon_req_parser_m = NULL;
        }
    
    if ( meas_req_parser_m )
        {
        delete meas_req_parser_m;
        meas_req_parser_m = NULL;
        }
    
    if ( ssid_parser_m )
        {
        delete ssid_parser_m;
        ssid_parser_m = NULL;
        }
    
    if ( bri_parser_m )
        {
        delete bri_parser_m;
        bri_parser_m = NULL;
        }
    
    if ( brd_parser_m )
        {
        delete brd_parser_m;
        brd_parser_m = NULL;
        }
    
    if ( ap_ch_rep_parser_m )
        {
        delete ap_ch_rep_parser_m;
        ap_ch_rep_parser_m = NULL;
        }

    if ( request_ie_parser_m )
        {
        delete request_ie_parser_m;
        request_ie_parser_m = NULL;
        }

    if ( report_m )
        {
        delete report_m;
        report_m = NULL;
        }

    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_handle_measurement_request_c::next_state()
    {
    DEBUG( "core_operation_handle_measurement_request_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            DEBUG( "core_operation_handle_measurement_request_c::next_state() - core_state_init" );
            
            if ( !server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_handle_measurement_request_c::next_state() - not connected, nothing to do" );

                return core_error_general;
                }

            status_m.set_da( measurement_request_m->source() );
            status_m.set_sa( server_m->own_mac_addr() );

            DEBUG( "core_operation_handle_measurement_request_c::next_state() - Request data layout" );
            DEBUG_BUFFER( measurement_request_m->data_length(), measurement_request_m->data() );
                        
            DEBUG1( "core_operation_handle_measurement_request_c::next_state() - Dialog token: 0x%02X",
                    measurement_request_m->dialog_token() );
            DEBUG1( "core_operation_handle_measurement_request_c::next_state() - Number of repetitions: 0x%04X",
                    measurement_request_m->nbr_of_repetitions() );
            
            // validate measurement interval
            if ( !validate_measurement_interval() )
                {
                DEBUG( "core_operation_handle_measurement_request_c::next_state() - Measurement Request coming too often, refusing request" );
                status_m.set_too_short_interval( true_t );
                }
            
            // loop through IEs
            for ( core_frame_dot11_ie_c* ie = measurement_request_m->first_ie(); ie; ie = measurement_request_m->next_ie() )
                {
                DEBUG( "core_operation_handle_measurement_request_c::next_state() - Found IE:" );
                DEBUG1( "core_operation_handle_measurement_request_c::next_state() - ID: 0x%02X",
                    ie->element_id() );
                DEBUG1( "core_operation_handle_measurement_request_c::next_state() - Length: 0x%02X",
                    ie->length() );
                
                ie_list_m.append( ie );
                }
            
            if ( ie_list_m.count() == 0 )
                {
                DEBUG( "core_operation_handle_measurement_request_c::next_state() - Measurement request didn't contain any IEs -> sending frame back" );
                 
                status_m.set_error_in_request( true_t );
                                
                return goto_state( core_state_create_negative_response );
                }
            
            return goto_state( core_state_get_next_ie );
            }
        case core_state_get_next_ie:
            {
            DEBUG( "core_operation_handle_measurement_request_c::next_state() - state core_state_get_next_ie" );

            initialise_beacon_request_data();

            if ( current_ie_m != NULL )
                {
                // deallocate IE
                delete current_ie_m;
                current_ie_m = NULL;
                }

            current_ie_m = ie_list_m.first();
            
            if ( current_ie_m == NULL )
                {
                DEBUG( "core_operation_handle_measurement_request_c::next_state() - All IEs processed" );
                
                if ( report_m != NULL )
                    {
                    // send created response
                    return goto_state( core_state_send_response );
                    }
                else
                    {
                    return goto_state( core_state_operation_finished );
                    }
                }
            
            // remove ie from list, it is now owned by current_ie_m
            ie_list_m.remove ( current_ie_m );
            
            return asynch_goto( core_state_process_current_ie );
            }
        case core_state_process_current_ie:
            {
            if ( current_ie_m->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_measurement_req )
                {
                // parse IE
                parse( current_ie_m, parsed_mask_m );
                
                // deallocate IE
                delete current_ie_m;
                current_ie_m = NULL;

                if ( status_m.error_in_request() == true_t )
                    {
                    DEBUG( "core_operation_handle_measurement_request_c::next_state() - Error in request, send frame back and stop further processing." );
                    return goto_state( core_state_create_negative_response );
                    }
                
                if ( status_m.refuse_request() == true_t )
                    {
                    DEBUG( "core_operation_handle_measurement_request_c::next_state() - Refusing request." );                  
                    return goto_state( core_state_create_negative_response );
                    }
                
                if ( status_m.too_short_interval() == true_t )
                    {
                    DEBUG( "core_operation_handle_measurement_request_c::next_state() - Too short request interval, refusing request." );
                    status_m.set_refuse_request( true_t );
                    return goto_state( core_state_create_negative_response );
                    }
                
                DEBUG( "core_operation_handle_measurement_request_c::next_state() - Going to core_state_make_measurement." );
                
                return asynch_goto( core_state_make_measurement, get_random_delay() );
                }

            DEBUG( "core_operation_handle_measurement_request_c::next_state() - Non-supported measurement request IE received, ignoring." );
            DEBUG( "core_operation_handle_measurement_request_c::next_state() - Going to core_state_get_next_ie." );
            
            return goto_state( core_state_get_next_ie );
            }
        case core_state_make_measurement:
            {
            ASSERT( beacon_req_parser_m );
            
            core_scan_mode_e scan_mode( core_scan_mode_active );
            actual_measurement_duration_m = get_channel_time( core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_active );
            core_ssid_s scan_ssid = get_ssid();

            if ( ap_ch_rep_parser_m )
                {
                core_scan_channels_c scan_channels = get_channels();
                status_m.set_scan_channels( scan_channels.channels() );
                }

            switch ( beacon_req_parser_m->measurement_mode() )
                {
                case core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_beacon:
                    {                    
                    DEBUG( "core_operation_handle_measurement_request_c::next_state() - beacon table measurement" );
                    actual_measurement_duration_m = get_channel_time( core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_beacon );
                    return asynch_goto( core_state_create_response );
                    }
                case core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_active:
                    {
                    DEBUG( "core_operation_handle_measurement_request_c::next_state() - issuing active scan request" );
                    break;
                    }
                case core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_passive:
                    {
                    DEBUG( "core_operation_handle_measurement_request_c::next_state() - issuing passive scan request" );
                    scan_mode = core_scan_mode_passive;
                    actual_measurement_duration_m = get_channel_time( core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_passive );
                    break;
                    }
                default:
                    {
                    // by default perform active mode measurement
                    break;
                    }
                }

            u32_t ch_time( actual_measurement_duration_m );
            
            DEBUG( "core_operation_handle_measurement_request_c::next_state() - issuing scan request:" );
            DEBUG1S( "core_operation_handle_measurement_request_c::next_state() - SSID: ", scan_ssid.length, &scan_ssid.ssid[0] );
#ifdef _DEBUG
            u16_t mask( 0 );
            core_tools_c::copy(
                reinterpret_cast<u8_t*>( &mask ),
                &status_m.get_scan_channels().channels2dot4ghz[0],            
                sizeof( status_m.get_scan_channels().channels2dot4ghz ) );
#endif // _DEBUG
            DEBUG1( "core_operation_handle_measurement_request_c::next_state() - channel mask: %013b", mask );
            DEBUG1( "core_operation_handle_measurement_request_c::next_state() - min ch time: %u", ch_time );
            DEBUG1( "core_operation_handle_measurement_request_c::next_state() - max ch time: %u", ch_time );
            
            server_m->get_scan_list().remove_entries_by_age(
                server_m->get_device_settings().scan_list_expiration_time );

            server_m->get_scan_list().set_tag(
                core_scan_list_tag_scan );

            server_m->register_event_handler( this );
            server_m->register_frame_handler( this );

            drivers_m->scan(
                request_id_m,
                scan_mode,
                scan_ssid,
                server_m->get_device_settings().scan_rate,
                status_m.get_scan_channels(),
                ch_time,
                ch_time,
                false_t );
            
            operation_state_m = core_state_scan_start;

            return core_error_request_pending;
            }
        case core_state_scan_start:
            {
            DEBUG( "core_operation_handle_measurement_request_c::next_state() - scan request completed, waiting for scan completion" );

            break;
            }
        case core_state_create_response:
            {
            core_scan_list_tag_e tag( core_scan_list_tag_scan );
            if ( beacon_req_parser_m->measurement_mode() == core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_beacon )
                {
                tag = core_scan_list_tag_none;
                }
#ifdef _DEBUG
            server_m->get_scan_list().print_contents();      
#endif
            core_scan_list_iterator_by_tag_c iter(
                server_m->get_scan_list(),
                tag );
            
            bool_t no_match( true_t );
            
            // loop through the scan list
            for ( core_ap_data_c* ap_data = iter.first(); ap_data; ap_data = iter.next() )
                {
                if ( !match_found( *ap_data ) )
                    {
                    // no match found, proceed to the next beacon/probe
                    continue;
                    }
                
                no_match = false_t;
                
                if ( !create_response( *ap_data ) )
                    {
                    // create_response returns false_t only in out-of-memory situation
                    return core_error_no_memory;
                    }
                }
            
            if ( no_match )
                {
                create_negative_response_based_on_meas_report_mode( MEASUREMENT_REPORT_MODE_OK );
                }
            
            // check if more IEs need to be added to the report
            DEBUG( "core_operation_handle_measurement_request_c::next_state() - Going to core_state_get_next_ie." );
            
            return goto_state( core_state_get_next_ie );
            }
        case core_state_create_negative_response:
            {
            if ( !create_negative_response() )
                {
                DEBUG( "core_operation_handle_measurement_request_c::next_state() - Unable to create response to request" );
                
                // create_response returns false only in out-of-memory situation
                return core_error_no_memory;
                }
            
            return goto_state( core_state_send_response );
            }
        case core_state_send_response:
            {
            DEBUG1( "core_operation_handle_measurement_request_c::next_state() - sending frame (%u bytes): ",
                report_m->data_length() );
            DEBUG_BUFFER(
                report_m->data_length(),
                report_m->data() );
            
            server_m->send_management_frame(
                core_frame_type_dot11,
                report_m->data_length(),
                report_m->data(),
                status_m.da() );
            
            return goto_state( core_state_operation_finished );
            }
        case core_state_operation_finished:
            {
            DEBUG( "core_operation_handle_measurement_request_c::next_state() - state core_state_operation_finished" );
            
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
bool_t core_operation_handle_measurement_request_c::add_to_report(
        const core_frame_rm_ie_c* ie ) 
    {
    DEBUG( "core_operation_handle_measurement_request_c::add_to_report()" );
    
    if ( report_m == NULL )
        {
        // instantiate response
        report_m = core_frame_action_rm_c::instance(
                                            0,                          // Duration
                                            status_m.da(),              // Destination
                                            status_m.sa(),              // Source
                                            status_m.da(),              // BSSID
                                            0,                          // Sequence Control
                                            core_frame_action_rm_c::core_dot11_action_rm_type_meas_resp, // Action Type
                                            measurement_request_m->dialog_token(),     // Dialog Token
                                            ie );                       // RM IE
        
        if ( report_m == NULL )
            {
            // unable to instantiate response
            return false_t;
            }
        }
    else
        {
        report_m->append_ie( ie );
        }
    
    return true_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_handle_measurement_request_c::parse(
        core_frame_dot11_ie_c* ie,
        u16_t& parsed_mask )
    {
    DEBUG( "core_operation_handle_measurement_request_c::parse() - * Measurement Request IE received *" );
    
    // refuse repetitive measurements
    if ( measurement_request_m->nbr_of_repetitions() )
        {
        DEBUG( "core_operation_handle_measurement_request_c::parse() - refusing repeated measurement" );
        status_m.set_refuse_request( true_t );
        }
    
    if ( meas_req_parser_m != NULL )
        {
        delete meas_req_parser_m;
        meas_req_parser_m = NULL;
        }
    
    meas_req_parser_m = core_frame_rm_ie_c::instance( *ie );
    
    if ( meas_req_parser_m )
        {
        parsed_mask |= MEASUREMENT_REQUEST_PARSED;

        validate_measurement_request();
        
        if ( meas_req_parser_m->measurement_type() == core_frame_rm_ie_c::core_frame_rm_ie_action_type_beacon_request )
            {
            // instantiate a parser for beacon request
            if ( beacon_req_parser_m != NULL )
                {
                delete beacon_req_parser_m;
                beacon_req_parser_m = NULL;
                }
            beacon_req_parser_m = core_frame_rm_ie_beacon_request_c::instance( *meas_req_parser_m );
            
            validate_beacon_request();
            
            if ( beacon_req_parser_m )
                {
                parsed_mask |= BEACON_REQUEST_PARSED;

                // loop through IEs
                for ( core_frame_dot11_ie_c* beacon_req_ie = beacon_req_parser_m->first_ie(); beacon_req_ie; beacon_req_ie = beacon_req_parser_m->next_ie() )
                    {
                    DEBUG1( "core_operation_handle_measurement_request_c::parse() - 802.11k Beacon Request IE ID: 0x%02X",
                        beacon_req_ie->element_id() );
                    DEBUG1( "core_operation_handle_measurement_request_c::parse() - 802.11k Beacon Request IE length: 0x%02X",
                        beacon_req_ie->length() );
                    
                    if ( beacon_req_ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_ssid )
                        {
                        // instantiate a parser for ssid
                        if ( ssid_parser_m != NULL )
                            {
                            delete ssid_parser_m;
                            ssid_parser_m = NULL;
                            }
                        DEBUG( "core_operation_handle_measurement_request_c::parse() - *** SSID Sub-element ***" );
                        
                        ssid_parser_m = core_frame_mgmt_ie_ssid_c::instance( *beacon_req_ie );
                        if ( ssid_parser_m )
                            {
                            parsed_mask |= SSID_IE_PARSED;
                            const core_ssid_s ssid = ssid_parser_m->ssid();
                            if ( ssid.length != 0 )
                                {
                                DEBUG1S( "core_operation_handle_measurement_request_c::parse() - SSID: ", ssid.length, &ssid.ssid[0] );
                                }
                            else
                                {
                                DEBUG( "core_operation_handle_measurement_request_c::parse() - SSID: <zero length ssid>" );
                                }
                            }
                        else
                            {
                            parsed_mask |= SSID_IE_PARSING_FAILED;
                            status_m.set_error_in_request( true_t );
                            }
                        }
                    else if ( beacon_req_ie->element_id() == core_frame_rm_ie_beacon_request_c::core_frame_rm_ie_beacon_request_element_id_beacon_reporting_information )
                        {
                        DEBUG( "core_operation_handle_measurement_request_c::parse() - *** Beacon Reporting Information Sub-element ***" );
                        // instantiate a parser for BRI
                        if ( bri_parser_m != NULL )
                            {
                            delete bri_parser_m;
                            bri_parser_m = NULL;
                            }
                        bri_parser_m = core_frame_rm_ie_beacon_request_ie_c::instance( *beacon_req_ie );
                        if ( bri_parser_m )
                            {
                            validate_beacon_reporting_information();
                            parsed_mask |= BRI_IE_PARSED;
                            }
                        else
                            {
                            parsed_mask |= BRI_IE_PARSING_FAILED;
                            status_m.set_error_in_request( true_t );
                            }
                        }
                    else if ( beacon_req_ie->element_id() == core_frame_rm_ie_beacon_request_c::core_frame_rm_ie_beacon_request_element_id_beacon_reporting_detail )
                        {
                        DEBUG( "core_operation_handle_measurement_request_c::parse() - *** Beacon Reporting Detail Sub-element ***" );
                        // instantiate a parser for BRD
                        if ( brd_parser_m != NULL )
                            {
                            delete brd_parser_m;
                            brd_parser_m = NULL;
                            }
                        brd_parser_m = core_frame_rm_ie_beacon_request_detail_ie_c::instance( *beacon_req_ie );
                        if ( brd_parser_m )
                            {
                            validate_beacon_reporting_detail();
                            parsed_mask |= BRD_IE_PARSED;
                            }
                        else
                            {
                            parsed_mask |= BRD_IE_PARSING_FAILED;
                            status_m.set_error_in_request( true_t );
                            }
                        }
                    else if ( beacon_req_ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_request )
                        {
                        // instantiate a parser for request information element
                        if ( request_ie_parser_m != NULL )
                            {
                            delete request_ie_parser_m;
                            request_ie_parser_m = NULL;
                            }
                        DEBUG( "core_operation_handle_measurement_request_c::parse() - *** Request Sub-element ***" );
                        
                        request_ie_parser_m = core_frame_mgmt_ie_request_ie_c::instance( *beacon_req_ie );
                        if ( request_ie_parser_m )
                            {
                            parsed_mask |= REQUEST_IE_PARSED;
                            }
                        else
                            {
                            parsed_mask |= REQUEST_IE_PARSING_FAILED;
                            status_m.set_error_in_request( true_t );
                            }
                        }
                    else if ( beacon_req_ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_ap_channel_report )
                        {
                        // instantiate a parser for ap channel report
                        if ( ap_ch_rep_parser_m != NULL )
                            {
                            delete ap_ch_rep_parser_m;
                            ap_ch_rep_parser_m = NULL;
                            }
                        DEBUG( "core_operation_handle_measurement_request_c::parse() - *** AP Channel Report Sub-element ***" );
                        
                        ap_ch_rep_parser_m = core_frame_mgmt_ie_ap_channel_report_c::instance( *beacon_req_ie );
                        if ( ap_ch_rep_parser_m )
                            {
                            parsed_mask |= AP_CH_REP_IE_PARSED;
                            }
                        else
                            {
                            parsed_mask |= AP_CH_REP_IE_PARSING_FAILED;
                            status_m.set_error_in_request( true_t );
                            }
                        }

                    delete beacon_req_ie;
                    beacon_req_ie = NULL;
                    }
                }
            }
        else
            {
            // Incapable measurement request
            create_negative_response_based_on_meas_report_mode( MEASUREMENT_REPORT_MODE_INCAPABLE );
            }
        }
    else
        {
        status_m.set_error_in_request( true_t );
        }
    
    DEBUG( "core_operation_handle_measurement_request_c::parse() - parsed_mask:" );
    DEBUG1( "core_operation_handle_measurement_request_c::parse() - %012b", parsed_mask );
    DEBUG( "core_operation_handle_measurement_request_c::parse() - ||||||||||||" );
    DEBUG( "core_operation_handle_measurement_request_c::parse() - |||||||||||+--MEASUREMENT_REQUEST_PARSED");
    DEBUG( "core_operation_handle_measurement_request_c::parse() - ||||||||||+---BEACON_REQUEST_PARSED");
    DEBUG( "core_operation_handle_measurement_request_c::parse() - |||||||||+----SSID_IE_PARSED");
    DEBUG( "core_operation_handle_measurement_request_c::parse() - ||||||||+-----BRI_IE_PARSED");
    DEBUG( "core_operation_handle_measurement_request_c::parse() - |||||||+------BRD_IE_PARSED");
    DEBUG( "core_operation_handle_measurement_request_c::parse() - ||||||+-------AP_CH_REP_IE_PARSED");
    DEBUG( "core_operation_handle_measurement_request_c::parse() - |||||+--------REQUEST_IE_PARSED");
    DEBUG( "core_operation_handle_measurement_request_c::parse() - ||||+---------SSID_IE_PARSING_FAILED");
    DEBUG( "core_operation_handle_measurement_request_c::parse() - |||+----------BRI_IE_PARSING_FAILED");
    DEBUG( "core_operation_handle_measurement_request_c::parse() - ||+-----------BRD_IE_PARSING_FAILED");
    DEBUG( "core_operation_handle_measurement_request_c::parse() - |+------------AP_CH_REP_IE_PARSING_FAILED");
    DEBUG( "core_operation_handle_measurement_request_c::parse() - +-------------REQUEST_IE_PARSING_FAILED");
            
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_operation_handle_measurement_request_c::create_negative_response()
    {
    DEBUG( "core_operation_handle_measurement_request_c::create_negative_response()" );
    
    bool_t result( false_t );
    
    if ( status_m.error_in_request() )
        {        
        DEBUG( "core_operation_handle_measurement_request_c::create_negative_response() - Error in request, send it back and stop further processing." );
        
        result = create_response_to_invalid_request();
        }
    else if ( status_m.refuse_request() )
        {
        DEBUG( "core_operation_handle_measurement_request_c::create_negative_response() - Refusing to make the requested measurement" );
        
        result = create_negative_response_based_on_meas_report_mode( MEASUREMENT_REPORT_MODE_REFUSED );
        }
    else
        {
        ASSERT( 0 );
        }
    
    return result;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_operation_handle_measurement_request_c::create_negative_response_based_on_meas_report_mode(
    u8_t measurement_report_mode )
    {
    DEBUG( "core_operation_handle_measurement_request_c::create_negative_response_based_on_meas_report_mode()" );

    bool_t result( false_t );
        
    // create IE with no sub-elements
    core_frame_rm_ie_c* rm_report = core_frame_rm_ie_c::instance(
                                                                meas_req_parser_m->measurement_token(),
                                                                measurement_report_mode,
                                                                meas_req_parser_m->measurement_type() );
    if ( rm_report )
        {
        result = add_to_report( rm_report );
        
        delete rm_report;
        rm_report = NULL;
        
        return result;
        }

    if ( measurement_report_mode == MEASUREMENT_REPORT_MODE_REFUSED )
        {
        DEBUG( "core_operation_handle_measurement_request_c::create_negative_response_based_on_meas_report_mode() - Unable to instantiate core_frame_rm_ie_parameter_c" );
        }
    else if ( measurement_report_mode == MEASUREMENT_REPORT_MODE_OK )
        {
        DEBUG( "core_operation_handle_measurement_request_c::create_negative_response_based_on_meas_report_mode() - Unable to instantiate core_frame_rm_ie_parameter_c" );
        }
    else if ( measurement_report_mode == MEASUREMENT_REPORT_MODE_INCAPABLE )
        {
        DEBUG( "core_operation_handle_measurement_request_c::create_negative_response_based_on_meas_report_mode() - Incapable measurement request, unable to instantiate core_frame_rm_ie_parameter_c" );
        }

    return result;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_operation_handle_measurement_request_c::create_response_to_invalid_request()
    {
    DEBUG( "core_operation_handle_measurement_request_c::create_response_to_invalid_request()" );

    ASSERT( !report_m );
    
    // send the received frame back
    report_m = core_frame_action_c::instance( *measurement_request_m );
        
    if ( report_m )
        {
        // set the MSB of the Category field to 1 to indicate error situation
        // Ref. 7.3.1.11 Action field
        report_m->set_category( report_m->category() + CATEGORY_ERROR_IND );

        // switch DA and SA
        report_m->set_source( status_m.da() );
        report_m->set_destination( status_m.sa() );
        
        return true_t;
        }
   
    return false_t;      
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_handle_measurement_request_c::validate_beacon_request() 
    {
    if ( !beacon_req_parser_m )
        {
        DEBUG( "core_operation_handle_measurement_request_c::validate_beacon_request() - no beacon request element present, refusing measurement" );
        status_m.set_refuse_request( true_t );
        return;
        }
        
    if ( beacon_req_parser_m->measurement_mode() > core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_beacon )
        {
        DEBUG1( "core_operation_handle_measurement_request_c::validate_beacon_request() - unknown Measurement Mode %u",
                beacon_req_parser_m->measurement_mode() );
        status_m.set_error_in_request( true_t );
        }
    
    if ( beacon_req_parser_m->measurement_mode() != core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_beacon &&
         ( meas_req_parser_m->measurement_request_mode() & DURATION_MANDATORY ) &&
         ( ( beacon_req_parser_m->measurement_mode() == core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_active &&
             beacon_req_parser_m->measurement_duration() > server_m->get_device_settings().active_scan_max_ch_time ) ||
           ( beacon_req_parser_m->measurement_mode() == core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_passive &&
             beacon_req_parser_m->measurement_duration() > server_m->get_device_settings().passive_scan_max_ch_time ) ) )
        {
        DEBUG( "core_operation_handle_measurement_request_c::validate_beacon_request() - too big measurement duration requested, refusing request" );
        status_m.set_refuse_request( true_t );
        }
    
    if ( beacon_req_parser_m->measurement_mode() != core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_beacon &&
         server_m->get_connection_data()->voice_call_state() == true_t )
        {
        DEBUG( "core_operation_handle_measurement_request_c::validate_beacon_request() - non-Beacon Table Mode requested and voice call on, refusing request" );
        status_m.set_refuse_request( true_t );
        }
    
    if ( !validate_scan_channels() )
        {
        DEBUG( "core_operation_handle_measurement_request_c::validate_beacon_request() - invalid scan channels, refusing request" );
        status_m.set_refuse_request( true_t );
        }
        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_handle_measurement_request_c::validate_beacon_reporting_information() 
    {
    if ( bri_parser_m->reporting_condition() != core_frame_rm_ie_beacon_request_ie_c::core_frame_rm_ie_bri_reporting_condition_default )
        {
        DEBUG1( "core_operation_handle_measurement_request_c::validate_beacon_reporting_information() - reporting condition 0x%02X not supported",
                bri_parser_m->reporting_condition() );
        status_m.set_refuse_request( true_t );
        }    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_handle_measurement_request_c::validate_beacon_reporting_detail() 
    {
    if ( brd_parser_m->reporting_detail() != core_frame_rm_ie_beacon_request_detail_ie_c::core_frame_rm_ie_brd_reporting_detail_no_fields_or_elements &&
         brd_parser_m->reporting_detail() != core_frame_rm_ie_beacon_request_detail_ie_c::core_frame_rm_ie_brd_reporting_detail_fields_and_requested_elements &&
         brd_parser_m->reporting_detail() != core_frame_rm_ie_beacon_request_detail_ie_c::core_frame_rm_ie_brd_reporting_detail_default )
        {
        DEBUG1( "core_operation_handle_measurement_request_c::validate_beacon_reporting_detail() - reporting detail 0x%02X not supported",
                brd_parser_m->reporting_detail() );
        status_m.set_refuse_request( true_t );
        }    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_operation_handle_measurement_request_c::validate_measurement_interval() 
    {
    u64_t previous_timestamp = server_m->get_connection_data()->get_previous_rrm_measurement_request_time();
    u64_t current_timestamp = core_am_tools_c::timestamp();
    
    if ( previous_timestamp )
        {
        u32_t min_interval = server_m->get_device_settings().rrm_min_measurement_interval;
        
        DEBUG1( "core_operation_handle_measurement_request_c::validate_measurement_interval() - min. interval in seconds: %u", 
                server_m->get_device_settings().rrm_min_measurement_interval / SECONDS_FROM_MICROSECONDS );
        
        DEBUG1( "core_operation_handle_measurement_request_c::validate_measurement_interval() - seconds since previous request: %u", 
                static_cast<u32_t> ( ( current_timestamp - previous_timestamp ) / SECONDS_FROM_MICROSECONDS ) );
        
        if ( min_interval && ( previous_timestamp + min_interval >= current_timestamp ) )
            {
            // interval is too short
            return false_t;
            }
        }
        
    server_m->get_connection_data()->set_previous_rrm_measurement_request_time( current_timestamp );
    
    return true_t;   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_handle_measurement_request_c::validate_measurement_request() 
    {
    if ( meas_req_parser_m->measurement_type() != core_frame_rm_ie_c::core_frame_rm_ie_action_type_beacon_request )
        {
        DEBUG1( "core_operation_handle_measurement_request_c::validate_measurement_request() - non-supported Measurement Type 0x%02X",
                meas_req_parser_m->measurement_type() );
        status_m.set_refuse_request( true_t );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
bool_t core_operation_handle_measurement_request_c::receive_frame(
    const core_frame_dot11_c* frame,
    u8_t rcpi )
    {   
    if ( frame->type() != core_frame_dot11_c::core_dot11_type_beacon &&
         frame->type() != core_frame_dot11_c::core_dot11_type_probe_resp )
        {
        DEBUG( "core_operation_handle_measurement_request_c::receive_frame() - not a beacon or a probe" );        
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
        DEBUG1S( "core_operation_handle_measurement_request_c::receive_frame() - SSID: ",
            ssid.length, &ssid.ssid[0] );

        core_mac_address_s bssid(
            ap_data->bssid() );
        DEBUG6( "core_operation_handle_measurement_request_c::receive_frame() - BSSID: %02X:%02X:%02X:%02X:%02X:%02X",
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
bool_t core_operation_handle_measurement_request_c::notify(
    core_am_indication_e indication )
    {
    if ( operation_state_m == core_state_scan_start &&
         indication == core_am_indication_wlan_scan_complete )
        {
        server_m->unregister_event_handler( this );
        server_m->unregister_frame_handler( this );

        DEBUG( "core_operation_handle_measurement_request_c::notify() - scan complete" );

        asynch_goto( core_state_create_response, CORE_TIMER_IMMEDIATELY );

        return true_t;
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_operation_handle_measurement_request_c::validate_scan_channels()
    {
    core_scan_channels_c scan_channels;
    
    switch ( beacon_req_parser_m->channel_number() )
        {
        case SCAN_ALL_CHANNELS_IN_REGULATORY_CLASS:
            {
            DEBUG( "core_operation_handle_measurement_request_c::validate_scan_channels() - scan all channels in Regulatory Class" );
            scan_channels.set( SCAN_CHANNELS_2DOT4GHZ_ETSI );
            break;
            }
        case SCAN_ALL_CHANNELS_IN_AP_CH_REPORT:
            {
            DEBUG( "core_operation_handle_measurement_request_c::validate_scan_channels() - scan all channels from the AP Channel report" );
            scan_channels.set( server_m->get_connection_data()->current_ap_data()->ap_channel_report() );
            
            if ( scan_channels.is_empty( SCAN_BAND_2DOT4GHZ ) )
                {
                DEBUG( "core_operation_handle_measurement_request_c::validate_scan_channels() - No AP Channel report available, using full mask" );
                scan_channels.set( SCAN_CHANNELS_2DOT4GHZ_ETSI );
                }
            break;
            }
        default:
            {
            DEBUG1( "core_operation_handle_measurement_request_c::validate_scan_channels() - scan channel %u",
                    beacon_req_parser_m->channel_number() );
            scan_channels.add( SCAN_BAND_2DOT4GHZ, beacon_req_parser_m->channel_number() );
            break;
            }
        }

    scan_channels.set( server_m->get_core_settings().valid_scan_channels( scan_channels.channels() ) );
    
    if ( scan_channels.is_empty( SCAN_BAND_2DOT4GHZ ) )
        {
        DEBUG( "core_operation_handle_measurement_request_c::validate_scan_channels() - no valid channels after check" );
        return false_t;
        }
    
    status_m.set_scan_channels( scan_channels.channels() );

    return true_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u16_t core_operation_handle_measurement_request_c::get_channel_time(
                                core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_e measurement_mode )
    {
    u16_t ch_time( static_cast<u16_t> ( server_m->get_device_settings().active_scan_max_ch_time ) );
    u16_t min_ch_time( static_cast<u16_t> ( server_m->get_device_settings().active_scan_min_ch_time ) );
    
    switch( measurement_mode )
        {
        case core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_passive:
            {
            ch_time = static_cast<u16_t> ( server_m->get_device_settings().passive_scan_max_ch_time );
            min_ch_time = static_cast<u16_t> ( server_m->get_device_settings().passive_scan_min_ch_time );
            break;
            }
        case core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_beacon:
            {
            ch_time = BEACON_TABLE_MEASUREMENT_DURATION;
            min_ch_time = BEACON_TABLE_MEASUREMENT_DURATION;
            break;
            }
        case core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_active: // flow-through on purpose
        default:
            {
            // active mode set above by default
            }
        }

    if ( beacon_req_parser_m )
        {
        if ( beacon_req_parser_m->measurement_duration() > min_ch_time &&
             beacon_req_parser_m->measurement_duration() < ch_time )
            {
            ch_time = beacon_req_parser_m->measurement_duration();
            }
        }
    
    return ch_time;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_ssid_s core_operation_handle_measurement_request_c::get_ssid()
    {
    core_ssid_s ssid = BROADCAST_SSID;
    if ( ssid_parser_m )
        {
        ssid = ssid_parser_m->ssid();
        }
    return ssid;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_channels_c core_operation_handle_measurement_request_c::get_channels()
    {
    core_scan_channels_c channels;
    u8_t channel( 0 );

    if ( ap_ch_rep_parser_m )
        {
        for( u16_t i=0; i < (ap_ch_rep_parser_m->data_length() - CORE_FRAME_MGMT_IE_AP_CHANNEL_REPORT_CHANNEL_OFFSET); i++ )
            {
            channel = ap_ch_rep_parser_m->ap_channel_report_channel( i );
            channels.add( SCAN_BAND_2DOT4GHZ, channel );
            }
        }
    return channels;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_operation_handle_measurement_request_c::match_found( core_ap_data_c& ap_data )
    {
    bool_t ssid_match_found( false_t );
    bool_t bssid_match_found( false_t );

    if ( ssid_parser_m )
        {
        const core_ssid_s ap_ssid = ap_data.ssid();
        const core_ssid_s request_ssid = ssid_parser_m->ssid();
        
        if ( request_ssid == ap_ssid )
            {
            DEBUG( "core_operation_handle_measurement_request_c::match_found() - ssid matches" );
            ssid_match_found = true_t;
            }
        }
    else
        {
        ssid_match_found = true_t;
        }

    ASSERT( beacon_req_parser_m );
    
    const core_mac_address_s request_bssid = beacon_req_parser_m->bssid();
    const core_mac_address_s ap_bssid = ap_data.bssid();
    
    const core_mac_address_s broadcast_bssid = BROADCAST_MAC_ADDR;
    
    if ( broadcast_bssid != request_bssid )
        {
        if ( request_bssid == ap_bssid )
            {
            DEBUG( "core_operation_handle_measurement_request_c::match_found() - bssid matches" );
            bssid_match_found = true_t;
            }
        }
    else
        {
        bssid_match_found = true_t;
        }

    return ( ssid_match_found && bssid_match_found );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_operation_handle_measurement_request_c::create_response(
    core_ap_data_c& ap_data )
    {
    core_frame_rm_ie_beacon_report_frame_body_ie_c* frame_body_ie = NULL;
    
    if ( brd_parser_m )
        {
        if ( brd_parser_m->reporting_detail() == core_frame_rm_ie_beacon_request_detail_ie_c::core_frame_rm_ie_brd_reporting_detail_fields_and_requested_elements )
            {
            if ( request_ie_parser_m )
                {
                u8_t ie_id_list[REQUEST_IE_MAX_LENGTH] = { 0 };
                u8_t ie_id_list_length = request_ie_parser_m->element_list( ie_id_list );

                frame_body_ie = core_frame_rm_ie_beacon_report_frame_body_ie_c::instance(
                    ap_data,
                    ie_id_list,
                    ie_id_list_length );
                }

            if ( frame_body_ie == NULL )
                {
                DEBUG( "core_operation_handle_measurement_request_c::create_response() - Unable to instantiate core_frame_rm_ie_beacon_report_frame_body_ie_c" );
                return false_t;
                }
                
            DEBUG( "core_operation_handle_measurement_request_c::create_response() - Reported Frame Body data layout" );
            DEBUG_BUFFER( frame_body_ie->data_length(), frame_body_ie->data() );

            }
        else if ( brd_parser_m->reporting_detail() == core_frame_rm_ie_beacon_request_detail_ie_c::core_frame_rm_ie_brd_reporting_detail_default )
            {
            frame_body_ie = core_frame_rm_ie_beacon_report_frame_body_ie_c::instance(
                ap_data );

            if ( frame_body_ie == NULL )
                {
                DEBUG( "core_operation_handle_measurement_request_c::create_response() - Unable to instantiate core_frame_rm_ie_beacon_report_frame_body_ie_c" );
                return false_t;
                }
                
            DEBUG( "core_operation_handle_measurement_request_c::create_response() - Reported Frame Body data layout" );
            DEBUG_BUFFER( frame_body_ie->data_length(), frame_body_ie->data() );
            }
        }
    else
        {
        frame_body_ie = core_frame_rm_ie_beacon_report_frame_body_ie_c::instance(
            ap_data );

        if ( frame_body_ie == NULL )
            {
            DEBUG( "core_operation_handle_measurement_request_c::create_response() - Unable to instantiate core_frame_rm_ie_beacon_report_frame_body_ie_c" );
            return false_t;
            }
            
        DEBUG( "core_operation_handle_measurement_request_c::create_response() - Reported Frame Body data layout" );
        DEBUG_BUFFER( frame_body_ie->data_length(), frame_body_ie->data() );
        }

    core_frame_rm_ie_beacon_report_ie_c* ie = core_frame_rm_ie_beacon_report_ie_c::instance(
                BEACON_TABLE_REGULATORY_CLASS,
                actual_measurement_duration_m,
                BEACON_TABLE_REPORTED_FRAME_INFORMATION,
                ANTENNA_ID,
                ap_data );
    
    if ( ie == NULL )
        {
        DEBUG( "core_operation_handle_measurement_request_c::create_response() - Unable to instantiate core_frame_rm_ie_beacon_report_ie_c" );
        return false_t;
        }
        
    DEBUG( "core_operation_handle_measurement_request_c::create_response() - Beacon Report data layout" );
    DEBUG_BUFFER( ie->length(), ie->data() );

    // create beacon report ie
    core_frame_rm_ie_beacon_report_c* beacon_report_ie = NULL;

    if ( frame_body_ie != NULL )
        {
        beacon_report_ie = core_frame_rm_ie_beacon_report_c::instance(
                beacon_req_parser_m->measurement_token(),
                MEASUREMENT_REPORT_MODE_OK,
                ie,
                frame_body_ie );
        }
    else
        {
        beacon_report_ie = core_frame_rm_ie_beacon_report_c::instance(
                beacon_req_parser_m->measurement_token(),
                MEASUREMENT_REPORT_MODE_OK,
                ie );
        }
    
    delete ie;
    ie = NULL;

    if ( frame_body_ie )
        {
        delete frame_body_ie;
        frame_body_ie = NULL;
        }
    
    if ( beacon_report_ie == NULL )
        {
        DEBUG( "core_operation_handle_measurement_request_c::create_response() - Unable to instantiate core_frame_rm_ie_beacon_report_c" );
        return false_t;
        }
    
    DEBUG( "core_operation_handle_measurement_request_c::create_response() - Measurement Report data layout" );
    DEBUG_BUFFER( beacon_report_ie->data_length(), beacon_report_ie->data() );

    if ( !add_to_report( beacon_report_ie ) )
        {
        // add_to_report returns NULL only in out-of-memory situation
        DEBUG( "core_operation_handle_measurement_request_c::create_response() - Unable to create response to request" );
        
        // deallocate IE
        delete beacon_report_ie;
        beacon_report_ie = NULL;
        
        return false_t;
        }

    DEBUG( "core_operation_handle_measurement_request_c::create_response() - dot11 frame data layout" );
    DEBUG_BUFFER( report_m->data_length(), report_m->data() );
    
    // deallocate IE
    delete beacon_report_ie;
    beacon_report_ie = NULL;
    
    return true_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_handle_measurement_request_c::initialise_beacon_request_data()
    {
    DEBUG( "core_operation_handle_measurement_request_c::initialise_beacon_request_data()" );
    
    parsed_mask_m = MEASUREMENT_REQUEST_PARSED;
    actual_measurement_duration_m = 0;
    status_m.set_error_in_request( false_t );
    status_m.set_refuse_request( false_t );

    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_operation_handle_measurement_request_c::get_random_delay()
    {
    u32_t delay_in_us( 0 );
    
    if ( beacon_req_parser_m != NULL &&
         beacon_req_parser_m->randomization_interval() != 0 &&
         beacon_req_parser_m->measurement_mode() != core_frame_rm_ie_beacon_request_c::core_beacon_request_scan_mode_beacon )
        {
        u32_t delay_in_tus = core_am_tools_c::random() % beacon_req_parser_m->randomization_interval();
        DEBUG1( "core_operation_handle_measurement_request_c::get_random_delay() - random delay in TUs: %u", delay_in_tus );
        delay_in_us = delay_in_tus * TIMEUNITS_FROM_MICROSECONDS;  
        }

    DEBUG1( "core_operation_handle_measurement_request_c::get_random_delay() - random delay in us: %u", delay_in_us );
    
    return delay_in_us;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_operation_handle_measurement_request_c::user_cancel(
    bool_t do_graceful_cancel )
    {
    DEBUG1( "core_operation_handle_measurement_request_c::user_cancel(do_graceful_cancel=%d)", do_graceful_cancel );
    
    if ( !do_graceful_cancel )
        {
        /**
         * If we are in a middle of a scan, we have to schedule our own
         * event.
         */
        if ( operation_state_m == core_operation_handle_measurement_request_c::core_state_scan_start &&
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


