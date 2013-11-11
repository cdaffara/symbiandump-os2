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
#include "dsr_test_steps.h"
#include "efm_teststepbase.h"

CDSRTestStep ::CDSRTestStep()
   {
   SetTestStepName(KDSRTest );
   }

TVerdict CDSRTestStep::doTestStepL()
   {
    RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
    _L("RFeatureControl::Open failed: error = %d"),err);
 
     
    TFeatureEntry entry1( KDSR1Uid );

    TInt supported = control.FeatureSupported( entry1 );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for supported feature; returned value is %d"),supported);
    TESTDIAGNOSTICERROR(entry1.FeatureData() == 0,
          _L("RFeatureControl::FeatureData - 0 expected; returned value is %x"),entry1.FeatureData());
    
    TFeatureEntry entry2( KDSR2Uid);
    supported = control.FeatureSupported( entry2 );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for supported feature; returned value is %d"),supported);
    TESTDIAGNOSTICERROR(entry2.FeatureData() == 0,
          _L("RFeatureControl::FeatureData - 0 expected; returned value is %x"),entry2.FeatureData());

    TFeatureEntry entry3( KDSR3Uid);
    supported = control.FeatureSupported( entry3 );
    TESTDIAGNOSTICERROR(supported == KErrNotFound,
          _L("RFeatureControl::FeatureSupported - KerrNotFound expected for a feature not from DSRs; returned value is %d"),supported);
    
    //try to add a feature that is listed in DSR
    
    TBitFlags32 flags( 0 );
    flags.Set( EFeatureSupported );
    flags.Set( EFeatureModifiable );
    TFeatureEntry newEntry( KDSR2Uid, flags, KDefaultData1 );           
    err = control.AddFeature( newEntry );
    TESTDIAGNOSTICERROR(err == KErrNone,
              _L("RFeatureControl::AddFeature - KErrNone expected; error = %d"),err);
        
    // Enable DSR features, of which last one not supposed to locate in DSR    
    err = control.EnableFeature( KDSR2Uid );
    TESTDIAGNOSTICERROR(err == KErrNone,
              _L("RFeatureControl::EnableFeature - KErrNone expected; error = %d"),err);  
                                
        
    err = control.EnableFeature( KDSR3Uid );
    TESTDIAGNOSTICERROR(err == KErrNotFound,
              _L("RFeatureControl::EnableFeature - KErrNotFound expected; error = %d"),err);                    

    // try to modify data for a feature from DSR
    err = control.SetFeature( KDSR2Uid, KChangeData );
    TESTDIAGNOSTICERROR(err == KErrNone,
              _L("RFeatureControl::SetFeature - KErrNone expected; error = %d"),err);    
    TFeatureEntry checkdataentry( KDSR2Uid );
    supported = control.FeatureSupported( checkdataentry );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for a DSR feature; returned value is %d"),supported);
    TESTDIAGNOSTICERROR(checkdataentry.FeatureData() == KChangeData,
              _L("RFeatureControl::FeatureData - KChangeData expected; returned value is %x"),checkdataentry.FeatureData());    
    
    //delete a feature from a DSR
    err = control.DeleteFeature( KDSR2Uid );
    TESTDIAGNOSTICERROR(err == KErrNone,
              _L("RFeatureControl::DeleteFeature - KErrNone expected; error = %d"),err);

    control.Close();
    return TestStepResult();
   }

CDSRBoundsTestStep ::CDSRBoundsTestStep()
   {
   SetTestStepName(KDSRBoundsTest);
   }

TVerdict CDSRBoundsTestStep::doTestStepL()
   {
    RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
    _L("RFeatureControl::Open failed: error = %d"), err);
     
    // Check the lower bound of the DSR. It should be supported.
    TFeatureEntry entry1( KDSRLowBound1 );
    TInt supported = control.FeatureSupported( entry1 );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for supported feature; returned value is %d"), supported);
    
    // Check the upper bound of the DSR. It should be supported.
    TFeatureEntry entry2( KDSRHighBound1 );
    supported = control.FeatureSupported( entry2 );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for supported feature; returned value is %d"), supported);

    // Check just below the lower bound of the DSR. It should not be found.
    const TUid KDSRLowBound1M1 = { KDSRLowBound1.iUid - 1 };
    TFeatureEntry entry3( KDSRLowBound1M1 );
    supported = control.FeatureSupported( entry3 );
    TESTDIAGNOSTICERROR(supported == KErrNotFound,
          _L("RFeatureControl::FeatureSupported - KErrNotFound expected for unsupported feature; returned value is %d"), supported);

    // Check just above the lower bound of the DSR. It should be supported.
    const TUid KDSRLowBound1P1 = { KDSRLowBound1.iUid + 1 };
    TFeatureEntry entry4( KDSRLowBound1P1 );
    supported = control.FeatureSupported( entry4 );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for supported feature; returned value is %d"), supported);

    // Check just below the upper bound of the DSR. It should be supported.
    const TUid KDSRHighBound1M1 = { KDSRHighBound1.iUid - 1 };
    TFeatureEntry entry5( KDSRHighBound1M1 );
    supported = control.FeatureSupported( entry5 );
    TESTDIAGNOSTICERROR(supported == KFeatureSupported,
          _L("RFeatureControl::FeatureSupported - KFeatureSupported expected for supported feature; returned value is %d"), supported);

    // Check just above the upper bound of the DSR. It should not be found.
    const TUid KDSRHighBound1P1 = { KDSRHighBound1.iUid + 1 };
    TFeatureEntry entry6( KDSRHighBound1P1 );
    supported = control.FeatureSupported( entry6 );
    TESTDIAGNOSTICERROR(supported == KErrNotFound,
          _L("RFeatureControl::FeatureSupported - KErrNotFound expected for unsupported feature; returned value is %d"), supported);

    // Check if KMaxTInt is supported. It should not be found.
    const TUid KDSRKMaxTIntUID = { KMaxTInt };
    TFeatureEntry entry7( KDSRKMaxTIntUID );
    supported = control.FeatureSupported( entry7 );
    TESTDIAGNOSTICERROR(supported == KErrNotFound,
          _L("RFeatureControl::FeatureSupported - KErrNotFound expected for 'KMaxTInt' feature; returned value is %d"), supported);

    // Check if KMinTInt is supported. It should not be found.
    const TUid KDSRKMinTIntUID = { KMinTInt };
    TFeatureEntry entry8( KDSRKMinTIntUID );
    supported = control.FeatureSupported( entry8 );
    TESTDIAGNOSTICERROR(supported == KErrNotFound,
          _L("RFeatureControl::FeatureSupported - KErrNotFound expected for 'KMinTInt' feature; returned value is %d"), supported);

   control.Close();
   return TestStepResult();
   }

