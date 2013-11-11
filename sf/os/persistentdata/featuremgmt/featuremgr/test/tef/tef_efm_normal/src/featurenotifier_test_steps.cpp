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

#include <featmgr/featurecontrol.h>
#include "featurenotifier_test_steps.h"
#include <e32cmn.h>	//Semaphore


CFeatureNotifier_NewLStep::CFeatureNotifier_NewLStep()
	{
	SetTestStepName(KFeatureNotifier_NewL);	
	}

TVerdict CFeatureNotifier_NewLStep::doTestStepL()
   {
   CFeatureNotifier* notif = NULL;
   TRAPD(err, notif = CFeatureNotifier :: NewL(*this));
   TEST(err == KErrNone);
   CleanupStack::PushL(notif);
   CleanupStack::PopAndDestroy(notif);
   return TestStepResult();
   }

CFeatureNotifier_NotifyRequestLStep::CFeatureNotifier_NotifyRequestLStep()
   {
   SetTestStepName(KFeatureNotifier_NotifyRequestL);
   }

TVerdict CFeatureNotifier_NotifyRequestLStep::doTestStepL()
   {
   CFeatureNotifier* notif = NULL;
   TRAPD(err, notif = CFeatureNotifier :: NewL(*this));
   CheckCondition(err == KErrNone, err == KErrNoMemory,	   
	   _L("CFeatureNotifier::NewL failed; error = %d"), err );
   // If we see KErrNoMemory here then we're probably performing OOM testing.
   // The notif object is invalid and all we can do is drop out here.
   if(err == KErrNoMemory) return TestStepResult();

   CleanupStack::PushL( notif );	
   err = notif->NotifyRequest( KModifiableUid );
   CheckCondition(err == KErrNone, err == KErrNoMemory,   
	   _L("CFeatureNotifier::NotifyRequest failed; error = %d"), err );
   if(err == KErrNoMemory)
	   {
	   CleanupStack::PopAndDestroy(notif);
	   return TestStepResult();
	   }
   err = notif->NotifyRequest( KModifiableUid );
   CheckCondition(err == KErrAlreadyExists, err == KErrNoMemory, 
	   _L("CFeatureNotifier::NotifyRequest failed; error = %d"), err );
   if(err == KErrNoMemory)
	   {
	   CleanupStack::PopAndDestroy(notif);
	   return TestStepResult();
	   }
   CheckNotifyRequestResultL(KModifiableUid);
   TBitFlags32 flags1( 0 );
   flags1.Set( EFeatureUninitialized );
   flags1.Set( EFeatureModifiable );
   flags1.Set( EFeaturePersisted );
   TFeatureEntry entry4( KNewUid3, flags1, KDefaultData1 );
   err = icontrol.AddFeature( entry4 );
   if (iLowCap)	   
		{
		//if we are running in the low capabilities environment we don't expect the feature 
		//to be modified and notification to be issued
		TESTDIAGNOSTICERROR(err == KErrPermissionDenied,	   
			_L("RFeatureControl::AddFeature - KErrPermissionDenied expected; error = %d"),err);
		}
   else
		{
		CheckCondition(err == KErrNone || err == KErrAlreadyExists, err == KErrNoMemory,
			 _L("RFeatureControl::AddFeature failed; error = %d"),err);
		}
	if(err == KErrNoMemory) // Drop out if we're out of memory - the notification isn't going to work.
		{
		CleanupStack::PopAndDestroy(notif);
		return TestStepResult();
		}
   err = notif->NotifyCancelAll(); // Cancel the previous notification request.
   TESTDIAGNOSTICERROR(err == KErrNone,	   
	   _L("CFeatureNotifier::NotifyCancel - KErrNone expected; error = %d"), err );
   err = notif->NotifyRequest( KNewUid3 );
   CheckCondition(err == KErrNone, err == KErrNoMemory,	   
	   _L("CFeatureNotifier::NotifyRequest failed; error = %d"), err );
   if(err == KErrNoMemory)  // Drop out if we're out of memory - the notification isn't going to work.
	   {
	   CleanupStack::PopAndDestroy(notif);
	   return TestStepResult();
	   }
   CheckDeleteNotificationResultL(KNewUid3);   
   
   CleanupStack::PopAndDestroy( notif );
   return TestStepResult();
   }
	  
CFeatureNotifier_NotifyRequestArrayLStep::CFeatureNotifier_NotifyRequestArrayLStep()
   {
	SetTestStepName(KFeatureNotifier_NotifyRequestArrayL);
   }
   
