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
* Description:  Main class for core server
*
*/

/*
* %version: 91 %
*/

#include "core_server.h"
#include "abs_core_server_callback.h"
#include "abs_core_driverif.h"
#include "abs_core_timer.h"
#include "abs_core_frame_handler.h"
#include "abs_core_event_handler.h"
#include "core_timer_factory.h"
#include "core_operation_connect.h"
#include "core_operation_update_device_settings.h"
#include "core_operation_scan.h"
#include "core_operation_get_available_iaps.h"
#include "core_operation_release.h"
#include "core_operation_unload_drivers.h"
#include "core_operation_check_rcpi.h"
#include "core_operation_handle_frame.h"
#include "core_operation_null.h"
#include "core_operation_get_rcpi.h"
#include "core_operation_configure_multicast_group.h"
#include "core_operation_update_power_mode.h"
#include "core_operation_update_rxtx_parameters.h"
#include "core_operation_get_statistics.h"
#include "core_operation_set_uapsd_settings.h"
#include "core_operation_set_power_save_settings.h"
#include "core_operation_protected_setup.h"
#include "core_operation_create_ts.h"
#include "core_operation_delete_ts.h"
#include "core_operation_power_save_test.h"
#include "core_operation_set_arp_filter.h"
#include "core_operation_directed_roam.h"
#include "core_frame_ethernet.h"
#include "core_connection_data.h"
#include "core_eapol_handler.h"
#include "core_frame_dot11.h"
#include "core_frame_echo_test.h"
#include "am_debug.h"
#include "core_callback.h"
#include "core_tools.h"
#include "core_tools_parser.h"
#include "core_wpx_adaptation_factory.h"
#include "core_traffic_stream_list_iter.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_server_c::core_server_c(
    abs_core_server_callback_c& callback,
    abs_core_driverif_c& drivers,
    const core_device_settings_s& settings,
    const core_mac_address_s& mac_address,
    u32_t features ) :
    callback_m( callback ),
    drivers_m( drivers ),
    device_settings_m( settings ),
    queue_m( ),
    queue_timer_m( NULL ),
    driver_unload_timer_m( NULL ),
    core_settings_m( features ),
    connection_data_m( NULL ),
    own_mac_addr_m( mac_address ),
    eapol_handler_m( NULL ),
    eapol_m( NULL ),
    cm_timer_m( ),
    roam_timer_m( NULL ),
    operation_timer_m( NULL ),
    driver_dhcp_timer_m( NULL ),
    frame_handler_m( NULL ),
    event_handler_m( NULL ),
    scan_list_m( ),
    wpx_adaptation_m( NULL )
    {
    DEBUG( "core_server_c::core_server_c()" );
    queue_m.clear();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_server_c::~core_server_c()
    {
    DEBUG( "core_server_c::~core_server_c()" );

    queue_m.clear();
    delete connection_data_m;

    if ( queue_timer_m )
        {
        queue_timer_m->stop();
        core_timer_factory_c::destroy_timer( queue_timer_m );
        queue_timer_m = NULL;
        }
    if ( driver_unload_timer_m )
        {
        driver_unload_timer_m->stop();
        core_timer_factory_c::destroy_timer( driver_unload_timer_m );
        driver_unload_timer_m = NULL;
        }
    if ( roam_timer_m )
        {
        roam_timer_m->stop();
        core_timer_factory_c::destroy_timer( roam_timer_m );
        roam_timer_m = NULL;
        }
    if ( operation_timer_m )
        {
        operation_timer_m->stop();
        core_timer_factory_c::destroy_timer( operation_timer_m );
        operation_timer_m = NULL;
        }
    if ( driver_dhcp_timer_m )
        {
        driver_dhcp_timer_m->stop();
        core_timer_factory_c::destroy_timer( driver_dhcp_timer_m );
        driver_dhcp_timer_m = NULL;
        }
    if ( eapol_m )
        {
        eapol_m->shutdown();
        delete eapol_m;
        }
    delete wpx_adaptation_m;
    delete eapol_handler_m;
    frame_handler_m = NULL;
    event_handler_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::init()
    {
    DEBUG( "core_server_c::init()" );

    drivers_m.init( this );

    core_callback_c* queue_timer_callback = 
        new core_callback_c( &(core_server_c::queue_timer_expired), this );
    if( !queue_timer_callback )
        {
        DEBUG("ERROR creating callback object");
        return core_error_no_memory;
        }
    queue_timer_m = core_timer_factory_c::create_timer( queue_timer_callback );
    if( !queue_timer_m )
        {
        DEBUG( "ERROR creating timer" );
        delete queue_timer_callback;
        return core_error_no_memory;
        }

    core_callback_c* unload_timer_callback = 
        new core_callback_c( &(core_server_c::unload_timer_expired), this );
    if( !unload_timer_callback )
        {
        DEBUG("ERROR creating callback object");
        return core_error_no_memory;
        }
    driver_unload_timer_m = core_timer_factory_c::create_timer( unload_timer_callback );
    if( !driver_unload_timer_m )
        {
        DEBUG( "ERROR creating timer" );
        delete unload_timer_callback;
        return core_error_no_memory;
        }
    
    core_callback_c* roam_timer_callback = 
        new core_callback_c( &(core_server_c::roam_timer_expired), this );
    if( !roam_timer_callback )
        {
        DEBUG("ERROR creating callback object");
        return core_error_no_memory;
        }
    roam_timer_m = core_timer_factory_c::create_timer( roam_timer_callback );
    if( !roam_timer_m )
        {
        DEBUG( "ERROR creating timer" );
        delete roam_timer_callback;
        return core_error_no_memory;
        } 

    core_callback_c* operation_timer_callback = 
        new core_callback_c( &(core_server_c::operation_timer_expired), this );
    if( !operation_timer_callback )
        {
        DEBUG("ERROR creating callback object");
        return core_error_no_memory;
        }
    operation_timer_m = core_timer_factory_c::create_timer( operation_timer_callback );
    if( !operation_timer_m )
        {
        DEBUG( "ERROR creating timer" );
        delete operation_timer_callback;
        return core_error_no_memory;
        } 

    core_callback_c* dhcp_timer_callback = 
        new core_callback_c( &(core_server_c::dhcp_timer_expired), this );
    if( !dhcp_timer_callback )
        {
        DEBUG("ERROR creating callback object");
        return core_error_no_memory;
        }        
    driver_dhcp_timer_m = core_timer_factory_c::create_timer( dhcp_timer_callback );
    if( !driver_dhcp_timer_m )
        {
        DEBUG( "ERROR creating timer" );
        delete dhcp_timer_callback;
        return core_error_no_memory;
        } 

    wpx_adaptation_m = core_wpx_adaptation_factory_c::instance(
        this, &drivers_m, &callback_m );
    if ( !wpx_adaptation_m )
        {
        DEBUG("ERROR creating WPX adaptation");
        return core_error_no_memory;
        }

    eapol_handler_m = new core_eapol_handler_c(
        this, &drivers_m, &callback_m );
    if ( !eapol_handler_m )
        {
        DEBUG("ERROR creating eapol handler");
        return core_error_no_memory;
        }

    core_settings_m.set_rcpi_boundaries(
        device_settings_m.rcpi_trigger,
        device_settings_m.rcpi_trigger + device_settings_m.rcpi_difference );

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_device_settings_s& core_server_c::get_device_settings()
    {
    return device_settings_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_settings_c& core_server_c::get_core_settings()
    {
    return core_settings_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_connection_data_c* core_server_c::get_connection_data()
    {
    return connection_data_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_wlan_eapol_interface_c& core_server_c::get_eapol_instance()
    {
    return *eapol_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::set_eapol_handler(
    abs_wlan_eapol_callback_interface_c* handler )
    {
    eapol_handler_m->set_eapol_handler( handler );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_mac_address_s& core_server_c::own_mac_addr()
    {
    return own_mac_addr_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::queue_int_operation(
    core_operation_base_c* operation )
    {
    DEBUG( "core_server_c::queue_int_operation()" );

    if ( !operation )
        {
        DEBUG( "core_server_c::queue_int_operation() - no operation" );

        return core_error_no_memory;
        }
    else if ( operation->is_flags( core_operation_base_c::core_base_flag_only_one_instance ) &&
        is_operation_in_queue_with_type( operation->operation_type() ) )
        {
        DEBUG( "core_server_c::queue_int_operation() - only one instance allowed in the operation queue" );

        delete operation;
        operation = NULL;

        return core_error_already_exists;
        }
    else
        {
        core_error_e ret = queue_m.append( operation );
        if ( ret != core_error_ok )
            {
            DEBUG1( "core_server_c::queue_int_operation() - unable to queue the request (%u)",
                ret );

            delete operation;
            operation = NULL;

            return ret;
            }
        }

    return core_error_ok;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::queue_int_operation_and_run_next(
    core_operation_base_c* operation )
    {
    DEBUG( "core_server_c::queue_int_operation_and_run_next()" );

    core_error_e ret = queue_int_operation( operation );
    if ( ret != core_error_ok )
        {
        return ret;
        }

    schedule_operation();

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::init_connection_data(
    const core_iap_data_s& iap_data,
    const core_device_settings_s& device_settings )
    {
    connection_data_m = new core_connection_data_c( iap_data, device_settings );

    if ( !connection_data_m )
        {
        return core_error_no_memory;
        }

    return core_error_ok;
    }
   
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
void core_server_c::clear_connection_data()
    {
    DEBUG( "core_server_c::clear_connection_data()" );
    
    delete connection_data_m;
    connection_data_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::schedule_roam_timer(
    u32_t delay )
    {
    DEBUG1( "core_server_c::schedule_roam_timer() - delay %u",
        delay );

    ASSERT( !roam_timer_m->is_active() );

    roam_timer_m->start(
        delay );    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::cancel_roam_timer()
    {
    DEBUG( "core_server_c::cancel_roam_timer()" );
    
    roam_timer_m->stop();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
void core_server_c::roam_timer_expired(
    void* this_ptr )
    {
    DEBUG("core_server_c::roam_timer_expired()");
    core_server_c* self = static_cast<core_server_c*>( this_ptr );

    // send an indication to adaptation
    self->callback_m.notify(
        core_notification_rcpi_roam_attempt_started,
        0, 
        NULL );
    
    core_operation_base_c* command = new core_operation_check_rcpi_c(
        REQUEST_ID_CORE_INTERNAL,
        self,
        &(self->drivers_m),
        &(self->callback_m),
        core_operation_check_rcpi_c::core_rcpi_check_reason_timer );

    self->queue_int_operation_and_run_next( command );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::schedule_operation_timer(
    u32_t delay )
    {
    DEBUG1( "core_server_c::schedule_operation_timer() - delay %u",
        delay );

    ASSERT( !operation_timer_m->is_active() );
    operation_timer_m->start(
        delay );    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::cancel_operation_timer()
    {
    DEBUG( "core_server_c::cancel_operation_timer()" );
    operation_timer_m->stop();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
void core_server_c::operation_timer_expired(
    void* this_ptr )
    {
    DEBUG("core_server_c::operation_timer_expired()");
    core_server_c* self = static_cast<core_server_c*>( this_ptr );

    core_operation_base_c* command = self->queue_m.first();
    ASSERT( command );
    ASSERT( command->is_executing() );
    self->request_complete( command->request_id(), core_error_ok );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_server_c::is_operation_in_queue_with_flags(
    u32_t feature_flags )
    {
    core_type_list_iterator_c<core_operation_base_c> iter( queue_m );
    for( core_operation_base_c* current = iter.first(); current; current = iter.next() )
        {
        if ( current->is_flags( feature_flags ) )
            {
            return true_t;
            }
        }

    return false_t;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_server_c::is_operation_in_queue_with_type(
    u32_t type )
    {
    core_type_list_iterator_c<core_operation_base_c> iter( queue_m );
    for( core_operation_base_c* current = iter.first(); current; current = iter.next() )
        {
        if ( current->operation_type() == type )
            {
            return true_t;
            }
        }

    return false_t;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_server_c::is_dhcp_timer_active()
    {
    return driver_dhcp_timer_m->is_active();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::schedule_dhcp_timer(
    u32_t delay )
    {
    DEBUG1( "core_server_c::schedule_dhcp_timer() - delay %u",
        delay );

    ASSERT( !is_dhcp_timer_active() );

    driver_dhcp_timer_m->start(
        delay );    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::cancel_dhcp_timer()
    {
    DEBUG( "core_server_c::cancel_dhcp_timer()" );
    
    driver_dhcp_timer_m->stop();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::dhcp_timer_expired(
    void* this_ptr )
    {
    DEBUG( "core_server_c::dhcp_timer_expired()" );

    core_server_c* self = static_cast<core_server_c*>( this_ptr );

    core_operation_base_c* command = new core_operation_power_save_test_c(
        REQUEST_ID_CORE_INTERNAL,
        self,
        &(self->drivers_m),
        &(self->callback_m) );        

    self->queue_int_operation_and_run_next( command );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::schedule_unload_timer(
    u32_t delay )
    {
    DEBUG1( "core_server_c::schedule_unload_timer() - delay %u",
        delay );

    ASSERT( !driver_unload_timer_m->is_active() );

    driver_unload_timer_m->start(
        delay );    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::cancel_unload_timer()
    {
    DEBUG( "core_server_c::cancel_unload_timer()" );

    driver_unload_timer_m->stop();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::cancel_operations_with_flags(
    u32_t feature_flags )
    {
    DEBUG( "core_server_c::cancel_operations_with_flags()" );
    DEBUG1( "core_server_c::cancel_operations_with_flags() - feature_flags %u",
        feature_flags );

    core_operation_base_c* command = queue_m.first();
    while ( command )
        {
        if ( !command->is_executing() &&
             command->is_flags( feature_flags ) )
            {
            DEBUG2( "core_server_c::cancel_operations_with_flags() - canceling operation (id %u, type %u)",
                command->request_id(), command->operation_type() );
            if ( command->request_id() != REQUEST_ID_CORE_INTERNAL )
                {
                DEBUG( "core_server_c::cancel_operations_with_flags() - adaptation request, completing it" );
                callback_m.request_complete(
                    command->request_id(),
                    core_error_cancel );
                }

            queue_m.remove( command );
            delete command;
            command = queue_m.first();
            }
        else
            {
            command = queue_m.next();
            }        
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::cancel_operations_with_type(
    u32_t type )
    {
    DEBUG( "core_server_c::cancel_operation_with_type()" );
    DEBUG1( "core_server_c::cancel_operation_with_type() - type %u",
        type );

    core_operation_base_c* command = queue_m.first();
    while ( command )
        {
        if ( !command->is_executing() &&
             command->operation_type() == type )
            {
            DEBUG2( "core_server_c::cancel_operation_with_type() - canceling operation (id %u, type %u)",
                command->request_id(), command->operation_type() );
            if ( command->request_id() != REQUEST_ID_CORE_INTERNAL )
                {
                DEBUG( "core_server_c::cancel_operation_with_type() - adaptation request, completing it" );
                callback_m.request_complete(
                    command->request_id(),
                    core_error_cancel );
                }

            queue_m.remove( command );
            delete command;
            command = queue_m.first();
            }
        else
            {
            command = queue_m.next();
            }
        }       
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::cancel_all_operations(
    bool_t is_graceful_cancel )
    {
    DEBUG( "core_server_c::cancel_all_operations()" );

    core_operation_base_c* command = queue_m.first();
    while ( command )
        {
        if ( !command->is_executing() )
            {
            DEBUG2( "core_server_c::cancel_all_operations() - canceling operation (id %u, type %u)",
                command->request_id(), command->operation_type() );
            if ( command->request_id() != REQUEST_ID_CORE_INTERNAL )
                {
                DEBUG( "core_server_c::cancel_all_operations() - adaptation request, completing it" );
                callback_m.request_complete(
                    command->request_id(),
                    core_error_cancel );
                }

            queue_m.remove( command );
            delete command;
            command = queue_m.first();
            }
        else
            {
            command = queue_m.next();
            }
        }

    command = queue_m.first();
    if ( command )
        {
        if( is_graceful_cancel )
            {
            DEBUG2( "core_server_c::cancel_all_operations() - canceling currently executing operation (id %u, type %u) (graceful cancel)",
                command->request_id(), command->operation_type() );
            }
        else
            {
            DEBUG2( "core_server_c::cancel_all_operations() - canceling currently executing operation (id %u, type %u) (forced cancel)",
                command->request_id(), command->operation_type() );
            }

        ASSERT( command->is_executing() );

        // Let operation handle it's own cancelling
        command->user_cancel_operation( is_graceful_cancel );
        }   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_server_c::create_eapol_instance(
    core_eapol_operating_mode_e mode )
    {
    DEBUG( "core_server_c::create_eapol_instance()" );

    if ( eapol_m &&
        eapol_m->operating_mode() == mode )
        {
        DEBUG( "core_server_c::create_eapol_instance() - an instance already exists" );

        return true_t;
        }

    if ( eapol_m )
        {
        DEBUG( "core_server_c::create_eapol_instance() - deleting old core_wlan_eapol_interface_c instance" );

        eapol_m->shutdown();
        delete eapol_m;
        eapol_m = NULL;
        }
    
    DEBUG1( "core_server_c::create_eapol_instance() - creating core_wlan_eapol_interface_c (mode %u)",
        mode );

    eapol_m = new core_wlan_eapol_interface_c( callback_m );
    if ( !eapol_m )
        {
        DEBUG( "core_server_c::create_eapol_instance() - unable to create core_wlan_eapol_interface_c" );
        return false_t;
        }
    core_error_e error = eapol_m->load_eapol( mode, eapol_handler_m );
    if ( error != core_error_ok )
        {
        DEBUG1( "core_server_c::create_eapol_instance() - load_eapol failed with %i", error );
        delete eapol_m;
        eapol_m = NULL;
        return false_t;
        }
    error = eapol_m->configure();
    if ( error != core_error_ok )
        {
        DEBUG1( "core_server_c::create_eapol_instance() - configure failed with %i", error );
        delete eapol_m;
        eapol_m = NULL;
        return false_t;
        }

    return true_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
abs_core_frame_handler_c* core_server_c::frame_handler()
    {
    return frame_handler_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::register_frame_handler(
    abs_core_frame_handler_c* handler )
    {
    ASSERT( handler );
    ASSERT( handler == frame_handler_m || !frame_handler_m );

    frame_handler_m = handler;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::unregister_frame_handler(
    abs_core_frame_handler_c* handler )
    {
    ASSERT( handler );
    
    if ( frame_handler_m != handler )
        {
        DEBUG1( "core_server_c::unregister_frame_handler() - handler 0x%08X not registered",
            handler );

        return;
        }

    frame_handler_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_scan_list_c& core_server_c::get_scan_list()
    {
    return scan_list_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
abs_core_event_handler_c* core_server_c::event_handler()
    {
    return event_handler_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::register_event_handler(
    abs_core_event_handler_c* handler )
    {
    ASSERT( handler );
    ASSERT( handler == event_handler_m || !event_handler_m );

    event_handler_m = handler;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::unregister_event_handler(
    abs_core_event_handler_c* handler )
    {
    ASSERT( handler );

    if ( event_handler_m != handler )
        {
        DEBUG1( "core_server_c::unregister_event_handler() - handler 0x%08X not registered",
            handler );

        return;
        }

    event_handler_m = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::schedule_roam(
    core_operation_handle_bss_lost_c::core_bss_lost_reason_e reason )
    {
    if ( !is_operation_in_queue_with_type( core_operation_handle_bss_lost ) )
        {       
        DEBUG( "core_server_c::schedule_roam() - scheduling a core_operation_handle_bss_lost operation" );

        core_operation_base_c* command = new core_operation_handle_bss_lost_c(
            REQUEST_ID_CORE_INTERNAL,
            this,
            &drivers_m,
            &callback_m,
            reason );

        queue_int_operation_and_run_next( command );
        }
    else
        {
        DEBUG( "core_server_c::schedule_roam() - core_operation_handle_bss_lost already in the queue" );
        }        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_server_c::send_management_frame(
    core_frame_type_e frame_type,
    const u16_t frame_length,
    const u8_t* const frame_data,
    const core_mac_address_s& destination )
    {
    /** Management frames can be sent without downgraded user priority. */
    drivers_m.send_frame(
        frame_type,
        frame_length,
        frame_data,
        core_tools_c::convert_ac_to_user_priority( core_access_class_voice ),
        destination,
        true_t );    

    return true_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_server_c::send_data_frame(
    const core_ap_data_c& ap_data,
    core_frame_type_e frame_type,
    const u16_t frame_length,
    const u8_t* const frame_data,
    core_access_class_e frame_priority,
    const core_mac_address_s& destination,
    bool_t send_unencrypted )
    {
    DEBUG( "core_server_c::send_data_frame()" );
    
    u8_t initial_priority(
        core_tools_c::convert_ac_to_user_priority( frame_priority ) );
    u8_t user_priority = initial_priority;
      
    if ( ap_data.is_wmm_ie_present() )
        {
        /**
         * Collect a list of active streams per user priority.
         */
        bool_t is_ts_for_user_priority[MAX_QOS_USER_PRIORITY];
        core_tools_c::fillz(
            &is_ts_for_user_priority[0],
            sizeof( is_ts_for_user_priority ) );

        core_traffic_stream_list_iter_c iter(
            get_connection_data()->traffic_stream_list() );

        core_traffic_stream_c* entry = iter.first();
        while( entry )
            {
            if ( entry->status() == core_traffic_stream_status_active )
                {
                is_ts_for_user_priority[entry->user_priority()] = true_t;
                }

            entry = iter.next();
            }

        /**
         * See if downgrade should be done.
         */
        bool_t is_send_ok( false_t );        

        while ( user_priority <= initial_priority && !is_send_ok )
            {
            core_access_class_e ac(
                core_tools_c::convert_user_priority_to_ac( user_priority ) );

#if 0
            DEBUG1( "core_server_c::send_data_frame() - user_priority is %u",
                user_priority );
            DEBUG1( "core_server_c::send_data_frame() - ac is %u",
                ac );
            DEBUG1( "core_server_c::send_data_frame() - is_admission_control_required is %u",
                ap_data.is_admission_control_required( ac ) );
            DEBUG1( "core_server_c::send_data_frame() - is_ts_for_user_priority is %u",
                is_ts_for_user_priority[user_priority] );
#endif // 0

            if ( !ap_data.is_admission_control_required( ac ) ||
                 is_ts_for_user_priority[user_priority] )
                {
                is_send_ok = true_t;
                }
            else
                {
                user_priority--;
                }
            }

        if ( !is_send_ok )
            {
            DEBUG( "core_server_c::send_data_frame() - unable to send the frame due to admission control settings" );
            return false_t;
            }
        }

    if ( user_priority == initial_priority )
        {
        DEBUG1( "core_server_c::send_data_frame() - no need to downgrade user priority %u",
            user_priority );
        }
    else
        {
        DEBUG2( "core_server_c::send_data_frame() - user priority %u downgraded to %u",
            initial_priority, user_priority );
        }

    drivers_m.send_frame(
        frame_type,
        frame_length,
        frame_data,
        user_priority,
        destination,
        send_unencrypted ); 

    return true_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
abs_core_wpx_adaptation_c& core_server_c::get_wpx_adaptation_instance()
    {
    return *wpx_adaptation_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::set_protected_setup_handler(
    abs_core_protected_setup_handler_c* handler )
    {
    eapol_handler_m->set_protected_setup_handler( handler );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::connect(
    u32_t request_id,
    const core_iap_data_s& settings,
    core_connect_status_e& connect_status,
    core_type_list_c<core_ssid_entry_s>* ssid_list )
    {
    DEBUG( "core_server_c::connect()" );

    core_operation_base_c* command = new core_operation_connect_c(
        request_id, this, &drivers_m, &callback_m, settings, ssid_list, connect_status );

    queue_ext_operation_and_run_next( command, request_id );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::release(
    u32_t request_id )
    {
    DEBUG( "core_server_c::release()" );

    /**
     * Cancel all pending roaming operations.
     */
    cancel_operations_with_flags(
        core_operation_base_c::core_base_flag_roam_operation );

    /**
     * If there's an executing roam operation, let it handle
     * its own cancel.
     */
    core_operation_base_c* operation = queue_m.first();
    if( operation &&
        operation->is_flags( core_operation_base_c::core_base_flag_roam_operation ) )
        {
        operation->user_cancel_operation( true_t ); // Using graceful cancel
        }

    core_operation_base_c* command = new core_operation_release_c(
        request_id, this, &drivers_m, &callback_m, core_release_reason_external_request );

    queue_ext_operation_and_run_next( command, request_id );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::get_scan_result(
    u32_t request_id,
    core_scan_mode_e scan_mode,
    const core_ssid_s& scan_ssid,    
    const core_scan_channels_s& scan_channels,
    u8_t scan_max_age,
    ScanList& scan_data,
    bool_t is_current_ap_added )
    {
    DEBUG( "core_server_c::get_scan_result()" );
    
    core_operation_base_c* command = new core_operation_scan_c(
        request_id, this, &drivers_m, &callback_m,
        scan_mode, scan_ssid, scan_channels, scan_max_age, scan_data, 
        true_t,
        is_current_ap_added );

    queue_ext_operation_and_run_next( command, request_id );    
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::get_available_iaps(
    u32_t request_id,
    bool_t is_active_scan_allowed,
    core_type_list_c<core_iap_data_s>& iap_data_list,
    core_type_list_c<core_iap_availability_data_s>& iap_availability_list,
    core_type_list_c<core_ssid_entry_s>* iap_ssid_list,
    ScanList& scan_data )
    {
    DEBUG( "core_server_c::get_available_iaps()" );

    core_operation_base_c* command = new core_operation_get_available_iaps_c(
        request_id, 
        this, 
        &drivers_m, 
        &callback_m, 
        is_active_scan_allowed, 
        iap_data_list, 
        iap_availability_list,
        iap_ssid_list,
        scan_data );

    queue_ext_operation_and_run_next( command, request_id );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::get_current_rcpi(
    u32_t request_id,
    u32_t& rcpi )
    {
    DEBUG( "core_server_c::get_current_rcpi()" );
    
    core_operation_base_c* command = new core_operation_get_rcpi_c(
        request_id, this, &drivers_m, &callback_m, rcpi );

    queue_ext_operation_and_run_next( command, request_id );    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::disable_wlan(
    u32_t request_id )
    {
    DEBUG( "core_server_c::disable_wlan()" );

    get_core_settings().set_wlan_enabled( false_t );

    /**
     * Schedule an immediate driver unload.
     */ 
    unload_drivers();

    core_operation_base_c* command = new core_operation_null_c(
        request_id, this, &drivers_m, &callback_m, core_error_ok );

    queue_ext_operation_and_run_next( command, request_id );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::enable_wlan(
    u32_t request_id )
    {
    DEBUG( "core_server_c::enable_wlan()" );
    
    core_settings_m.set_wlan_enabled( true_t );
    
    core_operation_base_c* command = new core_operation_null_c(
        request_id, this, &drivers_m, &callback_m, core_error_ok );

    queue_ext_operation_and_run_next( command, request_id );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::unload_drivers()
    {
    DEBUG( "core_server_c::unload_drivers()" );

    /**
     * If drivers are loaded, schedule an immediate unload.
     */
    if( get_core_settings().is_driver_loaded() )
        {
        cancel_unload_timer();
        schedule_unload_timer( CORE_TIMER_IMMEDIATELY );

        return core_error_ok;
        }
    else
        {
        return core_error_drivers_not_loaded;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::get_packet_statistics(
    u32_t request_id,
    core_packet_statistics_s& statistics )
    {
    DEBUG( "core_server_c::get_packet_statistics()" );

    core_operation_base_c* command = new core_operation_get_statistics_c(
        request_id, this, &drivers_m, &callback_m, statistics );

    queue_ext_operation_and_run_next( command, request_id );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::create_traffic_stream(
    u32_t request_id,
    u8_t tid,
    u8_t user_priority,
    bool_t is_automatic_stream,
    const core_traffic_stream_params_s& params,
    u32_t& stream_id,
    core_traffic_stream_status_e& stream_status )
    {
    DEBUG( "core_server_c::create_traffic_stream()" );

    core_operation_base_c* command = new core_operation_create_ts_c(
        request_id,
        this,
        &drivers_m,
        &callback_m,
        tid,
        user_priority,
        is_automatic_stream,
        params,
        stream_id,
        stream_status );
   
    queue_ext_operation_and_run_next( command, request_id );    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::delete_traffic_stream(
    u32_t request_id,
    u32_t stream_id )
    {
    DEBUG( "core_server_c::delete_traffic_stream()" );

    core_operation_base_c* command = new core_operation_delete_ts_c(
        request_id,
        this,
        &drivers_m,
        &callback_m,
        stream_id );

    queue_ext_operation_and_run_next( command, request_id );    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
void core_server_c::run_protected_setup(
    u32_t request_id, 
    const core_iap_data_s& iap_data,
    core_type_list_c<core_iap_data_s>& iap_data_list,
    core_protected_setup_status_e& protected_setup_status )
    {
    DEBUG( "core_server_c::run_protected_setup()" );
    
    core_operation_base_c* command = new core_operation_protected_setup_c(
        request_id, this, &drivers_m, &callback_m, iap_data, iap_data_list,
        protected_setup_status );

    queue_ext_operation_and_run_next( command, request_id );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::directed_roam(
    u32_t request_id,
    const core_mac_address_s& bssid )
    {
    DEBUG( "core_server_c::directed_roam()" );

    core_operation_base_c* command = new core_operation_directed_roam_c(
        request_id, this, &drivers_m, &callback_m, bssid );

    queue_ext_operation_and_run_next( command, request_id );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::get_current_bssid(
    core_mac_address_s& bssid )
    {
    DEBUG( "core_server_c::get_current_bssid()" );

    bssid = ZERO_MAC_ADDR;
    if ( core_settings_m.is_connected() &&
         connection_data_m &&
         connection_data_m->current_ap_data() )
        {
        bssid = connection_data_m->current_ap_data()->bssid();

        return core_error_ok;
        }

    return core_error_not_connected;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::get_current_ssid(
    core_ssid_s& ssid )
    {
    DEBUG( "core_server_c::get_current_ssid()" );

    ssid = BROADCAST_SSID;
    if ( core_settings_m.is_connected() &&
         connection_data_m &&
         connection_data_m->current_ap_data() )
        {
        ssid = connection_data_m->ssid();

        return core_error_ok;
        }

    return core_error_not_connected;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::get_current_security_mode(
    core_connection_security_mode_e& mode )
    {
    DEBUG( "core_server_c::get_current_security_mode()" );

    mode = core_connection_security_mode_open;
    if ( core_settings_m.is_connected() &&
         connection_data_m &&
         connection_data_m->current_ap_data() )
        {
        mode = core_tools_c::security_mode(
            connection_data_m->iap_data(),
            *connection_data_m->current_ap_data() );

        return core_error_ok;
        }

    return core_error_not_connected;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::get_current_connection_state(
    core_connection_state_e& state )
    {
    DEBUG( "core_server_c::get_current_connection_state()" );

    state = core_settings_m.connection_state();

    return core_error_ok;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::update_device_settings(
    core_device_settings_s& settings )
    {
    DEBUG( "core_server_c::update_device_settings()" );

    device_settings_m = settings;

    core_operation_base_c* command = new core_operation_update_device_settings_c(
        REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m );

    queue_int_operation_and_run_next( command );

    return core_error_ok;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::set_power_save_mode(
    const core_power_save_mode_s& mode )
    {
    DEBUG( "core_server_c::set_power_save_mode()" );

    core_settings_m.set_preferred_power_save_mode( mode );

    core_operation_base_c* command = new core_operation_update_power_mode_c(
        REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m );

    queue_int_operation_and_run_next( command );

    return core_error_ok;        
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::add_bssid_to_rogue_list(
    const core_mac_address_s& bssid )
    {
    DEBUG( "core_server_c::add_bssid_to_rogue_list()" );

    core_settings_m.add_mac_to_permanent_blacklist(
        bssid, core_ap_blacklist_reason_external );

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_error_e core_server_c::remove_bssid_from_rogue_list(
    const core_mac_address_s& bssid )
    {
    DEBUG( "core_server_c::remove_bssid_from_rogue_list()" );

    core_settings_m.remove_mac_from_permanent_blacklist( bssid );

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::get_rogue_list(
    core_type_list_c<core_mac_address_s>& rogue_list )
    {
    DEBUG( "core_server_c::get_rogue_list()" );

    core_type_list_c<core_ap_blacklist_entry_s>& perm_list = core_settings_m.permanent_blacklist();
    DEBUG1( "core_server_c::get_rogue_list() - %d permanent addresses", perm_list.count() );

    // Loop through permanent blacklist
    core_ap_blacklist_entry_s* addr = perm_list.first();
    while ( addr )
        {
        // Address must be copied, because rogue_list get ownership for its entries.
        core_mac_address_s* copy_addr = new core_mac_address_s;
        if ( !copy_addr )
            {
            return core_error_no_memory;
            }
        *copy_addr = addr->bssid;
        DEBUG_MAC( copy_addr->addr );
        
        core_error_e status = rogue_list.append( copy_addr );
        if ( status != core_error_ok )
            {
            return status;
            }

        addr = perm_list.next();
        }

    // Check if connection is available.
    if ( connection_data_m )
        {
        core_type_list_c<core_ap_blacklist_entry_s>& temp_list = connection_data_m->temporary_blacklist();
        DEBUG1( "core_server_c::get_rogue_list() - %d temporary addresses", temp_list.count() );
        
        // Loop through temporary blacklist
        addr = temp_list.first();
        while ( addr )
            {
            // Address must be copied, because rogue_list get ownership for its entries.
            core_mac_address_s* copy_addr = new core_mac_address_s;
            if ( !copy_addr )
                {
                return core_error_no_memory;
                }
            *copy_addr = addr->bssid;
            DEBUG_MAC( copy_addr->addr );
            
            core_error_e status = rogue_list.append( copy_addr );
            if ( status != core_error_ok )
                {
                return status;
                }
    
            addr = temp_list.next();
            }
        }
    else
        {
        DEBUG( "core_server_c::get_rogue_list() - connection is not available, temporary blacklist is empty" );
        }

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
core_error_e core_server_c::set_rcp_level_notification_boundary(
    const i32_t rcp_level_boundary,
    const i32_t hysteresis )
    {
    DEBUG( "core_server_c::set_rcp_level_notification_boundary()" );

    core_settings_m.set_rcpi_boundaries(
        rcp_level_boundary,
        rcp_level_boundary + hysteresis );

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::clear_packet_statistics()
    {
    DEBUG( "core_server_c::clear_packet_statistics()" );

    core_settings_m.clear_connection_statistics();
    core_settings_m.roam_metrics().clear_metrics();

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
core_error_e core_server_c::get_uapsd_settings(
    core_uapsd_settings_s& settings )
    {
    DEBUG( "core_server_c::get_uapsd_settings()" );

    settings = core_settings_m.uapsd_settings();

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_error_e core_server_c::set_uapsd_settings(
    const core_uapsd_settings_s& settings )
    {
    DEBUG( "core_server_c::set_uapsd_settings()" );

    core_operation_base_c* command = new core_operation_set_uapsd_settings_c(
        REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m, settings );

    queue_int_operation_and_run_next( command );

    return core_error_ok;  
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
core_error_e core_server_c::get_power_save_settings(
    core_power_save_settings_s& settings )
    {
    DEBUG( "core_server_c::get_power_save_settings()" );

    settings = core_settings_m.power_save_settings();

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
core_error_e core_server_c::set_power_save_settings(
    const core_power_save_settings_s& settings )
    {
    DEBUG( "core_server_c::set_power_save_settings()" );

    core_operation_base_c* command = new core_operation_set_power_save_settings_c(
        REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m, settings );

    queue_int_operation_and_run_next( command );

    return core_error_ok;  
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::get_current_ap_info(
    core_ap_information_s& info )
    {
    DEBUG( "core_server_c::get_current_ap_info()" );

    if ( core_settings_m.is_connected() &&
         connection_data_m &&
         connection_data_m->current_ap_data() )
        {
        info = core_tools_parser_c::get_ap_info(
            connection_data_m->iap_data(),
            *connection_data_m->current_ap_data() );

        return core_error_ok;
        }

    return core_error_not_connected;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::set_enabled_features(
    u32_t features )
    {
    DEBUG( "core_server_c::set_enabled_features()" );

    core_settings_m.set_enabled_features( features );
    
    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::get_roam_metrics(
    core_roam_metrics_s& roam_metrics )
    {
    DEBUG( "core_server_c::get_roam_metrics()" );

    core_roam_metrics_c& metrics = get_core_settings().roam_metrics();

    roam_metrics.connection_attempt_total_count = metrics.roam_attempt_count( core_roam_reason_initial_connect );
    roam_metrics.unsuccesfull_connection_attempt_count = 
          metrics.roam_attempt_failed_count( core_roam_failed_reason_timeout )
        + metrics.roam_attempt_failed_count( core_roam_failed_reason_ap_status_code )
        + metrics.roam_attempt_failed_count( core_roam_failed_reason_eapol_failure )
        + metrics.roam_attempt_failed_count( core_roam_failed_reason_other_failure );

    roam_metrics.roaming_counter = metrics.roam_success_count();
    roam_metrics.coverage_loss_count = metrics.roam_attempt_count( core_roam_reason_bss_lost );

    roam_metrics.last_roam_total_duration = metrics.roam_total_delay() / MILLISECONDS_FROM_MICROSECONDS;
    roam_metrics.last_roam_data_path_broken_duration = metrics.roam_total_delay() / MILLISECONDS_FROM_MICROSECONDS;

    roam_metrics.last_roam_reason = core_roam_reason_none;
    if ( get_connection_data() )
        {
        roam_metrics.last_roam_reason = get_connection_data()->last_roam_reason();
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::set_arp_filter(
    const core_arp_filter_s& filter )
    {
    DEBUG( "core_server_c::set_arp_filter()" );

    core_operation_base_c* command = new core_operation_set_arp_filter_c(
        REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m, filter );

    queue_int_operation_and_run_next( command );

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::configure_multicast_group(
    bool_t join_group,
    const core_mac_address_s& multicast_addr )
    {
    DEBUG( "core_server_c::configure_multicast_group" );

    core_operation_base_c* command = new core_operation_configure_multicast_group_c(
        REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m, join_group, multicast_addr );

    queue_int_operation_and_run_next( command );

    return core_error_ok;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_server_c::get_current_ac_traffic_info(
    core_ac_traffic_information_s& info )
    {
    DEBUG( "core_server_c::get_current_ac_traffic_info()" );

    if ( core_settings_m.is_connected() &&
         connection_data_m &&
         connection_data_m->current_ap_data() )
        {
        info.status_for_voice = connection_data_m->ac_traffic_status(
            core_access_class_voice );
        info.status_for_video = connection_data_m->ac_traffic_status(
            core_access_class_video );
        info.status_for_best_effort = connection_data_m->ac_traffic_status(
            core_access_class_best_effort );
        info.status_for_background = connection_data_m->ac_traffic_status(
            core_access_class_background );
        info.mode_for_voice = connection_data_m->ac_traffic_mode(
            core_access_class_voice );
        info.mode_for_video = connection_data_m->ac_traffic_mode(
            core_access_class_video );
        info.mode_for_best_effort = connection_data_m->ac_traffic_mode(
            core_access_class_best_effort );
        info.mode_for_background = connection_data_m->ac_traffic_mode(
            core_access_class_background );

        return core_error_ok;
        }

    return core_error_not_connected;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::request_complete(
    u32_t /* request_id */,
    core_error_e status )
    {
    DEBUG( "core_server_c::request_complete()" );
    
    core_operation_base_c* operation = queue_m.first();
    
    ASSERT( operation );
    ASSERT( operation->is_executing() );

    core_error_e ret = operation->continue_operation( status );
    if ( ret == core_error_request_pending )
        {
        DEBUG( "core_server_c::request_complete() - operation is not done yet" );
        return;
        }

    DEBUG1( "core_server_c::request_complete() - operation completed with code %u",
        ret );

    if ( operation->request_id() != REQUEST_ID_CORE_INTERNAL )
        {
        callback_m.request_complete(
            operation->request_id(),
            ret );
        }

    queue_m.remove( operation );
    delete operation;
    operation = NULL;

    schedule_operation();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::cancel_request(
    u32_t request_id )
    {
    DEBUG( "core_server_c::cancel_request()" );
    
    // Find correct operation
    core_operation_base_c* operation = queue_m.first();
    while ( operation )
        {
        if ( operation->request_id() == request_id )
            {
            break;
            }
        operation = queue_m.next();
        }
    
    if ( !operation )
        {
        DEBUG( "core_server_c::cancel_request() - operation not found" );
        return;
        }
    
    if ( operation->is_executing() )
        {
        // Let operation handle it's own cancelling
        operation->user_cancel_operation( true_t ); // Using graceful cancel
        }
    else
        {
        // When operation is not yet running, we don't need to ask for it's permissions...
        queue_m.remove( operation );
        delete operation;
        operation = NULL;
        
        // Use null operation for completing correctly
        operation = new core_operation_null_c(
            request_id, this, &drivers_m, &callback_m, core_error_cancel );
        queue_ext_operation_and_run_next( operation, request_id );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  
void core_server_c::receive_frame(
    core_frame_type_e frame_type,
    const u16_t frame_length,
    const u8_t* const frame_data,
    u8_t frame_rcpi )
    {
    DEBUG( "core_server_c::receive_frame()" );

    DEBUG1( "core_server_c::receive_frame() - frame type: %u", frame_type );
    DEBUG1( "core_server_c::receive_frame() - frame (%u bytes): ",
        frame_length );
    DEBUG_BUFFER(
        frame_length,
        frame_data );

    if ( frame_type == core_frame_type_ethernet )
        {
        core_frame_ethernet_c* frame = core_frame_ethernet_c::instance(
            frame_length,
            frame_data,
            false_t );
        if ( frame )             
            {
            /**
             * Currently all Ethernet frames are for EAPOL.
             */

            /**
             * If an Ethernet frame is received during a roam, store it
             * for later use.
             */
            if ( get_connection_data()->is_eapol_connecting() )
                {
                DEBUG( "core_server_c::receive_frame() - (re-)association in progress, storing EAPOL frame" );

                (void)eapol_m->store_frame( *frame );
                }
            /**
             * Frames are discarded during disconnect.
             */
            else if ( get_connection_data()->is_disconnecting() )
                {
                DEBUG( "core_server_c::receive_frame() - disconnect in progress, discarding EAPOL frame" );
                }
            /**
             * Otherwise send it to EAPOL for processing.
             */
            else if ( eapol_m )
                {
                // Clear stored frame so that it is not sent later.
                eapol_m->clear_stored_frame();
                
                (void)eapol_m->process_frame( *frame );
                }
            else
                {
                DEBUG( "core_server_c::receive_frame() - EAPOL not instantiated, discarding EAPOL frame" );
                }

            delete frame;
            frame = NULL;
            }

        return;
        }

    /**
     * WPX frames are handled separately.
     */
    if ( wpx_adaptation_m->handle_wpx_frame(
            frame_type,
            frame_length,
            frame_data ) )
        {
        return;
        }

    /**
     * Offer dot11 and echo test frames to the registered frame handler.
     */
    bool_t is_handled( false_t );

    if ( frame_type == core_frame_type_dot11 &&
         frame_handler_m )
        {                
        core_frame_dot11_c* frame = core_frame_dot11_c::instance(
            frame_length,
            frame_data,
            false_t );
        if ( frame &&
             frame_handler_m->receive_frame( frame, frame_rcpi ) )
            {
            DEBUG( "core_server_c::receive_frame() - dot11 frame handled by the operation" );

            is_handled = true_t;
            }

        delete frame;
        frame = NULL;
        }
    else if ( frame_type == core_frame_type_test &&
              frame_handler_m )
        {
        core_frame_ethernet_c* frame = core_frame_ethernet_c::instance(
            frame_length,
            frame_data,
            false_t );
        if ( frame )
            {
            core_frame_echo_test_c* test_frame = core_frame_echo_test_c::instance(
                *frame );
            if ( test_frame &&
                 frame_handler_m->receive_test_frame( test_frame, frame_rcpi ) )
                {
                DEBUG( "core_server_c::receive_frame() - echo test frame handled by the operation" );

                is_handled = true_t;                
                }

            delete test_frame;
            test_frame = NULL;

            delete frame;
            frame = NULL;            
            }
        }

    /**
     * All other frames are processed in an operation.
     */
    if ( !is_handled )
        {
        DEBUG( "core_server_c::receive_frame() - queueing the frame for processing" );

        core_operation_base_c* command = new core_operation_handle_frame_c(
            REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m,
            frame_type, frame_length, frame_data );

        queue_int_operation_and_run_next( command );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
void core_server_c::notify(
    core_am_indication_e indication )
    {
    DEBUG( "core_server_c::notify()" );

#ifdef _DEBUG
    switch ( indication )
        {
        case core_am_indication_wlan_media_disconnect:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_media_disconnect" );
            break;
        case core_am_indication_wlan_hw_failed:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_hw_failed" );
            break;
        case core_am_indication_wlan_beacon_lost:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_beacon_lost" );
            break;
        case core_am_indication_wlan_power_mode_failure:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_power_mode_failure" );
            break;
        case core_am_indication_wlan_tx_fail:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_tx_fail" );
            break;
        case core_am_indication_wlan_bss_regained:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_bss_regained" );
            break;
        case core_am_indication_wlan_wep_decrypt_failure:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_wep_decrypt_failure" );
            break;
        case core_am_indication_wlan_pairwise_key_mic_failure:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_pairwise_key_mic_failure" );
            break;
        case core_am_indication_wlan_group_key_mic_failure: 
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_group_key_mic_failure" );
            break;
        case core_am_indication_wlan_scan_complete:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_scan_complete" );
            break;
        case core_am_indication_wlan_rcpi_trigger:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_rcpi_trigger" );
            break;
        case core_am_indication_wlan_signal_loss_prediction:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_signal_loss_prediction" );
            break;
        case core_am_indication_wlan_power_save_test_trigger:
            DEBUG( "core_server_c::notify() - core_am_indication_wlan_power_save_test_trigger" );
            break;       
        case core_am_indication_os_power_standby:
            DEBUG( "core_server_c::notify() - core_am_indication_os_power_standby" );
            break;
        case core_am_indication_bt_connection_established:
            DEBUG( "core_server_c::notify() - core_am_indication_bt_connection_established" );
            break;
        case core_am_indication_bt_connection_disconnected:
            DEBUG( "core_server_c::notify() - core_am_indication_bt_connection_disconnected" );
            break;
        case core_am_indication_voice_call_on:
            DEBUG( "core_server_c::notify() - core_am_indication_voice_call_on" );
            break;
        case core_am_indication_voice_call_off:
            DEBUG( "core_server_c::notify() - core_am_indication_voice_call_off" );
            break;
        default:
            break;
        }
#endif // _DEBUG

    if ( event_handler_m &&
         event_handler_m->notify( indication ) )
        {
        DEBUG( "core_server_c::notify() - indication handled by the operation" );

        return;
        }

    switch ( indication )
        {
        case core_am_indication_os_power_standby:
            {
            /**
             * Cancel all operations including the currently executing one.
             */
            cancel_all_operations( true_t );

            /**
             * Force an immediate driver unload.
             */ 
            queue_unload_drivers();

            break;
            }
        case core_am_indication_wlan_hw_failed:
            {
            /**
             * Cancel all operations including the currently executing one.
             */
            cancel_all_operations( false_t );

            /**
             * Force an immediate driver unload.
             */
            queue_unload_drivers();

            break;
            }
        case core_am_indication_wlan_media_disconnect:
            {
            /**
             * If the current operation is connect, handle_bss_lost or protected_setup, the indication
             * is silently ignored. Otherwise this indication is handled like a BSS lost.
             */
            core_operation_base_c* op = queue_m.first();
            if( op &&
                op->is_executing() &&
                ( op->operation_type() == core_operation_handle_bss_lost ||
                  op->operation_type() == core_operation_connect ||
                  op->operation_type() == core_operation_protected_setup ) )
                {
                DEBUG( "core_server_c::notify() - ignoring indication" );
                }
            else
                {
                schedule_roam(
                    core_operation_handle_bss_lost_c::core_bss_lost_reason_media_disconnect );
                }

            break;
            }
        case core_am_indication_wlan_beacon_lost:
            /** Falls through on purpose. */
        case core_am_indication_wlan_power_mode_failure:
            /** Falls through on purpose. */
        case core_am_indication_wlan_tx_fail:
            {
            // bss_lost is only handled in infrastructure mode
            if ( connection_data_m->iap_data().operating_mode() == core_operating_mode_infrastructure )
                {
                // send an indication to adaptation
                callback_m.notify(
                    core_notification_bss_lost,
                    0, 
                    NULL );

                schedule_roam(
                    core_operation_handle_bss_lost_c::core_bss_lost_reason_bss_lost );
                }

            break;
            }
        case core_am_indication_wlan_pairwise_key_mic_failure:
            {
            DEBUG( "core_server_c::notify() - a pairwise MIC failure has occured" );

            mic_failure( false_t );
            
            break;
            }
        case core_am_indication_wlan_group_key_mic_failure:
            {
            DEBUG( "core_server_c::notify() - a group MIC failure has occured" );

            mic_failure( true_t );

            break;
            }
        case core_am_indication_wlan_bss_regained:
            // Do nothing.
            break;
        case core_am_indication_wlan_wep_decrypt_failure:
            // Do nothing.
            break;
        case core_am_indication_bt_connection_established:
            {
            core_settings_m.set_bt_connection_established( true_t );
            core_operation_base_c* command = new core_operation_update_rxtx_parameters_c(
                REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m );

            queue_int_operation_and_run_next( command );

            break;
            }
        case core_am_indication_bt_connection_disconnected:
            {
            core_settings_m.set_bt_connection_established( false_t );
            core_operation_base_c* command = new core_operation_update_rxtx_parameters_c(
                REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m );

            queue_int_operation_and_run_next( command );

            break;
            }
        case core_am_indication_wlan_scan_complete:
            {
            DEBUG( "core_server_c::notify() - scan complete received without pending scan operations" );
            ASSERT( false_t );

            break;
            }
        case core_am_indication_wlan_rcpi_trigger:
            {
            if ( !is_operation_in_queue_with_type( core_operation_check_rcpi ) && 
                 !is_operation_in_queue_with_type( core_operation_handle_bss_lost ) )
                {
                // send an indication to adaptation
                callback_m.notify(
                    core_notification_rcpi_roam_attempt_started,
                    0, 
                    NULL );

                // create operation to handle the indication
                core_operation_base_c* command = new core_operation_check_rcpi_c(
                    REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m, core_operation_check_rcpi_c::core_rcpi_check_reason_rcpi_trigger );

                queue_int_operation_and_run_next( command );
                }
            else
                {
                DEBUG( "core_server_c::notify() - roaming operation already in queue " );
                }

            break;
            }
        case core_am_indication_wlan_signal_loss_prediction:
            {
            if ( !is_operation_in_queue_with_type( core_operation_check_rcpi ) && 
                 !is_operation_in_queue_with_type( core_operation_handle_bss_lost ) )
                {
                // send an indication to adaptation
                /*
                callback_m.notify(
                    core_notification_rcpi_roam_attempt_started,
                    0, 
                    NULL );
                 */

                // create operation to handle the indication
                /*
                core_operation_base_c* command = new core_operation_check_rcpi_c(
                    REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m, core_operation_check_rcpi_c::core_rcpi_check_reason_signal_loss_prediction );

                queue_int_operation_and_run_next( command, LIST_HIGH_PRIORITY );
                 */
                }
            else
                {
                DEBUG( "core_server_c::notify() - roaming operation already in queue " );
                }

            break;
            }
        case core_am_indication_wlan_power_save_test_trigger:
            {
            DEBUG( "core_server_c::notify() - scheduling a power save test" );

            core_operation_base_c* command = new core_operation_power_save_test_c(
                REQUEST_ID_CORE_INTERNAL,
                this,
                &drivers_m,
                &callback_m );

            queue_int_operation_and_run_next( command );

            break;
            }
        case core_am_indication_voice_call_on:
            {
            DEBUG( "core_server_c::notify() - voice call is on" );
            connection_data_m->set_voice_call_state( true_t );
            break;
            }
        case core_am_indication_voice_call_off:
            {
            DEBUG( "core_server_c::notify() - voice call is off" );
            connection_data_m->set_voice_call_state( false_t );
            break;
            }
        case core_am_indication_wlan_ap_ps_mode_error:
            // Do nothing.
            break;
        default:
            DEBUG( "core_server_c::notify() - unknown indication" );
            ASSERT( false_t );
            break;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
void core_server_c::schedule_operation()
    {
    DEBUG( "core_server_c::schedule_operation()" );
    
    core_operation_base_c* operation = queue_m.first();

    DEBUG1( "core_server_c::schedule_operation() - %u operation(s) in the queue",
        queue_m.count());

#ifdef _DEBUG
    if( operation && operation->is_executing() )
        {
        DEBUG2("core_server_c::schedule_operation() - operation 0x%08X (type %u) is executing",
            operation, operation->operation_type() );
        }
#endif // _DEBUG

    if( !operation &&
        core_settings_m.connection_state() == core_connection_state_notconnected &&
        core_settings_m.is_driver_loaded() &&
        !driver_unload_timer_m->is_active() )
        {
        // If 1) no more operations and 
        // 2) no connection exists and
        // 3) drivers are loaded and
        // 4) driver not already active,
        // start unload timer
        DEBUG( "core_server_c::schedule_operation() - starting unload timer" );
        schedule_unload_timer(
            device_settings_m.unload_driver_timer * SECONDS_FROM_MICROSECONDS );

        return;
        }

    if ( operation &&
         !operation->is_executing() &&
         !queue_timer_m->is_active() )
        {
        DEBUG( "core_server_c::schedule_operation() - scheduling a new operation" );
        queue_timer_m->start( CORE_TIMER_IMMEDIATELY );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// 
bool_t core_server_c::is_cm_active()
    {
    return !cm_timer_m.is_wpa_allowed();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::queue_ext_operation_and_run_next(
    core_operation_base_c* operation,
    u32_t request_id )
    {
    DEBUG1( "core_server_c::queue_ext_operation_and_run_next() - request id %u",
        request_id );
    if( !operation )
        {
        DEBUG( "core_server_c::queue_ext_operation_and_run_next() - no operation, completing request." );
        callback_m.request_complete( request_id, core_error_no_memory );
        }
    else if ( operation->is_flags( core_operation_base_c::core_base_flag_only_one_instance ) &&
        is_operation_in_queue_with_type( operation->operation_type() ) )
        {
        DEBUG( "core_server_c::queue_ext_operation_and_run_next() - only one instance allowed in the operation queue, completing request" );
        callback_m.request_complete( request_id, core_error_ok );

        delete operation;
        operation = NULL;
        }
    else
        {
        core_error_e ret = queue_m.append( operation );
        if ( ret != core_error_ok )
            {
            DEBUG1( "core_server_c::queue_ext_operation_and_run_next() - unable to queue the request (%u)",
                ret );
            callback_m.request_complete( request_id, ret );

            delete operation;
            operation = NULL;
            }
        }
 
    schedule_operation();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
void core_server_c::queue_timer_expired( void* this_ptr )
    {
    DEBUG( "core_server_c::queue_timer_expired()" );    
    core_server_c* self = static_cast<core_server_c*>( this_ptr );
    
    core_operation_base_c* operation = self->queue_m.first();
    if( !operation )
        {
        /**
         * This shouldn't normally happen but it can happen if we have
         * schedule an operation and cancel it before it has a chance to
         * start executing.
         */

        return;
        }

    ASSERT( operation );
    ASSERT( !operation->is_executing() );

    // Set operation priority in the queue to maximum
    self->queue_m.remove( operation );
    self->queue_m.append( operation, LIST_TOP_PRIORITY );
    
    // If operation says it needs drivers, stop driver unload timer
    if( operation->is_flags( core_operation_base_c::core_base_flag_drivers_needed ) &&
        self->driver_unload_timer_m->is_active() )
        {
        DEBUG( "core_server_c::queue_timer_expired() - stopping unload timer" );
        self->cancel_unload_timer();
        }
    
    core_error_e ret = operation->start_operation();
    if( ret == core_error_request_pending )
        {
        DEBUG( "core_server_c::queue_timer_expired() - operation is not done yet" );
        return;
        }

    DEBUG1( "core_server_c::queue_timer_expired() - operation completed with code %u",
        ret );

    if ( operation->request_id() != REQUEST_ID_CORE_INTERNAL )
        {
        self->callback_m.request_complete(
            operation->request_id(),
            ret );
        }    

    self->queue_m.remove( operation );
    delete operation;
    operation = NULL;

    self->schedule_operation();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
void core_server_c::unload_timer_expired( void* this_ptr )
    {
    DEBUG("core_server_c::unload_timer_expired()");
    core_server_c* self = static_cast<core_server_c*>( this_ptr );

    /**
     * Due to configurable unload timer value and different platform timing issues
     * it's possible that a new operation has been added to the operation queue
     * AFTER this timer was started but the queue timer has not yet been triggered.
     * 
     * Therefore we have to abort driver unloading if there's an operation in the
     * queue that requires drivers.
     */
    if( self->is_operation_in_queue_with_flags(
        core_operation_base_c::core_base_flag_drivers_needed ) )
        {
        DEBUG( "core_server_c::unload_timer_expired() - operation in queue, aborting driver unloading" );

        return;
        }

    self->queue_unload_drivers();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::mic_failure(
    bool_t is_group_key_fail )
    {
    DEBUG( "core_server_c::mic_failure()" );
    
    core_connection_security_mode_e mode = core_connection_security_mode_open;
    
    if( core_error_ok != get_current_security_mode( mode ) )
        {
        DEBUG( "core_server_c::mic_failure() - not connected, ignoring mic failure" );
        return;
        }
    
    if( mode != core_connection_security_mode_wpa && 
        mode != core_connection_security_mode_wpa_psk )
        {
        DEBUG( "core_server_c::mic_failure() - security mode not WPA, ignoring mic failure" );
        return;
        }

    if( is_group_key_fail )
        {
        if( connection_data_m->current_ap_data() != NULL &&
            connection_data_m->current_ap_data()->best_group_cipher() != core_cipher_suite_tkip )
            {
            DEBUG( "core_server_c::mic_failure() - TKIP not the best group cipher, ignoring mic failure" );
            return;
            }
        }
    else
        {
        if( connection_data_m->current_ap_data() != NULL &&
            connection_data_m->current_ap_data()->best_pairwise_cipher() != core_cipher_suite_tkip )
            {
            DEBUG( "core_server_c::mic_failure() - TKIP not the best pairwise cipher, ignoring mic failure" );
            return;
            }
        }
        
    const core_mac_address_s bssid(
        connection_data_m->current_ap_data()->bssid() );

    cm_timer_m.mic_failure();

    network_id_c network_id(
        const_cast<u8_t*>( &bssid.addr[0] ),
        MAC_ADDR_LEN,
        &own_mac_addr_m.addr[0],
        MAC_ADDR_LEN,
        eapol_m->ethernet_type() );

    ASSERT( eapol_m );
    (void)eapol_m->tkip_mic_failure(
        &network_id,
        !cm_timer_m.is_wpa_allowed() ? true_t : false_t,
        is_group_key_fail ? wlan_eapol_if_eapol_tkip_mic_failure_type_group_key :
            wlan_eapol_if_eapol_tkip_mic_failure_type_pairwise_key );


    if ( !cm_timer_m.is_wpa_allowed() )
        {
        DEBUG( "core_server_c::mic_failure() - two MIC failures within 60 seconds, disconnecting" );
                
        core_operation_base_c* command = new core_operation_release_c(
            REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m, core_release_reason_tkip_mic_failure );

        queue_int_operation_and_run_next( command );
        }        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_server_c::queue_unload_drivers()
    {
    DEBUG( "core_server_c::queue_unload_drivers()" );

    core_operation_base_c* command = new core_operation_unload_drivers_c(
        REQUEST_ID_CORE_INTERNAL, this, &drivers_m, &callback_m );

    queue_int_operation_and_run_next( command );    
    }
