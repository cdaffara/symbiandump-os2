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
* Test program exercises the swi observer UPS API.
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
static RTestWrapper test(_L("UPSTESTOBSIF"));

static RUpsManagement sMngmntSession;

void PopulateDatabaseL()
{
	test.Start(_L("Populate database"));
	RUpsSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);

	RThread thd;
	RUpsSubsession clientSubsession;
	TInt r = clientSubsession.Initialise(session, thd);
	test(r == KErrNone);
	CleanupClosePushL(clientSubsession);

	RBuf destination;
	destination.CreateL(100);
	CleanupClosePushL(destination);

	for(TInt i=0 ; i<100; ++i)
		{
		TServiceId serviceId = {42};
		if( i & 1) serviceId.iUid = 43;
		destination.Zero();
		destination.AppendFormat(_L("destination %x"), i);
		
		TUpsDecision dec = EUpsDecNo;
		TRequestStatus rs;
		clientSubsession.Authorise(EFalse, serviceId, destination, _L8("Opaque data"), dec, rs);
		User::WaitForRequest(rs);
		test(rs == KErrNone);
		if(serviceId.iUid == 42)
			{
			test(dec == EUpsDecYes);
			}
		else
			{
			test(dec == EUpsDecNo);
			}

		}

	CleanupStack::PopAndDestroy(&destination);
	CleanupStack::PopAndDestroy(&clientSubsession);
	CleanupStack::PopAndDestroy(&session);

	test.End();
}
	
#if 1
class CTestSwiIf;
NONSHARABLE_CLASS(CRequest) : public CActive
	{
public:
	static CRequest *NewL(RUpsSession &aSession, CTestSwiIf &aParent, TInt aId, TUpsDecision aExpected);
	~CRequest();
private:
	CRequest(CTestSwiIf &aParent, TUpsDecision aExpected);
	void ConstructL(RUpsSession &aSession, TInt aId);

	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

	CTestSwiIf &iParent;

	RUpsSubsession iSubSession;

	TUpsDecision iDec;
	TUpsDecision iExpected;
	};


NONSHARABLE_CLASS(CTestSwiIf) : public CActive
	{
public:
	static CTestSwiIf *NewL();

	~CTestSwiIf();

	void IncUsers();
	void DecUsers();
private:
	CTestSwiIf();
	void ConstructL();

	enum EState
		{
		EPrePolicyChange,
		EPostPolicyChange,
		ERevertPolicyChange,
		ETestingComplete
		};
	
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

	TInt iUsers;

	RFs iFs;
	EState iState;
	RUpsSession iUpsSession;
	RUpsManagement iManagementSession;
	};

CTestSwiIf *CTestSwiIf::NewL()
	{
	CTestSwiIf *self = new(ELeave) CTestSwiIf;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTestSwiIf::CTestSwiIf()
	:	CActive(CActive::EPriorityStandard), iState(EPrePolicyChange)
	{
	CActiveScheduler::Add(this);
	}

_LIT(KCheckIfFailed, "z:\\private\\10283558\\policies\\ups_102836c3_0000002b_checkiffailed.rsc");
_LIT(KResourceFileDirC, "c:\\private\\10283558\\policies\\");
_LIT(KResourceFileOnC, "c:\\private\\10283558\\policies\\ups_102836c3_0000002b.rsc");

void CTestSwiIf::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());

	// Make sure the C: policy is deleted.
	(void)iFs.Delete(KResourceFileOnC);

	// Make sure server is not holding cached values for the C: policy
	sMngmntSession.ShutdownServer();
	sMngmntSession.Close();
	User::LeaveIfError(sMngmntSession.Connect());

	User::LeaveIfError(iUpsSession.Connect());
	User::LeaveIfError(iManagementSession.Connect());


	TRequestStatus *rs = &iStatus;
	*rs = KRequestPending;
	User::RequestComplete(rs, KErrNone);
	SetActive();
	}


CTestSwiIf::~CTestSwiIf()
	{
	Cancel();
	iManagementSession.Close();
	iUpsSession.Close();
	iFs.Close();
	}

void CTestSwiIf::IncUsers()
	{
	++iUsers;
	}

void CTestSwiIf::DecUsers()
	{
	--iUsers;
	if((iUsers <= 0) && (iState == ETestingComplete))
		{
		CActiveScheduler::Stop();
		}
	}


void CTestSwiIf::RunL()
	{
	test(iStatus.Int() == KErrNone);
	switch(iState)
		{
		case EPrePolicyChange:
			{
			PopulateDatabaseL();

			(void)CRequest::NewL(iUpsSession, *this, 42, EUpsDecYes);
			(void)CRequest::NewL(iUpsSession, *this, 18, EUpsDecYes);
			(void)CRequest::NewL(iUpsSession, *this, 75, EUpsDecNo);
			(void)CRequest::NewL(iUpsSession, *this, 20, EUpsDecYes);
			(void)CRequest::NewL(iUpsSession, *this, 15, EUpsDecNo);

			(void)iFs.MkDirAll(KResourceFileDirC);

			CFileMan *fileman = CFileMan::NewL(iFs);
			CleanupStack::PushL(fileman);
			TInt r = fileman->Copy(KCheckIfFailed, KResourceFileOnC);
			User::LeaveIfError(r);
			CleanupStack::PopAndDestroy(fileman);

			TRequestStatus rs;
			iManagementSession.NotifyPolicyFilesChanged(rs);
			iManagementSession.CancelNotifyPolicyFilesChanged();
			User::WaitForRequest(rs);
			if(rs.Int() != KErrCancel) User::Leave(rs.Int());

			iState = EPostPolicyChange;
			iManagementSession.NotifyPolicyFilesChanged(iStatus);
			SetActive();
			break;
			}
		case EPostPolicyChange:
			// Notify complete, do some more queries
			(void)CRequest::NewL(iUpsSession, *this, 42, EUpsDecYes);
			(void)CRequest::NewL(iUpsSession, *this, 18, EUpsDecYes);
			(void)CRequest::NewL(iUpsSession, *this, 75, EUpsDecYes);
			(void)CRequest::NewL(iUpsSession, *this, 20, EUpsDecYes);
			(void)CRequest::NewL(iUpsSession, *this, 15, EUpsDecYes);

			// Revert change
			User::LeaveIfError(iFs.Delete(KResourceFileOnC));

			iState = ERevertPolicyChange;
			iManagementSession.NotifyPolicyFilesChanged(iStatus);
			SetActive();
			break;

		case ERevertPolicyChange:
			iState = ETestingComplete;
			if(iUsers <= 0)
				{
				CActiveScheduler::Stop();
				}
			break;
			
		case ETestingComplete:
			break;
		}
	}

void CTestSwiIf::DoCancel()
	{
	switch(iState)
		{
		case EPrePolicyChange:
			{
			TRequestStatus *rs = &iStatus;
			if(*rs == KRequestPending)
				{
				User::RequestComplete(rs, KErrCancel);
				}
			break;
			}

		case EPostPolicyChange:
			iManagementSession.CancelNotifyPolicyFilesChanged();
			break;

		case ERevertPolicyChange:
			iManagementSession.CancelNotifyPolicyFilesChanged();
			break;

		case ETestingComplete:
			break;
		}

	}

TInt CTestSwiIf::RunError(TInt aError)
	{
	User::Panic(_L("CTestSwiIf::RunError"), aError);
	/*lint -unreachable */
	return KErrNone;
	}

CRequest *CRequest::NewL(RUpsSession &aSession, CTestSwiIf &aParent, TInt aId, TUpsDecision aExpected)
	{
	CRequest *self = new(ELeave) CRequest(aParent, aExpected);
	CleanupStack::PushL(self);
	self->ConstructL(aSession, aId);
	CleanupStack::Pop(self);
	return self;
	}

CRequest::CRequest(CTestSwiIf &aParent, TUpsDecision aExpected)
	:	CActive(CActive::EPriorityStandard-1),
		iParent(aParent),
		iExpected(aExpected)
	{
	CActiveScheduler::Add(this);
	iParent.IncUsers();
	}

