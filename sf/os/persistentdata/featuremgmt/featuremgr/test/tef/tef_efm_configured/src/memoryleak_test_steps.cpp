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
// Feature Notifier component test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featmgr/featurenotifier.h>
#include <featmgr/featurecontrol.h>
#include "memoryleak_test_steps.h"

// -- new function start

CMemoryLeak_ReNotifyRequestLStep::CMemoryLeak_ReNotifyRequestLStep()
	{
	SetTestStepName(KMemoryLeak_ReNotifyRequestL);
	}

TVerdict CMemoryLeak_ReNotifyRequestLStep::doTestStepL()
	{
	CFeatureNotifier* notif = NULL;
	TRAPD(err, notif = CFeatureNotifier :: NewL(*this));
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		   _L("CFeatureNotifier::NewL - KErrNone expected; error = %d"), err );
	// If we see KErrNoMemory here then we're probably performing OOM testing.
	// The notif object is invalid and all we can do is drop out here.
	if(err == KErrNoMemory) return TestStepResult();
	
	CleanupStack::PushL( notif );	
	err = notif->NotifyRequest( KDefaultSupportedUid );
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		   _L("CFeatureNotifier::NotifyRequest - KErrNone expected; error = %d"), err );
	if(err == KErrNoMemory)
		   {
		   CleanupStack::PopAndDestroy(notif);
		   return TestStepResult();
		   }
	TInt count_before = notif->CountAllocCells();
	for (int i=0; i<100;i++)
        CheckNotifyRequestResultL( KDefaultSupportedUid );
	TInt count_after = notif->CountAllocCells();
	TInt count = notif->NumberOfNotifyFeatures();

    // If the count is not 1 item in the array, then there's an error
    if( 1 != count )
 	   { 
  	   TESTDIAGNOSTICERROR( EFalse, 
    			 _L("CFeatureNotifier::NumberOfNotifyFeatures - 1 item is expected; error count = %d"), count );
    	}
    
    // The count before should be the same as the count after
    if( count_before != count_after )
 	   { 
  	   TESTDIAGNOSTICERROR( EFalse, 
    			 _L("CFeatureNotifier::NumberOfNotifyFeatures ; error cells allocated before= %d"), count_before);
  	   TESTDIAGNOSTICERROR( EFalse, 
    			 _L("CFeatureNotifier::NumberOfNotifyFeatures ; error cells allocated after = %d"), count_after);
    	}
    
	err = notif->NotifyCancelAll(); // Cancel the previous notification request.
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		   _L("CFeatureNotifier::NotifyCancel - KErrNone expected; error = %d"), err );

	CleanupStack::PopAndDestroy( notif );
	return TestStepResult();
	}

// -- new function end


   