TVerdict CFeatureNotifier_NotifyRequestArrayLStep::doTestStepL()
   {
   //add a feature to be deleted in the Delete test
   TBitFlags32 flags1( 0 );
   flags1.Set( EFeatureUninitialized );
   flags1.Set( EFeatureModifiable );
   flags1.Set( EFeaturePersisted );
   TFeatureEntry entry4( KNewUid4, flags1, KDefaultData1 );
   TInt err = icontrol.AddFeature( entry4 );
   if (iLowCap)	   
		{
		//if we are running in the low capabilities environment we don't expect the feature 
		//to be modified and notification to be issued
		TESTDIAGNOSTICERROR(err == KErrPermissionDenied,	   
			_L("RFeatureControl::AddFeature - KErrPermissionDenied expected; error = %d"),err);
		}
   else
		{	
		CheckCondition(err == KErrNone, err == KErrNoMemory, 
			 _L("RFeatureControl::AddFeature failed; error = %d"),err);			 
		}
   if (err == KErrNoMemory)
	   return TestStepResult();
			  
   CFeatureNotifier* notif = NULL;
   TRAP(err, notif = CFeatureNotifier :: NewL(*this));
   CheckCondition(err == KErrNone, err == KErrNoMemory,   
	   _L("CFeatureNotifier::NewL failed; error = %d"), err );
   // If we see KErrNoMemory here then we're probably performing OOM testing.
   // The notif object is invalid and all we can do is drop out here.
   if(err == KErrNoMemory) return TestStepResult();

   CleanupStack::PushL( notif );	
   RFeatureUidArray uids;
   CleanupClosePushL( uids );
   
   uids.AppendL( KDefaultUnsupportedUid );
   uids.AppendL( KDefaultSupportedUid );
   uids.AppendL( KModifiableUid );
   uids.AppendL( KUnmodifiableUid );
   uids.AppendL(KNewUid4);
   err = notif->NotifyRequest( uids );
   CheckCondition(err == KErrNone, err == KErrNoMemory,	   
	   _L("CFeatureNotifier::NotifyRequest failed; error = %d"), err );
   if(err == KErrNoMemory)
	   {
	   CleanupStack::PopAndDestroy( &uids );
	   CleanupStack::PopAndDestroy(notif);
	   return TestStepResult();
	   }
   err = notif->NotifyRequest( uids );
   CheckCondition(err == KErrAlreadyExists, err == KErrNoMemory,	   
	   _L("CFeatureNotifier::NotifyRequest - KErrAlreadyExists expected; error = %d"), err );	
   if(err == KErrNoMemory)
	   {
	   CleanupStack::PopAndDestroy( &uids );
	   CleanupStack::PopAndDestroy(notif);
	   return TestStepResult();
	   }  
   
   CheckNotifyRequestResultL(KModifiableUid);
   
   CheckDeleteNotificationResultL(KNewUid4);
   
   CleanupStack::PopAndDestroy( &uids );
   CleanupStack::PopAndDestroy( notif );
   return TestStepResult();
   }
   
   
CFeatureNotifier_NotifyCancelLStep::CFeatureNotifier_NotifyCancelLStep()
   {
   SetTestStepName(KFeatureNotifier_NotifyCancelL);
   }
   
   
TVerdict CFeatureNotifier_NotifyCancelLStep::doTestStepL()
	{
	CFeatureNotifier* notif = NULL;
	TRAPD(err, notif = CFeatureNotifier :: NewL(*this));
	CheckCondition(err == KErrNone, err == KErrNoMemory,  
	   _L("CFeatureNotifier::NewL failed; error = %d"), err );
	// If we see KErrNoMemory here then we're probably performing OOM testing.
	// The notif object is invalid and all we can do is drop out here.
	if(err == KErrNoMemory) 
		{
		return TestStepResult();
		}

	CleanupStack::PushL( notif );	
	err = notif->NotifyRequest( KModifiableUid );
	CheckCondition(err == KErrNone, err == KErrNoMemory, 
	   _L("CFeatureNotifier::NotifyRequest failed; error = %d"), err );
	if(err == KErrNoMemory)
		{
		CleanupStack::PopAndDestroy(notif);
		return TestStepResult();
		}
	err = notif->NotifyCancel( KDefaultSupportedUid );
	CheckCondition(err == KErrNotFound, err == KErrNoMemory, 
	   _L("CFeatureNotifier::NotifyCancel failed; error = %d"), err );
	if(err == KErrNoMemory)
		{
		CleanupStack::PopAndDestroy(notif);
		return TestStepResult();
		}
	err = notif->NotifyCancel( KModifiableUid );
	CheckCondition(err == KErrNone, err == KErrNotFound,
	   _L("CFeatureNotifier::NotifyCancel faield; error = %d"), err );
	if(err == KErrNotFound)
		{
		CleanupStack::PopAndDestroy(notif);
		return TestStepResult();
		}

	iNotifyCompleted = EFalse;
	CheckNotifyCancelResultL(KModifiableUid);
	CleanupStack::PopAndDestroy( notif );
	return TestStepResult();
	}
   
	  
