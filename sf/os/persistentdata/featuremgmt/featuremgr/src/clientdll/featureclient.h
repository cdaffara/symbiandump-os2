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




#ifndef FEATURECLIENT_H
#define FEATURECLIENT_H

//  INCLUDES
#include <e32base.h>
#include <featmgr/featurecmn.h>

// CLASS DECLARATION

/**
* Feature manager client interface.
*
*/
class MFeatureClient
    {
    public: // From MFeatureClient

        /**
        * Checks if specified feature is supported and returns 
        * its flags and data if requested.
        * @param aFeature feature entry
        * @return Positive (1) if the feature is supported,
        *         Zero if feature is not supported.
        *         KErrNotFound if feature not found.
        *         Otherwise one of the Symbian error codes. 
        */
        virtual TInt FeatureSupported( TFeatureEntry& aFeature ) const = 0;

        /**
        * Fills feature status array according to status/data in server 
        * 
        * @param aFeatures feature entry array
        * @return KErrNone if no error fetching feature statuses.
        *         Otherwise one of the Symbian OS error codes.
        */
        virtual TInt FeaturesSupported( RFeatureArray& aFeatures ) = 0;

   		/**
        * Enables a certain feature.
        *
        * @param aFeature Feature ID.
        * @return   KErrNone if the feature was succesfully enabled.
        *           KErrAccessDenied if the feature is not modifiable.
        *           KErrPermissionDenied if a capability check fails.
        *           Otherwise one of the Symbian error codes. 
        */
        virtual TInt EnableFeature( TUid aFeature ) const = 0;

       /**
        * Disable a certain feature.
        *
        * @param aFeature Feature ID.
        * @return   KErrNone if the feature was succesfully disabled.
        *           KErrAccessDenied if the feature is not modifiable.
        *           KErrPermissionDenied if a capability check fails.
        *           Otherwise one of the Symbian error codes. 
        */
        virtual TInt DisableFeature( TUid aFeature ) const = 0;

   		/**
        * Enables or disables a certain feature and sets feature data.
        *
        * @param aFeature Feature ID.
        * @param aEnabled Feature enable or disable.
        * @param aData Feature data.
        * @return   KErrNone if the feature was succesfully enabled.
        *           KErrAccessDenied if the feature is not modifiable.
        *           KErrPermissionDenied if a capability check fails.
        *           Otherwise one of the Symbian error codes. 
        */
        virtual TInt SetFeature( TUid aFeature, TBool aEnabled, TInt aData ) const = 0;

   		/**
        * Sets certain feature's data.
        *
        * @param aFeature Feature ID.
        * @param aData Feature data.
        * @return   KErrNone if feature data was succesfully set.
        *           KErrAccessDenied if the feature is not modifiable.
        *           KErrPermissionDenied if a capability check fails.
        *           Otherwise one of the Symbian error codes. 
        */
        virtual TInt SetFeature( TUid aFeature, TInt aData ) const = 0;

   		/**
        * Add a new feature.
        *
        * @param aFeature Feature entry.
        * @return   KErrNone if the feature was succesfully added.
        *           KErrAccessDenied if the feature is not modifiable.
        *           KErrPermissionDenied if a capability check fails.
        *           KErrAlreadyExists if feature already exists.
        *           Otherwise one of the Symbian error codes. 
        */
        virtual TInt AddFeature( TFeatureEntry aEntry ) const = 0;

   		/**
        * Delete a feature.
        *
        * @param aFeature Feature ID.
        * @return   KErrNone if the feature was successfully deleted.
        *           KErrAccessDenied if the feature is not added using AddFeature() method.
        *           KErrPermissionDenied if a capability check fails.
        *           KErrNotFound if the feature not found.
        *           Otherwise one of the Symbian error codes. 
        */
        virtual TInt DeleteFeature( TUid aFeature ) const = 0;

        /**
        * Lists all supported features.
        *
        * @param aSupportedFeatures A reference to a client owned RFeatureUidArray 
        *           array which will be filled with the IDs of supported features. 
        * @return  - One of the Symbian error codes.
        */
        virtual void ListSupportedFeaturesL( RFeatureUidArray& aSupportedFeatures ) = 0;

        /**
        * Requests notification for subset of features changes.
        *
        * @param aFeatures Features, from which client requests notification. 
        * @param aFeatUid Feature id, which will be set by server on completion. 
        * @param aStatus Client request to be signaled. 
        * @return  - One of the Symbian error codes.
        */
        virtual TInt RequestNotification( RFeatureUidArray& aFeatures, TUid& aFeatUid, 
            TRequestStatus& aStatus ) = 0;

        /**
         * Re-requests notification of a feature change.
         * 
         * @param aFeatUid UId of the feature to be re-requested.
         * @param aStatus Client request to be signalled asynchronously.
         * @return One of the Symbian OS system-wide error codes.
         */
        virtual void ReRequestNotification( TUid& aFeatUid, TRequestStatus& aStatus ) = 0;
        
        /**
        * Cancel notification request for single feature's changes.
        *
        * @param aFeature Feature, for which client notification cancellation.
        * @return - One of the Symbian error codes.
        */
        virtual TInt RequestNotifyCancel( TUid aFeature ) const = 0;

        /**
        * Cancel notification request for all features changes.
        *
        * @return  - One of the Symbian error codes.
        */
        virtual TInt RequestNotifyCancelAll( ) const = 0;

    };

#endif      // FEATURECLIENT_H
            
// End of File
