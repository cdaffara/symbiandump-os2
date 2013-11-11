// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// EFM test steps for component testing
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <featmgr/featurecontrol.h>
#include <featmgr/featmgr.h>
#include <featdiscovery.h>
#include "component_test_steps.h"
#include <apgcli.h>
#include <apacmdln.h>


#define THREADS 100

void CNotifierObserver::HandleNotifyChange( TFeatureChangeType, TFeatureEntry )
	{
	CActiveScheduler::Stop();
	}

void CNotifierObserver::HandleNotifyError( TInt )
	{
	}

CFeatMgrStressTest::CFeatMgrStressTest()
   {
   SetTestStepName( KFeatMgrStressTest );
   }

CFeatMgrStressTest::~CFeatMgrStressTest()
	{	
	//Wait for a second to let test threads die naturally before trying to 
	//kill them.  If the threads are forcefully killed there can be issues
	//closing iThreadsArr
	User::After(1000000);
	
	// Killing each created thread 
    for ( TInt  i=0; i<iThreadsArr.Count(); ++i)
         {
         iThreadsArr[i].Kill(1);
         }
    // Closing the array of threads
    iThreadsArr.Close();
    // Closing the critical section
    iSection.Close();
	}

TVerdict CFeatMgrStressTest::doTestStepL()
	{
	User::LeaveIfError(iSection.CreateLocal());
	
	for(TInt i=0; i<THREADS; i++)
		{	
		TBuf<10> threadName;
		threadName.Format(_L("Thread%d"), i);

		RThread thread;
		TInt err = thread.Create(threadName, CFeatMgrStressTest::ThreadeEntryPoint, KDefaultStackSize, NULL, this);
	
		if( KErrNone != err )
			{
			User::Panic(_L("Thread could not be created"), err);
			}
		else
			{
			User::LeaveIfError(iThreadsArr.Append(thread));
			iThreadsArr[i].Resume();
			}
		}
	
	// To keep the main thread which created the other threads alive untill the test finishes
	RSemaphore semaphore;
	User::LeaveIfError(semaphore.CreateGlobal(_L("ThreadsSemaphore"), 0));
	semaphore.Wait();
	
	TBuf<500> temp;
	_LIT(KPostfix, "; ");
	
	for( TInt i=0; i<iThreadsInfoArr.Count(); ++i )
		{
		TBool testfailed = EFalse;
		temp.Format(_L("Thread id = %d:  "), iThreadsInfoArr[i].iId);
		
		if( KErrNone != iThreadsInfoArr[i].iStartServerErr )
			{
			temp.Append(_L("RFeatureControl::Open expects KErrNone, error value: "));
			temp.AppendNum(iThreadsInfoArr[i].iStartServerErr);
			temp.Append(KPostfix);
			testfailed = ETrue;
			}
		if( KErrNone != iThreadsInfoArr[i].iAddFeatErr )
			{
			temp.Append(_L("RFeatureControl::AddFeature expects KErrNone, error value: "));
			temp.AppendNum(iThreadsInfoArr[i].iAddFeatErr);
			temp.Append(KPostfix);
			testfailed = ETrue;
			}
		if( KErrNotFound != iThreadsInfoArr[i].iSetFeatErr )
			{
			temp.Append(_L("RFeatureControl::SetFeature expects KErrNotFound, error value: "));
			temp.AppendNum(iThreadsInfoArr[i].iSetFeatErr);
			temp.Append(KPostfix);
			testfailed = ETrue;
			}
		if( KFeatureSupported != iThreadsInfoArr[i].iDetailsFeatErr )
			{
			temp.Append(_L("RFeatureControl::FeatureSupported expects KFeatureSupported, error value: "));
			temp.AppendNum(iThreadsInfoArr[i].iDetailsFeatErr);
			temp.Append(KPostfix);
			testfailed = ETrue;
			}	
		if( KErrNone != iThreadsInfoArr[i].iDeleteFeatErr )
			{
			temp.Append(_L("RFeatureControl::DeleteFeature expects KErrNone, error value: "));
			temp.AppendNum(iThreadsInfoArr[i].iDeleteFeatErr);
			temp.Append(KPostfix);
			testfailed = ETrue;
			}				
		if( KErrNone != iThreadsInfoArr[i].iRequestNotif )
			{
			temp.Append(_L("CFeatureNotifier::NotifyRequest expects KErrNone, error value: "));
			temp.AppendNum(iThreadsInfoArr[i].iRequestNotif);
			temp.Append(KPostfix);
			testfailed = ETrue;
			}
		if( KErrNone != iThreadsInfoArr[i].iSetFeatNotif )
			{
			temp.Append(_L("RFeatureControl::SetFeature after notification request expects KErrNone, error value: "));
			temp.AppendNum(iThreadsInfoArr[i].iSetFeatNotif);
			temp.Append(KPostfix);
			testfailed = ETrue;
			}
		
		if( testfailed )
			{
			TESTDIAGNOSTICERROR(EFalse, temp, i);
			}
		}
	return TestStepResult();	
	}