CFeatureNotifier_NotifyMiltipleCancel ::CFeatureNotifier_NotifyMiltipleCancel() 
	{
	SetTestStepName(KFeatureNotifier_NotifyMiltipleCancelL );
	}
   
   
TVerdict CFeatureNotifier_NotifyMiltipleCancel ::doTestStepL()
	{ 
	CFeatureNotifier* notif = NULL;
	TRAPD(err, notif = CFeatureNotifier :: NewL(*this));
	CheckCondition(err == KErrNone, err == KErrNoMemory,
		_L("CFeatureNotifier::NewL failed; error = %d"), err );
	// If we see KErrNoMemory here then we're probably performing OOM testing.
	// The notif object is invalid and all we can do is drop out here.
	if(err == KErrNoMemory) 
		{
		return TestStepResult();
		}

	CleanupStack::PushL( notif );	
	RFeatureUidArray uids;
	CleanupClosePushL( uids );

	uids.AppendL( KDefaultUnsupportedUid );
	uids.AppendL( KDefaultSupportedUid );
	uids.AppendL( KModifiableUid );
	uids.AppendL( KUnmodifiableUid );
	err = notif->NotifyRequest( uids );
	CheckCondition(err == KErrNone, err == KErrNoMemory,
		_L("CFeatureNotifier::NotifyRequest failed; error = %d"), err );
	if(err == KErrNoMemory)
		{
		CleanupStack::PopAndDestroy( &uids );
		CleanupStack::PopAndDestroy(notif);
		return TestStepResult();
		}
	err = notif->NotifyCancel( KDefaultUnsupportedUid );
	CheckCondition(err == KErrNone, err == KErrNotFound,
		_L("CFeatureNotifier::NotifyCancel failed; error = %d"), err );
	if(err == KErrNotFound)
		{
		CleanupStack::PopAndDestroy( &uids );
		CleanupStack::PopAndDestroy(notif);
		return TestStepResult();
		}
	err = notif->NotifyCancel( KDefaultSupportedUid );
	CheckCondition(err == KErrNone, err == KErrNotFound,
		_L("CFeatureNotifier::NotifyCancel failed; error = %d"), err );
	if(err == KErrNotFound)
		{
		CleanupStack::PopAndDestroy( &uids );
		CleanupStack::PopAndDestroy(notif);
		return TestStepResult();
		}
	err = notif->NotifyCancelAll();
	TESTDIAGNOSTICERROR(err == KErrNone,
			_L("CFeatureNotifier::NotifyCancelAll - KErrNone expected; error = %d"), err );
	CheckNotifyCancelResultL(KModifiableUid);
	CleanupStack::PopAndDestroy( &uids );
	CleanupStack::PopAndDestroy( notif );

	return TestStepResult();	
	}
   
   

CFeatureNotifier_UnchangedFeatureTestStep::CFeatureNotifier_UnchangedFeatureTestStep() 
	{
	SetTestStepName(KFeatureNotifier_UnchangedFeatureTest);
	}
   
   
TVerdict CFeatureNotifier_UnchangedFeatureTestStep ::doTestStepL()
	{
	CFeatureNotifier* notif = NULL;
	TRAPD(err, notif = CFeatureNotifier :: NewL(*this));
	TESTDIAGNOSTICERROR(err == KErrNone,	   
	   _L("CFeatureNotifier::NewL - KErrNone expected; error = %d"), err );
	// If we see KErrNoMemory here then we're probably performing OOM testing.
	// The notif object is invalid and all we can do is drop out here.
	if(err == KErrNoMemory) return TestStepResult();

	CleanupStack::PushL( notif );	
	err = notif->NotifyRequest( KModifiableUid );
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		_L("RFeatureControl::NotifyRequest - KErrNone is expected; error = %d"),err);

	WaitL(KWaitDelay);
	TESTDIAGNOSTIC(!iNotifyCompleted,	   
		_L("Feature notification should not be issued because the feature has not been changed"));

	err = notif->NotifyCancel( KModifiableUid );
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		_L("RFeatureControl::NotifyCancel - KErrNone is expected; error = %d"),err);

		
	CleanupStack::PopAndDestroy( notif );
	return TestStepResult();
	}

