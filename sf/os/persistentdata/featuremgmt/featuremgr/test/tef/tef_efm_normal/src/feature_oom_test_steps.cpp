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
// Feature Control test steps to be executed with the TEF '!OOM' option
// to run the 'doTestStepL' method in low memory conditions. Required
// resources (usually an RFeatureControl object) are member variables and are
// set up/destroyed in 'doTestStepPreambleL' and 'doTestStepPostambleL'
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featmgr/featurecontrol.h>
#include "feature_oom_test_steps.h"

//
// This test does not have a preamble or postamble.
// The constructor is in the header, only sets the test name.
//
TVerdict CFeatureOpenOOMTestStep::doTestStepL()
    {
    RFeatureControl control;
    TInt err = control.Open();       
    // Actually we expect an error, but TEF ignores it and just looks for
    // memory leaks.
    TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::Connect failed: error = %d"),err);
    control.Close();
    return TestStepResult();
    }


TVerdict CFeatureSupportedOOMTestStep::doTestStepPreambleL()
    {
    TInt err = icontrol.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::Connect failed: error = %d"),err);
    return TestStepResult();
    }

// See if we get a memory leak using 'FeatureSupported'. I doubt we'll even see
// an out of memory issue here (despite the environment).
TVerdict CFeatureSupportedOOMTestStep::doTestStepL()
    {
    TInt supported = icontrol.FeatureSupported( KDefaultSupportedUid );    
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
        _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for supported feature; returned value is %d"), supported);

    supported = icontrol.FeatureSupported( KDefaultUnsupportedUid );
    TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
        _L("RFeatureControl::FeatureSupported - KFeatureUnsupported expected for unsupported feature; returned value is %d"), supported);

    supported = icontrol.FeatureSupported( KUninitializedUid );
    TESTDIAGNOSTICERROR(supported == KErrNotReady,
        _L("RFeatureControl::FeatureSupported - KErrNotReady expected for uninitialized feature; returned value is %d"),supported);
    return TestStepResult();
    }

// Close the RFeatureControl member.
TVerdict CFeatureSupportedOOMTestStep::doTestStepPostambleL()
    {
    icontrol.Close();
    return TestStepResult();
    }

// See if we get a memory leak using 'EnableFeature'. I doubt we'll even see
// an out of memory issue here (despite the environment). TEF will ignore
// errors (with the !OOM option).
TVerdict CFeatureEnableOOMTestStep::doTestStepL()
    {
    TInt err = icontrol.EnableFeature( KModifiableUid );
    TESTDIAGNOSTICERROR(err == KErrNone,
              _L("RFeatureControl::EnableFeature - KErrNone expected; error = %d"),err);                    

    err = icontrol.EnableFeature( KUnmodifiableUid );
    TESTDIAGNOSTICERROR(err == KErrAccessDenied,
              _L("RFeatureControl::EnableFeature - KErrAccessDenied expected; error = %d"),err);                        

    err = icontrol.EnableFeature( KUnknownUid );
    TESTDIAGNOSTICERROR(err == KErrNotFound,
              _L("RFeatureControl::EnableFeature - KErrNotFound expected; error = %d"),err);                    
       
    err = icontrol.EnableFeature( KPersistedUid );
    TESTDIAGNOSTICERROR(err == KErrAccessDenied,
    _L("RFeatureControl::EnableFeature - KErrAccessDenied expected; error = %d"),err);                    
    return TestStepResult();
    }

// See if we get a memory leak using 'DisableFeature'. I doubt we'll even see
// an out of memory issue here (despite the environment). TEF will ignore
// errors (with the !OOM option).
TVerdict CFeatureDisableOOMTestStep::doTestStepL()
    {
    TInt err = icontrol.DisableFeature( KModifiableUid );
    TESTDIAGNOSTICERROR(err == KErrNone,
              _L("RFeatureControl::DisableFeature - KErrNone expected; error = %d"),err);                    

    err = icontrol.DisableFeature( KUnmodifiableUid );
    TESTDIAGNOSTICERROR(err == KErrAccessDenied,
              _L("RFeatureControl::DisableFeature - KErrAccessDenied expected; error = %d"),err);                        

    err = icontrol.DisableFeature( KUnknownUid );
    TESTDIAGNOSTICERROR(err == KErrNotFound,
              _L("RFeatureControl::DisableFeature - KErrNotFound expected; error = %d"),err);                    
    return TestStepResult();
    }

// See if we get a memory leak using 'SetFeature'. I doubt we'll even see
// an out of memory issue here (despite the environment). TEF will ignore
// errors (with the !OOM option).
TVerdict CFeatureSetFeatureOOMTestStep::doTestStepL()
    {
    TInt err = icontrol.SetFeature( KDefaultSupportedUid, KDefaultData1 );
    TESTDIAGNOSTICERROR(err == KErrNone,       
        _L("RFeatureControl::SetFeature - KErrNone expected; error = %d"),err);       
    err = icontrol.SetFeature( KDefaultUnsupportedUid, KDefaultData1 );
    TESTDIAGNOSTICERROR(err == KErrNone,       
        _L("RFeatureControl::SetFeature - KErrNone expected; error = %d"),err);

    err = icontrol.SetFeature( KUnmodifiableUid, KDefaultData1 );
    TESTDIAGNOSTICERROR(err == KErrAccessDenied, 
        _L("RFeatureControl::SetFeature - KErrAccessDenied expected; error = %d"),err);
    
    err = icontrol.SetFeature( KUnknownUid, KDefaultData1 );
    TESTDIAGNOSTICERROR(err == KErrNotFound, 
    _L("RFeatureControl::SetFeature - KErrNotFound expected; error = %d"),err);
        
    return TestStepResult();
    }

