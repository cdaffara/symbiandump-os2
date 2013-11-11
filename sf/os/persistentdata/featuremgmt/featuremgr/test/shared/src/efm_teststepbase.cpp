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
// Base class for capability aware test steps and a auxilarry test step to publish 
// features needed by other test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include "efm_teststepbase.h"
#include <featmgr/featurecontrol.h>
#include <featdiscovery.h>

CEFMTestStepPublishFeatures::CEFMTestStepPublishFeatures()
   {
   SetTestStepName(KEFMTestStepPublishFeatures);
   } 

TVerdict CEFMTestStepPublishFeatures::doTestStepL()
{
	RFeatureControl control;
    TInt err = control.Open();
    TESTDIAGNOSTICERROR(err==KErrNone,
    		_L("Failed to open RFeatureControl connection during initial feature publishing: error = %d"),err);
    CleanupClosePushL( control );
    
    // Make sure feature which is not supported exists
    TInt supported = control.FeatureSupported( KDefaultUnsupportedUid );
    if( supported != KFeatureUnsupported && supported != KFeatureSupported )
        {
        TBitFlags32 flags( 0 );
        flags.Set( EFeatureModifiable );
        flags.Set( EFeaturePersisted );
        TFeatureEntry entry( KDefaultUnsupportedUid, flags, KDefaultData1 );
        err = control.AddFeature( entry );
        TESTDIAGNOSTICERROR(err==KErrNone,
        		_L("Failed to add feature during initial feature publishing: error = %d"),err);
        }
    else
        {
        err = control.SetFeature( KDefaultUnsupportedUid, EFalse, KDefaultData1 );
        TESTDIAGNOSTICERROR(err==KErrNone,
        		_L("Failed to set feature data during initial feature publishing: error = %d"),err);
        }
    
    // Make sure feature which is supported exists
    supported = control.FeatureSupported( KDefaultSupportedUid );
    if( supported != KFeatureUnsupported && supported != KFeatureSupported )
        {
        TBitFlags32 flags( 0 );
        flags.Set( EFeatureSupported );
        flags.Set( EFeatureModifiable );
        flags.Set( EFeaturePersisted );
        TFeatureEntry entry( KDefaultSupportedUid, flags, KDefaultData1 );
        err = control.AddFeature( entry );
        TESTDIAGNOSTICERROR(err==KErrNone,
        		_L("Failed to add feature during initial feature publishing: error = %d"),err);
        }
    else
        {
        err = control.SetFeature( KDefaultSupportedUid, ETrue, KDefaultData1 );
        TESTDIAGNOSTICERROR(err==KErrNone,
        		_L("Failed to set feature data during initial feature publishing: error = %d"),err);
        }

    // Make sure feature which is modifiable exists
    supported = control.FeatureSupported( KModifiableUid );
    if( supported != KFeatureUnsupported && supported != KFeatureSupported )
        {
        TBitFlags32 flags( 0 );
        flags.Set( EFeatureSupported );
        flags.Set( EFeatureModifiable );
        flags.Set( EFeaturePersisted );
        TFeatureEntry entry( KModifiableUid, flags, KDefaultData1 );
        err = control.AddFeature( entry );
        TESTDIAGNOSTICERROR(err==KErrNone,
        		_L("Failed to add feature during initial feature publishing: error = %d"),err);
        }
    else
        {
        err = control.SetFeature( KModifiableUid, ETrue, KDefaultData1 );
        TESTDIAGNOSTICERROR(err==KErrNone,
        		_L("Failed to set feature data during initial feature publishing: error = %d"),err);
        }

    // Make sure feature which is unmodifiable exists
    supported = control.FeatureSupported( KUnmodifiableUid );
    if( supported != KFeatureUnsupported && supported != KFeatureSupported )
        {
        TBitFlags32 flags( 0 );
        flags.Set( EFeatureSupported );
        TFeatureEntry entry( KUnmodifiableUid, flags, KDefaultData1 );
        err = control.AddFeature( entry );
        TESTE(err==KErrNone,err);
        }

    // Make sure uninitialized feature exists
    supported = control.FeatureSupported( KUninitializedUid );
    if( supported != KFeatureUnsupported && supported != KFeatureSupported
        && supported != KErrNotReady )
        {
        TBitFlags32 flags( 0 );
        flags.Set( EFeatureUninitialized );
        flags.Set( EFeatureModifiable);
        TFeatureEntry entry( KUninitializedUid, flags, KDefaultData1 );
        err = control.AddFeature( entry );
        TESTDIAGNOSTICERROR(err==KErrNone,
        		_L("Failed to add feature during initial feature publishing: error = %d"),err);
        }
        
   //Make sure that persisted feature exists
   
  supported = control.FeatureSupported(KPersistedUid);
   if(supported != KFeatureUnsupported && supported != KFeatureSupported)
   {
   	TBitFlags32 flags( 0 );
   	//flags.Set( EFeatureSupported );
   	flags.Set( EFeaturePersisted );
   	TFeatureEntry entry( KPersistedUid, flags, KDefaultData1 );
        err = control.AddFeature( entry );
        TESTDIAGNOSTICERROR(err==KErrNone,
        		_L("Failed to add feature during initial feature publishing: error = %d"),err);
   }


    CleanupStack::PopAndDestroy( &control );        
    
    return TestStepResult();	
}

TVerdict CEFMConfigurableTestStepBase::doTestStepPreambleL()
{
	//read wether this test step is a OOM test
	GetBoolFromConfig(ConfigSection(), KTestOOM, iOOM);
	
	//read wether this test step is a Low-capability test
	GetBoolFromConfig(ConfigSection(), KTestLowCap, iLowCap);
	
	return TestStepResult();
}

void CEFMConfigurableTestStepBase::CheckCondition(TBool aMainCondition, TBool aOOMCondition, TPtrC aLoggingMessage, TInt aErrorCode)
{
	TBool cond = iOOM?aMainCondition||aOOMCondition:aMainCondition;  
	if (!cond)
		{
		ERR_PRINTF2( aLoggingMessage, aErrorCode );
		SetTestStepResult(EFail);
		}		
}
