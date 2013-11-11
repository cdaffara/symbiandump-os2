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
// EFM test steps for reconciliation testing
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featdiscovery.h>
#include <featmgr/featurecontrol.h>
#include "efm_configured_testserver.h"
#include "reconciliation_test_steps.h"
#include "efm_teststepbase.h"

CReconciliationTestStep ::CReconciliationTestStep ()
   {
   SetTestStepName(KReconciliationTest );
   }

TVerdict CReconciliationTestStep::doTestStepL()
   {
    RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
    _L("RFeatureControl::Open failed: error = %d"),err);
 
    RFeatureUidArray supportedFeatures;
    err = control.ListSupportedFeatures( supportedFeatures );
    TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::ListSupportedFeatures failed: error = %d"),err);
    
    TInt count = supportedFeatures.Count();
    INFO_PRINTF2(_L("Supported features count %d"), count);
    for(TInt i = 0; i < count; i++)
        {
        INFO_PRINTF2( _L("uid 0x%08x"), supportedFeatures[i].iUid );
        }
    
    supportedFeatures.Close(); 
 
    TInt supported = control.FeatureSupported( KDefaultSupportedUid );    
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("Reconciliation test - KFeatureSupported expected for a feature defined as supported in the persisted file; returned value is %d"),supported);
    
    supported = control.FeatureSupported( KBlacklistedUid );
    TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
          _L("Reconciliation test - KFeatureUnsupported expected for a feature blacklisted in the Z:-drive data file; returned value is %d"),supported);
    
	supported = control.FeatureSupported( KModifiableUid );
    TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
          _L("Reconciliation test - KFeatureUnsupported expected for a feature disabledin in the C:-drive data file; returned value is %d"),supported);

    supported = control.FeatureSupported( KSupportedUpgradeableUid );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("Reconciliation test - KFeatureUnsupported expected for a feature upgradeable in the Z:-drive data file; returned value is %d"),supported);

 	supported = control.FeatureSupported( KSupportedUpgradeableBlacklistedUid );
    TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
          _L("Reconciliation test - KFeatureUnsupported expected for a feature UpgradeableBlacklisted in the Z:-drive data file; returned value is %d"),supported);

 	supported = control.FeatureSupported( KSupportedUpgradeableModifiableUid );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("Reconciliation test - KFeatureUnsupported expected for a feature UpgradeableModifiabled in the Z:-drive data file; returned value is %d"),supported);

 	supported = control.FeatureSupported( KSupportedUpgradeableModifiableUninitialisedUid );
    TESTDIAGNOSTICERROR(supported == KErrNotReady,
          _L("Reconciliation test - KFeatureUnsupported expected for a feature UpgradeableModifiableBlacklisted in the Z:-drive data file; returned value is %d"),supported);
	
		supported = control.FeatureSupported( KSupportedUpgradeableModifiablePersistedUid );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("Reconciliation test - KFeatureUnsupported expected for a feature UpgradeableModifiablePersisted in the Z:-drive data file; returned value is %d"),supported);


	control.Close();
    return TestStepResult();
   }

CFeatureManagerPanicswithInvaliedFlagsetsStep::CFeatureManagerPanicswithInvaliedFlagsetsStep()
   {
   SetTestStepName(KFeatureManagerPanicswithInvaliedFlagsets);
   }       
      
TVerdict CFeatureManagerPanicswithInvaliedFlagsetsStep::doTestStepL()
    {
    RFeatureControl control;
    TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrGeneral,
          _L("RFeatureControl::Open should fail with KErrGeneral: error = %d"),err);
    control.Close();
    
    return TestStepResult();
    } 
    
CFeatureManagerInvalidFlagsetsRuntimeStep::CFeatureManagerInvalidFlagsetsRuntimeStep()
   {
   SetTestStepName(KFeatureManagerInvalidFlagsetsRuntime);
   }       
      
TVerdict CFeatureManagerInvalidFlagsetsRuntimeStep::doTestStepL()
    {
    RFeatureControl control;
    
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::Open: KErrNone error cod e expected: error = %d"),err);
    TFeatureEntry suppprted_entry(KDefaultSupportedUid);
    TInt supported = control.FeatureSupported( suppprted_entry );
    TESTDIAGNOSTICERROR(supported == KErrNotFound,
          _L("KDefaultSupportedUid feature expected to be unknown because of invalid status flag combination; the value is = %d"),supported);
    control.Close();
    
    return TestStepResult();
    }    

CFeatureControlDeleteStep::CFeatureControlDeleteStep()
   {
   SetTestStepName(KFeatureControlDelete);
   }       
      
