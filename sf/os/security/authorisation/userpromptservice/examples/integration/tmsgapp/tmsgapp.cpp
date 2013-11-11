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
* Example Code
*
*/


#include "tmsgapp.h"
#include <e32debug.h>
#include <f32file.h>

// UPS client library is only included to allow the test application
// to reset its decision records.
#include <ups/upsclient.h>	

_LIT(KTo, "+442071541000");
_LIT_SECURE_ID(KMySecureId, 0x01000003);

// CAlarm ********************************************************************
inline CAlarm::CAlarm() :CTimer(-1) {CActiveScheduler::Add(this);}
inline void CAlarm::ConstructL() {CTimer::ConstructL();}

void CAlarm::RunL() 
	{
	iObserver->WakeupL();
	}

void CAlarm::After(MAlarmObserver* aObserver, TTimeIntervalMicroSeconds32 aInterval)
	{
	iObserver = aObserver;
	CTimer::After(aInterval);
	}

// CSendMessages *************************************************************
CSendMessages::CSendMessages() : CActive(EPriorityStandard) 
	{
	CActiveScheduler::Add(this);
	}

CSendMessages::~CSendMessages() 
	{
	Deque();
	iMsgCon.Close();
	}

CSendMessages* CSendMessages::NewLC()
	{
	CSendMessages* self = new(ELeave) CSendMessages();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSendMessages::ConstructL()
	{
	iAlarm.ConstructL();
	User::LeaveIfError(iMsgCon.Connect());
	TRequestStatus* status = &iStatus;
	*status = KRequestPending;
	SetActive();
	User::RequestComplete(status, KErrNone);
	}

TInt CSendMessages::RunError(TInt /* aError */)
	{
	CActiveScheduler::Stop(); // Non-recoverable error
	return KErrNone;
	}

void CSendMessages::DoCancel()
	{
	iMsgCon.CancelSendMsg();	
	iAlarm.Cancel();
	}

void CSendMessages::WakeupL()
/**
 * Timeout the pending message.
 */
	{
	iMsgCon.CancelSendMsg();
	}

void CSendMessages::RunL()
/**
 * Send a series of test messages and cancellations that should trigger
 * user prompts.
 * All test cases timeout after KDefaultTimeout to enable automated tests
 */
	{
	TInt err = iStatus.Int();
	if (iStatus != KErrNone && iStatus != KErrCancel && iStatus != KErrPermissionDenied) 
		{
		User::Leave(err);
		}
	iAlarm.Cancel();
	
	RDebug::Printf("Test %d completed with error %d", iTestNum, 0);
		
	++iTestNum;	// move on to next test case	
	switch (iTestNum)
		{
		case 1:
			{
			// Send message and wait for user prompt to complete			
			_LIT(KBody, "Hello");
			iMsgCon.SendMsg(KTo, KBody, iStatus);
			SetActive();
			iAlarm.After(this, KDefaultTimeout);
			break;
			}
		case 2:
			{
			// Send a message and cancel straight away, should be no user prompt
			_LIT(KBody, "Hello - Cancel immediate");
			iMsgCon.SendMsg(KTo, KBody, iStatus);
			SetActive();
			iAlarm.After(this, 1);
			break;
			}
		case 3:
			{
			// Send a message and attempt to cancel whilst the user prompt is being displayed.
			_LIT(KBody, "Hello - Cancel 5 secs");
			iMsgCon.SendMsg(KTo, KBody, iStatus);
			SetActive();
			iAlarm.After(this, 5 * 1000 * 1000);
			break;
			}
		case 4:
			{
			// Force the message server to exit
			iMsgCon.Close();
			_LIT(KInfo, "Waiting for messageserver to exit");
			User::InfoPrint(KInfo);
			User::After(5 * 1000 * 1000);	
			User::LeaveIfError(iMsgCon.Connect());	// reconnect

			// Send a message to the new server instance
			_LIT(KBody, "Hello again");
			iMsgCon.SendMsg(KTo, KBody, iStatus);
			SetActive();
			iAlarm.After(this, KDefaultTimeout);
			break;
			}
		default:
			// All done
			CActiveScheduler::Stop();
			break;
		}	
	}

void DeleteOldDecisionsL()
/**
 * Delete all decision records for this test application
 */
	{
	UserPromptService::RUpsManagement m;
	CleanupClosePushL(m);
	User::LeaveIfError(m.Connect());
	UserPromptService::CDecisionFilter* f = UserPromptService::CDecisionFilter::NewLC();
	f->SetClientSid(KMySecureId, UserPromptService::EEqual);
	m.RemoveDecisionsL(*f);
	CleanupStack::PopAndDestroy(2, &m); // m, f
	}

void MainL()
	{
	// Create active scheduler, reset UPS and start tests
	CActiveScheduler* s=new(ELeave) CActiveScheduler;	
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	DeleteOldDecisionsL();	// reset decision database
	CSendMessages* test = CSendMessages::NewLC();
	s->Start();
	CleanupStack::PopAndDestroy(2, s); // s, test
	
	// Add log file for automated test environment
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile log;
	CleanupClosePushL(log);
	User::LeaveIfError(log.Replace(fs, _L("c:\\tmsgapp.log"), EFileShareAny|EFileWrite));
	User::LeaveIfError(log.Write(_L8("\n\n0 tests failed out of 1\n")));
	CleanupStack::PopAndDestroy(2, &fs);
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	return KErrNone;
	}
	
