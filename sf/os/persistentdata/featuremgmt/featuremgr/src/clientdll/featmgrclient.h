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



#ifndef FEATMGRCLIENT_H
#define FEATMGRCLIENT_H

// INCLUDES
#include <e32std.h>
#include <babitflags.h>
#include <featmgr/featurecmn.h>
#include "featmgrclientserver.h"
#include "featureclient.h"

// CLASS DECLARATION

/**
  Base class of the client-side handle to a session with the Feature Manager Server.

*/
NONSHARABLE_CLASS(RFeatMgrClient) : public RSessionBase, public MFeatureClient
    {
    public:  // Constructors and destructor
        
        /**
         C++ default constructor.
         */
        RFeatMgrClient();

    public: // New functions

        /**
         Connects to server
        
         @return KErrNone if connect succesfully, otherwise system wide error
        */
        TInt Connect();
        
		/**
         From MFeatureClient
        */
        TInt FeatureSupported( TFeatureEntry& aFeature ) const;

        TInt FeaturesSupported( RFeatureArray& aFeatures );

        TInt EnableFeature( TUid aFeature ) const;

        TInt DisableFeature( TUid aFeature ) const;

        TInt SetFeature( TUid aFeature, TBool aEnabled, TInt aData ) const;

        TInt SetFeature( TUid aFeature, TInt aData ) const;

        TInt AddFeature( TFeatureEntry aEntry ) const;

        TInt DeleteFeature( TUid aFeature ) const;

        void ListSupportedFeaturesL( RFeatureUidArray& aSupportedFeatures );

        TInt RequestNotification( RFeatureUidArray& aFeatures, TUid& aFeatUid, 
            TRequestStatus& aStatus );

        void ReRequestNotification( TUid& aFeatUid, TRequestStatus& aStatus );
        
        TInt RequestNotifyCancel( TUid aFeature ) const;

        TInt RequestNotifyCancelAll( ) const;

        TInt SWIStart() const;
        
        TInt SWIEnd() const; 
                
    private:

        /**
         Return version of server
         @param
         @return version of server
        */
         TVersion ServerVersion() const;
         
        /**
         Starts server
         @return KErrNone if started succesfully, otherwise system wide error
        */
        TInt StartServer();
        
        void SendUidArrayL(RFeatureUidArray& aFeatures, TInt &retval);

        void SendRcvFeatureArrayL(RFeatureArray& aFeatures, TInt &retval);

    private:

        TPtr8 iFeaturePckg;


		#ifdef EXTENDED_FEATURE_MANAGER_TEST
        // Public DEBUG API functions
    public:
        void ResourceMark();
        void ResourceCheck();
        TInt ResourceCount();
        void SetHeapFailure(TInt aAllocFailType, TInt aRate);
		TInt NumberOfNotifyFeatures( void ) const;
		TInt CountAllocCells( void ) const;
		#endif    
    
    };

#endif      // FEATMGRCLIENT_H   
            
// End of File
