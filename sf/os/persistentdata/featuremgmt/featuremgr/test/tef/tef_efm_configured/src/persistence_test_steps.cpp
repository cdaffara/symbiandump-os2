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
// EFM test steps for persistence testing
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featmgr/featurecontrol.h>
#include "efm_teststepbase.h"
#include "persistence_test_steps.h"

CDeleteFeatStep::CDeleteFeatStep() 
	{
	SetTestStepName(KDeleteFeatureStep);
	}

TVerdict CDeleteFeatStep::doTestStepL()
	{
	// Open first session to featmgr  server
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
		  _L("RFeatureControl::Open failed: error = %d"),err);

	// Adding persisted feature
	TBitFlags32 persistflags( 0 );
	persistflags.Set( EFeaturePersisted);
	TFeatureEntry persistentry( KPersistedUid, persistflags, KDefaultData1 );
	// Adding persisted feature	
	err = control.AddFeature(persistentry);
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("Persisted feature addition failed: error = %d"),err);

	// Adding non-persisted feature
	TBitFlags32 nonpersistflags( 0 );
	nonpersistflags.Set( EFeatureModifiable);
	TFeatureEntry nonpersistentry( KNotPersistedUid, nonpersistflags, KDefaultData1 );
	// Adding non-persisted feature	
	err = control.AddFeature(nonpersistentry);
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("Non-persisted feature addition failed: error = %d"),err);
	
	// Close the first session to featmgr
	control.Close();	
	// Wait a while until the first session closes fully before starting a new one
	User::After(2000000);

	// Opening the second session to featmgr server
	err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
		  _L("RFeatureControl::Open failed: error = %d"),err);

	// Deleting persisted feature
	err = control.DeleteFeature(KPersistedUid);
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("Persited feature deletion failed: error = %d"),err);

	// Deleting non-persisted feature
	err = control.DeleteFeature(KNotPersistedUid);
	TESTDIAGNOSTICERROR(err==KErrNotFound,
			   _L("Non-persited feature deletion failed: error = %d"),err);

	// Close the second session to featmgr server
	control.Close();
	
	return TestStepResult();	
    }

CPersistenceStoreSettingsTestStep::CPersistenceStoreSettingsTestStep()
	{
	SetTestStepName(KPersistenceStoreSettingsTest);
	}

