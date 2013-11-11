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
// Implementation of the base class for capability aware test steps. 
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include "efm_featurenotifierstepbase.h"

static TInt WaitCallBack( TAny* aSelf )
	{
	if( aSelf )
		{
		TWaitInfo* info = static_cast<TWaitInfo*>( aSelf );
		if( info->iPeriodic )
			{
			info->iPeriodic->Cancel();
			}
		if( info->iWait )
			{
			if( info->iWait->IsStarted() )
				{
				info->iWait->AsyncStop();
				}
			}
		}
	
	return KErrNone;
	}


void CFeatureNotifierStepBase::WaitL( TInt aIntervalInMicorseconds )
	{
	TWaitInfo info;
	
	// Construct periodic
	CPeriodic* periodic = CPeriodic::NewL( CActive::EPriorityStandard );
	CleanupStack::PushL( periodic );
	info.iPeriodic = periodic;
	
	// Construct active scheduler wait
	CActiveSchedulerWait* wait = new( ELeave ) CActiveSchedulerWait;
	CleanupStack::PushL( wait );
	info.iWait = wait;
	iWait = wait;
	
	// Start timer and wait
	TCallBack cb( WaitCallBack, &info );
	periodic->Start( aIntervalInMicorseconds, aIntervalInMicorseconds, cb );
	wait->Start();
	
	// Cleanup
	CleanupStack::PopAndDestroy( wait );
	CleanupStack::PopAndDestroy( periodic );
	}


TVerdict CFeatureNotifierStepBase::doTestStepPreambleL(void)
	{
	CEFMConfigurableTestStepBase::doTestStepPreambleL();
	iSched= new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iSched);		
	TInt err = icontrol.Open();
	TESTDIAGNOSTICERROR(err==KErrNone, 
		 _L("RFeatureControl::Open failed: error = %d"), err);
	return TestStepResult();
	}

TVerdict CFeatureNotifierStepBase::doTestStepPostambleL(void)
	{
	CEFMConfigurableTestStepBase::doTestStepPostambleL();
	CActiveScheduler::Install(NULL);
	delete iSched;
	icontrol.Close();
	return TestStepResult();		
	}


void CFeatureNotifierStepBase :: HandleNotifyChange( TFeatureChangeType /* aType */ , TFeatureEntry /* aFeature */ )
	{
	iWait->AsyncStop();
	iNotifyCompleted = ETrue;	
	}
	
void CFeatureNotifierStepBase:: HandleNotifyError( TInt aError )
   {
   INFO_PRINTF2(_L("CFeatureNotifier::HandleNotifyError %d"),aError);
   SetTestStepResult(EFail);
   }


void CFeatureNotifierStepBase::CheckNotifyRequestResultL(TUid aUid)
	{
	TInt err = icontrol.SetFeature( aUid, KChangeData );
	WaitL(KWaitDelay);
	if (iLowCap)	   
	   {
	   //if we are running in the low capabilities environment we don't expect the feature 
	   //to be modified and notification to be issued
	   TESTDIAGNOSTICERROR(err == KErrPermissionDenied,	   
			  _L("RFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);	   
	   TESTDIAGNOSTIC(!iNotifyCompleted,	   
			  _L("Feature notification should not be issued due to insufficient capabilities"));
	   }
	else
	   {
		//if test environment has WDD capability then SetFeature should succeed and feature 
		//change notification should be issued
	   TESTDIAGNOSTICERROR(err == KErrNone,
				  _L("RFeatureControl::SetFeature - KErrNone expected; error = %d"),err);	   
	   TESTDIAGNOSTIC(iNotifyCompleted,	   
				  _L("Feature notification is expected to be issued"));
	   }
	}

void CFeatureNotifierStepBase::CheckNotifyCancelResultL(TUid aUid)
	{
	TInt err = icontrol.SetFeature( aUid, KChangeData );
	WaitL(KWaitDelay);
	if (iLowCap)	   
	   {
	   //if we are running in the low capabilities environment we don't expect the feature 
	   //to be modified and notification to be issued
	   TESTDIAGNOSTICERROR(err == KErrPermissionDenied,	   
			_L("RFeatureControl::SetFeature - KErrPermissionDenied expected; error = %d"),err);
	   TESTDIAGNOSTIC(!iNotifyCompleted,	   
			_L("Feature notification should not be issued due to insufficient capabilities"));	   
	   }
	else
	   {
		//if test environment has WDD capability then SetFeature should succeed and feature 
		//change notification should be issued
	   TESTDIAGNOSTICERROR(err == KErrNone,
			_L("RFeatureControl::SetFeature - KErrNone expected; error = %d"),err);					
	   TESTDIAGNOSTIC(!iNotifyCompleted,	   
			_L("Feature notification should not be issued as the natification has been cancelled"));
	   }
	}
	
void CFeatureNotifierStepBase::CheckDeleteNotificationResultL(TUid aUid)
	{
	TInt err = icontrol.DeleteFeature(aUid);
   
	if (iLowCap)	   
	   {
	   //if we are running in the low capabilities environment we don't expect the feature 
	   //to be modified and notification to be issued
	   TESTDIAGNOSTICERROR(err == KErrPermissionDenied,	   
			  _L("RFeatureControl::DeleteFeature - KErrPermissionDenied expected; error = %d"),err);
	   }
	else
	   {	
		TESTDIAGNOSTICERROR(err == KErrNone, 
			 _L("RFeatureControl::DeleteFeature - KErrNone or KErrAlreadyExists expected; error = %d"),err);			 
		}
   
   	WaitL(KWaitDelay);
   
   	if (iLowCap)	   
	   {
	   
	   TESTDIAGNOSTIC(!iNotifyCompleted,	   
			  _L("Feature notification should not be issued due to insufficient capabilities"));
	   }
	   
	else
		{
		TESTDIAGNOSTIC(iNotifyCompleted,	   
				  _L("Feature notification is expected to be issued"));	
   
		}	
	}



