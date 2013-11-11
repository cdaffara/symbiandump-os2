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
// This file consists of two classes, one for testing backup and the other for testing restore.
// 
//

/**
 @file
 @test
 @internalComponent
*/

#include <featmgr/featurecontrol.h>
#include "queuestep.h"
#include "efm_test_consts.h"
#include "tef_efm_bursuitedefs.h"
#include "tef_efm_bursuitesimstepbase.h"


CQueueBackupStep::~CQueueBackupStep()
	{
	}

CQueueBackupStep::CQueueBackupStep()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KQUEUEBackupStep);
	}

/**
 * The thread's actions are to open a RFeatureControl and SetFeature on KNewUid1 and then
 * close the connection to the server. Upon an error, the SetTestStepResult function is called
 * and the Test Step is set to EFail.
 */ 
void CQueueBackupStep::DoThreadActionL()
	{
	TInt error( KErrNone );
	RFeatureControl local;
	User::LeaveIfError( local.Open() );

	// set feature for KNewUid1 will be queued during backup
	error = local.SetFeature( KNewUid1, ETrue, KDefaultData1 );     // Set UID1 feature to be TRUE
	if( KErrNone != error )
		{
		// Upon error ...
		ERR_PRINTF1( _L("SetFeature() did not complete correctly.") );
		SetTestStepResult( EFail );
		}
		
	// Exit cleanly
	local.Close();
	return;
	}

/**
 * This function sets up a cleanup stack and then makes the call to DoThreadActionL
 */
TInt CQueueBackupStep::ThreadEntryPoint( TAny* aPtrThis )
	{
	// Creating the cleanup stack for the thread
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if( NULL != cleanupStack )
		{
		TRAPD( err, ((CQueueBackupStep*) aPtrThis)->DoThreadActionL() );
		__ASSERT_ALWAYS(!err, User::Panic(_L("Thread panic in CQUEUEBackupStep::ThreadEntryPoint"),err));
	
		// deleting the cleanup stack for the thread
		delete cleanupStack;
		return( 1 );
		}
	
 	return( 0 );
	}

/**
 * This test confirms that an action that takes place during a backup is queued up and then completed 
 * after the backup has finished correctly.
 */
TVerdict CQueueBackupStep::doTestStepL()
	{
	CTefEfmBURSuiteSimStepBase* simulate = CTefEfmBURSuiteSimStepBase::NewLC();
	TInt set_error( KErrNone );
	TInt add_error( KErrNone );
	TInt sup_error( KErrNone );
	TBitFlags32 testFlag;;
	testFlag.Set( EFeatureModifiable );
	TFeatureEntry toAdd1( KNewUid1, testFlag, KDefaultData1 );
    SetTestStepResult( EFail );    // assume test step fails

    // (1) set up thread 
	_LIT( KQThreadName, "QUEUEStep Backup Thread" );
	TPtrC threadname( KQThreadName );	
	RThread thread;
	TInt thread_err = thread.Create( threadname, CQueueBackupStep::ThreadEntryPoint, KDefaultStackSize, NULL, this );
	if( KErrNone != thread_err )
		{
		User::Panic( _L("CQUEUEBackupStep Thread could not be created"), thread_err );
		}
    
	// (2) Connect to Feature Manager
	RFeatureControl control;
	User::LeaveIfError( control.Open() );
	
	// (3) Add the new feature for the first time
	add_error = control.AddFeature( toAdd1 );
	if( KErrNone != add_error )
		{
		ERR_PRINTF2( _L("Cannot add a feature during test setup. %d"), add_error );
		SetTestStepResult( EFail );
		return TestStepResult();
		}
	
	// (4) Initialise the feature to be FALSE (disabled)
	set_error = control.SetFeature( KNewUid1, EFalse, KDefaultData1 );
	if( KErrNone != set_error )
		{
		ERR_PRINTF2( _L("Cannot set the feature during test setup for backup. %d"), set_error );
		SetTestStepResult( EFail );
		return TestStepResult();
		}
	
	// (5) Simulate a backup starting
	simulate->Simulate_StartBackupL();

	// (6) this sets feature to be TRUE (enabled) -- see thread function DoThreadActionL
	thread.Resume();        
	User::After( 3000000 );   // wait for thread to start SetFeature
	SetTestStepResult( CTefEfmBURSuiteStepBase::CopyFeatureFileBackupL() );

	// (7) Simulate a backup ending
	simulate->Simulate_EndBackupL();

	// (8) We can now test the feature to see if it is supported or not. It should be supported because the
	// request is queued-up during Simulate_StartBackupL and Simulate_EndBackupL and then committed at the end
	// of Simulate_EndBackupL.
	if( EPass == TestStepResult() ) 
		{
		sup_error = control.FeatureSupported( KNewUid1 );
		if( KFeatureSupported != sup_error )
			{
			ERR_PRINTF1( _L("Feature is not supported, which means that Feature Manager is NOT queuing the requests properly during backup"));
			SetTestStepResult( EFail );
			}
		}
	else
		{
		ERR_PRINTF1( _L("The backup did not complete correctly (because CopyFeatureFileBackupL failed)") );
		SetTestStepResult( EFail );
		}
	
	// (9) Cleanup: Destroy thread and close all handles etc..
	thread.Kill( KErrCancel  );
	thread.Close();
	control.DeleteFeature( KNewUid1 );
	control.Close();
	
	CleanupStack::PopAndDestroy(simulate);
	return TestStepResult();
	}



