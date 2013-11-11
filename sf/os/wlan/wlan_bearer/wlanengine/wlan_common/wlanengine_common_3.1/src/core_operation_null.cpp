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
* Description:  Statemachine for null operation (does nothing)
*
*/


#include "core_operation_null.h"
#include "core_server.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_null_c::core_operation_null_c(
    u32_t request_id,
    core_server_c* server,
    abs_core_driverif_c* drivers,
    abs_core_server_callback_c* adaptation,
    core_error_e status ) :
    core_operation_base_c( core_operation_null, request_id, server, drivers, adaptation,
        core_base_flag_none ),
    status_m( status )
    {
    DEBUG( "core_operation_null_c::core_operation_null_c()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_operation_null_c::~core_operation_null_c()
    {
    DEBUG( "core_operation_null_c::~core_operation_null_c()" );
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
core_error_e core_operation_null_c::next_state()
    {
    DEBUG( "core_operation_null_c::next_state()" );
    DEBUG1( "core_operation_null_c::next_state() - completing request with %u",
        status_m );

    return status_m;
    }