CFeatureNotifier_CancelFeatureTwiceTestStep::CFeatureNotifier_CancelFeatureTwiceTestStep() 
	{
	SetTestStepName(KFeatureNotifier_CancelFeatureTwiceTest);
	}
   
   
TVerdict CFeatureNotifier_CancelFeatureTwiceTestStep::doTestStepL()
	{
	CFeatureNotifier* notif = NULL;
	TRAPD(err, notif = CFeatureNotifier :: NewL(*this));
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		_L("CFeatureNotifier::NewL - KErrNone expected; error = %d"), err );
	// If we see KErrNoMemory here then we're probably performing OOM testing.
	// The notif object is invalid and all we can do is drop out here.
	if(err == KErrNoMemory) return TestStepResult();

	CleanupStack::PushL( notif );	
	err = notif->NotifyRequest( KModifiableUid );
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		_L("RFeatureControl::NotifyRequest - KErrNone is expected; error = %d"),err);

	err = notif->NotifyCancel( KModifiableUid );
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		_L("RFeatureControl::NotifyCancel - KErrNone is expected; error = %d"),err);

	err = notif->NotifyCancel( KModifiableUid );
	TESTDIAGNOSTICERROR(err == KErrNotFound,	   
		_L("RFeatureControl::NotifyCancel - KErrNotFound is expected since the notification has already been cancelled; error = %d"),err);

	CheckNotifyCancelResultL(KModifiableUid);

	err = notif->NotifyCancel( KDefaultSupportedUid );
	TESTDIAGNOSTICERROR(err == KErrNotFound,	   
		_L("RFeatureControl::NotifyCancel - KErrNotFound is expected since the notification has not been requested; error = %d"),err);

	CleanupStack::PopAndDestroy( notif );
	return TestStepResult();
	}

CFeatureNotifier_NonExistingFeatureNotificationTestStep::CFeatureNotifier_NonExistingFeatureNotificationTestStep() 
	{
	SetTestStepName(KFeatureNotifier_NonExistingFeatureNotificationTest);
	}
   
   
TVerdict CFeatureNotifier_NonExistingFeatureNotificationTestStep::doTestStepL()
	{
	TESTDIAGNOSTIC(icontrol.FeatureSupported( KNewUid2 ) == KErrNotFound,	   
		_L("The feature that is supposed to be not defined has been found - test presets are broken"));

	TInt err=KErrNone;
	CFeatureNotifier* notif = NULL;
	TRAP(err, notif = CFeatureNotifier :: NewL(*this));
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		_L("CFeatureNotifier::NewL failed; error = %d"),err);
	// If we see KErrNoMemory here then we're probably performing OOM testing.
	// The notif object is invalid and all we can do is drop out here.
	if(err == KErrNoMemory) return TestStepResult();

	CleanupStack::PushL( notif );	
	err = notif->NotifyRequest( KNewUid2 );
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		_L("RFeatureControl::NotifyRequest - KErrNone is expected; error = %d"),err);

	TBitFlags32 featureFlags(0x25);
	TFeatureEntry entry(KNewUid2,featureFlags,KChangeData);
	err = icontrol.AddFeature( entry );
	WaitL(KWaitDelay);
	if (iLowCap)	   
		{
	   //if we are running in the low capabilities environment we don't expect the feature 
	   //to be added
		TESTDIAGNOSTICERROR(err == KErrPermissionDenied,	   
			_L("RFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
	   TESTDIAGNOSTIC(!iNotifyCompleted,	   
			_L("Feature notification should not be issued due to insufficient capabilities"));	   
		}
	 else
		{
 		//if test environment has WDD capability then SetFeature should succeed and feature 
 		//change notification should be issued
		TESTDIAGNOSTICERROR(err==KErrNone,
			_L("Failed to add new feature: error = %d"),err);	
 		TESTDIAGNOSTIC(iNotifyCompleted,	   
			_L("Feature notification should be issued after adding the feature"));
 		}	

	CleanupStack::PopAndDestroy( notif );

	return TestStepResult();
	}

CFeatureNotifier_NonExistingFeatureCancellationTestStep::CFeatureNotifier_NonExistingFeatureCancellationTestStep() 
	{
	SetTestStepName(KFeatureNotifier_NonExistingFeatureCancellationTest);
	}
	   
	   
TVerdict CFeatureNotifier_NonExistingFeatureCancellationTestStep::doTestStepL()
	{
	TESTDIAGNOSTIC(icontrol.FeatureSupported( KUnknownUid ) == KErrNotFound,
		_L("The feature that is supposed to be not defined has been found - test presets are broken"));

	TInt err=KErrNone;
	CFeatureNotifier* notif = NULL;
	TRAP(err, notif = CFeatureNotifier :: NewL(*this));
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		_L("CFeatureNotifier::NewL failed; error = %d"),err);
	// If we see KErrNoMemory here then we're probably performing OOM testing.
	// The notif object is invalid and all we can do is drop out here.
	if(err == KErrNoMemory) return TestStepResult();

	CleanupStack::PushL( notif );	
	err = notif->NotifyRequest( KUnknownUid );
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		_L("RFeatureControl::NotifyRequest - KErrNone is expected for an unknown feature; error = %d"),err);

	err = notif->NotifyCancel( KUnknownUid );
	TESTDIAGNOSTICERROR(err == KErrNone,	   
		_L("RFeatureControl::NotifyCancel - KErrNone is expected for an unknown feature; error = %d"),err);

	CleanupStack::PopAndDestroy( notif );		
	return TestStepResult();
	}

