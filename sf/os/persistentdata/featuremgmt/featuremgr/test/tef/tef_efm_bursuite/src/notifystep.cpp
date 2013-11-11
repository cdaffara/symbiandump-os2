// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include <featmgr/featurecontrol.h>
#include "notifystep.h"
#include "efm_test_consts.h"
#include "tef_efm_bursuitedefs.h"
#include "tef_efm_bursuitesimstepbase.h"



/**
 * Call SetTestStepName in the constructor as the controlling
 * framework uses the test step name immediately following construction to set
 * up the step's unique logging ID.
 */
CNotifyBackupStep::CNotifyBackupStep()
	{
	SetTestStepName( KNotifyBackupStep );
	return;
	}

CNotifyBackupStep::~CNotifyBackupStep()
	{
	return;
	}

/**
 * The actions are to open a RFeatureControl and Delete Feature on KNewUid1 and then
 * close the connection to the server. Upon an error, the SetTestStepResult function is called
 * and the Test Step is set to EFail.
 */ 
void CNotifyBackupStep::DoThreadActionL()
	{
	TInt error( KErrNone );
	RFeatureControl local;
	User::LeaveIfError( local.Open() );

	// set feature for KNewUid1 will be queued during backup
	error = local.DeleteFeature( KNewUid1 );
	if( KErrNone != error )
		{
		// Upon error ...
		ERR_PRINTF1( _L("DeleteFeature() did not complete correctly.") );
		SetTestStepResult( EFail );
		}
		
	// Exit cleanly
	local.Close();
	return;
	}

/**
 * This function sets up a cleanup stack and then makes the call to DoThreadActionL
 */
TInt CNotifyBackupStep::ThreadEntryPoint( TAny* aPtrThis )
	{
	// Creating the cleanup stack for the thread
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if( NULL != cleanupStack )
		{
		TRAPD( err, ((CNotifyBackupStep*) aPtrThis)->DoThreadActionL() );
		__ASSERT_ALWAYS(!err, User::Panic(_L("Thread panic in CNotifyBackupStep::ThreadEntryPoint"),err));
	
		// deleting the cleanup stack for the thread
		delete cleanupStack;
		return( 1 );
		}
	
 	return( 0 );
	}

/**
 * Callback upon a change in the feature's status.
 * Tests if the correct change has occured for a specific stage of the test and 
 * stops the Scheduler.
 */
void CNotifyBackupStep::HandleNotifyChange( TFeatureChangeType aType  , TFeatureEntry  /*aFeature*/  )
	{
	// We are notified of a change.. this should be Deleted 
	if( aType == EFeatureFeatureDeleted )
		{
		SetTestStepResult(EPass);
		}
	else
		{
		SetTestStepResult(EFail);
		}
	
	CActiveScheduler::Stop();
	return;
	}

/**
 * This is used if an error occurs during a notification.
 */
void CNotifyBackupStep::HandleNotifyError( TInt aErr)
	{
	ERR_PRINTF2( _L("Notify error occured %d"), aErr );
	SetTestStepResult(EFail);
	CActiveScheduler::Stop();
	}

/**
 * Tests if a notification occurs when a feature is changed during a backup operation.
 */ 