void CRequest::ConstructL(RUpsSession &aSession, TInt aId)
	{
	RThread thd;
	User::LeaveIfError(iSubSession.Initialise(aSession, thd));
	TServiceId serviceId = {42};
	if( aId & 1) serviceId.iUid = 43;
	RBuf destination;
	destination.CreateL(100);
	CleanupClosePushL(destination);
	destination.AppendFormat(_L("destination %x"), aId);
		
	iDec = EUpsDecNo;
	iSubSession.Authorise(EFalse, serviceId, destination, _L8("Opaque data"), iDec, iStatus);
	SetActive();

	CleanupStack::PopAndDestroy(&destination);
	}

CRequest::~CRequest()
	{
	iSubSession.Close();
	iParent.DecUsers();
	}

void CRequest::RunL()
	{
	test(iStatus.Int() == KErrNone);
	test(iDec == iExpected);
	delete this;
	}

void CRequest::DoCancel()
	{
	}

TInt CRequest::RunError(TInt aError)
	{
	User::Panic(_L("CRequest::RunError"), aError);
	/*lint -unreachable */
	return KErrNone;
	}


#endif
void TestSwiObserverL()
{
	test.Start(_L("Testing swi observer functions work from here..."));

	RUpsManagement session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);

	session.NotifyPluginsMayHaveChangedL();

	TRequestStatus rs;
	session.NotifyPolicyFilesChanged(rs);
	User::WaitForRequest(rs);

	test(rs.Int() == KErrNone);

	session.CancelNotifyPolicyFilesChanged();

	TSecureId ourSid(0x10283559);
	session.DeleteDecisionsForExeL(ourSid);


	CleanupStack::PopAndDestroy(&session);
	
	test.End();
}

// -------- entrypoint --------


void MainL()
	{
	CActiveScheduler *scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);
	
	test.Title(_L("c:\\upstestobsif.log"));
	test.Start(_L(" @SYMTestCaseID:SEC-UPS-OBSIF-0001 Testing RUpsSession SWI observer IF "));

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	User::LeaveIfError(sMngmntSession.Connect());
	sMngmntSession.ShutdownServer();
	sMngmntSession.Close();

	TBuf<21> notifierConfig(_L("!:\\upsrefnotifier.txt"));
	notifierConfig[0] = fs.GetSystemDriveChar();

	TInt lineLength = User::CommandLineLength();
	switch(lineLength)
		{
		default:
			// fall through - extra command line arguments are ignored
		case 2:
			// 2 char arg - Delete DB and run interactive
			(void) fs.Delete(_L("c:\\Private\\10283558\\database\\ups.db"));
			// Fall through to also delete notifier config file
		case 1:
			// 1 char arg - Run interactive, without deleting DB 
			(void) fs.Delete(notifierConfig);
			break;
		case 0:
			{
			// No args - delete DB and run in silent mode
			(void) fs.Delete(_L("c:\\Private\\10283558\\database\\ups.db"));

			(void) fs.Delete(notifierConfig);
			RFile file;
			User::LeaveIfError(file.Create(fs, notifierConfig, EFileShareExclusive | EFileWrite));
			User::LeaveIfError(file.Write(_L8("Always")));
			file.Close();
			break;
			}
		}

	User::LeaveIfError(sMngmntSession.Connect());
	CleanupClosePushL(sMngmntSession);


	CTestSwiIf *t = CTestSwiIf::NewL();
	CActiveScheduler::Start();
	delete t;

	PopulateDatabaseL();
	TestSwiObserverL();
	
	sMngmntSession.ShutdownServer();

	// Close top level session (low level session was closed by
	// ShutdownServer, but we still need to do the RUpsManagement
	// cleanup).
	CleanupStack::PopAndDestroy(&sMngmntSession);

	(void) fs.Delete(notifierConfig);
	CleanupStack::PopAndDestroy(&fs);
	
	test.End();
	test.Close();

	CleanupStack::PopAndDestroy(scheduler);
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

