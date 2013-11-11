/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




#ifndef FEATMGRSESSION_H
#define FEATMGRSESSION_H

//  INCLUDES
#include <e32base.h>
#include "featmgrclientserver.h"
#include "featmgrserver.h"

// FORWARD DECLARATIONS
class CFeatMgrFeatureRegistry;

// CLASS DECLARATION

/**
* Contains pending request if feature info is not ready.
*/
NONSHARABLE_CLASS(CFeatMgrPendingRequest) : public CBase
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CFeatMgrPendingRequest* NewL(const RMessage2& aMessage);
        
        /**
        * Destructor.
        */
        virtual ~CFeatMgrPendingRequest();
	
	private:
	    /**
        * C++ default constructor.
        */
        CFeatMgrPendingRequest();
        
        /**
        * ConstructL
        */
	    void ConstructL(const RMessage2& aMessage);
	        
	public: // data
	
	    // The offset of a component link object within elements that form the list.
	    static const TInt iOffset;
	    
	    // Message for later completion is stored here.
        RMessage2 iMessage; 
	    
	private: // data
	
	    // A link object encapsulates a pointer to the next link object in the list.
	    TSglQueLink iLink;       
	
	};


// CLASS DECLARATION

/**
*  Represents a session for a client thread on the server-side.
*  A session acts as a channel of communication between the client 
*  and the server. 
*
*/
NONSHARABLE_CLASS(CFeatMgrSession) : public CSession2
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CFeatMgrSession* NewL(CFeatMgrServer& aServer, CFeatMgrFeatureRegistry& aRegistry);
        
        /**
        * Destructor.
        */
        virtual ~CFeatMgrSession();
        
#ifdef EXTENDED_FEATURE_MANAGER_TEST
        /**
        * 2nd phase construct for sessions - called by the CServer framework
        */
        void CreateL();
#endif // EXTENDED_FEATURE_MANAGER_TEST
        
        
    public: // New functions
        
        /**
        * Called by the server when feature info is ready.
        * Calls request handling functions for pending requests.
        */
        void ServicePendingRequestsL();
        
        /**
        * Called by the server when existing feature has changed or new one added.
        * Completes client notification requests if such exist for feature changed.
        */
        void ServiceNotifications( TFeatureServerEntry& aFeature, TFeatureChangeType aType );

    	/**
    	* Panics client thread.
    	*
    	* @param aMessage Message that is being handled
    	* @param aPanic Panic type
    	*/
    	static void PanicClient( const RMessage2& aMessage, TFeatMgrPanic aPanic );
        
    
    public: // Functions from base classes
 
    protected:  // New functions
        
    protected:  // Functions from base classes
        
        /**
        * From CSession2
        *
        * @param aMessage The message containing the client request
        */
        virtual void ServiceL( const RMessage2& aMessage );
        virtual TInt CountResources();

    private:

        /**
        * C++ default constructor.
        */
        CFeatMgrSession(CFeatMgrServer& aServer, CFeatMgrFeatureRegistry& aRegistry);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Calls matching function of CFeatMgrServer for handling the request.
        *
        * @param aMessage The message containing the client request
        */
        void DispatchMessageL( const RMessage2& aMessage );
        
        TBool IsWriteOperation( const TInt aFunction ) const;
        
    private:    // Data
        
        /**
    	* Feature Manager server reference. 
    	*/
    	CFeatMgrServer& iFeatMgrServer; 
    	
        /**
    	* Feature Manager feature handler reference. 
    	*/
    	CFeatMgrFeatureRegistry& iRegistry;
    	
    	/**
    	* Linked list of CFeatMgrPendingRequest objects.
    	*/
    	TSglQue<CFeatMgrPendingRequest> iList; 

        /**
        * Array of IDs of supported features.
        */
        RFeatureUidArray iNotifyFeatures;
        
        /**
        * Request-message used for signaling client.
        */
        RMessagePtr2 iNotifyMessage;

    };
    
#endif      // FEATMGRSESSION_H 
            
// End of File
