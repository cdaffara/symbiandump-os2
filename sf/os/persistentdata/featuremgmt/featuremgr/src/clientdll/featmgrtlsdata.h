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




#ifndef FEATMGR_TLS_DATA_H
#define FEATMGR_TLS_DATA_H

//  INCLUDES
#include <e32base.h>
#include <featmgr/featurecmn.h>
#include "featmgrclient.h"
#include "featureclient.h"

// FORWARD DECLARATIONS
class MFeatureObserver;

//Exported function for test purpose only 
IMPORT_C TInt GetClientCount();

// CLASS DECLARATION

/**
* Feature manager internal implementation.
* This object is stored in TLS.
*
*/
NONSHARABLE_CLASS(CFeatMgrTlsData) : public CBase, public MFeatureClient
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CFeatMgrTlsData* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CFeatMgrTlsData();

    public:
		/**
        * Check if TLS can be freed, i.e. if client count is zero (or less).
        *
        * @return ETrue if TLS can be freed, EFalse otherwise.
        */
		TBool CanBeFreed() const;
		/**
        * Increse client count for this thread.
        */
		void IncreaseClientCount();

		/**
        * Decrease client count for this thread
        */
		void DecreaseClientCount();
		
		/**
        * Get client count for this thread. Used for testing purpose only
        */
        int ClientCount();
        
		/**
        * From MFeatureClient
        */
        TInt FeatureSupported( TFeatureEntry& aFeature ) const;
        
        TInt FeaturesSupported( RFeatureArray& aFeatures );

        TInt EnableFeature( TUid aFeature ) const;

        TInt DisableFeature( TUid aFeature ) const;

        TInt SetFeature( TUid aFeature, TBool aEnabled, TInt aData ) const;

        TInt SetFeature( TUid aFeature, TInt aData ) const;

        TInt AddFeature( TFeatureEntry aFeature ) const;

        TInt DeleteFeature( TUid aFeature ) const;

        void ListSupportedFeaturesL( RFeatureUidArray& aSupportedFeatures );
  
        TInt RequestNotification( RFeatureUidArray& aFeatures, TUid& aFeatUid, 
                TRequestStatus& aStatus );

        void ReRequestNotification( TUid& aFeatUid, TRequestStatus& aStatus );

        TInt RequestNotifyCancel( TUid aFeature ) const;

        TInt RequestNotifyCancelAll( ) const;

        static void DeleteClient();

        TInt SWIStart() const;
        
        TInt SWIEnd() const; 
        
    private:

        /**
        * C++ default constructor.
        */
        CFeatMgrTlsData();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

		// Keep count of clients so that we know when to free TLS
		TInt iClientCount;
		
		// Feature Manager server client 
		RFeatMgrClient iFeatMgrClient;
		

#ifdef EXTENDED_FEATURE_MANAGER_TEST
		// Debug only API functions
    public:
        void ResourceMark();
        void ResourceCheck();
        TInt ResourceCount();
        void SetHeapFailure(TInt aAllocFailType, TInt aRate);
		TInt NumberOfNotifyFeatures( void ) const;
		TInt CountAllocCells( void ) const;
#endif
      		
    };

#endif      // FEATMGR_TLS_DATA_H
            
// End of File