TVerdict CNotifyBackupStep::doTestStepL()
	{
	CTefEfmBURSuiteSimStepBase* simulate = CTefEfmBURSuiteSimStepBase::NewLC();
	
	TInt add_error( KErrNone );

	TBitFlags32 testFlag;
	testFlag.Set( EFeatureModifiable );
	TFeatureEntry toAdd( KNewUid1, testFlag, KDefaultData1 );
	
    SetTestStepResult( EFail );    // assume test step fails

    // Set up an active scheduler & install it
	CActiveScheduler* schedule = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(schedule);
	
	// Create a notification object
	CFeatureNotifier* notif = NULL;
	notif = CFeatureNotifier :: NewL(*this);
	CleanupStack::PushL( notif );
    
    // set up a thread to delete a feature
	_LIT( KQThreadName, "CNotifyBackupStep Backup Thread" );
	TPtrC threadname( KQThreadName );	
	RThread thread;
	TInt thread_err = thread.Create( threadname, CNotifyBackupStep::ThreadEntryPoint, KDefaultStackSize, NULL, this );
	if( KErrNone != thread_err )
		{
		User::Panic( _L("CNotifyBackupStep Thread could not be created"), thread_err );
		}
    
	// open a local control to add a feature
	RFeatureControl control;
	User::LeaveIfError( control.Open() );

	// Add a feature
	add_error = control.AddFeature(toAdd);
	if( (add_error != KErrNone) && (add_error != KErrAlreadyExists) )
		{
		ERR_PRINTF1( _L("Cannot add a feature during test setup.") );
		SetTestStepResult( EFail );
		return TestStepResult();
		}

	// request notification on UID1
	notif->NotifyRequest(KNewUid1);
	
	// Simulate a backup
	simulate->Simulate_StartBackupL();

	SetTestStepResult( CTefEfmBURSuiteStepBase::CopyFeatureFileBackupL() );

	// Delete the UID1 feature asynchronously during a backup
	thread.Resume();        
	User::After( 3000000 );   // wait for thread to start DeleteFeature
	
	simulate->Simulate_EndBackupL();
	
	// Check to see if we have notification
	CActiveScheduler::Start();
	

	// Cleanup
	
	CleanupStack::PopAndDestroy( notif );
	delete schedule;

	thread.Kill( KErrCancel  );
	thread.Close();
	control.DeleteFeature( KNewUid1 );
	control.Close();
	
	CleanupStack::PopAndDestroy(simulate);
	return TestStepResult();
	}


// ------------

/**
 * Call SetTestStepName in the constructor as the controlling
 * framework uses the test step name immediately following construction to set
 * up the step's unique logging ID.
 */
CNotifyRestoreStep::CNotifyRestoreStep()
	{
	SetTestStepName( KNotifyRestoreStep );
	return;
	}

CNotifyRestoreStep::~CNotifyRestoreStep()
	{
	return;
	}

/**
 * Callback upon a change in the feature's status.
 * Tests if the correct change has occured for a particular stage of the test and 
 * stops the Active Scheduler at the end of the test.
 */
void CNotifyRestoreStep::HandleNotifyChange( TFeatureChangeType aType  , TFeatureEntry  aFeature  )
	{
	static int counter = 0;
	static TVerdict status = EPass;
	// We are notified of a change.. this should be Deleted 
	if( ( aFeature.FeatureUid() == KNewUid1 && aType == EFeatureFeatureDeleted ) ||
	    ( aFeature.FeatureUid() == KNewUid2 && aType == EFeatureFeatureCreated ) ||
	    ( aFeature.FeatureUid() == KNewUid3 && aType == EFeatureStatusUpdated ) )
		{
		if( EPass == status ) 
			{
			status = EPass;
			}
		}
	else
		{
		status = EFail;
		}
	
	counter++;
	if( counter == 3 )
		{
		CActiveScheduler::Stop();
		SetTestStepResult( status );
		}
	
	return;
	}

/**
 * This is used if an error occurs during a notification.
 */
void CNotifyRestoreStep::HandleNotifyError( TInt aErr)
	{
	ERR_PRINTF2( _L("Notify error occured %d"), aErr );
	SetTestStepResult(EFail);
	CActiveScheduler::Stop();
	}


/**
 * Tests if a notification occurs when a feature is changed due to a restore taking place.
 */ 
