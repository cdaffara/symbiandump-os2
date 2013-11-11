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
* Description:  Timer for handling WPA TKIP counter measures
*
*/


#ifndef CORE_TIMER_COUNTER_MEASURES_H
#define CORE_TIMER_COUNTER_MEASURES_H

#include "am_platform_libraries.h"
#include "abs_core_timer.h"

/**
 * Timer for handling WPA TKIP counter measures.
 *
 * @since S60 v3.1
 */
NONSHARABLE_CLASS( core_timer_counter_measures_c )
    {

public:

    /** The possible states of the timer. */
    enum core_state_e
        {
        core_state_not_running,
        core_state_activated,
        core_state_blocked
        };

    /**
     * Constructor.
     */
    core_timer_counter_measures_c();

    /**
     * Destructor.
     */
    virtual ~core_timer_counter_measures_c();

    /**
     * Check whether WPA connections are allowed or not.
     *
     * @since S60 v3.1
     * @return Whether WPA connections are allowed or not.
     */
    bool_t is_wpa_allowed();

    /**
     * Used to indicate to the timer that a MIC failure has occured.
     */
    void mic_failure();

    /**
     * Called by the timer framework when timer expires.
     */
    static void timer_expired( void* this_ptr );

private:

    // Prohibit copy constructor
    core_timer_counter_measures_c(
        const core_timer_counter_measures_c& );
    // Prohibit assigment operator
    core_timer_counter_measures_c& operator=(
        const core_timer_counter_measures_c& );

private: // data

    /**
     * Current timer state.
     */
    core_state_e state_m;

    /** 
     * Counter measures timer.
     */
    abs_core_timer_c* timer_m;

    };

#endif // CORE_TIMER_COUNTER_MEASURES_H
