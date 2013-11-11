/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef FEATMGRFEATUREREGISTRY_H
#define FEATMGRFEATUREREGISTRY_H

//  INCLUDES
#include <e32svr.h>
#include <s32file.h>
#include <featmgr/featurecmn.h>
#include <featmgr/featureinfoplugin.h>
#include "featmgrfeatureentry.h"
#include "swilistener.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class MFeatMgrRegistryObserver
    {
    public:
        virtual void HandleFeatureChange( TFeatureServerEntry& aFeature, 
            TFeatureChangeType aType ) = 0;
    };

// CLASS DECLARATION
NONSHARABLE_CLASS(CFeatMgrFeatureRegistry) : public CBase, MSWICallBack
    {
    private:
	
    	/* 
    	* Status for SWI which must be kept in memory in order for calls made to the new
    	* API functions SWIStart and SWIEnd can be related to actual installation/uninstallation.
    	* 
    	* ESWIOutOfMemory will inform FeatMgr that an out-of-memory condition has occurred during
    	* caching and/or memory pre-allocation for the new features to be added.
    	*/
    	enum TSWIStatus
	    	{
	    	ESWIComplete=0,
	    	ESWIInstalling,
	    	ESWIAborted,
	    	ESWIOutOfMemory 
	    	};
    	
	    /*
	    * Enumerations to identify the category of the operation to apply on the cached 
	    * features after SWI completes.
	    */
		enum TSWIOperationCat
			{ 
			ESWIAddFeat,
			ESWIDeleteFeat,
			ESWISetFeatAndData,
			ESWISetFeatData,
			ESWISetFeat
			};
		
		/*
		* The operation tuple to be cached
		*/
		struct TSWICachedOperation
			{
			TSWIOperationCat iCat;
			TFeatureServerEntry iFeatEntry;
			};

    public:
        
        /**
        * Two-phased constructor.
        */
        static CFeatMgrFeatureRegistry* NewL( RFs& aFs, MFeatMgrRegistryObserver& aObserver );

        /**
        * Destructor.
        */
        virtual ~CFeatMgrFeatureRegistry();

    public:
           
        /**
        * Reads feature files.
        */
        void ReadFeatureFilesL();

        /**
        * Reads runtime feature file.
        */
        void ReadRuntimeFeaturesL( TBool &aFeaturesReady );

        /**
        * Merges features to common feature array from plugin.
        */
        void MergePluginFeaturesL( RArray<FeatureInfoCommand::TFeature>& aList );

        /**
        * Merges features to common feature array from plugin.
        */
        void MergePluginFeaturesL( RFeatureArray& aList );

        /**
        * Returns feature support status.
        */
        TInt IsFeatureSupported( TFeatureServerEntry& aFeature );

        /**
        * Adds feature.
        * @param aFeature A reference to a client owned TFeatureEntry object
        * @param aProcessId The id of the process calling this function. This is needed to identify 
        * 					the executable launched by Software Installer (SWI) and cache its to-be
        * 					added features. The default value for aProcessId enables other clients to 
        * 					add features directly to the list (allowing backward compatibility).
        * When set to ETrue the function 
        */
        TInt AddFeature( TFeatureServerEntry& aFeature, TUint aProcessId = 0 );

        /**
        * Deletes feature.
        */
        TInt DeleteFeature( TUid aFeature, TUint = 0 );

        /**
        * Sets feature support status and optionally data.
        */
        TInt SetFeature( TUid aFeature, TInt aEnable, const TUint32 *aData = NULL, TUint = 0 );

        /**
        * Lists supported features to array.
        */
        void SupportedFeaturesL( RFeatureUidArray& aSupportedFeatures );

        /**
        * Returns number of supported features.
        */
        TInt NumberOfSupportedFeatures();
        
        /**
         * Make a backup of the feature list into member iFeatureListBackup, then reset 
         * the feature list iFeatureList.
		 *
		 * Leaves if there is an error during back up, with KErrNoMemory or any of the 
		 * other system-wide error codes.
         */ 
        void ResetFeaturesL();
        
        /**
		 * This function will handle the required notifications for new, deleted and changed features
		 * after a restore operation has taken place.
         */ 
        void HandleRestoredFeatureNotificationsL();
        
        /**
		 * Returns the fully qualified path and filename for the runtime feature data file.
         */ 
        TFileName GetFeaturesFilePathAndName( void );

        /**
        * Indicates that SWI started. It takes the Id of the process calling it as a parameter.
        */
        TInt SWIStart(TUint);
        
        /**
        * Indicates that SWI ended. It takes the Id of the process calling it as a parameter.
        */
        TInt SWIEnd(TUint);
        
        /**
         * Add a new command to the cache.
         * 
         * @return error KErrArgument if the maximum number of cached features is reached.
         * 				 KErrNoMemory if there is no memory to create a new command to cached,
         * 							  or if an out-of-memory condition has occured during caching.
         */
         TInt SWICacheCommand(TSWIOperationCat aOptCat, TFeatureServerEntry aFeature);
        
        /**
        * Commit changes for the cached features into feature manager.
        */
        void CommitSWIFeatChanges();
        
        /**
        * Reset and clean up SWI-related states and member variables.
        */
        void SWIReset();

        /*
        * Handle the case when SWI aborts.
        */
        void SWIAborted();
        
        /*
        * Virtual function from MSWICallBack.
        * It is fired when the timer runs out to indicate that SWI finished before a call to 
        * SWIEnd or the launched executable crashed or hanged. It will deal with rolling-back 
        * all cached changes. It is called after 15 seconds.
        */
        void SWITimedOut();
        
        /*
        * Return the status of the caching mechanism (started or not).
        */
        TBool SWICacheStarted();
        
        /*
        * Returns the status of the caching mechanism (successful, aborted, out-of-memory).
        */
        TBool SWICacheStatusOOM();
        
    private:

        CFeatMgrFeatureRegistry( RFs& aFs, MFeatMgrRegistryObserver& aObserver );
        void ConstructL();
        
        TInt ReadFiles( const TDesC& aFilePath, CDir* aDir );
        void ReadFileL( const TDesC& aFilePath );
        void CompareFeatureListsL();
        void ReadFilesFromDirL( const TDesC& aDirName );
        void UpdateRuntimeFeaturesFileL( TFeatureServerEntry& aFeature, TFeatureChangeType aType );

        TInt HandleChange( TFeatureServerEntry& aFeature, TFeatureChangeType aType );
        TInt ValidateFeature( TUid aFeature, TInt &aIndex );
        TInt SearchFeature( TUid aFeature );
        TInt EnableFeature( TUid aFeature );
        TInt DisableFeature( TUid aFeature );
        TBool IsFlagSet( TInt aIndex, TFeatureFlags aFlag );
        static TInt FindByUid( const TUid *aFeature, const TFeatureServerEntry& aItem );
        static TInt FindByUid( const TFeatureServerEntry& aFeature, 
        const TFeatureServerEntry& aItem );
        void ValidateHeaderL( RFileReadStream& stream, TUint32& count, TUint32& countDSRs );
        void WriteHeaderAndEntriesL( RFileWriteStream &aStream, RFeatureServerArray& aArray );
        void ValidateFeatureFlagL(TBitFlags32 aFlags);
        void ValidateRuntimeFeatureFlagL(TBitFlags32 aFlags);
        
     private:
        
        // Observer of feature changes
        MFeatMgrRegistryObserver& iObserver;
        // Array of features
        RArray<TFeatureServerEntry> iFeatureList;
        // Struct to contain information of default supported ranges
        class TDefaultRange
	        {
	        public:
    	        TUid iLowUid;
    	        TUid iHighUid;
	        };
        // Array of supported ranges
        RArray<TDefaultRange> iRangeList;
        // File server session
        RFs& iFs;
        
        // Array of features (backup up for BUR)
        RArray<TFeatureServerEntry> iFeatureListBackup;

        // Flag to tell feature manager to cache newly modified features during SWI.
        TBool iSWICacheFeature;
        // Tracks the SWI status to check if an installation/uninstallation is in progress before 
        // starting caching.
        TSWIStatus iSWIStatus;
        // Array that holds the operations to apply on cached features when SWI ends.
        RArray<TSWICachedOperation> iSWICachedOperations;
        // Id of the process launched by SWI whose commands to feature manager are to be cached.
        TUint iSWIProcessId;
        // Listens to the SWI P&S property KSAUidSoftwareInstallKeyValue 
        CSWIListener* iSWIListener;
        // Timer to handle the situation when the launched exe by SWI manipulates features and then 
        // hangs/crashes, or a call to SWIEnd was not made.
        CSWITimer* iSWITimer;
        // Traces if an "out-of-memory" condition has occurred during caching. If so, FeatMgr will
        // not commit the cache when the installation is successful. It will be disregarded.
        TBool iOomOccured;
        // Counts the number of features allowed to be cached during SWI install/uninstall. 
        // The maximum is set to 50 (MAXSWIOPS)
        TInt iSWIOperations;
        // Counts how many AddFeature commands are cached during SWI
        TInt iAddFeatCount;
    };

#endif      // FEATMGRFEATUREREGISTRY_H 
            
// End of File
