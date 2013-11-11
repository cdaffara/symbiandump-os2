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



#ifndef FEATMGRFEATUREENTRY_H
#define FEATMGRFEATUREENTRY_H

#include <e32cmn.h>
#include <featmgr/featurecmn.h>

class RReadStream;
class RWriteStream;


/** 
An enumeration for accessing internal feature status flags bits 24..31 used
inside the server. See TFeatureFlags in published header featurecmn.h for the
flag definitions 0..23. 

@see TFeatureFlags
@see TBitFlagsT
@internalComponent 
*/
enum TFeatureFlagsServer 
    {
    // Do not define flags here for values 0..23. See TFeatureFlags enum.
   
    /** If set, the feature flag is a Runtime feature flag (created at run time).
    if not set means source of feature flag is ROM or plug-in */
    EFeatureRuntime         = 24,
    
    // Reserved flags, these are not currently used in the server
    EFeatureFlagUnused25    = 25,
    EFeatureFlagUnused26    = 26,
    EFeatureFlagUnused27    = 27,
    EFeatureFlagUnused28    = 28,
    EFeatureFlagUnused29    = 29,
    EFeatureFlagUnused30    = 30,
    EFeatureFlagUnused31    = 31
    };


// CLASS DECLARATION
// @see corresponding client side class TFeatureEntry

class TFeatureServerEntry
    {
    public:
                                                    
        /** 
        * Constructors.
        */
        TFeatureServerEntry();
        TFeatureServerEntry( TUid aFeature );
        TFeatureServerEntry( TUid aFeature, TBitFlags32 aFlags, TUint32 aData );
        TFeatureServerEntry( TFeatureEntry& aEntry );
            
        /** 
        * Returns feature status flags.
        */
        TBitFlags32 FeatureFlags() const;

        /** 
        * Returns feature data.
        */
        TUint32 FeatureData() const;
        
        /** 
        * Returns feature UID.
        */
        TUid FeatureUid() const;
        
        /**
        * Internalize object from stream
        */
        void InternalizeL( RReadStream& aStream );
        
        /**
        * Externalize object from stream
        */
        void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
         *This sets a feature's flags and data
         */
        void inline Set( TFeatureServerEntry aEntry)
            {
            iFlags = aEntry.FeatureFlags();
            iData = aEntry.FeatureData();
            }
       
        /**
         *This sets a feature's flags and data
         */
        void inline Set(TBitFlags32 aFlags, TUint32 aData)
            {
            iFlags = aFlags;
            iData = aData;
            }

    private:

        /** UID of the feature. */         
        TUid iFeatureID;
        
        /** Value of the feature status flags.
        @see TFeatureFlags
        @see TFeatureFlagsServer */
        TBitFlags32 iFlags;
        
        /** Data associated with feature. */
        TUint32 iData;
    };

/**
* Defines TFeatureEntry array. 
*/
typedef RArray<TFeatureServerEntry> RFeatureServerArray;

#endif  // FEATMGRFEATUREENTRY_H

// End of File
