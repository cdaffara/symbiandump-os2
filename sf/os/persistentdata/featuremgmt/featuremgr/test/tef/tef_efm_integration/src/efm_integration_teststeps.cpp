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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
#include <featmgr/featurecontrol.h>
#include <featdiscovery.h>
#include "efm_integration_teststeps.h"

//the header file generated with the "features" tool based on singlesection_test_featuredatabase.xml
#include "singlesection_test_featureUIDs.h"

//the header file generated with the "features" tool based on multisection_test_featuredatabase_rofs2.xml
#include "multisection_test_test_featuredatabase_rofs2.h"

using namespace NFeature;

//a DSR feature not defined in the XML file
const TUid KTestDSRUid2 = {0x10281802};

CSingleRomSectionTestStep::CSingleRomSectionTestStep()
	{
	SetTestStepName(KSingleRomSectionTest);	
	}

TVerdict CSingleRomSectionTestStep::doTestStepL()	
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
    INFO_PRINTF2(_L("Supported features count in the rom %d"), count);
    
	for(TInt i = 0; i < count; i++)
       {
         INFO_PRINTF2( _L("uid 0x%08x"), supportedFeatures[i].iUid );
       }

	RFeatureArray uids;
	uids.Append(KTestSupportedUid );
	uids.Append(KTestUpgradableUid );
	uids.Append(KTestModifiableUid);
	uids.Append(KTestBlacklistedUid);
	uids.Append(KTestPersistedUid);
	uids.Append(KTestUnsupportedUid);
	uids.Append(KTestOverwrittenByIBYUid);
	uids.Append(KTestDSRUid1);
	uids.Append(KTestDSRUid2);
	TInt err1 = control.FeaturesSupported( uids );
	TInt index = uids.Find( KTestSupportedUid );
	if(index == KErrNotFound)
		{
		INFO_PRINTF2( _L("uid not found = 0x%08x"),KTestSupportedUid  );
		SetTestStepResult(EFail);
		}
	   
	else
		{
		TBool featureSupported = uids[index].FeatureFlags().IsSet( EFeatureSupported );
		TESTDIAGNOSTICERROR(featureSupported ,_L("EFeatureSupported flag is set to '1' for KTestSupportedUid feature in the xml file; returned value is %d"),featureSupported );
		}
	    	    	
	index = uids.Find( KTestUnsupportedUid );
   
	if(index == KErrNotFound)
		{
		INFO_PRINTF2( _L("uid not found = 0x%08x"),KTestUnsupportedUid  );
		SetTestStepResult(EFail);
		}	   
	else
		{
		TBool featureSupported = uids[index].FeatureFlags().IsSet( EFeatureSupported );
		TESTDIAGNOSTICERROR(!featureSupported ,_L("EFeatureSupported flag is set to '0' for KTestUnsupportedUid feature in the xml file; returned value is %d"),featureSupported );
		}
	    
	index = uids.Find( KTestUpgradableUid );
  
	if(index == KErrNotFound)
		{
		INFO_PRINTF2( _L("uid not found = 0x%08x"),KTestUpgradableUid  );
		SetTestStepResult(EFail);
		}
	else
		{	     
		TBool upgradable_flag_value = uids[index].FeatureFlags().IsSet( EFeatureUpgradeable );
		TESTDIAGNOSTICERROR(upgradable_flag_value ,_L("EFeatureUpgradeable flag is set to '1' for KTestUpgradableUid feature in the xml file; returned value is %d"),upgradable_flag_value);      	
		}	
   
	index = uids.Find( KTestModifiableUid);	   
	if(index == KErrNotFound)
		{
		INFO_PRINTF2( _L("uid not found = 0x%08x"),KTestModifiableUid);
		SetTestStepResult(EFail);
		}
	else
		{    	
		TBool modifiable_flag_value = uids[index].FeatureFlags().IsSet( EFeatureModifiable );
		TESTDIAGNOSTICERROR(modifiable_flag_value ,_L("EFeatureModifiable is set to '1' for KTestModifiableUid feature in the xml file; returned value is %d"),modifiable_flag_value);     	
		}	
	    
	index = uids.Find( KTestBlacklistedUid );	   
	if(index == KErrNotFound)	   
		{
		INFO_PRINTF2( _L("uid not found = 0x%08x"), KTestBlacklistedUid);
		SetTestStepResult(EFail);
		}	   
	else
		{	     
		TBool blacklisted_flag_value = uids[index].FeatureFlags().IsSet( EFeatureBlackListed  );
		TESTDIAGNOSTICERROR(blacklisted_flag_value, _L("EFeatureBlackListed is set to '1' for KTestBlacklistedUid feature in the xml file; returned value is %d"),blacklisted_flag_value);
		}	
	   
	index = uids.Find( KTestPersistedUid );
	if(index == KErrNotFound)	   
		{
		INFO_PRINTF2( _L("uid not found = 0x%08x"),KTestPersistedUid  );
		SetTestStepResult(EFail);
		}
	else
   		{
   		TBool persisted_flag_value = uids[index].FeatureFlags().IsSet(  EFeaturePersisted  );
   		TESTDIAGNOSTICERROR(persisted_flag_value,_L("EFeatureBlackListed is set to '1' for KTestPersistedUid feature in the xml file; returned value is %d"),persisted_flag_value);
   		}
   
	index = uids.Find( KTestOverwrittenByIBYUid );
	if(index == KErrNotFound)
		{
   		INFO_PRINTF2( _L("uid not found = 0x%08x"),KTestOverwrittenByIBYUid );
   		SetTestStepResult(EFail);
		}     
	else
		{
		TBool featureSupported = uids[index].FeatureFlags().IsSet(EFeatureSupported);
		TESTDIAGNOSTICERROR(featureSupported,_L("EFeatureSupported flag is set to '1' for KTestOverwrittenByIBY feature in the IBY file; returned value is %d"),featureSupported );
		TUint32 data = uids[index].FeatureData();
		TESTDIAGNOSTICERROR(data==1,_L("The feature data for KTestOverwrittenByIBY is expected to be 1 as defined in the IBY file; returned value is %d"),data);
		}
	    	    
	TInt not_ready = control.FeatureSupported(KTestUninitialisedUid);
	TESTDIAGNOSTICERROR(not_ready == KErrNotReady,_L("KErrNotReady is expected for KTestUninitialisedUid; return values is  %d"),not_ready );

	index = uids.Find( KTestDSRUid1 );
	if(index == KErrNotFound)
		{
		INFO_PRINTF2( _L("uid not found = 0x%08x"),KTestDSRUid1);
		SetTestStepResult(EFail);
		}  
	else
		{	
		TBool featureSupported = uids[index].FeatureFlags().IsSet( EFeatureSupported );
		TESTDIAGNOSTICERROR(!featureSupported ,_L("The feature KTestDSRUid1 is defined as unsupported in the xml file; the return valuse is  %d"),featureSupported );
		}
  
	index = uids.Find( KTestDSRUid2 );
	if(index == KErrNotFound)
  		{
  		INFO_PRINTF2( _L("uid not found = 0x%08x"),KTestDSRUid2);
  		SetTestStepResult(EFail);
  		}
	else
		{
		TBool featureSupported = uids[index].FeatureFlags().IsSet(EFeatureSupported);
		TESTDIAGNOSTICERROR(featureSupported,_L("EFeatureSupported flag must be set to '1' for a feature from DSR; returned value is %d"),featureSupported );
		}
  
	supportedFeatures.Close();
  
	control.Close();
	return TestStepResult();
	}