// --------------------------------------------------
// Restore class

CQueueRestoreStep::~CQueueRestoreStep()
	{
	}

CQueueRestoreStep::CQueueRestoreStep()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KQUEUERestoreStep);
	}


/**
 * The thread's actions are to open a RFeatureControl and SetFeature on KNewUid1 and then
 * close the connection to the server. Upon an error, the SetTestStepResult function is called
 * and the Test Step is set to EFail.
 */ 
void CQueueRestoreStep::DoThreadActionUid1L()
	{
	TInt set_error( KErrNone );
	RFeatureControl local;
	User::LeaveIfError( local.Open() );

	// (8)  Async: Queue a pending SetFeature(EFalse) to disable the "restored" KNewUid1 feature;
	set_error = local.SetFeature( KNewUid1, EFalse, KDefaultData1 );
	if( KErrNone != set_error )
		{
		// Upon error ...
		ERR_PRINTF1( _L("SetFeature() did not complete correctly.") );
		SetTestStepResult( EFail );
		}
	
	// Exit cleanly
	local.Close();
	return;
	}

/**
 * The thread's actions are to open a RFeatureControl and AddFeature on KNewUid2 and then
 * close the connection to the server. Upon an error, the SetTestStepResult function is called
 * and the Test Step is set to EFail.
 */ 
void CQueueRestoreStep::DoThreadActionUid2L()
	{
	TInt add_error( KErrNone );
	
	RFeatureControl local;
	User::LeaveIfError( local.Open() );

	// (9)  Async: Queue a pending AddFeature() to create a completely new feature;

	TBitFlags32 testFlag;
	testFlag.Set( EFeatureModifiable );
	testFlag.Set( EFeaturePersisted );
	TFeatureEntry toAdd2( KNewUid2, testFlag, KDefaultData1 );

	add_error = local.AddFeature( toAdd2 );
	if( KErrNone != add_error )
		{
		ERR_PRINTF2( _L("Cannot add a feature during test setup. %d"), add_error );
		SetTestStepResult( EFail );
		}

	// Exit cleanly
	local.Close();
	return;
	}


/**
 * This function sets up a cleanup stack and then makes the call to the correct DoThreadActionL
 */
TInt CQueueRestoreStep::ThreadEntryPointUID1( TAny* aPtrThis )
	{
	// Creating the cleanup stack for the thread
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if( NULL != cleanupStack )
		{
		TRAPD( err, ((CQueueRestoreStep*) aPtrThis)->DoThreadActionUid1L() );
		__ASSERT_ALWAYS(!err, User::Panic(_L("Thread panic in CQUEUERestoreStep::ThreadEntryPoints DoThreadActionUid1L"),err));
	
		// deleting the cleanup stack for the thread
		delete cleanupStack;
		return( 1 );
		}
	
 	return( 0 );
	}

/**
 * This function sets up a cleanup stack and then makes the call to the correct DoThreadActionL
 */
TInt CQueueRestoreStep::ThreadEntryPointUID2( TAny* aPtrThis )
	{
	// Creating the cleanup stack for the thread
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if( NULL != cleanupStack )
		{
		TRAPD( err, ((CQueueRestoreStep*) aPtrThis)->DoThreadActionUid2L() );
		__ASSERT_ALWAYS(!err, User::Panic(_L("Thread panic in CQUEUERestoreStep::ThreadEntryPoints DoThreadActionUid2L"),err));
	
		// deleting the cleanup stack for the thread
		delete cleanupStack;
		return( 1 );
		}
	
 	return( 0 );
	}

/**
 * This test confirms that an action that takes place during a RESTORE is queued up and then completed 
 * after the restore operation has finished correctly.
 */
