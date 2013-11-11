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
* Description:  Class for storing traffic stream parameters.
*
*/

/*
* %version: 1 %
*/

#include "core_virtual_traffic_stream.h"
#include "core_tools.h"
#include "am_debug.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_virtual_traffic_stream_c::core_virtual_traffic_stream_c(
    u8_t requested_tid,
    u8_t tid,
    u8_t user_priority,
    bool_t is_automatic_stream,
    const core_traffic_stream_params_s& params,
    u32_t stream_id,
    core_traffic_stream_status_e stream_status ) :
    requested_tid_m( requested_tid ),
    tid_m( tid ),
    user_priority_m( user_priority ),
    is_automatic_stream_m( is_automatic_stream ),
    params_m( params ),
    id_m( stream_id ),
    status_m( stream_status )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_virtual_traffic_stream_c::~core_virtual_traffic_stream_c()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u32_t core_virtual_traffic_stream_c::id() const
    {
    return id_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_virtual_traffic_stream_c::requested_tid() const
    {
    return requested_tid_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
u8_t core_virtual_traffic_stream_c::tid() const
    {
    return tid_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_virtual_traffic_stream_c::set_tid(
    u8_t tid )
    {
    tid_m = tid;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
u8_t core_virtual_traffic_stream_c::user_priority() const
    {
    return user_priority_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_access_class_e core_virtual_traffic_stream_c::access_class() const
    {
    return core_tools_c::convert_user_priority_to_ac( user_priority_m );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool_t core_virtual_traffic_stream_c::is_automatic_stream() const
    {
    return is_automatic_stream_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
const core_traffic_stream_params_s& core_virtual_traffic_stream_c::params() const
    {
    return params_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_traffic_stream_status_e core_virtual_traffic_stream_c::status() const
    {
    return status_m;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void core_virtual_traffic_stream_c::set_status(
    core_traffic_stream_status_e status )
    {
    status_m = status;
    }
