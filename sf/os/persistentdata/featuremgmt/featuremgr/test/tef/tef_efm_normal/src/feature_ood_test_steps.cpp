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
// dsr_test_steps.cpp
// EFM test steps for DSR feature handling
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featdiscovery.h>
#include <featmgr/featurecontrol.h>
#include "feature_ood_test_steps.h"
#include "efm_teststepbase.h"
#include "efm_test_consts.h"

COODTestStep::COODTestStep()
   {
   SetTestStepName(KOODTest);
   }

TVerdict COODTestStep::doTestStepL()
   {
#ifdef _DEBUG	
   RFeatureControl control;
   TInt err = control.Open();
   TESTDIAGNOSTICERROR(err==KErrNone,
		   _L("RFeatureControl::Open failed: error = %d"),err);
   RFs fs;
   fs.Connect();
   CleanupClosePushL(fs);
   fs.SetErrorCondition(KErrDiskFull);  
   TInt supported = control.FeatureSupported( KDefaultSupportedUid );    
   fs.SetErrorCondition(0);
   TESTDIAGNOSTICERROR(supported == KFeatureSupported,
	  _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for supported feature; returned value is %d"),supported);

   fs.SetErrorCondition(KErrDiskFull);  
   err = control.DisableFeature(KDefaultSupportedUid);
   fs.SetErrorCondition(0);
   TESTDIAGNOSTICERROR(err==KErrDiskFull,
		   _L("RFeatureControl::DisableFeature is expected to fail with KErrDiskFull: returned value is = %d"),err);
   
   fs.SetErrorCondition(KErrDiskFull);
   supported = control.FeatureSupported( KDefaultUnsupportedUid );
   fs.SetErrorCondition(0);
   TESTDIAGNOSTICERROR(supported == KFeatureUnsupported,
	  _L("RFeatureControl::FeatureSupported - KFeatureUnsupported expected for unsupported feature; returned value is %d"),supported);
   
   fs.SetErrorCondition(KErrDiskFull);
   err = control.EnableFeature(KDefaultUnsupportedUid);
   fs.SetErrorCondition(0);
   TESTDIAGNOSTICERROR(err==KErrDiskFull,
		   _L("RFeatureControl::EnableFeature is expected to fail with KErrDiskFull: returned value is = %d"),err);
   fs.SetErrorCondition(KErrDiskFull);  
   err = control.SetFeature(KModifiableUid, ETrue, KChangeData);
   fs.SetErrorCondition(0);
   TESTDIAGNOSTICERROR(err==KErrDiskFull,
		   _L("RFeatureControl::SetFeature is expected to fail with KErrDiskFull: returned value is = %d"),err);
   //control.DeleteFeature(KDefualtModifiableUid);
   TBitFlags32 flags( 0 );
   flags.Set( EFeatureSupported );
   flags.Set( EFeatureModifiable );
   flags.Set( EFeaturePersisted );
   TFeatureEntry entryToAdd( KOODUid, flags, KDefaultData1 );
   fs.SetErrorCondition(KErrDiskFull);  
   err = control.AddFeature(entryToAdd);
   fs.SetErrorCondition(0);
   TESTDIAGNOSTICERROR(err==KErrDiskFull,
		   _L("RFeatureControl::AddFeature is expected to fail with KErrDiskFull: returned value is = %d"),err);
   CleanupStack::PopAndDestroy(&fs);
   control.Close();
#else
   INFO_PRINTF1(_L("OOD testing can only be performed on DEBUG builds"));  	 
#endif
   return TestStepResult();
   }

