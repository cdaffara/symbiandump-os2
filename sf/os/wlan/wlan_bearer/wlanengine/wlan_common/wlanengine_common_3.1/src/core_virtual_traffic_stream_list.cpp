/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Class for storing virtual traffic streams.
*
*/

/*
* %version: 1 %
*/

#include "core_virtual_traffic_stream_list.h"
#include "core_tools.h"
#include "core_am_tools.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_virtual_traffic_stream_list_c::core_virtual_traffic_stream_list_c() :
    ts_list_m( ),
    next_stream_id_m( 0 )
    {
    DEBUG( "core_virtual_traffic_stream_list_c::core_virtual_traffic_stream_list_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_virtual_traffic_stream_list_c::~core_virtual_traffic_stream_list_c()
    {
    DEBUG( "core_virtual_traffic_stream_list_c::~core_virtual_traffic_stream_list_c()" );

    for( entry_s* iter = ts_list_m.first(); iter; iter = ts_list_m.next() )
        {
        delete iter->traffic_stream;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_virtual_traffic_stream_list_c::count() const
    {
    return ts_list_m.count();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_virtual_traffic_stream_list_c::add_traffic_stream(
    const core_virtual_traffic_stream_c& stream )
    {
    entry_s* entry = new entry_s;
    if( entry )
        {        
        entry->traffic_stream = new core_virtual_traffic_stream_c(
            stream.requested_tid(),
            stream.tid(),
            stream.user_priority(),
            stream.is_automatic_stream(),
            stream.params(),
            stream.id(),
            stream.status() );
        if( entry->traffic_stream )
            {
            ts_list_m.append( entry );
            }
        else
            {
            DEBUG( "core_virtual_traffic_stream_list_c::add_traffic_stream() - unable to create core_virtual_traffic_stream_c" );

            delete entry;
            entry = NULL;

            return core_error_no_memory;
            }
        }
    else
        {
        DEBUG( "core_virtual_traffic_stream_list_c::update_traffic_stream() - unable to create entry_s" );

        return core_error_no_memory;
        }

    return core_error_ok;         
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_virtual_traffic_stream_list_c::add_traffic_stream(
    u8_t requested_tid,
    u8_t tid,
    u8_t user_priority,
    bool_t is_automatic_stream,
    const core_traffic_stream_params_s& params,
    u32_t& stream_id,
    core_traffic_stream_status_e stream_status )
    {
    entry_s* entry = new entry_s;
    if( entry )
        {
        stream_id = next_stream_id_m++;
        entry->traffic_stream = new core_virtual_traffic_stream_c(
            requested_tid,
            tid,
            user_priority,
            is_automatic_stream,
            params,
            stream_id,
            stream_status );
        if( entry->traffic_stream )
            {
            ts_list_m.append( entry );
            }
        else
            {
            DEBUG( "core_virtual_traffic_stream_list_c::add_traffic_stream() - unable to create core_virtual_traffic_stream_c" );

            delete entry;
            entry = NULL;

            return core_error_no_memory;
            }
        }
    else
        {
        DEBUG( "core_virtual_traffic_stream_list_c::update_traffic_stream() - unable to create entry_s" );

        return core_error_no_memory;
        }

    return core_error_ok;         
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_virtual_traffic_stream_list_c::remove_traffic_stream_by_tid(
    u8_t tid )
    {
    DEBUG1( "core_virtual_traffic_stream_list_c::remove_traffic_stream_by_tid() - removing an entry with TID %u",
        tid );

    core_type_list_iterator_c<entry_s> ts_iter( ts_list_m );
    for( entry_s* iter = ts_iter.first(); iter; iter = ts_iter.next() )
        {
        if( iter->traffic_stream->tid() == tid )
            {
            DEBUG( "core_virtual_traffic_stream_list_c::remove_traffic_stream_by_tid() - matching entry found" );

            ts_iter.remove();
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_virtual_traffic_stream_list_c::remove_traffic_stream_by_id(
    u32_t stream_id )
    {
    DEBUG1( "core_virtual_traffic_stream_list_c::remove_traffic_stream_by_tid() - removing an entry with stream ID %u",
        stream_id );

    core_type_list_iterator_c<entry_s> ts_iter( ts_list_m );
    for( entry_s* iter = ts_iter.first(); iter; iter = ts_iter.next() )
        {
        if( iter->traffic_stream->id() == stream_id )
            {
            DEBUG( "core_virtual_traffic_stream_list_c::remove_traffic_stream_by_tid() - matching entry found" );

            ts_iter.remove();
            }
        }   
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_access_class_traffic_mode_e core_virtual_traffic_stream_list_c::traffic_mode_by_ac(
    core_access_class_e access_class )
    {
    core_type_list_iterator_c<entry_s> ts_iter( ts_list_m );
    for( entry_s* iter = ts_iter.first(); iter; iter = ts_iter.next() )
        {
        if( iter->traffic_stream->access_class() == access_class &&
            !iter->traffic_stream->is_automatic_stream() )
            {
            return core_access_class_traffic_mode_manual;
            }
        }   

    return core_access_class_traffic_mode_automatic;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_virtual_traffic_stream_list_c::print_contents()
    {
    core_type_list_iterator_c<entry_s> ts_iter( ts_list_m );
    for( entry_s* iter = ts_iter.first(); iter; iter = ts_iter.next() )
        {
        DEBUG1( "core_virtual_traffic_stream_list_c::print_contents() - stream ID: %u",
            iter->traffic_stream->id() );
        DEBUG1( "core_virtual_traffic_stream_list_c::print_contents() - requested TID: %u",
            iter->traffic_stream->requested_tid() );
        DEBUG1( "core_virtual_traffic_stream_list_c::print_contents() - current TID: %u",
            iter->traffic_stream->tid() );
        DEBUG1( "core_virtual_traffic_stream_list_c::print_contents() - UP: %u",
            iter->traffic_stream->user_priority() );
        if( iter->traffic_stream->is_automatic_stream() )
            {
            DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - automatic: yes" );
            }
        else
            {
            DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - automatic: no" );
            }
        switch( iter->traffic_stream->params().direction )
            {
            case core_traffic_stream_direction_uplink:
                DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - direction: uplink" );
                break;
            case core_traffic_stream_direction_downlink:
                DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - direction: downlink" );
                break;
            case core_traffic_stream_direction_bidirectional:
                DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - direction: bi-directional" );
                break;
            }
        switch ( iter->traffic_stream->status() )
            {
            case core_traffic_stream_status_undefined:
                DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_undefined" );
                break;            
            case core_traffic_stream_status_active:
                DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_active" );
                break;
            case core_traffic_stream_status_inactive_not_required:
                DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_inactive_not_required" );
                break;
            case core_traffic_stream_status_inactive_deleted_by_ap:
                DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_inactive_deleted_by_ap" );
                break;
            case core_traffic_stream_status_inactive_no_bandwidth:
                DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_inactive_no_bandwidth" );
                break;
            case core_traffic_stream_status_inactive_invalid_parameters:
                DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_inactive_invalid_parameters" );
                break;
            case core_traffic_stream_status_inactive_other:
                DEBUG( "core_virtual_traffic_stream_list_c::print_contents() - status: core_traffic_stream_status_inactive_other" );
                break;
            }
        DEBUG( "core_virtual_traffic_stream_list_c::print_contents()" );
        }
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_virtual_traffic_stream_list_c::next_tid()
    {
    bool_t tid_array[MAX_TRAFFIC_STREAM_TID] =
        { false_t, false_t, false_t, false_t, false_t,false_t,false_t,false_t };
    for( entry_s* iter = ts_list_m.first(); iter; iter = ts_list_m.next() )
        {
        if( iter->traffic_stream->requested_tid() != TRAFFIC_STREAM_TID_NONE )
            {
            /**
             * Both active and inactive streams that were requested
             * with a certain TID.
             */            
            tid_array[iter->traffic_stream->requested_tid()] = true_t;            
            }
        else if( iter->traffic_stream->tid() != TRAFFIC_STREAM_TID_NONE ) 
            {
            /**
             * Active streams.
             */
            tid_array[iter->traffic_stream->tid()] = true_t;            
            }
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
