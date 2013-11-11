/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Test program exercises the skeleton UPS server API.
* See individual test functions for more information.
*
*/


/**
 @file
*/

#include <e32ldr.h>
#include <e32ldr_private.h>
#include "rtestwrapper.h"

#include <ups/upsclient.h>
#include "f32file.h"

using namespace UserPromptService;

/** Top-level test object renders stages and confirms conditions. */
static RTestWrapper test(_L("UPSTESTOOM"));


static void TestFlurryL()
/**
	Launch multiple requests
 */
	{
	RUpsSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);

	RThread thd;

#if 0
	RUpsSubsession testSubsession1;
	testSubsession1.Initialise(session, thd);
	
	TServiceId serviceId = {42};
	TRequestStatus rs1;
	
	// complete normally - no opaque data
	TUpsDecision dec1 = EUpsDecYes;
	testSubsession1.Authorise(EFalse, serviceId, _L("req1"), dec1, rs1);

	User::After(1000);

	User::WaitForRequest(rs1);
	User::LeaveIfError(rs1.Int());
	test(dec1 == EUpsDecNo);
#else
	RUpsSubsession testSubsession1;
	testSubsession1.Initialise(session, thd);
	RUpsSubsession testSubsession2;
	testSubsession2.Initialise(session, thd);
	RUpsSubsession testSubsession3;
	testSubsession3.Initialise(session, thd);
	RUpsSubsession testSubsession4;
	testSubsession4.Initialise(session, thd);
	RUpsSubsession testSubsession5;
	testSubsession5.Initialise(session, thd);
	RUpsSubsession testSubsession6;
	testSubsession6.Initialise(session, thd);
	
	TServiceId serviceId = {42};
	TRequestStatus rs1;
	TRequestStatus rs2;
	TRequestStatus rs3;
	TRequestStatus rs4;
	TRequestStatus rs5;
	TRequestStatus rs6;
	
	// complete normally - no opaque data
	TUpsDecision dec1 = EUpsDecYes;
	TUpsDecision dec2 = EUpsDecYes;
	TUpsDecision dec3 = EUpsDecYes;
	TUpsDecision dec4 = EUpsDecYes;
	TUpsDecision dec5 = EUpsDecYes;
	TUpsDecision dec6 = EUpsDecYes;
	testSubsession1.Authorise(EFalse, serviceId, _L("req1"), dec1, rs1);
	testSubsession2.Authorise(EFalse, serviceId, _L("reqX"), dec2, rs2);
	testSubsession3.Authorise(EFalse, serviceId, _L("req2"), dec3, rs3);
	testSubsession4.Authorise(EFalse, serviceId, _L("reqX"), dec4, rs4);
	testSubsession5.Authorise(EFalse, serviceId, _L("req3"), dec5, rs5);
	testSubsession6.Authorise(EFalse, serviceId, _L("req4"), dec6, rs6);

	User::After(1000);

	User::WaitForRequest(rs1);

	User::WaitForRequest(rs2);
	User::WaitForRequest(rs4);

	User::WaitForRequest(rs3);
	User::WaitForRequest(rs5);
	User::WaitForRequest(rs6);
	User::LeaveIfError(rs1.Int());
	User::LeaveIfError(rs2.Int());
	User::LeaveIfError(rs3.Int());
	User::LeaveIfError(rs4.Int());
	User::LeaveIfError(rs5.Int());
	User::LeaveIfError(rs6.Int());
	test(dec1 == EUpsDecNo);
	test(dec2 == EUpsDecNo);
	test(dec3 == EUpsDecNo);
	test(dec4 == EUpsDecNo);
	test(dec5 == EUpsDecNo);
	test(dec6 == EUpsDecNo);
#endif
	
	CleanupStack::PopAndDestroy(&session);
	}

