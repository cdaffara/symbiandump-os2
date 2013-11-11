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
* Description:  Statemachine for setting the power save settings.
*
*/


#include "core_operation_set_power_save_settings.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_set_power_save_settings_c::core_operation_set_power_save_settings_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    const core_power_save_settings_s& settings ) :
    core_operation_base_c( core_operation_set_power_save_settings, request_id, server, drivers, adaptation,
        core_base_flag_none ),
    power_save_settings_m( settings )
    {
    DEBUG( "core_operation_set_power_save_settings_c::core_operation_set_power_save_settings_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_set_power_save_settings_c::~core_operation_set_power_save_settings_c()
    {
    DEBUG( "core_operation_set_power_save_settings_c::~core_operation_set_power_save_settings_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_set_power_save_settings_c::next_state()
    {
    DEBUG( "core_operation_set_power_save_settings_c::next_state()" );    

    switch ( operation_state_m )
        {
        case core_state_init:
            {
            server_m->get_core_settings().set_power_save_settings(
                power_save_settings_m );

            if ( !server_m->get_core_settings().is_connected() )
                {
                DEBUG( "core_operation_set_power_save_settings_c::next_state() - not connected, no reason to set anything" );

                return core_error_ok;
                }

            operation_state_m = core_state_set_power_save_settings;

            DEBUG( "core_operation_set_power_save_settings_c::next_state() - setting power save settings" );
            DEBUG1( "core_operation_set_power_save_settings_c::nex() - stay_in_uapsd_power_save_for_voice: %u",
                power_save_settings_m.stay_in_uapsd_power_save_for_voice );
            DEBUG1( "core_operation_set_power_save_settings_c::nex() - stay_in_uapsd_power_save_for_video: %u",
                power_save_settings_m.stay_in_uapsd_power_save_for_video );
            DEBUG1( "core_operation_set_power_save_settings_c::nex() - stay_in_uapsd_power_save_for_best_effort: %u",
                power_save_settings_m.stay_in_uapsd_power_save_for_best_effort );
            DEBUG1( "core_operation_set_power_save_settings_c::nex() - stay_in_uapsd_power_save_for_background: %u",
                power_save_settings_m.stay_in_uapsd_power_save_for_background );

            DEBUG1( "core_operation_set_power_save_settings_c::nex() - stay_in_legacy_power_save_for_voice: %u",
                power_save_settings_m.stay_in_legacy_power_save_for_voice );
            DEBUG1( "core_operation_set_power_save_settings_c::nex() - stay_in_legacy_power_save_for_video: %u",
                power_save_settings_m.stay_in_legacy_power_save_for_video );
            DEBUG1( "core_operation_set_power_save_settings_c::nex() - stay_in_legacy_power_save_for_best_effort: %u",
                power_save_settings_m.stay_in_legacy_power_save_for_best_effort );
            DEBUG1( "core_operation_set_power_save_settings_c::nex() - stay_in_legacy_power_save_for_background: %u",
                power_save_settings_m.stay_in_legacy_power_save_for_background );

            drivers_m->set_power_save_settings(
                request_id_m,
                power_save_settings_m );

            break;
            }
        case core_state_set_power_save_settings:
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
