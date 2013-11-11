// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



#include <featdiscovery.h>
#include <e32cmn.h>
#include "featdiscoveryimpl.h"

// -----------------------------------------------------------------------------
// TFeatureSet::TFeatureSet()
// -----------------------------------------------------------------------------
//
EXPORT_C TFeatureSet::TFeatureSet() :
    iCount( 0 )
    {
    }

// -----------------------------------------------------------------------------
// TFeatureSet::~TFeatureSet()
// -----------------------------------------------------------------------------
//
EXPORT_C TFeatureSet::~TFeatureSet()
    {
    iStatus.Close();
    }

// -----------------------------------------------------------------------------
// TFeatureSet::Append(TUid)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TFeatureSet::Append( TUid aFeature )
    {
    TInt err;
    TFeatureStat feature;
    feature.iFeatureID = aFeature;
    feature.iSupported=EFalse;
    
    err = iStatus.Append( feature );
    if( err == KErrNone )
        {
        iCount++;
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// TFeatureSet::IsFeatureSupported(TUid)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TFeatureSet::IsFeatureSupported( TUid aFeature ) const
    {
    TBool featureSupported( EFalse );
    TFeatureStat feature;
    feature.iFeatureID = aFeature;
    TInt index = iStatus.Find( feature );
    if( index != KErrNotFound )
        {
        featureSupported = iStatus[index].iSupported;
        }
    
    return featureSupported;
    }

// -----------------------------------------------------------------------------
// TFeatureSet::AreAllFeaturesSupported()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TFeatureSet::AreAllFeaturesSupported() const
    {
    //if the request array is empty return true 
    if( !iCount ) return ETrue;
    
    TBool allSupported( ETrue );
    TInt count( iStatus.Count() );
    
    if( count != iCount )
        {
        // Features have been removed from array, because they don't exist.
        allSupported = EFalse;
        }
    else
        {
        for(TInt i(0); i < count; i++)
            {
            if( !iStatus[i].iSupported )
                {
                allSupported = EFalse;
                break;
                }
            }
        }
    
    return allSupported;
    }

// -----------------------------------------------------------------------------
// TFeatureSet::Count()
// -----------------------------------------------------------------------------
//
TInt TFeatureSet::Count()
    {
    return iStatus.Count();
    }

// -----------------------------------------------------------------------------
// TFeatureSet::Reset()
// -----------------------------------------------------------------------------
//
TUid TFeatureSet::FeatureId( TInt aIndex ) const
    {
    return iStatus[aIndex].iFeatureID;
    }

// -----------------------------------------------------------------------------
// TFeatureSet::Reset()
// -----------------------------------------------------------------------------
//
void TFeatureSet::Reset()
    {
    iStatus.Reset();
    }

// -----------------------------------------------------------------------------
// TFeatureSet::Append()
// -----------------------------------------------------------------------------
//
TInt TFeatureSet::Append( TUid aFeature, TBool aSupported )
    {
    TFeatureStat feature;
    feature.iFeatureID = aFeature;
    feature.iSupported = aSupported;
    
    return iStatus.Append( feature );
    }

// -----------------------------------------------------------------------------
// CFeatureDiscovery::CFeatureDiscovery* NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CFeatureDiscovery* CFeatureDiscovery::NewL()
    {
    CFeatureDiscovery* self = NewLC();
    CleanupStack::Pop( self);

    return self;
    }


// -----------------------------------------------------------------------------
// CFeatureDiscovery::CFeatureDiscovery* NewLC()
// -----------------------------------------------------------------------------
//
EXPORT_C CFeatureDiscovery* CFeatureDiscovery::NewLC()
    {
    CFeatureDiscovery* self = new( ELeave ) CFeatureDiscovery();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------
// CFeatureDiscovery::ConstructL
//
// Symbian OS default constructor, initializes variables and cache 
// ---------------------------------------------------------
//
void CFeatureDiscovery::ConstructL()
    {
    iImpl = CFeatureDiscoveryImpl::NewL();
    }


// -----------------------------------------------------------------------------
// CFeatureDiscovery::~CFeatureDiscovery()
// -----------------------------------------------------------------------------
//
CFeatureDiscovery::~CFeatureDiscovery()
    {
    delete iImpl;
    }


// -----------------------------------------------------------------------------
// CFeatureDiscovery::CFeatureDiscovery()
// -----------------------------------------------------------------------------
//
CFeatureDiscovery::CFeatureDiscovery()
    {
    }


// -----------------------------------------------------------------------------
// CFeatureDiscovery::IsFeatureSupportedL(TInt)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CFeatureDiscovery::IsFeatureSupportedL(TInt aFeature)
    {
    return CFeatureDiscoveryImpl::IsFeatureSupportedL( TUid::Uid( aFeature ) );
    }

// -----------------------------------------------------------------------------
// CFeatureDiscovery::IsFeatureSupportedL(TUid)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CFeatureDiscovery::IsFeatureSupportedL(TUid aFeature)
    {
    return CFeatureDiscoveryImpl::IsFeatureSupportedL( aFeature );
    }

// -----------------------------------------------------------------------------
// CFeatureDiscovery::IsSupported(TInt)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CFeatureDiscovery::IsSupported(TInt aFeature) const
    {
    return iImpl->IsSupported( TUid::Uid( aFeature ) );
    }

// -----------------------------------------------------------------------------
// CFeatureDiscovery::IsSupported(TUid)
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CFeatureDiscovery::IsSupported(TUid aFeature) const
    {
    return iImpl->IsSupported( aFeature );
    }

// -----------------------------------------------------------------------------
// CFeatureDiscovery::FeaturesSupportedL(TFeatureSet&)
// -----------------------------------------------------------------------------
//
EXPORT_C void CFeatureDiscovery::FeaturesSupportedL( TFeatureSet& aFeatures )
    {
    CFeatureDiscoveryImpl::FeaturesSupportedL( aFeatures );
    }
    
// -----------------------------------------------------------------------------
// CFeatureDiscovery::FeaturesSupported(TFeatureSet&)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFeatureDiscovery::FeaturesSupported( TFeatureSet& aFeatures ) const
    {
    return iImpl->FeaturesSupported( aFeatures );
    }

// EOF