CFeatureNotifier_Thread_1_TestStep::CFeatureNotifier_Thread_1_TestStep()
	{
	SetTestStepName(KFeatureNotifier_Thread_1_TestStep);
	}

CFeatureNotifier_Thread_1_TestStep::~CFeatureNotifier_Thread_1_TestStep()
	{
	delete iNotif;
	}

TVerdict CFeatureNotifier_Thread_1_TestStep::doTestStepL()
	{
	INFO_PRINTF1(_L("Thread one created"));
	
	// Defining feature A
	TBitFlags32 flags( 0 );
	flags.Set( EFeatureModifiable );
	TFeatureEntry entryA( KFeatureAUid, flags, KDefaultData1 );
	
	// Adding feature A	
	INFO_PRINTF2(_L("Thread one adding feature 0x%x"), KFeatureAUid.iUid);
	TInt err = icontrol.AddFeature(entryA);
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("Feature addition failed: error = %d"),err);

	// Create a notifier
	TRAP(err, iNotif = CFeatureNotifier :: NewL(*this));
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("CFeatureNotifier::NewL failed; error = %d"),err);

    // Request notification when modifying feature A
	err = iNotif->NotifyRequest( KFeatureAUid );
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("RFeatureControl::NotifyRequest - KErrNone is expected for feature KFeatureAUid; error = %d"),err);
	
	// Resume the second thread
	RSemaphore semaphore2;
	semaphore2.OpenGlobal(_L("semaphore2"), EOwnerProcess);
	semaphore2.Signal();
	
	// Keep the notification request alive until a modification notification is 
	// issued by thread 3. 
	// The length of time to wait is the sum of all times for all feature operations 
	// performed by thread 2. This is because thread 2 is responsible of activating 
	// thread 3 which at the end of its execution modifies feature A (which is being
	// handled here).
	// NOTE: if in the future extra functions are added to this code the time intervals 
	// for the waiting in all threads must be updated. Fairlure to do this will cause 
	// unbalance in the sequence of issuing and receiveing notifications between the threads.
	//	INFO_PRINTF1(_L("Thread one waiting for notification about modifying feature A"));	
	iNotificationTypeToExpect = EFeatureDataUpdated;
	iUIDToExpect = KFeatureAUid;
	TRAP(err, WaitL(KWaitDelay*5.5));
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("WaitL() - KErrNone is expected for feature KFeatureBUid; error = %d"),err);
	TESTDIAGNOSTIC(iNotifyCompleted,       
			_L("Feature notification should not be issued because the feature has not been changed"));
	
	// Cancel notification for feature A
	INFO_PRINTF2(_L("Thread one cancelling notification about feature 0x%x"), KFeatureAUid.iUid );	
 	err = iNotif->NotifyCancel( KFeatureAUid );
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("RFeatureControl::NotifyCancel - KErrNone is expected for feature KFeatureAUid; error = %d"),err);
	
	return TestStepResult();
	}