TVerdict CQueueRestoreStep::doTestStepL()
	{
	CTefEfmBURSuiteSimStepBase* simulate = CTefEfmBURSuiteSimStepBase::NewLC();
	TInt set_error( KErrNone );
	TInt add_error( KErrNone );
	TInt sup_error( KErrNone );
	TInt del_error( KErrNone );
	TInt thread_err( KErrNone );
	TBitFlags32 testFlag;;
	testFlag.Set( EFeatureModifiable );
	testFlag.Set( EFeaturePersisted );
	TFeatureEntry toAdd1( KNewUid1, testFlag, KDefaultData1 );
	SetTestStepResult( EFail );    // assume test step fails

    // (1) set up threads 
	_LIT( KQThreadNameUID1, "QUEUEStep Restore Thread UID1" );
	TPtrC threadname_uid1( KQThreadNameUID1 );	
	RThread thread_uid1;
	thread_err = thread_uid1.Create( threadname_uid1, CQueueRestoreStep::ThreadEntryPointUID1, KDefaultStackSize, NULL, this );
	if( KErrNone != thread_err )
		{
		User::Panic( _L("CQUEUERestoreStep Thread UID1 could not be created"), thread_err );
		}

	_LIT( KQThreadNameUID2, "QUEUEStep Restore Thread UID2" );
	TPtrC threadname_uid2( KQThreadNameUID2 );	
	RThread thread_uid2;
	thread_err = thread_uid2.Create( threadname_uid2, CQueueRestoreStep::ThreadEntryPointUID2, KDefaultStackSize, NULL, this );
	if( KErrNone != thread_err )
		{
		User::Panic( _L("CQUEUERestoreStep Thread UID2 could not be created"), thread_err );
		}

	// (2) Connect to Feature Manager
	RFeatureControl control;
	User::LeaveIfError( control.Open() );
	
	// (3) Add the new feature for the first time
	add_error = control.AddFeature( toAdd1 );
	if( KErrNone != add_error )
		{
		ERR_PRINTF2( _L("Cannot add a feature during test setup. %d"), add_error );
		SetTestStepResult( EFail );
		return TestStepResult();
		}
	
	// (4) Initialise the feature to be ETrue (enabled)
	set_error = control.SetFeature( KNewUid1, ETrue, KDefaultData1 );
	if( KErrNone != set_error )
		{
		ERR_PRINTF2( _L("Cannot set the feature during test setup for restore. %d"), set_error );
		SetTestStepResult( EFail );
		return TestStepResult();
		}
	
	// (5) Simulate a backup starting, copying, and ending.
	simulate->Simulate_StartBackupL();
	SetTestStepResult( CTefEfmBURSuiteStepBase::CopyFeatureFileBackupL() );
	simulate->Simulate_EndBackupL();

	// (6) Delete this feature all-together and prove it doesn't exist in memory.
	del_error = control.DeleteFeature( KNewUid1 );
	if( KErrNone != del_error )
		{
		ERR_PRINTF2( _L("Cannot delete the feature during test setup for restore. %d"), del_error );
		SetTestStepResult( EFail );
		return TestStepResult();
		}
	del_error = control.FeatureSupported( KNewUid1 );
	if( KErrNotFound != del_error )
		{
		ERR_PRINTF2( _L("The UID1 feature ws not deleted correctly during test setup for restore. %d"), del_error );
		SetTestStepResult( EFail );
		return TestStepResult();
		}
	
	// (7) Start a restore operation;
	simulate->Simulate_StartRestoreL();

	// (6) see thread function DoThreadActionL (this thread does two things steps (8) and (9))
	//     thread uid1 -> SetFeature(EFalse) to disable the "restored" feature;
	//     thread uid2 -> AddFeature() to create a completely new feature;
	// There are no assumptions made about which thread will run first
	thread_uid1.Resume();        
	thread_uid2.Resume();        

	// wait for threads to call their Feature() functions then do the Restore copy().
	User::After( 3000000 );   
	SetTestStepResult( CTefEfmBURSuiteStepBase::CopyFeatureFileRestoreL() );

	// (10) End the restore operation in two steps (in the following order)
	//   [a] re-read the new features;
	//   [b] commit the pending change requests;
	simulate->Simulate_EndRestoreL();

	// (11) We can now test the features to see if they are supported or not. They should be supported because the
	// requests are queued-up during Simulate_StartRestoreL and Simulate_EndRestoreL and then committed at the end
	// of Simulate_EndRestoreL. This means two things:
	//    [a] The restored feature KNewUid1 should be restored and end up as disabled if queuing worked
	//    [b] A new feature KNewUid2 should be added.
    
	if( EPass == TestStepResult() ) 
		{
		// [a] restored KUid1
		sup_error = control.FeatureSupported( KNewUid1 );
		if( KFeatureUnsupported != sup_error )
			{
			ERR_PRINTF1( _L("KNewUid1 Feature is not KFeatureUnsupported, which means that Feature Manager is NOT queuing the requests properly during restore"));
			SetTestStepResult( EFail );
			}
		// [b] restored KUid2
		sup_error = control.FeatureSupported( KNewUid2 );
		if( KFeatureUnsupported != sup_error )
			{
			ERR_PRINTF1( _L("KNewUid2 Feature is not KFeatureUnsupported , which means that Feature Manager is NOT queuing the requests properly during restore"));
			ERR_PRINTF1( _L(" -- The KNewUid2 feature should be KFeatureUnsupported , which means that the feature is uninitialised"));
			SetTestStepResult( EFail );
			}
		}
	else
		{
		ERR_PRINTF1( _L("The restore did not complete correctly") );
		SetTestStepResult( EFail );
		}
	
	// (12) Cleanup: Destroy threads and close all handles etc..
	thread_uid1.Kill( KErrCancel  );
	thread_uid1.Close();
	control.DeleteFeature( KNewUid1 );

  	thread_uid2.Kill( KErrCancel  );
	thread_uid2.Close();
	control.DeleteFeature( KNewUid2 );
	control.Close();
	
	CleanupStack::PopAndDestroy(simulate);
	return TestStepResult();
	}


