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
* Description:  Statemachine for settings static WEP keys
*
*/


#include "core_sub_operation_set_static_wep.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_set_static_wep_c::core_sub_operation_set_static_wep_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation ) :  
    core_operation_base_c( core_operation_unspecified, request_id, server, drivers, adaptation,
        core_base_flag_drivers_needed )
    {
    DEBUG( "core_sub_operation_set_static_wep_c::core_sub_operation_set_static_wep_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_sub_operation_set_static_wep_c::~core_sub_operation_set_static_wep_c()
    {
    DEBUG( "core_sub_operation_set_static_wep_c::~core_sub_operation_set_static_wep_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_sub_operation_set_static_wep_c::next_state()
    {
    DEBUG( "core_sub_operation_set_static_wep_c::next_state()" );

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            operation_state_m = core_state_req_set_key1;

            if ( server_m->get_connection_data()->iap_data().wep_key( WEP_KEY1 ).key_length )
                {
                DEBUG( "core_sub_operation_set_static_wep_c::next_state() - setting WEP key #1" );

                drivers_m->add_cipher_key(
                    core_cipher_key_type_wep,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY1 ).key_index,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY1 ).key_length,
                    &server_m->get_connection_data()->iap_data().wep_key( WEP_KEY1 ).key_data[0],
                    BROADCAST_MAC_ADDR,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY1 ).key_index ==
                        server_m->get_connection_data()->iap_data().default_wep_key_index() );
                }
                
            if ( server_m->get_connection_data()->iap_data().wep_key( WEP_KEY2 ).key_length )
                {
                DEBUG( "core_sub_operation_set_static_wep_c::next_state() - setting WEP key #2" );

                drivers_m->add_cipher_key(
                    core_cipher_key_type_wep,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY2 ).key_index,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY2 ).key_length,
                    &server_m->get_connection_data()->iap_data().wep_key( WEP_KEY2 ).key_data[0],
                    BROADCAST_MAC_ADDR,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY2 ).key_index ==
                        server_m->get_connection_data()->iap_data().default_wep_key_index() );
                }

            if ( server_m->get_connection_data()->iap_data().wep_key( WEP_KEY3 ).key_length )
                {
                DEBUG( "core_sub_operation_set_static_wep_c::next_state() - setting WEP key #3" );

                drivers_m->add_cipher_key(
                    core_cipher_key_type_wep,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY3 ).key_index,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY3 ).key_length,
                    &server_m->get_connection_data()->iap_data().wep_key( WEP_KEY3 ).key_data[0],
                    BROADCAST_MAC_ADDR,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY3 ).key_index ==
                        server_m->get_connection_data()->iap_data().default_wep_key_index() );
                }

            if ( server_m->get_connection_data()->iap_data().wep_key( WEP_KEY4 ).key_length )
                {
                DEBUG( "core_sub_operation_set_static_wep_c::next_state() - setting WEP key #4" );

                drivers_m->add_cipher_key(
                    core_cipher_key_type_wep,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY4 ).key_index,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY4 ).key_length,
                    &server_m->get_connection_data()->iap_data().wep_key( WEP_KEY4 ).key_data[0],
                    BROADCAST_MAC_ADDR,
                    server_m->get_connection_data()->iap_data().wep_key( WEP_KEY4 ).key_index ==
                        server_m->get_connection_data()->iap_data().default_wep_key_index() );
                }

            DEBUG( "core_sub_operation_set_static_wep_c::next_state() - WEP keys set" );
            
            return core_error_ok;
            }
        default:
            {
            ASSERT( false_t );
            }
        }

    return core_error_request_pending;
    }