void CFeatureNotifier_Thread_1_TestStep::HandleNotifyChange( TFeatureChangeType aType, TFeatureEntry aFeature )
	{
	if( aType == iNotificationTypeToExpect )
		{
		if( iUIDToExpect == aFeature.FeatureUid() )
			{
			INFO_PRINTF2(_L("Thread one has been notified of modifying feature 0x%x"), iUIDToExpect.iUid );
			}
		else
			{
			TESTDIAGNOSTICERROR(EFalse, 
					_L("Thread one rnotification for unexpected feature 0x%x"), aFeature.FeatureUid().iUid);
			}
		}
	else
		{
		TESTDIAGNOSTICERROR(EFalse, 
				_L("Thread one has been notified for unexpected notification type: %d"), (TInt)aType);
		}
	
	iWait->AsyncStop();
	iNotifyCompleted = ETrue;	
	}

void CFeatureNotifier_Thread_1_TestStep::HandleNotifyError( TInt aError )
	{
	INFO_PRINTF2(_L("T1: Thread one has notification error %d"), aError);
	}

CFeatureNotifier_Thread_2_TestStep::CFeatureNotifier_Thread_2_TestStep()
	{
	SetTestStepName(KFeatureNotifier_Thread_2_TestStep);
	}

CFeatureNotifier_Thread_2_TestStep::~CFeatureNotifier_Thread_2_TestStep()
	{
	delete iNotif;
	}

TVerdict CFeatureNotifier_Thread_2_TestStep::doTestStepL()
	{
    // Suspending thread 2
	RSemaphore semaphore2;
	semaphore2.CreateGlobal(_L("semaphore2"), 0, EOwnerProcess);
	INFO_PRINTF1(_L("Thread two created and waitng for signal"));
	semaphore2.Wait();
	INFO_PRINTF1(_L("Thread two has been signaled by thread one"));
	
	// Create a notifier
	TInt err=KErrNone;
	TRAP(err, iNotif = CFeatureNotifier :: NewL(*this));
	TESTDIAGNOSTICERROR(err == KErrNone,
			_L("CFeatureNotifier::NewL failed; error = %d"),err);
	
    // Requesting notification for the non-exisiting feature B
    INFO_PRINTF2(_L("Thread two requesting notification for non-existing feature 0x%x"), KFeatureBUid.iUid );
	err = iNotif->NotifyRequest( KFeatureBUid );
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("RFeatureControl::NotifyRequest - KErrNone is expected for feature KFeatureBUid; error = %d"),err);
	
	// Resume the third thread
	RSemaphore semaphore3;
	semaphore3.OpenGlobal(_L("semaphore3"), EOwnerProcess);
	semaphore3.Signal();	
	
	// Keep the request alive to listen to the notification of adding feature B in thread 3.
	//	INFO_PRINTF1(_L("Thread two waiting for notification for adding feature B"));	
	iNotificationTypeToExpect = EFeatureFeatureCreated;
	iUIDToExpect = KFeatureBUid;
	TRAP(err, WaitL(KWaitDelay));	
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("WaitL() - KErrNone is expected for feature KFeatureBUid; error = %d"),err);
	TESTDIAGNOSTIC(iNotifyCompleted,
			_L("Feature notification should not be issued because the feature has not been added"));

	iNotifyCompleted = EFalse;
	
	// Keep the request alive to listen to the notification of modifying feature B in thread 3.
	//	INFO_PRINTF1(_L("Thread two waiting for notification for modifying feature B"));	
	iNotificationTypeToExpect = EFeatureDataUpdated;
	iUIDToExpect = KFeatureBUid;
	TRAP(err, WaitL(KWaitDelay*1.5));
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("WaitL() - KErrNone is expected for feature KFeatureBUid; error = %d"),err);
	TESTDIAGNOSTIC(iNotifyCompleted,
			_L("Feature notification should not be issued because the feature has not been modified"));
	
	iNotifyCompleted = EFalse;
	
	// Keep the request alive to listen to the notification of modifying feature B in thread 3.
	//	INFO_PRINTF1(_L("Thread two waiting for notification for modifying feature B"));	
	iNotificationTypeToExpect = EFeatureNoChange;
	iUIDToExpect = KFeatureBUid;
	TRAP(err, WaitL(KWaitDelay*1.5));
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("WaitL() - KErrNone is expected for feature KFeatureBUid; error = %d"),err);
	TESTDIAGNOSTIC(iNotifyCompleted,
			_L("Feature notification should not be issued because the feature has not been modified"));
	
	iNotifyCompleted = EFalse;

	// Keep the request alive to listen to the notification of deleting feature B in thread 3.
	//	INFO_PRINTF1(_L("Thread two waiting for notification for deleting feature B"));	
	iNotificationTypeToExpect = EFeatureFeatureDeleted;
	iUIDToExpect = KFeatureBUid;
	TRAP(err, WaitL(KWaitDelay*1.5));	
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("WaitL() - KErrNone is expected for feature KFeatureBUid; error = %d"),err);
	TESTDIAGNOSTIC(iNotifyCompleted,
			_L("Feature notification should not be issued because the feature has not been deleted"));

	iNotifyCompleted = EFalse;

	// Cancelling notification for feature B issued by thread 2
	INFO_PRINTF2(_L("Thread two cancelling notification for feature 0x%x"), KFeatureBUid.iUid );	
 	err = iNotif->NotifyCancel( KFeatureBUid );
	// KErrNotFound is expected because the feature has been deleted in the previous step
 	TESTDIAGNOSTICERROR(err == KErrNotFound,	       
			_L("RFeatureControl::NotifyCancel - KErrNotFound is expected for feature KFeatureBUid; error = %d"),err);
	
 	// Requesting notification for feature A which is created by thread 1
	INFO_PRINTF2(_L("Thread two requesting notification for feature  0x%x"), KFeatureAUid.iUid );
	err = iNotif->NotifyRequest( KFeatureAUid );
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("RFeatureControl::NotifyRequest - KErrNone is expected for feature KFeatureAUid; error = %d"),err);	
	
	// Keep the request for A alive until thread 3 modifies the feature.
	//	INFO_PRINTF1(_L("Thread two waiting for notification about modifying feature A"));	
	iNotificationTypeToExpect = EFeatureDataUpdated;
	iUIDToExpect = KFeatureAUid;
	TRAP(err, WaitL(KWaitDelay*1.5));	
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("WaitL() - KErrNone is expected for feature KFeatureBUid; error = %d"),err);
	TESTDIAGNOSTIC(iNotifyCompleted,
			_L("Feature notification should not be issued because the feature has not been added"));

	iNotifyCompleted = EFalse;

	// Cancelling notification for feature A from thread 2.
	INFO_PRINTF2(_L("Thread two cancelling notification for feature  0x%x"), KFeatureAUid.iUid );	
 	err = iNotif->NotifyCancel( KFeatureAUid );
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("RFeatureControl::NotifyCancel - KErrNone is expected for feature KFeatureAUid; error = %d"),err);

	return TestStepResult();
	}