// See if we get a memory leak using 'AddFeature'. I doubt we'll even see
// an out of memory issue here (despite the environment). TEF will ignore
// errors (with the !OOM option).
TVerdict CFeatureAddFeatureOOMTestStep::doTestStepL()
    {
    // Try to add already existing feature.
    TBitFlags32 flags( 0 );
    flags.Set( EFeatureSupported );
    flags.Set( EFeatureModifiable );
    TFeatureEntry entry( KDefaultSupportedUid, flags, KDefaultData1 );
    TInt err = icontrol.AddFeature( entry );
    TESTDIAGNOSTICERROR(err == KErrAlreadyExists,
       _L("RFeatureControl::AddFeature - KErrAlreadyExists expected; error = %d"),err);
    
    // Add new feature. If test run sequentially, feature already exists 
    TFeatureEntry entry2( KNewUid, flags, KDefaultData1 );
    err = icontrol.AddFeature( entry2 );
    TESTDIAGNOSTICERROR(err == KErrNone || err == KErrAlreadyExists, 
      _L("RFeatureControl::AddFeature - KErrNone or KErrAlreadyExists expected; error = %d"),err);

    TBitFlags32 flags1( 0 );
    flags1.Set( EFeatureUninitialized );
    flags1.Set( EFeatureModifiable );
    TFeatureEntry entry3( KNewUid1, flags1, KDefaultData1 );
    err = icontrol.AddFeature( entry3 );
    TESTDIAGNOSTICERROR(err == KErrNone || err == KErrAlreadyExists, 
       _L("RFeatureControl::AddFeature - KErrNone or KErrAlreadyExists expected; error = %d"),err);
       
    return TestStepResult();
    }

// See if we get a memory leak using 'DeleteFeature'. I doubt we'll even see
// an out of memory issue here (despite the environment). TEF will ignore
// errors (with the !OOM option).
TVerdict CFeatureDelFeatureOOMTestStep::doTestStepPreambleL()
    {
    CFeatureSupportedOOMTestStep::doTestStepPreambleL();
    TBitFlags32 flags1( 0 );
    flags1.Set( EFeatureUninitialized );
    flags1.Set( EFeatureModifiable );
    flags1.Set( EFeaturePersisted );
    TFeatureEntry entry4( KNewUid3, flags1, KDefaultData1 );
    // Add a feature so we can attempt to delete it later..
    TInt err = icontrol.AddFeature( entry4 );
    TESTDIAGNOSTICERROR(err == KErrNone || err == KErrAlreadyExists, 
       _L("CFeatureDelFeatureOOMTestStep::doTestStepPreamble - KErrNone or KErrAlreadyExists expected adding feature; error = %d"),err);
    return TestStepResult();
    }

TVerdict CFeatureDelFeatureOOMTestStep::doTestStepL()
    {
    TInt err = icontrol.DeleteFeature( KNewUid3 );
    TESTDIAGNOSTICERROR(err == KErrNone || err == KErrNotFound, 
       _L("CFeatureDelFeatureOOMTestStep::doTestStepL - KErrNone or KErrNotFound expected deleting feature; error = %d"),err);
    return TestStepResult();
    }    

// Close the RFeatureControl member. Attempt to remove the feature added in
// the preamble.
TVerdict CFeatureDelFeatureOOMTestStep::doTestStepPostambleL()
    {
    TInt err = icontrol.DeleteFeature( KNewUid3 );
    TESTDIAGNOSTICERROR(err == KErrNone || err == KErrNotFound, 
       _L("CFeatureDelFeatureOOMTestStep::doTestStepPostamble - KErrNone or KErrNotFound expected deleting feature; error = %d"),err);
    CFeatureSupportedOOMTestStep::doTestStepPostambleL();
    return TestStepResult();
    }


// See if we get a memory leak using 'ListSupportedFeatures'.
// More to likely to give an OOM failure (which we want) than the above tests,
// because we are dealing with more information.
TVerdict CListSupportedFeaturesOOM::doTestStepL()
    {
    RFeatureUidArray supportedFeatures;
    TInt err = icontrol.ListSupportedFeatures( supportedFeatures );
    TESTDIAGNOSTICERROR(err==KErrNone || err==KErrNoMemory,
        _L("RFeatureControl::ListSupportedFeatures failed: error = %d"),err);
    
    if (err!=KErrNoMemory)
    	{
    	//print the content of the array
    	TInt count = supportedFeatures.Count();
    	INFO_PRINTF2(_L("Supported features count %d"), count);
    	for(TInt i = 0; i < count; i++)
    		{
    		INFO_PRINTF2( _L("uid 0x%08x"), supportedFeatures[i].iUid );
    		}
    	//check that the supported feature is listed
    	TESTDIAGNOSTICERROR(KErrNotFound!=supportedFeatures.Find(KDefaultSupportedUid),
    			_L("RFeatureControl::ListSupportedFeatures has not found the feature that is supported: error = %d"),err);
    
    	supportedFeatures.Close();
    	}
    
    return TestStepResult();
    }

