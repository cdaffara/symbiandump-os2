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

#ifndef TESTEVENTINTERFACE_H
#define TESTEVENTINTERFACE_H

//  INCLUDES
#include <e32std.h>

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
// Interface class for the event system.
class TEventIf
    {
    public: // Enumerations
        /**
        * Event request types.
        */
        enum TReqType
            {
            // The following can be used by the test module
            EReqEvent = 1,
            EWaitEvent,
            ERelEvent,
            ESetEvent,
            EUnsetEvent,
            
            // The rest are special
            // Not used by the test module
            EEnable,
            ESetWaitPending,
            ECmdError,
            ECancelWait,
            };
        
        /**
        * Event types.
        */
        enum TEventType
            {
            EIndication,
            EState,
            };
            
    private: // Enumerations
        
    public:  // Constructors and destructor

        /**
        * C++ constructor with params.
        */
        TEventIf( TReqType aType = EEnable, 
                  const TName& aName = KNullDesC(), 
                  TEventType aEventType = EIndication ) 
            { iType = aType; iName = aName; iEventType = aEventType; }

    public: // New functions
        
        /**
        * Get request type.
        */        
        inline TReqType Type() const { return iType; }
         
        /**
        * Get event name.
        */        
        inline const TName& Name() const { return iName; } 
        
        /**
        * Get event type.
        */        
        inline TEventType EventType() const { return iEventType; } 
        
        /**
        * Set request type.
        */        
        inline void SetType( TReqType aType ){ iType = aType; } 
        
        /**
        * Set event name.
        */        
        inline void SetName( const TName& aName ){ iName = aName; }
        
        /**
        * Set event to state event.
        */        
        inline void SetEventType( TEventType aEventType )
            { iEventType = aEventType; }
        
        /**
        * Set request type and event name.
        */        
        inline void Set( TReqType aType, const TName& aName )
            { iType = aType; iName = aName; } 
        
        /**
        * Set request type, event name and event type.
        */        
        inline void Set( TReqType aType, 
                         const TName& aName, 
                         TEventType aEventType )
            { iType = aType; iName = aName; iEventType = aEventType; } 
        
        /**
        * Copy data from existing TEventIf (or derived class).
        */
        inline void Copy( const TEventIf& aEvent) 
            { iType = aEvent.iType; 
            iName = aEvent.iName; 
            iEventType = aEvent.iEventType; } 
          
    public: // Functions from base classes
        
    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:
        
    public:   //Data
        
    protected:  // Data
        /**
        * Request type.
        */
        TReqType iType; 
        
        /**
        * Event name (must be unique).
        */
        TName iName;     
        
        /**
        * Event type.
        */
        TEventType iEventType;
        
    private:    // Data
        
    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
        
    };
    
// Packaged TEventIf
typedef TPckg<TEventIf> TEventIfPckg;
 
#endif      // TESTEVENTINTERFACE_H

// End of File