void CFeatureNotifier_Thread_2_TestStep::HandleNotifyChange( TFeatureChangeType aType, TFeatureEntry aFeature )
	{
	// Make sure that the notification type received is that we are expecting at the specific instance in the test
	if( aType == iNotificationTypeToExpect )
		{
		if( iNotificationTypeToExpect == EFeatureFeatureCreated )
			{
			// Make sure that the feature being notified is the one we are requesting notification for at the specific intance in the test
			if( iUIDToExpect == aFeature.FeatureUid() )
				{
				INFO_PRINTF2(_L("Thread two has been notified of adding feature 0x%x"), iUIDToExpect.iUid );
				}
			else
				{
				TESTDIAGNOSTICERROR(EFalse, 
						_L("Thread two received addition notification for unexpected feature 0x%x"), aFeature.FeatureUid().iUid);
				}
			}
		else if( iNotificationTypeToExpect == EFeatureNoChange )
					{
					if( aType == iNotificationTypeToExpect )
						{
						if( iUIDToExpect == aFeature.FeatureUid() )
							{
							INFO_PRINTF2(_L("Thread two has been notified of attempt to modify feature but no change were made 0x%x"), iUIDToExpect.iUid );
							}
						else
							{
							TESTDIAGNOSTICERROR(EFalse, 
									_L("Thread two received attempted modification notification for unexpected feature 0x%x"), aFeature.FeatureUid().iUid);
							}
						}
					}
		else if( iNotificationTypeToExpect == EFeatureDataUpdated )
			{
			if( aType == iNotificationTypeToExpect )
				{
				if( iUIDToExpect == aFeature.FeatureUid() )
					{
					INFO_PRINTF2(_L("Thread two has been notified of modifying feature 0x%x"), iUIDToExpect.iUid );
					}
				else
					{
					TESTDIAGNOSTICERROR(EFalse, 
							_L("Thread two received modification notification for unexpected feature 0x%x"), aFeature.FeatureUid().iUid);
					}
				}
			}
		else if( iNotificationTypeToExpect == EFeatureFeatureDeleted )
			{
			if( aType == iNotificationTypeToExpect )
				{
				if( iUIDToExpect == aFeature.FeatureUid() )
					{
					INFO_PRINTF2(_L("Thread two has been notified of deleting feature 0x%x"), iUIDToExpect.iUid );
					}
				else
					{
					TESTDIAGNOSTICERROR(EFalse, 
							_L("Thread two received deletion notification for unexpected feature 0x%x"), aFeature.FeatureUid().iUid);
					}
				}
			}
		}
	else
		{
		TESTDIAGNOSTICERROR(EFalse, 
				_L("Thread two received notification for unexpected notification type %d"), (TInt)(aType) );
		}

	iWait->AsyncStop();
	iNotifyCompleted = ETrue;	
	}

