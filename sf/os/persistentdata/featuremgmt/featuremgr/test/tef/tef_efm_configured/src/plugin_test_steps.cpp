// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// EFM test steps for plugin testing
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featdiscovery.h>
#include <featmgr/featurecontrol.h>
#include "plugin_test_steps.h"
#include "efm_teststepbase.h"
#include "featmgrconfiguration.h"
#include <e32property.h>
#include "e32cmn.h"

static _LIT_SECURITY_POLICY_PASS(KPass);
static _LIT_SECURITY_POLICY_C1( KRead, ECapabilityReadDeviceData);
static _LIT_SECURITY_POLICY_C1( KWrite, ECapabilityWriteDeviceData);	
	

CInvalidPluginTest::CInvalidPluginTest()
	{
	SetTestStepName( KInvalidPluginTest );
	}

TVerdict CInvalidPluginTest::doTestStepL()
	{
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrGeneral,
	_L("RFeatureControl::Open succeeded, it is expected to fail with KErrGeneral; error = %d"),err);
	control.Close();
	return TestStepResult();
	}

CRefPluginsTest::CRefPluginsTest()
	{
	SetTestStepName( KRefPluginsTest );
	}

TVerdict CRefPluginsTest::doTestStepL()
	{
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
			_L("RFeatureControl::Open failed: error = %d"),err);
	
	// simple feature from the normal plugin
	TFeatureEntry entry( KDefaultSupportedUid );
	TInt supported = control.FeatureSupported( entry );
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,
			_L("RFeatureControl::FeatureSupported - KFeatureSupported expected; returned value is %d"),supported);
	
	// enhanced feature from the enhanced plugin
	TFeatureEntry entry1( KFeatureBUid );
	supported = control.FeatureSupported( entry1 );
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,
			_L("RFeatureControl::FeatureSupported - KFeatureSupported expected; returned value is %d"),supported);

	// simple feature from the mixed plugin
	TFeatureEntry entry3( KModifiableUid );
	supported = control.FeatureSupported( entry3 );
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,
			_L("RFeatureControl::FeatureSupported - KFeatureSupported expected; returned value is %d"),supported);
	
	// enhanced feature from the mixed plugin
	TFeatureEntry entry4( KNewUid );
	supported = control.FeatureSupported( entry4 );
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,
			_L("RFeatureControl::FeatureSupported - KFeatureSupported expected; returned value is %d"),supported);
	
	control.Close();
	return TestStepResult();
	}

CPluginCapabilitiesTest::CPluginCapabilitiesTest()
   {
   SetTestStepName( KPluginCapabilitiesTest );
   }

