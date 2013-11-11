/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Statemachine for getting the packet statistics.
*
*/


#include "core_operation_get_statistics.h"
#include "core_roam_metrics.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_get_statistics_c::core_operation_get_statistics_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_packet_statistics_s& statistics ) :
    core_operation_base_c( core_operation_get_statistics, request_id, server, drivers, adaptation,
        core_base_flag_none ),
    client_statistics_m( statistics )
    {
    DEBUG( "core_operation_get_statistics_c::core_operation_get_statistics_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_get_statistics_c::~core_operation_get_statistics_c()
    {
    DEBUG( "core_operation_get_statistics_c::~core_operation_get_statistics_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_get_statistics_c::next_state()
    {
    DEBUG( "core_operation_get_statistics_c::next_state()" );    

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            server_m->get_core_settings().roam_metrics().trace_current_roam_metrics();

            if ( !server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_connect_c::next_state() - not connected, returning cached packet statistics" );

                client_statistics_m = server_m->get_core_settings().connection_statistics();

                return core_error_ok;
                }

            operation_state_m = core_state_req_packet_statistics;

            DEBUG( "core_operation_get_statistics_c::next_state() - requesting packet statistics from drivers" );

            drivers_m->get_packet_statistics(
                request_id_m,
                current_statistics_m );        

            break;
            }
        case core_state_req_packet_statistics:
            {
            core_packet_statistics_s statistics =
                server_m->get_core_settings().connection_statistics();

            DEBUG( "core_operation_get_statistics_c::next_state() - previous packet statistics:" );
            DEBUG1( "core_operation_get_statistics_c::next_state() - tx_frames:           %u",
                statistics.tx_frames );
            DEBUG1( "core_operation_get_statistics_c::next_state() - rx_frames:           %u",
                statistics.rx_frames );
            DEBUG1( "core_operation_get_statistics_c::next_state() - tx_multicast_frames: %u",
                statistics.tx_multicast_frames );
            DEBUG1( "core_operation_get_statistics_c::next_state() - rx_multicast_frames: %u",
                statistics.rx_multicast_frames );
            DEBUG1( "core_operation_get_statistics_c::next_state() - fcs_errors:          %u",
                statistics.fcs_errors );
            DEBUG1( "core_operation_get_statistics_c::next_state() - tx_retries:          %u",
                statistics.tx_retries );
            DEBUG1( "core_operation_get_statistics_c::next_state() - tx_errors:           %u",
                statistics.tx_errors );

            server_m->get_core_settings().update_connection_statistics(
                current_statistics_m );

            client_statistics_m = server_m->get_core_settings().connection_statistics();

            DEBUG( "core_operation_get_statistics_c::next_state() - current packet statistics:" );
            DEBUG1( "core_operation_get_statistics_c::next_state() - tx_frames:           %u",
                client_statistics_m.tx_frames );
            DEBUG1( "core_operation_get_statistics_c::next_state() - rx_frames:           %u",
                client_statistics_m.rx_frames );
            DEBUG1( "core_operation_get_statistics_c::next_state() - tx_multicast_frames: %u",
                client_statistics_m.tx_multicast_frames );
            DEBUG1( "core_operation_get_statistics_c::next_state() - rx_multicast_frames: %u",
                client_statistics_m.rx_multicast_frames );
            DEBUG1( "core_operation_get_statistics_c::next_state() - fcs_errors:          %u",
                client_statistics_m.fcs_errors );
            DEBUG1( "core_operation_get_statistics_c::next_state() - tx_retries:          %u",
                client_statistics_m.tx_retries );
            DEBUG1( "core_operation_get_statistics_c::next_state() - tx_errors:           %u",
                client_statistics_m.tx_errors );

            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }
