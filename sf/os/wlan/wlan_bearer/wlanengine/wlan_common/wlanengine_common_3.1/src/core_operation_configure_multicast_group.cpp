/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for configuring multicast address to drivers
*
*/

/*
* %version: 7 %
*/

#include "core_operation_configure_multicast_group.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_configure_multicast_group_c::core_operation_configure_multicast_group_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    bool_t join_group,
    const core_mac_address_s& multicast_addr ):
    core_operation_base_c( core_operation_configure_multicast_group, request_id, server, drivers, adaptation,
        core_base_flag_drivers_needed | core_base_flag_connection_needed ),
    is_join_m( join_group ),
    multicast_addr_m( multicast_addr )
    {
    DEBUG( "core_operation_configure_multicast_group_c::core_operation_configure_multicast_group_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_configure_multicast_group_c::~core_operation_configure_multicast_group_c()
    {
    DEBUG( "core_operation_configure_multicast_group_c::~core_operation_configure_multicast_group_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_configure_multicast_group_c::next_state()
    {
    DEBUG( "core_operation_configure_multicast_group_c::next_state()" );
    
    switch ( operation_state_m )
        {
        case core_state_init:
            {                        
            operation_state_m = core_state_req_configure_multicast_group;

            if ( !server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_configure_multicast_group_c::next_state() - not connected, nothing to do" );

                return core_error_general;
                }

            drivers_m->configure_multicast_group(
                request_id_m,
                is_join_m,
                multicast_addr_m );

            break;
            }
        case core_state_req_configure_multicast_group:
            {
            DEBUG( "core_operation_configure_multicast_group_c::next_state() -  DONE" );
            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }
