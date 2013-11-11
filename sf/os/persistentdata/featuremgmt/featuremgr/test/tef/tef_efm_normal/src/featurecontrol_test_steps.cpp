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
// Feature Control component test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featmgr/featurecontrol.h>
#include "featurecontrol_test_steps.h"

CFeatureControlConnetctionStep::CFeatureControlConnetctionStep()
   {
   SetTestStepName(KFeatureControlConnetction);
   }

TVerdict CFeatureControlConnetctionStep::doTestStepL()
   {
   RFeatureControl control;
   TInt err = control.Connect();       
   TESTDIAGNOSTICERROR(err==KErrNone,
         _L("RFeatureControl::Connect failed: error = %d"),err);
   control.Close();
   
   err = control.Open();       
   TESTDIAGNOSTICERROR(err==KErrNone,
         _L("RFeatureControl::Open failed: error = %d"),err);
   control.Close();
   control.Close();
   err = control.Open();       
   TESTDIAGNOSTICERROR(err==KErrNone,
         _L("RFeatureControl::Open failed after RFeatureControl::Close called twice: error = %d"),err);
   control.Close();
   return TestStepResult();
   }

CFeatureControlSingleFeatureRequestStep::CFeatureControlSingleFeatureRequestStep()
   {
   SetTestStepName(KFeatureControlSingleFeatureRequest);
   } 

TVerdict CFeatureControlSingleFeatureRequestStep::doTestStepL()
	{
	RFeatureControl control;
	TInt err = control.Open();
	TESTDIAGNOSTICERROR(err==KErrNone,
	  _L("RFeatureControl::Open failed: error = %d"),err);

	TInt supported = control.FeatureSupported( KDefaultSupportedUid );    
	TESTDIAGNOSTICERROR(supported == KFeatureSupported,
	  _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for supported feature; returned value is %d"),supported);
	
	supported = control.FeatureSupported( KDefaultUnsupportedUid );
	TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
	  _L("RFeatureControl::FeatureSupported - KFeatureUnsupported expected for unsupported feature; returned value is %d"),supported);

	supported = control.FeatureSupported( KUninitializedUid );
	TESTDIAGNOSTICERROR(supported == KErrNotReady,
	  _L("RFeatureControl::FeatureSupported - KErrNotReady expected for uninitialized feature; returned value is %d"),supported);
	control.Close();

	return TestStepResult();
	}
      
CFeatureControlDataAndStatusRequestStep::CFeatureControlDataAndStatusRequestStep()
   {
   SetTestStepName(KFeatureControlDataAndStatusRequest);
   }         

CFeatureControlMultipleFeatureRequestStep::CFeatureControlMultipleFeatureRequestStep()
   {
   SetTestStepName(KFeatureControlMultipleFeatureRequest);   
   }

TVerdict CFeatureControlMultipleFeatureRequestStep::doTestStepL(  )
   {
    RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
         _L("RFeatureControl::Open failed: error = %d"),err);
    CleanupClosePushL( control );
    RFeatureArray uids;
    TInt supported;
    CleanupClosePushL( uids );
    
    // Make query with empty array
    err = control.FeaturesSupported( uids );
    TESTDIAGNOSTICERROR(err == KErrArgument,
          _L("RFeatureControl::FeaturesSupported - KErrArgument expected; error = %d"),err);    
    
    TInt index = uids.Find( KDefaultSupportedUid );
    TESTDIAGNOSTICERROR(index == KErrNotFound,
          _L("KErrNotFound expected; returned value is %d"),index);        
    
    // Add one supported feature and query statuses.
    uids.AppendL( KDefaultSupportedUid );
    err = control.FeaturesSupported( uids );
    TESTDIAGNOSTICERROR(err == KErrNone,
          _L("RFeatureControl::FeaturesSupported - KErrNone expected; error = %d"),err);    
    index = uids.Find( KDefaultSupportedUid );
    TESTDIAGNOSTICERROR(index != KErrNotFound,
          _L("Feature is expected to be found; return value is %d"),index);        
    index = uids.Find( KDefaultUnsupportedUid );
    TESTDIAGNOSTICERROR(index == KErrNotFound,
          _L("KErrNotFound expected for not added feature; return value is %d"),index);        
    index = uids.Find( KUnknownUid );
    TESTDIAGNOSTICERROR(index == KErrNotFound,
          _L("KErrNotFound expected for non existing feature; return value is %d"),index);        
    
    // Append feature which is supposed to be non-existent and re-query statuses.
    uids.AppendL( KUnknownUid );
    err = control.FeaturesSupported( uids );
    TESTDIAGNOSTICERROR(err == KErrNone,
          _L("RFeatureControl::FeaturesSupported - KErrNone expected; error = %d"),err);    
    index = uids.Find( KUnknownUid );
    TESTDIAGNOSTICERROR(index == KErrNotFound,
          _L("KErrNotFound still expected for non existing feature; return value is %d"),index);        
    
    // Add one more entry and query support statuses again
    uids.AppendL( KDefaultUnsupportedUid );
    err = control.FeaturesSupported( uids );
    TESTDIAGNOSTICERROR(err == KErrNone,
          _L("RFeatureControl::FeaturesSupported - KErrNone expected; error = %d"),err);    
    index = uids.Find( KDefaultUnsupportedUid );
    TESTDIAGNOSTICERROR(index != KErrNotFound,
          _L("Feature is expected to be found; return value is %d"),index); 
    
    // Finally check support statuses for supported and 
    // not-supported features are as expected.
    index = uids.Find( KDefaultSupportedUid );
    TESTDIAGNOSTICERROR(index != KErrNotFound,
          _L("Feature is expected to be found; return value is %d"),index);        
    if(index != KErrNotFound)
         {    
       supported = static_cast<TInt>(uids[index].FeatureFlags().IsSet( EFeatureSupported ));
       TESTDIAGNOSTICERROR(supported == KFeatureSupported,
             _L("KFeatureSupported expected; return value is %d"),supported);            
         }
    index = uids.Find( KDefaultUnsupportedUid );
    TESTDIAGNOSTICERROR(index != KErrNotFound,
          _L("Feature is expected to be found; return value is %d"),index);            
    if(index != KErrNotFound)
        {
        supported = static_cast<TInt>(uids[index].FeatureFlags().IsSet( EFeatureSupported ));
        TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
              _L("KFeatureUnsupported expected; return value is %d"),index);                    
        }
    
    CleanupStack::PopAndDestroy( &uids );
    CleanupStack::PopAndDestroy( &control );
    return TestStepResult();
    }

TVerdict CFeatureControlDataAndStatusRequestStep::doTestStepL()      
    {
    RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::Open failed: error = %d"),err);
    
    // Fetch status for "unknown" feature
    TFeatureEntry emptyEntry(TUid::Uid(0x1000005));
    TInt supported = control.FeatureSupported( emptyEntry );
    TESTDIAGNOSTICERROR(supported == KErrNotFound,
          _L("RFeatureControl::FeatureSupported - KErrNotFound expected for unknown feature; returned value is %d"),supported);
    
    // Fetch initially set data for supported and modifiable feature
    TFeatureEntry entry( KDefaultSupportedUid );
    supported = control.FeatureSupported( entry );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for supported feature; returned value is %d"),supported);
    TESTDIAGNOSTICERROR(entry.FeatureData() == KDefaultData1,
          _L("RFeatureControl::FeatureData - KDefaultData1 expected; returned value is %x"),entry.FeatureData());    
    
    // Modify and refetch data for supported and modifiable feature
    err = control.SetFeature( KDefaultSupportedUid, KChangeData );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrNone,
              _L("RFeatureControl::SetFeature - KErrNone expected; error = %d"),err);
       }

    supported = control.FeatureSupported( entry );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for supported feature; returned value is %d"),supported);
    if (iLowCap)
       {
        TESTDIAGNOSTICERROR(entry.FeatureData() == KDefaultData1,
              _L("RFeatureControl::FeatureData - KDefaultData1 expected; returned value is %x"),entry.FeatureData());    
       }
    else
       {
        TESTDIAGNOSTICERROR(entry.FeatureData() == KChangeData,
              _L("RFeatureControl::FeatureData - KChangeData expected; returned value is %x"),entry.FeatureData());    
       }
    
    // Try to modify unmodifiable feature
    TFeatureEntry entry2( KUnmodifiableUid );    
    err = control.SetFeature( KUnmodifiableUid, KChangeData );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrAccessDenied,
              _L("RFeatureControl::SetFeature - KErrAccessDenied expected; error = %d"),err);    
       }

    supported = control.FeatureSupported( entry2 );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for unmodifiable feature; returned value is %d"),supported);
    TESTDIAGNOSTICERROR(entry2.FeatureData() == KDefaultData1, 
          _L("RFeatureControl::FeatureData - KDefaultData1 expected; returned value is %x"),entry2.FeatureData());    
    
    control.Close();
    return TestStepResult();
    }     

CFeatureControlFeatureEnablingStep::CFeatureControlFeatureEnablingStep()
   {
   SetTestStepName(KFeatureControlFeatureEnabling);
   }       
      