static void TestRUpsManagementL()
/**
	Attempt to delete database
 */
	{
//	test.Start(_L("Testing RUpsManagement"));
	RThread thd;
	TRequestStatus rs;
	TInt r;

	// Create filter
	TServiceId serviceId = {43};
	CDecisionFilter *filter = CDecisionFilter::NewLC();
	filter->SetClientSid(thd.SecureId(), EEqual);
	filter->SetServerSid(thd.SecureId(), EEqual);
	filter->SetServiceId(serviceId, EEqual);

	RUpsSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);

	RUpsSubsession clientSubsession;
	User::LeaveIfError(clientSubsession.Initialise(session, thd));
	CleanupClosePushL(clientSubsession);

	test.Printf(_L("Open management session\n"));
	RUpsManagement mngmnt;
	User::LeaveIfError(mngmnt.Connect());
	CleanupClosePushL(mngmnt);
	
	test.Printf(_L("View create - then delete DB\n"));
	mngmnt.CreateView(*filter, rs);

	test.Printf(_L("Delete database\n"));
	TRAP(r, mngmnt.DeleteDatabaseL());
	test((r == KErrNone) || (r == KErrNoMemory));

	test.Printf(_L("Now see what view create completed with....\n"));
	User::WaitForRequest(rs);
	// The createview may actually complete if the DeleteDatabase fails due to OOM
//	test((rs.Int() == KErrAbort) || (rs.Int() == KErrNoMemory) || (rs.Int() == KErrNone));
	test((rs.Int() == KErrAbort) || (rs.Int() == KErrNoMemory));

	test.Printf(_L("Add entry to new database\n"));
	TUpsDecision dec = EUpsDecYes;
	clientSubsession.Authorise(EFalse, serviceId, _L("DB delete 1"), _L8("Opaque data"), dec, rs);
	User::WaitForRequest(rs);
	test(rs == KErrNone);
	test(dec == EUpsDecNo);

	dec = EUpsDecYes;
	clientSubsession.Authorise(EFalse, serviceId, _L("DB delete 2"), _L8("Opaque data"), dec, rs);
	User::WaitForRequest(rs);
	test(rs == KErrNone);
	test(dec == EUpsDecNo);


	test.Printf(_L("View create - immediate cancel\n"));
	mngmnt.CreateView(*filter, rs);
	mngmnt.CancelAndCloseView();
	User::WaitForRequest(rs);
	test((rs.Int() == KErrCancel) || (rs.Int() == KErrNoMemory));

	test.Printf(_L("View create - iterate through it\n"));
	mngmnt.CreateView(*filter, rs);

	User::WaitForRequest(rs);
	test((rs.Int() == KErrNone) || (rs.Int() == KErrNoMemory));
	

	CDecisionRecord *record = 0;
	r = KErrNone;
	while(r == KErrNone)
		{
		TRAP(r, record = mngmnt.NextMatchL());
		if(record == 0)
			{
			break;
			}
		test(r == KErrNone);
    	if(r == KErrNone)
    		{
    		CleanupStack::PushL(record);
			CDecisionFilter *exactFilter = CDecisionFilter::NewLC(record->iClientSid,
																  record->iEvaluatorId,
																  record->iServiceId,
																  record->iServerSid,
																  record->iFingerprint,
																  record->iClientEntity,
																  record->iMajorPolicyVersion);
				
			mngmnt.UpdateDecision(record->iRecordId, ETrue, rs);
			User::WaitForRequest(rs);
			test(rs.Int() == KErrNone);
			TRAP(r, mngmnt.RemoveDecisionsL(*exactFilter));
			test(r == KErrNone);

			CleanupStack::PopAndDestroy(exactFilter);
			CleanupStack::PopAndDestroy(record);
    		}
		
		};

	
	TRAP(r, record = mngmnt.NextMatchL());
	test((r == KErrNone) && (record == 0));

	mngmnt.CancelAndCloseView();

	test.Printf(_L("Close management session and clientSubsession\n"));
	CleanupStack::PopAndDestroy(&mngmnt);
	CleanupStack::PopAndDestroy(&clientSubsession);
	CleanupStack::PopAndDestroy(&session);
	CleanupStack::PopAndDestroy(filter);

//	test.End();
	}