TVerdict CNotifyRestoreStep::doTestStepL()
	{
	CTefEfmBURSuiteSimStepBase* simulate = CTefEfmBURSuiteSimStepBase::NewLC();
	TInt add_error( KErrNone );
	TInt set_error( KErrNone );
	TInt del_error( KErrNone );

	TBitFlags32 testFlag;
	testFlag.Set( EFeatureModifiable );
	testFlag.Set( EFeaturePersisted );
	TFeatureEntry toAdd1( KNewUid1, testFlag, KDefaultData1 );
	TFeatureEntry toAdd2( KNewUid2, testFlag, KDefaultData1 );
	TFeatureEntry toAdd3( KNewUid3, testFlag, KDefaultData1 );
	
    SetTestStepResult( EFail );    // assume test step fails

    // Set up an active scheduler & install it
	CActiveScheduler* schedule = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(schedule);
	
	// open a local control to add a feature
	RFeatureControl control;
	User::LeaveIfError( control.Open() );

	// Add a feature UID3 first
	add_error = control.AddFeature( toAdd3 );
	if( (add_error != KErrNone) && (add_error != KErrAlreadyExists) )
		{
		ERR_PRINTF1( _L("Cannot add a feature during test setup.") );
		SetTestStepResult( EFail );
		return TestStepResult();
		}
	
	// set feature for KNewUid3 will be queued during backup
	set_error = control.SetFeature( KNewUid3, ETrue, KDefaultData1 );
	if( KErrNone != set_error )
		{
		// Upon error ...
		ERR_PRINTF1( _L("SetFeature() did not complete correctly.") );
		SetTestStepResult( EFail );
		}

	// Add a feature UID2 first
	add_error = control.AddFeature( toAdd2 );
	if( (add_error != KErrNone) && (add_error != KErrAlreadyExists) )
		{
		ERR_PRINTF1( _L("Cannot add a feature during test setup.") );
		SetTestStepResult( EFail );
		return TestStepResult();
		}
	
	// set feature for KNewUid2 will be queued during backup
	set_error = control.SetFeature( KNewUid2, ETrue, KDefaultData1 );
	if( KErrNone != set_error )
		{
		// Upon error ...
		ERR_PRINTF1( _L("SetFeature() did not complete correctly.") );
		SetTestStepResult( EFail );
		}

	// Do a backup
	simulate->Simulate_StartBackupL();
	SetTestStepResult( CTefEfmBURSuiteStepBase::CopyFeatureFileBackupL() );
	simulate->Simulate_EndBackupL();

	// delete the backed up feature, so it will be re-added during a restore
	del_error = control.DeleteFeature( KNewUid2 );
	if( KErrNone != del_error )
		{
		// Upon error ...
		ERR_PRINTF1( _L("SetFeature() did not complete correctly.") );
		SetTestStepResult( EFail );
		}
	
	// set feature for KNewUid3 to new data KChangeData
	set_error = control.SetFeature( KNewUid3, ETrue, KChangeData );
	if( KErrNone != set_error )
		{
		// Upon error ...
		ERR_PRINTF1( _L("SetFeature() did not complete correctly.") );
		SetTestStepResult( EFail );
		}
	
	// Add a feature
	add_error = control.AddFeature( toAdd1 );
	if( (add_error != KErrNone) && (add_error != KErrAlreadyExists) )
		{
		ERR_PRINTF1( _L("Cannot add a feature during test setup.") );
		SetTestStepResult( EFail );
		return TestStepResult();
		}

	// set feature for KNewUid1 will be queued during backup
	set_error = control.SetFeature( KNewUid1, ETrue, KDefaultData1 );     // Set UID1 feature to be TRUE
	if( KErrNone != set_error )
		{
		// Upon error ...
		ERR_PRINTF1( _L("SetFeature() did not complete correctly.") );
		SetTestStepResult( EFail );
		}
	
	// Create a notification object
	CFeatureNotifier* notif1 = CFeatureNotifier :: NewL(*this);
	CleanupStack::PushL( notif1 );

	CFeatureNotifier* notif2 = CFeatureNotifier :: NewL(*this);
	CleanupStack::PushL( notif2 );

	CFeatureNotifier* notif3 = CFeatureNotifier :: NewL(*this);
	CleanupStack::PushL( notif3 );
	
	// request notification on UID1, UID2 and UID3
	notif1->NotifyRequest( KNewUid1 );
	notif2->NotifyRequest( KNewUid2 );
	notif3->NotifyRequest( KNewUid3 );
	
	// Simulate a restore
	simulate->Simulate_StartRestoreL();
	SetTestStepResult( CTefEfmBURSuiteStepBase::CopyFeatureFileRestoreL() );
	simulate->Simulate_EndRestoreL();
	
	// Check to see if we have notification
	CActiveScheduler::Start();

	// Cleanup
	
	CleanupStack::PopAndDestroy( notif3 );
	CleanupStack::PopAndDestroy( notif2 );
	CleanupStack::PopAndDestroy( notif1 );
	CleanupStack::PopAndDestroy(simulate);
	delete schedule;
	
	return TestStepResult();
	}



