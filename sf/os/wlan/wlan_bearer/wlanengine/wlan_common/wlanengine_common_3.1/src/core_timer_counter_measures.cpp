/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Timer for handling WPA TKIP counter measures
*
*/


#include "core_timer_factory.h"
#include "core_timer_counter_measures.h"
#include "core_callback.h"
#include "am_debug.h"

/** Wait time for the next secure connection failure (60 sec). */
const uint_t CORE_CM_FAIL_WAIT_TIME = 60000000;

/** Blocking time for WPA connections (60 sec). */
const uint_t CORE_CM_BLOCKING_TIME = 60000000;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
core_timer_counter_measures_c::core_timer_counter_measures_c() :
    state_m( core_state_not_running ),
    timer_m( NULL )
    {
    DEBUG( "core_timer_counter_measures_c::core_timer_counter_measures_c()" );

    core_callback_c* timer_callback = 
        new core_callback_c( &(core_timer_counter_measures_c::timer_expired), this );
    if( !timer_callback )
        {
        DEBUG( "core_timer_counter_measures_c::core_timer_counter_measures_c() - unable to create callbacks" );
        }

    timer_m = core_timer_factory_c::create_timer( timer_callback );
    if( !timer_m )
        {
        delete timer_callback;
        DEBUG( "core_timer_counter_measures_c::core_timer_counter_measures_c() - unable to create timer" );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
core_timer_counter_measures_c::~core_timer_counter_measures_c()
    {
    DEBUG( "core_timer_counter_measures_c::~core_timer_counter_measures_c()" );

    if ( timer_m )
        {
        timer_m->stop();
        core_timer_factory_c::destroy_timer( timer_m );      
        }    
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
bool_t core_timer_counter_measures_c::is_wpa_allowed()
    {
    return ( state_m != core_state_blocked );
    }
    
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void core_timer_counter_measures_c::mic_failure()
    {
    DEBUG( "core_timer_counter_measures_c::mic_failure()" );

    switch ( state_m )
        {
        case core_state_not_running:
            DEBUG( "core_timer_counter_measures_c::mic_failure() - new state is core_state_activated" );
            state_m = core_state_activated;
            if ( timer_m )
                {
                timer_m->start( CORE_CM_FAIL_WAIT_TIME );
                }
            break;
        case core_state_activated:
            DEBUG( "core_timer_counter_measures_c::mic_failure() - new state is core_state_blocked" );
            state_m = core_state_blocked;
            if ( timer_m )
                {
                timer_m->stop();
                timer_m->start( CORE_CM_BLOCKING_TIME );
                }
            break;
        default:
            DEBUG( "core_timer_counter_measures_c::mic_failure() - indication in wrong state" );
            ASSERT( false_t );
            break;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void core_timer_counter_measures_c::timer_expired(
    void* this_ptr )
    {
    DEBUG( "core_timer_counter_measures_c::timer_expired()" );

    core_timer_counter_measures_c* self =
        static_cast<core_timer_counter_measures_c*>( this_ptr );

    switch ( self->state_m )
        {
        case core_state_activated:
            /** Falls through on purpose. */
        case core_state_blocked:
            DEBUG( "core_timer_counter_measures_c::timer_expired() - new state is core_state_not_running" );
            self->state_m = core_state_not_running;
            break;
        default:
            DEBUG( "core_timer_counter_measures_c::timer_expired() - expiration in wrong state" );
            ASSERT( false_t );
            break;
        }
    }
