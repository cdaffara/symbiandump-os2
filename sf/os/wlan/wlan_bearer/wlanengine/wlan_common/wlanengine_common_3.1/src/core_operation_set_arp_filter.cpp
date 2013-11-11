/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State machine for updating arp filter
*
*/


#include "core_operation_set_arp_filter.h"
#include "core_server.h"
#include "core_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_set_arp_filter_c::core_operation_set_arp_filter_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    const core_arp_filter_s& filter ) :
    core_operation_base_c( core_operation_set_arp_filter, request_id, server, 
        drivers, adaptation, core_base_flag_none ),
    arp_filter_m( filter )
    {
    DEBUG( "core_operation_set_arp_filter_c::core_operation_set_arp_filter_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_set_arp_filter_c::~core_operation_set_arp_filter_c()
    {
    DEBUG( "core_operation_set_arp_filter_c::~core_operation_set_arp_filter_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_set_arp_filter_c::next_state()
    {
    DEBUG( "core_operation_set_arp_filter_c::next_state()" );
    
    switch ( operation_state_m )
        {
        case core_state_init:
            {

            if ( !server_m->get_core_settings().is_driver_loaded() )
                {
                DEBUG( "core_operation_set_arp_filter_c::next_state() - driver not loaded, enabling not possible, completing operation" );

                return core_error_ok;
                }

            operation_state_m = core_state_set_arp_filter;

            if ( arp_filter_m.enable_filter )
                {
                DEBUG(  "core_operation_set_arp_filter_c::next_state() - setting ARP IP filter" );  
                DEBUG4( "core_operation_set_arp_filter_c::next_state() - IP addr: %d.%d.%d.%d",
                        ( arp_filter_m.ipv4_addr & 0x000000ff ),  
                        ( arp_filter_m.ipv4_addr & 0x0000ff00 ) >>  8,
                        ( arp_filter_m.ipv4_addr & 0x00ff0000 ) >> 16,
                        ( arp_filter_m.ipv4_addr & 0xff000000 ) >> 24 );
                }
            else
                {
                DEBUG( "core_operation_set_arp_filter_c::next_state() - disabling ARP IP filter" );  
                }

            drivers_m->set_arp_filter( request_id_m, arp_filter_m );

            break;
            }
        case core_state_set_arp_filter:
            {
            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }
