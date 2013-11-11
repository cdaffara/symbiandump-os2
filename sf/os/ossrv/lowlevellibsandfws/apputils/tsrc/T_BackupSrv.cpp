// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32debug.h>
#include <e32test.h>
#include <babackup.h>
#include "T_backupSrv.h"
LOCAL_D RTest MainTest(_L(" T_BackupSrv.cpp"));

_LIT(KFileName1, "FileName1");
_LIT(KFileName2, "FileName2");
_LIT(KFileName3, "FileName3");
_LIT(KFileName4, "FileName4");

// Nasty global convenience function
LOCAL_D void LogThread()
{
	TBuf<150> buf((RThread().Name()));
	RDebug::Print(_L("*** Currently in thread : "));
	RDebug::Print(buf);
}

//
//	class CBackupOperationObserver
//
CBackupOperationObserver* CBackupOperationObserver::NewL(TInt aObserverNumber)
	{
	LogThread();
	RDebug::Print(_L("CBackupOperationObserver::NewL"));

	CBackupOperationObserver* self = new (ELeave) CBackupOperationObserver();
	CleanupStack::PushL(self);
	self->ConstructL(aObserverNumber);
	CleanupStack::Pop();
	return self;
	}

CBackupOperationObserver::CBackupOperationObserver()
 : CActive(0)
	{ ; }

CBackupOperationObserver::~CBackupOperationObserver()
	{
	LogThread();
	RDebug::Print(_L("CBackupOperationObserver::~CBackupOperationObserver"));

	iBackupSession->DeRegisterBackupOperationObserver(*this);
	delete iBackupSession;

	delete iLocalRTest;
	}

void CBackupOperationObserver::ConstructL(TInt aObserverNumber)
	{
	// Set up the AO callback
	CActiveScheduler::Add(this);
	SetActive();
	iStatus = KRequestPending;

	// Create a new session for this backup notification observer
	iBackupSession = CBaBackupSessionWrapper::NewL();
	iBackupSession->RegisterBackupOperationObserverL(*this);
	iObserverNumber = aObserverNumber;
	iLocalRTest = new (ELeave) RTest(_L("BackupOperationObserver"));
	}

void CBackupOperationObserver::HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes)
	{
	LogThread();
	RDebug::Print(_L("CBackupOperationObserver::HandleBackupOperationEventL"));

	TBuf<150> buf;
	buf.Format(_L("Backup observer number %d received a notification of operation type %d and file lock type: %d\n"),iObserverNumber, aBackupOperationAttributes.iOperation, aBackupOperationAttributes.iFileFlag);
	RDebug::Print(buf);
	}

void CBackupOperationObserver::DoCancel()
	{ ; }

void CBackupOperationObserver::RunL()
	{
	// Okay - we're back in the right thread!
	LogThread();
	RDebug::Print(_L("CBackupOperationObserver::RunL"));

	// Finished with this object
	delete this;

	// And the active scheduler in this thread
	CActiveScheduler::Current()->Stop();

	}

void CBackupOperationObserver::Kill(RThread* aThread)
	{
	LogThread();
	RDebug::Print(_L("CBackupFileObserver::Kill"));

	// Trigger the Active Object locally - in a different thread!
	TRequestStatus* tempStatus=(&iStatus);
	aThread->RequestComplete(tempStatus, KErrNone);
	}

//
//	class CBackupFileObserver
//

CBackupFileObserver::CBackupFileObserver()
 : CActive(0)
  { ;  }

void CBackupFileObserver::ConstructL(TInt aFileObserverNumber)
	{
	// Set up the AO callback
	CActiveScheduler::Add(this);
	SetActive();
	iStatus = KRequestPending;

	// Create a new session for this file lock observer
	iBackupSession = CBaBackupSessionWrapper::NewL();
	iFileObserverNumber = aFileObserverNumber;
	iFileLocksChanged = 0;
	iFileLockState = ELocked;
	iLocalRTest = new (ELeave) RTest(_L("BackupFileObserver"));
	}

CBackupFileObserver* CBackupFileObserver::NewL(TInt aFileObserverNumber)
	{
	LogThread();
	RDebug::Print(_L("CBackupFileObserver::NewL"));

	CBackupFileObserver* self=new(ELeave) CBackupFileObserver();
	CleanupStack::PushL(self);
	self->ConstructL(aFileObserverNumber);
	CleanupStack::Pop();
	return self;
	}

CBackupFileObserver::~CBackupFileObserver()
	{
	LogThread();
	RDebug::Print(_L("CBackupFileObserver::~CBackupFileObserver"));

	delete iBackupSession;

	delete iLocalRTest;
	}

void CBackupFileObserver::AddFileL(TInt aFileNumber)
	{
	LogThread();
	RDebug::Print(_L("CBackupFileObserver::AddFileL"));

	// We base the filename of the number of thread in the RArray
	iFileName.Format(_L("FileName%d"), aFileNumber);

	iBackupSession->RegisterFileL(iFileName,*this);
	}

TInt CBackupFileObserver::GetFileLocksChanged()
	{
	return iFileLocksChanged;
	}

void CBackupFileObserver::ZeroFileLocksChanged()
	{
	iFileLocksChanged = 0;
	}

void CBackupFileObserver::SetDelay(TBool aDelay)
{
	iDelay = aDelay;
}

void CBackupFileObserver::ChangeFileLockL(const TDesC& aFileName,TFileLockFlags aFlags)
	{
	LogThread();
	RDebug::Print(_L("BackupFileObserver::ChangeFileLockL"));

	// Keep a count of how many release notifications there have been
	if (aFlags != MBackupObserver::ETakeLock)
		{
		iFileLocksChanged++;
		}

	// If delay is set then insert wait now
	if (iDelay && (!(aFileName.Compare(KFileName1))))
		{
		User::After(10000000);
		}

	// Check this file is the one for this observer - if not fail test;
	if (iFileName.Compare(aFileName))
		{
		RDebug::Print(_L("\nReceived notification for non-registered file!"));
		(*iLocalRTest)(EFalse);
		}

	// Update the local file lock array
	CBackupFileObserver::TFileLock flag = (CBackupFileObserver::TFileLock) aFlags;
	iFileLockState = flag;

	// Test output
	TBuf<150> buf;
	buf.Format(_L("File backup observer number %d was notified for file %S of file lock type: %d\n"),iFileObserverNumber, &aFileName, flag);
	RDebug::Print(buf);
	}

void CBackupFileObserver::DoCancel()
	{ ; }

void CBackupFileObserver::RunL()
	{
	// Okay - we're back in the right thread!
	LogThread();
	RDebug::Print(_L("CBackupFileObserver::RunL"));

	// Finished with this object
	delete this;

	// And the active scheduler in this thread
	CActiveScheduler::Current()->Stop();

	}

void CBackupFileObserver::Kill(RThread* aThread)
	{
	LogThread();
	RDebug::Print(_L("CBackupFileObserver::Kill"));

	// Trigger the Active Object locally - in a different thread!
	TRequestStatus* tempStatus=(&iStatus);
	aThread->RequestComplete(tempStatus, KErrNone);
	}

//
// class CBackupTestsStateMachine
//