void CFeatMgrStressTest::DoThreadActionL()
	{
	TThreadId id = RThread().Id();
	TUid uid = {id.Id()};
	uid.iUid += 0x1001000;

	// Create the thread info instance that would hold the results for the tests below
	TThreadTestInfo info;
	info.iId = uid.iUid;
	    
	// A. Create a session to the server 
	RFeatureControl control;
	info.iStartServerErr = control.Open();		// Expected: KErrNone
	
	// B. Adding new feature	
	TBitFlags32  flags( 0 );
    flags.Set( EFeatureSupported );
    flags.Set( EFeatureModifiable );  
	TFeatureEntry newEentry( uid, flags, KDefaultData1 );
	info.iAddFeatErr = control.AddFeature( newEentry );		// Expected: KErrNone
	
	// C. Setting an unexisting feature
	info.iSetFeatErr = control.SetFeature(KUnknownUid, ETrue, 4);		// Expected: KErrNotFound
	
	// D. Requesting feature details
	info.iDetailsFeatErr = control.FeatureSupported( uid );				// Expected: KFeatureSupported
	
	// E. Add notification for some feature	
	// Create an active scheduler
	CActiveScheduler* activescheduler = new(ELeave) CActiveScheduler();
    CleanupStack::PushL(activescheduler); 
    // Install the active scheduler 
    CActiveScheduler::Install(activescheduler);
 	// Create a notifier
	CFeatureNotifier *notif = NULL;
	// Creating the notifier observer
	CNotifierObserver* pObserver = new(ELeave) CNotifierObserver();
	MFeatureObserver* pntr = (MFeatureObserver*)( pObserver ); 
	notif = CFeatureNotifier::NewL(*pntr);    
    CleanupStack::PushL( notif );	
    info.iRequestNotif = notif->NotifyRequest( uid );				// Expected: KErrNone
    info.iSetFeatNotif = control.SetFeature(uid, KChangeData);		// Expected: KErrNone
    CActiveScheduler::Start();
    CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(2);
	
	// F. Delete added feature	
	info.iDeleteFeatErr = control.DeleteFeature( uid );			// Expected: KErrNone
	
	// Closing the session to featmgr server
	control.Close();

	iSection.Wait();	
	iThreadsInfoArr.Append(info);		
	
	if( (++iThreadsCount) == THREADS )
		{
		RSemaphore semaphore;
		User::LeaveIfError(semaphore.OpenGlobal(_L("ThreadsSemaphore")));
		semaphore.Signal();
		iSection.Signal();
		return;
		}
	iSection.Signal();
	}

TInt CFeatMgrStressTest::ThreadeEntryPoint( TAny* aPtrThis)
	{
	// Creating the cleanup stack for the thread
	CTrapCleanup* cleanupStack = CTrapCleanup::New();

	TRAPD( err, ((CFeatMgrStressTest*) aPtrThis)->DoThreadActionL() );
	__ASSERT_ALWAYS(!err, User::Panic(_L("Thread panic"),err));

	// deleting the cleanup stack for the thread
	delete cleanupStack;
 	return 1;
	}

CMulticonnectionTestStep1::CMulticonnectionTestStep1()
   {
   SetTestStepName(KMulticonnectionTest1);
   }

TVerdict CMulticonnectionTestStep1::doTestStepL()
   {
   //Create RFeatureControl object
   RFeatureControl control;
   TInt err = control.Connect();       
   CleanupClosePushL( control );

   //Create a connection via FeatureManager          
   TRAP(err, FeatureManager::InitializeLibL());   
   
   //request feature information using CFeatureDiscovery class
   TBool supported( CFeatureDiscovery::IsFeatureSupportedL(KDefaultSupportedUid) );
   TESTDIAGNOSTIC(supported,
		   _L("Feature state request using FeatureDiscovery failed: the feature is expected to be supported"));
     
   //set feature state using RFeatureControl object
   err = control.SetFeature(KDefaultSupportedUid, EFalse, 0);
   TESTDIAGNOSTICERROR(err==KErrNone,
		   _L("Feature state setting failed: error = %d"),err);
   
   //request feature state using FeatureManager
   supported = FeatureManager::FeatureSupported( KDefaultSupportedUid.iUid );
   TESTDIAGNOSTICERROR(supported==KFeatureUnsupported,
		   _L("Feature state request using FeatureManager failed, KFeatureUnsupported expected, returned value is = %d"),supported);
   
   //close FeatureManager connection
   FeatureManager::UnInitializeLib(); 
   
   //request feature state using RFeatureControl
   supported = control.FeatureSupported( KDefaultSupportedUid );  
   TESTDIAGNOSTICERROR(supported==KFeatureUnsupported,
		   _L("Feature state request using FeatureControl failed, KFeatureUnsupported expected, returned value is = %d"),supported);

   CleanupStack::PopAndDestroy( &control );
   return TestStepResult();
   }

CMulticonnectionTestStep2::CMulticonnectionTestStep2()
   {
   SetTestStepName(KMulticonnectionTest2);
   }

TVerdict CMulticonnectionTestStep2::doTestStepL()
	{
	//request feature information using CFeatureDiscovery class
	TBool supported( CFeatureDiscovery::IsFeatureSupportedL(KDefaultUnsupportedUid) );
	TESTDIAGNOSTIC(!supported,
		   _L("Feature state request using FeatureDiscovery failed: the feature is expected to be unsupported"));

	supported = CFeatureDiscovery::IsFeatureSupportedL(KModifiableUid);
	TESTDIAGNOSTIC(supported,
		   _L("Feature state request using FeatureDiscovery failed: the feature is expected to be supported"));

	supported = CFeatureDiscovery::IsFeatureSupportedL(KUnmodifiableUid);
	TESTDIAGNOSTIC(supported,
		   _L("Feature state request using FeatureDiscovery failed: the feature is expected to be supported"));

	supported = CFeatureDiscovery::IsFeatureSupportedL(KUninitializedUid);
	TESTDIAGNOSTIC(!supported,
		   _L("Feature state request using FeatureDiscovery failed: the feature is expected to be unsupported"));


	return TestStepResult();
	}

