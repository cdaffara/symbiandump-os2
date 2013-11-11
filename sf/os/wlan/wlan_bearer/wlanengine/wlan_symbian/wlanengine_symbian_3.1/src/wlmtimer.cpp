/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  generic timer
*
*/


#include "wlmtimer.h"
#include "am_debug.h"
#include "core_callback.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// am_symbian_timer_c::am_symbian_timer_c
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
am_symbian_timer_c::am_symbian_timer_c(
    core_callback_c* callback ) :
    CTimer( CTimer::EPriorityStandard ),
    callback_m( callback )
    {
    DEBUG( "am_symbian_timer_c::am_symbian_timer_c()" );
    // Not the best place for this but...
    TRAPD( err, CTimer::ConstructL() );
    if ( err != KErrNone )
        {
        DEBUG1( "am_symbian_timer_c::am_symbian_timer_c() - ConstructL leaved with %d",
            err );
        }
    }

// Destructor
am_symbian_timer_c::~am_symbian_timer_c()
    {
    DEBUG( "am_symbian_timer_c::~am_symbian_timer_c()" );
    Cancel();

    delete callback_m;
    }

// -----------------------------------------------------------------------------
// am_symbian_timer_c::start
// -----------------------------------------------------------------------------
//
void am_symbian_timer_c::start(
    unsigned long int delay )
    {
    DEBUG( "am_symbian_timer_c::start()" );
    After( delay );
    }

// -----------------------------------------------------------------------------
// am_symbian_timer_c::stop
// -----------------------------------------------------------------------------
//
void am_symbian_timer_c::stop()
    {
    DEBUG( "am_symbian_timer_c::stop()" );
    Cancel();
    }

// -----------------------------------------------------------------------------
// am_symbian_timer_c::is_active
// -----------------------------------------------------------------------------
//
bool_t am_symbian_timer_c::is_active() const
    {
    return IsActive();
    }

// -----------------------------------------------------------------------------
// am_symbian_timer_c::RunL
// -----------------------------------------------------------------------------
//
void am_symbian_timer_c::RunL()
    {
    DEBUG( "am_symbian_timer_c::RunL()" );
    callback_m->func_m( callback_m->ptr_m );
    }

//  End of File  
