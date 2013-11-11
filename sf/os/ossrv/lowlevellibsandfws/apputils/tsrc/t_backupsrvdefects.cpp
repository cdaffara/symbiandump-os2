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

#include <e32test.h>
#include "t_backupsrvdefects.h"


RTest TheTest (_L("t_backupsrvdefects"));
#define TEST(arg)  TheTest((arg), __LINE__)

_LIT(KFileName1, "FileName1");


CBUDefectTestMachine* CBUDefectTestMachine::NewL()
	{
	CBUDefectTestMachine* self = new (ELeave) CBUDefectTestMachine();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CBUDefectTestMachine::CBUDefectTestMachine()
	:CActive(0)
	{}

CBUDefectTestMachine::~CBUDefectTestMachine()
	{
	delete iWrapper;

	// Cancel this if it's active
	if (IsActive())
		{
		Cancel();
		}
	}	
	
void CBUDefectTestMachine::ConstructL()
	{
	iWrapper = CBaBackupSessionWrapper::NewL();
		
	iNextState = EMachineStart;

	// Add this to the Active Scheduler and set us active
	CActiveScheduler::Add(this);
	SetActive();	
	}

// Called when CloseAll request is completed or the active object is scheduled.	
void CBUDefectTestMachine::RunL()	
	{
	switch(iNextState)
		{
		case EMachineStart:
			// Send 2 requests of CloseAll with different file lock states.
			// The second request will be ignored. The first one will be completed.		
			iWrapper->CloseAll(MBackupObserver::EReleaseLockReadOnly, iStatus);
			iWrapper->CloseAll(MBackupObserver::EReleaseLockNoAccess, iStatus);

			iNextState = EMachineCloseAllsEndSameClient;
			SetActive();
			break;
		case EMachineCloseAllsEndSameClient:
			// The first CloseAll request is completed.		
			TEST(iStatus == KErrNone);

			// Send 2 requests of CloseAll from different client.
			// The second client should be completed with KErrServerBusy before 
			// the first client is completed with KErrNone
			iWrapper->CloseAll(MBackupObserver::EReleaseLockReadOnly, iStatus);
			iBackupFileObserver->CloseAll(MBackupObserver::EReleaseLockReadOnly);

			iNextState = EMachineCloseAllsEndOtherClient;
			SetActive();
			break;
		case EMachineCloseAllsEndOtherClient:
			// The first CloseAll client is completed successfully
			TEST(iStatus == KErrNone);

			// Tests under CloseAll operation, CloseFileL requested by the other client will leave;
			// CloseFileL requested by the same client will be successful and not affect the CloseAll
			// process.
			iWrapper->CloseAll(MBackupObserver::EReleaseLockReadOnly, iStatus);
			
			TRAPD(err, iBackupFileObserver->CloseFileL(KFileName1,MBackupObserver::EReleaseLockNoAccess));
			TEST(err == KErrServerBusy);
			
			iWrapper->CloseFileL(KFileName1,MBackupObserver::EReleaseLockReadOnly);

			iNextState = EMachineCloseAllsEndCloseFile;
			SetActive();
			
			break;
			
		case EMachineCloseAllsEndCloseFile:
			// The CloseAll is completed successfully
			TEST(iStatus == KErrNone);
			
			CActiveScheduler::Stop();	
			break;
		
		default:
			break;
		}
	}
	
void CBUDefectTestMachine::DoCancel()
	{ 
	Complete();
	}

void CBUDefectTestMachine::Complete()
	{
	TRequestStatus* tempStatus=&iStatus;
	User::RequestComplete(tempStatus, KErrNone);
	}

/*
Sets the other client of the backup server
*/
void CBUDefectTestMachine::SetFileObserver(CBackupFileObserver* aBackupFileObserver)
	{
	iBackupFileObserver = aBackupFileObserver;
	}
	


//CBackupFileObserver
CBackupFileObserver* CBackupFileObserver::NewL(CBUDefectTestMachine& aDefectMachine, const TDesC& aLockedFileName)
	{
	CBackupFileObserver* self = new (ELeave) CBackupFileObserver(aDefectMachine, aLockedFileName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CBackupFileObserver::CBackupFileObserver(CBUDefectTestMachine& aTestMachine, const TDesC& aLockedFileName)
:CActive(0), iTestMachine(aTestMachine), iLockedFileName(aLockedFileName)
	{}
	
void CBackupFileObserver::ConstructL()
	{
	iWrapper = CBaBackupSessionWrapper::NewL();

	iWrapper->RegisterFileL(iLockedFileName, *this);
	iTestMachine.SetFileObserver(this);
	
	// Add this to the Active Scheduler and set us active
	CActiveScheduler::Add(this);
	}
	
CBackupFileObserver::~CBackupFileObserver()
	{
	delete iWrapper;
	
	// Cancel this if it's active
	if (IsActive())
		{
		Cancel();
		}	
	}

void CBackupFileObserver::CloseAll(MBackupObserver::TFileLockFlags aFlags)
	{
	iWrapper->CloseAll(aFlags, iStatus);
	if(! IsActive())
		{
		SetActive();
		}
	}
	
void CBackupFileObserver::CloseFileL(const TDesC& aFileName,MBackupObserver::TFileLockFlags aFlags)
	{
	iWrapper->CloseFileL(aFileName,aFlags);
	}

	
void CBackupFileObserver::ChangeFileLockL(const TDesC& aFileName,TFileLockFlags aFlags)
	{
	TEST(aFileName == KFileName1);
	TEST(aFlags == MBackupObserver::EReleaseLockReadOnly);
	}

/**
Called when CloseAll request is completed.
*/
void CBackupFileObserver::RunL()	
	{
	if(iTestMachine.iNextState == EMachineCloseAllsEndOtherClient)
		{
		// As the second CloseAll client, Tests the CloseAll request is completed with KErrServerBusy 
		// while the first one is still pending
		TEST(iStatus == KErrServerBusy); 
		TEST(iTestMachine.iStatus == KRequestPending);	
		}
	}

void CBackupFileObserver::DoCancel()
	{ 
	TRequestStatus* tempStatus=&iStatus;
	User::RequestComplete(tempStatus, KErrNone);
	}

/**
Tests CBaBackupSessionWrapper::RegisterFileL leaves with KErrServerBusy while the sever is under 
CloseAll operation. And the CloseAll request is complete successfully.
*/	
void TestRegisterFileL()
	{
	class TSimpleObserver : public MBackupObserver
	{
	void ChangeFileLockL(const TDesC& /*aFileName*/,TFileLockFlags /*aFlags*/) {;}
	} simpleObserver;
	
	CBaBackupSessionWrapper* wrapper = CBaBackupSessionWrapper::NewL();
	CleanupStack::PushL(wrapper);
	TRequestStatus status;
	wrapper->CloseAll(MBackupObserver::EReleaseLockReadOnly, status);
	TRAPD(err,wrapper->RegisterFileL(KFileName1, simpleObserver));
	TEST(err == KErrServerBusy);
	User::WaitForRequest(status);
	TEST(status == KErrNone);
	CleanupStack::PopAndDestroy(wrapper);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-4053
@SYMTestCaseDesc        Tests the updated functions work as expectation
@SYMTestPriority        High
@SYMTestActions         Calls the updated functions under the specified circumstance (sever under 
						CloseAll operation). They run as expectation:
                        1. Call CBaBackupSessionWrapper::RegisterFileL while the sever is under 
                           CloseAll operation. It leaves with KErrServerBusy. The CloseAll 
                           request is complete successfully.
                        2. Call CBaBackupSessionWrapper::CloseAll() 2 times from the same client. 
                           The second request will be ignored. The first one will be completed 
                           successfully.	
                        3. Call CBaBackupSessionWrapper::CloseAll() 2 times from the different clients. 
                           The second request will be completed immediately with KErrServerBusy. The 
                           first one will be completed successfully.	
                        4. Call CBaBackupSessionWrapper::CloseFileL() while the sever is under CloseAll 
                           operation requested by the other client. It leaves with KErrServerBusy. The 
                           CloseAll request is complete successfully.
                        5. Call CBaBackupSessionWrapper::CloseFileL() while the sever is 
                           under CloseAll operation requested by the same client. It is ignored and 
                           the CloseAll request is complete successfully.
                    
@SYMTestExpectedResults Test must not fail
@SYMDEF                 PDEF121575
*/	
void PDEF121575L()
	{
	
	TestRegisterFileL();
	CBUDefectTestMachine* bUSrvDefectMachine = CBUDefectTestMachine::NewL();
	CleanupStack::PushL(bUSrvDefectMachine);
	
	CBackupFileObserver* fileObserver = CBackupFileObserver::NewL(*bUSrvDefectMachine, KFileName1);
	
	CleanupStack::Pop(bUSrvDefectMachine);
	
	// start the machine
	bUSrvDefectMachine->Complete();
	
	CActiveScheduler::Start();
	
	delete bUSrvDefectMachine ;
	delete fileObserver ;
	
	return;	
	
	}
	
void StartTestsL()
	{
	TheTest.Start (_L("PDEF121575: BAFL Backup System Multiple Simultaneous Call Problema\n"));
	PDEF121575L();
	}

TInt E32Main()
	{
	TheTest.Title ();
	
	__UHEAP_MARK;


	CTrapCleanup* theTrapCleanup=CTrapCleanup::New();
	CActiveScheduler *activeScheduler=new CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);

	TRAPD(error, StartTestsL());
	TEST(error == KErrNone);
	
	delete activeScheduler;
	delete theTrapCleanup;
	
	__UHEAP_MARKEND; 
	
	TheTest.End();	
	
	return(KErrNone);
	}