CPluginIgnoringTestStep::CPluginIgnoringTestStep()
   {
   SetTestStepName(KPluginIgnoringTest);
   }

TVerdict CPluginIgnoringTestStep::doTestStepL()
	{
   //first we need to check whether the plugin is present in the plugin foled on the C:-drive
    RProcess helperProcess;
    TInt err =helperProcess.Create(_L("pluginhelper.exe"),_L("-? c:\\resource\\plugins\\normal_plugin.rsc"));
    TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("failed to create helper process, returned value is = %d"),err);
    
	TRequestStatus status;    
    helperProcess.Rendezvous(status);
    helperProcess.Resume();	
 	User::WaitForRequest(status);
    if (helperProcess.ExitReason()==KErrNotFound)
    	{
    	INFO_PRINTF1(_L("Plugin file was not found in the plugin folder, test is not performed"));
    	return TestStepResult();
    	}
	
	RFeatureControl control;
	err = control.Connect();       	
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RFeatureControl::Connect failed, returned value is = %d"),err);
	
	err = control.FeatureSupported( KPluginIgnoringUid );
	TESTDIAGNOSTICERROR(err==KErrNotFound,
			   _L("C:-drive based plugin has not been ignored, the feature is expected to be not found, returned value is = %d"),err);
	
	control.Close();

	return TestStepResult();
	}

CNotificationHandler::CNotificationHandler(TInt aHandlerID, MNotificationOrderVerifier* aNotificationOrderVerifier) : 
iNotificationOrderVerifier(aNotificationOrderVerifier),
iID(aHandlerID)
	{
	}

void CNotificationHandler::HandleNotifyChange( TFeatureChangeType, TFeatureEntry aFeature )
	{
	iNotificationOrderVerifier->OnNotification(iID, aFeature.FeatureUid().iUid);	
	}

void CNotificationHandler::HandleNotifyError( TInt /*aError*/ ) 
	{
	}

CMultipleNotificationRequestsStep::CMultipleNotificationRequestsStep()
	{
	SetTestStepName(KMultipleNotificationRequests);
	}

TVerdict CMultipleNotificationRequestsStep::doTestStepPreambleL(void)
	{
	notificationHandler1 =  new (ELeave)CNotificationHandler(CNotificationHandler::EHandler1, this);
	CleanupStack::PushL( notificationHandler1 );
	notificationHandler2 =  new (ELeave)CNotificationHandler(CNotificationHandler::EHandler2, this);
	CleanupStack::Pop( notificationHandler1 );
	return TestStepResult();
	}

TVerdict CMultipleNotificationRequestsStep::doTestStepPostambleL(void)
	{
	delete notificationHandler1;
	delete notificationHandler2;
	return TestStepResult();
	}
	
TVerdict CMultipleNotificationRequestsStep::doTestStepL()
	{
	// Install the active scheduler
	CActiveScheduler* activescheduler = new(ELeave) CActiveScheduler();
	CleanupStack::PushL(activescheduler); 	 
	CActiveScheduler::Install(activescheduler);
	
	TInt err = KErrNone;
	// Creating notifiers
	CFeatureNotifier* notif1 = CFeatureNotifier::NewL(*notificationHandler1);
	CleanupStack::PushL( notif1 );
	
	CFeatureNotifier* notif2 = CFeatureNotifier::NewL(*notificationHandler2);	
	CleanupStack::PushL( notif2 );
		
	TBitFlags32 flags( 0 );
	flags.Set( EFeatureSupported);
	flags.Set( EFeatureModifiable );

	TFeatureEntry entry1( KNewUid1, flags, KDefaultData1 );
	TFeatureEntry entry2( KNewUid2, flags, KDefaultData1 );

	RFeatureControl control;
	err = control.Open();
	CleanupClosePushL( control );
	
	TESTDIAGNOSTICERROR(err==KErrNone,
		_L("RFeatureControl::Open failed, returned value is = %d"),err);			 
	
	err = control.AddFeature(entry1);
	TESTDIAGNOSTICERROR(err==KErrNone||err==KErrAlreadyExists,
		_L("RFeatureControl::AddFeature failed, returned value is = %d"),err);
	err = control.AddFeature(entry2);	
	TESTDIAGNOSTICERROR(err==KErrNone||err==KErrAlreadyExists,
		_L("RFeatureControl::AddFeature failed, returned value is = %d"),err);
	err = notif1->NotifyRequest( KNewUid1 );
	TESTDIAGNOSTICERROR(err==KErrNone,
		_L("CFeatureNotifier::NotifyRequest failed, returned value is = %d"),err);
	err = notif2->NotifyRequest( KNewUid2 );
	TESTDIAGNOSTICERROR(err==KErrNone,
		_L("CFeatureNotifier::NotifyRequest failed, returned value is = %d"),err);
	control.SetFeature(KNewUid1, KChangeData);
	TESTDIAGNOSTICERROR(err==KErrNone,
		_L("RFeatureControl::SetFeature failed, returned value is = %d"),err);		
	control.SetFeature(KNewUid2, KChangeData);
	TESTDIAGNOSTICERROR(err==KErrNone,
		_L("RFeatureControl::SetFeature failed, returned value is = %d"),err);
	if (TestStepResult() == EFail)
		return TestStepResult();		
	CActiveScheduler::Start();
	
	control.DeleteFeature(KNewUid1);
	control.DeleteFeature(KNewUid2);
	
	CleanupStack::PopAndDestroy( &control );
	
	CleanupStack::PopAndDestroy( notif2 );
	CleanupStack::PopAndDestroy( notif1 );
	CleanupStack::PopAndDestroy( activescheduler );
	
	return TestStepResult();
	}