TVerdict CPluginCapabilitiesTest::doTestStepL()
	{
    User::LeaveIfError(RProperty::Define(KPropertyCat2, KPropertyKey, RProperty::EInt, KRead, KPass));
	// Used to test the capability of the plug-in to read and write value of a P&S property
	User::LeaveIfError(RProperty::Define(KAccessTestPropertyCat, KPropertyKey, RProperty::EInt, KRead, KWrite));
	RProperty propertyHndl;
	User::LeaveIfError(propertyHndl.Attach(KPropertyCat2, KPropertyKey, EOwnerThread));		
	
	TInt propVal = KErrNotFound;
	TInt err = propertyHndl.Set(ENonReadDeviceDataCap);
	
	if( KErrNone == err )
    	{
    	RFeatureControl control;
    	err = control.Open();
    	TESTDIAGNOSTICERROR(err==KErrNone,
    			_L("RFeatureControl::Open failed: error = %d"),err);

    	// To guarantee that the asynchronous loading of the plugin by the server thread 
    	// is completed before getting the P&S property value.
    	User::After(500000);
    	
        err = propertyHndl.Get(propVal);
        if( KErrNone == err )
        	{	
        	TESTDIAGNOSTICERROR(propVal == KErrPermissionDenied,		
        			_L("Plugin with calls to non-ReadDeviceData capability APIs expects KErrPermissionDenied; returned value is %d"), propVal);
        	}
        else
        	{
        	TESTDIAGNOSTICERROR(EFalse,
        			_L("Property value could not be read; returned value is %d"), err);
          	}
       	control.Close();
       	// To guarantee closing the session to the server and deleting the plugin before openning another one
       	User::After(1000000);
    	}
    else
    	{
    	TESTDIAGNOSTICERROR(EFalse,
    			_L("Property could not be set; returned value is %d"), err);
      	}
    
	err = propertyHndl.Set(EReadDeviceDataCap);	
	if( KErrNone == err )
    	{
    	RFeatureControl control;
    	err = control.Open();
    	TESTDIAGNOSTICERROR(err==KErrNone,
    			_L("RFeatureControl::Open failed: error = %d"),err);

    	// To guarantee that the asynchronous loading of the plugin by the server thread 
    	// is completed before getting the P&S property value.
    	User::After(500000);

        err = propertyHndl.Get(propVal);
        if( KErrNone == err )
        	{
        	TESTDIAGNOSTICERROR(propVal == KErrNone,
          	      _L("Plugin with calls to ReadDeviceData capability APIs expects KErrNone; returned value is %d"), propVal);
        	}
        else
        	{
        	TESTDIAGNOSTICERROR(EFalse,
        			_L("Property value could not be read; returned value is %d"), err);
          	}
       	control.Close();
    	}
    else
    	{
    	TESTDIAGNOSTICERROR(EFalse,
        	      _L("Property could not be set; returned value is %d"), err);
      	}
	
	RProperty::Delete(KAccessTestPropertyCat, KPropertyKey);	
	propertyHndl.Close();
	propertyHndl.Delete(KPropertyCat2, KPropertyKey);	
		
	return TestStepResult();		
	}

CInvalidFlagsPluginTest::CInvalidFlagsPluginTest()
   {
   SetTestStepName( KInvalidFlagsPluginTest );
   }

TInt CInvalidFlagsPluginTest::GetInvalidFlag(TInt i)
	{
	switch(i)
	{
	case 2:
		return 0x00000010;
	case 3:
		return 0x00000030;
	case 4:
		return 0x0000000C;
	case 5:
		return 0x00000018;
	case 6:
		return 0x00000028;
	case 7:
		return 0x0000002C;
	case 8:
		return 0x0000001C;
	case 9:
		return 0x00000038;
	case 10:
		return 0x0000003C;
	default:
		return -1;
	};
	}

TVerdict CInvalidFlagsPluginTest::doTestStepL()
	{
	TInt err = RProperty::Define(KPropertyCat1, KPropertyKey, RProperty::EInt, KRead, KPass);	
	RProperty propertyHndl;
	User::LeaveIfError(propertyHndl.Attach(KPropertyCat1, KPropertyKey, EOwnerThread));		
	TInt updateVal = -1;	// Just to confirm that the value has been updated properly
	
	if( err != KErrAlreadyExists)
		{
		// First invalid flag value
		User::LeaveIfError(propertyHndl.Set(0x00000020));
		User::LeaveIfError(propertyHndl.Get(updateVal));
		}
	// Confirm P&S property value has been set
	if( updateVal==0x00000020 )
		{
		TInt i = 1;		
		
		do
			{
			RFeatureControl control;
			TInt err = control.Open();
			TESTDIAGNOSTICERROR(err==KErrNone,
				    _L("RFeatureControl::Open failed: error = %d"),err);
		
			TFeatureEntry entry1( KDefaultSupportedUid );
			TInt supported = control.FeatureSupported( entry1 );
			TESTDIAGNOSTICERROR(supported == KErrServerTerminated,
					_L("RFeatureControl::FeatureSupported - We expect the server to panic when one of the plugins is a corrupt plugin with an invalid flag value; returned value is %d"),supported);

			i++;
			TInt temp = GetInvalidFlag(i);			
			User::LeaveIfError(propertyHndl.Set(temp));
			User::LeaveIfError(propertyHndl.Get(updateVal));
			
			control.Close();
			User::After(1000000);
			}
		while( i<=10 );
		
		propertyHndl.Close();
		propertyHndl.Delete(KPropertyCat1, KPropertyKey);	
		}
	else
		{
		TESTDIAGNOSTICERROR(EFalse,
				_L("P&S prperty has not been set properly; val is: %d"), updateVal);
		}
	
	return TestStepResult();
	}

