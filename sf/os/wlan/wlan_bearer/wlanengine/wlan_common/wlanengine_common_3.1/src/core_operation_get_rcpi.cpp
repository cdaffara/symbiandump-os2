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
* Description:  Statemachine for getting the RCPI value of the current connection
*
*/


#include "core_operation_get_rcpi.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_get_rcpi_c::core_operation_get_rcpi_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    u32_t& rcpi ) :
    core_operation_base_c( core_operation_get_rcpi, request_id, server, drivers, adaptation,
        core_base_flag_none ),
    current_rcpi_m( rcpi )
    {
    DEBUG( "core_operation_get_rcpi_c::core_operation_get_rcpi_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_get_rcpi_c::~core_operation_get_rcpi_c()
    {
    DEBUG( "core_operation_get_rcpi_c::~core_operation_get_rcpi_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_get_rcpi_c::next_state()
    {
    DEBUG( "core_operation_get_rcpi_c::next_state()" );
    
    switch ( operation_state_m )
        {
        case core_state_init:
            {
            DEBUG( "core_operation_get_rcpi_c::next_state() - requesting current RCPI value" );
            
            if( server_m->get_core_settings().is_connected() )
                {
                drivers_m->get_current_rcpi(
                    request_id_m,
                    current_rcpi_m );
                operation_state_m = core_state_req_rcpi;
                break;
                }
            else
                {
                return core_error_not_supported;
                }
            }
        case core_state_req_rcpi:
            {
            DEBUG1( "core_operation_get_rcpi_c::next_state() - current RCPI is %u",
                current_rcpi_m );

            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }
