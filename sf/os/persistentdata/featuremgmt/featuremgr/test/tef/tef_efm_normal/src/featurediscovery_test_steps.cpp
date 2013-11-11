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
// Feature Discovery component test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featdiscovery.h>
#include <featmgr/featurecontrol.h>
#include "featurediscovery_test_steps.h"

CFeatureDiscoverySingleFeatureStaticRequestStep::CFeatureDiscoverySingleFeatureStaticRequestStep()
   {
   SetTestStepName(KFeatureDiscoverySingleFeatureStaticRequest);
   }

TVerdict CFeatureDiscoverySingleFeatureStaticRequestStep::doTestStepL()
    {
    TBool supported( CFeatureDiscovery::IsFeatureSupportedL(KDefaultUnsupportedUid) );
 	TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
    	_L("CFeatureDiscovery::IsFeatureSupportedL - KFeatureUnsupported expected: return value is %d"),supported);
    supported = CFeatureDiscovery::IsFeatureSupportedL(KDefaultUnsupportedUid.iUid);
    TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
    	_L("CFeatureDiscovery::IsFeatureSupportedL - KFeatureUnsupported expected: return value is %d"),supported);
    
    supported = CFeatureDiscovery::IsFeatureSupportedL(KDefaultSupportedUid);
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
    	_L("CFeatureDiscovery::IsFeatureSupportedL - KFeatureSupported expected: return value is %d"),supported);
    supported = CFeatureDiscovery::IsFeatureSupportedL(KDefaultSupportedUid.iUid);
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
    	_L("CFeatureDiscovery::IsFeatureSupportedL - KFeatureSupported expected: return value is %d"),supported);
    
   return TestStepResult();
   }

CFeatureDiscoverySingleFeatureDynamicRequestStep::CFeatureDiscoverySingleFeatureDynamicRequestStep()
   {
   SetTestStepName(KFeatureDiscoverySingleFeatureDynamicRequest);
   }

TVerdict CFeatureDiscoverySingleFeatureDynamicRequestStep::doTestStepL()
   {
    CFeatureDiscovery* disc = NULL;
    TRAPD(err,disc = CFeatureDiscovery::NewL());
    TESTDIAGNOSTICERROR(err==KErrNone,_L("CFeatureDiscovery::NewLC failed: error = %d"),err);
    if (err)
    	return TestStepResult();
	
	CleanupStack::PushL(disc);    
    
    TBool supported( disc->IsSupported(KDefaultUnsupportedUid) );
    TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
    	_L("CFeatureDiscovery::IsSupported - KFeatureUnsupported expected: return value is %d"),supported);
    
    supported = disc->IsSupported(KDefaultUnsupportedUid.iUid);
    TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
    	_L("CFeatureDiscovery::IsSupported - KFeatureUnsupported expected: return value is %d"),supported);
    
    supported = disc->IsSupported(KDefaultSupportedUid);
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
    	_L("CFeatureDiscovery::IsSupported - KFeatureSupported expected: return value is %d"),supported);
    
    supported = disc->IsSupported(KDefaultSupportedUid.iUid);
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
    	_L("CFeatureDiscovery::IsSupported - KFeatureSupported expected: return value is %d"),supported);
    
    CleanupStack::PopAndDestroy( disc );

   return TestStepResult();
   }

CFeatureDiscoveryMultipleFeatureStaticRequestStep::CFeatureDiscoveryMultipleFeatureStaticRequestStep()
   {
   SetTestStepName(KFeatureDiscoveryMultipleFeatureStaticRequest);
   }

TVerdict CFeatureDiscoveryMultipleFeatureStaticRequestStep::doTestStepL()
    {    
    TFeatureSet featset;

    // Check whether empty array provides expected result
    TBool supported = featset.IsFeatureSupported(KDefaultSupportedUid);
    TESTDIAGNOSTICERROR(!supported,
    	_L("TFeatureSet::IsFeatureSupported - EFalse expected: return value is %d"),supported);
    TBool allSupported = featset.AreAllFeaturesSupported();
    TESTDIAGNOSTICERROR(allSupported,
    	_L("TFeatureSet::AreAllFeaturesSupported - ETrue expected: return value is %d"),allSupported);
    
    // Append two supported features in feature set array.
    User::LeaveIfError( featset.Append( KDefaultSupportedUid ) );
    User::LeaveIfError( featset.Append( KModifiableUid ) );

    // Query feature statuses from server and check statuses.
    CFeatureDiscovery::FeaturesSupportedL( featset );    
    supported = featset.IsFeatureSupported(KDefaultSupportedUid);
    TESTDIAGNOSTICERROR(supported,
    	_L("TFeatureSet::IsFeatureSupported - ETrue expected: return value is %d"),supported);
        
    supported = featset.IsFeatureSupported(KModifiableUid);
    TESTDIAGNOSTICERROR(supported,
    	_L("TFeatureSet::IsFeatureSupported - ETrue expected: return value is %d"),supported);
    // Check even one which not appended in list
    supported = featset.IsFeatureSupported(KUnmodifiableUid);
    TESTDIAGNOSTICERROR(!supported,
    	_L("TFeatureSet::IsFeatureSupported - EFalse expected: return value is %d"),supported);
    // Check whether all QUERIED features are supported
    allSupported = featset.AreAllFeaturesSupported();
    TESTDIAGNOSTICERROR(allSupported,
    	_L("TFeatureSet::AreAllFeaturesSupported - ETrue expected: return value is %d"),allSupported);
    
    // Append feature which is supposed to be non-existent and 
    // check whether features supported after that.
    User::LeaveIfError( featset.Append( KUnknownUid ) );
    CFeatureDiscovery::FeaturesSupportedL( featset );
    supported = featset.IsFeatureSupported( KUnknownUid );
    TESTDIAGNOSTICERROR(!supported,
    	_L("TFeatureSet::IsFeatureSupported - EFalse expected: return value is %d"),supported);
    // Check whether all QUERIED features are supported
    allSupported = featset.AreAllFeaturesSupported();
    TESTDIAGNOSTICERROR(!allSupported,
    	_L("TFeatureSet::AreAllFeaturesSupported - EFalse expected: return value is %d"),allSupported);
    
    // Test with two supported features and one unsupported feature
    TFeatureSet featset2;

    // Append two supported features in feature set array.
    User::LeaveIfError( featset2.Append( KDefaultSupportedUid ) );
    User::LeaveIfError( featset2.Append( KModifiableUid ) );

    // Query feature statuses from server and check statuses.
    CFeatureDiscovery::FeaturesSupportedL( featset2 );
    allSupported = featset2.AreAllFeaturesSupported();
    TESTDIAGNOSTICERROR(allSupported,
    	_L("TFeatureSet::AreAllFeaturesSupported - ETrue expected: return value is %d"),allSupported );
    
    // Append unsupported feature in feature set array.
    User::LeaveIfError( featset2.Append( KDefaultUnsupportedUid ) );
    // Query feature statuses from server and check statuses.
    CFeatureDiscovery::FeaturesSupportedL( featset2 );
    supported = featset2.IsFeatureSupported(KDefaultUnsupportedUid);
    TESTDIAGNOSTICERROR(!supported,
    	_L("TFeatureSet::IsFeatureSupported - EFalse expected: return value is %d"),supported);
    
    allSupported = featset2.AreAllFeaturesSupported();
    TESTDIAGNOSTICERROR(!allSupported,
    	_L("TFeatureSet::AreAllFeaturesSupported - EFalse expected: return value is %d"),allSupported );     
   
   return TestStepResult();
   }

CFeatureDiscoveryMultipleFeatureDynamicRequestStep::CFeatureDiscoveryMultipleFeatureDynamicRequestStep()
   {
   SetTestStepName(KFeatureDiscoveryMultipleFeatureDynamicRequest);
   }

TVerdict CFeatureDiscoveryMultipleFeatureDynamicRequestStep::doTestStepL()
   {
    CFeatureDiscovery* disc = NULL;
    TRAPD(err,disc = CFeatureDiscovery::NewL());
    TESTDIAGNOSTICERROR(err==KErrNone,_L("CFeatureDiscovery::NewLC failed: error = %d"),err);
    if (err)
    	return TestStepResult();
	
	CleanupStack::PushL(disc);  
    TFeatureSet featset;

    // Append supported feature in feature set array.
    User::LeaveIfError( featset.Append( KDefaultSupportedUid ) );
    
    // Query feature statuses from server and check status.
    err = disc->FeaturesSupported( featset );
    TESTDIAGNOSTICERROR(!err, 
    	_L("CFeatureDiscovery::FeaturesSupported - KErrNone expected; error = %d"),err);
    TBool supported = featset.IsFeatureSupported(KDefaultSupportedUid);
    TESTDIAGNOSTICERROR(supported, 
    	_L("TFeatureSet::IsFeatureSupported - ETrue expected; return value is %d"),supported);
    // Check even one which not appended in list
    supported = featset.IsFeatureSupported(KUnmodifiableUid);
    TESTDIAGNOSTICERROR(!supported, 
    	_L("TFeatureSet::IsFeatureSupported - EFalse expected; return value is %d"),supported);
    // Check whether all QUERIED features are supported
    TBool allSupported = featset.AreAllFeaturesSupported();
    TESTDIAGNOSTICERROR(allSupported, 
    	_L("TFeatureSet::AreAllFeaturesSupported  - ETrue expected; return value is %d"),allSupported);
    
    // Append one more supported feature.
    User::LeaveIfError( featset.Append( KModifiableUid ) );
    // Query feature statuses from server and check all QUERIED features are supported
    err = disc->FeaturesSupported( featset );
    TESTDIAGNOSTICERROR(!err, 
    	_L("CFeatureDiscovery::FeaturesSupported - KErrNone expected; error = %d"),err);
    allSupported = featset.AreAllFeaturesSupported();
    TESTDIAGNOSTICERROR(allSupported, 
    	_L("TFeatureSet::AreAllFeaturesSupported  - ETrue expected; return value is %d"),allSupported);
    
    // Append feature which is supposed to be non-existent and 
    // check whether features supported after that.
    User::LeaveIfError( featset.Append( KUnknownUid ) );
    err = disc->FeaturesSupported( featset );
    TESTDIAGNOSTICERROR(!err, 
    	_L("CFeatureDiscovery::FeaturesSupported - KErrNone expected; error = %d"),err);
    supported = featset.IsFeatureSupported( KUnknownUid );
    TESTDIAGNOSTICERROR(!supported, 
    	_L("TFeatureSet::IsFeatureSupported - EFalse expected; return value is %d"),supported);
    // Check whether all QUERIED features are supported
    allSupported = featset.AreAllFeaturesSupported();
    TESTDIAGNOSTICERROR(!allSupported, 
    	_L("TFeatureSet::AreAllFeaturesSupported  - EFalse expected; return value is %d"),allSupported);
    
    CleanupStack::PopAndDestroy( disc );
   
   return TestStepResult();
   }
