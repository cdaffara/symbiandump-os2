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
#include "statestep.h"
#include "efm_test_consts.h"
#include "tef_efm_bursuitedefs.h"
#include "tef_efm_bursuitesimstepbase.h"


CStateStep::~CStateStep()
	{
	}

CStateStep::CStateStep()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KStateStep);
	}

/**
 * This function makes the call to DoThreadActionL
 */
TInt CStateStep::ThreadEntryPoint( TAny* aPtrThis )
	{
	// Creating the cleanup stack for the thread
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if( NULL != cleanupStack )
		{
		TRAPD( err, ((CStateStep*) aPtrThis)->DoThreadActionL() );
		__ASSERT_ALWAYS(!err, User::Panic(_L("Thread panic"),err));
	
		// deleting the cleanup stack for the thread
		delete cleanupStack;
		return( 1 );
		}
	
 	return( 0 );
	}

/**
 * Call the BUR operations in the "wrong" order, to trigger the state machine error handling
 * functionality (and therefore the feature manager error handling code).
 */
TVerdict CStateStep::doTestStepL()
	{
	CTefEfmBURSuiteSimStepBase* simulate = CTefEfmBURSuiteSimStepBase::NewLC();

	TInt add_error( KErrNone );
	TBitFlags32 testFlag;;
	testFlag.Set( EFeatureModifiable );
	TFeatureEntry toAdd( KNewUid1, testFlag, KDefaultData1 );
    SetTestStepResult( EFail );    // assume test step fails

    // set up thread 
	_LIT( KQThreadName, "StateStep Thread" );
	TPtrC threadname( KQThreadName );	
	RThread thread;
	TInt thread_err = thread.Create( threadname, CStateStep::ThreadEntryPoint, KDefaultStackSize, NULL, this );
	if( KErrNone != thread_err )
		{
		User::Panic( _L("CStateStep Thread could not be created"), thread_err );
		}
    
	RFeatureControl control;

	User::LeaveIfError( control.Open() );
	control.DeleteFeature(KNewUid1);
	add_error = control.AddFeature(toAdd);
	if (add_error ==KErrNone)
		{
		TInt setErr = control.SetFeature(KNewUid1, EFalse, KDefaultData1);
		if (setErr == KErrNone)
			{
			// Simulate a backup
			simulate->Simulate_StartBackupL();
			thread.Resume();
			User::After( 3000000 ); // microseconds

			// takes us into an error state
			simulate->Simulate_StartBackupL();
			
			// start restore and end backup
			simulate->Simulate_StartRestoreL();
			simulate->Simulate_EndBackupL();
		
			if( EPass == TestStepResult() ) 
				{
				TInt featSup = control.FeatureSupported( KNewUid1 );
				if (featSup != KFeatureSupported )
					{
					ERR_PRINTF1( _L("feature not supported"));
					SetTestStepResult( EFail );
					}
				
				}
			else
				{
				SetTestStepResult( EPass );
				}
			}
		else
			{
			ERR_PRINTF2( _L("unable to set status err = %d"), setErr);
			SetTestStepResult( EFail );
			}
		}
	else
		{
		ERR_PRINTF2( _L("Cannot add a feature during test setup. %d"), add_error );
		SetTestStepResult( EFail );
		}
	
	control.DeleteFeature( KNewUid1 );
	control.Close();

	CleanupStack::PopAndDestroy(simulate);	
	return TestStepResult();
	}

/**
 * The thread's actions are to open a RFeatureControl and SetFeature on KNewUid1 and then
 * close the connection to the server. Upon an error, the SetTestStepResult function is called
 * and the Test Step is set to EFail.
 */ 
void CStateStep::DoThreadActionL()
	{
	TInt error( KErrNone );
	TBitFlags32  flags( 0 );
	
	flags.Set( EFeatureModifiable );
	TFeatureEntry newEentry( KNewUid1, flags, KDefaultData1 );

	RFeatureControl local;
	User::LeaveIfError( local.Open() );
	
	// set feature for KNewUid1
	error = local.SetFeature( KNewUid1 , ETrue, KDefaultData1 );
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