CMultipleRomSectionsTestStep::CMultipleRomSectionsTestStep()
	{
	SetTestStepName(KMultipleRomSectionsTest);	
	}

TVerdict CMultipleRomSectionsTestStep::doTestStepL()
	{   
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
    		_L("RFeatureControl::Open failed: error = %d"),err);
    		
	TInt supported = 0;
   
	supported = control.FeatureSupported(KTestCompROMUid1);
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,_L("KFeatureSupported expected for KTestCompROMUid1; returned value is %d"),supported);
      
	supported = control.FeatureSupported(KTestCompROMUid2);
	TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,_L("KFeatureUnsupported expected for KTestCompROMUid2; returned value is %d"),supported);
      
	supported = control.FeatureSupported(KTestCompROMUid3);
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,_L("KFeatureSupported expected for KTestCompROMUid3; returned value is %d"),supported);
   
	supported = control.FeatureSupported(KTestCompROMUid4);
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,_L("KFeatureSupported expected for KTestCompROMUid4; returned value is %d"),supported);
   
	supported = control.FeatureSupported(KTestCompROMUid5);
	TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,_L("KFeatureUnsupported expected for KTestCompROMUid5; returned value is %d"),supported);
            
	supported = control.FeatureSupported(KTestCompROMUid6);
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,_L("KFeatureSupported expected for KTestCompROMUid6; returned value is %d"),supported);
      
	supported = control.FeatureSupported(KTestCompROMUid7);
	TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,_L("KFeatureUnsupported expected for KTestCompROMUid7; returned value is %d"),supported);
   
	supported = control.FeatureSupported(KTestCompROMUid8);
	TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,_L("KFeatureUnsupported expected for KTestCompROMUid8; returned value is %d"),supported);
   
	TFeatureEntry feature1(KTestCompROMUid9);   
	supported = control.FeatureSupported(feature1);
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,_L("KFeatureSupported expected for KTestCompROMUid9; returned value is %d"),supported);
	TESTDIAGNOSTICERROR(feature1.FeatureData() == 0x1,_L("0x1 is expected for the user data for KTestCompROMUid9; returned value is %d"),feature1.FeatureData());
   
	TFeatureEntry feature2(KTestCompROMUid10);
	supported = control.FeatureSupported(feature2);
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,_L("KFeatureSupported expected for KTestCompROMUid10; returned value is %d"),supported);
	TESTDIAGNOSTICERROR(feature2.FeatureData() == 0x1,_L("0x1 is expected for the user data for KTestCompROMUid10; returned value is %d"),feature2.FeatureData());
   
	control.Close();
	return TestStepResult();
	}

CFeatRegVerificationTestStep::CFeatRegVerificationTestStep()
	{
	SetTestStepName(KFeatRegVerificationTest);	
	}

TVerdict CFeatRegVerificationTestStep::doTestStepL()
	{   
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
    		_L("RFeatureControl::Open failed: error = %d"),err);
    		
	TInt supported = 0;
   
	supported = control.FeatureSupported(KTestSupportedUid);
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,_L("KFeatureSupported expected for KTestSupportedUid; returned value is %d"),supported);
      
	supported = control.FeatureSupported(KTestUnsupportedUid);
	TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,_L("KFeatureUnsupported expected for KTestUnsupportedUid; returned value is %d"),supported);
      
	supported = control.FeatureSupported(KTestDSRUid1);
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,_L("KFeatureSupported expected for KTestDSRUid1; returned value is %d"),supported);
      
	control.Close();
	return TestStepResult();
	}

