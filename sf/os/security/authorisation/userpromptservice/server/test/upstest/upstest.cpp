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
#include <ups/upserr.h>
#include "f32file.h"

using namespace UserPromptService;

/** Top-level test object renders stages and confirms conditions. */
static RTestWrapper test(_L("UPSTEST"));

/**
	This session handle is defined at the file level so each individual test
	does not have to connect to the server.
 */
static RUpsSession sTestSession;
/**
	This subsession handle is defined at the file level so each individual test
	does not have to connect to the server and create a subssesion.
 */
static RUpsSubsession sTestSubsession;

// -------- open / close session --------

static void TestOpenCloseSession()
/**
	Open and close a connection to the UPS server.
 */
	{
	test.Start(_L("TestOpenCloseSession"));
	
	RUpsSession s;
	TInt r = s.Connect();
	test(r == KErrNone);

	r = s.ShareAuto();
	test(r == KErrNone);
	
	s.Close();
	
	test.End();
	}

// -------- open / close subsession --------

static void TestOpenCloseSubsession()
/**
	Open and close a subsession on the UPS server.
 */
	{
	test.Start(_L("TestOpenCloseSubsession"));
	
	RUpsSubsession ss;
	RThread thd;
	TInt r = ss.Initialise(sTestSession, thd);
	test(r == KErrNone);
	ss.Close();
	
	test.End();
	}

// -------- Authorise succeed, cancel --------
_LIT8(KOpaqueData, "<ce>fred</ce>");
static void TestAuthoriseL()
/**
	Launch an asynchronous Authorise request on a UPS subsession.
	Let it complete normally, and also cancel it.
 */
	{
	test.Start(_L("TestAuthorise"));
	
	TServiceId serviceId = {43};
	TRequestStatus rs;

	
	test.Next(_L("complete normally - no opaque data\n"));
	TUpsDecision dec = EUpsDecYes;
	sTestSubsession.Authorise(EFalse, serviceId, KNullDesC, dec, rs);
	test(dec == EUpsDecYes);	// not changed yet
	User::WaitForRequest(rs);
	test(rs == KErrNone);
	test(dec == EUpsDecNo);
	
	test.Next(_L("cancel - empty opaque data\n"));
	dec = EUpsDecYes;	// ensure changed
	sTestSubsession.Authorise(EFalse, serviceId, KNullDesC, KNullDesC8, dec, rs);
	test(dec == EUpsDecYes);	// not changed yet
	test.Printf(_L("About to cancel - current status is %d"), rs.Int());
	sTestSubsession.CancelPrompt();
	User::WaitForRequest(rs);
	test(rs == KErrCancel);
	test(dec == EUpsDecYes);	// not changed

	test.Next(_L("Opaque data\n"));
	dec = EUpsDecYes;	// ensure changed
	sTestSubsession.Authorise(EFalse, serviceId, _L("opaque data test"), KOpaqueData, dec, rs);
	test(dec == EUpsDecYes);	// not changed yet
	User::WaitForRequest(rs);
	test(rs == KErrNone);
	test(dec == EUpsDecNo);

	
	test.Next(_L("cancel when no outstanding - harmless\n"));
	sTestSubsession.CancelPrompt();

	test.Next(_L("cancel/close when sub session never created - harmless\n"));
	RUpsSubsession uninitialisedCancel;
	uninitialisedCancel.CancelPrompt();
	uninitialisedCancel.Close();
	
	test.End();
	}

static void TestFlurryL()
/**
	Launch multiple requests
 */
	{
	test.Start(_L("TestFlurry"));

	RThread thd;
	RUpsSubsession testSubsession1;
	testSubsession1.Initialise(sTestSession, thd);
	RUpsSubsession testSubsession2;
	testSubsession2.Initialise(sTestSession, thd);
	RUpsSubsession testSubsession3;
	testSubsession3.Initialise(sTestSession, thd);
	RUpsSubsession testSubsession4;
	testSubsession4.Initialise(sTestSession, thd);
	RUpsSubsession testSubsession5;
	testSubsession5.Initialise(sTestSession, thd);
	RUpsSubsession testSubsession6;
	testSubsession6.Initialise(sTestSession, thd);
	
	TServiceId serviceId = {43};
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
#if 0
	// Will change immediately in non-interactive testing
	test(dec1 == EUpsDecYes);	// not changed yet
	test(dec2 == EUpsDecYes);	// not changed yet
	test(dec3 == EUpsDecYes);	// not changed yet
	test(dec4 == EUpsDecYes);	// not changed yet
	test(dec5 == EUpsDecYes);	// not changed yet
	test(dec6 == EUpsDecYes);	// not changed yet
#endif
	User::After(1000);

	User::WaitForRequest(rs1);

	User::WaitForRequest(rs2);
	User::WaitForRequest(rs4);

	User::WaitForRequest(rs3);
	User::WaitForRequest(rs5);
	User::WaitForRequest(rs6);
	test(rs1 == KErrNone);
	test(rs2 == KErrNone);
	test(rs3 == KErrNone);
	test(rs4 == KErrNone);
	test(rs5 == KErrNone);
	test(rs6 == KErrNone);
	test(dec1 == EUpsDecNo);
	test(dec2 == EUpsDecNo);
	test(dec3 == EUpsDecNo);
	test(dec4 == EUpsDecNo);
	test(dec5 == EUpsDecNo);
	test(dec6 == EUpsDecNo);
	
	test.End();
	}