CBackupTestsStateMachine* CBackupTestsStateMachine::NewL()
	{
	CBackupTestsStateMachine* self = new (ELeave) CBackupTestsStateMachine();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CBackupTestsStateMachine::CBackupTestsStateMachine()
 : CActive(0)
{ ; }

void CBackupTestsStateMachine::ConstructL()
	{
	// Set up the RTest for this thread and display we're started
	iLocalRTest = new (ELeave) RTest(_L("T_BACKUP_SRV"));
	iLocalRTest->Title();

	// Set the ititial state
	iState = EStateMachineStart;

	// Zero the EndBackupRecursionCount (see below)
	iEndBackupRecursionCount = 0;

	// Add this to the Active Scheduler and set us active
	CActiveScheduler::Add(this);
	SetActive();

	// Create the backup "command" session
	iBackupSession = CBaBackupSessionWrapper::NewL();

	// Set up the mutex
	iMutex.CreateLocal();
	}

// Observers need to have their own thread
void CBackupTestsStateMachine::CreateObserversThreadsL()
	{
	RDebug::Print(_L("Starting observers threads"));

	TInt i;
	TBuf<30> newThreadNames;
	TInt error;
	for (i = 0; i<4 ; i++)
		{
		// Backup observer
		iBackupObserverThreads.AppendL(new (ELeave) RThread);
		newThreadNames.Format(_L("Backup Observer Thread %d"), i+1);
		error = iBackupObserverThreads[i]->Create(newThreadNames, CBackupTestsStateMachine::BackupObserversThreadStartL, 0x2000, NULL, (TAny*)this);
		(*iLocalRTest)(error==KErrNone);
		iBackupObserverThreads[i]->Resume();

		// File observer
		iFileObserverThreads.AppendL(new (ELeave) RThread);
		newThreadNames.Format(_L("File Observer Thread %d"), i+1);
		error = iFileObserverThreads[i]->Create(newThreadNames, CBackupTestsStateMachine::FileObserversThreadStartL, 0x2000, NULL, (TAny*)this);

		(*iLocalRTest)(error==KErrNone);
		iFileObserverThreads[i]->Resume();

		// Brief delay to let the observer threads get started
		User::After(1000000);
		}

	}

// Static starting function for the backup observers threads
TInt CBackupTestsStateMachine::BackupObserversThreadStartL(TAny* aPtr)
	{
	// Create the Cleanup Stack and Active Scheduler for this thread

	CTrapCleanup* theTrapCleanup = CTrapCleanup::New();
	CActiveScheduler *activeScheduler = new CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);

	// Create the observer instances
	CBackupTestsStateMachine* objectPtr = static_cast<CBackupTestsStateMachine*>(aPtr);
	TRAPD(error, objectPtr->CreateBackupObserverInstanceL());
	User::LeaveIfError(error);

	// Go to Active Scheduler main loop for this thread
	CActiveScheduler::Start();

	// And we're done
	delete activeScheduler;
	delete theTrapCleanup;

	return KErrNone;
	}

// Static starting function for the file observers threads
TInt CBackupTestsStateMachine::FileObserversThreadStartL(TAny* aPtr)
{
	// Create the Cleanup Stack and Active Scheduler for this thread
	CTrapCleanup* theTrapCleanup = CTrapCleanup::New();
	CActiveScheduler* activeScheduler = new CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);

	// Create the observer instances
	CBackupTestsStateMachine* objectPtr = static_cast<CBackupTestsStateMachine*>(aPtr);
	TRAPD(error, objectPtr->CreateFileObserverInstanceL());
	User::LeaveIfError(error);

	// Go to Active Scheduler main loop for this thread
	CActiveScheduler::Start();

	// And we're done
	delete activeScheduler;
	delete theTrapCleanup;

	return KErrNone;
}

void CBackupTestsStateMachine::CreateBackupObserverInstanceL()
	{
	iMutex.Wait();

	TInt count = iBackupObserverThreads.Count();

	// Create the new object instance (one object per thread)
	// We base the thread number of the number of thread in the RArray
	CBackupOperationObserver* newObserver = CBackupOperationObserver::NewL(count);
	iBackupObservers.AppendL(newObserver);

	iMutex.Signal();
	}

void CBackupTestsStateMachine::CreateFileObserverInstanceL()
{
	iMutex.Wait();

	TInt count = iBackupFileObservers.Count();

	// Create the new object instance (one object per thread)
	CBackupFileObserver* newObserver = CBackupFileObserver::NewL(count + 1);
	iBackupFileObservers.AppendL(newObserver);

	// Register the file for this thread / instance
	iBackupFileObservers[count]->AddFileL(count + 1);

	iMutex.Signal();
}

// State Machine destructor
CBackupTestsStateMachine::~CBackupTestsStateMachine()
	{
	// Close our session into the backup server
	delete iBackupSession;

	// Delete all the observers (only 3 of each of by this point)
	TInt i;

	for (i = 0; i<3 ; i++)
		{
		iBackupObservers[i]->Kill(iBackupObserverThreads[i]);
		iBackupFileObservers[i]->Kill(iFileObserverThreads[i]);
		User::After(50000000);
		}
	iBackupObservers.Close();
	iBackupFileObservers.Close();

	// Kill the observer threads
	for (i = 0; i<3 ; i++)

	{
	iBackupObserverThreads[i]->Kill(KErrNone);
	delete iBackupObserverThreads[i];
	iFileObserverThreads[i]->Kill(KErrNone);
	delete iFileObserverThreads[i];
	}
	iBackupObserverThreads.Close();
	iFileObserverThreads.Close();

	// Display we're finished
	iLocalRTest->Close();
	delete iLocalRTest;

	// Cancel this is it's active
	if (IsActive())
		{
		Cancel();
		}
	}

