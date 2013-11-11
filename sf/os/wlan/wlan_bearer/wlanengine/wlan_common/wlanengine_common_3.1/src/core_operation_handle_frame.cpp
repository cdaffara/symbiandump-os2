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
* Description:  Statemachine for handling a frame received from drivers
*
*/


#include "core_operation_handle_frame.h"
#include "core_operation_ibss_merge.h"
#include "core_operation_handle_delete_ts.h"
#include "core_operation_update_power_mode.h"
#include "core_operation_handle_measurement_request.h"
#include "core_operation_handle_neighbor_response.h"
#include "core_frame_radio_measurement_action.h"
#include "core_frame_action_wmm.h"
#include "core_frame_wmm_ie_tspec.h"
#include "core_frame_action_rm.h"
#include "core_frame_action_nr.h"
#include "core_frame_tim_ie.h"
#include "core_frame_dot11.h"
#include "core_server.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_handle_frame_c::core_operation_handle_frame_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_frame_type_e frame_type,
    u16_t data_length,
    const u8_t* data ) :
    core_operation_base_c( core_operation_handle_frame, request_id, server, drivers, adaptation, 
        core_base_flag_drivers_needed | core_base_flag_connection_needed ),
    type_m( frame_type ),
    data_length_m( data_length ),
    data_m( NULL ),
    frame_m( NULL )
    {
    DEBUG( "core_operation_handle_frame_c::core_operation_handle_frame_c()" );

    data_m = new u8_t[data_length_m];
    if ( data_m )
        {
        core_tools_c::copy(
            data_m,
            data,
            data_length );
        }    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_handle_frame_c::~core_operation_handle_frame_c()
    {
    DEBUG( "core_operation_handle_frame_c::~core_operation_handle_frame_c()" );

    delete frame_m;
    frame_m = NULL;
    delete[] data_m;
    data_m = NULL;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_handle_frame_c::next_state()
    {
    DEBUG( "core_operation_handle_frame_c::next_state()" );
    
    switch ( operation_state_m )
        {
        case core_state_init:
            {
            if ( !data_m )
                {
                DEBUG( "core_operation_handle_frame_c::next_state() - no frame, aborting" );
                failure_reason_m = core_error_no_memory;

                return cancel();
                }            

            if ( !server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_handle_frame_c::next_state() - not connected, discarding frame" );
                
                return core_error_ok;
                }

            switch ( type_m )
                {
                case core_frame_type_dot11:
                    {
                    DEBUG( "core_operation_handle_frame_c::next_state() - core_frame_type_dot11" );
                    frame_m = core_frame_dot11_c::instance(
                        data_length_m,
                        data_m,
                        false_t );
                    if ( !frame_m )
                        {
                        DEBUG( "core_operation_handle_frame_c::next_state() - unable to create parser" );
                        failure_reason_m = core_error_general;

                        return cancel();                        
                        }

#ifdef _DEBUG                        
                    DEBUG1( "core_operation_handle_frame_c::next_state() - frame control: %04X",
                        frame_m->frame_control() );        
                    if ( frame_m->frame_control() & core_frame_dot11_c::core_dot11_frame_control_to_ds_mask )
                        {
                        DEBUG( "core_operation_handle_frame_c::next_state() - frame control ToDS: enabled" );
                        }
                    else
                        {
                        DEBUG( "core_operation_handle_frame_c::next_state() - frame control ToDS: disabled" );
                        }             
                    if ( frame_m->frame_control() & core_frame_dot11_c::core_dot11_frame_control_from_ds_mask )
                        {
                        DEBUG( "core_operation_handle_frame_c::next_state() - frame control FromDS: enabled" );
                        }
                    else
                        {
                        DEBUG( "core_operation_handle_frame_c::next_state() - frame control FromDS: disabled" );
                        }            
                    DEBUG1( "core_operation_handle_frame_c::next_state() - duration: %04X",
                        frame_m->duration() );
        
                    core_mac_address_s mac( ZERO_MAC_ADDR );
                    mac = frame_m->destination();            
                    DEBUG6( "core_operation_handle_frame_c::next_state() - destination: %02X:%02X:%02X:%02X:%02X:%02X",
                        mac.addr[0], mac.addr[1], mac.addr[2],
                        mac.addr[3], mac.addr[4], mac.addr[5] );
            
                    mac = frame_m->source();            
                    DEBUG6( "core_operation_handle_frame_c::next_state() - source: %02X:%02X:%02X:%02X:%02X:%02X",
                        mac.addr[0], mac.addr[1], mac.addr[2],
                        mac.addr[3], mac.addr[4], mac.addr[5] );

                    mac = frame_m->bssid();            
                    DEBUG6( "core_operation_handle_frame_c::next_state() - bssid: %02X:%02X:%02X:%02X:%02X:%02X",
                        mac.addr[0], mac.addr[1], mac.addr[2],
                        mac.addr[3], mac.addr[4], mac.addr[5] );                    
#endif // _DEBUG

                    if ( frame_m->type() == core_frame_dot11_c::core_dot11_type_beacon &&
                         server_m->get_core_settings().is_connected() )
                        {
                        if ( server_m->get_connection_data()->iap_data().operating_mode() == core_operating_mode_ibss )
                            {
                            operation_state_m = core_state_done;

                            DEBUG( "core_operation_handle_frame_c::next_state() - beacon received in IBSS mode" );

                            /**
                             * Beacons received in IBSS mode are an indication that a merge
                             * is needed.
                             */
                            core_operation_base_c* operation = new core_operation_ibss_merge_c(
                                request_id_m,
                                server_m,
                                drivers_m,
                                adaptation_m,
                                frame_m );

                            return run_sub_operation( operation );
                            }
                        else
                            {
                            DEBUG( "core_operation_handle_frame_c::next_state() - beacon received in infrastructure mode" );

                            /**
                             * Beacons received in infrastructure mode happen after an association.
                             *
                             * They are used to update the current AP data since beacons contain
                             * fields not present in probe responses.
                             */
                            if ( server_m->get_connection_data() &&
                                 server_m->get_connection_data()->current_ap_data() &&
                                 server_m->get_connection_data()->current_ap_data()->bssid() == frame_m->bssid() )
                                {
                                core_frame_beacon_c* beacon = core_frame_beacon_c::instance(
                                    *frame_m );
                                if ( beacon )
                                    {
                                    for( core_frame_dot11_ie_c* ie = beacon->first_ie(); ie; ie = beacon->next_ie() )
                                        {
                                        if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_tim )
                                            {
                                            core_frame_tim_ie_c* tim_ie = core_frame_tim_ie_c::instance( *ie );
                                            if ( tim_ie &&
                                                 tim_ie->dtim_period() )
                                                {
                                                DEBUG1( "core_operation_handle_frame_c::next_state() - updating AP DTIM period to %u",
                                                    tim_ie->dtim_period() );

                                                server_m->get_connection_data()->current_ap_data()->set_dtim_period(
                                                    tim_ie->dtim_period() );

                                                /**
                                                 * Schedule a power mode update since we may have to adjust the wakeup interval
                                                 * if DTIM skipping is used.
                                                 * 
                                                 * We should avoid changing power mode if DHCP timer is active.
                                                 */
                                                if( !server_m->is_dhcp_timer_active() )
                                                    {
                                                    DEBUG( "core_operation_handle_frame_c::next_state() - scheduling a power save update" );

                                                    core_operation_base_c* operation = new core_operation_update_power_mode_c(
                                                        request_id_m,
                                                        server_m,
                                                        drivers_m,
                                                        adaptation_m );
    
                                                    server_m->queue_int_operation( operation );
                                                    }
                                                else
                                                    {
                                                    DEBUG( "core_operation_handle_frame_c::next_state() - DHCP timer active, no power mode update" );
                                                    }
                                                }

                                            delete tim_ie;
                                            tim_ie = NULL;
                                            }

                                        delete ie;
                                        ie = NULL;
                                        }

                                    delete beacon;
                                    beacon = NULL;
                                    }
                                }
                            }
                        }
                    else if ( frame_m->type() == core_frame_dot11_c::core_dot11_type_action )
                        {
                        core_frame_action_c* action = core_frame_action_c::instance(
                            *frame_m );
                        if ( action )
                            {
                            DEBUG( "core_operation_handle_frame_c::next_state() - 802.11 action frame found" );
                            DEBUG1( "core_operation_handle_frame_c::next_state() - 802.11 action category: 0x%02X",
                                action->category() );
                            if ( action->category() == core_frame_action_c::core_dot11_action_category_wmm_qos )
                                {
                                core_frame_action_wmm_c* wmm_action = core_frame_action_wmm_c::instance( *action );
                                if ( wmm_action )
                                    {
                                    DEBUG( "core_operation_handle_frame_c::next_state() - 802.11 WMM action frame found" );
                                    DEBUG1( "core_operation_handle_frame_c::next_state() - 802.11 WMM action type: 0x%02X",
                                        wmm_action->action_type() );
                                    DEBUG1( "core_operation_handle_frame_c::next_state() - 802.11 WMM action dialog token: 0x%02X",
                                        wmm_action->dialog_token() );
                                    DEBUG1( "core_operation_handle_frame_c::next_state() - 802.11 WMM action status: 0x%02X",
                                        wmm_action->status() );

                                    if ( wmm_action->action_type() == core_frame_action_wmm_c::core_dot11_action_wmm_type_delts )
                                        {
                                        for( core_frame_dot11_ie_c* ie = wmm_action->first_ie(); ie; ie = wmm_action->next_ie() )
                                            {
                                            DEBUG1( "core_operation_handle_frame_c::next_state() - 802.11 WMM action IE ID: 0x%02X",
                                                ie->element_id() );
                                            DEBUG1( "core_operation_handle_frame_c::next_state() - 802.11 WMM action IE length: 0x%02X",
                                                ie->length() );

                                            if ( ie->element_id() == core_frame_dot11_ie_c::core_frame_dot11_ie_element_id_wmm_tspec )
                                                {
                                                core_frame_wmm_ie_tspec_c* tspec_ie = core_frame_wmm_ie_tspec_c::instance( *ie );
                                                if ( tspec_ie )
                                                    {
                                                    core_operation_base_c* operation = new core_operation_handle_delete_ts_c(
                                                        request_id_m,
                                                        server_m,
                                                        drivers_m,
                                                        adaptation_m,   
                                                        tspec_ie->tid() );

                                                    server_m->queue_int_operation( operation );

                                                    delete tspec_ie;
                                                    tspec_ie = NULL;
                                                    }
                                                }

                                            delete ie;
                                            ie = NULL;
                                            }
                                        }

                                    delete wmm_action;
                                    wmm_action = NULL;
                                    }
                                }
                            else if ( action->category() == core_frame_action_c::core_dot11_action_category_rm )
                                {
                                core_frame_radio_measurement_action_c* radio_measurement_action = core_frame_radio_measurement_action_c::instance( *action, true_t );
                                if ( radio_measurement_action )
                                    {
                                    DEBUG( "core_operation_handle_frame_c::next_state() - 802.11 RM action frame found" );
                                    DEBUG1( "core_operation_handle_frame_c::next_state() - 802.11 RM action type: 0x%02X",
                                        radio_measurement_action->action_type() );

                                    if ( radio_measurement_action->action_type() == core_frame_radio_measurement_action_c::core_dot11_action_rm_type_meas_req )
                                        {
                                        DEBUG( "core_operation_handle_frame_c::next_state() - Measurement Request received -> create operation to handle it" );

                                        // copy received frame to a new memory location, rm_action will have the ownership of the new data
                                        core_frame_action_rm_c* rm_action = core_frame_action_rm_c::instance( *action, true_t );
                                        if ( rm_action )
                                            {
                                            // operation will deallocate rm_action when finished
                                            core_operation_base_c* operation = new core_operation_handle_measurement_request_c(
                                                request_id_m,
                                                server_m,
                                                drivers_m,
                                                adaptation_m,   
                                                rm_action );
    
                                            server_m->queue_int_operation( operation );
                                            }
                                        }
                                    else if ( radio_measurement_action->action_type() == core_frame_radio_measurement_action_c::core_dot11_action_rm_type_neighbor_resp )
                                        {
                                        DEBUG( "core_operation_handle_frame_c::next_state() - Neighbor Report Response received -> create operation to handle it" );

                                        // copy received frame to a new memory location, nr_action will have the ownership of the new data
                                        core_frame_action_nr_c* nr_action = core_frame_action_nr_c::instance( *action, true_t );
                                        if ( nr_action )
                                            {
                                            // operation will deallocate nr_action when finished
                                            core_operation_base_c* operation = new core_operation_handle_neighbor_response_c(
                                                request_id_m,
                                                server_m,
                                                drivers_m,
                                                adaptation_m,   
                                                nr_action );
    
                                            server_m->queue_int_operation( operation );
                                            }
                                        }
                                    else
                                        {
                                        DEBUG( "core_operation_handle_frame_c::next_state() - 802.11 RM action type not supported, ignoring..." );
                                        }

                                    delete radio_measurement_action;
                                    radio_measurement_action = NULL;
                                    }
                                }

                            delete action;
                            action = NULL;
                            }
                        }
                    else
                        {
                        DEBUG( "core_operation_handle_frame_c::next_state() - unknown frame, discarding" );
                        }

                    return core_error_ok;
                    }
                case core_frame_type_snap:
                    {
                    DEBUG( "core_operation_handle_frame_c::next_state() - core_frame_type_snap, discarding" );
                    
                    return core_error_ok;
                    }
                default:
                    {
                    DEBUG( "core_operation_handle_frame_c::next_state() - unknown frame type, discarding" );
                    
                    return core_error_ok;
                    }
                }            
            }
        case core_state_done:
            {
            DEBUG( "core_operation_handle_frame_c::next_state() - frame processing done" );
            
            return core_error_ok;
            }            
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }
