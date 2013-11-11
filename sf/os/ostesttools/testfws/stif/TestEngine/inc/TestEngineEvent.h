/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description: This file contains the header file of the event 
* interface.
*
*/

#ifndef TESTENGINEEVENT_H
#define TESTENGINEEVENT_H

//  INCLUDES
#include <StifTestEventInterface.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None


// FUNCTION PROTOTYPES
// None


// FORWARD DECLARATIONS
// None


// CLASS DECLARATION

// DESCRIPTION
// Interface class for the test system.
class TEventMsg
    :public TEventIf
    {
    public:     // Enumerations
        // None

    private:    // Enumerations
        // None

    public:     // Constructors and destructor
        /**
        * C++ default constructor.
        */
        TEventMsg();

        /**
        * C++ destructor.
        */
        ~TEventMsg();

    public:     // New functions
        
        /**
        * Set event.
        */
        void Set( TEventType aEventType );

        /**
        * Wait event.
        */
        void Wait( const RMessage2& aMessage );
            
        /**
        * Cancel pending Wait.
        */
        void CancelWait();
             
        /**
        * Release event. Unset released.
        */
        void Release();
        
        /**
        * Unset event. Blocks until Release is called.
        */
        void Unset( TRequestStatus& aStatus );
        
    public:     // Functions from base classes
        // None
        
    protected:  // New functions
        // None
        
    protected:  // Functions from base classes
        // None
        
    private:
        // None
        
    public:     //Data
        // None
        
    protected:  // Data
        // None
        
    private:    // Data
        /**
        * Wait RMessage
        */
        RMessage2 iWaitMsg;
        
        /**
        * Wait pending
        */
        TBool iWaitPending;
        
        /**
        * StateEventPending.
        */ 
        TBool iStateEventPending;
        
        /**
        * Unset status.
        */ 
        TRequestStatus* iStatus;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None
    };

#endif      // TESTENGINEEVENT_H

// End of File
