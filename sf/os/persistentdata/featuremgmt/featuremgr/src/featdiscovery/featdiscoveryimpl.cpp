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



#include <e32cmn.h>
#include <featdiscovery.h>
#include "featdiscoveryimpl.h"
#include "featurecontrol.h"
#include "featurecmn.h"

// -----------------------------------------------------------------------------
// CFeatureDiscoveryImpl::CFeatureDiscoveryImpl* NewL()
// -----------------------------------------------------------------------------
//
CFeatureDiscoveryImpl* CFeatureDiscoveryImpl::NewL()
    {
    CFeatureDiscoveryImpl* self = new( ELeave ) CFeatureDiscoveryImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }



// ---------------------------------------------------------
// CFeatureDiscoveryImpl::ConstructL
//
// Symbian OS default constructor, initializes variables and cache 
// ---------------------------------------------------------
//
void CFeatureDiscoveryImpl::ConstructL()
    {
    TInt err( iFeatControl.Connect() );
    User::LeaveIfError( err );
    }


// -----------------------------------------------------------------------------
// CFeatureDiscoveryImpl::~CFeatureDiscoveryImpl()
// -----------------------------------------------------------------------------
//
CFeatureDiscoveryImpl::~CFeatureDiscoveryImpl()
    {
    iFeatControl.Close();
    }


// -----------------------------------------------------------------------------
// CFeatureDiscoveryImpl::CFeatureDiscoveryImpl()
// -----------------------------------------------------------------------------
//
CFeatureDiscoveryImpl::CFeatureDiscoveryImpl()
    {
    }


// -----------------------------------------------------------------------------
// CFeatureDiscoveryImpl::IsFeatureSupportedL(TUid)
// -----------------------------------------------------------------------------
//
TBool CFeatureDiscoveryImpl::IsFeatureSupportedL(TUid aFeature)
    {
    RFeatureControl featControl;
    TInt err( featControl.Connect() );
    User::LeaveIfError( err );
    TFeatureEntry feature( aFeature );
    err = featControl.FeatureSupported( feature );
    featControl.Close();
    
    return (( err > 0 ) ? ETrue : EFalse );
    }

// -----------------------------------------------------------------------------
// CFeatureDiscoveryImpl::IsSupported(TUid)
// -----------------------------------------------------------------------------
//
TBool CFeatureDiscoveryImpl::IsSupported(TUid aFeature)
    {
    TFeatureEntry feature( aFeature );
    
    return (( iFeatControl.FeatureSupported( feature ) > 0 ) ? ETrue : EFalse );
    }

// -----------------------------------------------------------------------------
// CFeatureDiscoveryImpl::FeaturesSupportedL(TFeatureSet&)
// -----------------------------------------------------------------------------
//
void CFeatureDiscoveryImpl::FeaturesSupportedL( TFeatureSet& aFeatures )
    {
    RFeatureControl featControl;
    CleanupClosePushL( featControl );
    TInt err( featControl.Connect() );
    User::LeaveIfError( err );
    
    // Construct feature entry array used by feature control
    RFeatureArray features;
    CleanupClosePushL( features );
    TInt count( aFeatures.Count() );
    
    for(TInt i(0); i < count; i++)
        {
        TFeatureEntry feature( aFeatures.FeatureId( i ) );
        features.AppendL( feature );
        }

    // Fetch feature information from server
    err = featControl.FeaturesSupported( features );
    User::LeaveIfError( err );

    // Refresh count of features after query, non existing features are removed 
    count = features.Count();
    // Write information back to format feature discovery uses
    aFeatures.Reset();
    
    for(TInt i(0); i < count; i++)
        {
        const TUid uid( features[i].FeatureUid() );
        const TBool supported( features[i].FeatureFlags().IsSet( EFeatureSupported ) );
        err = aFeatures.Append( uid, supported );
        User::LeaveIfError( err );
        }

    CleanupStack::PopAndDestroy( &features );
    CleanupStack::PopAndDestroy( &featControl );
    }

// -----------------------------------------------------------------------------
// CFeatureDiscoveryImpl::FeaturesSupported(TFeatureSet&)
// -----------------------------------------------------------------------------
//
TInt CFeatureDiscoveryImpl::FeaturesSupported( TFeatureSet& aFeatures )
    {
    // Construct feature entry array used by feature control
    TInt err( KErrNone );
    RFeatureArray features;
    TInt count( aFeatures.Count() );
    
    for(TInt i(0); i < count; i++)
        {
        TFeatureEntry feature( aFeatures.FeatureId( i ) );
        err = features.Append( feature );
        if( err != KErrNone )
            {
            break;
            }
        }

    if( err == KErrNone )
        {
        // Fetch feature information from server
        err = iFeatControl.FeaturesSupported( features );
        // Refresh count of features after query, non existing features are removed
        count = features.Count();
        }

    if( err == KErrNone )
        {
        // Write information back to format feature discovery uses
        aFeatures.Reset();
        
        for(TInt i(0); i < count; i++)
            {
            const TUid uid( features[i].FeatureUid() );
            const TBool supported( features[i].FeatureFlags().IsSet( EFeatureSupported ) );
            err = aFeatures.Append( uid, supported );
            if( err != KErrNone )
                {
                break;
                }
            }
        }
    
    features.Close();
    return err;
    }
// EOF