// Common starting function for all test-related calls to CloseAll
void CBackupTestsStateMachine::CloseAllStartL(StateMachineState aNextState, MBackupObserver::TFileLockFlags aFlag)
	{
	StartBackupL();
	iState = aNextState;
	iBackupSession->CloseAll(aFlag, iStatus);
	}

// Common ending function for all test-related calls to CloseAll
void CBackupTestsStateMachine::CloseAllEndL(StateMachineState aNextState, TInt aExpectedNotifications)
	{
	iState = aNextState;
	SignalEndBackupL();
	EndBackup(aExpectedNotifications, ETrue);
	iLocalRTest->End();
	Complete();
	}

// State machine call back - get here by calls to Complete and returns for asynchronous server calls
void CBackupTestsStateMachine::RunL()
	{
	switch(iState)
		{
		case EStateMachineStart:
			// Create observers threads
			CreateObserversThreadsL();
			iState = ECloseAllNormalReadOnly;
			Complete();
			break;
		case ECloseAllNormalReadOnly:
			iLocalRTest->Start(_L("\nCloseAllFiles normal ReadOnly\n"));
			CloseAllStartL(ECloseAllNormalReadOnlyReturned, MBackupObserver::EReleaseLockReadOnly);
			break;
		case ECloseAllNormalReadOnlyReturned:
			CloseAllEndL(ECloseAllNormalNoAccess, 4);
			break;
		case ECloseAllNormalNoAccess:
			iLocalRTest->Start(_L("\nCloseAllFiles normal NoAccess\n"));
			CloseAllStartL(ECloseAllNormalNoAccessReturned, MBackupObserver::EReleaseLockNoAccess);
			break;
		case ECloseAllNormalNoAccessReturned:
			CloseAllEndL(ECloseAllDelayReadOnly, 4);
			break;
		case ECloseAllDelayReadOnly:
			iLocalRTest->Start(_L("\nCloseAllFiles delay ReadOnly\n"));
			iBackupFileObservers[0]->SetDelay(ETrue);
			CloseAllStartL(ECloseAllDelayReadOnlyReturned, MBackupObserver::EReleaseLockReadOnly);
			break;
		case ECloseAllDelayReadOnlyReturned:
			iBackupFileObservers[0]->SetDelay(EFalse);
			CloseAllEndL(ECloseAllDelayNoAccess, 4);
			// Nice long wait for the timer to expire in the other thread
			User::After(10000000);
			break;
		case ECloseAllDelayNoAccess:
			iLocalRTest->Start(_L("\nCloseAllFiles delay NoAccess\n"));
			iBackupFileObservers[0]->SetDelay(ETrue);
			CloseAllStartL(ECloseAllDelayNoAccessReturned, MBackupObserver::EReleaseLockNoAccess);
			break;
		case ECloseAllDelayNoAccessReturned:
			iBackupFileObservers[0]->SetDelay(EFalse);
			CloseAllEndL(ECloseAllDropFileSession, 4);
			// Nice long wait for the timer to expire in the other thread
			User::After(10000000);
			break;
		case ECloseAllDropFileSession:
			iLocalRTest->Start(_L("\nCloseAllFiles dropping file session\n"));
			// Drop one of the file registration / observer sessions
			iBackupFileObservers[0]->Kill(iFileObserverThreads[0]);
			// Nice long wait for the observer to be killed in the other thread
			User::After(10000000);
			// Remove it from the list
			iBackupFileObservers.Remove(0);
			// Kill the thread
			iFileObserverThreads[0]->Kill(KErrNone);
			delete iFileObserverThreads[0];
			iFileObserverThreads.Remove(0);
			// All done - start this sub-test
			CloseAllStartL(ECloseAllDropFileSessionReturned, MBackupObserver::EReleaseLockReadOnly);
			break;
		case ECloseAllDropFileSessionReturned:
			CloseAllEndL(ECloseAllDropBackupObserverSession, 3);
			break;
		case ECloseAllDropBackupObserverSession:
			iLocalRTest->Start(_L("\nCloseAllFiles dropping backup session\n"));
			// Drop one of the backup observer sessions
			iBackupObservers[0]->Kill(iBackupObserverThreads[0]);
			// Nice long wait for the observer to be killed in the other thread
			User::After(10000000);
			// Remove it from the list
			iBackupObservers.Remove(0);
			// Kill the thread
			iBackupObserverThreads[0]->Kill(KErrNone);
			delete iBackupObserverThreads[0];
			iBackupObserverThreads.Remove(0);
			// All done - start this sub-test
			CloseAllStartL(ECloseAllDropBackupObserverSessionReturned, MBackupObserver::EReleaseLockReadOnly);
			break;
		case ECloseAllDropBackupObserverSessionReturned:
			CloseAllEndL(ESingleFileTests, 3);
			break;
		case ESingleFileTests:
			iLocalRTest->Start(_L("\nSingle file lock tests\n"));
			StartBackupL();
			SingleFileLockTestsL();
			SignalEndBackupL();
			// Can call EndBackup synchronously here as nothing in SingleFileLockTests is asynchronous
			EndBackup(3, EFalse);
			iLocalRTest->End();
 			iState = ENoBackupSessionSingleFileTests;
			Complete();
			break;
		// Required to cover the situation when the backup server is used purely for message signalling
		// For example in the LogEng compnent
		case ENoBackupSessionSingleFileTests:
			iLocalRTest->Start(_L("\nNo backup session single file lock tests\n"));
			SingleFileLockTestsL();
			// Can call EndBackup synchronously here as nothing in SingleFileLockTests is asynchronous
			EndBackup(3, EFalse);
			iLocalRTest->End();
			iState = EStateMachineEnd;
			Complete();
			break;
		case EStateMachineEnd:
			RDebug::Print(_L("\nEnd of state machine\n"));
			End();
			break;
		default:
			RDebug::Print(_L("\nCBackupTestsStateMachine::RunL problem"));
			break;
		}
	if (!IsActive())
		{
		SetActive();
		}
	}

