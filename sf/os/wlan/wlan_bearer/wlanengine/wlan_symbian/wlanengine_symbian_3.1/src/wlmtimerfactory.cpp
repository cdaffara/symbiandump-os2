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
* Description:  Factory class for timers
*
*/


#include "core_timer_factory.h"
#include "wlmtimer.h"
#include "am_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// core_timer_factory_c::create_timer
// -----------------------------------------------------------------------------
//
abs_core_timer_c* core_timer_factory_c::create_timer(
    core_callback_c* callback )
    {
    DEBUG( "core_timer_factory_c::create_timer()" );
    
    am_symbian_timer_c* timer = new am_symbian_timer_c( callback );

    if ( timer )
        {
        CActiveScheduler::Add( timer );
        }

    return timer;
    }

// -----------------------------------------------------------------------------
// core_timer_factory_c::destroy_timer
// -----------------------------------------------------------------------------
//
void core_timer_factory_c::destroy_timer(
    abs_core_timer_c* timer )
    {
    DEBUG( "core_timer_factory_c::destroy_timer()" );

    delete static_cast<am_symbian_timer_c*>( timer );
    }

//  End of File  
