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

#ifndef TESTSERVEREVENT_H
#define TESTSERVEREVENT_H

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
class CTestExecution;

// CLASS DECLARATION


// DESCRIPTION
// Interface class for the test system.
class TEvent
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
        TEvent( TThreadId aThreadId );
        
        /**
        * C++ destructor.
        */
        ~TEvent();

    public: // New functions
        
        /**
        * Set event.
        */
        void Set( TEventType aEventType );
        
       	/** 
        * Check event status.
        */ 
        TBool EventPending();
        
        /** 
        * Check waiting status.
        */ 
        TBool WaitEventPending();
        
        /**
        * First phase of waiting.
        */
		TInt SetWaitPending( TRequestStatus* aStatus = NULL );

        /**
        * Wait event.
        */
        void Wait();
        
        /**
        * Complete pending wait.
        */
		void CompletePendingWait( TInt aError );

        /**
        * Release event. Completes conceivably pending unset.
        */
        void Release();
        
        /**
        * Unset event. Unset blocks until Release is called 
        * (i.e. iUnsetMessage is compeled from release).
        */
        TInt Unset( const RMessage2& aMessage, CTestExecution* aExecution );
        
        /**
        * Get TRequestStatus of the event.
        */ 
        const TRequestStatus* RequestStatus() const { return iReq; }; 

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
        TRequestStatus iRequestStatus;
        
        /**
        * Thread id.
        */
        TThreadId iThread;

        /**
        * Unset message.
        */
        RMessage2 iUnsetMessage;

        /**
        * Flag for state event status.
        */
        TBool iEventPending;
        /**
        * Flag for indicating that iReq is owned by us.
        */
        TBool iOwnReq;
        
        /**
        * Execution object. Is non-NULL if unset is pending.
        */ 
        CTestExecution* iExecution;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None
    };

#endif      // TESTSERVEREVENT_H

// End of File