void MainL()
	{
	test.Title(_L("c:\\upstestoom.log"));
	test.Start(_L(" @SYMTestCaseID:SEC-UPS-OOM-0001 Testing UPS OOM "));

	// We need to increase the priority of the thread running the test code to make sure
	// that asynchronous ups management calls, for example CreateView() , won't finish
	// before following synchronous cancellation or termination calls, for example:
	// DeleteDatabaseL() or CancelAndCloseView().
	RThread thread;
	TThreadPriority currentPri = thread.Priority();
	currentPri = (TThreadPriority)((TInt)currentPri+10);
	thread.SetPriority(currentPri);

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TBuf<21> notifierConfig(_L("!:\\upsrefnotifier.txt"));
	notifierConfig[0] = fs.GetSystemDriveChar();

	TBuf<35> database(_L("!:\\Private\\10283558\\database\\ups.db"));
	database[0] = fs.GetSystemDriveChar();

	TInt lineLength = User::CommandLineLength();
	switch(lineLength)
		{
		default:
			// fall through - extra command line arguments are ignored
		case 2:
			(void) fs.Delete(database);
			// Fall through to also delete notifier config file
		case 1:
			(void) fs.Delete(notifierConfig);
			break;
		case 0:
			{
			// No args so run in silent mode
			(void) fs.Delete(database);
			(void) fs.Delete(notifierConfig);
			RFile file;
			User::LeaveIfError(file.Create(fs, notifierConfig, EFileShareExclusive | EFileWrite));
			User::LeaveIfError(file.Write(_L8("Never")));
			file.Close();
			break;
			}
		}

	// Connect to server
	RUpsSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);

	TInt err = KErrNone;
	TInt err2 = KErrNone;
	TInt run = 0;
	TInt passingRuns = 0;
	static const TInt maxRun = 1000;
	static const TInt passThreshold = 5;
	for(run=1; run<=maxRun; ++run)
		{
		test.Printf(_L("\n\nOOM -- Run %d\n"), run);
		err = session.SetServerHeapFail(run);
		if(err == KErrNoMemory)
			{
			// Reinitialisation failed
			test.Printf(_L("\tReinitialisation failed\n"));
			session.ResetServerHeapFail();
			continue;
			}
		if(err != KErrNone)
			{
			// Failed to set heap fail, maybe the previous loop crashed the server??
			test.Printf(_L("Failed to set heap fail with error code %d\n"), err);
			test(EFalse);
			break;
			}

		// Run the test
		//test.Start(_L("TestFlurry"));
		test.Printf(_L("TestFlurry\n"));
		TRAP(err, TestFlurryL());
		test.Printf(_L("TestRUpsManagementL\n"));
		TRAP(err2, TestRUpsManagementL());
		if(err == KErrNone)
			{
			err = err2;
			}
		test.Printf(_L("done\n"));
		//test.End();
		// Clear the heap fail
		test.Printf(_L("Reseting heap failure\n"));
		err2 = session.ResetServerHeapFail();
		if((err == KErrServerTerminated) || (err2 == KErrServerTerminated))
			{
			test.Printf(_L("\tUPS server died\n"));
			test(EFalse);
			break;
			}
		if((err == KErrNone) && (err2 != KErrNone))
			{
			err = err2;
			}
		
		// Did it work?
		if(err == KErrNone)
			{
			++passingRuns;
			}
		else
			{
			passingRuns = 0;
			}
		
		if(passingRuns > passThreshold) break;
		} // End of OOM loop

	if(run > maxRun)
		{
		User::Leave(err);
		}

	// Synchronous call to shutdown the server
	session.ShutdownServer();
	// Close top level session (low level session was closed by
	// ShutdownServer, but we still need to do the RUpsSession
	// cleanup).
	CleanupStack::PopAndDestroy(&session);

	(void) fs.Delete(notifierConfig);
	CleanupStack::PopAndDestroy(&fs);
	
	test.End();
	test.Close();
}

void PanicIfError(TInt r)
	{
	if(r != KErrNone)
		{
		User::Panic(_L("upstestoom failed: "), r);
		}
	}


// -------- entrypoint --------
TInt E32Main()
/**
	Executable entrypoint establishes connection with UPS server
	and then invokes tests for each functional area.
	
	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
 */
	{
	// disable lazy DLL unloading so kernel heap balances at end
	RLoader l;
	PanicIfError(l.Connect());
	PanicIfError(l.CancelLazyDllUnload());
	l.Close();
	
	__UHEAP_MARK;
	//__KHEAP_MARK;
	
	// allocating a cleanup stack also installs it
	CTrapCleanup* tc = CTrapCleanup::New();
	if (tc == 0)
		return KErrNoMemory;

	TRAPD(err, MainL());
	PanicIfError(err);
	delete tc;
	
	//__KHEAP_MARKEND;
	__UHEAP_MARKEND;
	
	
	return KErrNone;
	}

// End of file