// -------- RUpsSubsession --------
_LIT(KSayYes,"SayYes");
static void TestRUpsSubsession()
/**
	Attempt query with server checks passed and without
 */
	{
	//
	// Tests for RUpsSubsession
	//
	RThread thd;

	test.Start(_L("Testing RUpsSubsession"));
	RUpsSubsession clientSubsession;
	TInt r = clientSubsession.Initialise(sTestSession, thd);
	test(r == KErrNone);

	test.Next(_L("Query with server checks passed"));
	TServiceId serviceId = {42};
	TUpsDecision dec = EUpsDecNo;
	TRequestStatus rs;
	// Query saying our checks were ok, expect to get decision set to EUpsDecYes
	clientSubsession.Authorise(ETrue, serviceId, _L("Destination"), _L8("Opaque data"), dec, rs);
	User::WaitForRequest(rs);
	test(rs == KErrNone);
	test(dec == EUpsDecYes);



	test.Next(_L("Try closing client subsession before it is really created"));
	clientSubsession.Close();

	test.Next(_L("Re-\"create\" client subsession"));
	r = clientSubsession.Initialise(sTestSession, thd);
	test(r == KErrNone);

	test.Next(_L("Query with server checks failed, ie query UPS, expect fail"));
	// Query saying our checks failed, should talk to UPS and change decision to EUpsDecNo
	dec = EUpsDecYes;
	clientSubsession.Authorise(EFalse, serviceId, KNullDesC, dec, rs);
	User::WaitForRequest(rs);
	test(rs == KErrNone);
	test(dec == EUpsDecNo);


	test.Next(_L("Query with server checks failed, ie query UPS, special destination, expect yes"));
	// Query saying our checks failed, should talk to UPS and change decision to EUpsDecNo
	dec = EUpsDecYes;
	clientSubsession.Authorise(EFalse, serviceId, KSayYes(), dec, rs);
	User::WaitForRequest(rs);
	test(rs == KErrNone);
	test(dec == EUpsDecYes);


	clientSubsession.Close();
	test.End();
	}

TInt ThreadFunction(TAny *)
	{
	return KErrNone;
	}

static void TestRUpsSubsessionDeathL()
	{
	RThread thd;
	TRequestStatus thdStatus;
	User::LeaveIfError(thd.Create(_L("MyThread"), ThreadFunction, 4096, 4096, 4096, 0, EOwnerThread));
//	thd.SetHandle(666);
	thd.Rendezvous(thdStatus);
	thd.Kill(KErrAbort);
	User::WaitForRequest(thdStatus);

	test.Start(_L("Testing RUpsSubsession"));
	RUpsSubsession clientSubsession;
	TInt r = clientSubsession.Initialise(sTestSession, thd);
	test(r == KErrNone);

	test.Next(_L("Query with dead thread id"));
	TServiceId serviceId = {43};
	TUpsDecision dec = EUpsDecYes;
	TRequestStatus rs;
	thd.Close();
	clientSubsession.Authorise(EFalse, serviceId, _L("req1"), dec, rs);
	User::WaitForRequest(rs);
	test(rs == KErrNone);
	test(dec == EUpsDecNo);

	
	clientSubsession.Close();
	test.End();
	}

