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
* Description:  Statemachine for unloading drivers
*
*/

/*
* %version: 11 %
*/

#include "core_operation_unload_drivers.h"
#include "core_operation_release.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_unload_drivers_c::core_operation_unload_drivers_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation ) :
    core_operation_base_c( core_operation_unload_drivers, request_id, server, drivers, adaptation,
        core_base_flag_only_one_instance )   
    {
    DEBUG( "core_operation_unload_drivers_c::core_operation_unload_drivers_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_unload_drivers_c::~core_operation_unload_drivers_c()
    {
    DEBUG( "core_operation_unload_drivers_c::~core_operation_unload_drivers_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_unload_drivers_c::next_state()
    {
    DEBUG( "core_operation_unload_drivers_c::next_state()" );
    switch( operation_state_m )
        {
        case core_state_init:
            {
            DEBUG( "- core_state_init" );
            if( server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_unload_drivers_c::next_state() - still connected, notifying state change" );
                // connection is open, first send state change notification
                u8_t buf[5];
                buf[0] = static_cast<u8_t>( core_connection_state_notconnected );
                buf[1] = static_cast<u8_t>( core_release_reason_hw_failure );
                adaptation_m->notify( 
                    core_notification_connection_state_changed,
                    sizeof( buf ),
                    buf );

                server_m->get_connection_data()->set_last_connection_state(
                    core_connection_state_notconnected );

                // Delay unloading a little bit so that data pipe can be
                // disconnected from adaptation side
                return asynch_goto( core_state_notifying, DELAY_FOR_AGENT );
                }
            else
                {               
                // not connected, jump directly to unloading drivers
                return goto_state( core_state_releasing );
                }
            }
        case core_state_notifying:
            {
            DEBUG( "core_operation_connect_c::next_state() - notification sent, releasing" );
            core_operation_base_c* operation = new core_operation_release_c(
                request_id_m,
                server_m,
                drivers_m,
                adaptation_m,
                core_release_reason_hw_failure );
            operation_state_m = core_state_releasing;
            return run_sub_operation( operation );
            }
        case core_state_releasing:
            {
            DEBUG( "core_operation_unload_drivers_c::next_state() - unloading drivers" );
            drivers_m->unload_drivers( request_id_m );
            operation_state_m = core_state_unloading;
            break;
            }
        case core_state_unloading:
            {
            DEBUG( "core_operation_unload_drivers_c::next_state() - unloading done" );
            server_m->get_core_settings().set_driver_state( false_t );
            return core_error_ok;    
            } 
        default:
            {
            ASSERT( false_t );
            }
        }
    return core_error_request_pending;
    }