// This function is defined so that CSingleNotificationRequest1 and CSingleNotificationRequest2
// objects are able to print to TEF log files. It was not possible to print from within their
// HandleNotifyChange functions.
void CMultipleNotificationRequestsStep::OnNotification(TInt aHandlerID, TInt aFeatureID)
	{
	if( aHandlerID == CNotificationHandler::EHandler1 )
		{
		TESTDIAGNOSTICERROR(aFeatureID == KNewUid1.iUid, _L("CSingleNotificationRequest1::HandleNotifyChange - feature KNewUid1 is expected; feature recevied = %d"), aFeatureID );
		}
	else if( aHandlerID == CNotificationHandler::EHandler1 )
		{
		TESTDIAGNOSTICERROR(aFeatureID == KNewUid2.iUid, _L("CSingleNotificationRequest2::HandleNotifyChange - feature KNewUid2 is expected; feature recevied = %d"), aFeatureID );
		}
	if (++iNotificationCounter == 2)
		{
		CActiveScheduler::Stop();
		}
	}

CFeatmgrStartEndInstall::CFeatmgrStartEndInstall()
	{
	SetTestStepName(KFeatmgrStartEndInstall);
	}

TVerdict CFeatmgrStartEndInstall::doTestStepL()
	{
	RFeatureControl control;
	TInt err(KErrNone);
	
	err = control.Open();       	
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RFeatureControl::Open failed, returned value is = %d"),err);
		
	
	/*
	* General Test Cases
	*/
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 1: Calling SWIStart without SWI running
	// SWIStart
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 1: Calling SWIStart without SWI running"));
	err = control.SWIStart();
	TESTDIAGNOSTICERROR(err==KErrNotReady, 
			_L("RFeatureControl::SWIStart expects KErrNotReady; returned value is = %d"),err);
	// Wait before starting the next test
	User::After(100000);
	
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 2: Call SWIEnd without SWI running 
	// SWIEnd
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 2: Call SWIEnd without SWI running"));
	err = control.SWIEnd();
	TESTDIAGNOSTICERROR(err==KErrNotReady, 
			_L("RFeatureControl::SWIEnd expects KErrNotReady; returned value is = %d"),err);
	control.Close();
	// Wait before starting the next test
	User::After(100000);

			
	/*
	* SWI Installation Test Cases 
	*/
	
	// Locals for the test scenarios
	RProcess process; 
	TBitFlags32 flags( 0 ); 	
	flags.Set( EFeatureSupported);
	flags.Set( EFeatureModifiable );
	TFeatureEntry entry1( KNewUid1, flags, KDefaultData1 );
	TFeatureEntry entry3( KNewUid3, flags, KDefaultData1 );
	TFeatureEntry query1(KNewUid1);
	TFeatureEntry query3(KNewUid3);
	TUint32 data;
	
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 3: Successful installation and features manipulation
	// P&S install -> SWIStart -> SWIEnd -> P&S success
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 3: Successful installation and features manipulation"));
	// Launching dummyswi.exe
	_LIT(KDummySWIPath, "dummyswi.exe");
	err = process.Create(KDummySWIPath, _L("1"));	// 1 = install success
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
	
		// This delay is used to allow P&S to complete the initial setting of the 
		// KSAUidSoftwareInstallKeyValue property to ESASwisInstall before we proceed with 
		// our test case. Otherwise SWIStart will be called before the property is set 
		// properly and will return KErrNotReady. 
		User::After(200000);
		
		err = control.Open();       	
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
		
		// Adding features
		err = control.AddFeature(entry1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		err = control.AddFeature(entry3);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid3; returned value is = %d"),err);
	
		// Enable feature
		err = control.EnableFeature(KNewUid1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
		        _L("RFeatureControl::EnableFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		
		// Disable feature
		err = control.DisableFeature(KNewUid1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
		        _L("RFeatureControl::DisableFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		
		// Set features
		err = control.SetFeature(KNewUid1, ETrue, KChangeData);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SetFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		err = control.SetFeature(KNewUid3, KChangeData);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SetFeature expects KErrNone for KNewUid3; returned value is = %d"),err);
		
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported before delete expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);

		// Delete KNewUid1
		err = control.DeleteFeature(KNewUid1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::DeleteFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		
		//
		// Check features are supported and retrieve their details BEFORE they are 
		// committed to the server.
		//
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported after delete expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
		
		// KNewUid3
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid3; returned value is = %d"),err);
		data = query3.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid3; returned value is = %d"),data);
	
		err = control.SWIEnd();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIEnd expects KErrNone; returned value is = %d"),err);
		
		// Delay until the P&S property is set to ESASwisStatusSuccess in dummyswi.exe to indicate successful installation
		User::After(1500000);
		
		//
		// Check features are supported and retrieve their details AFTER they are 
		// committed in the server.
		//
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
		
		// KNewUid3
		err = control.FeatureSupported(query3);	
		TESTDIAGNOSTICERROR(err==KFeatureSupported, 
				_L("RFeatureControl::FeatureSupported expects KFeatureSupported for KNewUid3; returned value is = %d"),err);
		data = query3.FeatureData();
		TESTDIAGNOSTICERROR(data==KChangeData, 	
				_L("TFeatureEntry::FeatureData expects KChangeData for KNewUid3; returned value is = %d"),data);
		
		// Delete added feature to clean up
		err = control.DeleteFeature(KNewUid3);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::DeleteFeature expects KErrNone for KNewUid3; returned value is = %d"),err);
		// Check that feature has been really deleted
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid3; returned value is = %d"),err);
		
		// Resetting
		query1 = TFeatureEntry(KNewUid1);
		query3 = TFeatureEntry(KNewUid3);
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);

	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 4: Call to SWIStart without a corresponding call to SWIEnd during installation 
	// (without time out).
	// Also simulates the case of an exe that calls SWIStart but crashes before calling SWIEnd.
	// P&S install -> SWIStart -> P&S success
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 4: Call to SWIStart without a corresponding call to SWIEnd during installation (without time out)"));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("1"));	
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);
		
		err = control.Open();       	
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		// Adding features
		err = control.AddFeature(entry1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
	
		// Delay until the P&S property is set to ESASwisStatusSuccess in dummyswi.exe to indicate successful installation
		User::After(1300000);
		
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		// Resetting
		query1 = TFeatureEntry(KNewUid1);
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);

	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 5: Call to SWIStart without a corresponding call to SWIEnd during installation 
	// (with time out).
	// P&S install -> SWIStart -> Time-out -> P&S success
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 5: Call to SWIStart without a corresponding call to SWIEnd during installation (with time out)"));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("3"));	// 3 = install timed-out
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();       	
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		// Adding features
		err = control.AddFeature(entry1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
	
		// Delay until the P&S property is set to ESASwisStatusAborted in dummyswi.exe
		User::After(16300000);
		
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		// Resetting
		query1 = TFeatureEntry(KNewUid1);
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);

	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 6: Abort SWI install after launched exe makes a call to SWIEnd
	// P&S install -> SWIStart -> SWIEnd -> P&S abort
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 6: Abort SWI install after launched exe makes a call to SWIEnd"));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("2"));	// 2 = install abort
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
			
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();       	
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		// Adding features
		err = control.AddFeature(entry1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		err = control.SWIEnd();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIEnd expects KErrNone; returned value is = %d"),err);
		
		// Delay until the P&S property is set to ESASwisStatusAborted in dummyswi.exe
		User::After(1500000);
	
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		// Resetting
		query1 = TFeatureEntry(KNewUid1);
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);

		
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 7: Abort SWI install without launched exe making a call to SWIEnd (timed-out)
	// P&S install -> SWIStart -> P&S abort -> Time-out
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 7: Abort SWI install without launched exe making a call to SWIEnd (timed-out)"));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("2"));
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		// Adding features
		err = control.AddFeature(entry1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
	
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		// Delay until the P&S property is set to ESASwisStatusAborted in dummyswi.exe then fire timeout
		User::After(16300000);
	
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		// resetting
		query1 = TFeatureEntry(KNewUid1);
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);

	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 8: Double call to SWIStart during installation. First call will be effective
	// while the second will be ignored and will return KErrInUse.
	// It handles as well the case when no features are added between SWIStart and SWIEnd.
	// P&S install -> SWIStart -> SWIStart -> SWIEnd -> P&S success
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 8: Double call to SWIStart during installation"));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("1"));
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();       	
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrInUse, 
				_L("RFeatureControl::SWIStart expects KErrInUse; returned value is = %d"),err);
	
		err = control.SWIEnd();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		// Delay until the P&S property is set to ESASwisStatusSuccess in dummyswi.exe to indicate successful installation
		User::After(1500000);
	
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);
	
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 9: Call to SWIStart followed by double calls to SWIEnd during installation.
	// First call will be effective and second will be ignored and will return KErrNotReady.
	// P&S install -> SWIStart -> SWIEnd -> SWIEnd -> P&S success
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 9: Call to SWIStart followed by double calls to SWIEnd during installation"));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("1"));	
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();       	
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
		
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
		
		err = control.SWIEnd();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		err = control.SWIEnd();	
		TESTDIAGNOSTICERROR(err==KErrNotReady, 
				_L("RFeatureControl::SWIStart expects KErrNotReady; returned value is = %d"),err);
	
		// Delay until the P&S property is set to ESASwisStatusAborted in dummyswi.exe
		User::After(1500000);
		
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);
	
	
	/*
	* SWI Uninstall Test Cases 
	*/
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 10: Successful uninstallation and features manipulation
	// P&S uninstall -> SWIStart -> SWIEnd -> P&S success
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 10: Successful uninstallation and features manipulation"));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("4"));	// 4 = uninstall success
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();       	
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
			
		// Adding features
		err = control.AddFeature(entry1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		err = control.AddFeature(entry3);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid3; returned value is = %d"),err);
	
		// Set features
		err = control.SetFeature(KNewUid1, ETrue, KChangeData);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SetFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		err = control.SetFeature(KNewUid3, KChangeData);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SetFeature expects KErrNone for KNewUid3; returned value is = %d"),err);
		
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported before delete expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
		
		// Delete KNewUid1
		err = control.DeleteFeature(KNewUid1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::DeleteFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		
		// Check features are supported and retrieve their details BEFORE they are 
		// committed in the server.
		//
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported after delete expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
		
		// KNewUid3
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid3; returned value is = %d"),err);
		data = query3.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid3; returned value is = %d"),data);
	
		err = control.SWIEnd();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIEnd expects KErrNone; returned value is = %d"),err);
		
		// Delay until the P&S property is set to ESASwisStatusSuccess in dummyswi.exe to indicate successful installation
		User::After(1500000);
		
		//
		// Check features are supported and retrieve their details AFTER they are 
		// committed in the server.
		//
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
		
		// KNewUid3
		err = control.FeatureSupported(query3);	
		TESTDIAGNOSTICERROR(err==KFeatureSupported, 
				_L("RFeatureControl::FeatureSupported expects KFeatureSupported for KNewUid3; returned value is = %d"),err);
		data = query3.FeatureData();
		TESTDIAGNOSTICERROR(data==KChangeData, 	
				_L("TFeatureEntry::FeatureData expects KChangeData for KNewUid3; returned value is = %d"),data);
	
		// Delete added feature to clean up
		err = control.DeleteFeature(KNewUid3);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::DeleteFeature expects KErrNone for KNewUid3; returned value is = %d"),err);
		// Making sure feature deleted
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		
		// Resetting
		query1 = TFeatureEntry(KNewUid1);
		query3 = TFeatureEntry(KNewUid3);	
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);

		
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 11: Call to SWIStart without a corresponding call to SWIEnd during uninstallation 
	// (without time out).
	// Also simulates the case of an exe that calls SWIStart but crashes before calling SWIEnd.
	// P&S uninstall -> SWIStart -> P&S success
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 11: Call to SWIStart without a corresponding call to SWIEnd during uninstallation"));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("4"));	
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();       	
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		// Adding features
		err = control.AddFeature(entry1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
	
		// Delay until the P&S property is set to ESASwisStatusSuccess in dummyswi.exe to indicate successful installation
		User::After(1500000);
		
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		// Resetting
		query1 = TFeatureEntry(KNewUid1);
		control.Close();	
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);
	
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 12: Call to SWIStart without a corresponding call to SWIEnd during uninstallation 
	// (with time out).
	// P&S uninstall -> SWIStart -> Time-out -> P&S success
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 12: Call to SWIStart without a corresponding call to SWIEnd during uninstallation (with time out)"));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("6"));	// 6 = uninstall timed-out
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();       	
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		// Delay until the P&S property is set to ESASwisStatusAborted in dummyswi.exe
		User::After(16300000);
		
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);

	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 13: Abort SWI uninstall after launched exe makes a call to SWIEnd
	// P&S uninstall -> SWIStart -> SWIEnd -> P&S abort
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 13: Abort SWI uninstall after launched exe makes a call to SWIEnd"));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("5"));	// 5 = Abort uninstall
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
			
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();       	
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		// Adding features
		err = control.AddFeature(entry1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		err = control.SWIEnd();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIEnd expects KErrNone; returned value is = %d"),err);
		
		// Delay until the P&S property is set to ESASwisStatusAborted in dummyswi.exe
		User::After(1500000);
	
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		query1 = TFeatureEntry(KNewUid1);
	
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);

	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 14: Abort SWI uninstall without launched exe making a call to SWIEnd 
	// P&S uninstall -> SWIStart -> P&S abort -> Time out
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 14: Abort SWI uninstall without launched exe making a call to SWIEnd"));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("5"));
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		// Adding features
		err = control.AddFeature(entry1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
	
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		// Delay until the P&S property is set to ESASwisStatusAborted in dummyswi.exe then fire timeout
		User::After(16300000);
	
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		// resetting
		query1 = TFeatureEntry(KNewUid1);
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000); 

	
	/*
	* Simultaneous calls to FeatMgr
	*/

	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 15: Simultaneous additions to FeatMgr by the launched exe from SWI (after calling
	// SWIStart) and by another external exe (helping_exe.exe).
	// P&S install -> SWIStart -> AddFeature -> Launch helping_exe ------------> SWIEnd -> P&S success
	//													|				      |
	//													-----> AddFeature ---->
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 15: Simultanuous additions to FeatMgr by the launched exe from SWI (after calling SWIStart) and by another external exe."));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("4"));
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		// KNewUid1 does not exist at this point
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
		
		// KNewUid3 added by this exe
		err = control.AddFeature(entry3);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid3; returned value is = %d"),err);
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid3; returned value is = %d"),err);
		data = query3.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid3; returned value is = %d"),data);
	
		// Starting helping exe
		_LIT(KHelpingExePath, "helping_exe.exe");
		err = process.Create(KHelpingExePath, _L("1"));
		process.Resume();
		
		// Wait for helping_exe to finish
		User::After(600000);
	
		// KNewUid1: added by the external exe (helping_exe)
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KFeatureSupported,
				_L("RFeatureControl::FeatureSupported expects KFeatureSupported for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==KDefaultData1,
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		// KNewUid3 not commited yet to registry
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KErrNotFound,
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid13 returned value is = %d"),err);
		data = query3.FeatureData();
		TESTDIAGNOSTICERROR(data==0,
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid3; returned value is = %d"),data);
	
		err = control.SWIEnd();
		TESTDIAGNOSTICERROR(err==KErrNone,
				_L("RFeatureControl::SWIEnd expects KErrNone; returned value is = %d"),err);
		
		// Delay until the P&S property is set to ESASwisStatusSuccess in dummyswi.exe to indicate successful installation
		User::After(1000000);
		
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KFeatureSupported,
				_L("RFeatureControl::FeatureSupported expects KFeatureSupported for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==KDefaultData1,
				_L("TFeatureEntry::FeatureData expects KDefaultData1 for KNewUid1; returned value is = %d"),data);
	
		// KNewUid3
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KFeatureSupported,
				_L("RFeatureControl::FeatureSupported expects KFeatureSupported for KNewUid13 returned value is = %d"),err);
		data = query3.FeatureData();
		TESTDIAGNOSTICERROR(data==KDefaultData1,
				_L("TFeatureEntry::FeatureData expects KDefaultData1 for KNewUid3; returned value is = %d"),data);
	
		//
		// Deleting added feature to clean up
		//
		err = control.DeleteFeature(KNewUid1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::DeleteFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		// Check that feature has been really deleted
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		err = control.DeleteFeature(KNewUid3);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::DeleteFeature expects KErrNone for KNewUid3; returned value is = %d"),err);
		// Check that feature has been really deleted
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid3; returned value is = %d"),err);
		
		// Resetting
		query1 = TFeatureEntry(KNewUid1);
		query3 = TFeatureEntry(KNewUid3);
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);

	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 16: A call to SWIStart from this exe and a call to SWIEnd from helping_exe followed
	// after that by a call to SWIEnd from this exe. 
	// P&S install -> SWIStart (exe1) -> Add feature (exe1) -> SWIEnd (exe2) -> SWIEnd (exe1) -> P&S success
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 16: A call to SWIStart from this exe and a call to SWIEnd from helping_exe followed after that by a call to SWIEnd from this exe."));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("1"));
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
			
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
	
		// Adding KNewUid1
		err = control.AddFeature(entry1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound,
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0,
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
	
		// Starting helping exe which is going to call SWIEnd (passing 2 as a parameter to indicate this)
		_LIT(KHelpingExePath, "helping_exe.exe");
		err = process.Create(KHelpingExePath, _L("2"));
		process.Resume();
		
		User::After(500000);
			
		// KNewUid1 : will not be committed to registry by the call to SWIEnd from the helping_exe
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound,
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0,
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
			
		err = control.SWIEnd();
		TESTDIAGNOSTICERROR(err==KErrNone,
				_L("RFeatureControl::SWIEnd expects KErrNone; returned value is = %d"),err);
		
		// Delay until the P&S property is set to ESASwisStatusSuccess in dummyswi.exe to indicate successful installation
		User::After(1000000);
		
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KFeatureSupported,
				_L("RFeatureControl::FeatureSupported expects KFeatureSupported for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==KDefaultData1,
				_L("TFeatureEntry::FeatureData expects KDefaultData1 for KNewUid1; returned value is = %d"),data);

		// Delete a feature to clean up
		err = control.DeleteFeature(KNewUid1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::DeleteFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
	
		// Resetting
		query1 = TFeatureEntry(KNewUid1);
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);
	

	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 17: A call to SWIStart-SWIEnd followed by another call to SWIStart-SWIEnd before
	// the SWI successfully finishes. This will work fine and will have the features cached by  
	// the two tuples. However, it is not a recommended use of the API.
	// P&S install -> SWIStart -> SWIEnd -> SWIStart ->  SWIEnd -> P&S success
	////////////////////////////////////////////////////////////////////////////////////////////
	INFO_PRINTF1(_L("SCENARIO 17: A call to SWIStart-SWIEnd followed by another call to SWIStart-SWIEnd."));
	// Launching dummyswi.exe
	err = process.Create(KDummySWIPath, _L("1"));
	TESTDIAGNOSTICERROR(err==KErrNone,
			   _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
	if( err==KErrNone )
		{
		process.Resume();
		
		// Allow P&S to complete initialising
		User::After(200000);

		err = control.Open();
		TESTDIAGNOSTICERROR(err==KErrNone,
				   _L("RFeatureControl::Open failed, returned value is = %d"),err);
		
		//
		// First SWIStart-SWIEnd call
		//
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);	

		// Adding feature KNewUid1
		err = control.AddFeature(entry1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound,
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0,
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
		
		err = control.SWIEnd();
		TESTDIAGNOSTICERROR(err==KErrNone,
				_L("RFeatureControl::SWIEnd expects KErrNone; returned value is = %d"),err);
		
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),data);
		
		User::After(200000);			
		
		//
		// Second SWIStart-SWIEnd call
		//
		err = control.SWIStart();
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
		
		// Adding features KNewUid3
		err = control.AddFeature(entry3);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::AddFeature expects KErrNone for KNewUid3; returned value is = %d"),err);
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KErrNotFound,
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid3; returned value is = %d"),err);
		data = query3.FeatureData();
		TESTDIAGNOSTICERROR(data==0,
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid3; returned value is = %d"),data);
		
		err = control.SWIEnd();
		TESTDIAGNOSTICERROR(err==KErrNone,
				_L("RFeatureControl::SWIEnd expects KErrNone; returned value is = %d"),err);
		
		// KNewUid3
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 	
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid3; returned value is = %d"),err);
		data = query3.FeatureData();
		TESTDIAGNOSTICERROR(data==0, 	
				_L("TFeatureEntry::FeatureData expects 0 for KNewUid3; returned value is = %d"),data);
		
		// Delay until the P&S property is set to ESASwisStatusSuccess in dummyswi.exe to indicate successful installation
		User::After(1100000);
		
		// KNewUid1
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KFeatureSupported, 	
				_L("RFeatureControl::FeatureSupported expects KFeatureSupported for KNewUid1; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==KDefaultData1, 	
				_L("TFeatureEntry::FeatureData expects KDefaultData1 for KNewUid1; returned value is = %d"),data);
		
		// KNewUid3
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KFeatureSupported, 	
				_L("RFeatureControl::FeatureSupported expects KFeatureSupported for KNewUid3; returned value is = %d"),err);
		data = query1.FeatureData();
		TESTDIAGNOSTICERROR(data==KDefaultData1, 	
				_L("TFeatureEntry::FeatureData expects KDefaultData1 for KNewUid3; returned value is = %d"),data);
			
		//
		// Delete added feature to clean up
		//
		err = control.DeleteFeature(KNewUid1);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::DeleteFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
		// Check that feature has been really deleted
		err = control.FeatureSupported(query1);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid1; returned value is = %d"),err);
		err = control.DeleteFeature(KNewUid3);
		TESTDIAGNOSTICERROR(err==KErrNone, 
				_L("RFeatureControl::DeleteFeature expects KErrNone for KNewUid3; returned value is = %d"),err);
		// Check that feature has been really deleted
		err = control.FeatureSupported(query3);
		TESTDIAGNOSTICERROR(err==KErrNotFound, 
				_L("RFeatureControl::FeatureSupported expects KErrNotFound for KNewUid3; returned value is = %d"),err);
		
		// Resetting
		query1 = TFeatureEntry(KNewUid1);
		query3 = TFeatureEntry(KNewUid3);
		control.Close();
		}
	process.Close();
	// Wait before starting the next test
	User::After(100000);
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// SCENARIO 18: Successful installation and features manipulation (Enable/Disable without modifying data)
	// Add Feature with user data -> P&S install -> SWIStart -> Enable/Disable Feature -> 
	//  -> SWIEnd -> P&S success -> No change to user data
	////////////////////////////////////////////////////////////////////////////////////////////
    INFO_PRINTF1(_L("SCENARIO 18: Successful installation and Enable or Disable feature should not modify userdata"));
    // Launching dummyswi.exe
    err = process.Create(KDummySWIPath, _L("1"));   // 1 = install success
    TESTDIAGNOSTICERROR(err==KErrNone,
               _L("RProcess::Create expects KErrNone, returned value is = %d"),err);
    if( err==KErrNone )
        {
        process.Resume();
    
        // Allow P&S to complete initialising
        User::After(200000);
        
        err = control.Open();
        TESTDIAGNOSTICERROR(err==KErrNone, _L("RFeatureControl::Open failed, returned value is = %d"),err);
        
        err = control.AddFeature(entry1);
        TESTDIAGNOSTICERROR(err==KErrNone, _L("RFeatureControl::AddFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
        
        err = control.SWIStart();
        TESTDIAGNOSTICERROR(err==KErrNone, _L("RFeatureControl::SWIStart expects KErrNone; returned value is = %d"),err);
        
        // Enable feature
        err = control.EnableFeature(KNewUid1);
        TESTDIAGNOSTICERROR(err==KErrNone, _L("RFeatureControl::EnableFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
        
        // Disable feature
        err = control.DisableFeature(KNewUid1);
        TESTDIAGNOSTICERROR(err==KErrNone, _L("RFeatureControl::EnableFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
        
        err = control.SWIEnd();
        TESTDIAGNOSTICERROR(err==KErrNone, _L("RFeatureControl::SWIEnd expects KErrNone; returned value is = %d"),err);
        
        // Delay until the P&S property is set to ESASwisStatusSuccess in dummyswi.exe to indicate successful installation
        User::After(1500000);
        
        err = control.FeatureSupported(query1);
        TESTDIAGNOSTICERROR(err==KErrNone, _L("TFeatureEntry::FeatureData expects 0 for KNewUid1; returned value is = %d"),err);
        TESTDIAGNOSTICERROR(query1.FeatureData()==KDefaultData1, _L("TFeatureEntry::FeatureData expects non-zero for KNewUid1; returned value is = %d"), err);
        
        // Delete KNewUid1
        err = control.DeleteFeature(KNewUid1);
        TESTDIAGNOSTICERROR(err==KErrNone, _L("RFeatureControl::DeleteFeature expects KErrNone for KNewUid1; returned value is = %d"),err);
        
        query1 = TFeatureEntry(KNewUid1);
        
        control.Close();

        }
    process.Close();
    // Wait before starting the next test
    User::After(100000);
	

	return TestStepResult();
	}