TVerdict CFeatureControlFeatureEnablingStep::doTestStepL(  )
    {
    RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::Open failed: error = %d"),err);
    
    err = control.EnableFeature( KModifiableUid );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::EnableFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrNone,
              _L("RFeatureControl::EnableFeature - KErrNone expected; error = %d"),err);                    
       }

    err = control.EnableFeature( KUnmodifiableUid );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::EnableFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrAccessDenied,
              _L("RFeatureControl::EnableFeature - KErrAccessDenied expected; error = %d"),err);                        
       }

    err = control.EnableFeature( KUnknownUid );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::EnableFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrNotFound,
              _L("RFeatureControl::EnableFeature - KErrNotFound expected; error = %d"),err);                    
       }
       
	err = control.EnableFeature( KPersistedUid );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::EnableFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrAccessDenied,
              _L("RFeatureControl::EnableFeature - KErrAccessDenied expected; error = %d"),err);                    
       }

    
    control.Close();
    return TestStepResult();
    }      

CFeatureControlFeatureDisablingStep::CFeatureControlFeatureDisablingStep()
   {
   SetTestStepName(KFeatureControlFeatureDisabling);
   }       
      
TVerdict CFeatureControlFeatureDisablingStep::doTestStepL(  )
    {
    RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::Open failed: error = %d"),err);
    
    err = control.DisableFeature( KModifiableUid );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::DisableFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrNone,
              _L("RFeatureControl::DisableFeature - KErrNone expected; error = %d"),err);                    
       }

    err = control.DisableFeature( KUnmodifiableUid );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::DisableFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrAccessDenied,
              _L("RFeatureControl::DisableFeature - KErrAccessDenied expected; error = %d"),err);                        
       }

    err = control.DisableFeature( KUnknownUid );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::DisableFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrNotFound,
              _L("RFeatureControl::DisableFeature - KErrNotFound expected; error = %d"),err);                    
       }
    
    control.Close();
    return TestStepResult();
    }
    
CFeatureControlFeatureDataSettingStep::CFeatureControlFeatureDataSettingStep()
   {
   SetTestStepName(KFeatureControlFeatureDataSetting);
   }       

TVerdict CFeatureControlFeatureDataSettingStep::doTestStepL(  )
    {
    RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::Open failed: error = %d"),err);

    err = control.SetFeature( KDefaultSupportedUid, KDefaultData1 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
       TESTDIAGNOSTICERROR(err == KErrNone,       
              _L("RFeatureControl::SetFeature - KErrNone expected; error = %d"),err);       
       }
    err = control.SetFeature( KDefaultUnsupportedUid, KDefaultData1 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
       TESTDIAGNOSTICERROR(err == KErrNone,       
              _L("RFeatureControl::SetFeature - KErrNone expected; error = %d"),err);
       }

    err = control.SetFeature( KUnmodifiableUid, KDefaultData1 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrAccessDenied, 
              _L("RFeatureControl::SetFeature - KErrAccessDenied expected; error = %d"),err);
       }
    
    err = control.SetFeature( KUnknownUid, KDefaultData1 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrNotFound, 
              _L("RFeatureControl::SetFeature - KErrNotFound expected; error = %d"),err);
           
       }
        
    control.Close();
    return TestStepResult();
    }

CFeatureControlFeatureAddingStep::CFeatureControlFeatureAddingStep()
   {   
   SetTestStepName(KFeatureControlFeatureAdding);
   }
   
TVerdict CFeatureControlFeatureAddingStep::doTestStepL(  )     
    {
    RFeatureControl control;
    TInt err( control.Open() );
    TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::Open failed: error = %d"),err);
    
    // Try to add already existing feature.
    TBitFlags32 flags( 0 );
    flags.Set( EFeatureSupported );
    flags.Set( EFeatureModifiable );
    TFeatureEntry entry( KDefaultSupportedUid, flags, KDefaultData1 );
    err = control.AddFeature( entry );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::AddFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrAlreadyExists,
              _L("RFeatureControl::AddFeature - KErrAlreadyExists expected; error = %d"),err);
       }
    
    // Add new feature. If test run sequentially, feature already exists 
    TFeatureEntry entry2( KNewUid, flags, KDefaultData1 );
    err = control.AddFeature( entry2 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::AddFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrNone || err == KErrAlreadyExists, 
              _L("RFeatureControl::AddFeature - KErrNone or KErrAlreadyExists expected; error = %d"),err);
       }
    TBitFlags32 flags1( 0 );
    flags1.Set( EFeatureUninitialized );
    flags1.Set( EFeatureModifiable );
    TFeatureEntry entry3( KNewUid1, flags1, KDefaultData1 );
    err = control.AddFeature( entry3 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::AddFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrNone || err == KErrAlreadyExists, 
              _L("RFeatureControl::AddFeature - KErrNone or KErrAlreadyExists expected; error = %d"),err);
       }
       
    /*TBitFlags32 flags2( 0 );
    flags2.Set( EFeatureUninitialized );
    TFeatureEntry entry4( KNewUid3, flags2, KDefaultData1 );
    err = control.AddFeature( entry4 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RFeatureControl::AddFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err == KErrArgument, 
              _L("RFeatureControl::AddFeature - KErrNone or KErrAlreadyExists expected; error = %d"),err);
       }*/

    
    control.Close();
    return TestStepResult();
    }
        
