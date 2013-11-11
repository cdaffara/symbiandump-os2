/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Iterator for a virtual traffic stream list.
*
*/

/*
* %version: 2 %
*/

#include "core_virtual_traffic_stream_list_iter.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_virtual_traffic_stream_list_iter_c::core_virtual_traffic_stream_list_iter_c(
    core_virtual_traffic_stream_list_c& list ) :
    iter_m( list.ts_list_m )
    {    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_virtual_traffic_stream_list_iter_c::~core_virtual_traffic_stream_list_iter_c()
    {    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_virtual_traffic_stream_c* core_virtual_traffic_stream_list_iter_c::first()
    {
    core_virtual_traffic_stream_list_c::entry_s* entry = iter_m.first();
    if( entry )
        {
        return entry->traffic_stream;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_virtual_traffic_stream_c* core_virtual_traffic_stream_list_iter_c::next()
    {
    core_virtual_traffic_stream_list_c::entry_s* entry = iter_m.next();
    if( entry )
        {
        return entry->traffic_stream;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_virtual_traffic_stream_c* core_virtual_traffic_stream_list_iter_c::current() const
    {
    core_virtual_traffic_stream_list_c::entry_s* entry = iter_m.current();
    if( entry )
        {
        return entry->traffic_stream;
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_virtual_traffic_stream_list_iter_c::remove()
    {
    core_virtual_traffic_stream_list_c::entry_s* entry = iter_m.current();
    core_error_e ret = iter_m.remove();
    if( ret == core_error_ok &&
        entry )
        {
        delete entry->traffic_stream;
        delete entry;
        entry = NULL;
        }

    return ret;
    }
