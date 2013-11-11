/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of core_traffic_stream_list_c class.
*
*/

/*
* %version: 10 %
*/

#include "core_traffic_stream_list.h"
#include "core_tools.h"
#include "core_am_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_list_c::core_traffic_stream_list_c() :
    ts_list_m( )
    {
    DEBUG( "core_traffic_stream_list_c::core_traffic_stream_list_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_list_c::~core_traffic_stream_list_c()
    {
    DEBUG( "core_traffic_stream_list_c::~core_traffic_stream_list_c()" );

    for( entry_s* iter = ts_list_m.first(); iter; iter = ts_list_m.next() )
        {
        delete iter->traffic_stream;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_traffic_stream_list_c::count() const
    {
    return ts_list_m.count();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_c* core_traffic_stream_list_c::first()
    {
    entry_s* iter = ts_list_m.first();
    if ( iter )
        {
        return iter->traffic_stream;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_c* core_traffic_stream_list_c::next()
    {
    entry_s* iter = ts_list_m.next();
    if ( iter )
        {
        return iter->traffic_stream;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_c* core_traffic_stream_list_c::current() const
    {
    entry_s* iter = ts_list_m.current();
    if ( iter )
        {
        return iter->traffic_stream;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_traffic_stream_list_c::update_traffic_stream(
    const core_traffic_stream_c& traffic_stream )
    {
    DEBUG1( "core_traffic_stream_list_c::update_traffic_stream() - searching an entry with TID %u",
        traffic_stream.tid() );

    entry_s* iter = ts_list_m.first();
    while( iter )
        {
        if ( iter->traffic_stream->tid() == traffic_stream.tid() )
            {
            DEBUG( "core_traffic_stream_list_c::update_traffic_stream() - entry with matching TID found, replacing entry" );
            *iter->traffic_stream = traffic_stream;

            return core_error_ok;
            }

        iter = ts_list_m.next();
        }

    DEBUG( "core_traffic_stream_list_c::update_traffic_stream() - no entry matching the TID found, adding a new entry" );

    entry_s* entry = new entry_s;
    if ( entry )
        {
        entry->traffic_stream = new core_traffic_stream_c(
            traffic_stream.tid(),
            traffic_stream.user_priority() );
        if ( entry->traffic_stream )
            {
            *entry->traffic_stream = traffic_stream;

            ts_list_m.append( entry );
            }
        else
            {
            DEBUG( "core_traffic_stream_list_c::update_traffic_stream() - unable to create core_traffic_stream_c" );
            delete entry;

            return core_error_no_memory;
            }
        }
    else
        {
        DEBUG( "core_traffic_stream_list_c::update_traffic_stream() - unable to create entry_s" );

        return core_error_no_memory;
        }

    return core_error_ok;         
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_traffic_stream_list_c::remove_traffic_stream_by_tid(
    u8_t tid )
    {
    DEBUG1( "core_traffic_stream_list_c::remove_traffic_stream_by_tid() - removing an entry with TID %u", tid );

    entry_s* iter = ts_list_m.first();
    while( iter )
        {
        if ( iter->traffic_stream->tid() == tid )
            {
            DEBUG( "core_traffic_stream_list_c::remove_traffic_stream_by_tid() - matching entry found" );

            ts_list_m.remove( iter );
            iter = NULL;
            }
        else
            {
            iter = ts_list_m.next();
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_traffic_stream_list_c::print_contents()
    {
    for( entry_s* iter = ts_list_m.first(); iter; iter = ts_list_m.next() )
        {
        DEBUG1( "core_traffic_stream_list_c::print_contents() - TID: %u",
            iter->traffic_stream->tid() );
        DEBUG1( "core_traffic_stream_list_c::print_contents() - UP: %u",
            iter->traffic_stream->user_priority() );
        switch( iter->traffic_stream->direction() )
            {
            case core_traffic_stream_direction_uplink:
                DEBUG( "core_traffic_stream_list_c::print_contents() - direction: uplink" );
                break;
            case core_traffic_stream_direction_downlink:
                DEBUG( "core_traffic_stream_list_c::print_contents() - direction: downlink" );
                break;
            case core_traffic_stream_direction_bidirectional:
                DEBUG( "core_traffic_stream_list_c::print_contents() - direction: bi-directional" );
                break;
            }
        switch ( iter->traffic_stream->status() )
            {
            case core_traffic_stream_status_undefined:
                DEBUG( "core_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_undefined" );
                break;
            case core_traffic_stream_status_active:
                DEBUG( "core_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_active" );
                break;
            case core_traffic_stream_status_inactive_not_required:
                DEBUG( "core_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_inactive_not_required" );
                break;
            case core_traffic_stream_status_inactive_deleted_by_ap:
                DEBUG( "core_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_inactive_deleted_by_ap" );
                break;
            case core_traffic_stream_status_inactive_no_bandwidth:
                DEBUG( "core_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_inactive_no_bandwidth" );
                break;
            case core_traffic_stream_status_inactive_invalid_parameters:
                DEBUG( "core_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_inactive_invalid_parameters" );
                break;
            case core_traffic_stream_status_inactive_other:
                DEBUG( "core_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_inactive_other" );
                break;
            }
        DEBUG( "core_traffic_stream_list_c::print_contents()" );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
medium_time_s core_traffic_stream_list_c::admitted_medium_time()
    {
    medium_time_s medium_time( MEDIUM_TIME_NOT_DEFINED );

    for( entry_s* iter = ts_list_m.first(); iter; iter = ts_list_m.next() )
        {
        u16_t ts_medium_time(
            iter->traffic_stream->medium_time() );       
        if( iter->traffic_stream->direction() == core_traffic_stream_direction_bidirectional )
            {
            /**
             * The admitted medium time of a bi-directional stream has to be
             * multiplied by two because it contains both an uplink and
             * a downlink component.
             */
            ts_medium_time *= 2;
            }

        medium_time.up[iter->traffic_stream->user_priority()] += ts_medium_time;
        medium_time.ac[iter->traffic_stream->access_class()] += ts_medium_time;
        }

    return medium_time;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_traffic_stream_list_c::is_traffic_stream_for_access_class(
    core_access_class_e access_class )
    {
    for( entry_s* iter = ts_list_m.first(); iter; iter = ts_list_m.next() )
        {
        if ( core_tools_c::convert_user_priority_to_ac( iter->traffic_stream->user_priority() ) ==
             access_class )
            {
            return true_t;
            }
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_traffic_stream_list_c::is_traffic_stream_for_tid(
    u8_t tid )
    {
    for( entry_s* iter = ts_list_m.first(); iter; iter = ts_list_m.next() )
        {
        if( iter->traffic_stream->tid() == tid )
            {
            return true_t;
            }
        }

    return false_t;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_traffic_stream_list_c::set_traffic_stream_status(
    core_traffic_stream_status_e status )
    {
    for( entry_s* iter = ts_list_m.first(); iter; iter = ts_list_m.next() )
        {
        iter->traffic_stream->set_status( status );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_traffic_stream_list_c::next_tid()
    {
    bool_t tid_array[MAX_TRAFFIC_STREAM_TID] =
        { false_t, false_t, false_t, false_t, false_t,false_t,false_t,false_t };
    for( entry_s* iter = ts_list_m.first(); iter; iter = ts_list_m.next() )
        {
        tid_array[iter->traffic_stream->tid()] = true_t;
        }
    u8_t tid( 0 );
    while( tid < MAX_TRAFFIC_STREAM_TID )
        {
        if( !tid_array[tid] )
            {
            return tid;
            }
        else
            {
            ++tid;
            }
        }

    return MAX_TRAFFIC_STREAM_TID;
    }
