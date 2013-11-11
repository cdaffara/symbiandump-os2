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
* Description:  Class for requesting notifications to Publish & Subscribe
*                properties.
*
*/


#ifndef WLMPLATFORMSUBSCRIBER_H
#define WLMPLATFORMSUBSCRIBER_H

#include <e32base.h>
#include <e32property.h>

enum TWlmSubscribeType
    {
    EWlmSubscribeTypeCenRep,
    EWlmSubscribeTypePubSub,
    };
    
class CRepository;

/**
* Callback interface to client
*/
class MWlmPlatformResponder
    {
    public: // New functions
        
        /**
        * From The subscribed property has changed.
        * @since Series 60 3.0
        * @param aCategory Category of the changed property.
        * @param aKey Key of the changed property.
        */
        virtual void HandlePropertyChangedL( const TUid& aCategory, const TUint aKey ) = 0;
    };

// CLASS DECLARATION
/**
* Class for requesting notifications about changed PubSub/CenRep properties/keys.
* @lib wlmplatform.dll
* @since Series 60 3.0
*/
NONSHARABLE_CLASS( CWlmPlatformSubscriber ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @since Series 60 3.0
        * @param aType specifies whether observed key is in CenRep or PubSub
        * @param aCallback specifies the callback object
        * @param aCategory specifies the key's category (PubSub)
        *        or repository file (CenRep)
        * @param aKey specifies the key to be watched
        */
        static CWlmPlatformSubscriber* NewL(
            TWlmSubscribeType aType,
            MWlmPlatformResponder& aCallback,
            const TUid& aCategory,
            const TUint aKey );
        
        /**
        * Destructor.
        */
        virtual ~CWlmPlatformSubscriber();

    public: // New functions
        
        /**
        * IssueRequest subscribes to changes of a property
        * @since Series 60 3.0
        */
        void IssueRequest();

        /**
        * Get requests the value of the property under subscription
        * (callback only tells the client that the value has changed)
        * @since Series 60 3.0
        */
        TInt Get( TInt& aValue );

    protected:  // Functions from base classes
        
        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */        
        virtual TInt RunError( TInt aError );

        /**
        * From CActive
        */
        virtual void RunL();

    private:

        /**
        * C++ default constructor.
        */
        CWlmPlatformSubscriber(
            TWlmSubscribeType aType,
            MWlmPlatformResponder& aCallback,
            const TUid& aCategory,
            const TUint aKey );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
    
        /** Type of the property to watch (PubSub or CenRep). */
        TWlmSubscribeType iType;
    
        /** Callback to call when the property changes. */
        MWlmPlatformResponder &iCallback;
        
        /** The category/repository to watch. */
        const TUid iCategory;
        
        /** The key to watch. */
        const TUint iKey;

        /** The property to watch in PubSub. */
        RProperty iProperty;
        
        /** The handle to CenRep. */
        CRepository* iCenRep;
    };

#endif // WLMPLATFORMSUBSCRIBER_H