void CBackupTestsStateMachine::DoCancel()
	{
	Complete();
	}

void CBackupTestsStateMachine::RunError()
	{ ; }

// Common function to start a backup
void CBackupTestsStateMachine::StartBackupL()
	{
	TBackupOperationAttributes attribs;
	attribs.iFileFlag=MBackupObserver::EReleaseLockNoAccess;
	attribs.iOperation=MBackupOperationObserver::EStart;
	iBackupSession->NotifyBackupOperationL(attribs);
	RDebug::Print(_L("\nStarting backup\n"));
	}

void CBackupTestsStateMachine::SignalEndBackupL()
	{
	// Tell the server the backup is over
	TBackupOperationAttributes attribs;
	attribs.iFileFlag=MBackupObserver::EReleaseLockNoAccess;
	attribs.iOperation=MBackupOperationObserver::EEnd;
	iBackupSession->NotifyBackupOperationL(attribs);
	}

// This function works in two ways. If oneTimeOnly is ETrue then all file lock
// change notifications must have been received before this function is called.
// If oneTimeOnly is EFalse this function recursively calls itself (a finate number
// of times) until all notifications have arrived (this happens with single file testing)
void CBackupTestsStateMachine::EndBackup(TInt aFileLockChangesExpected, TBool oneTimeOnly)
	{
	// Get the total notification count so far
	TInt numberOfObservers = iBackupFileObservers.Count();
	TInt totalNotificationCount = 0;
	for (TInt i=0; i<numberOfObservers ; i++)
		{
		totalNotificationCount += iBackupFileObservers[i]->GetFileLocksChanged();
		}
	if (aFileLockChangesExpected == totalNotificationCount)
		{
		// Reset the recursion count
		iEndBackupRecursionCount = 0;

		// Zero the notification counts in the file lock observers
		for (TInt i = 0 ; i<numberOfObservers ; i++)
			{
			iBackupFileObservers[i]->ZeroFileLocksChanged();
			}

		// Debug output
		TBuf<100> buf;
		buf.Format(_L("\nBackup finished sucsessfully on recusion count %d of EndBackup\n"), iEndBackupRecursionCount);
		RDebug::Print(buf);
		}
	else if (oneTimeOnly)
		{
		// No second chances - fail here
		(*iLocalRTest)(EFalse);
		}
	else
		{
		// Give it 5 more seconds (in 10 .5 second iterations) for the notifications to arrive
		User::After(500000);
		// 5 seconds is more than enough (timeouts in server should have gone off by now anyway)
		iEndBackupRecursionCount++;
		if (iEndBackupRecursionCount > 10)
			{
				(*iLocalRTest)(EFalse);
			}
		// Recursively calling isn't great but it needs to leave the function so the AO can run
		EndBackup(aFileLockChangesExpected, EFalse);
		}

	}

