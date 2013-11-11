/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Abstract interface for a generic timer class
*
*/


#ifndef ABS_CORE_TIMER_H
#define ABS_CORE_TIMER_H

#include "am_platform_libraries.h"
#include "core_types.h"

/**
* Abstract interface class for a generic timer.
*
* Timers implementing this interface are created using
* the timer factory.
* @see core_timer_factory_c::create_timer
*/
class abs_core_timer_c
    {
public: // New functions

    /**
     * Destructor.
     */
    virtual ~abs_core_timer_c() {};
    
    /**
     * Start the timer.
     * @param delay Expiration time in micro seconds.   
     */
    virtual void start(
        u32_t delay ) = 0;

    /**
     * Stop the timer.    
     */
    virtual void stop() = 0;

    /**
     * Check whether the timer is active.
     * @return Whether the timer is active.
     */
    virtual bool_t is_active() const = 0;
    };

#endif // ABS_CORE_TIMER_H
