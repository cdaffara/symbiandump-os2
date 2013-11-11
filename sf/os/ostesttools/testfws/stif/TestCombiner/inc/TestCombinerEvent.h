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

#ifndef TESTCOMBINEREVENT_H
#define TESTCOMBINEREVENT_H

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
class CLogger;
class CStifLogger;

// CLASS DECLARATION


// DESCRIPTION
// Interface class for the test system.
class TEventTc
    :public TEventIf
    {
    public: // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        /**
        * C++ default constructor.
        */
        TEventTc( CStifLogger* aLogger );
        
        /**
        * C++ parametric constructor.
        */
        TEventTc( TName& aEventName, CStifLogger* aLogger );
  
        /**
        * C++ destructor.
        */
        ~TEventTc();

    public: // New functions
        
        /**
        * Set request status member.
        */
        void SetRequestStatus( TRequestStatus* aStatus );
        
        /**
        * Complete request status member.
        */
        void Complete( TInt aError );

        /**
        * Set event pending.
        */
        void SetEvent( TEventType aEventType );
        
        /**
        * Wait event.
        */
        void WaitEvent( TRequestStatus& aStatus );
        
    public: // Functions from base classes
        // None
        
    protected:  // New functions
        // None
        
    protected:  // Functions from base classes
        // None
        
    private:
        // None
        
    public:   //Data
        // None
        
    protected:  // Data
        // None
        
    private:    // Data
        /**
        * Request status.
        */
        TRequestStatus* iReq; 
        
        /**
        * Logger.
        */ 
        CStifLogger*  iLogger;
                        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None
    };

#endif      // TESTCOMBINEREVENT_H

// End of File