static void TestRUpsManagementL()
/**
	Attempt to delete database
 */
	{
	test.Start(_L("Testing RUpsManagement"));
	RThread thd;
	TRequestStatus rs;

	// Create filter
	TServiceId serviceId = {43};
	CDecisionFilter *filter = CDecisionFilter::NewLC();
	filter->SetClientSid(thd.SecureId(), EEqual);
	filter->SetServerSid(thd.SecureId(), EEqual);
	filter->SetServiceId(serviceId, EEqual);

	RUpsSubsession clientSubsession;
	TInt r = clientSubsession.Initialise(sTestSession, thd);
	test(r == KErrNone);

	test.Next(_L("Open management session"));
	RUpsManagement mngmnt;
	r = mngmnt.Connect();
	test(r == KErrNone);
	User::LeaveIfError(r);

	test.Next(_L("View create - then delete DB"));
	mngmnt.CreateView(*filter, rs);

	test.Next(_L("Delete database"));
	TRAP(r, mngmnt.DeleteDatabaseL());
	test(r == KErrNone);

	test.Next(_L("Now see what view create completed with...."));
	User::WaitForRequest(rs);
	test(rs.Int() == KErrAbort);

	test.Next(_L("Add entry to new database"));
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


	test.Next(_L("View create - immediate cancel"));
	mngmnt.CreateView(*filter, rs);
	mngmnt.CancelAndCloseView();
	User::WaitForRequest(rs);
	test(rs.Int() == KErrCancel);

	mngmnt.UpdateDecision(TUint32(-23), ETrue, rs);
	mngmnt.CancelUpdateDecision();
	User::WaitForRequest(rs);
	test(rs.Int() == KErrCancel);

	mngmnt.UpdateDecision(TUint32(-23), ETrue, rs);
	User::WaitForRequest(rs);
	test(rs.Int() == KErrNotFound);

	test.Next(_L("View create - when busy"));
	TRequestStatus rs2;
	mngmnt.CreateView(*filter, rs);
	mngmnt.CreateView(*filter, rs2);
	User::WaitForRequest(rs2);
	test(rs2.Int() == KErrServerBusy);
	User::WaitForRequest(rs);
	test(rs.Int() == KErrNone);
	mngmnt.CancelAndCloseView();

	test.Next(_L("View create - iterate through it"));
	mngmnt.CreateView(*filter, rs);

	User::WaitForRequest(rs);
	test(rs.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(filter);

	CDecisionRecord *record = 0;
	r = KErrNone;
	TInt recordCount = 0;
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
			++recordCount;
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
	test(recordCount == 2);
	
	TRAP(r, record = mngmnt.NextMatchL());
	test((r == KErrNone) && (record == 0));

	mngmnt.CancelAndCloseView();

	test.Next(_L("Close management session and clientSubsession"));
	mngmnt.Close();
	clientSubsession.Close();

	test.End();
	}

void TestSwiObserverSecurityL()
{
	test.Start(_L("Testing swi observer functions do not work from here..."));

	TInt r;

	RUpsManagement session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);

	TUid ourSid;
	ourSid.iUid = 0x10283559;
	TRAP(r, session.DeleteDecisionsForExeL(ourSid));
	test(r == KErrPermissionDenied);

	TRAP(r, session.NotifyPluginsMayHaveChangedL());
	test(r == KErrPermissionDenied);

	TRequestStatus rs;
	session.NotifyPolicyFilesChanged(rs);
	User::WaitForRequest(rs);

	test(rs.Int() == KErrPermissionDenied);

	session.CancelNotifyPolicyFilesChanged();

	CleanupStack::PopAndDestroy(&session);
	
	test.End();
}

// -------- entrypoint --------


void MainL()
	{
	test.Title(_L("c:\\upstest.log"));
	test.Start(_L(" @SYMTestCaseID:SEC-UPS-0001 Testing RUpsSubsession "));

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

	//RThread ourThread;
	//ourThread.SetPriority(EPriorityMore);
	//ourThread.Close();
//	User::SetProcessCritical(User::ESystemCritical);
//	User::SetCritical(User::ESystemCritical);
	TestOpenCloseSession();


	TInt r = sTestSession.Connect();
	test(r == KErrNone);
	User::LeaveIfError(r);

	TestRUpsSubsessionDeathL();

	TestOpenCloseSubsession();

	TestSwiObserverSecurityL();
	
	RThread thd;
	r = sTestSubsession.Initialise(sTestSession, thd);
	test(r == KErrNone);
	User::LeaveIfError(r);
	
	TestFlurryL();

	TestAuthoriseL();
	
	sTestSubsession.Close();

	TestRUpsSubsession();

	TestRUpsManagementL();

	sTestSession.ShutdownServer();

	// Close top level session (low level session was closed by
	// ShutdownServer, but we still need to do the RUpsSession
	// cleanup).
	sTestSession.Close();

	(void) fs.Delete(notifierConfig);
	CleanupStack::PopAndDestroy(&fs);
	
	test.End();
	test.Close();
}

void PanicIfError(TInt r)
	{
	if(r != KErrNone)
		{
		User::Panic(_L("upstest failed: "), r);
		}
	}


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
	if(err != KErrNone)
		{
		User::Panic(_L("upstest failed: "), err);
		}
	delete tc;
	
	//__KHEAP_MARKEND;
	__UHEAP_MARKEND;
	
	
	return KErrNone;
	}