TVerdict CPersistenceStoreSettingsTestStep::doTestStepL()
	{
	RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
		_L("RFeatureControl::Open failed: error = %d"),err );

	//first check that the information for the supported feature is correct
	TFeatureEntry suppprted_entry(KDefaultSupportedUid);
    TInt supported = control.FeatureSupported( suppprted_entry );
    TESTDIAGNOSTIC(supported == KFeatureSupported,
          _L("KDefaultSupportedUid feature that is supposed to be supported is unsupported, test presets are broken"));
    TESTDIAGNOSTICERROR(suppprted_entry.FeatureData() == KDefaultData1,
          _L("RFeatureControl::FeatureData - Feature data is expected to be KDefaultData1 for a feature that has not been modified, returned value is = %x"),suppprted_entry.FeatureData());
	
	//change feature data
	err = control.SetFeature( KDefaultSupportedUid, KChangeData );
    TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::SetFeature - KErrNone expected; error = %d"),err );    
    
    //check that the information for the unsupported feature is correct
	TFeatureEntry unsuppprted_entry( KDefaultUnsupportedUid );
    supported = control.FeatureSupported( unsuppprted_entry );
    TESTDIAGNOSTIC( supported == KFeatureUnsupported,
          _L( "KDefaultUnsupportedUid feature that is supposed to be unsupported is supported, test presets are broken" ) );
    
	//change the feature state
	err = control.EnableFeature( KDefaultUnsupportedUid );
    TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::EnableFeature - KErrNone expected; error = %d"),err );    
	
	//check that the information for the modifiable feature is correct
	TFeatureEntry modifiable_entry( KModifiableUid );
    supported = control.FeatureSupported( modifiable_entry );
    TESTDIAGNOSTIC(supported == KFeatureSupported,
          _L("KModifiableUid feature that is supposed to be supported is unsupported, test presets are broken"));
    
	//delete modifiable feature to check that deleted features are persisted
	err = control.DeleteFeature(KModifiableUid);
	TESTDIAGNOSTICERROR( err == KErrAccessDenied,
              _L("RFeatureControl::DeleteFeature - KErrAccessDenied expected; error = %d"),err );        

    //check that the information for the not persisted feature is correct
	TFeatureEntry not_persisted_entry( KNotPersistedUid );
    supported = control.FeatureSupported( not_persisted_entry );
    TESTDIAGNOSTIC(supported == KFeatureSupported,
          _L("KNotPersistedUid feature that is supposed to be supported is unsupported, test presets are broken"));
    TESTDIAGNOSTICERROR(not_persisted_entry.FeatureData() == KDefaultData1,
            _L("RFeatureControl::FeatureData - Feature data is expected to be KDefaultData1 for a feature that has not been modified, returned value is = %x"),not_persisted_entry.FeatureData());

	//try to change the data and then disable the not persisted feature
	err = control.SetFeature( KNotPersistedUid, KChangeData );
    TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::SetFeature - KErrNone expected; error = %d"),err );        
	err = control.DisableFeature( KNotPersistedUid );
    TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::DisableFeature - KErrNone expected; error = %d"),err );
    //Build time Feature combinations
    TFeatureEntry persisted_entry( KModifiableUninitialisedPersistedUid );
    supported = control.FeatureSupported(persisted_entry );
    TESTDIAGNOSTICERROR(supported == KErrNotReady,
          _L("KModifiableUninitialisedPersistedUid  feature that is supposed to be KErrNotReady is supported, test presets are broken %"),supported);          
     
     err = control.EnableFeature( KModifiableUninitialisedPersistedUid );
	TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::AddFeature - KErrNone expected for a feature from supportedpersisted_entry; error = %d"),err );    
    err = control.FeatureSupported(KModifiableUninitialisedPersistedUid);
	TESTDIAGNOSTICERROR( err == KFeatureSupported,
              _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for a feature from DSR; error = %d"),err );    
         
    TFeatureEntry modifiablepersisted_entry( KModifiablePersistedUid );
    supported = control.FeatureSupported(modifiablepersisted_entry );
    TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
          _L("KModifiablePersistedUid  feature that is supposed to be FeatureUnsupported is supported, test presets are broken %"),supported);          
          err = control.EnableFeature( KModifiablePersistedUid );
	TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::AddFeature - KErrNone expected for a feature from supportedpersisted_entry; error = %d"),err );    
    err = control.FeatureSupported(KModifiablePersistedUid);
	TESTDIAGNOSTICERROR( err == KFeatureSupported,
              _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for a feature from DSR; error = %d"),err );    
              
     //Run time feature combinations 
     TBitFlags32 flags2( 0 );
    flags2.Set( EFeatureUninitialized );
    flags2.Set( EFeatureModifiable );
    flags2.Set( EFeaturePersisted );
    TFeatureEntry entry4( KNewUid3, flags2, KDefaultData1 );
    err = control.AddFeature( entry4 );
    TESTDIAGNOSTICERROR(err == KErrNone, 
             _L("RFeatureControl::AddFeature - KErrNone or KErrAlreadyExists expected; error = %d"),err);
    
    
    //check that a feature from DSR is supported
    supported = control.FeatureSupported( KDSR1Uid );
    TESTDIAGNOSTIC(supported == KFeatureSupported,
          _L("A feature from DSR is unsupported, test presets are broken"));
	
	//Add and disable the DSR feature to check that DSR features are persisted
	TBitFlags32 persistedFlags( 0 );
    persistedFlags.Set( EFeatureSupported );
	persistedFlags.Set( EFeatureModifiable );
    persistedFlags.Set( EFeaturePersisted );
    TFeatureEntry dsr_entry( KDSR1Uid, persistedFlags, KDefaultData1 );
	err = control.AddFeature( dsr_entry );
	TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::AddFeature - KErrNone expected for a feature from DSR; error = %d"),err );    
    err = control.DisableFeature(KDSR1Uid);
	TESTDIAGNOSTICERROR( err == KErrNone,
              _L("RFeatureControl::DisableFeature - KErrNone expected for a feature from DSR; error = %d"),err );    

    
	//add a new feature
	TFeatureEntry new_entry( KNewUid, persistedFlags, KChangeData );
    err = control.AddFeature( new_entry );
	TESTDIAGNOSTICERROR(KErrNone==err,
	         _L("Problem occured while adding a new feature: err=%d"),err);
	
	//add a new feature without the "persisted" flag
	TBitFlags32 notPersistedFlags( 0 );
	notPersistedFlags.Set( EFeatureSupported );
	notPersistedFlags.Set( EFeatureModifiable );    
    TFeatureEntry new_entry2( KNewUid2, notPersistedFlags, KChangeData );
    err = control.AddFeature( new_entry2 );
	TESTDIAGNOSTICERROR(KErrNone==err,
	         _L("Problem occured while adding a new feature: err=%d"),err);
	
	//Close the connection with the server and wait for the server to be  terminated
	control.Close();
	return TestStepResult();
	}

CPersistenceLoadSettingsTestStep::CPersistenceLoadSettingsTestStep()
	   {
	   SetTestStepName(KPersistenceLoadSettingsTest);
	   }

TVerdict CPersistenceLoadSettingsTestStep::doTestStepL()
{
	RFeatureControl control;
	//reopen the connection with the server
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
		_L("RFeatureControl::Open failed while reopening the connection: error = %d"),err);
	
	//now check that the features have been persisted
	
	//check that the changed feature data has been persisted
	TFeatureEntry supported_entry( KDefaultSupportedUid );
	TInt supported = control.FeatureSupported( supported_entry );
	TESTDIAGNOSTIC( supported == KFeatureSupported,
	      _L("KDefaultSupportedUid feature that is supposed to be supported is unsupported after the server restart") );
	TESTDIAGNOSTICERROR(supported_entry.FeatureData() == KChangeData,
	      _L("RFeatureControl::FeatureData - Feature data has not been persisted, returned value is = %x"),supported_entry.FeatureData());
	
	//check that feature state has been persisted
	TFeatureEntry unsuppprted_entry( KDefaultUnsupportedUid );
	supported = control.FeatureSupported( unsuppprted_entry );
	TESTDIAGNOSTIC( supported == KFeatureSupported,
	      _L( "KDefaultUnsupportedUid feature that is supposed to be supported is unsupported after the server restart" ) );
	      
	
	TFeatureEntry modifiableUninitialisedPersistedsupported_entry( KModifiableUninitialisedPersistedUid );
	 supported = control.FeatureSupported( modifiableUninitialisedPersistedsupported_entry );
	TESTDIAGNOSTIC( supported == KFeatureSupported,
	      _L("KModifiableUninitialisedPersistedUid feature that is supposed to be supported is unsupported after the server restart") );
	      
	TFeatureEntry modifiablePersistedsupported_entry( KModifiablePersistedUid );
	supported = control.FeatureSupported(modifiablePersistedsupported_entry );
	TESTDIAGNOSTIC( supported == KFeatureSupported,
	      _L("KModifiablePersistedUid feature that is supposed to be supported is unsupported after the server restart") );
	      
	      
	TFeatureEntry modifiableUninitialiseRuntimePersistedsupported_entry( KNewUid3 );
	supported = control.FeatureSupported(modifiableUninitialiseRuntimePersistedsupported_entry );
	TESTDIAGNOSTIC( supported == KErrNotReady,
	      _L("KNewUid3 feature that is supposed to be supported is unsupported after the server restart") );
	
	
	//check that the feature without the "persisted" flag hasn't been persisted
	TFeatureEntry not_persisted_entry( KNotPersistedUid );
	supported = control.FeatureSupported( not_persisted_entry );
	TESTDIAGNOSTIC(supported == KFeatureSupported,
	      _L("The feature without the 'persisted' flag should not be persisted"));
	TESTDIAGNOSTICERROR(not_persisted_entry.FeatureData() == KDefaultData1,
	        _L("RFeatureControl::FeatureData - Feature data for the feature without the 'persisted' flag has been persisted, returned value is = %x"),not_persisted_entry.FeatureData());	    
	
	//check that the deleted feature has been persisted
	TFeatureEntry modifiable_entry( KModifiableUid );
	supported = control.FeatureSupported( modifiable_entry );
	TESTDIAGNOSTICERROR(supported==KFeatureSupported,
	         _L("This feature Deleted feature should be reported supported as build time added features cannot be deleted, returned value is %d"),supported);
	
	//check that the feature from DSR has been persisted
	supported = control.FeatureSupported( KDSR1Uid );
	TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
	      _L("A feature from DSR has not been persisted, return value is %d"),supported );
	
	//check that added feature has been persisted
	TFeatureEntry new_entry( KNewUid );
	supported = control.FeatureSupported( new_entry );
	TESTDIAGNOSTICERROR(supported==KFeatureSupported,
	         _L("Newly added feature should be supported, returned value is %d"), supported);
	TESTDIAGNOSTICERROR(new_entry.FeatureData()==KChangeData,
			 _L("Data assigned to the added feature is expected to be KChangeData, returned value is %d"),new_entry.FeatureData());
	
	//check that added feature whithout the "persisted" flag has not been persisted
	supported = control.FeatureSupported( KNewUid2 );
	TESTDIAGNOSTICERROR(supported==KErrNotFound,
	         _L("Newly added feature without the 'persisted' flag should not be persisted, returned value is %d"), supported);
	
	control.Close();
	return TestStepResult();
}
