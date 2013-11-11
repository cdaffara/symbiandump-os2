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
* Description:  Factory class for timers implementing abs_core_timer_c interface
*
*/


#ifndef CORE_TIMER_FACTORY_H
#define CORE_TIMER_FACTORY_H

#include "am_platform_libraries.h"

class abs_core_timer_c;
class abs_core_timer_callback_c;
class core_callback_c;

/**
* This class implements a timer factory that is used to instantiate
* timers implementing abs_core_timer_c interface.
* @see abs_core_timer_c
*/
NONSHARABLE_CLASS(core_timer_factory_c)
    {
    public:
        
        /**
        * Create a new timer.       
        * @param callback Handle to callback interface
        * @return Pointer to the created timer.
        */
        static abs_core_timer_c* create_timer(
            core_callback_c* callback );

        /**
        * Destroy an old timer.
        * @param timer Timer to be destroyed.
        */
        static void destroy_timer(
            abs_core_timer_c* timer );

    private: // private constructor
    
        core_timer_factory_c();
    };

#endif // CORE_TIMER_FACTORY_H