void CFeatureNotifier_Thread_2_TestStep::HandleNotifyError( TInt aError )
	{
	INFO_PRINTF2(_L("Thread two has notification error %d"), aError);
	}

CFeatureNotifier_Thread_3_TestStep::CFeatureNotifier_Thread_3_TestStep()
	{
	SetTestStepName(KFeatureNotifier_Thread_3_TestStep);
	}

TVerdict CFeatureNotifier_Thread_3_TestStep::doTestStepL()
	{
    // Suspending thread 3
	RSemaphore semaphore3;
	semaphore3.CreateGlobal(_L("semaphore3"), 0, EOwnerProcess);
	INFO_PRINTF1(_L("Thread three created and waitng for signal"));
	semaphore3.Wait();
	INFO_PRINTF1(_L("Thread three has been signaled by thread two"));

	// Defining feature B
	TBitFlags32 flags1( 0 );
	flags1.Set( EFeatureModifiable );
	TFeatureEntry entryB( KFeatureBUid, flags1, KDefaultData1 );
	
	// Adding feature B
	// This will issue a notification to thread 2 which will be waiting for it.
	INFO_PRINTF2(_L("Thread three adding feature 0x%x"), KFeatureBUid.iUid );
	TInt err = icontrol.AddFeature(entryB);
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("Feature B addition failed: error = %d"),err);
	// Add delay until thread 2 handles the addition notification issued.
	//	INFO_PRINTF1(_L("Thread three waiting before modifying feature B"));
	TRAP(err, WaitL(KWaitDelay));
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("WaitL() - KErrNone is expected for feature KFeatureBUid; error = %d"),err);
		
	// Modifying feature B
	// This will issue a notification to thread 2 which will be waiting for it.
	INFO_PRINTF2(_L("Thread three modifying feature  0x%x"), KFeatureBUid.iUid );
	err = icontrol.SetFeature( KFeatureBUid, KChangeData );
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("Feature B modification failed: error = %d"),err);
	// Add delay until thread 2 handles the modification notification issued.
	//	INFO_PRINTF1(_L("Thread three waiting before deleting feature B"));
	TRAP(err, WaitL(KWaitDelay));
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("WaitL() - KErrNone is expected for feature KFeatureBUid; error = %d"),err);
		
	// Modifying feature B - No change happen since the data will be the same 
	// This will issue a notification to thread 2 which will be waiting for it.
	INFO_PRINTF2(_L("Thread three modifying feature  0x%x"), KFeatureBUid.iUid );
	err = icontrol.SetFeature( KFeatureBUid, KChangeData );
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("Feature B modification failed: error = %d"),err);
	// Add delay until thread 2 handles the modification notification issued.
	//	INFO_PRINTF1(_L("Thread three waiting before deleting feature B"));
	TRAP(err, WaitL(KWaitDelay));
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("WaitL() - KErrNone is expected for feature KFeatureBUid; error = %d"),err);
	
	// Deleting feature B
	// This will issue a notification to thread 2 which will be waiting for it.
	INFO_PRINTF2(_L("Thread three deleting feature 0x%x"), KFeatureBUid.iUid );
	err = icontrol.DeleteFeature( KFeatureBUid );
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("Feature B deletion failed: error = %d"),err);
	// Add delay until thread 2 handles the deletion notification issued.
	//	INFO_PRINTF1(_L("Thread three waiting before modifying feature A"));
	TRAP(err, WaitL(KWaitDelay));
	TESTDIAGNOSTICERROR(err == KErrNone,       
			_L("WaitL() - KErrNone is expected for feature KFeatureBUid; error = %d"),err);
		
	// Modify feature A	
	// This will issue a notification to both thread 1 and 2 which will be waiting for it.
	INFO_PRINTF2(_L("Thread three modifying feature 0x%x"), KFeatureAUid.iUid );
	err = icontrol.SetFeature( KFeatureAUid, KChangeData );
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("Feature A modification failed: error = %d"),err);
	
	return TestStepResult();
	}
