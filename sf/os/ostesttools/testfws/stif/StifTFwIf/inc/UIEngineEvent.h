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
* Description: This file contains the header file of the 
* CUIEngineEvent.
*
*/

#ifndef STIF_TFW_IF_EVENT_H
#define STIF_TFW_IF_EVENT_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
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
class CUIEngine;

// CLASS DECLARATION

// DESCRIPTION

// CUIEngineEvent is a class that is used to run test cases using
// STIF Test Framework.
class CUIEngineEvent
        : public CActive
    {
    public: // Enumerations
        // None

    private: // Enumerations
        // Object state
        enum TEventState
            {
            EEventIdle,
            EEventRequested,
            EEventWait,
            EEventWaitCompleted,
            EEventReleased,
            };

    public: // Constructors and destructor

        /**
        * NewL is two-phased constructor.
        */
        static CUIEngineEvent* NewL( CUIEngine* aUIEngine );

        /**
        * Destructor of CUIEngineEvent.
        */
        ~CUIEngineEvent();

    public: // New functions

        /**
        * Request request event.
        */
        TInt Request( TDesC& aEventName, TUint32 aMaster, TUint32 aSlave );

        /**
        * Release releases event.
        */
        TInt Release();
        
        /**
        * Return master id.
        */ 
        TUint32 Master() const { return iMaster; }
        
        /**
        * Return event name.
        */ 
        const TDesC& EventName() const { return iEvent.Name(); }


    public: // Functions from base classes

        /**
        * From CActive RunL handles request completion.
        */
        void RunL();

        /**
        * From CActive DoCancel handles request cancellation.
        */
        void DoCancel();

        /**
        * From CActive RunError handles error situations.
        */
        TInt RunError( TInt aError );

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private: // New functions

        /** 
        * C++ default constructor.
        */
        CUIEngineEvent( CUIEngine* aUIEngine );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();


    public: //Data
        // None
    
    protected: // Data
        // None

    private: // Data
    
        // CUIEngineEvent State 
        TEventState     iState;

        // Pointer to UIEngine
        CUIEngine*     iUIEngine;

        // EventIf class for event control
        TEventIf        iEvent;
        TEventIfPckg    iEventPckg;
        
        // Master identifier
        TUint32         iMaster;

        // Slave identifier
        TUint32         iSlave;

    public: // Friend classes
        // None

    protected: // Friend classes
        // None

    private: // Friend classes
        // None

    };



#endif // STIF_TFW_IF_EVENT_H

// End of File