CFeatureControlFeatureDataAndStatusSettingStep::CFeatureControlFeatureDataAndStatusSettingStep()
   {
   SetTestStepName(KFeatureControlFeatureDataAndStatusSetting);
   }       

TVerdict CFeatureControlFeatureDataAndStatusSettingStep::doTestStepL(  )
    {
    RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::Open failed: error = %d"),err);

    err = control.SetFeature( KModifiableUid, EFalse, KDefaultData1 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RRFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
        TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::SetFeature - KErrNone expected: error = %d"),err);
       }    
    
    err = control.SetFeature( KModifiableUid, ETrue, KDefaultData1  );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RRFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
      TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::SetFeature - KErrNone expected: error = %d"),err);
       }    
        
    TFeatureEntry entry( KModifiableUid );
    TInt supported = control.FeatureSupported( entry );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected: returned value is  %d"),err);
    TESTDIAGNOSTICERROR(entry.FeatureData() == KDefaultData1,
          _L("RFeatureControl::FeatureData - KDefaultData1 expected: returned value is %x"),entry.FeatureData());
    
    err = control.SetFeature( KModifiableUid, ETrue, KChangeData );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RRFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
      TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::FeatureSupported - KErrNone expected: error = %d"),err);
          }    
       
    TFeatureEntry entry2( KModifiableUid );
    supported = control.FeatureSupported( entry2 );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected: error = %d"),err);
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(entry2.FeatureData() == KDefaultData1,
          _L("RFeatureControl::FeatureData - KDefaultData1 expected: returned value is %x"),entry.FeatureData());    
       }
    else
       {
       TESTDIAGNOSTICERROR(entry2.FeatureData() == KChangeData,
          _L("RFeatureControl::FeatureData - KChangeData expected: returned value is %x"),entry.FeatureData());    
          }    

        
    err = control.SetFeature( KUnmodifiableUid, ETrue, KDefaultData1 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RRFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
      TESTDIAGNOSTICERROR(err==KErrAccessDenied,
          _L("RFeatureControl::SetFeature - KErrAccessDenied expected: error = %d"),err);
          }    
    
    err = control.SetFeature( KUnmodifiableUid, EFalse, KDefaultData1 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RRFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
      TESTDIAGNOSTICERROR(err==KErrAccessDenied,
          _L("RFeatureControl::SetFeature - KErrAccessDenied expected: error = %d"),err);
          }    
    
    err = control.SetFeature( KUnknownUid, ETrue, KDefaultData1 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RRFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
       TESTDIAGNOSTICERROR(err==KErrNotFound,
          _L("RFeatureControl::SetFeature - KErrNotFound expected: error = %d"),err);
          }
    err = control.SetFeature( KUnknownUid, EFalse, KDefaultData1 );
    if (iLowCap)
       {
       TESTDIAGNOSTICERROR(err == KErrPermissionDenied,       
              _L("RRFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
       }
    else
       {
      TESTDIAGNOSTICERROR(err==KErrNotFound,
          _L("RFeatureControl::SetFeature - KErrNotFound expected: error = %d"),err);          
       }
   
       
    control.Close();
    return TestStepResult();
    } 
            
CFeatureControlListSupportedFeaturesStep::CFeatureControlListSupportedFeaturesStep()
   {
   SetTestStepName(KFeatureControlListSupportedFeatures);
   }       
      
TVerdict CFeatureControlListSupportedFeaturesStep::doTestStepL( )
    {
    RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::Open failed: error = %d"),err);
        
    RFeatureUidArray supportedFeatures;
    err = control.ListSupportedFeatures( supportedFeatures );
    TESTDIAGNOSTICERROR(err==KErrNone,
          _L("RFeatureControl::ListSupportedFeatures failed: error = %d"),err);
    
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
    control.Close();
    return TestStepResult();
    }      

CFeatureControlInvalidUsePanicTestStep::CFeatureControlInvalidUsePanicTestStep()
	{
	SetTestStepName(KFeatureControlInvalidUsePanicTest);
	}

TVerdict CFeatureControlInvalidUsePanicTestStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	RFeatureControl control;
	//we expect a panic as a result of the following instruction as the RFeatureControl object has not been opened
	TInt error = control.FeatureSupported(KDefaultSupportedUid);
	ERR_PRINTF2( _L("RFeatureControl::FeatureSupported is expected to cause a panic, return value is %d"), error );
	return TestStepResult();
	}