void CBackupTestsStateMachine::Complete()
	{
	// Trigger the Active Object locally
	TRequestStatus* tempStatus=(&iStatus);
	User::RequestComplete(tempStatus, KErrNone);
	}

void CBackupTestsStateMachine::Start()
	{
	// Time to start testing
	RDebug::Print(_L("\nCBackupTestsStateMachine::Start"));
	Complete();
	}

void CBackupTestsStateMachine::End()
	{
	// We're done testing - kill the Active Scheduler
	RDebug::Print(_L("\nAll test complete\n"));
	Cancel();
	CActiveScheduler::Current()->Stop();
	}

void CBackupTestsStateMachine::SingleFileLockTestsL()
	{
	TFileName file;

	// File 1
	file.Copy(KFileName1);
	iLocalRTest->Next(file);

	iBackupSession->CloseFileL(KFileName1, MBackupObserver::EReleaseLockReadOnly);
	iBackupSession->RestartFile(file);

//	iBackupSession->CloseFileL(KFileName1, MBackupObserver::EReleaseLockNoAccess);
//	iBackupSession->RestartFile(file);

	// File 2
	file.Copy(KFileName2);
	iLocalRTest->Next(file);

	iBackupSession->CloseFileL(KFileName2, MBackupObserver::EReleaseLockReadOnly);
	iBackupSession->RestartFile(file);

//	iBackupSession->CloseFileL(KFileName2, MBackupObserver::EReleaseLockNoAccess);
//	iBackupSession->RestartFile(file);

	// File 3
	file.Copy(KFileName3);
	iLocalRTest->Next(file);

	iBackupSession->CloseFileL(KFileName3, MBackupObserver::EReleaseLockReadOnly);
	iBackupSession->RestartFile(file);

//	iBackupSession->CloseFileL(KFileName3, MBackupObserver::EReleaseLockNoAccess);
//	iBackupSession->RestartFile(file);

	// File 3
	file.Copy(KFileName4);
	iLocalRTest->Next(file);

	iBackupSession->CloseFileL(KFileName4, MBackupObserver::EReleaseLockReadOnly);
	iBackupSession->RestartFile(file);

//	iBackupSession->CloseFileL(KFileName4, MBackupObserver::EReleaseLockNoAccess);
//	iBackupSession->RestartFile(file);
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0467
@SYMTestCaseDesc        Tests the functionality of CBaBackupSessionWrapper class
@SYMTestPriority        High
@SYMTestActions         Tests for the enabling backup of files
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
LOCAL_D void StartTestsL()
	{
	// For the sake of logging let's start off by renaming the main "command" thread
	RThread().RenameMe(_L("Main thread"));

	// Create state machine
	CBackupTestsStateMachine* stateMachine = CBackupTestsStateMachine::NewL();

	// Kick it off
	stateMachine->Start();

	// Start the Active Scheduler
	CActiveScheduler::Start();

	// Clean up the state machine
	delete stateMachine;
	}

TInt E32Main()
	{
	MainTest.Title();
	MainTest.Start(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0467 Loading Device"));

	__UHEAP_MARK;

	CTrapCleanup* theTrapCleanup=CTrapCleanup::New();
	CActiveScheduler *activeScheduler=new CActiveScheduler;
	CActiveScheduler::Install(activeScheduler);

	TRAPD(error, StartTestsL());
	User::LeaveIfError(error);

	delete activeScheduler;
	delete theTrapCleanup;

	__UHEAP_MARKEND;

	MainTest.End();


	return(KErrNone);
	}