CCombinationNCPluginTestStep::CCombinationNCPluginTestStep()
   {
   SetTestStepName( KCombinationNCPluginTest );
   }

TVerdict CCombinationNCPluginTestStep::doTestStepL()
	{
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
		  _L("RFeatureControl::Open failed: error = %d"),err);

	TFeatureEntry entry1( KDefaultSupportedUid );
	TInt supported = control.FeatureSupported( entry1 );
	TESTDIAGNOSTICERROR(supported == KErrServerTerminated,
			_L("RFeatureControl::FeatureSupported - We expect the server to panic when one of the plugins is a corrupt plugin inspite of the normal plugin; returned value is %d"),supported);
		
	control.Close();
	return TestStepResult();
    }

CCombinationNHPluginTestStep::CCombinationNHPluginTestStep()
   {
   SetTestStepName( KCombinationNHPluginTest );
   }

TVerdict CCombinationNHPluginTestStep::doTestStepL()
	{
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
		  _L("RFeatureControl::Open failed: error = %d"),err);

	TFeatureEntry entry1( KDefaultSupportedUid );
	TInt supported = control.FeatureSupported( entry1 );
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,
	      _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for a supported feature; returned value is %d"),supported);
	
	TFeatureEntry entry2( KHangingPluginUid );
	supported = control.FeatureSupported( entry2 );
	TESTDIAGNOSTICERROR(supported == KErrNotFound,
	      _L("RFeatureControl::FeatureSupported - KErrNotFound expected for the feature defined in the hanging plugin as the plugin must be killed before defining the feature; returned value is %d"),supported);
	
	control.Close();
	return TestStepResult();
    }

CSimpleEnhancedPluginTestStep::CSimpleEnhancedPluginTestStep()
   {
   SetTestStepName( KSimpleEnhancedPluginTest );
   }

TVerdict CSimpleEnhancedPluginTestStep::doTestStepL()
	{
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
		  _L("RFeatureControl::Open failed: error = %d"),err);

	TFeatureEntry entry1( KFeatureAUid );
	TInt supported = control.FeatureSupported( entry1 );
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,
	      _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for a supported feature; returned value is %d"),supported);

	TESTDIAGNOSTICERROR(entry1.FeatureData() == 0,
	      _L("Feature A data value is not consistent and it is %d"), entry1.FeatureData());

	TFeatureEntry entry2( KFeatureBUid );
	supported = control.FeatureSupported( entry2 );
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,
	      _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for a supported feature; returned value is %d"),supported);

	const TInt KBData( 0x000000bb );
	TESTDIAGNOSTICERROR( (entry2.FeatureData() == 0 || entry2.FeatureData() == KBData),
		  _L("Feature B data value is not consistent and it is either 0 or 187 %d"), entry2.FeatureData());

	TFeatureEntry entry3( KFeatureCUid );
	const TInt KCData( 0x000000cc );
	supported = control.FeatureSupported( entry3 );
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,
	      _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for a supported feature; returned value is %d"),supported);

	TESTDIAGNOSTICERROR(entry3.FeatureData() == KCData,
	      _L("Feature C data is not consistent and its value is %d rather than 204"), entry3.FeatureData());

	control.Close();
	return TestStepResult();
    }

CNormalPluginTestStep::CNormalPluginTestStep()
   {
   SetTestStepName(KNormalPluginTest );
   }

