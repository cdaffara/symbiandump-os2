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
* Description:  generic Symbian timer
*
*/


#ifndef WLMTIMER_H
#define WLMTIMER_H

// INCLUDES
#include <e32base.h>
#include "abs_core_timer.h"

class core_callback_c;

// CLASS DECLARATION
/**
* This class implements a generic Symbian timer.
*
* Timers are instantiated using the the timer factory.
* @see core_timer_factory_c::create_timer
*/
NONSHARABLE_CLASS( am_symbian_timer_c ) :
    public CTimer,
    public abs_core_timer_c
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        * @param callback Callback to call when the timer expires.
        */
        am_symbian_timer_c(
            core_callback_c* callback );
        
        /**
        * Destructor.
        */
        virtual ~am_symbian_timer_c();

    public: // Functions from base classes

        /**
        * From abs_core_timer_c Start the timer.
        * @param delay Expiracy time in micro seconds.
        */        
        void start(
            unsigned long int delay );

        /**
        * From abs_core_timer_c Stop the timer.
        */
        void stop();

        /**
        * From abs_core_timer_c Check whether the timer is active.
        * @ return True if the timer is active, false otherwise.
        */
        bool_t is_active() const;

    protected:  // Functions from base classes
        
        /**
        * From CTimer Action of timer expiration.
        */
        void RunL();

    private:

        // Prohibit copy constructor if not deriving from CBase.
        am_symbian_timer_c(
            const am_symbian_timer_c& );
        // Prohibit assigment operator if not deriving from CBase.
        am_symbian_timer_c& operator=(
            const am_symbian_timer_c& );

    private:    // Data

        /** Callback to call when the timer expires. */
        core_callback_c* callback_m;
    };

#endif // WLMTIMER_H
            
// End of File