TVerdict CFeatureControlDeleteStep::doTestStepL()
    {
    RFeatureControl control;
    TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::Open should return with KErrNone: error = %d"),err);
    const TUid KNewModifiableUid = { 0x00000201 };
    const TUid KNewNonModifiableUid = { 0x00000202 };
    const TUid KNewDSRUid = {0x10000001  };
    const TUid KNewDSRUid1 = {0x10000002  };
    const TUid KNonExist =  {0x00000301 };
    // Try to delete a feature which in Z drive.
    TFeatureEntry suppprted_entry(KDefaultSupportedUid);
    TInt supported = control.FeatureSupported( suppprted_entry );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("KDefaultSupportedUid feature that is supposed to be supported is unsupported = %d"),supported);
    
    err = control.DeleteFeature(KDefaultSupportedUid);

	TESTDIAGNOSTICERROR( err == KErrAccessDenied,
              _L("RFeatureControl::DeleteFeature - KErrAccessDenied expected as the feature is there in the z: drive; error = %d"),err ); 
    
    //Try to delete modifiable feature
    TBitFlags32 flags1(0);
    flags1.Set( EFeatureModifiable );
    TFeatureEntry entry1( KNewModifiableUid, flags1, KDefaultData1 );
    err = control.AddFeature( entry1 );
    TESTDIAGNOSTICERROR(err == KErrNone, 
             _L("RFeatureControl::AddFeature - KErrNone  expected; error = %d"),err);
    err = control.DeleteFeature(KNewModifiableUid);

	TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::DeleteFeature - KErrNone expected; error = %d"),err );    
              
    //Try to delete non modifiable feature    

     TBitFlags32 flags2(0), flags3(0);
      flags2.Set(EFeatureSupported);
    
    TFeatureEntry entry2( KNewNonModifiableUid, flags2, KDefaultData1 );
    err = control.AddFeature( entry2 );
    TESTDIAGNOSTICERROR(err == KErrNone, 
             _L("RFeatureControl::AddFeature - KErrNone expected; error = %d"),err);
    
    err = control.EnableFeature(KNewNonModifiableUid);
    TESTDIAGNOSTICERROR( err == KErrAccessDenied,
              _L("RFeatureControl::DeleteFeature - KErrAccessDenied expected; error = %d"),err );
    
  
    err = control.DeleteFeature(KNewNonModifiableUid);

	TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::DeleteFeature - KErrNone expected; error = %d"),err );        
              
    //Try to delte non existent feature
    
    err = control.DeleteFeature(KNonExist);

	TESTDIAGNOSTICERROR( err == KErrNotFound,
              _L("RFeatureControl::DeleteFeature - KErrNotFound expected; error = %d"),err ); 
              
    //Try to delete DSR Range feature 
    
    err = control.FeatureSupported(KNewDSRUid1);
    TESTDIAGNOSTICERROR( err == KFeatureSupported,
              _L("RFeatureControl::DeleteFeature - KFeatureSupported expected for a feature from DSR; error = %d"),err );    
    TFeatureEntry dsrZdrive_entry( KNewDSRUid1);
    err = control.AddFeature( dsrZdrive_entry );
	TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::AddFeature - KErrNone expected for a feature from DSR; error = %d"),err );    
    err = control.DeleteFeature(KNewDSRUid1);
    TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::DeleteFeature - KErrNone expected for a feature from DSR; error = %d"),err );    
    err = control.FeatureSupported(KNewDSRUid1);
    TESTDIAGNOSTICERROR( err == KFeatureSupported,
              _L("RFeatureControl::DeleteFeature - KFeatureSupported expected for a feature from DSR; error = %d"),err ); 


    //Try to delete DSR Range feature which is overwritten at the run time.
    TBitFlags32 flags4( 0 );
    flags3.Set( EFeatureSupported );
    flags3.Set( EFeatureModifiable );
	TFeatureEntry dsr_entry( KNewDSRUid, flags3, KDefaultData1 );
	err = control.AddFeature( dsr_entry );
	TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::AddFeature - KErrNone expected for a feature from DSR; error = %d"),err );    
    err = control.DisableFeature(KNewDSRUid);
	TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::DisableFeature - KErrNone expected for a feature from DSR; error = %d"),err );    
              
    err = control.FeatureSupported(KNewDSRUid);
    TESTDIAGNOSTICERROR( err == KFeatureUnsupported,
              _L("RFeatureControl::DeleteFeature - KFeatureSupported expected for a feature from DSR; error = %d"),err );    
    
    err = control.DeleteFeature(KNewDSRUid);
    TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::DeleteFeature - KErrNone expected for a feature from DSR; error = %d"),err );    
              
    err = control.FeatureSupported(KNewDSRUid);
    TESTDIAGNOSTICERROR( err == KFeatureSupported,
              _L("RFeatureControl::DeleteFeature - KFeatureSupported expected for a feature from DSR; error = %d"),err ); 
                        
    control.Close();

    return TestStepResult();
    }      