TVerdict CNormalPluginTestStep::doTestStepL()
	{
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
	_L("RFeatureControl::Open failed: error = %d"),err);


	TFeatureEntry entry1( KDefaultSupportedUid );

	TInt supported = control.FeatureSupported( entry1 );
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,
	      _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for a supported feature; returned value is %d"),supported);

	TFeatureEntry entry2( KDefaultUnsupportedUid );
	supported = control.FeatureSupported( entry2 );
	TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
	      _L("RFeatureControl::FeatureSupported - KFeatureUnsupported expected for an unsupported feature; returned value is %d"),supported);

	TFeatureEntry entry3( KUnknownUid );
	supported = control.FeatureSupported( entry3 );
	TESTDIAGNOSTICERROR(supported == KErrNotFound,
	      _L("RFeatureControl::FeatureSupported - KErrNotFound expected for a feature not from DSRs; returned value is %d"),supported);

	TFeatureEntry entry4( KDSR1Uid );
	supported = control.FeatureSupported( entry4 );
	TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
	      _L("RFeatureControl::FeatureSupported - KFeatureUnsupported expected for a feature that overrides a feature from DSRs; returned value is %d"),supported);

	control.Close();
	return TestStepResult();
    }

CCorruptPluginTestStep::CCorruptPluginTestStep()
   {
   SetTestStepName(KCorruptPluginTest );
   }

TVerdict CCorruptPluginTestStep::doTestStepL()
	{
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
	_L("RFeatureControl::Open failed: error = %d"),err);
	
	
	TFeatureEntry entry1( KDefaultSupportedUid );
	
	TInt supported = control.FeatureSupported( entry1 );
	TESTDIAGNOSTICERROR(supported == KErrServerTerminated,
	      _L("RFeatureControl::FeatureSupported - We expect the server to panic when one of the plugins is a corrupt plugin; returned value is %d"),supported);
	
	control.Close();
	return TestStepResult();
	}

CHangingPluginTestStep::CHangingPluginTestStep()
	{
	SetTestStepName(KHangingPluginTest );
	}

TVerdict CHangingPluginTestStep::doTestStepL()
	{
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
	_L("RFeatureControl::Open failed: error = %d"), err);
	TFeatureEntry entry1( KHangingPluginUid );
	TInt supported = control.FeatureSupported( entry1 );
	TESTDIAGNOSTICERROR(supported == KErrNotFound,
	      _L("RFeatureControl::FeatureSupported - KErrNotFound expected for the feature defined in the hanging plugin as the plugin must be killed before defining the feature; returned value is %d"),supported);
	
	control.Close();
	return TestStepResult();
	}


CSlowStartPluginTestStep::CSlowStartPluginTestStep()
	{
	SetTestStepName(KSlowStartPluginTest);
	}

TVerdict CSlowStartPluginTestStep::doTestStepL()
	{
	RFeatureControl control;

	TInt err = control.Open();

	TESTDIAGNOSTICERROR(err==KErrNone,
		_L("RFeatureControl::Open failed: error = %d"), err);

	TFeatureEntry entry1( KSlowStartPluginUid );

	TInt supported = control.FeatureSupported( entry1 );

	TESTDIAGNOSTICERROR(supported,
		_L("RFeatureControl::FeatureSupported - true expected for the feature defined in the slow start plugin; returned value is %d"), supported);

	control.Close();
	return TestStepResult();
	}

CSimpleMergePluginTestStep::CSimpleMergePluginTestStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName( KSimpleMergePluginTestStep );
	}


TVerdict CSimpleMergePluginTestStep::doTestStepL()
	{
	const TUint32 KTestDefaultData( 0x00000000 );
	RFeatureControl control;
	TInt err = control.Open();
	if (err == KErrNone) 
		{
		TFeatureEntry testEntry( KDefaultSupportedUid);
		err = control.FeatureSupported(testEntry);
		if (err != KErrNotFound)
			{
			if (KTestDefaultData == testEntry.FeatureData())
				{
				ERR_PRINTF1( _L(" The features entry is equal to the default data this is an error "));
				SetTestStepResult(EFail);
				}

			control.Close();
			}
		else //Feature not found
			{
			ERR_PRINTF1( _L(" The feature has not be found."));
			SetTestStepResult(EFail);
			}
		}
		else //Unable to open
		{
		ERR_PRINTF1( _L(" Can not open a connection to RFeatureControl."));
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	
	}



